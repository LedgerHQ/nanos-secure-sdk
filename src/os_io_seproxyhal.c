/*******************************************************************************
*   Ledger Nano S - Secure firmware
*   (c) 2016, 2017, 2018 Ledger
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

#ifdef HAVE_BOLOS_UX
#include "bolos_ux.h"
#endif

#ifdef HAVE_IO_U2F
#include "u2f_processing.h"
#include "u2f_transport.h"
#endif



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
volatile unsigned int G_io_timeout;
volatile unsigned int G_io_timeout_limit;

// usb endpoint buffer
unsigned char G_io_usb_ep_buffer[MAX(USB_SEGMENT_SIZE, BLE_SEGMENT_SIZE)];

#ifndef IO_RAPDU_TRANSMIT_TIMEOUT_MS 
#define IO_RAPDU_TRANSMIT_TIMEOUT_MS 2000UL
#endif // IO_RAPDU_TRANSMIT_TIMEOUT_MS

void io_set_timeout(unsigned int timeout_ms) {
  // reset the next timeout
  G_io_timeout_limit = timeout_ms;
  // adjust current timeout, if activated
  if (G_io_timeout) {
    G_io_timeout = timeout_ms;
  }
}

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

void io_seproxyhal_request_mcu_status(void) {
  // send the general status
  G_io_seproxyhal_spi_buffer[0] = SEPROXYHAL_TAG_REQUEST_STATUS;
  G_io_seproxyhal_spi_buffer[1] = 0;
  G_io_seproxyhal_spi_buffer[2] = 0;
  io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, 3);
}

#ifdef HAVE_IO_USB
#ifdef HAVE_L4_USBLIB
static volatile unsigned char G_io_usb_ep_xfer_len[IO_USB_MAX_ENDPOINTS];
#include "usbd_def.h"
#include "usbd_core.h"
extern USBD_HandleTypeDef USBD_Device;

void io_seproxyhal_handle_usb_event(void) {
  switch(G_io_seproxyhal_spi_buffer[3]) {
    case SEPROXYHAL_TAG_USB_EVENT_RESET:
      USBD_LL_SetSpeed(&USBD_Device, USBD_SPEED_FULL);  
      USBD_LL_Reset(&USBD_Device);
      // ongoing APDU detected, throw a reset, even if not the media. to avoid potential troubles.
      if (G_io_apdu_media != IO_APDU_MEDIA_NONE) {
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
    /* This event is received when a new SETUP token had been received on a control endpoint */
    case SEPROXYHAL_TAG_USB_EP_XFER_SETUP:
      // assume length of setup packet, and that it is on endpoint 0
      USBD_LL_SetupStage(&USBD_Device, &G_io_seproxyhal_spi_buffer[6]);
      break;

    /* This event is received after the prepare data packet has been flushed to the usb host */
    case SEPROXYHAL_TAG_USB_EP_XFER_IN:
      USBD_LL_DataInStage(&USBD_Device, G_io_seproxyhal_spi_buffer[3]&0x7F, &G_io_seproxyhal_spi_buffer[6]);
      break;

    /* This event is received when a new DATA token is received on an endpoint */
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

// TODO, refactor this using the USB DataIn event like for the U2F tunnel
void io_usb_send_ep(unsigned int ep, unsigned char* buffer, unsigned short length, unsigned int timeout) {
  unsigned int rx_len;

  // don't spoil the timeout :)
  if (timeout) {
    timeout++;
  }

  // won't send if overflowing seproxyhal buffer format
  if (length > 255) {
    return;
  }
  
  G_io_seproxyhal_spi_buffer[0] = SEPROXYHAL_TAG_USB_EP_PREPARE;
  G_io_seproxyhal_spi_buffer[1] = (3+length)>>8;
  G_io_seproxyhal_spi_buffer[2] = (3+length);
  G_io_seproxyhal_spi_buffer[3] = ep|0x80;
  G_io_seproxyhal_spi_buffer[4] = SEPROXYHAL_TAG_USB_EP_PREPARE_DIR_IN;
  G_io_seproxyhal_spi_buffer[5] = length;
  io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, 6);
  io_seproxyhal_spi_send(buffer, length);

  // if timeout is requested
  if(timeout) {
    for (;;) {
      if (!io_seproxyhal_spi_is_status_sent()) {
        io_seproxyhal_general_status();
      }

      rx_len = io_seproxyhal_spi_recv(G_io_seproxyhal_spi_buffer, sizeof(G_io_seproxyhal_spi_buffer), 0);

      // wait for ack of the seproxyhal
      // discard if not an acknowledgment
      if (G_io_seproxyhal_spi_buffer[0] != SEPROXYHAL_TAG_USB_EP_XFER_EVENT
        || rx_len != 6 
        || G_io_seproxyhal_spi_buffer[3] != (ep|0x80)
        || G_io_seproxyhal_spi_buffer[4] != SEPROXYHAL_TAG_USB_EP_XFER_IN
        || G_io_seproxyhal_spi_buffer[5] != length) {
        
        /*
        // handle loss of communication with the host
        // only use ticker event to decrement the timeout, this avoid finger/button/display events to intervene and disrupt the timeout accounting
        if (G_io_seproxyhal_spi_buffer[0] == SEPROXYHAL_TAG_TICKER_EVENT 
          && timeout 
          && timeout--==1) {
          THROW(EXCEPTION_IO_RESET);
        }
        */

        // link disconnected ?
        if(G_io_seproxyhal_spi_buffer[0] == SEPROXYHAL_TAG_STATUS_EVENT) {
          if (!(U4BE(G_io_seproxyhal_spi_buffer, 3) & SEPROXYHAL_TAG_STATUS_EVENT_FLAG_USB_POWERED)) {
           THROW(EXCEPTION_IO_RESET);
          }
        }
        
        // usb reset ?
        //io_seproxyhal_handle_usb_event();
        // also process other transfer requests if any (useful for HID keyboard while playing with CAPS lock key, side effect on LED status)
        // also handle IO timeout in a centralized and configurable way
        io_seproxyhal_handle_event();

        // no general status ack, io_event is responsible for it
        continue;
      }

      // chunk sending succeeded
      break;
    }
  }
}

