
/*******************************************************************************
 *   Ledger Nano S - Secure firmware
 *   (c) 2022 Ledger
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

#include <string.h>

#include "os_math.h"
#include "os_ux.h"
#include "os_task.h"
#include "nbgl_lns.h"

/* On LNS, NBGL is exposed to the app, but under the hood, the SDK still sends
 * BAGL commands to the MCU
 */

typedef struct bagl_element_e bagl_element_t;

// callback returns NULL when element must not be redrawn (with a changing color or what so ever)
typedef const bagl_element_t *(*bagl_element_callback_t)(const bagl_element_t *element);

// a graphic element is an element with defined text and actions depending on user touches
struct bagl_element_e {
    bagl_component_t component;
    const char      *text;
};

void io_seproxyhal_display_icon(const bagl_component_t    *icon_component,
                                const bagl_icon_details_t *icon_details);

void io_seproxyhal_display_default(const bagl_element_t *element);

// Kept for retro-compatibility
typedef struct {
} ux_state_t;

extern ux_state_t        G_ux;
extern bolos_ux_params_t G_ux_params;

extern void ux_process_button_event(const uint8_t seph_packet[]);
extern void ux_process_ticker_event(void);
extern void ux_process_default_event(void);
extern void ux_process_displayed_event(void);

/**
 * Initialize the user experience structure
 */
#define UX_INIT()

/**
 * Request a wake up of the device (pin lock screen, ...) to display a new interface to the user.
 * Wake up prevents power-off features. Therefore, security wise, this function shall only
 * be called to request direct user interaction.
 */
#define UX_WAKE_UP()                      \
    G_ux_params.ux_id = BOLOS_UX_WAKE_UP; \
    G_ux_params.len   = 0;                \
    os_ux(&G_ux_params);                  \
    G_ux_params.len = os_sched_last_status(TASK_BOLOS_UX);

/**
 * forward the button push/release events to the os ux handler. if not used by it, it will
 * be used by App controls
 */
#define UX_BUTTON_PUSH_EVENT(seph_packet) ux_process_button_event(seph_packet)

/**
 * forward the finger_event to the os ux handler. if not used by it, it will
 * be used by App controls
 */
#define UX_FINGER_EVENT(seph_packet)

/**
 * forward the ticker_event to the os ux handler. Ticker event callback is always called whatever
 * the return code of the ux app. Ticker event interval is assumed to be 100 ms.
 */
#define UX_TICKER_EVENT(seph_packet, callback) ux_process_ticker_event()

/**
 * Forward the event, ignoring the UX return code, the event must therefore be either not processed
 * or processed with extreme care by the application afterwards
 */
#define UX_DEFAULT_EVENT() ux_process_default_event()

/**
 * forward the button push/release events to the os ux handler. if not used by it, it will
 * be used by App controls
 */
#define UX_DISPLAYED_EVENT(...) ux_process_displayed_event()

#include "glyphs.h"
