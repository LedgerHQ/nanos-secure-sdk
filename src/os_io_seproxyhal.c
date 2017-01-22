/*******************************************************************************
*   Ledger Nano S - Secure firmware
*   (c) 2016, 2017 Ledger
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

#ifdef OS_IO_SEPROXYHAL

#include "os_io_seproxyhal.h"

#ifdef HAVE_BLE
#include "hci.h"
#endif // HAVE_BLE


#ifdef DEBUG
#define LOG printf
#else 
#define LOG(...)
#endif

volatile io_apdu_state_e G_io_apdu_state; // by default
volatile unsigned short G_io_apdu_offset; // total length already received
volatile unsigned short G_io_apdu_length; // total length to be received
volatile unsigned short G_io_apdu_seq;
volatile io_apdu_media_t G_io_apdu_media;
volatile unsigned int G_button_mask;
volatile unsigned int G_button_same_mask_counter;

void io_seproxyhal_general_status(void) {
  // avoid troubles
  if (io_seproxyhal_spi_is_status_sent()) {
    return;
  }
  // send the general status
  G_io_seproxyhal_spi_buffer[0] = SEPROXYHAL_TAG_GENERAL_STATUS;
  G_io_seproxyhal_spi_buffer[1] = 0;
  G_io_seproxyhal_spi_buffer[2] = 2;
  G_io_seproxyhal_spi_buffer[3] = SEPROXYHAL_TAG_GENERAL_STATUS_LAST_COMMAND>>8;
  G_io_seproxyhal_spi_buffer[4] = SEPROXYHAL_TAG_GENERAL_STATUS_LAST_COMMAND;
  io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, 5);
}

/* it's a status, but it shall be a command instead to avoid perturbating the simple seproxyhal bus logic
void os_io_seproxyhal_general_status_processing(void) {
  // send the general status
  G_io_seproxyhal_spi_buffer[0] = SEPROXYHAL_TAG_GENERAL_STATUS;
  G_io_seproxyhal_spi_buffer[1] = 0;
  G_io_seproxyhal_spi_buffer[2] = 2;
  G_io_seproxyhal_spi_buffer[3] = SEPROXYHAL_TAG_GENERAL_STATUS_MORE_COMMAND>>8;
  G_io_seproxyhal_spi_buffer[4] = SEPROXYHAL_TAG_GENERAL_STATUS_MORE_COMMAND;
  io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, 5);
}
*/


void io_seproxyhal_request_mcu_status(void) {
  // send the general status
  G_io_seproxyhal_spi_buffer[0] = SEPROXYHAL_TAG_REQUEST_STATUS;
  G_io_seproxyhal_spi_buffer[1] = 0;
  G_io_seproxyhal_spi_buffer[2] = 0;
  io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, 3);
}

//#define WAIT_MS(x) { volatile unsigned int i = 0xAA*x; while (i--); }

#ifdef HAVE_IO_USB
#ifdef HAVE_L4_USBLIB
static volatile unsigned char G_io_usb_ep_xfer_len[IO_USB_MAX_ENDPOINTS];
#include "usbd_def.h"
#include "usbd_core.h"
extern USBD_HandleTypeDef USBD_Device;

void io_seproxyhal_handle_usb_event(void) {
  switch(G_io_seproxyhal_spi_buffer[3]) {
    case SEPROXYHAL_TAG_USB_EVENT_RESET:
      // ongoing APDU detected, throw a reset
      USBD_LL_SetSpeed(&USBD_Device, USBD_SPEED_FULL);  
      USBD_LL_Reset(&USBD_Device);
      if (G_io_apdu_media == IO_APDU_MEDIA_USB_HID) {
        THROW(EXCEPTION_IO_RESET);
      }
      break;
    case SEPROXYHAL_TAG_USB_EVENT_SOF:
      USBD_LL_SOF(&USBD_Device);
      break;
    case SEPROXYHAL_TAG_USB_EVENT_SUSPENDED:
      USBD_LL_Suspend(&USBD_Device);
      break;
    case SEPROXYHAL_TAG_USB_EVENT_RESUMED:
      USBD_LL_Resume(&USBD_Device);
      break;
  }
}

uint16_t io_seproxyhal_get_ep_rx_size(uint8_t epnum) {
  return G_io_usb_ep_xfer_len[epnum&0x7F];
}

void io_seproxyhal_handle_usb_ep_xfer_event(void) {
  switch(G_io_seproxyhal_spi_buffer[4]) {
    case SEPROXYHAL_TAG_USB_EP_XFER_SETUP:
      // assume length of setup packet, and that it is on endpoint 0
      USBD_LL_SetupStage(&USBD_Device, &G_io_seproxyhal_spi_buffer[6]);
      break;

    case SEPROXYHAL_TAG_USB_EP_XFER_IN:
      USBD_LL_DataInStage(&USBD_Device, G_io_seproxyhal_spi_buffer[3]&0x7F, &G_io_seproxyhal_spi_buffer[6]);
      break;

    case SEPROXYHAL_TAG_USB_EP_XFER_OUT:
      // saved just in case it is needed ...
      G_io_usb_ep_xfer_len[G_io_seproxyhal_spi_buffer[3]&0x7F] = G_io_seproxyhal_spi_buffer[5];
      USBD_LL_DataOutStage(&USBD_Device, G_io_seproxyhal_spi_buffer[3]&0x7F, &G_io_seproxyhal_spi_buffer[6]);
      break;
  }
}

#else
//no usb lib: X86 for example

void io_seproxyhal_handle_usb_event(void) {
}
void io_seproxyhal_handle_usb_ep_xfer_event(void) { 
}

#endif // HAVE_L4_USBLIB