void io_usb_send_apdu_data(unsigned char* buffer, unsigned short length) {
  // wait for 20 events before hanging up and timeout (~2 seconds of timeout)
  io_usb_send_ep(0x82, buffer, length, 20);
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


void io_seproxyhal_handle_capdu_event(void) {
  if(G_io_apdu_state == APDU_IDLE) 
  {
    G_io_apdu_media = IO_APDU_MEDIA_RAW; // for application code
    G_io_apdu_state = APDU_RAW; // for next call to io_exchange
    G_io_apdu_length = U2BE(G_io_seproxyhal_spi_buffer, 1);
    // copy apdu to apdu buffer
    os_memmove(G_io_apdu_buffer, G_io_seproxyhal_spi_buffer+3, G_io_apdu_length);
  }
}

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

    case SEPROXYHAL_TAG_CAPDU_EVENT:
      io_seproxyhal_handle_capdu_event();
      return 1;

      // ask the user if not processed here
    case SEPROXYHAL_TAG_TICKER_EVENT:
      // process ticker events to timeout the IO transfers, and forward to the user io_event function too
      if(G_io_timeout) {
        G_io_timeout-=MIN(G_io_timeout, 100);
        #warning TODO use real ticker event interval here instead of the x100ms multiplier
        if (!G_io_timeout) {
          // timeout !
          G_io_apdu_state = APDU_IDLE;
          THROW(EXCEPTION_IO_RESET);
        }
      }
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
  5, 0xE0, 0x40, 0x00, 0x00, 0x00,
  //9, 0xe0, 0x22, 0x00, 0x00, 0x04, 0x31, 0x32, 0x33, 0x34,
};
#endif // DEBUG_APDU

#ifdef HAVE_BOLOS_APP_STACK_CANARY
#define APP_STACK_CANARY_MAGIC 0xDEAD0031
extern unsigned int app_stack_canary;
#endif // HAVE_BOLOS_APP_STACK_CANARY

void io_seproxyhal_init(void) {
  // Enforce OS compatibility
  check_api_level(CX_COMPAT_APILEVEL);

#ifdef HAVE_BOLOS_APP_STACK_CANARY
  app_stack_canary = APP_STACK_CANARY_MAGIC;
#endif // HAVE_BOLOS_APP_STACK_CANARY  

  G_io_apdu_state = APDU_IDLE;
  G_io_apdu_offset = 0;
  G_io_apdu_length = 0;
  G_io_apdu_seq = 0;
  G_io_apdu_media = IO_APDU_MEDIA_NONE;
  G_io_timeout_limit = NO_TIMEOUT;

  #ifdef DEBUG_APDU
  debug_apdus_offset = 0;
  #endif // DEBUG_APDU


  #ifdef HAVE_USB_APDU
  io_usb_hid_init();
  #endif // HAVE_USB_APDU

  io_seproxyhal_init_ux();
  io_seproxyhal_init_button();
}

void io_seproxyhal_init_ux(void) {
  // initialize the touch part
  G_bagl_last_touched_not_released_component = NULL;

}

