/*******************************************************************************
*   Ledger Nano S - Secure firmware
*   (c) 2016, 2017, 2018, 2019 Ledger
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
********************************************************************************/

#include "os.h"
#include <string.h>

#include "os_io_seproxyhal.h"

// apdu buffer must hold a complete apdu to avoid troubles
unsigned char G_io_apdu_buffer[IO_APDU_BUFFER_SIZE];


void os_boot(void) {
  // TODO patch entry point when romming (f)
  // set the default try context to nothing
  try_context_set(NULL);
}

#ifdef HAVE_USB_APDU

/**
 *  Ledger Protocol 
 *  HID Report Content
 *  [______________________________]
 *   CCCC TT VVVV.........VV FILL..
 *
 *  All fields are big endian encoded.
 *  CCCC: 2 bytes channel identifier (when multi application are processing).
 *  TT: 1 byte content tag
 *  VVVV..VV: variable length content
 *  FILL..: 00's to fillup the HID report length
 *
 *  LL is at most the length of the HID Report.
 *
 *  Command/Response APDU are split in chunks.
 * 
 *  Filler only allowed at the end of the last hid report of a apdu chain in each direction.
 * 
 *  APDU are using either standard or extended header. up to the application to check the total received length and the lc field
 *
 *  Tags:
 *  Direction:Host>Token T:0x00 V:no  Get protocol version big endian encoded. Replied with a protocol-version. Channel identifier is ignored for this command.
 *  Direction:Token>Host T:0x00 V:yes protocol-version-4-bytes-big-endian. Channel identifier is ignored for this reply.
 *  Direction:Host>Token T:0x01 V:no  Allocate channel. Replied with a channel identifier. Channel identifier is ignored for this command.
 *  Direction:Token>Host T:0x01 V:yes channel-identifier-2-bytes. Channel identifier is ignored for this reply.
 *  Direction:*          T:0x02 V:no  Ping. replied with a ping. Channel identifier is ignored for this command.
 *  NOTSUPPORTED Direction:*          T:0x03 V:no  Abort. replied with an abort if accepted, else not replied.
 *  Direction:*          T:0x05 V=<sequence-idx-U16><seq==0?totallength:NONE><apducontent> APDU (command/response) packet.
 */

volatile unsigned int   G_io_usb_hid_total_length;
volatile unsigned int   G_io_usb_hid_channel;
volatile unsigned int   G_io_usb_hid_remaining_length;
volatile unsigned int   G_io_usb_hid_sequence_number;
volatile unsigned char* G_io_usb_hid_current_buffer;

