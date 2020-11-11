
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

#ifdef TARGET_NANOX
#define HAVE_SEPROXYHAL_MCU
#define HAVE_MCU_PROTECT
#endif // TARGET_NANOX

#include "os.h"
#include "os_io_usb.h"
#include "string.h"


#ifdef OS_IO_SEPROXYHAL

#include "os_io_seproxyhal.h"

#ifdef HAVE_BLUENRG
#include "hci.h"
#endif // HAVE_BLUENRG

#include "ux.h"
#include "checks.h"

#ifdef HAVE_IO_U2F
#include "u2f_processing.h"
#include "u2f_transport.h"
#endif

#ifndef VERSION
#define VERSION "dummy"
#endif // VERSION

#ifdef DEBUG
#define LOG printf
#else 
#define LOG(...)
#endif

#ifdef HAVE_IO_USB
#ifdef HAVE_L4_USBLIB
#include "usbd_def.h"
#include "usbd_core.h"
extern USBD_HandleTypeDef USBD_Device;
#endif
#endif

void io_seproxyhal_handle_ble_event(void);

unsigned int os_io_seph_recv_and_process(unsigned int dont_process_ux_events);

io_seph_app_t G_io_app;

  // usb endpoint buffer
unsigned char G_io_usb_ep_buffer[MAX(USB_SEGMENT_SIZE, BLE_SEGMENT_SIZE)];

// discriminated from io to allow for different memory placement
typedef struct ux_seph_s {
  unsigned int button_mask;
  unsigned int button_same_mask_counter;
#ifdef TARGET_BLUE
  bagl_element_t* last_touched_not_released_component;
#endif // TARGET_BLUE
} ux_seph_os_and_app_t;
ux_seph_os_and_app_t G_ux_os;


#ifndef IO_RAPDU_TRANSMIT_TIMEOUT_MS 
#define IO_RAPDU_TRANSMIT_TIMEOUT_MS 2000UL
#endif // IO_RAPDU_TRANSMIT_TIMEOUT_MS

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