void io_usb_send_apdu_data(unsigned char* buffer, unsigned short length) {
  unsigned int rx_len;
  unsigned int timeout = 20;

  // won't send if overflowing seproxyhal buffer format
  if (length > 255) {
    return;
  }
  
  G_io_seproxyhal_spi_buffer[0] = SEPROXYHAL_TAG_USB_EP_PREPARE;
  G_io_seproxyhal_spi_buffer[1] = (3+length)>>8;
  G_io_seproxyhal_spi_buffer[2] = (3+length);
  G_io_seproxyhal_spi_buffer[3] = 0x82;
  G_io_seproxyhal_spi_buffer[4] = SEPROXYHAL_TAG_USB_EP_PREPARE_DIR_IN;
  G_io_seproxyhal_spi_buffer[5] = length;
  io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, 6);
  io_seproxyhal_spi_send(buffer, length);

  for (;;) {
    if (!io_seproxyhal_spi_is_status_sent()) {
      io_seproxyhal_general_status();
    }

    rx_len = io_seproxyhal_spi_recv(G_io_seproxyhal_spi_buffer, sizeof(G_io_seproxyhal_spi_buffer), 0);

    // wait for ack of the seproxyhal
    // discard if not an acknowledgment
    if (G_io_seproxyhal_spi_buffer[0] != SEPROXYHAL_TAG_USB_EP_XFER_EVENT
      || rx_len != 6 
      || G_io_seproxyhal_spi_buffer[3] != 0x82 
      || G_io_seproxyhal_spi_buffer[4] != SEPROXYHAL_TAG_USB_EP_XFER_IN
      || G_io_seproxyhal_spi_buffer[5] != length) {
      // usb reset ?
      io_seproxyhal_handle_usb_event();

      // link disconnected ?
      if(G_io_seproxyhal_spi_buffer[0] == SEPROXYHAL_TAG_STATUS_EVENT) {
        if (!(U4BE(G_io_seproxyhal_spi_buffer, 3) & SEPROXYHAL_TAG_STATUS_EVENT_FLAG_USB_POWERED)) {
         THROW(EXCEPTION_IO_RESET);
        }
      }

      if (!io_event(CHANNEL_SPI)) {
        LOG("missing NOTIFY_INDICATE event, %02X received\n", G_io_seproxyhal_spi_buffer[0]);
      }

      // handle loss of communication with the host
      if (! timeout--) {
        THROW(EXCEPTION_IO_RESET);
      }

      // no general status ack, io_event is responsible for it
      continue;
    }

    // chunk sending succeeded
    break;
  }
}

#endif // HAVE_IO_USB

#ifdef HAVE_BLE
void io_seproxyhal_handle_bluenrg_event(void) {
  BEGIN_TRY {
    TRY {
      // handle the incoming packet       
      HCI_recv_packet(G_io_seproxyhal_spi_buffer+3, U2BE(G_io_seproxyhal_spi_buffer, 1));
      
    }
    /*
    CATCH(EXCEPTION_IO_RESET) {
      // rethrow that one
      THROW(EXCEPTION_IO_RESET);
    }
    CATCH_ALL {
      // ignore
    }
    */
    FINALLY {

    }
  }
  END_TRY;
}

#else
void io_seproxyhal_handle_bluenrg_event(void) {

}
#endif

unsigned int io_seproxyhal_handle_event(void) {
  unsigned int rx_len = U2BE(G_io_seproxyhal_spi_buffer, 1);

  switch(G_io_seproxyhal_spi_buffer[0]) {
  #ifdef HAVE_IO_USB
    case SEPROXYHAL_TAG_USB_EVENT:
      if (rx_len != 3+1) {
        return 0;
      }
      io_seproxyhal_handle_usb_event();
      return 1;

    case SEPROXYHAL_TAG_USB_EP_XFER_EVENT:
      if (rx_len < 3+3) {
        // error !
        return 0;
      }
      io_seproxyhal_handle_usb_ep_xfer_event();
      return 1;
  #endif // HAVE_IO_USB

  #ifdef HAVE_BLE
    case SEPROXYHAL_TAG_BLUENRG_RECV_EVENT:
      io_seproxyhal_handle_bluenrg_event();
      return 1;
  #endif // HAVE_BLE

      // ask the user if not processed here
    default:
      return io_event(CHANNEL_SPI);
  }
  // defaulty return as not processed
  return 0;
}

bagl_element_t* volatile G_bagl_last_touched_not_released_component;

//#define DEBUG_APDU
#ifdef DEBUG_APDU
volatile unsigned int debug_apdus_offset;
const char debug_apdus[] = {
  9, 0xe0, 0x22, 0x00, 0x00, 0x04, 0x31, 0x32, 0x33, 0x34,
};
#endif // DEBUG_APDU

void io_seproxyhal_init(void) {
  // Enforce OS compatibility
  check_api_level(CX_COMPAT_APILEVEL);

  G_io_apdu_state = APDU_IDLE;
  G_io_apdu_offset = 0;
  G_io_apdu_length = 0;
  G_io_apdu_seq = 0;
  G_io_apdu_media = IO_APDU_MEDIA_NONE;

  #ifdef DEBUG_APDU
  debug_apdus_offset = 0;
  #endif // DEBUG_APDU


  #ifdef HAVE_USB_APDU
  io_usb_hid_init();
  #endif // HAVE_USB_APDU

  io_seproxyhal_init_ux();

  // init ticker (not to be done on blue)
  io_seproxyhal_setup_ticker(100);
}

void io_seproxyhal_init_ux(void) {
  // initialize the touch part
  G_bagl_last_touched_not_released_component = NULL;

  // no button push so far
  G_button_mask = 0;
  G_button_same_mask_counter = 0;
}

#ifdef HAVE_BAGL

unsigned int io_seproxyhal_touch_out(const bagl_element_t* element, bagl_element_callback_t before_display) {
  const bagl_element_t* el;
  if (element->out != NULL) {
    el = (const bagl_element_t*)PIC(((bagl_element_callback_t)PIC(element->out))(element));
    // backward compatible with samples and such
    if (! el) {
      return 0;
    }
    if ((unsigned int)el != 1) {
      element = el;
    }
  }

  // out function might have triggered a draw of its own during a display callback
  if (before_display) {
    el = before_display(element);
    if (!el) {
      return 0;
    }
    if ((unsigned int)el != 1) {
      element = el;
    }
  }

  io_seproxyhal_display(element);
  return 1;
}

unsigned int io_seproxyhal_touch_over(const bagl_element_t* element, bagl_element_callback_t before_display) {
  bagl_element_t e;
  const bagl_element_t* el;
  if (element->over != NULL) {
    el = (const bagl_element_t*)PIC(((bagl_element_callback_t)PIC(element->over))(element));
    // backward compatible with samples and such
    if (!el) {
      return 0;
    }
    if ((unsigned int)el != 1) {
      element = el;
    }
  }

  // over function might have triggered a draw of its own during a display callback
  os_memmove(&e, (void*)element, sizeof(bagl_element_t));
  e.component.fgcolor = element->overfgcolor;
  e.component.bgcolor = element->overbgcolor;

  //element = &e; // for INARRAY checks, it disturbs a bit. avoid it

  if (before_display) {
    el = before_display(element);
    element = &e;
    if (!el) {
      return 0;
    }
    // problem for default screen_before_before_display where we return the given element, it have could been modified. but we don't know here
    if ((unsigned int)el != 1) {
      element = el;
    }
  }

  //else 
  {
    element = &e;
  }

  io_seproxyhal_display(element);
  return 1;
}

unsigned int io_seproxyhal_touch_tap(const bagl_element_t* element, bagl_element_callback_t before_display) {
  const bagl_element_t* el;
  if (element->tap != NULL) {
    el = (const bagl_element_t*)PIC(((bagl_element_callback_t)PIC(element->tap))(element));
    // backward compatible with samples and such
    if (!el) {
      return 0;
    }
    if ((unsigned int)el != 1) {
      element = el;
    }
  }

  // tap function might have triggered a draw of its own during a display callback
  if (before_display) {
    el = before_display(element);
    if (!el) {
      return 0;
    }
    if ((unsigned int)el != 1) {
      element = el;
    }
  }
  io_seproxyhal_display(element);
  return 1;
}

