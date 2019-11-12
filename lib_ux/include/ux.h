
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

#pragma once

#include "bolos_target.h"

#include "os.h"

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
#ifdef TARGET_BLUE
    unsigned char touch_area_brim;
    int overfgcolor;
    int overbgcolor;
    bagl_element_callback_t tap;
    bagl_element_callback_t out;
    bagl_element_callback_t over;
#endif // TARGET_BLUE
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

#include "os_io_seproxyhal.h"

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
#ifdef HAVE_TINY_COROUTINE
    unsigned int return_value; // value replied by an asynch user consent
#endif                         // HAVE_TINY_COROUTINE
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
    ux.params.len = sizeof(G_ux_params.u.status_bar);                          \
    ux.params.u.status_bar.fgcolor = fg;                                       \
    ux.params.u.status_bar.bgcolor = bg;                                       \
    os_ux_blocking(&ux.params);

/**
 * Request displaying the next element in the UX structure.
 * Take into account if a seproxyhal status has already been issued.
 * Take into account if the next element is allowed/denied for display by the
 * registered preprocessor
 */
#define UX_DISPLAY_NEXT_ELEMENT()                                              \
    while (ux.elements && ux.elements_current < ux.elements_count &&           \
           !io_seproxyhal_spi_is_status_sent() && (os_perso_isonboarded() != BOLOS_UX_OK || os_global_pin_is_validated() == BOLOS_UX_OK)) {                              \
        const bagl_element_t *element = &ux.elements[ux.elements_current];     \
        if (!ux.elements_preprocessor ||                                       \
            (element = ux.elements_preprocessor(element))) {                   \
            if ((unsigned int)element == 1) { /*backward compat with coding to \
                                                 avoid smashing everything*/   \
                element = &ux.elements[ux.elements_current];                   \
            }                                                                  \
            io_seproxyhal_display(element);                                    \
        }                                                                      \
        ux.elements_current++;                                                 \
    }

/**
 * Request a wake up of the device (backlight, pin lock screen, ...) to display
 * a new interface to the user. Wake up prevent both autolock and power off
 * features. Therefore, security wise, this function shall only be called to
 * request direct user interaction.
 */
#define UX_WAKE_UP()                                                           \
    ux.params.ux_id = BOLOS_UX_WAKE_UP;                                        \
    ux.params.len = 0;                                                         \
    os_ux(&ux.params);                                         \
    ux.params.len = os_sched_last_status(TASK_BOLOS_UX);


/**
 * Force redisplay of the screen from the given index in the screen's element
 * array
 */
#ifndef HAVE_TINY_COROUTINE
#define UX_REDISPLAY_IDX(index)                                                \
    io_seproxyhal_init_ux();                                                   \
    io_seproxyhal_init_button(); /*ensure to avoid release of a button from a  \
                                    nother screen to mess up with the          \
                                    redisplayed screen */                      \
    ux.elements_current = index;                                               \
    ux.params.len = os_sched_last_status(TASK_BOLOS_UX); \
    /* REDRAW is redisplay already, use os_ux retrun value to check */         \
    if (ux.params.len != BOLOS_UX_IGNORE &&                                    \
        ux.params.len != BOLOS_UX_CONTINUE) {                                  \
        UX_DISPLAY_NEXT_ELEMENT();                                             \
    }
#else // HAVE_TINY_COROUTINE
#define UX_REDISPLAY_IDX(index)                                                \
    io_seproxyhal_init_ux();                                                   \
    io_seproxyhal_init_button();                                               \
    ux.params.len = os_sched_last_status(TASK_BOLOS_UX); \
    if (ux.params.len != BOLOS_UX_IGNORE &&                                    \
        ux.params.len != BOLOS_UX_CONTINUE) {                                  \
        ux.elements_current = index;                                           \
    }
#endif // HAVE_TINY_COROUTINE

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

#define UX_DISPLAY_REQUEST(elements_array, preprocessor)                       \
  ux.elements = elements_array;              \
  ux.elements_count = sizeof(elements_array)/sizeof(elements_array[0]); \
  ux.button_push_handler = elements_array ## _button;              \
  ux.elements_preprocessor = preprocessor;         \
  UX_WAKE_UP();

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
    os_ux(&ux.params);                                         \
    ux.params.len = os_sched_last_status(TASK_BOLOS_UX); \
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
 * Macro to process sequentially display a screen. The call finished when the UX
 * is completely displayed.
 */
#define UX_WAIT_DISPLAYED()                                                    \
    do {                                                                       \
        UX_DISPLAY_NEXT_ELEMENT();                                             \
        io_seproxyhal_spi_recv(G_io_seproxyhal_spi_buffer,                     \
                               sizeof(G_io_seproxyhal_spi_buffer), 0);         \
        io_seproxyhal_handle_event();                                          \
        /* all items have been displayed */                                    \
    } while (!UX_DISPLAYED());

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
#ifdef TARGET_BLUE
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
#else // TARGET_BLUE
#define UX_FINGER_EVENT(seph_packet)
#endif // TARGET_BLUE

/**
 * forward the ticker_event to the os ux handler. Ticker event callback is
 * always called whatever the return code of the ux app. Ticker event interval
 * is assumed to be 100 ms.
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
    os_ux(&ux.params); \
    ux.params.len = os_sched_last_status(TASK_BOLOS_UX);

/**
 * Setup the TICKER_EVENT interval. Application shall not use this entry point
 * as it's the main ticking source. Use the ::UX_SET_INTERVAL_MS instead.
 */
void io_seproxyhal_setup_ticker(unsigned int interval_ms);

void io_seproxyhal_request_mcu_status(void);

/**
 * Helper function to order the MCU to display the given bitmap with the given
 * color index, a table of size: (1<<bit_per_pixel) with little endian encoded
 * colors. Deprecated
 */
void io_seproxyhal_display_bitmap(int x, int y, unsigned int w, unsigned int h,
                                  unsigned int *color_index,
                                  unsigned int bit_per_pixel,
                                  unsigned char *bitmap);

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

#define UX_MENU_END                                                            \
    { NULL, NULL, 0, NULL, NULL, NULL, 0, 0 }

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

#include "glyphs.h"