io_usb_hid_receive_status_t io_usb_hid_receive (io_send_t sndfct, unsigned char* buffer, unsigned short l) {
  // avoid over/under flows
  if (buffer != G_io_usb_ep_buffer) {
    os_memset(G_io_usb_ep_buffer, 0, sizeof(G_io_usb_ep_buffer));
    os_memmove(G_io_usb_ep_buffer, buffer, MIN(l, sizeof(G_io_usb_ep_buffer)));
  }

  // process the chunk content
  switch(G_io_usb_ep_buffer[2]) {
  case 0x05:
    // ensure sequence idx is 0 for the first chunk ! 
    if ((unsigned int)U2BE(G_io_usb_ep_buffer, 3) != (unsigned int)G_io_usb_hid_sequence_number) {
      // ignore packet
      goto apdu_reset;
    }
    // cid, tag, seq
    l -= 2+1+2;
    
    // append the received chunk to the current command apdu
    if (G_io_usb_hid_sequence_number == 0) {
      /// This is the apdu first chunk
      // total apdu size to receive
      G_io_usb_hid_total_length = U2BE(G_io_usb_ep_buffer, 5); //(G_io_usb_ep_buffer[5]<<8)+(G_io_usb_ep_buffer[6]&0xFF);
      // check for invalid length encoding (more data in chunk that announced in the total apdu)
      if (G_io_usb_hid_total_length > sizeof(G_io_apdu_buffer)) {
        goto apdu_reset;
      }
      // seq and total length
      l -= 2;
      // compute remaining size to receive
      G_io_usb_hid_remaining_length = G_io_usb_hid_total_length;
      G_io_usb_hid_current_buffer = G_io_apdu_buffer;

      // retain the channel id to use for the reply
      G_io_usb_hid_channel = U2BE(G_io_usb_ep_buffer, 0);

      if (l > G_io_usb_hid_remaining_length) {
        l = G_io_usb_hid_remaining_length;
      }
      // copy data
      os_memmove((void*)G_io_usb_hid_current_buffer, G_io_usb_ep_buffer+7, l);
    }
    else {
      // check for invalid length encoding (more data in chunk that announced in the total apdu)
      if (l > G_io_usb_hid_remaining_length) {
        l = G_io_usb_hid_remaining_length;
      }

      /// This is a following chunk
      // append content
      os_memmove((void*)G_io_usb_hid_current_buffer, G_io_usb_ep_buffer+5, l);
    }
    // factorize (f)
    G_io_usb_hid_current_buffer += l;
    G_io_usb_hid_remaining_length -= l;
    G_io_usb_hid_sequence_number++;
    break;

  case 0x00: // get version ID
    // do not reset the current apdu reception if any
    os_memset(G_io_usb_ep_buffer+3, 0, 4); // PROTOCOL VERSION is 0
    // send the response
    sndfct(G_io_usb_ep_buffer, IO_HID_EP_LENGTH);
    // await for the next chunk
    goto apdu_reset;

  case 0x01: // ALLOCATE CHANNEL
    // do not reset the current apdu reception if any
    cx_rng(G_io_usb_ep_buffer+3, 4);
    // send the response
    sndfct(G_io_usb_ep_buffer, IO_HID_EP_LENGTH);
    // await for the next chunk
    goto apdu_reset;

  case 0x02: // ECHO|PING
    // do not reset the current apdu reception if any
    // send the response
    sndfct(G_io_usb_ep_buffer, IO_HID_EP_LENGTH);
    // await for the next chunk
    goto apdu_reset;
  }

  // if more data to be received, notify it
  if (G_io_usb_hid_remaining_length) {
    return IO_USB_APDU_MORE_DATA;
  }

  // reset sequence number for next exchange
  io_usb_hid_init();
  return IO_USB_APDU_RECEIVED;

apdu_reset:
  io_usb_hid_init();
  return IO_USB_APDU_RESET;
}

void io_usb_hid_init(void) {
  G_io_usb_hid_sequence_number = 0; 
  //G_io_usb_hid_remaining_length = 0; // not really needed
  //G_io_usb_hid_total_length = 0; // not really needed
  //G_io_usb_hid_current_buffer = G_io_apdu_buffer; // not really needed
}

/**
 * sent the next io_usb_hid transport chunk (rx on the host, tx on the device)
 */
void io_usb_hid_sent(io_send_t sndfct) {
  unsigned int l;

  // only prepare next chunk if some data to be sent remain
  if (G_io_usb_hid_remaining_length) {
    // fill the chunk
    os_memset(G_io_usb_ep_buffer, 0, IO_HID_EP_LENGTH-2);

    // keep the channel identifier
    G_io_usb_ep_buffer[0] = (G_io_usb_hid_channel>>8)&0xFF;
    G_io_usb_ep_buffer[1] = G_io_usb_hid_channel&0xFF;
    G_io_usb_ep_buffer[2] = 0x05;
    G_io_usb_ep_buffer[3] = G_io_usb_hid_sequence_number>>8;
    G_io_usb_ep_buffer[4] = G_io_usb_hid_sequence_number;

    if (G_io_usb_hid_sequence_number == 0) {
      l = ((G_io_usb_hid_remaining_length>IO_HID_EP_LENGTH-7) ? IO_HID_EP_LENGTH-7 : G_io_usb_hid_remaining_length);
      G_io_usb_ep_buffer[5] = G_io_usb_hid_remaining_length>>8;
      G_io_usb_ep_buffer[6] = G_io_usb_hid_remaining_length;
      os_memmove(G_io_usb_ep_buffer+7, (const void*)G_io_usb_hid_current_buffer, l);
      G_io_usb_hid_current_buffer += l;
      G_io_usb_hid_remaining_length -= l;
      l += 7;
    }
    else {
      l = ((G_io_usb_hid_remaining_length>IO_HID_EP_LENGTH-5) ? IO_HID_EP_LENGTH-5 : G_io_usb_hid_remaining_length);
      os_memmove(G_io_usb_ep_buffer+5, (const void*)G_io_usb_hid_current_buffer, l);
      G_io_usb_hid_current_buffer += l;
      G_io_usb_hid_remaining_length -= l;
      l += 5;
    }
    // prepare next chunk numbering
    G_io_usb_hid_sequence_number++;
    // send the chunk
    // always pad :)
    sndfct(G_io_usb_ep_buffer, sizeof(G_io_usb_ep_buffer));
  }
  // cleanup when everything has been sent (ack for the last sent usb in packet)
  else {
    G_io_usb_hid_sequence_number = 0; 
    G_io_usb_hid_current_buffer = NULL;

    // we sent the whole response
    G_io_apdu_state = APDU_IDLE;
  }
}