void io_seproxyhal_touch(const bagl_element_t* elements, unsigned short element_count, unsigned short x, unsigned short y, unsigned char event_kind) {
  io_seproxyhal_touch_element_callback(elements, element_count, x, y, event_kind, NULL);  
}

// browse all elements and until an element has changed state, continue browsing
// return if processed or not
void io_seproxyhal_touch_element_callback(const bagl_element_t* elements, unsigned short element_count, unsigned short x, unsigned short y, unsigned char event_kind, bagl_element_callback_t before_display) {
  unsigned char comp_idx;
  unsigned char last_touched_not_released_component_was_in_current_array = 0;

  // find the first empty entry
  for (comp_idx=0; comp_idx < element_count; comp_idx++) {
    // process all components matching the x/y/w/h (no break) => fishy for the released out of zone
    // continue processing only if a status has not been sent
    if (io_seproxyhal_spi_is_status_sent()) {
      // continue instead of return to process all elemnts and therefore discard last touched element
      break;
    }

    // only perform out callback when element was in the current array, else, leave it be
    if (&elements[comp_idx] == G_bagl_last_touched_not_released_component) {
      last_touched_not_released_component_was_in_current_array = 1;
    }

    // the first component drawn with a 
    if ((elements[comp_idx].component.type & BAGL_FLAG_TOUCHABLE) 
        && elements[comp_idx].component.x-elements[comp_idx].touch_area_brim <= x && x<elements[comp_idx].component.x+elements[comp_idx].component.width+elements[comp_idx].touch_area_brim
        && elements[comp_idx].component.y-elements[comp_idx].touch_area_brim <= y && y<elements[comp_idx].component.y+elements[comp_idx].component.height+elements[comp_idx].touch_area_brim) {

      // outing the previous over'ed component
      if (&elements[comp_idx] != G_bagl_last_touched_not_released_component 
              && G_bagl_last_touched_not_released_component != NULL) {
        // only out the previous element if the newly matching will be displayed 
        if (!before_display || before_display(&elements[comp_idx])) {
          if (io_seproxyhal_touch_out(G_bagl_last_touched_not_released_component, before_display)) {
            // previous component is considered released
            G_bagl_last_touched_not_released_component = NULL;
            // a display has been issued, avoid double display, wait for another touch event (20ms)
            return;
          }
        }
        // avoid a non displayed new element to pop out of the blue
        continue;
      }

      /*
      if (io_seproxyhal_spi_is_status_sent()) {
        // continue instead of return to process all elements and therefore discard last touched element
        continue;
      }
      */
      
      // callback the hal to notify the component impacted by the user input
      else if (event_kind == SEPROXYHAL_TAG_FINGER_EVENT_RELEASE) {
        if (io_seproxyhal_touch_tap(&elements[comp_idx], before_display)) {
          // unmark the last component, we've been notified TOUCH 
          G_bagl_last_touched_not_released_component = NULL;
          return;
        }
      }
      else if (event_kind == SEPROXYHAL_TAG_FINGER_EVENT_TOUCH) {
        // ask for overing
        if (io_seproxyhal_touch_over(&elements[comp_idx], before_display)) {
          // remember the last touched component
          G_bagl_last_touched_not_released_component = (bagl_element_t*)&elements[comp_idx];
          return;
        }
      }
    }
  }

  // if overing out of component or over another component, the out event is sent after the over event of the previous component
  if(last_touched_not_released_component_was_in_current_array 
    && G_bagl_last_touched_not_released_component != NULL) {

    // we won't be able to notify the out, don't do it, in case a diplay refused the dra of the relased element and the position matched another element of the array (in autocomplete for example)
    if (io_seproxyhal_spi_is_status_sent()) {
      return;
    }
    
    if (io_seproxyhal_touch_out(G_bagl_last_touched_not_released_component, before_display)) {
      // ok component out has been emitted
      G_bagl_last_touched_not_released_component = NULL;
    }
  }

  // not processed
}

void io_seproxyhal_display_default(bagl_element_t * element) {
  // process automagically address from rom and from ram
  unsigned int type = (element->component.type & ~(BAGL_FLAG_TOUCHABLE));

  if (type != BAGL_NONE) {
    if (element->text != NULL) {
      unsigned int text_adr = PIC((unsigned int)element->text);
      unsigned short length = sizeof(bagl_component_t)+strlen((const char*)text_adr);
      G_io_seproxyhal_spi_buffer[0] = SEPROXYHAL_TAG_SCREEN_DISPLAY_STATUS;
      G_io_seproxyhal_spi_buffer[1] = length>>8;
      G_io_seproxyhal_spi_buffer[2] = length;
      io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, 3);
      io_seproxyhal_spi_send((const void*)&element->component, sizeof(bagl_component_t));
      io_seproxyhal_spi_send((const void*)text_adr, length-sizeof(bagl_component_t));
    }
    else {
      unsigned short length = sizeof(bagl_component_t);
      G_io_seproxyhal_spi_buffer[0] = SEPROXYHAL_TAG_SCREEN_DISPLAY_STATUS;
      G_io_seproxyhal_spi_buffer[1] = length>>8;
      G_io_seproxyhal_spi_buffer[2] = length;
      io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, 3);
      io_seproxyhal_spi_send((const void*)&element->component, sizeof(bagl_component_t));
    }
  }
}

void io_seproxyhal_display_bitmap(int x, int y, unsigned int w, unsigned int h, unsigned int* color_index, unsigned int bit_per_pixel, unsigned char* bitmap) {
  // component type = ICON
  // component icon id = 0
  // => bitmap transmitted
  bagl_component_t c;
  os_memset(&c, 0, sizeof(c));
  c.type = BAGL_ICON;
  c.x = x;
  c.y = y;
  c.width = w;
  c.height = h;
  // done by memset // c.icon_id = 0;

  // color index size
  h = ((1<<bit_per_pixel)*sizeof(unsigned int)); 
  // bitmap size
  w = ((w*c.height*bit_per_pixel)/8)+((w*c.height*bit_per_pixel)%8?1:0);
  unsigned short length = sizeof(bagl_component_t)
                          +1 /* bpp */
                          +h /* color index */
                          +w; /* image bitmap */
  G_io_seproxyhal_spi_buffer[0] = SEPROXYHAL_TAG_SCREEN_DISPLAY_STATUS;
  G_io_seproxyhal_spi_buffer[1] = length>>8;
  G_io_seproxyhal_spi_buffer[2] = length;
  io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, 3);
  io_seproxyhal_spi_send((unsigned char*)&c, sizeof(bagl_component_t));
  G_io_seproxyhal_spi_buffer[0] = bit_per_pixel;
  io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, 1);
  io_seproxyhal_spi_send((unsigned char*)color_index, h);
  io_seproxyhal_spi_send(bitmap, w);
}