void io_seproxyhal_handle_usb_event(void) {
  switch(G_io_seproxyhal_spi_buffer[3]) {
    case SEPROXYHAL_TAG_USB_EVENT_RESET:
      USBD_LL_SetSpeed(&USBD_Device, USBD_SPEED_FULL);  
      USBD_LL_Reset(&USBD_Device);
      // ongoing APDU detected, throw a reset, even if not the media. to avoid potential troubles.
      if (G_io_app.apdu_media != IO_APDU_MEDIA_NONE) {
        THROW(EXCEPTION_IO_RESET);
      }
      os_memset(G_io_app.usb_ep_xfer_len, 0, sizeof(G_io_app.usb_ep_xfer_len));
      os_memset(G_io_app.usb_ep_timeouts, 0, sizeof(G_io_app.usb_ep_timeouts));
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
  if ((epnum & 0x7F) < IO_USB_MAX_ENDPOINTS) {
  return G_io_app.usb_ep_xfer_len[epnum&0x7F];
}
  return 0;
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
      if ((G_io_seproxyhal_spi_buffer[3]&0x7F) < IO_USB_MAX_ENDPOINTS) {
        // discard ep timeout as we received the sent packet confirmation
        G_io_app.usb_ep_timeouts[G_io_seproxyhal_spi_buffer[3]&0x7F].timeout = 0;
        // propagate sending ack of the data
        USBD_LL_DataInStage(&USBD_Device, G_io_seproxyhal_spi_buffer[3]&0x7F, &G_io_seproxyhal_spi_buffer[6]);
      }
      break;

    /* This event is received when a new DATA token is received on an endpoint */
    case SEPROXYHAL_TAG_USB_EP_XFER_OUT:
      if ((G_io_seproxyhal_spi_buffer[3]&0x7F) < IO_USB_MAX_ENDPOINTS) {
        // saved just in case it is needed ...
        G_io_app.usb_ep_xfer_len[G_io_seproxyhal_spi_buffer[3]&0x7F] = G_io_seproxyhal_spi_buffer[5];
        // prepare reception
        USBD_LL_DataOutStage(&USBD_Device, G_io_seproxyhal_spi_buffer[3]&0x7F, &G_io_seproxyhal_spi_buffer[6]);
      }
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
// TODO add a blocking parameter, for HID KBD sending, or use a USB busy flag per channel to know if 
// the transfer has been processed or not. and move on to the next transfer on the same endpoint
void io_usb_send_ep(unsigned int ep, unsigned char* buffer, unsigned short length, unsigned int timeout) {
  
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
  // setup timeout of the endpoint
  G_io_app.usb_ep_timeouts[ep&0x7F].timeout = IO_RAPDU_TRANSMIT_TIMEOUT_MS;
}

void io_usb_send_apdu_data(unsigned char* buffer, unsigned short length) {
  // wait for 20 events before hanging up and timeout (~2 seconds of timeout)
  io_usb_send_ep(0x82, buffer, length, 20);
}

#ifdef HAVE_WEBUSB
void io_usb_send_apdu_data_ep0x83(unsigned char* buffer, unsigned short length) {
  // wait for 20 events before hanging up and timeout (~2 seconds of timeout)
  io_usb_send_ep(0x83, buffer, length, 20);
}
#endif // HAVE_WEBUSB

#endif // HAVE_IO_USB

#ifdef HAVE_BLUENRG
void io_seproxyhal_handle_bluenrg_event(void) {
  BEGIN_TRY {
    TRY {
      // handle the incoming packet       
      HCI_recv_packet(G_io_seproxyhal_spi_buffer+3, MIN(U2BE(G_io_seproxyhal_spi_buffer, 1), sizeof(G_io_seproxyhal_spi_buffer)-3));
      
    }
    FINALLY {

    }
  }
  END_TRY;
}

#else // HAVE_BLUENRG
void io_seproxyhal_handle_bluenrg_event(void) {

}
#endif // HAVE_BLUENRG

void io_seproxyhal_handle_capdu_event(void) {
  if (G_io_app.apdu_state == APDU_IDLE) {
    size_t max = MIN(sizeof(G_io_apdu_buffer)-3, sizeof(G_io_seproxyhal_spi_buffer)-3);
    size_t size = U2BE(G_io_seproxyhal_spi_buffer, 1);

    G_io_app.apdu_media = IO_APDU_MEDIA_RAW; // for application code
    G_io_app.apdu_state = APDU_RAW; // for next call to io_exchange
    G_io_app.apdu_length = MIN(size, max);
    // copy apdu to apdu buffer
    os_memmove(G_io_apdu_buffer, G_io_seproxyhal_spi_buffer+3, G_io_app.apdu_length);
  }
}

unsigned int io_seproxyhal_handle_event(void) {
  unsigned int rx_len = U2BE(G_io_seproxyhal_spi_buffer, 1);

  switch(G_io_seproxyhal_spi_buffer[0]) {
  #ifdef HAVE_IO_USB
    case SEPROXYHAL_TAG_USB_EVENT:
      if (rx_len != 1) {
        return 0;
      }
      io_seproxyhal_handle_usb_event();
      return 1;

    case SEPROXYHAL_TAG_USB_EP_XFER_EVENT:
      if (rx_len < 3) {
        // error !
        return 0;
      }
      io_seproxyhal_handle_usb_ep_xfer_event();
      return 1;
  #endif // HAVE_IO_USB

  #ifdef HAVE_BLE
  #ifdef HAVE_BLUENRG
    case SEPROXYHAL_TAG_BLUENRG_RECV_EVENT:
      io_seproxyhal_handle_bluenrg_event();
      goto check_ble_apdu;
  #endif // HAVE_BLUENRG
    case SEPROXYHAL_TAG_BLE_RECV_EVENT:
      io_seproxyhal_handle_ble_event();
  #ifdef HAVE_BLUENRG
    check_ble_apdu:
  #endif // HAVE_BLUENRG
      if (G_io_app.apdu_state == APDU_IDLE && G_io_app.apdu_length) {
        G_io_app.apdu_media = IO_APDU_MEDIA_BLE; // for application code
        G_io_app.apdu_state = APDU_BLE; // for next call to io_exchange
      }
      return 1;
  #endif // HAVE_BLE

    case SEPROXYHAL_TAG_CAPDU_EVENT:
      io_seproxyhal_handle_capdu_event();
      return 1;

      // ask the user if not processed here
    case SEPROXYHAL_TAG_TICKER_EVENT:
      // process ticker events to timeout the IO transfers, and forward to the user io_event function too
      G_io_app.ms += 100; // value is by default, don't change the ticker configuration
#ifdef HAVE_IO_USB
      {
        unsigned int i = IO_USB_MAX_ENDPOINTS;
        while(i--) {
          if (G_io_app.usb_ep_timeouts[i].timeout) {
            G_io_app.usb_ep_timeouts[i].timeout-=MIN(G_io_app.usb_ep_timeouts[i].timeout, 100);
            if (!G_io_app.usb_ep_timeouts[i].timeout) {
              // timeout !
              G_io_app.apdu_state = APDU_IDLE;
              THROW(EXCEPTION_IO_RESET);
            }
          }
        }
        /* fallthrough */
      }
#endif // HAVE_IO_USB
#ifdef HAVE_BLE_APDU
      {
        if (G_io_app.ble_xfer_timeout) {
          G_io_app.ble_xfer_timeout -= MIN(G_io_app.ble_xfer_timeout, 100);
          if (!G_io_app.ble_xfer_timeout) {
            G_io_app.apdu_state = APDU_IDLE;
            THROW(EXCEPTION_IO_RESET);
          }
        }
        /* fallthrough */
      }
#endif // HAVE_BLE_APDU
      // no break is intentional
    default:
      return io_event(CHANNEL_SPI);
  }
  // defaultly return as not processed
  return 0;
}

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
#ifndef HAVE_BOLOS
  // Enforce OS compatibility
  check_api_level(CX_COMPAT_APILEVEL);

#ifdef HAVE_MCU_PROTECT
  // engage RDP2 on MCU
  unsigned char c[4];
  c[0] = SEPROXYHAL_TAG_MCU;
  c[1] = 0;
  c[2] = 1;
  c[3] = SEPROXYHAL_TAG_MCU_TYPE_PROTECT;
  io_seproxyhal_spi_send(c, 4);
#endif // HAVE_MCU_PROTECT
#endif // HAVE_BOLOS

#ifdef HAVE_BOLOS_APP_STACK_CANARY
  app_stack_canary = APP_STACK_CANARY_MAGIC;
#endif // HAVE_BOLOS_APP_STACK_CANARY  

  // wipe the io structure before it's used
#ifdef TARGET_NANOX
  unsigned int plane = G_io_app.plane_mode;
#endif // TARGET_NANOX
  os_memset(&G_io_app, 0, sizeof(G_io_app));
#ifdef TARGET_NANOX
  G_io_app.plane_mode = plane;
#endif // TARGET_NANOX

  G_io_app.apdu_state = APDU_IDLE;
  G_io_app.apdu_length = 0;
  G_io_app.apdu_media = IO_APDU_MEDIA_NONE;

  G_io_app.ms = 0;

  #ifdef DEBUG_APDU
  debug_apdus_offset = 0;
  #endif // DEBUG_APDU

  #ifdef HAVE_USB_APDU
  io_usb_hid_init();
  #endif // HAVE_USB_APDU

  io_seproxyhal_init_ux();
  io_seproxyhal_init_button();

#if !defined(HAVE_BOLOS)
  check_audited_app();
#endif // !defined(HAVE_BOLOS)
}

void io_seproxyhal_init_ux(void) {
#ifdef TARGET_BLUE
  // initialize the touch part
  G_ux_os.last_touched_not_released_component = NULL;
#endif // TARGET_BLUE

// #ifdef TARGET_NANOX
//   // wipe frame buffer
//   screen_clear();
// #endif // TARGET_NANOX
}

void io_seproxyhal_init_button(void) {
  // no button push so far
  G_ux_os.button_mask = 0;
  G_ux_os.button_same_mask_counter = 0;
}

#ifdef HAVE_BAGL

#ifdef TARGET_BLUE
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

  // swap colors
  os_memmove(&e, (void*)element, sizeof(bagl_element_t));
  e.component.fgcolor = element->overfgcolor;
  e.component.bgcolor = element->overbgcolor;

  io_seproxyhal_display(&e);
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
    if (&elements[comp_idx] == G_ux_os.last_touched_not_released_component) {
      last_touched_not_released_component_was_in_current_array = 1;
    }

    // the first component drawn with a 
    if ((elements[comp_idx].component.type & BAGL_FLAG_TOUCHABLE) 
        && elements[comp_idx].component.x-elements[comp_idx].touch_area_brim <= x && x<elements[comp_idx].component.x+elements[comp_idx].component.width+elements[comp_idx].touch_area_brim
        && elements[comp_idx].component.y-elements[comp_idx].touch_area_brim <= y && y<elements[comp_idx].component.y+elements[comp_idx].component.height+elements[comp_idx].touch_area_brim) {

      // outing the previous over'ed component
      if (&elements[comp_idx] != G_ux_os.last_touched_not_released_component 
              && G_ux_os.last_touched_not_released_component != NULL) {
        // only out the previous element if the newly matching will be displayed 
        if (!before_display || before_display(&elements[comp_idx])) {
          if (io_seproxyhal_touch_out(G_ux_os.last_touched_not_released_component, before_display)) {
            // previous component is considered released
            G_ux_os.last_touched_not_released_component = NULL;
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
          G_ux_os.last_touched_not_released_component = NULL;
          return;
        }
      }
      else if (event_kind == SEPROXYHAL_TAG_FINGER_EVENT_TOUCH) {
        // ask for overing
        if (io_seproxyhal_touch_over(&elements[comp_idx], before_display)) {
          // remember the last touched component
          G_ux_os.last_touched_not_released_component = (bagl_element_t*)&elements[comp_idx];
          return;
        }
      }
    }
  }

  // if overing out of component or over another component, the out event is sent after the over event of the previous component
  if(last_touched_not_released_component_was_in_current_array 
    && G_ux_os.last_touched_not_released_component != NULL) {

    // we won't be able to notify the out, don't do it, in case a diplay refused the dra of the relased element and the position matched another element of the array (in autocomplete for example)
    if (io_seproxyhal_spi_is_status_sent()) {
      return;
    }
    
    if (io_seproxyhal_touch_out(G_ux_os.last_touched_not_released_component, before_display)) {
      // ok component out has been emitted
      G_ux_os.last_touched_not_released_component = NULL;
    }
  }

  // not processed
}
#endif // TARGET_BLUE

