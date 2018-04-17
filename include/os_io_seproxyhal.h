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

#ifndef OS_IO_SEPROXYHAL_H
#define OS_IO_SEPROXYHAL_H

#include "os.h"

#ifdef OS_IO_SEPROXYHAL

#include "seproxyhal_protocol.h"

// helper macro to swap values, without intermediate value
#define SWAP(a, b)                                                             \
    {                                                                          \
        a ^= b;                                                                \
        b ^= a;                                                                \
        a ^= b;                                                                \
    }

#ifdef HAVE_BAGL
#include "bagl.h"

typedef struct bagl_element_e bagl_element_t;

// callback returns NULL when element must not be redrawn (with a changing color
// or what so ever)
typedef const bagl_element_t *(*bagl_element_callback_t)(
    const bagl_element_t *element);

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

extern bagl_element_t *volatile G_bagl_last_touched_not_released_component;
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

#ifndef BUTTON_FAST_THRESHOLD_CS
#define BUTTON_FAST_THRESHOLD_CS 8 // x100MS
#endif                             // BUTTON_FAST_THRESHOLD_CS
#ifndef BUTTON_FAST_ACTION_CS
#define BUTTON_FAST_ACTION_CS 3 // x100MS
#endif                          // BUTTON_FAST_ACTION_CS

typedef unsigned int (*button_push_callback_t)(
    unsigned int button_mask, unsigned int button_mask_counter);
#define BUTTON_LEFT 1
#define BUTTON_RIGHT 2
// flag set when fast threshold is reached and above
#define BUTTON_EVT_FAST 0x40000000UL
#define BUTTON_EVT_RELEASED 0x80000000UL
void io_seproxyhal_button_push(button_push_callback_t button_push_callback,
                               unsigned int new_button_mask);

// hal point (if application has to reprocess elements)
void io_seproxyhal_display(const bagl_element_t *element);

// Helper function that give a realistic timing of scrolling for label with text
// larger than screen
unsigned int bagl_label_roundtrip_duration_ms(const bagl_element_t *e,
                                              unsigned int average_char_width);
unsigned int
bagl_label_roundtrip_duration_ms_buf(const bagl_element_t *e, const char *str,
                                     unsigned int average_char_width);

// default version to be called by ::io_seproxyhal_display if nothing to be done
// by the application
void io_seproxyhal_display_default(const bagl_element_t *element);
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

// only init button handling related variables (not to be done when switching
// screen to avoid the release triggering unwanted behavior)
void io_seproxyhal_init_button(void);

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

// legacy function to send over EP 0x82
void io_usb_send_apdu_data(unsigned char *buffer, unsigned short length);

// trigger a transfer over an usb endpoint and waits for it to occur if timeout
// is != 0
void io_usb_send_ep(unsigned int ep, unsigned char *buffer,
                    unsigned short length, unsigned int timeout);

void io_usb_ccid_reply(unsigned char *buffer, unsigned short length);

#define NO_TIMEOUT (0UL)
// Function that allow applications to modulate the APDU handling timeout
// timeout is defaulty disabled.
// having an APDU handling timeout is useful to solve multiple media
// interactions.
void io_set_timeout(unsigned int timeout);

typedef enum {
    APDU_IDLE,
    APDU_BLE,
    APDU_BLE_WAIT_NOTIFY,
    APDU_NFC_M24SR,
    APDU_NFC_M24SR_SELECT,
    APDU_NFC_M24SR_FIRST,
    APDU_NFC_M24SR_RAPDU,
    APDU_USB_HID,
    APDU_USB_CCID,
    APDU_U2F,
    APDU_RAW,
} io_apdu_state_e;

extern volatile io_apdu_state_e G_io_apdu_state; // by default
extern volatile unsigned short
    G_io_apdu_offset; // total length already received
extern volatile unsigned short G_io_apdu_length; // total length to be received
extern volatile unsigned short G_io_apdu_seq;
extern volatile io_apdu_media_t G_io_apdu_media;

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

#ifdef HAVE_IO_U2F
#include "u2f_service.h"
extern u2f_service_t G_io_u2f;
#endif // HAVE_IO_U2F

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
    unsigned int callback_interval_ms;
    bolos_ux_params_t params;
} ux_state_t;
extern ux_state_t ux;

/**
 * Initialize the user experience structure
 */
#define UX_INIT() os_memset(&ux, 0, sizeof(ux));

/**
 * Setup the status bar foreground and background colors.
 */
#define UX_SET_STATUS_BAR_COLOR(fg, bg)                                        \
    ux.params.ux_id = BOLOS_UX_STATUS_BAR;                                     \
    ux.params.len = 0;                                                         \
    ux.params.u.status_bar.fgcolor = fg;                                       \
    ux.params.u.status_bar.bgcolor = bg;                                       \
    os_ux(&ux.params);