void io_seproxyhal_display_icon(bagl_component_t* icon_component, bagl_icon_details_t* icon_details) {
#ifdef SEPROXYHAL_TAG_SCREEN_DISPLAY_RAW_STATUS
  unsigned int len;
  unsigned int icon_len;
  unsigned int icon_off=0;

  struct display_raw_s {
    struct {
      struct {
        unsigned char tag;
        unsigned char len[2];
      } seph;
      unsigned char type;
    } header;
    union {
      short val;
      char b[2];
    } x;
    union {
      short val;
      char b[2];
    } y;
    union {
      unsigned short val;
      char b[2];
    } w;
    union {
      unsigned short val;
      char b[2];
    } h;
    unsigned char bpp;
  } __attribute__((packed)) raw;

  raw.header.seph.tag = SEPROXYHAL_TAG_SCREEN_DISPLAY_RAW_STATUS;
  raw.header.type = SEPROXYHAL_TAG_SCREEN_DISPLAY_RAW_STATUS_START;
  raw.x.val = icon_component->x;
  raw.y.val = icon_component->y;
  raw.w.val = icon_component->width;
  raw.h.val = icon_component->height;
  raw.bpp = icon_details->bpp;


  icon_len = raw.w.val*raw.h.val*raw.bpp/8 + (((raw.w.val*raw.h.val*raw.bpp)%8)?1:0);

  // optional, don't send too much on a single packet for MCU to receive it. when stream mode will be on, this will be useless
  // min of remaining space in the packet vs. total icon size + color index size
  len = MIN(sizeof(G_io_seproxyhal_spi_buffer) - sizeof(raw), icon_len + (1<<raw.bpp)*4);

  // sizeof packet
  raw.header.seph.len[0] = (len + sizeof(raw) - sizeof(raw.header.seph)) >> 8;
  raw.header.seph.len[1] = (len + sizeof(raw) - sizeof(raw.header.seph));

  // swap endianess of coordinates
  SWAP(raw.x.b[0], raw.x.b[1]);
  SWAP(raw.y.b[0], raw.y.b[1]);
  SWAP(raw.w.b[0], raw.w.b[1]);
  SWAP(raw.h.b[0], raw.h.b[1]);

  io_seproxyhal_spi_send(&raw, sizeof(raw));
  io_seproxyhal_spi_send(PIC(icon_details->colors), (1<<raw.bpp)*4);
  len -= (1<<raw.bpp)*4;
  io_seproxyhal_spi_send(PIC(icon_details->bitmap), len);
  // advance in the bitmap to be transmitted
  icon_len -= len;
  icon_off += len;

  // still some bitmap data to transmit
  while(icon_len) {
    // wait displayed event
    io_seproxyhal_spi_recv(G_io_seproxyhal_spi_buffer, sizeof(G_io_seproxyhal_spi_buffer), 0);
    
    raw.header.type = SEPROXYHAL_TAG_SCREEN_DISPLAY_RAW_STATUS_CONT;
    
    len = MIN((sizeof(G_io_seproxyhal_spi_buffer) - sizeof(raw.header)), icon_len);
    raw.header.seph.len[0] = (len+1)>>8;
    raw.header.seph.len[1] = (len+1);
    io_seproxyhal_spi_send(&raw.header, sizeof(raw.header));
    io_seproxyhal_spi_send(PIC(icon_details->bitmap)+icon_off, len);

    icon_len -= len;
    icon_off += len;
  }
#endif // SEPROXYHAL_TAG_SCREEN_DISPLAY_RAW_STATUS
}

unsigned int bagl_label_roundtrip_duration_ms(const bagl_element_t* e, unsigned int average_char_width) {
  // not a scrollable label
  if (e == NULL || (e->component.type != BAGL_LABEL && e->component.type != BAGL_LABELINE)) {
    return 0;
  }
  
  unsigned int text_adr = PIC((unsigned int)e->text);
  unsigned int textlen = 0;
  
  // no delay, no text to display
  if (!text_adr) {
    return 0;
  }
  textlen = strlen((const char*)text_adr);
  
  // no delay, all text fits
  textlen = textlen * average_char_width;
  if (textlen <= e->component.width) {
    return 0; 
  }
  
  // compute scrolled text length
  return 2*(textlen - e->component.width)*1000/e->component.icon_id + 2*(e->component.stroke & ~(0x80))*100;
}

void io_seproxyhal_setup_ticker(unsigned int interval_ms) {
  G_io_seproxyhal_spi_buffer[0] = SEPROXYHAL_TAG_SET_TICKER_INTERVAL;
  G_io_seproxyhal_spi_buffer[1] = 0;
  G_io_seproxyhal_spi_buffer[2] = 2;
  G_io_seproxyhal_spi_buffer[3] = (interval_ms>>8)&0xff;
  G_io_seproxyhal_spi_buffer[4] = (interval_ms)&0xff;
  io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, 5);
}


void io_seproxyhal_power_off(void) {
  G_io_seproxyhal_spi_buffer[0] = SEPROXYHAL_TAG_DEVICE_OFF;
  G_io_seproxyhal_spi_buffer[1] = 0;
  G_io_seproxyhal_spi_buffer[2] = 0;
  io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, 3);
}

void io_seproxyhal_se_reset(void) {
  G_io_seproxyhal_spi_buffer[0] = SEPROXYHAL_TAG_SE_POWER_OFF;
  G_io_seproxyhal_spi_buffer[1] = 0;
  G_io_seproxyhal_spi_buffer[2] = 0;
  io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, 3);
}

void io_seproxyhal_disable_io(void) {
    /* keep ticker on for BOLOS_UX power/lock management
    // disable ticker
    G_io_seproxyhal_spi_buffer[0] = SEPROXYHAL_TAG_SET_TICKER_INTERVAL;
    G_io_seproxyhal_spi_buffer[1] = 0;
    G_io_seproxyhal_spi_buffer[2] = 2;
    G_io_seproxyhal_spi_buffer[3] = 0;
    G_io_seproxyhal_spi_buffer[4] = 0;
    io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, 5);
    */

    // ble off
    G_io_seproxyhal_spi_buffer[0] = SEPROXYHAL_TAG_BLE_RADIO_POWER;
    G_io_seproxyhal_spi_buffer[1] = 0;
    G_io_seproxyhal_spi_buffer[2] = 1;
    G_io_seproxyhal_spi_buffer[3] = 0;
    io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, 4);

    // usb off
    G_io_seproxyhal_spi_buffer[0] = SEPROXYHAL_TAG_USB_CONFIG;
    G_io_seproxyhal_spi_buffer[1] = 0;
    G_io_seproxyhal_spi_buffer[2] = 1;
    G_io_seproxyhal_spi_buffer[3] = SEPROXYHAL_TAG_USB_CONFIG_DISCONNECT;
    io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, 4);  
}