void io_seproxyhal_init_button(void) {
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

void io_seproxyhal_display_bitmap(int x, int y, unsigned int w, unsigned int h, unsigned int* color_index, unsigned int bit_per_pixel, unsigned char* bitmap) {
  // component type = ICON
  // component icon id = 0
  // => bitmap transmitted
  bagl_component_t c;
  bagl_icon_details_t d;
  os_memset(&c, 0, sizeof(c));
  c.type = BAGL_ICON;
  c.x = x;
  c.y = y;
  c.width = w;
  c.height = h;
  // done by memset // c.icon_id = 0;
  d.width = w;
  d.height = h;
  d.bpp = bit_per_pixel;
  d.colors = color_index;
  d.bitmap = bitmap;

  io_seproxyhal_display_icon(&c, &d);
  /*
  // color index size
  h = ((1<<bit_per_pixel)*sizeof(unsigned int)); 
  // bitmap size
  w = ((w*c.height*bit_per_pixel)/8)+((w*c.height*bit_per_pixel)%8?1:0);
  unsigned short length = sizeof(bagl_component_t)
                          +1 // bpp 
                          +h // color index
                          +w; // image bitmap
  G_io_seproxyhal_spi_buffer[0] = SEPROXYHAL_TAG_SCREEN_DISPLAY_STATUS;
  G_io_seproxyhal_spi_buffer[1] = length>>8;
  G_io_seproxyhal_spi_buffer[2] = length;
  io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, 3);
  io_seproxyhal_spi_send((unsigned char*)&c, sizeof(bagl_component_t));
  G_io_seproxyhal_spi_buffer[0] = bit_per_pixel;
  io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, 1);
  io_seproxyhal_spi_send((unsigned char*)color_index, h);
  io_seproxyhal_spi_send(bitmap, w);
  */
}

#ifdef SEPROXYHAL_TAG_SCREEN_DISPLAY_RAW_STATUS
unsigned int io_seproxyhal_display_icon_header_and_colors(bagl_component_t* icon_component, bagl_icon_details_t* icon_details, unsigned int* icon_len) {
  unsigned int len;

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


  *icon_len = raw.w.val*raw.h.val*raw.bpp/8 + (((raw.w.val*raw.h.val*raw.bpp)%8)?1:0);

  // optional, don't send too much on a single packet for MCU to receive it. when stream mode will be on, this will be useless
  // min of remaining space in the packet vs. total icon size + color index size
  len = MIN(sizeof(G_io_seproxyhal_spi_buffer) - sizeof(raw), *icon_len + (1<<raw.bpp)*4);

  // sizeof packet
  raw.header.seph.len[0] = (len + sizeof(raw) - sizeof(raw.header.seph)) >> 8;
  raw.header.seph.len[1] = (len + sizeof(raw) - sizeof(raw.header.seph));

  // swap endianess of coordinates (make it big endian)
  SWAP(raw.x.b[0], raw.x.b[1]);
  SWAP(raw.y.b[0], raw.y.b[1]);
  SWAP(raw.w.b[0], raw.w.b[1]);
  SWAP(raw.h.b[0], raw.h.b[1]);

  io_seproxyhal_spi_send(&raw, sizeof(raw));
  io_seproxyhal_spi_send(PIC(icon_details->colors), (1<<raw.bpp)*4);
  len -= (1<<raw.bpp)*4;  

  // remaining length of bitmap bits to be displayed
  return len;
}
#endif // SEPROXYHAL_TAG_SCREEN_DISPLAY_RAW_STATUS

void io_seproxyhal_display_icon(bagl_component_t* icon_component, bagl_icon_details_t* icon_details) {
  bagl_component_t icon_component_mod;
  // ensure not being out of bounds in the icon component agianst the declared icon real size
  os_memmove(&icon_component_mod, icon_component, sizeof(bagl_component_t));
  icon_component_mod.width = icon_details->width;
  icon_component_mod.height = icon_details->height;
  icon_component = &icon_component_mod;

#ifdef SEPROXYHAL_TAG_SCREEN_DISPLAY_RAW_STATUS
  unsigned int len;
  unsigned int icon_len;
  unsigned int icon_off=0;

  len = io_seproxyhal_display_icon_header_and_colors(icon_component, icon_details, &icon_len);
  io_seproxyhal_spi_send(PIC(icon_details->bitmap), len);
  // advance in the bitmap to be transmitted
  icon_len -= len;
  icon_off += len;

  // still some bitmap data to transmit
  while(icon_len) {
    // wait displayed event
    io_seproxyhal_spi_recv(G_io_seproxyhal_spi_buffer, sizeof(G_io_seproxyhal_spi_buffer), 0);
    
    G_io_seproxyhal_spi_buffer[0] = SEPROXYHAL_TAG_SCREEN_DISPLAY_RAW_STATUS;
    G_io_seproxyhal_spi_buffer[3] = SEPROXYHAL_TAG_SCREEN_DISPLAY_RAW_STATUS_CONT;

    len = MIN((sizeof(G_io_seproxyhal_spi_buffer) - 4), icon_len);
    G_io_seproxyhal_spi_buffer[1] = (len+1)>>8;
    G_io_seproxyhal_spi_buffer[2] = (len+1);
    io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, 4);
    io_seproxyhal_spi_send(PIC(icon_details->bitmap)+icon_off, len);

    icon_len -= len;
    icon_off += len;
  }
#else // !SEPROXYHAL_TAG_SCREEN_DISPLAY_RAW_STATUS // for nano s
  // component type = ICON, provided bitmap
  // => bitmap transmitted


  // color index size
  unsigned int h = (1<<(icon_details->bpp))*sizeof(unsigned int); 
  // bitmap size
  unsigned int w = ((icon_component->width*icon_component->height*icon_details->bpp)/8)+((icon_component->width*icon_component->height*icon_details->bpp)%8?1:0);
  unsigned short length = sizeof(bagl_component_t)
                          +1 /* bpp */
                          +h /* color index */
                          +w; /* image bitmap size */
  G_io_seproxyhal_spi_buffer[0] = SEPROXYHAL_TAG_SCREEN_DISPLAY_STATUS;
  G_io_seproxyhal_spi_buffer[1] = length>>8;
  G_io_seproxyhal_spi_buffer[2] = length;
  io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, 3);
  io_seproxyhal_spi_send((unsigned char*)icon_component, sizeof(bagl_component_t));
  G_io_seproxyhal_spi_buffer[0] = icon_details->bpp;
  io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, 1);
  io_seproxyhal_spi_send((unsigned char*)PIC(icon_details->colors), h);
  io_seproxyhal_spi_send((unsigned char*)PIC(icon_details->bitmap), w);
#endif // !SEPROXYHAL_TAG_SCREEN_DISPLAY_RAW_STATUS
}