/**
 * Request displaying the next element in the UX structure.
 * Take into account if a seproxyhal status has already been issued.
 * Take into account if the next element is allowed/denied for display by the
 * registered preprocessor
 */
#define UX_DISPLAY_NEXT_ELEMENT()                                              \
    while (ux.elements && ux.elements_current < ux.elements_count &&           \
           !io_seproxyhal_spi_is_status_sent()) {                              \
        const bagl_element_t *element = &ux.elements[ux.elements_current];     \
        if (!ux.elements_preprocessor ||                                       \
            (element = ux.elements_preprocessor(element))) {                   \
            if ((unsigned int)element ==                                       \
                1) { /*backward compat with coding to avoid smashing           \
                        everything*/                                           \
                element = &ux.elements[ux.elements_current];                   \
            }                                                                  \
            io_seproxyhal_display(element);                                    \
        }                                                                      \
        ux.elements_current++;                                                 \
    }

/**
 * Request a wake up of the device (backlight, pin lock screen, ...) to display
 * a new interface to the user.
 * Wake up prevent both autolock and power off features. Therefore, security
 * wise, this function shall only
 * be called to request direct user interaction.
 */
#define UX_WAKE_UP()                                                           \
    ux.params.ux_id = BOLOS_UX_WAKE_UP;                                        \
    ux.params.len = 0;                                                         \
    ux.params.len = os_ux(&ux.params);                                         \
    ux_check_status(ux.params.len);


/**
 * Force redisplay of the screen from the given index in the screen's element
 * array
 */
#define UX_REDISPLAY_IDX(index)                                                \
    io_seproxyhal_init_ux();                                                   \
    ux.elements_current = index;                                               \
    /* REDRAW is redisplay already */                                          \
    if (ux.params.len != BOLOS_UX_IGNORE &&                                    \
        ux.params.len != BOLOS_UX_CONTINUE) {                                  \
        UX_DISPLAY_NEXT_ELEMENT();                                             \
    }

/**
 * Redisplay all elements of the screen
 */
#define UX_REDISPLAY() UX_REDISPLAY_IDX(0)

#define UX_DISPLAY(elements_array, preprocessor)                               \
    ux.elements = elements_array;                                              \
    ux.elements_count = sizeof(elements_array) / sizeof(elements_array[0]);    \
    ux.button_push_handler = elements_array##_button;                          \
    ux.elements_preprocessor = preprocessor;                                   \
    UX_WAKE_UP();                                                              \
    UX_REDISPLAY();

/**
 * Request a screen redisplay after the given milliseconds interval has passed.
 * Interval is not repeated, it's a single shot callback. must be reenabled (the
 * JS way).
 */
#define UX_CALLBACK_SET_INTERVAL(ms) ux.callback_interval_ms = ms;

/**
 * internal bolos ux event processing with callback in case event is to be
 * processed by the application
 */
#define UX_FORWARD_EVENT(callback, ignoring_app_if_ux_busy)                    \
    ux.params.ux_id = BOLOS_UX_EVENT;                                          \
    ux.params.len = 0;                                                         \
    ux.params.len = os_ux(&ux.params);                                         \
    ux_check_status(ux.params.len);                                            \
    if (ux.params.len == BOLOS_UX_REDRAW) {                                    \
        UX_REDISPLAY();                                                        \
    } else if (!ignoring_app_if_ux_busy ||                                     \
               (ux.params.len != BOLOS_UX_IGNORE &&                            \
                ux.params.len != BOLOS_UX_CONTINUE)) {                         \
        callback                                                               \
    }

#define UX_CONTINUE_DISPLAY_APP(displayed_callback)                            \
    UX_DISPLAY_NEXT_ELEMENT();                                                 \
    /* all items have been displayed */                                        \
    if (ux.elements_current >= ux.elements_count &&                            \
        !io_seproxyhal_spi_is_status_sent()) {                                 \
        displayed_callback                                                     \
    }

/**
 * Process display processed event (by the os_ux or by the application code)
 */
#define UX_DISPLAYED_EVENT(displayed_callback)                                 \
    UX_FORWARD_EVENT({ UX_CONTINUE_DISPLAY_APP(displayed_callback); }, 1)

/**
 * Deprecated version to be removed
 */
#define UX_DISPLAYED() (ux.elements_current >= ux.elements_count)

/**
 * Process button push events. Application's button event handler is called only
 * if the ux app does not deny it (modal frame displayed).
 */