void io_seproxyhal_backlight(unsigned int flags, unsigned int backlight_percentage) {
  G_io_seproxyhal_spi_buffer[0] = SEPROXYHAL_TAG_SET_SCREEN_CONFIG;
  G_io_seproxyhal_spi_buffer[1] = 0;
  G_io_seproxyhal_spi_buffer[2] = 2;
  G_io_seproxyhal_spi_buffer[3] = (backlight_percentage?0x80:0)|(flags & 0x7F); // power on
  G_io_seproxyhal_spi_buffer[4] = backlight_percentage;
  io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, 5);
}

void io_seproxyhal_button_push(button_push_callback_t button_callback, unsigned int new_button_mask) {
  if (button_callback) {
    unsigned int button_mask;
    unsigned int button_same_mask_counter;
    // enable speeded up long push
    if (new_button_mask == G_button_mask) {
      G_button_same_mask_counter++;
    }

    // append the button mask
    button_mask = G_button_mask | new_button_mask;

    // pre reset variable due to os_sched_exit
    button_same_mask_counter = G_button_same_mask_counter;

    // reset button mask
    if (new_button_mask == 0) {
      // reset next state when button are released
      G_button_mask = 0;
      G_button_same_mask_counter=0;

      // notify button released event
      button_mask |= BUTTON_EVT_RELEASED;
    }
    else {
      G_button_mask = button_mask;
    }

    // reset counter when button mask changes
    if (new_button_mask != G_button_mask) {
      G_button_same_mask_counter=0;
    }

    // indicate if button have been released
    button_callback(button_mask, button_same_mask_counter);
  }
}

#endif // HAVE_BAGL