void io_usb_hid_send(io_send_t sndfct, unsigned short sndlength) {
  // perform send
  if (sndlength) {
    G_io_usb_hid_sequence_number = 0; 
    G_io_usb_hid_current_buffer = G_io_apdu_buffer;
    G_io_usb_hid_remaining_length = sndlength;
    G_io_usb_hid_total_length = sndlength;
    io_usb_hid_sent(sndfct);
  }
}

#endif // HAVE_USB_APDU

REENTRANT(void os_memmove(void * dst, const void WIDE * src, unsigned int length)) {
#define DSTCHAR ((unsigned char *)dst)
#define SRCCHAR ((unsigned char WIDE *)src)
  if (dst > src) {
    while(length--) {
      DSTCHAR[length] = SRCCHAR[length];
    }
  }
  else {
    unsigned short l = 0;
    while (length--) {
      DSTCHAR[l] = SRCCHAR[l];
      l++;
    }
  }
#undef DSTCHAR
}

void os_memset(void * dst, unsigned char c, unsigned int length) {
#define DSTCHAR ((unsigned char *)dst)
  while(length--) {
    DSTCHAR[length] = c;
  }
#undef DSTCHAR
}

char os_memcmp(const void WIDE * buf1, const void WIDE * buf2, unsigned int length) {
#define BUF1 ((unsigned char const WIDE *)buf1)
#define BUF2 ((unsigned char const WIDE *)buf2)
  while(length--) {
    if (BUF1[length] != BUF2[length]) {
      return (BUF1[length] > BUF2[length])? 1:-1;
    }
  }
  return 0;
#undef BUF1
#undef BUF2

}

void os_xor(void * dst, void WIDE* src1, void WIDE* src2, unsigned int length) {
#define SRC1 ((unsigned char const WIDE *)src1)
#define SRC2 ((unsigned char const WIDE *)src2)
#define DST ((unsigned char *)dst)
  unsigned short l = length;
  // don't || to ensure all condition are evaluated
  while(!(!length && !l)) {
    length--;
    DST[length] = SRC1[length] ^ SRC2[length];
    l--;
  }
  // WHAT ??? glitch detected ?
  if (l!=length) {
    THROW(EXCEPTION);
  }
}

char os_secure_memcmp(void WIDE* src1, void WIDE* src2, unsigned int length) {
#define SRC1 ((unsigned char const WIDE *)src1)
#define SRC2 ((unsigned char const WIDE *)src2)
  unsigned short l = length;
  unsigned char xoracc=0;
  // don't || to ensure all condition are evaluated
  while(!(!length && !l)) {
    length--;
    xoracc |= SRC1[length] ^ SRC2[length];
    l--;
  }
  // WHAT ??? glitch detected ?
  if (l!=length) {
    THROW(EXCEPTION);
  }
  return xoracc;
}

try_context_t* try_context_get(void) {
  try_context_t* current_ctx;
  __asm volatile ("mov %0, r9":"=r"(current_ctx));
  return current_ctx;
}

try_context_t* try_context_get_previous(void) {
  try_context_t* current_ctx;
  __asm volatile ("mov %0, r9":"=r"(current_ctx));

  // first context reached ?
  if (current_ctx == NULL) {
    // DESIGN NOTE: if not done, then upon END_TRY a wrong context address may be use (if address 
    // à is readable in the arch, and therefore lead to faulty rethrow or worse)
    return NULL;
  }

  // return r9 content saved on the current context. It links to the previous context.
  // r4 r5 r6 r7 r8 r9 r10 r11 sp lr
  //                ^ platform register
  return (try_context_t*) current_ctx->jmp_buf[5];
}

void try_context_set(try_context_t* ctx) {
  __asm volatile ("mov r9, %0"::"r"(ctx));
}

#ifndef HAVE_BOLOS
void os_longjmp(unsigned int exception) {
  longjmp(try_context_get()->jmp_buf, exception);
}
#endif // HAVE_BOLOS