#define UX_BUTTON_PUSH_EVENT(seph_packet)                                      \
    UX_FORWARD_EVENT(                                                          \
        {                                                                      \
            if (ux.button_push_handler) {                                      \
                io_seproxyhal_button_push(ux.button_push_handler,              \
                                          seph_packet[3] >> 1);                \
            }                                                                  \
            UX_CONTINUE_DISPLAY_APP({});                                       \
        },                                                                     \
        1);

/**
 * Process finger events. Application's finger event handler is called only if
 * the ux app does not deny it (modal frame displayed).
 */
#define UX_FINGER_EVENT(seph_packet)                                           \
    UX_FORWARD_EVENT(                                                          \
        {                                                                      \
            io_seproxyhal_touch_element_callback(                              \
                ux.elements, ux.elements_count,                                \
                (seph_packet[4] << 8) | (seph_packet[5] & 0xFF),               \
                (seph_packet[6] << 8) | (seph_packet[7] & 0xFF),               \
                seph_packet[3], ux.elements_preprocessor);                     \
            UX_CONTINUE_DISPLAY_APP({});                                       \
        },                                                                     \
        1);

/**
 * forward the ticker_event to the os ux handler. Ticker event callback is
 * always called whatever the return code of the ux app.
 * Ticker event interval is assumed to be 100 ms.
 */
#define UX_TICKER_EVENT(seph_packet, callback)                                 \
    UX_FORWARD_EVENT(                                                          \
        {                                                                      \
            unsigned int UX_ALLOWED = (ux.params.len != BOLOS_UX_IGNORE &&     \
                                       ux.params.len != BOLOS_UX_CONTINUE);    \
            if (ux.callback_interval_ms) {                                     \
                ux.callback_interval_ms -= MIN(ux.callback_interval_ms, 100);  \
                if (!ux.callback_interval_ms) {                                \
                    callback                                                   \
                }                                                              \
            }                                                                  \
            if (UX_ALLOWED) {                                                  \
                UX_CONTINUE_DISPLAY_APP({});                                   \
            }                                                                  \
        },                                                                     \
        0);

/**
 * Forward the event, ignoring the UX return code, the event must therefore be
 * either not processed or processed with extreme care by the application
 * afterwards
 */
#define UX_DEFAULT_EVENT()                                                     \
    UX_FORWARD_EVENT({ UX_CONTINUE_DISPLAY_APP({}); }, 0);

/**
 * Start displaying the system keyboard input to allow. keyboard entry ends when
 * any ux call returns with an OK status.
 */
#define UX_DISPLAY_KEYBOARD(callback)                                          \
    ux.params.ux_id = BOLOS_UX_KEYBOARD;                                       \
    ux.params.len = 0;                                                         \
    ux.params.len = os_ux(&ux.params);                                         \
    ux_check_status(ux.params.len);

/**
 * This function is called after any BOLOS_UX. The goal is to check when a
 * blocking UX call is terminating (it could happen on any event kind).
 */
void ux_check_status(unsigned int status);

/**
 * Setup the TICKER_EVENT interval. Application shall not use this entry point
 * as it's the main ticking source. Use the ::UX_SET_INTERVAL_MS instead.
 */
void io_seproxyhal_setup_ticker(unsigned int interval_ms);

void io_seproxyhal_request_mcu_status(void);

/**
 * Helper function to order the MCU to display the given bitmap with the given
* color index, a table of size: (1<<bit_per_pixel) with little endian encoded
* colors.
* Deprecated
 */
void io_seproxyhal_display_bitmap(int x, int y, unsigned int w, unsigned int h,
                                  unsigned int *color_index,
                                  unsigned int bit_per_pixel,
                                  unsigned char *bitmap);

void io_seproxyhal_power_off(void);

void io_seproxyhal_se_reset(void);

void io_seproxyhal_disable_io(void);

void io_seproxyhal_backlight(unsigned int flags,
                             unsigned int backlight_percentage);

/**
 * helper structure to help handling icons
 */
typedef struct bagl_icon_details_s {
    unsigned int width;
    unsigned int height;
    // bit per pixel
    unsigned int bpp;
    const unsigned int *colors;
    const unsigned char *bitmap;
} bagl_icon_details_t;

/**
 * Helper function to send the given bitmap splitting into multiple DISPLAY_RAW
 * packet as the bitmap is not meant to fit in a single SEPROXYHAL packet.
 */
void io_seproxyhal_display_icon(bagl_component_t *icon_component,
                                bagl_icon_details_t *icon_details);

/**
 * Helper method on the Blue to output icon header to the MCU and allow for
 * bitmap transformation
 */
