
/*******************************************************************************
*   Ledger Nano S - Secure firmware
*   (c) 2019 Ledger
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

#include "os_io_usb.h"
#include <string.h>

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
  G_io_usb_hid_remaining_length = 0;
  G_io_usb_hid_current_buffer = NULL;
}

/**
 * sent the next io_usb_hid transport chunk (rx on the host, tx on the device)
 */
void io_usb_hid_sent(io_send_t sndfct) {
  unsigned int l;

  // only prepare next chunk if some data to be sent remain
  if (G_io_usb_hid_remaining_length && G_io_usb_hid_current_buffer) {
    // fill the chunk
    os_memset(G_io_usb_ep_buffer, 0, sizeof(G_io_usb_ep_buffer));

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
    }
    else {
      l = ((G_io_usb_hid_remaining_length>IO_HID_EP_LENGTH-5) ? IO_HID_EP_LENGTH-5 : G_io_usb_hid_remaining_length);
      os_memmove(G_io_usb_ep_buffer+5, (const void*)G_io_usb_hid_current_buffer, l);
      G_io_usb_hid_current_buffer += l;
      G_io_usb_hid_remaining_length -= l;   
    }
    // prepare next chunk numbering
    G_io_usb_hid_sequence_number++;
    // send the chunk
    // always padded (USB HID transport) :)
    sndfct(G_io_usb_ep_buffer, sizeof(G_io_usb_ep_buffer));
  }
  // cleanup when everything has been sent (ack for the last sent usb in packet)
  else {
    io_usb_hid_init();

    // we sent the whole response
    G_io_app.apdu_state = APDU_IDLE;
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