void io_seproxyhal_display_default(const bagl_element_t * element) {
  // process automagically address from rom and from ram
  unsigned int type = (element->component.type & ~(BAGL_FLAG_TOUCHABLE));

  // avoid sending another status :), fixes a lot of bugs in the end
  if (io_seproxyhal_spi_is_status_sent()) {
    return;
  }

  if (type != BAGL_NONE) {
    if (element->text != NULL) {
      unsigned int text_adr = PIC((unsigned int)element->text);
      // consider an icon details descriptor is pointed by the context
      if (type == BAGL_ICON && element->component.icon_id == 0) {
        io_seproxyhal_display_icon(&element->component, (bagl_icon_details_t*)text_adr);
      }
      else {
        unsigned short length = sizeof(bagl_component_t)+strlen((const char*)text_adr);
        G_io_seproxyhal_spi_buffer[0] = SEPROXYHAL_TAG_SCREEN_DISPLAY_STATUS;
        G_io_seproxyhal_spi_buffer[1] = length>>8;
        G_io_seproxyhal_spi_buffer[2] = length;
        io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, 3);
        io_seproxyhal_spi_send((unsigned char*)&element->component, sizeof(bagl_component_t));
        io_seproxyhal_spi_send((unsigned char*)text_adr, length-sizeof(bagl_component_t));
      }
    }
    else {
      unsigned short length = sizeof(bagl_component_t);
      G_io_seproxyhal_spi_buffer[0] = SEPROXYHAL_TAG_SCREEN_DISPLAY_STATUS;
      G_io_seproxyhal_spi_buffer[1] = length>>8;
      G_io_seproxyhal_spi_buffer[2] = length;
      io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, 3);
      io_seproxyhal_spi_send((unsigned char*)&element->component, sizeof(bagl_component_t));
    }
  }
}

unsigned int bagl_label_roundtrip_duration_ms(const bagl_element_t* e, unsigned int average_char_width) {
  return bagl_label_roundtrip_duration_ms_buf(e, e->text, average_char_width);
}

unsigned int bagl_label_roundtrip_duration_ms_buf(const bagl_element_t* e, const char* str, unsigned int average_char_width) {
  // not a scrollable label
  if (e == NULL || (e->component.type != BAGL_LABEL && e->component.type != BAGL_LABELINE)) {
    return 0;
  }
  
  unsigned int text_adr = PIC((unsigned int)str);
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
  for(;;);
}

void io_seproxyhal_se_reset(void) {
  G_io_seproxyhal_spi_buffer[0] = SEPROXYHAL_TAG_SE_POWER_OFF;
  G_io_seproxyhal_spi_buffer[1] = 0;
  G_io_seproxyhal_spi_buffer[2] = 0;
  io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, 3);
  for(;;);
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
      // each 100ms ~
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

    if (button_same_mask_counter >= BUTTON_FAST_THRESHOLD_CS) {
      // fast bit when pressing and timing is right
      if ((button_same_mask_counter%BUTTON_FAST_ACTION_CS) == 0) {
        button_mask |= BUTTON_EVT_FAST;
      }

      /*
      // fast bit when releasing and threshold has been exceeded
      if ((button_mask & BUTTON_EVT_RELEASED)) {
        button_mask |= BUTTON_EVT_FAST;
      }
      */

      // discard the release event after a fastskip has been detected, to avoid strange at release behavior
      // and also to enable user to cancel an operation by starting triggering the fast skip
      button_mask &= ~BUTTON_EVT_RELEASED;
    }

    // indicate if button have been released
    button_callback(button_mask, button_same_mask_counter);
  }
}

#endif // HAVE_BAGL

#ifdef HAVE_IO_U2F
u2f_service_t G_io_u2f;
#endif // HAVE_IO_U2F

unsigned short io_exchange(unsigned char channel, unsigned short tx_len) {
  unsigned short rx_len;

#ifdef HAVE_BOLOS_APP_STACK_CANARY
  // behavior upon detected stack overflow is to reset the SE
  if (app_stack_canary != APP_STACK_CANARY_MAGIC) {
    io_seproxyhal_se_reset();
  }
#endif // HAVE_BOLOS_APP_STACK_CANARY

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
      // prepare response timeout
      G_io_timeout = IO_RAPDU_TRANSMIT_TIMEOUT_MS;

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

          case APDU_RAW:
            if (tx_len > sizeof(G_io_apdu_buffer)) {
              THROW(INVALID_PARAMETER);
            }
            // reply the RAW APDU over SEPROXYHAL protocol
            G_io_seproxyhal_spi_buffer[0]  = SEPROXYHAL_TAG_RAPDU;
            G_io_seproxyhal_spi_buffer[1]  = (tx_len)>>8;
            G_io_seproxyhal_spi_buffer[2]  = (tx_len);
            io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, 3);
            io_seproxyhal_spi_send(G_io_apdu_buffer, tx_len);
            // finished, no chunking
            goto break_send;

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

#ifdef HAVE_BLE_APDU // versus U2F BLE
          case APDU_BLE:
            BLE_protocol_send(G_io_apdu_buffer, tx_len);
            goto break_send;
#endif // HAVE_BLE_APDU