unsigned int
io_seproxyhal_display_icon_header_and_colors(bagl_component_t *icon_component,
                                             bagl_icon_details_t *icon_details,
                                             unsigned int *icon_len);

// a menu callback is called with a given userid provided within the menu entry
// to allow for fast switch of the action to be taken
typedef void (*ux_menu_callback_t)(unsigned int userid);

typedef struct ux_menu_entry_s ux_menu_entry_t;

/**
 * Menu entry descriptor.
 */
struct ux_menu_entry_s {
    // other menu shown when validated
    const ux_menu_entry_t *menu;
    // callback called when entered (not executed when a menu entry is present)
    ux_menu_callback_t callback;
    // user identifier to allow for indirection in a separated table and
    // mutualise even more menu handling, passed to the given callback is any
    unsigned int userid;
    const bagl_icon_details_t *icon;
    const char *line1;
    const char *line2;
    char text_x;
    char icon_x;
};

#define UX_MENU_END                                                            \
    { NULL, NULL, 0, NULL, NULL, NULL, 0, 0 }

typedef const bagl_element_t *(*ux_menu_preprocessor_t)(
    const ux_menu_entry_t *, bagl_element_t *element);
typedef const ux_menu_entry_t *(*ux_menu_iterator_t)(unsigned int entry_idx);
typedef struct ux_menu_state_s {
    const ux_menu_entry_t *menu_entries;
    unsigned int menu_entries_count;
    unsigned int current_entry;
    ux_menu_preprocessor_t menu_entry_preprocessor;
    ux_menu_iterator_t menu_iterator;
    // temporary menu element for entry layout adjustments
    bagl_element_t tmp_element;
} ux_menu_state_t;
extern ux_menu_state_t ux_menu;

#define UX_MENU_INIT() os_memset(&ux_menu, 0, sizeof(ux_menu));

#define UX_MENU_DISPLAY(current_entry, menu_entries, menu_entry_preprocessor)  \
    ux_menu_display(current_entry, menu_entries, menu_entry_preprocessor);

// if current_entry == -1UL, then don't change the current entry
#define UX_MENU_UNCHANGED_ENTRY (-1UL)
void ux_menu_display(unsigned int current_entry,
                     const ux_menu_entry_t *menu_entries,
                     ux_menu_preprocessor_t menu_entry_preprocessor);
const bagl_element_t *
ux_menu_element_preprocessor(const bagl_element_t *element);
unsigned int ux_menu_elements_button(unsigned int button_mask,
                                     unsigned int button_mask_counter);

// a menu callback is called with a given userid provided within the menu entry
// to allow for fast switch of the action to be taken
typedef void (*ux_turner_callback_t)(void);

typedef struct ux_turner_step_s {
    const bagl_icon_details_t *icon;
    unsigned short fontid1;
    const char *line1;
    unsigned short fontid2;
    const char *line2;
    char text_x;
    char icon_x;
    unsigned int next_step_ms;
} ux_turner_step_t;

typedef struct ux_turner_state_s {
    const ux_turner_step_t *steps;
    unsigned int steps_count;
    unsigned int current_step;
    // temporary menu element for entry layout adjustments
    bagl_element_t tmp_element;
    button_push_callback_t button_callback;
    unsigned int elapsed_ms;
} ux_turner_state_t;
extern ux_turner_state_t ux_turner;

#define UX_TURNER_INIT() os_memset(&ux_turner, 0, sizeof(ux_turner));

#define UX_TURNER_DISPLAY(current_step, steps, steps_count,                    \
                          button_push_callback)                                \
    ux_turner_display(current_step, steps, steps_count, button_push_callback);

// if current_entry == -1UL, then don't change the current entry
#define UX_TURNER_UNCHANGED_ENTRY (-1UL)
void ux_turner_display(unsigned int current_step, const ux_turner_step_t *steps,
                       unsigned int steps_count,
                       button_push_callback_t button_callback);
// function to be called to advance to the next turner step when the programmed
// delay is expired
void ux_turner_ticker(unsigned int elpased_ms);

// avoid typing the size each time
#define SPRINTF(strbuf, ...) snprintf(strbuf, sizeof(strbuf), __VA_ARGS__)

#define ARRAYLEN(array) (sizeof(array) / sizeof(array[0]))
#define INARRAY(elementptr, array)                                             \
    ((unsigned int)elementptr >= (unsigned int)array &&                        \
     (unsigned int)elementptr < ((unsigned int)array) + sizeof(array))

/**
 * Wait until a UX call returns a definitve status. Handle all event packets in
 * between
 */
unsigned int os_ux_blocking(bolos_ux_params_t *params);

#endif // OS_IO_SEPROXYHAL

#endif // OS_IO_SEPROXYHAL_H