unsigned short io_exchange(unsigned char channel, unsigned short tx_len) {
  unsigned short rx_len;

#ifdef DEBUG_APDU
  if ((channel&~(IO_FLAGS)) == CHANNEL_APDU) {
    // ignore tx len

    // already received the data of the apdu when received the whole apdu
    if ((channel & (CHANNEL_APDU|IO_RECEIVE_DATA)) == (CHANNEL_APDU|IO_RECEIVE_DATA)) {
      // return apdu data - header
      return G_io_apdu_length-5;
    }

    // fetch next apdu
    if (debug_apdus_offset < sizeof(debug_apdus)) {
      G_io_apdu_length = debug_apdus[debug_apdus_offset]&0xFF;
      os_memmove(G_io_apdu_buffer, &debug_apdus[debug_apdus_offset+1], G_io_apdu_length);
      debug_apdus_offset += G_io_apdu_length+1;
      return G_io_apdu_length;
    }
  }
  after_debug:
#endif // DEBUG_APDU

  switch(channel&~(IO_FLAGS)) {
  case CHANNEL_APDU:
    // TODO work up the spi state machine over the HAL proxy until an APDU is available

    if (tx_len && !(channel&IO_ASYNCH_REPLY)) {

      // until the whole RAPDU is transmitted, send chunks using the current mode for communication
      for (;;) {
        switch(G_io_apdu_state) {
          default: 
            // delegate to the hal in case of not generic transport mode (or asynch)
            if (io_exchange_al(channel, tx_len) == 0) {
              goto break_send;
            }
          case APDU_IDLE:
            LOG("invalid state for APDU reply\n");
            THROW(INVALID_STATE);
            break;

#ifdef HAVE_USB_APDU
          case APDU_USB_HID:
            // only send, don't perform synchronous reception of the next command (will be done later by the seproxyhal packet processing)
            io_usb_hid_exchange(io_usb_send_apdu_data, tx_len, NULL, IO_RETURN_AFTER_TX);
            goto break_send;
#ifdef HAVE_USB_CLASS_CCID
          case APDU_USB_CCID:
            io_usb_ccid_reply(G_io_apdu_buffer, tx_len);
            goto break_send;
#endif // HAVE_USB_CLASS_CCID
#endif // HAVE_USB_APDU

#ifdef HAVE_BLE_APDU
          case APDU_BLE:
            BLE_protocol_send(G_io_apdu_buffer, tx_len);
            goto break_send;
#endif // HAVE_BLE_APDU



#ifdef HAVE_M24SR_NFC
          case APDU_NFC_M24SR:
            // split in apdu to write into the M24SR and wait M24SR rapdu response.
            // select the NDEF file
            // issue an APDU to read the first block content (containing the total apdu length) (read as much as possible)
            G_io_seproxyhal_spi_buffer[0] = SEPROXYHAL_TAG_M24SR_C_APDU;
            G_io_seproxyhal_spi_buffer[1] = 0;
            G_io_seproxyhal_spi_buffer[2] = 7;
            // Forge NDEF Select command
            G_io_seproxyhal_spi_buffer[3] = 0x00;
            G_io_seproxyhal_spi_buffer[4] = 0xA4;
            G_io_seproxyhal_spi_buffer[5] = 0x00;
            G_io_seproxyhal_spi_buffer[6] = 0x0C;
            G_io_seproxyhal_spi_buffer[7] = 0x02;
            G_io_seproxyhal_spi_buffer[8] = 0x00;
            G_io_seproxyhal_spi_buffer[9] = 0x01;

            // wait for a NFC_RESPONSE
            G_io_apdu_state = APDU_NFC_M24SR_SELECT;
            io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, 10);
            break;

          case APDU_NFC_M24SR_SELECT:
            // wait for select reply 
            rx_len = io_seproxyhal_spi_recv(G_io_seproxyhal_spi_buffer, sizeof(G_io_seproxyhal_spi_buffer), 0);

            if (rx_len-3 != U2(G_io_seproxyhal_spi_buffer[1],G_io_seproxyhal_spi_buffer[2])) {
              LOG("invalid TLV format\n");
              goto invalid_apdu_packet;
            }
            // expect M24SR RAPDU with 0x90 0x00
            if (G_io_seproxyhal_spi_buffer[0] != SEPROXYHAL_TAG_M24SR_RESPONSE_APDU_EVENT) {
              if (!io_event(CHANNEL_SPI)) {
                LOG("invalid SELECT reply\n");
                //goto invalid_apdu_packet;
              }

              // close the event if not done previously (by a display or whatever)
              if (!io_seproxyhal_spi_is_status_sent()) {
                  io_seproxyhal_general_status();
              }
              continue;
            }
            if (G_io_seproxyhal_spi_buffer[1] != 0 || G_io_seproxyhal_spi_buffer[2] != 2 || G_io_seproxyhal_spi_buffer[3] != 0x90 || G_io_seproxyhal_spi_buffer[4] != 0x00) {
              LOG("invalid SELECT reply\n");
              goto invalid_apdu_packet;
            }

            G_io_apdu_length = tx_len;
            G_io_apdu_offset = 0;

            tx_len = MIN(0xF6-2, G_io_apdu_length);

            // forge the first update binary, putting 0x00 0x00 into the RAPDU NDEF file length
            G_io_seproxyhal_spi_buffer[0] = SEPROXYHAL_TAG_M24SR_C_APDU;
            G_io_seproxyhal_spi_buffer[1] = 0;
            G_io_seproxyhal_spi_buffer[2] = tx_len + 5 /*update binary header*/ + 2 /* NDEF file length*/;
            // Forge NDEF Select command
            G_io_seproxyhal_spi_buffer[3] = 0x00;
            G_io_seproxyhal_spi_buffer[4] = 0xD6;
            G_io_seproxyhal_spi_buffer[5] = 0x00;
            G_io_seproxyhal_spi_buffer[6] = 0x00;
            G_io_seproxyhal_spi_buffer[7] = tx_len+2 /*NDEF file length*/;
            G_io_seproxyhal_spi_buffer[8] = 0x00;
            G_io_seproxyhal_spi_buffer[9] = 0x00;
            os_memmove(G_io_seproxyhal_spi_buffer+10, G_io_apdu_buffer+G_io_apdu_offset, tx_len);
            G_io_apdu_offset = tx_len;

            // wait for a NFC_RESPONSE
            G_io_apdu_state = APDU_NFC_M24SR_RAPDU;
            io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, G_io_seproxyhal_spi_buffer[2]+3);
            break;


          case APDU_NFC_M24SR_RAPDU:
            // wait for update binary reply 
            rx_len = io_seproxyhal_spi_recv(G_io_seproxyhal_spi_buffer, sizeof(G_io_seproxyhal_spi_buffer), 0);

            if (rx_len-3 != U2(G_io_seproxyhal_spi_buffer[1],G_io_seproxyhal_spi_buffer[2])) {
              LOG("invalid TLV format\n");
              goto invalid_apdu_packet;
            }
            // expect M24SR RAPDU with 0x90 0x00
            if (G_io_seproxyhal_spi_buffer[0] != SEPROXYHAL_TAG_M24SR_RESPONSE_APDU_EVENT) {
              if (!io_event(CHANNEL_SPI)) {
                LOG("invalid UPDATE BINARY reply\n");
                //goto invalid_apdu_packet;
              }

              // close the event if not done previously (by a display or whatever)
              if (!io_seproxyhal_spi_is_status_sent()) {
                  io_seproxyhal_general_status();
              }
              continue;
            } 
            if (G_io_seproxyhal_spi_buffer[1] != 0 || G_io_seproxyhal_spi_buffer[2] != 2 || G_io_seproxyhal_spi_buffer[3] != 0x90 || G_io_seproxyhal_spi_buffer[4] != 0x00) {
              LOG("invalid UPDATE BINARY reply\n");
              goto invalid_apdu_packet;
            }

            if (G_io_apdu_length == G_io_apdu_offset) {
              // update ndef length
              G_io_seproxyhal_spi_buffer[0] = SEPROXYHAL_TAG_M24SR_C_APDU;
              G_io_seproxyhal_spi_buffer[1] = 0;
              G_io_seproxyhal_spi_buffer[2] = 7 /*update binary header*/;
              // Forge NDEF Select command
              G_io_seproxyhal_spi_buffer[3] = 0x00;
              G_io_seproxyhal_spi_buffer[4] = 0xD6;
              G_io_seproxyhal_spi_buffer[5] = 0x00;
              G_io_seproxyhal_spi_buffer[6] = 0x00;
              G_io_seproxyhal_spi_buffer[7] = 2;
              G_io_seproxyhal_spi_buffer[8] = G_io_apdu_length>>8;
              G_io_seproxyhal_spi_buffer[9] = G_io_apdu_length;

              G_io_apdu_state = APDU_NFC_M24SR_FIRST;
              io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, G_io_seproxyhal_spi_buffer[2]+3);
            }
            else {
              // update a part of the apdu
              tx_len = MIN(0xF6, G_io_apdu_length-G_io_apdu_offset);

              // forge the first update binary, putting 0x00 0x00 into the RAPDU NDEF file length
              G_io_seproxyhal_spi_buffer[0] = SEPROXYHAL_TAG_M24SR_C_APDU;
              G_io_seproxyhal_spi_buffer[1] = 0;
              G_io_seproxyhal_spi_buffer[2] = tx_len + 5 /*update binary header*/;
              // Forge NDEF Select command
              G_io_seproxyhal_spi_buffer[3] = 0x00;
              G_io_seproxyhal_spi_buffer[4] = 0xD6;
              G_io_seproxyhal_spi_buffer[5] = (G_io_apdu_offset + 2 /*NDEF file length*/)>>8;
              G_io_seproxyhal_spi_buffer[6] = (G_io_apdu_offset + 2 /*NDEF file length*/);
              G_io_seproxyhal_spi_buffer[7] = tx_len;
              os_memmove(G_io_seproxyhal_spi_buffer+8, G_io_apdu_buffer+G_io_apdu_offset, tx_len);
              G_io_apdu_offset += tx_len;

              // wait for a NFC_RESPONSE
              io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, G_io_seproxyhal_spi_buffer[2]+3);
            }
            break;

          // update the NDEF length after all the content of the RAPDU has been written
          case APDU_NFC_M24SR_FIRST:
            // wait for last update binary reply 
            rx_len = io_seproxyhal_spi_recv(G_io_seproxyhal_spi_buffer, sizeof(G_io_seproxyhal_spi_buffer), 0);

            if (rx_len-3 != U2(G_io_seproxyhal_spi_buffer[1],G_io_seproxyhal_spi_buffer[2])) {
              LOG("invalid TLV format\n");
              goto invalid_apdu_packet;
            }
            // expect M24SR RAPDU with 0x90 0x00
            if (G_io_seproxyhal_spi_buffer[0] != SEPROXYHAL_TAG_M24SR_RESPONSE_APDU_EVENT) {
              if (!io_event(CHANNEL_SPI)) {
                LOG("invalid NDEF file length UPDATE BINARY reply\n");
                //goto invalid_apdu_packet;
              }

              // close the event if not done previously (by a display or whatever)
              if (!io_seproxyhal_spi_is_status_sent()) {
                  io_seproxyhal_general_status();
              }
              continue;
            }
            if (G_io_seproxyhal_spi_buffer[1] != 0 || G_io_seproxyhal_spi_buffer[2] != 2 || G_io_seproxyhal_spi_buffer[3] != 0x90 || G_io_seproxyhal_spi_buffer[4] != 0x00) {
              LOG("invalid NDEF file length UPDATE BINARY reply\n");
              goto invalid_apdu_packet;
            }
            goto break_send;
#endif // HAVE_M24SR_NFC
        }
        continue;

      break_send:
        // reset apdu state
        G_io_apdu_state = APDU_IDLE;
        G_io_apdu_offset = 0;
        G_io_apdu_length = 0;
        G_io_apdu_seq = 0;
        G_io_apdu_media = IO_APDU_MEDIA_NONE;

        // continue sending commands, don't issue status yet
        if (channel & IO_RETURN_AFTER_TX) {
          return 0;
        }
        // acknowledge the write request (general status OK) and no more command to follow (wait until another APDU container is received to continue unwrapping)
        io_seproxyhal_general_status();
        break;
      }

      // perform reset after io exchange
      if (channel & IO_RESET_AFTER_REPLIED) {
        reset();
      }
    }

    if (!(channel&IO_ASYNCH_REPLY)) {
      
      // already received the data of the apdu when received the whole apdu
      if ((channel & (CHANNEL_APDU|IO_RECEIVE_DATA)) == (CHANNEL_APDU|IO_RECEIVE_DATA)) {
        // return apdu data - header
        return G_io_apdu_length-5;
      }

      // reply has ended, proceed to next apdu reception (reset status only after asynch reply)
      G_io_apdu_state = APDU_IDLE;
      G_io_apdu_offset = 0;
      G_io_apdu_length = 0;
      G_io_apdu_seq = 0;
      G_io_apdu_media = IO_APDU_MEDIA_NONE;
    }

    // ensure ready to receive an event (after an apdu processing with asynch flag, it may occur if the channel is not correctly managed)
    if (!io_seproxyhal_spi_is_status_sent()) {
      io_seproxyhal_general_status();
    }

    // until a new whole CAPDU is received
    for (;;) {

      // wait until a SPI packet is available
      // NOTE: on ST31, dual wait ISO & RF (ISO instead of SPI)
      rx_len = io_seproxyhal_spi_recv(G_io_seproxyhal_spi_buffer, sizeof(G_io_seproxyhal_spi_buffer), 0);

      // can't process split TLV, continue
      if (rx_len-3 != U2(G_io_seproxyhal_spi_buffer[1],G_io_seproxyhal_spi_buffer[2])) {
        LOG("invalid TLV format\n");
      invalid_apdu_packet:
        G_io_apdu_state = APDU_IDLE;
        G_io_apdu_offset = 0;
        G_io_apdu_length = 0;
        G_io_apdu_seq = 0;

      send_last_command:
        io_seproxyhal_general_status();
        continue;
      }

      // depending on received TAG
      switch(G_io_seproxyhal_spi_buffer[0]) {

#ifdef HAVE_BLE
        case SEPROXYHAL_TAG_BLUENRG_RECV_EVENT:
          // process the packet
          io_seproxyhal_handle_bluenrg_event();

          // if the ble apdu state has advanced
          if (G_io_apdu_length) {
            G_io_apdu_media = IO_APDU_MEDIA_BLE; // for application code
            G_io_apdu_state = APDU_BLE; // for next call to io_exchange
            return G_io_apdu_length;
          } 
          goto send_last_command;
#endif // HAVE_BLE

#ifdef HAVE_IO_USB
        case SEPROXYHAL_TAG_USB_EVENT:
          if (rx_len != 3+1) {
            // invalid length, not processable
            goto invalid_apdu_packet;
          }
          io_seproxyhal_handle_usb_event();

          // no state change, we're not dealing with an apdu yet
          goto send_last_command;

        case SEPROXYHAL_TAG_USB_EP_XFER_EVENT:
          if (rx_len < 3+3) {
            // error !
            return 0;
          }
          io_seproxyhal_handle_usb_ep_xfer_event();

          // an apdu has been received, ack with mode commands (the reply at least)
          if (G_io_apdu_length > 0) {
            /* we keep the hand, no need to inform (this disrupt the spi_is_status_sent check)
            // acknowledge the write request (general status OK) and more command to follow (processing of the apdu)
            G_io_seproxyhal_spi_buffer[0] = SEPROXYHAL_TAG_GENERAL_STATUS;
            G_io_seproxyhal_spi_buffer[1] = 0;
            G_io_seproxyhal_spi_buffer[2] = 2;
            G_io_seproxyhal_spi_buffer[3] = SEPROXYHAL_TAG_GENERAL_STATUS_MORE_COMMAND>>8;
            G_io_seproxyhal_spi_buffer[4] = SEPROXYHAL_TAG_GENERAL_STATUS_MORE_COMMAND;
            io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, 5);
            */
            return G_io_apdu_length;
          }
          else {
            goto send_last_command;
          }
          break;
#endif // HAVE_IO_USB


#ifdef HAVE_M24SR_NFC
        // event triggered upon write of the 
        case SEPROXYHAL_TAG_M24SR_GPO_CHANGE_EVENT:
          // ensure ready to process a NFC APDU (or accept GPO change during a NFC session)
          if (G_io_apdu_state < APDU_NFC_M24SR && G_io_apdu_state != APDU_IDLE) {
            LOG("invalid state for NFC over M24SR\n");
            //THROW(INVALID_STATE);
            goto invalid_apdu_packet;
          }

          // only process when the GPO notifies of a valid NDEF length being available
          if (G_io_apdu_state == APDU_IDLE && G_io_seproxyhal_spi_buffer[3] == 1) {
            // issue an APDU to read the first block content (containing the total apdu length) (read as much as possible)
            G_io_seproxyhal_spi_buffer[0] = SEPROXYHAL_TAG_M24SR_C_APDU;
            G_io_seproxyhal_spi_buffer[1] = 0;
            G_io_seproxyhal_spi_buffer[2] = 7;
            // Forge NDEF Select command
            G_io_seproxyhal_spi_buffer[3] = 0x00;
            G_io_seproxyhal_spi_buffer[4] = 0xA4;
            G_io_seproxyhal_spi_buffer[5] = 0x00;
            G_io_seproxyhal_spi_buffer[6] = 0x0C;
            G_io_seproxyhal_spi_buffer[7] = 0x02;
            G_io_seproxyhal_spi_buffer[8] = 0x00;
            G_io_seproxyhal_spi_buffer[9] = 0x01;

            // wait for a NFC_RESPONSE
            G_io_apdu_state = APDU_NFC_M24SR_SELECT;
            io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, 10);
          }
          else {
            // NDEF message rewritten while being updated ?? => error ?
            LOG("unexpected GPO change\n");
            unsigned char processed = io_event(CHANNEL_SPI);
            // close the event if not done previously (by a display or whatever)
            if (!io_seproxyhal_spi_is_status_sent()) {
                io_seproxyhal_general_status();
            }

            if (!processed) {
              goto invalid_apdu_packet;
            }
          }

          break;
        case SEPROXYHAL_TAG_M24SR_RESPONSE_APDU_EVENT:
          // ensure ready to process a NFC APDU (shall have been started with GPO_CHANGE)
          switch(G_io_apdu_state) {
            case APDU_NFC_M24SR_SELECT:
              // expect 0x90 0x00
              if (G_io_seproxyhal_spi_buffer[1] != 0 || G_io_seproxyhal_spi_buffer[2] != 2 || G_io_seproxyhal_spi_buffer[3] != 0x90 || G_io_seproxyhal_spi_buffer[4] != 0x00) {
                LOG("invalid NFC SELECT reply\n");
                goto invalid_apdu_packet;
              }

              // read first part of the NDEF file, maximize throughput but to read only the total length

              // issue an APDU to read the first block content (containing the total apdu length) (read as much as possible)
              G_io_seproxyhal_spi_buffer[0] = SEPROXYHAL_TAG_M24SR_C_APDU;
              G_io_seproxyhal_spi_buffer[1] = 0;
              G_io_seproxyhal_spi_buffer[2] = 5;
              // Forge NDEF Select command
              G_io_seproxyhal_spi_buffer[3] = 0x00;
              G_io_seproxyhal_spi_buffer[4] = 0xB0;
              G_io_seproxyhal_spi_buffer[5] = 0x00;
              G_io_seproxyhal_spi_buffer[6] = 0x00;
              G_io_seproxyhal_spi_buffer[7] = M24SR_CHUNK_LENGTH;

              // wait for a NFC_RESPONSE
              G_io_apdu_state = APDU_NFC_M24SR_FIRST;
              io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, 8);
              break;
            case APDU_NFC_M24SR_FIRST:
              // ensure read has ended with a positive status
              if (G_io_seproxyhal_spi_buffer[rx_len-2] != 0x90 || G_io_seproxyhal_spi_buffer[rx_len-1] != 0x00) {
                LOG("invalid READBINARY reply\n");
                goto invalid_apdu_packet;
              }

              G_io_apdu_length = U2(G_io_seproxyhal_spi_buffer[3], G_io_seproxyhal_spi_buffer[4]);
              rx_len = MIN(G_io_apdu_length, rx_len -3 /*TagLen*/ -2 /*SW*/ -2 /*total apdu length (NDEF length)*/);

              // copy the first part of the apdu
              os_memmove(G_io_apdu_buffer, G_io_seproxyhal_spi_buffer+3 /*TagLen*/ +2 /*NDEF length*/, rx_len);
              G_io_apdu_offset = rx_len;

              G_io_apdu_state = APDU_NFC_M24SR;
              goto m24sr_continue_read_apdu;

            case APDU_NFC_M24SR:
              // ensure read has ended with a positive status
              if (G_io_seproxyhal_spi_buffer[rx_len-2] != 0x90 || G_io_seproxyhal_spi_buffer[rx_len-1] != 0x00) {
                LOG("invalid READBINARY reply (2)\n");
                goto invalid_apdu_packet;
              }

              rx_len = MIN(G_io_apdu_length-G_io_apdu_offset, rx_len -3 /*TagLen*/ -2 /*SW*/);

              // copy the first part of the apdu
              os_memmove(G_io_apdu_buffer+G_io_apdu_offset, G_io_seproxyhal_spi_buffer+3 /*TagLen*/, rx_len);
              G_io_apdu_offset += rx_len;

            m24sr_continue_read_apdu:
              // check if need more data to complete the CAPDU content
              if (G_io_apdu_length == G_io_apdu_offset) {
                // acknowledge the M24SR RAPDU EVENT (general status OK) and more command to follow (processing of the apdu)
                G_io_apdu_media = IO_APDU_MEDIA_NFC;
                return G_io_apdu_length;
              }
              else {
                // fetch more data from the M24SR memory
                G_io_seproxyhal_spi_buffer[0] = SEPROXYHAL_TAG_M24SR_C_APDU;
                G_io_seproxyhal_spi_buffer[1] = 0;
                G_io_seproxyhal_spi_buffer[2] = 5;
                // Forge NDEF Select command
                G_io_seproxyhal_spi_buffer[3] = 0x00;
                G_io_seproxyhal_spi_buffer[4] = 0xB0;
                G_io_seproxyhal_spi_buffer[5] = (G_io_apdu_offset +2 /*NDEF length*/)>>8;
                G_io_seproxyhal_spi_buffer[6] = (G_io_apdu_offset +2 /*NDEF length*/);
                G_io_seproxyhal_spi_buffer[7] = MIN(M24SR_CHUNK_LENGTH,G_io_apdu_length-G_io_apdu_offset);
                io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, 8);
              }
              break;
            default:
              LOG("invalid state for M24SR RAPDU EVENT\n");
              goto invalid_apdu_packet;
          }
          break;