#ifdef HAVE_IO_U2F
          // case to handle U2F channels. u2f apdu to be dispatched in the upper layers
          case APDU_U2F:
            // prepare reply, the remaining segments will be pumped during USB/BLE events handling while waiting for the next APDU

            // user presence + counter + rapdu + sw must fit the apdu buffer
            if (1+ 4+ tx_len +2 > sizeof(G_io_apdu_buffer)) {
              THROW(INVALID_PARAMETER);
            }

            // u2F tunnel needs the status words to be included in the signature response BLOB, do it now.
            // always return 9000 in the signature to avoid error @ transport level in u2f layers. 
            G_io_apdu_buffer[tx_len] = 0x90; //G_io_apdu_buffer[tx_len-2];
            G_io_apdu_buffer[tx_len+1] = 0x00; //G_io_apdu_buffer[tx_len-1];
            tx_len += 2;
            os_memmove(G_io_apdu_buffer+5, G_io_apdu_buffer, tx_len);
            // zeroize user presence and counter
            os_memset(G_io_apdu_buffer, 0, 5);
            u2f_message_reply(&G_io_u2f, U2F_CMD_MSG, G_io_apdu_buffer, tx_len+5);
            goto break_send;
#endif // HAVE_IO_U2F
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

    // until a new whole CAPDU is received
    for (;;) {
      if (!io_seproxyhal_spi_is_status_sent()) {
        io_seproxyhal_general_status();
      }

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
        continue;
      }

      // if an apdu is already ongoing, then discard packet as a new packet
      if (G_io_apdu_media != IO_APDU_MEDIA_NONE) {
        io_seproxyhal_handle_event();
        continue;
      }

      // depending on received TAG
      switch(G_io_seproxyhal_spi_buffer[0]) {

        case SEPROXYHAL_TAG_CAPDU_EVENT:
          io_seproxyhal_handle_capdu_event();
          if (G_io_apdu_length > 0) {
            G_io_timeout = G_io_timeout_limit;
            return G_io_apdu_length;
          }
          // discard gently, and wait for another event. but this is VERY ODD
          goto send_last_command;
          break;

#ifdef HAVE_BLE
        case SEPROXYHAL_TAG_BLUENRG_RECV_EVENT:
          // process the packet
          io_seproxyhal_handle_bluenrg_event();

          // if the ble apdu state has advanced
          if (G_io_apdu_length) {
            G_io_timeout = G_io_timeout_limit;
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
          // exit flow for apdu_ccid/apdu_u2f/apdu_hid
          if (G_io_apdu_length > 0) {
            G_io_timeout = G_io_timeout_limit;
            // invalid return when reentered and an apdu is already under processing
            return G_io_apdu_length;
          }
          else {
            goto send_last_command;
          }
          break;
#endif // HAVE_IO_USB

        default:
          // tell the application that a non-apdu packet has been received
          io_event(CHANNEL_SPI);
          continue;

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

    // process events
    for (;;) {
      // send general status before receiving next event
      if (!io_seproxyhal_spi_is_status_sent()) {
        io_seproxyhal_general_status();
      }

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

// current ux_menu context (could be pluralised if multiple nested levels of menu are required within bolos_ux for example)
ux_menu_state_t ux_menu;

const bagl_element_t ux_menu_elements[] = {
  // erase
  {{BAGL_RECTANGLE                      , 0x80,   0,   0, 128,  32, 0, 0, BAGL_FILL, 0x000000, 0xFFFFFF, 0, 0}, NULL, 0, 0, 0, NULL, NULL, NULL},

  // icons
  {{BAGL_ICON                           , 0x81,   3,  14,   7,   4, 0, 0, 0        , 0xFFFFFF, 0x000000, 0, BAGL_GLYPH_ICON_UP   }, NULL, 0, 0, 0, NULL, NULL, NULL },
  {{BAGL_ICON                           , 0x82, 118,  14,   7,   4, 0, 0, 0        , 0xFFFFFF, 0x000000, 0, BAGL_GLYPH_ICON_DOWN }, NULL, 0, 0, 0, NULL, NULL, NULL },
  

  // previous setting name
  {{BAGL_LABELINE                       , 0x41,  14,   3, 100,  12, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_REGULAR_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, NULL, 0, 0, 0, NULL, NULL, NULL },
  // next setting name
  {{BAGL_LABELINE                       , 0x42,  14,  35, 100,  12, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_REGULAR_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, NULL, 0, 0, 0, NULL, NULL, NULL },

  // current setting (x to be adjusted if icon is present etc)
  {{BAGL_ICON                           , 0x10,  14,   9,   0,   0, 0, 0, 0        , 0xFFFFFF, 0x000000, 0, 0  }, NULL, 0, 0, 0, NULL, NULL, NULL },
  // single line layout
  {{BAGL_LABELINE                       , 0x20,  14,  19, 100,  12, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_EXTRABOLD_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, NULL, 0, 0, 0, NULL, NULL, NULL },

  // 2 lines layout + icon
  {{BAGL_LABELINE                       , 0x21,  14,  12, 100,  12, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_EXTRABOLD_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, NULL, 0, 0, 0, NULL, NULL, NULL },
  {{BAGL_LABELINE                       , 0x22,  14,  26, 100,  12, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_EXTRABOLD_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, NULL, 0, 0, 0, NULL, NULL, NULL },

};

const ux_menu_entry_t* ux_menu_get_entry (unsigned int entry_idx) {
  if (ux_menu.menu_iterator) {
    return ux_menu.menu_iterator(entry_idx);
  } 
  return &ux_menu.menu_entries[entry_idx];
} 

const bagl_element_t* ux_menu_element_preprocessor(const bagl_element_t* element) {
  //todo avoid center alignment when text_x or icon_x AND text_x are not 0
  os_memmove(&ux_menu.tmp_element, element, sizeof(bagl_element_t));

  // ask the current entry first, to setup other entries
  const ux_menu_entry_t* current_entry = ux_menu_get_entry(ux_menu.current_entry);

  const ux_menu_entry_t* previous_entry = NULL;
  if (ux_menu.current_entry) {
    previous_entry = ux_menu_get_entry(ux_menu.current_entry-1);
  }
  const ux_menu_entry_t* next_entry = NULL;
  if (ux_menu.current_entry < ux_menu.menu_entries_count-1) {
    next_entry = ux_menu_get_entry(ux_menu.current_entry+1);
  }

  switch(element->component.userid) {
    case 0x81:
      if (ux_menu.current_entry == 0) {
        return NULL;
      }
      break;
    case 0x82:
      if (ux_menu.current_entry == ux_menu.menu_entries_count-1) {
        return NULL;
      }
      break;
    // previous setting name
    case 0x41:
      if (current_entry->line2 != NULL 
        || current_entry->icon != NULL
        || ux_menu.current_entry == 0
        || ux_menu.menu_entries_count == 1 
        || previous_entry->icon != NULL
        || previous_entry->line2 != NULL) {
        return 0;
      }
      ux_menu.tmp_element.text = previous_entry->line1;
      break;
    // next setting name
    case 0x42:
      if (current_entry->line2 != NULL 
        || current_entry->icon != NULL
        || ux_menu.current_entry == ux_menu.menu_entries_count-1
        || ux_menu.menu_entries_count == 1
        || next_entry->icon != NULL) {
        return NULL;
      }
      ux_menu.tmp_element.text = next_entry->line1;
      break;
    case 0x10:
      if (current_entry->icon == NULL) {
        return NULL;
      }
      ux_menu.tmp_element.text = (const char*)current_entry->icon;
      if (current_entry->icon_x) {
        ux_menu.tmp_element.component.x = current_entry->icon_x;
      }
      break;
    case 0x20:
      if (current_entry->line2 != NULL) {
        return NULL;
      }
      ux_menu.tmp_element.text = current_entry->line1;
      goto adjust_text_x;
    case 0x21:
      if (current_entry->line2 == NULL) {
        return NULL;
      }
      ux_menu.tmp_element.text = current_entry->line1;
      goto adjust_text_x;
    case 0x22:
      if (current_entry->line2 == NULL) {
        return NULL;
      }
      ux_menu.tmp_element.text = current_entry->line2;
    adjust_text_x:
      if (current_entry->text_x) {
        ux_menu.tmp_element.component.x = current_entry->text_x;
        // discard the 'center' flag
        ux_menu.tmp_element.component.font_id = BAGL_FONT_OPEN_SANS_EXTRABOLD_11px;
      }
      break;
  }
  // ensure prepro agrees to the element to be displayed
  if (ux_menu.menu_entry_preprocessor) {
    // menu is denied by the menu entry preprocessor
    return ux_menu.menu_entry_preprocessor(current_entry, &ux_menu.tmp_element);
  }

  return &ux_menu.tmp_element;
}

unsigned int ux_menu_elements_button (unsigned int button_mask, unsigned int button_mask_counter) {
  UNUSED(button_mask_counter);

  const ux_menu_entry_t* current_entry = ux_menu_get_entry(ux_menu.current_entry);

  switch (button_mask) {
    // enter menu or exit menu
    case BUTTON_EVT_RELEASED|BUTTON_LEFT|BUTTON_RIGHT:
      // menu is priority 1
      if (current_entry->menu) {
        // use userid as the pointer to current entry in the parent menu
        UX_MENU_DISPLAY(current_entry->userid, (const ux_menu_entry_t*)PIC(current_entry->menu), ux_menu.menu_entry_preprocessor);
        return 0;
      }
      // else callback
      else if (current_entry->callback) {
        ((ux_menu_callback_t)PIC(current_entry->callback))(current_entry->userid);
        return 0;
      }
      break;

    case BUTTON_EVT_FAST|BUTTON_LEFT:
    case BUTTON_EVT_RELEASED|BUTTON_LEFT:
      // entry 0 is the number of entries in the menu list
      if (ux_menu.current_entry == 0) {
        return 0;
      }
      ux_menu.current_entry--;
      goto redraw;

    case BUTTON_EVT_FAST|BUTTON_RIGHT:
    case BUTTON_EVT_RELEASED|BUTTON_RIGHT:
      // entry 0 is the number of entries in the menu list
      if (ux_menu.current_entry >= ux_menu.menu_entries_count-1) {
        return 0;
      }
      ux_menu.current_entry++;
    redraw:
#ifdef HAVE_BOLOS_UX
      screen_display_init(0);
#else
      UX_REDISPLAY();
#endif
      return 0;
  }
  return 1;
}

const ux_menu_entry_t UX_MENU_END_ENTRY = UX_MENU_END;

void ux_menu_display(unsigned int current_entry, 
                     const ux_menu_entry_t* menu_entries,
                     ux_menu_preprocessor_t menu_entry_preprocessor) {
  // reset to first entry
  ux_menu.menu_entries_count = 0;

  // count entries
  if (menu_entries) {
    for(;;) {
      if (os_memcmp(&menu_entries[ux_menu.menu_entries_count], &UX_MENU_END_ENTRY, sizeof(ux_menu_entry_t)) == 0) {
        break;
      }
      ux_menu.menu_entries_count++;
    }
  }

  if (current_entry != UX_MENU_UNCHANGED_ENTRY) {
    ux_menu.current_entry = current_entry;
    if (ux_menu.current_entry > ux_menu.menu_entries_count) {
      ux_menu.current_entry = 0;
    }
  }
  ux_menu.menu_entries = menu_entries;
  ux_menu.menu_entry_preprocessor = menu_entry_preprocessor;
  ux_menu.menu_iterator = NULL;

#ifdef HAVE_BOLOS_UX
  screen_state_init(0);

  // static dashboard content
  G_bolos_ux_context.screen_stack[0].element_arrays[0].element_array = ux_menu_elements;
  G_bolos_ux_context.screen_stack[0].element_arrays[0].element_array_count = ARRAYLEN(ux_menu_elements);
  G_bolos_ux_context.screen_stack[0].element_arrays_count = 1;

  // ensure the string_buffer will be set before each button is displayed
  G_bolos_ux_context.screen_stack[0].screen_before_element_display_callback = ux_menu_element_preprocessor;
  G_bolos_ux_context.screen_stack[0].button_push_callback = ux_menu_elements_button;

  screen_display_init(0);
#else
  // display the menu current entry
  UX_DISPLAY(ux_menu_elements, ux_menu_element_preprocessor);
#endif
}




ux_turner_state_t ux_turner;

const bagl_element_t ux_turner_elements[] = {
  // erase
  {{BAGL_RECTANGLE                      , 0x00,   0,   0, 128,  32, 0, 0, BAGL_FILL, 0x000000, 0xFFFFFF, 0, 0}, NULL, 0, 0, 0, NULL, NULL, NULL},

  // icons
  {{BAGL_ICON                           , 0x00,   3,  12,   7,   7, 0, 0, 0        , 0xFFFFFF, 0x000000, 0, BAGL_GLYPH_ICON_CROSS  }, NULL, 0, 0, 0, NULL, NULL, NULL },
  {{BAGL_ICON                           , 0x00, 117,  13,   8,   6, 0, 0, 0        , 0xFFFFFF, 0x000000, 0, BAGL_GLYPH_ICON_CHECK  }, NULL, 0, 0, 0, NULL, NULL, NULL },

  // current setting (x to be adjusted if icon is present etc)
  {{BAGL_ICON                           , 0x03,   0,   9,  14,  14, 0, 0, 0        , 0xFFFFFF, 0x000000, 0, 0  }, NULL, 0, 0, 0, NULL, NULL, NULL },

  // single line layout
  {{BAGL_LABELINE                       , 0x04,   0,  19, 128,  32, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_REGULAR_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, NULL, 0, 0, 0, NULL, NULL, NULL },
  // 2 lines layout + icon
  {{BAGL_LABELINE                       , 0x05,   0,  12, 128,  32, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_REGULAR_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, NULL, 0, 0, 0, NULL, NULL, NULL },
  {{BAGL_LABELINE                       , 0x06,   0,  26, 128,  32, 0, 0, 0        , 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_REGULAR_11px|BAGL_FONT_ALIGNMENT_CENTER, 0  }, NULL, 0, 0, 0, NULL, NULL, NULL },

};

const bagl_element_t* ux_turner_element_preprocessor(const bagl_element_t* element) {
  //todo avoid center alignment when text_x or icon_x AND text_x are not 0
  os_memmove(&ux_turner.tmp_element, element, sizeof(bagl_element_t));

  switch(element->component.userid) {

    case 0x03:
      if (ux_turner.steps[ux_turner.current_step].icon == NULL) {
        return NULL;
      }
      ux_turner.tmp_element.text = (const char*)ux_turner.steps[ux_turner.current_step].icon;
      if (ux_turner.steps[ux_turner.current_step].icon_x) {
        ux_turner.tmp_element.component.x = ux_turner.steps[ux_turner.current_step].icon_x;
      }
      break;
    case 0x04:
      if (ux_turner.steps[ux_turner.current_step].line2 != NULL) {
        return NULL;
      }
      if (ux_turner.steps[ux_turner.current_step].fontid1) {
        ux_turner.tmp_element.component.font_id = ux_turner.steps[ux_turner.current_step].fontid1;
      }
      ux_turner.tmp_element.text = ux_turner.steps[ux_turner.current_step].line1;
      goto adjust_text_x;
    case 0x05:
      if (ux_turner.steps[ux_turner.current_step].line2 == NULL) {
        return NULL;
      }
      if (ux_turner.steps[ux_turner.current_step].fontid1) {
        ux_turner.tmp_element.component.font_id = ux_turner.steps[ux_turner.current_step].fontid1;
      }
      ux_turner.tmp_element.text = ux_turner.steps[ux_turner.current_step].line1;
      goto adjust_text_x;
    case 0x06:
      if (ux_turner.steps[ux_turner.current_step].line2 == NULL) {
        return NULL;
      }
      if (ux_turner.steps[ux_turner.current_step].fontid2) {
        ux_turner.tmp_element.component.font_id = ux_turner.steps[ux_turner.current_step].fontid2;
      }
      ux_turner.tmp_element.text = ux_turner.steps[ux_turner.current_step].line2;
    adjust_text_x:
      if (ux_turner.steps[ux_turner.current_step].text_x) {
        ux_turner.tmp_element.component.x = ux_turner.steps[ux_turner.current_step].text_x;
      }
      break;
  }
  return &ux_turner.tmp_element;
}

unsigned int ux_turner_elements_button (unsigned int button_mask, unsigned int button_mask_counter) {
  return ux_turner.button_callback(button_mask, button_mask_counter);
}

#ifdef HAVE_BOLOS_UX
unsigned int ux_turner_ticker_bolos_ux(unsigned int ignored) {
  UNUSED(ignored);
  // switch to next step
  ux_turner.current_step=(ux_turner.current_step+1)%ux_turner.steps_count;
  // setup the next change
  G_bolos_ux_context.screen_stack[0].ticker_value = ux_turner.steps[ux_turner.current_step].next_step_ms;
  G_bolos_ux_context.screen_stack[0].ticker_interval = ux_turner.steps[ux_turner.current_step].next_step_ms;
  screen_display_init(0);
  return 0;
}
#else
void ux_turner_ticker(unsigned int elapsed_ms) {
  ux_turner.elapsed_ms -= MIN(ux_turner.elapsed_ms, elapsed_ms);
  if (ux_turner.elapsed_ms == 0) {
    // switch to next step
    ux_turner.current_step=(ux_turner.current_step+1)%ux_turner.steps_count;
    ux_turner.elapsed_ms = ux_turner.steps[ux_turner.current_step].next_step_ms;
    UX_DISPLAY(ux_turner_elements, ux_turner_element_preprocessor);
  }
}
#endif // HAVE_BOLOS_UX

void ux_turner_display(unsigned int current_step, 
                     const ux_turner_step_t* steps,
                     unsigned int steps_count,
                     button_push_callback_t button_callback) {
  // reset to first entry
  ux_turner.steps_count = steps_count;

  if (current_step != UX_TURNER_UNCHANGED_ENTRY) {
    ux_turner.current_step = current_step;
    if (ux_turner.current_step > ux_turner.steps_count) {
      ux_turner.current_step = 0;
    }
  }
  ux_turner.steps = steps;

  ux_turner.button_callback = button_callback;

#ifdef HAVE_BOLOS_UX
  screen_state_init(0);

  // static dashboard content
  G_bolos_ux_context.screen_stack[0].element_arrays[0].element_array = ux_turner_elements;
  G_bolos_ux_context.screen_stack[0].element_arrays[0].element_array_count = ARRAYLEN(ux_turner_elements);
  G_bolos_ux_context.screen_stack[0].element_arrays_count = 1;

  // ensure the string_buffer will be set before each button is displayed
  G_bolos_ux_context.screen_stack[0].screen_before_element_display_callback = ux_turner_element_preprocessor;
  G_bolos_ux_context.screen_stack[0].button_push_callback = ux_turner_elements_button;
  G_bolos_ux_context.screen_stack[0].ticker_value = ux_turner.steps[ux_turner.current_step].next_step_ms;
  G_bolos_ux_context.screen_stack[0].ticker_interval = ux_turner.steps[ux_turner.current_step].next_step_ms;
  G_bolos_ux_context.screen_stack[0].ticker_callback = ux_turner_ticker_bolos_ux;

  screen_display_init(0);
#else
  ux_turner.elapsed_ms = ux_turner.steps[ux_turner.current_step].next_step_ms;
  // display the menu current entry
  UX_DISPLAY(ux_turner_elements, ux_turner_element_preprocessor);
#endif
}

void ux_check_status_default(unsigned int status) {
  // nothing to be done here by default.
  UNUSED(status);
}

void ux_check_status(unsigned int status) __attribute__ ((weak, alias ("ux_check_status_default")));

const bagl_element_t const clear_element = {{BAGL_RECTANGLE, 0, 0, 0, 128, 32, 0, 0, 0, 0x000000, 0x000000, 0 , 0},NULL,0,0,0,NULL,NULL,NULL};
const bagl_element_t const printf_element = {{BAGL_LABELINE, 0, 0, 26, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000, BAGL_FONT_OPEN_SANS_EXTRABOLD_11px | BAGL_FONT_ALIGNMENT_CENTER |BAGL_FONT_ALIGNMENT_MIDDLE , 0},"Default printf",0,0,0,NULL,NULL,NULL};

void debug_wait_displayed(void) {
  // wait up the display processed
  io_seproxyhal_spi_recv(G_io_seproxyhal_spi_buffer, sizeof(G_io_seproxyhal_spi_buffer), 0);
  io_seproxyhal_general_status();
  // wait next event (probably a ticker, if not, too bad... this is debug !!)
  io_seproxyhal_spi_recv(G_io_seproxyhal_spi_buffer, sizeof(G_io_seproxyhal_spi_buffer), 0);  
}

void debug_printf(void* buffer) {
  io_seproxyhal_display_default(&clear_element);
  debug_wait_displayed();
  os_memmove(&ux_menu.tmp_element, &printf_element, sizeof(bagl_element_t));
  ux_menu.tmp_element.text = buffer;
  io_seproxyhal_display_default(&ux_menu.tmp_element);
  debug_wait_displayed();
}
#ifdef HAVE_DEBUG
#define L(x) debug_printf(x)
#else // HAVE_DEBUG
#define L(x)
#endif // HAVE_DEBUG

#endif // OS_IO_SEPROXYHAL