// BER encoded
// <tag> <length> <value>
// tag: 1 byte only
// length: 1 byte if little than 0x80, else 1 byte of length encoding (0x8Y, with Y the number of following bytes the length is encoded on) and then Y bytes of BE encoded total length
// value: no encoding, raw data
unsigned int os_parse_bertlv(unsigned char* mem, unsigned int mem_len, 
                             unsigned int * tlvoffset, unsigned int tag, unsigned int offset, void** buffer, unsigned int maxlength) {
  unsigned int ret, tlvoffset_in;
  unsigned int check_equals_buffer = offset & OS_PARSE_BERTLV_OFFSET_COMPARE_WITH_BUFFER;
  unsigned int get_address = offset & OS_PARSE_BERTLV_OFFSET_GET_LENGTH;
  offset &= ~(OS_PARSE_BERTLV_OFFSET_COMPARE_WITH_BUFFER|OS_PARSE_BERTLV_OFFSET_GET_LENGTH);

  // nothing to be read
  if (mem_len == 0 || buffer == NULL || (!get_address && *buffer == NULL)) {
    return 0;
  }

  // the tlv start address
  unsigned char* tlv = (unsigned char*) mem;
  unsigned int remlen = mem_len;
  ret = 0;

  // account for a shift in the tlv list before parsing
  tlvoffset_in = 0;
  if (tlvoffset) {
    tlvoffset_in = *tlvoffset;
  }

  // parse tlv until some tag to parse
  while(remlen>=2) {
    // tag matches
    unsigned int tlvtag = *tlv++;
    remlen--;
    if (remlen == 0) {
      goto retret; 
    }
    unsigned int tlvlen = *tlv++;
    remlen--;
    if (remlen == 0) {
      goto retret; 
    }
    if (tlvlen >= 0x80) {
      // invalid encoding
      if (tlvlen == 0x80) {
        goto retret; 
      }
      unsigned int tlvlenlen_ = tlvlen & 0x7F;
      tlvlen = 0;
      while(tlvlenlen_--) {
        // BE encoded
        tlvlen = (tlvlen << 8) | ((*tlv++)&0xFF);
        remlen--;
        if (remlen == 0) {
          goto retret; 
        }
      }
    }
    // check if tag matches
    if (tlvtag == (tag&0xFF)) {
      if (tlvoffset) {
        unsigned int o = (unsigned int) tlv - (unsigned int)mem;
        // compute the current position in the tlv bytes
        *tlvoffset = o;

        // skip the tag if the requested tlvoffset has not been matched yet.
        if (tlvoffset_in>o) {
          goto next_tlv;
        }
      }
      // avoid OOB
      if (offset > tlvlen || offset > remlen) {
        goto retret; 
      }

      // check maxlength is respected for equality
      if (check_equals_buffer && (tlvlen-offset) != maxlength) {
        // buffer to check the complete given length
        goto retret; 
      }

      maxlength = MIN(maxlength, MIN(tlvlen-offset, remlen));
      // robustness check to avoid memory dumping, only allowing data space dumps
      if (
        offset > mem_len
        || maxlength > mem_len
        || offset+maxlength > mem_len
        // don't rely only on provided app bounds to avoid address forgery
        || (unsigned int)tlv < (unsigned int)mem 
        || (unsigned int)tlv+offset < (unsigned int)mem 
        || (unsigned int)tlv+offset+maxlength < (unsigned int)mem 
        || (unsigned int)tlv > (unsigned int)mem+mem_len
        || (unsigned int)tlv+offset > (unsigned int)mem+mem_len
        || (unsigned int)tlv+offset+maxlength > (unsigned int)mem+mem_len) {
        goto retret; 
      }

      // retrieve the tlv's data content at the requested offset, and return the total data length
      if (get_address) {
        *buffer = tlv+offset;
        // return the tlv's total length from requested offset
        ret = MIN(tlvlen-offset, remlen);
        goto retret;
      }

      if (!check_equals_buffer) {
        os_memmove(*buffer, tlv+offset, maxlength);
      }
      else {
        ret = os_secure_memcmp(*buffer, tlv+offset, maxlength) == 0;
        goto retret; 
      }
      ret = maxlength;
      goto retret;
    }
  next_tlv:
    // skip to next tlv
    tlv += tlvlen;
    remlen-=MIN(remlen, tlvlen);
  }
retret:
  return ret;
}