#endif // HAVE_M24SR_NFC
        default:
          // tell the application that a non-apdu packet has been received
          io_event(CHANNEL_SPI);
          if (!io_seproxyhal_spi_is_status_sent()) {
            io_seproxyhal_general_status();
          }
          break;

      }
    }
    break;

  default:
    return io_exchange_al(channel, tx_len);
  }
}

unsigned int os_ux_blocking(bolos_ux_params_t* params) {
  unsigned int ret;
  // until a real status is returned
  ret = os_ux(params);
  while(ret == BOLOS_UX_IGNORE 
     || ret == BOLOS_UX_CONTINUE) {

    // send general status before receiving next event
    if (!io_seproxyhal_spi_is_status_sent()) {
      io_seproxyhal_general_status();
    }

    // process events
    for (;;) {
      /*unsigned int rx_len = */io_seproxyhal_spi_recv(G_io_seproxyhal_spi_buffer, sizeof(G_io_seproxyhal_spi_buffer), 0);

      switch (G_io_seproxyhal_spi_buffer[0]) {
        case SEPROXYHAL_TAG_FINGER_EVENT:
        case SEPROXYHAL_TAG_BUTTON_PUSH_EVENT:
        case SEPROXYHAL_TAG_TICKER_EVENT:
        case SEPROXYHAL_TAG_DISPLAY_PROCESSED_EVENT:
        case SEPROXYHAL_TAG_STATUS_EVENT:
          // perform UX event on these ones, don't process as an IO event
          break;

        default:
          // if malformed, then a stall is likely to occur
          if (io_seproxyhal_handle_event()) {
            // avoid twice if error
            if (!io_seproxyhal_spi_is_status_sent()) {
              io_seproxyhal_general_status();
            }
            continue;
          }
          break;
      }

      // pass the packet to the ux
      break;
    }
    // prepare processing of the packet by the ux
    params->ux_id = BOLOS_UX_EVENT;
    params->len = 0;
    ret = os_ux(params);
  }

  return ret;
} 

// so unoptimized
void screen_printc(unsigned char c) {
  unsigned char buf[4];
  buf[0] = SEPROXYHAL_TAG_PRINTF_STATUS;
  buf[1] = 0;
  buf[2] = 1;
  buf[3] = c;
  io_seproxyhal_spi_send(buf, 4);
#ifndef IO_SEPROXYHAL_DEBUG
  // wait printf ack (no race kthx)
  io_seproxyhal_spi_recv(buf, 3, 0);
  buf[0] = 0; // consume tag to avoid misinterpretation (due to IO_CACHE)
#endif // IO_SEPROXYHAL_DEBUG
}

#endif // OS_IO_SEPROXYHAL
