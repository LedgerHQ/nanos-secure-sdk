/*******************************************************************************
*   Ledger Nano S - Secure firmware
*   (c) 2016 Ledger
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

#ifndef OS_IO_SEPROXYHAL_H
#define OS_IO_SEPROXYHAL_H

#include "os.h"

#ifdef OS_IO_SEPROXYHAL

#include "seproxyhal_protocol.h"

#ifdef HAVE_BAGL
#include "bagl.h"

typedef struct bagl_element_e bagl_element_t;

// callback returns 0 when element must not be redrawn (with a changing color or
// what so ever)
typedef unsigned int (*bagl_element_callback_t)(const bagl_element_t *element);

// a graphic element is an element with defined text and actions depending on
// user touches
struct bagl_element_e {
    bagl_component_t component;

    const char *text;
    unsigned char touch_area_brim;
    int overfgcolor;
    int overbgcolor;
    bagl_element_callback_t tap;
    bagl_element_callback_t out;
    bagl_element_callback_t over;
};

// touch management helper function (callback the call with the element for the
// given position, taking into account touch release)
void io_seproxyhal_touch(const bagl_element_t *elements,
                         unsigned short element_count, unsigned short x,
                         unsigned short y, unsigned char event_kind);
void io_seproxyhal_touch_element_callback(
    const bagl_element_t *elements, unsigned short element_count,
    unsigned short x, unsigned short y, unsigned char event_kind,
    bagl_element_callback_t before_display);
// callback to be implemented by the se
void io_seproxyhal_touch_callback(const bagl_element_t *element,
                                  unsigned char event);

typedef unsigned int (*button_push_callback_t)(
    unsigned int button_mask, unsigned int button_mask_counter);
#define BUTTON_LEFT 1
#define BUTTON_RIGHT 2
#define BUTTON_EVT_RELEASED 0x80000000UL
void io_seproxyhal_button_push(button_push_callback_t button_push_callback,
                               unsigned int new_button_mask);

// hal point (if application has to reprocess elements)
void io_seproxyhal_display(const bagl_element_t *element);

// Helper function that give a realistic timing of scrolling for label with text
// larger than screen
unsigned int bagl_label_roundtrip_duration_ms(const bagl_element_t *e,
                                              unsigned int average_char_width);

// default version to be called by ::io_seproxyhal_display if nothing to be done
// by the application
void io_seproxyhal_display_default(bagl_element_t *element);
#endif // HAVE_BAGL

extern unsigned char G_io_seproxyhal_spi_buffer[IO_SEPROXYHAL_BUFFER_SIZE_B];

SYSCALL void io_seproxyhal_spi_send(const unsigned char *buffer PLENGTH(length),
                                    unsigned short length);

// return 1 if the previous seproxyhal exchange has been terminated with a
// status (packet which starts with 011x xxxx)
// else 0, which means the exchange needs to be closed.
SYSCALL unsigned int io_seproxyhal_spi_is_status_sent(void);

// not to be called by application (application is triggered using io_event
// instead), resered for seproxyhal
#define IO_CACHE 1
SYSCALL unsigned short
io_seproxyhal_spi_recv(unsigned char *buffer PLENGTH(maxlength),
                       unsigned short maxlength, unsigned int flags);

// HAL init, not meant to be called by applications, which shall call
// ::io_seproxyhal_init instead
void io_seproxyhal_spi_init(void);

// init all (io/ux/etc)
void io_seproxyhal_init(void);

// only reinit ux related globals
void io_seproxyhal_init_ux(void);

// delegate function for generic io_exchange
unsigned short io_exchange_al(unsigned char channel_and_flags,
                              unsigned short tx_len);

// for delegation of Native NFC / USB
unsigned char io_event(unsigned char channel);

void BLE_power(unsigned char powered, const char *discovered_name);
void USB_power(unsigned char enabled);

void io_seproxyhal_handle_usb_event(void);
void io_seproxyhal_handle_usb_ep_xfer_event(void);
uint16_t io_seproxyhal_get_ep_rx_size(uint8_t epnum);

// process event for io protocols when waiting for a ux operation to end
// return 1 when event replied, 0 else
unsigned int io_seproxyhal_handle_event(void);

// reply a general status last command
void io_seproxyhal_general_status(void);

// reply a MORE COMMANDS status for the proxyhal to wait for more data later
void os_io_seproxyhal_general_status_processing(void);

void io_usb_send_apdu_data(unsigned char *buffer, unsigned short length);

typedef enum {
    APDU_IDLE,
    APDU_BLE,
    APDU_BLE_WAIT_NOTIFY,
    APDU_NFC_M24SR,
    APDU_NFC_M24SR_SELECT,
    APDU_NFC_M24SR_FIRST,
    APDU_NFC_M24SR_RAPDU,
    APDU_USB_HID,
} io_apdu_state_e;

extern volatile io_apdu_state_e G_io_apdu_state; // by default
extern volatile unsigned short
    G_io_apdu_offset; // total length already received
extern volatile unsigned short G_io_apdu_length; // total length to be received
extern volatile unsigned short G_io_apdu_seq;
extern volatile io_apdu_media_t G_io_apdu_media;

#ifdef HAVE_USB
extern unsigned int usb_ep_xfer_len[7];
#endif // HAVE_USB

#ifdef HAVE_BLE_APDU
void BLE_protocol_recv(unsigned char data_length, unsigned char *att_data);

#define BLE_protocol_send_RET_OK 0
#define BLE_protocol_send_RET_BUSY 1
#define BLE_protocol_send_RET_ABORTED 2
// TODO ensure not being stuck in case disconnect or timeout during reply.
// add a timeout for reply operation
char BLE_protocol_send(unsigned char *response_apdu,
                       unsigned short response_apdu_length);
#endif // HAVE_BLE_APDU

/**
 *  Ledger Bluetooth Low Energy APDU Protocol
 *  Characteristic content:
 *  [______________________________]
 *   TT SSSS VVVV................VV
 *
 *  All fields are big endian encoded.
 *  TT: 1 byte content tag
 *  SSSS: 2 bytes sequence number, big endian encoded (start @ 0).
 *  VVVV..VV: variable length content. When SSSS is 0, the first two bytes
 * encodes in big endian the total length of the APDU to transport.
 *
 *  Command/Response APDU are split in chunks to fill up the bluetooth's
 * characteristic
 *
 *  APDU are using either standard or extended header. up to the application to
 * check the total received length and the lc field
 *
 *  Tags:
 *  Direction:*  T:0x05 S=<sequence-idx-U2BE>
 * V=<seq==0?totallength(U2BE):NONE><apducontent> APDU (command/response)
 * packet.
 *
 * Example:
 * --------
 *   Wrapping of Command APDU:
 *     E0 FF 12 13 14
 *     15 16 17 18 19 1A 1B 1C
 *     1D 1E 1F 20 21 22 23 24
 *     25 26 27 28 29 2A 2B 2C
 *     2D 2E 2F 30 31 32 33 34
 *     35
 *   Result in 3 chunks (20 bytes at most):
 *     0500000026E0FF12131415161718191A1B1C1D1E
 *     0500011F202122232425262728292A2B2C2D2E2F
 *     050002303132333435
 *
 *
 *   Wrapping of Response APDU:
 *     15 16 17 18 19 1a 1b 1c
 *     1d 1e 1f 20 21 22 23 24
 *     25 26 27 28 29 2a 2b 2c
 *     2d 2e 2f 30 31 32 33 34
 *     35 90 00
 *   Result in 3 chunks (20 bytes at most):
 *     050000002315161718191a1b1c1d1e1f20212223
 *     0500012425262728292a2b2c2d2e2f3031323334
 *     050002359000
 */