void io_seproxyhal_display_bitmap(int x, int y, unsigned int w, unsigned int h, unsigned int* color_index, unsigned int bit_per_pixel, unsigned char* bitmap) {
  // component type = ICON
  // component icon id = 0
  // => bitmap transmitted
  if (w && h) {
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


#if defined(TARGET_NANOX)
void io_seproxyhal_display_icon(bagl_component_t* icon_component, bagl_icon_details_t* icon_details) {
  bagl_component_t icon_component_mod;

  // // avoid sending another status :), fixes a lot of bugs in the end
  // if (io_seproxyhal_spi_is_status_sent()) {
  //   return;
  // }

  // ensure not being out of bounds in the icon component agianst the declared icon real size
  os_memmove(&icon_component_mod, icon_component, sizeof(bagl_component_t));
  icon_component_mod.width = icon_details->width;
  icon_component_mod.height = icon_details->height;
  icon_component = &icon_component_mod;

  bagl_draw_glyph(&icon_component_mod, icon_details);
}

void io_seproxyhal_display_default(const bagl_element_t* element) {

  const bagl_element_t* el = (const bagl_element_t*) PIC(element);
  const char* txt = (const char*)PIC(el->text);
  // process automagically address from rom and from ram
  unsigned int type = (el->component.type & ~(BAGL_FLAG_TOUCHABLE));

  // // avoid sending another status :), fixes a lot of bugs in the end
  // if (io_seproxyhal_spi_is_status_sent()) {
  //   return;
  // }

  if (type != BAGL_NONE) {
    if (txt != NULL) {
      // consider an icon details descriptor is pointed by the context
      if (type == BAGL_ICON && el->component.icon_id == 0) {
        // SECURITY: due to this wild cast, the code MUST be executed on the application side instead of in 
        //           the syscall sides to avoid buffer overflows and a real hard way of checking buffer 
        //           belonging in the syscall dispatch
        bagl_draw_glyph(&el->component, (const bagl_icon_details_t *)txt);
      }
      else {
        bagl_draw_with_context(&el->component, txt, strlen(txt), BAGL_ENCODING_LATIN1);
      }
    }
    else {
      bagl_draw_with_context(&el->component, NULL, 0, 0);
    }
  }
}

#else // TARGET_NANOX
void io_seproxyhal_display_icon(bagl_component_t* icon_component, bagl_icon_details_t* icon_det) {
  bagl_component_t icon_component_mod;
  const bagl_icon_details_t* icon_details = (bagl_icon_details_t*)PIC(icon_det);
  if (icon_details && icon_details->bitmap) {
    // ensure not being out of bounds in the icon component agianst the declared icon real size
    os_memmove(&icon_component_mod, (bagl_component_t *)PIC(icon_component), sizeof(bagl_component_t));
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
}

void io_seproxyhal_display_default(const bagl_element_t * el) {

  const bagl_element_t* element = (const bagl_element_t*) PIC(el);

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
        io_seproxyhal_display_icon((bagl_component_t*)&element->component, (bagl_icon_details_t*)text_adr);
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
#endif // TARGET_NANOX

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

void io_seproxyhal_button_push(button_push_callback_t button_callback, unsigned int new_button_mask) {
  if (button_callback) {
    unsigned int button_mask;
    unsigned int button_same_mask_counter;
    // enable speeded up long push
    if (new_button_mask == G_ux_os.button_mask) {
      // each 100ms ~
      G_ux_os.button_same_mask_counter++;
    }

    // when new_button_mask is 0 and 

    // append the button mask
    button_mask = G_ux_os.button_mask | new_button_mask;

    // pre reset variable due to os_sched_exit
    button_same_mask_counter = G_ux_os.button_same_mask_counter;

    // reset button mask
    if (new_button_mask == 0) {
      // reset next state when button are released
      G_ux_os.button_mask = 0;
      G_ux_os.button_same_mask_counter=0;

      // notify button released event
      button_mask |= BUTTON_EVT_RELEASED;
    }
    else {
      G_ux_os.button_mask = button_mask;
    }

    // reset counter when button mask changes
    if (new_button_mask != G_ux_os.button_mask) {
      G_ux_os.button_same_mask_counter=0;
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

void io_seproxyhal_disable_ble(void) {
    // ble off
  G_io_seproxyhal_spi_buffer[0] = SEPROXYHAL_TAG_BLE_RADIO_POWER;
  G_io_seproxyhal_spi_buffer[1] = 0;
  G_io_seproxyhal_spi_buffer[2] = 1;
  G_io_seproxyhal_spi_buffer[3] = 0;
  io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, 4);
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

#ifdef HAVE_BLE
    io_seproxyhal_disable_ble();
#endif // HAVE_BLE

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

#ifdef HAVE_IO_U2F
u2f_service_t G_io_u2f;
#endif // HAVE_IO_U2F

unsigned int os_io_seproxyhal_get_app_name_and_version(void) __attribute__((weak));
unsigned int os_io_seproxyhal_get_app_name_and_version(void) {
  unsigned int tx_len, len;
  // build the get app name and version reply
  tx_len = 0;
  G_io_apdu_buffer[tx_len++] = 1; // format ID

#ifndef HAVE_BOLOS
  // append app name
  len = os_registry_get_current_app_tag(BOLOS_TAG_APPNAME, G_io_apdu_buffer+tx_len+1, sizeof(G_io_apdu_buffer)-tx_len-1);
  G_io_apdu_buffer[tx_len++] = len;
  tx_len += len;
  // append app version
  len = os_registry_get_current_app_tag(BOLOS_TAG_APPVERSION, G_io_apdu_buffer+tx_len+1, sizeof(G_io_apdu_buffer)-tx_len-1);
  G_io_apdu_buffer[tx_len++] = len;
  tx_len += len;
#else // HAVE_BOLOS
  // append app name
  len = strlen("BOLOS");
  G_io_apdu_buffer[tx_len++] = len;
  strcpy((char*)(G_io_apdu_buffer+tx_len), "BOLOS");
  tx_len += len;
  // append app version
  len = strlen(VERSION);
  G_io_apdu_buffer[tx_len++] = len;
  strcpy((char*)(G_io_apdu_buffer+tx_len), VERSION);
  tx_len += len;
#endif // HAVE_BOLOS

#if !defined(HAVE_IO_TASK) || !defined(HAVE_BOLOS)
  // to be fixed within io tasks
  // return OS flags to notify of platform's global state (pin lock etc)
  G_io_apdu_buffer[tx_len++] = 1; // flags length
  G_io_apdu_buffer[tx_len++] = os_flags();
#endif // !defined(HAVE_IO_TASK) || !defined(HAVE_BOLOS)

  // status words
  G_io_apdu_buffer[tx_len++] = 0x90;
  G_io_apdu_buffer[tx_len++] = 0x00;
  return tx_len;
}


unsigned short io_exchange(unsigned char channel, unsigned short tx_len) {
  unsigned short rx_len;
  unsigned int timeout_ms;

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

reply_apdu:
  switch(channel&~(IO_FLAGS)) {
  case CHANNEL_APDU:
    // TODO work up the spi state machine over the HAL proxy until an APDU is available

    if (tx_len && !(channel&IO_ASYNCH_REPLY)) {
      // ensure it's our turn to send a command/status, could lag a bit before sending the reply
      while (io_seproxyhal_spi_is_status_sent()) {
        io_seproxyhal_spi_recv(G_io_seproxyhal_spi_buffer, sizeof(G_io_seproxyhal_spi_buffer), 0);
        // process without sending status on tickers etc, to ensure keeping the hand
        os_io_seph_recv_and_process(1);
      }

      // reinit sending timeout for APDU replied within io_exchange
      timeout_ms = G_io_app.ms + IO_RAPDU_TRANSMIT_TIMEOUT_MS;

      // until the whole RAPDU is transmitted, send chunks using the current mode for communication
      for (;;) {
        switch(G_io_app.apdu_state) {
          default: 
            // delegate to the hal in case of not generic transport mode (or asynch)
            if (io_exchange_al(channel, tx_len) == 0) {
              goto break_send;
            }
            /* fallthrough */
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

            // isngle packet reply, mark immediate idle
            G_io_app.apdu_state = APDU_IDLE;
            // finished, no chunking
            goto break_send;

#ifdef HAVE_USB_APDU
          case APDU_USB_HID:
            // only send, don't perform synchronous reception of the next command (will be done later by the seproxyhal packet processing)
            io_usb_hid_send(io_usb_send_apdu_data, tx_len);
            goto break_send;
#ifdef HAVE_USB_CLASS_CCID
          case APDU_USB_CCID:
            io_usb_ccid_reply(G_io_apdu_buffer, tx_len);
            goto break_send;
#endif // HAVE_USB_CLASS_CCID
#ifdef HAVE_WEBUSB
          case APDU_USB_WEBUSB:
            io_usb_hid_send(io_usb_send_apdu_data_ep0x83, tx_len);
            goto break_send;
#endif // HAVE_WEBUSB
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

            // the reply has been prepared by the application, stop sending anti timeouts
            u2f_message_set_autoreply_wait_user_presence(&G_io_u2f, false);

            // continue processing currently received command until completely received.
            while(!u2f_message_repliable(&G_io_u2f)) {

              io_seproxyhal_general_status();
              do {
                io_seproxyhal_spi_recv(G_io_seproxyhal_spi_buffer, sizeof(G_io_seproxyhal_spi_buffer), 0);
                // check for reply timeout
                if (G_io_app.ms >= timeout_ms) {
                  THROW(EXCEPTION_IO_RESET);
                }
                // avoid a general status to be replied
                io_seproxyhal_handle_event();
              } while (io_seproxyhal_spi_is_status_sent());
            }
#ifdef U2F_PROXY_MAGIC

            // user presence + counter + rapdu + sw must fit the apdu buffer
            if (1U+ 4U+ tx_len +2U > sizeof(G_io_apdu_buffer)) {
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

#else // U2F_PROXY_MAGIC
            u2f_message_reply(&G_io_u2f, U2F_CMD_MSG, G_io_apdu_buffer, tx_len);
#endif // U2F_PROXY_MAGIC
            goto break_send;
#endif // HAVE_IO_U2F
        }
        continue;

      break_send:

        // wait end of reply transmission
        // TODO: add timeout here to avoid spending too much time when host has disconnected
        while (G_io_app.apdu_state != APDU_IDLE) {
#ifdef HAVE_TINY_COROUTINE
          tcr_yield();
#else // HAVE_TINY_COROUTINE
          io_seproxyhal_general_status();
          do {
            io_seproxyhal_spi_recv(G_io_seproxyhal_spi_buffer, sizeof(G_io_seproxyhal_spi_buffer), 0);
            // check for reply timeout (when asynch reply (over hid or u2f for example))
            // this case shall be covered by usb_ep_timeout but is not, investigate that
            if (G_io_app.ms >= timeout_ms) {
              THROW(EXCEPTION_IO_RESET);
            }
            // avoid a general status to be replied
            io_seproxyhal_handle_event();
          } while (io_seproxyhal_spi_is_status_sent());
#endif // HAVE_TINY_COROUTINE
        }
        // reset apdu state
        G_io_app.apdu_state = APDU_IDLE;
        G_io_app.apdu_media = IO_APDU_MEDIA_NONE;

        G_io_app.apdu_length = 0;

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
        os_sched_exit_inline(EXCEPTION_IO_RESET);
        //reset();
      }
    }

#ifndef HAVE_TINY_COROUTINE
    if (!(channel&IO_ASYNCH_REPLY)) {
      
      // already received the data of the apdu when received the whole apdu
      if ((channel & (CHANNEL_APDU|IO_RECEIVE_DATA)) == (CHANNEL_APDU|IO_RECEIVE_DATA)) {
        // return apdu data - header
        return G_io_app.apdu_length-5;
      }

      // reply has ended, proceed to next apdu reception (reset status only after asynch reply)
      G_io_app.apdu_state = APDU_IDLE;
      G_io_app.apdu_media = IO_APDU_MEDIA_NONE;
    }
#else
#endif // HAVE_TINY_COROUTINE

    // reset the received apdu length
    G_io_app.apdu_length = 0;

    // ensure ready to receive an event (after an apdu processing with asynch flag, it may occur if the channel is not correctly managed)

    // until a new whole CAPDU is received
    for (;;) {

#ifdef HAVE_TINY_COROUTINE
      // give back hand to the seph task which interprets all incoming events first
      tcr_yield();
#else // HAVE_TINY_COROUTINE

      if (!io_seproxyhal_spi_is_status_sent()) {
        io_seproxyhal_general_status();
      }
      // wait until a SPI packet is available
      // NOTE: on ST31, dual wait ISO & RF (ISO instead of SPI)
      rx_len = io_seproxyhal_spi_recv(G_io_seproxyhal_spi_buffer, sizeof(G_io_seproxyhal_spi_buffer), 0);

      // can't process split TLV, continue
      if (rx_len < 3 && rx_len != U2(G_io_seproxyhal_spi_buffer[1],G_io_seproxyhal_spi_buffer[2])+3U) {
        LOG("invalid TLV format\n");
        G_io_app.apdu_state = APDU_IDLE;
        G_io_app.apdu_length = 0;
        continue;
      }

      io_seproxyhal_handle_event();
#endif // HAVE_TINY_COROUTINE

      // an apdu has been received asynchroneously, return it
      if (G_io_app.apdu_state != APDU_IDLE && G_io_app.apdu_length > 0) {
#ifndef HAVE_BOLOS_NO_DEFAULT_APDU
        // handle reserved apdus
        // get name and version
        if (os_memcmp(G_io_apdu_buffer, "\xB0\x01\x00\x00", 4) == 0) {
          tx_len = os_io_seproxyhal_get_app_name_and_version();
          // disable 'return after tx' and 'asynch reply' flags
          channel &= ~IO_FLAGS;
          goto reply_apdu; 
        }
        // exit app after replied
        else if (os_memcmp(G_io_apdu_buffer, "\xB0\xA7\x00\x00", 4) == 0) {
          tx_len = 0;
          G_io_apdu_buffer[tx_len++] = 0x90;
          G_io_apdu_buffer[tx_len++] = 0x00;
          // exit app after replied
          channel |= IO_RESET_AFTER_REPLIED;
          goto reply_apdu; 
        }
#ifndef BOLOS_OS_UPGRADER
        // seed cookie
        // host: <nothing>
        // device: <format(1B)> <len(1B)> <seed magic cookie if pin is entered(len)> 9000 | 6985
        else if (os_memcmp(G_io_apdu_buffer, "\xB0\x02\x00\x00", 4) == 0) {
          tx_len = 0;
          if (os_global_pin_is_validated() == BOLOS_UX_OK) {
            unsigned int i;
            // format
            G_io_apdu_buffer[tx_len++] = 0x01;

#ifndef HAVE_BOLOS
            i = os_perso_seed_cookie(G_io_apdu_buffer+1+1, MIN(64,sizeof(G_io_apdu_buffer)-1-1-2));
#else
            i = os_perso_seed_cookie_os(G_io_apdu_buffer+1+1, MIN(64,sizeof(G_io_apdu_buffer)-1-1-2));
#endif // HAVE_BOLOS

            G_io_apdu_buffer[tx_len++] = i;
            tx_len += i;
            G_io_apdu_buffer[tx_len++] = 0x90;
            G_io_apdu_buffer[tx_len++] = 0x00;
          }
          else {
            G_io_apdu_buffer[tx_len++] = 0x69;
            G_io_apdu_buffer[tx_len++] = 0x85;
          }
          channel &= ~IO_FLAGS;
          goto reply_apdu; 
        }
#endif // BOLOS_OS_UPGRADER
#endif // HAVE_BOLOS_NO_DEFAULT_APDU
        return G_io_app.apdu_length;
      }
    }
    break;

  default:
    return io_exchange_al(channel, tx_len);
  }
}

unsigned int os_io_seph_recv_and_process(unsigned int dont_process_ux_events) {
  // send general status before receiving next event
  if (!io_seproxyhal_spi_is_status_sent()) {
    io_seproxyhal_general_status();
  }

  io_seproxyhal_spi_recv(G_io_seproxyhal_spi_buffer, sizeof(G_io_seproxyhal_spi_buffer), 0);

  switch (G_io_seproxyhal_spi_buffer[0]) {
    case SEPROXYHAL_TAG_FINGER_EVENT:
    case SEPROXYHAL_TAG_BUTTON_PUSH_EVENT:
    case SEPROXYHAL_TAG_TICKER_EVENT:
    case SEPROXYHAL_TAG_DISPLAY_PROCESSED_EVENT:
    case SEPROXYHAL_TAG_STATUS_EVENT:
      // perform UX event on these ones, don't process as an IO event
      if (dont_process_ux_events) {
        return 0;
      }
      /* fallthrough */

    default:
      // if malformed, then a stall is likely to occur
      if (io_seproxyhal_handle_event()) {
        return 1;
      }
  }
  return 0;
}

unsigned int os_ux_blocking(bolos_ux_params_t* params) {
  unsigned int ret;

  // until a real status is returned
  os_ux(params);
  ret = os_sched_last_status(TASK_BOLOS_UX);
  while(ret == BOLOS_UX_IGNORE 
     || ret == BOLOS_UX_CONTINUE) {

    // if the IO task is not running, then need to pump events manually
    if (! os_sched_is_running(TASK_SUBTASKS_START)) {
      if (os_io_seph_recv_and_process(1)) {
        continue;
      }
      // prepare processing of the packet by the ux
      params->ux_id = BOLOS_UX_EVENT;
      params->len = 0;
      os_ux(params);
      ret = os_sched_last_status(TASK_BOLOS_UX);
    }
    else {
      // wait until UX takes some process time and update it's status
      os_sched_yield(BOLOS_UX_OK);
      // only retrieve the current UX state
      ret = os_sched_last_status(TASK_BOLOS_UX);
    }
  }

  return ret;
} 

// so unoptimized
void mcu_usb_printc(unsigned char c) {
  unsigned char buf[4];
#ifdef TARGET_NANOX
  buf[0] = SEPROXYHAL_TAG_PRINTF;
#else // TARGET_NANOX
  buf[0] = SEPROXYHAL_TAG_PRINTF_STATUS;
#endif // TARGET_NANOX
  buf[1] = 0;
  buf[2] = 1;
  buf[3] = c;
  io_seproxyhal_spi_send(buf, 4);
#ifndef TARGET_NANOX
#ifndef IO_SEPROXYHAL_DEBUG
  // wait printf ack (no race kthx)
  io_seproxyhal_spi_recv(buf, 3, 0);
  buf[0] = 0; // consume tag to avoid misinterpretation (due to IO_CACHE)
#endif // IO_SEPROXYHAL_DEBUG
#endif // TARGET_NANOX
}

#ifdef HAVE_IO_TASK
void io_process(void) {
  for (;;) {
    if (!(G_io_app.io_flags & IO_FINISHED)) {
      G_io_app.apdu_length = io_exchange(CHANNEL_APDU | G_io_app.io_flags, G_io_app.apdu_length);
      // mark IO as ended
      G_io_app.io_flags |= IO_FINISHED;
    }
    else {
      // pump packets (process all through handle_events)
      os_io_seph_recv_and_process(0);
    }
    // we have finished our call, notify the other tasks
    os_sched_yield_inline(BOLOS_UX_OK);
  } 
}
void io_task(void) {
  for(;;) {
    BEGIN_TRY {
      TRY {
        io_start();
        io_process();
      }
      CATCH_ALL {
        // any error leading here is triggering an IO stack reset
        os_sched_yield_inline(EXCEPTION_IO_RESET);
      }
      FINALLY {
      }
    }
    END_TRY;
  }
}

#endif // HAVE_IO_TASK

void io_seproxyhal_io_heartbeat(void) {
  io_seproxyhal_general_status();
  do {
    io_seproxyhal_spi_recv(G_io_seproxyhal_spi_buffer, sizeof(G_io_seproxyhal_spi_buffer), 0);
    // avoid a general status to be replied
    if(G_io_seproxyhal_spi_buffer[0] != SEPROXYHAL_TAG_TICKER_EVENT) {
      io_seproxyhal_handle_event();
    }
  } while (io_seproxyhal_spi_is_status_sent());
}
#endif // OS_IO_SEPROXYHAL