/**
 * Common structure for applications to perform asynchronous UX aside IO
 * operations
 */
typedef struct ux_state_s {
    const bagl_element_t *elements;
    unsigned int elements_count;
    unsigned int
        elements_current; // currently displayed UI element in a set of elements
    bagl_element_callback_t
        elements_preprocessor; // called before an element is displayed
    button_push_callback_t button_push_handler;
} ux_state_t;
extern ux_state_t ux;

#define UX_INIT() os_memset(&ux, 0, sizeof(ux));

#define UX_DISPLAY(elements_array, preprocessor)                               \
    ux.elements = elements_array;                                              \
    ux.elements_count = sizeof(elements_array) / sizeof(elements_array[0]);    \
    ux.elements_current = 1;                                                   \
    ux.button_push_handler = elements_array##_button;                          \
    ux.elements_preprocessor = preprocessor;                                   \
    if (!ux.elements_preprocessor ||                                           \
        ux.elements_preprocessor(&elements_array[0])) {                        \
        io_seproxyhal_display(&elements_array[0]);                             \
    }

#define UX_REDISPLAY()                                                         \
    ux.elements_current = 1;                                                   \
    if (!ux.elements_preprocessor ||                                           \
        ux.elements_preprocessor(&ux.elements[0])) {                           \
        io_seproxyhal_display(&ux.elements[0]);                                \
    }

#define UX_DISPLAYED() (ux.elements_current >= ux.elements_count)

#define UX_DISPLAY_PROCESSED_EVENT()                                           \
    while (ux.elements_current < ux.elements_count) {                          \
        if (!ux.elements_preprocessor ||                                       \
            ux.elements_preprocessor(&ux.elements[ux.elements_current])) {     \
            io_seproxyhal_display(&ux.elements[ux.elements_current++]);        \
            break;                                                             \
        }                                                                      \
        ux.elements_current++;                                                 \
    }

#define UX_BUTTON_PUSH_EVENT(seph_packet)                                      \
    if (ux.button_push_handler) {                                              \
        io_seproxyhal_button_push(ux.button_push_handler,                      \
                                  seph_packet[3] >> 1);                        \
    }

#define UX_FINGER_EVENT(seph_packet)                                           \
    io_seproxyhal_touch(ux.elements, ux.elements_count,                        \
                        (seph_packet[4] << 8) | (seph_packet[5] & 0xFF),       \
                        (seph_packet[6] << 8) | (seph_packet[7] & 0xFF),       \
                        seph_packet[3]);

void io_seproxyhal_setup_ticker(unsigned int interval_ms);

#endif // OS_IO_SEPROXYHAL

#endif // OS_IO_SEPROXYHAL_H
