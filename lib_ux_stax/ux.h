
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

#if defined(HAVE_BOLOS)
#include "bolos_privileged_ux.h"
#endif  // HAVE_BOLOS

#include "os_math.h"
#include "os_ux.h"
#include "os_task.h"
#include "nbgl_screen.h"
#include "nbgl_touch.h"

#include <string.h>

#define BUTTON_LEFT  1
#define BUTTON_RIGHT 2

typedef void (*asynchmodal_end_callback_t)(unsigned int ux_status);

/**
 * Common structure for applications to perform asynchronous UX aside IO operations
 */
typedef struct ux_state_s ux_state_t;

struct ux_state_s {
    bolos_task_status_t exit_code;
    bool validate_pin_from_dashboard;  // set to true when BOLOS_UX_VALIDATE_PIN is received from
                                       // Dashboard task

    asynchmodal_end_callback_t asynchmodal_end_callback;

    char string_buffer[128];
};

extern ux_state_t G_ux;
#if !defined(APP_UX)
extern bolos_ux_params_t G_ux_params;

extern void ux_process_finger_event(uint8_t seph_packet[]);
extern void ux_process_button_event(uint8_t seph_packet[]);
extern void ux_process_ticker_event(void);
extern void ux_process_default_event(void);
#endif  // !defined(APP_UX)

/**
 * Initialize the user experience structure
 */
#define UX_INIT() nbgl_objInit();

#ifdef HAVE_BOLOS
// to be used only by hal_io.c in BOLOS, for compatibility
#define UX_FORWARD_EVENT_REDRAWCB(bypasspincheck,                         \
                                  ux_params,                              \
                                  ux,                                     \
                                  os_ux,                                  \
                                  os_sched_last_status,                   \
                                  callback,                               \
                                  redraw_cb,                              \
                                  ignoring_app_if_ux_busy)                \
    ux_params.ux_id = BOLOS_UX_EVENT;                                     \
    ux_params.len   = 0;                                                  \
    os_ux(&ux_params);                                                    \
    ux_params.len = os_sched_last_status(TASK_BOLOS_UX);                  \
    if (ux.asynchmodal_end_callback                                       \
        && os_ux_get_status(BOLOS_UX_ASYNCHMODAL_PAIRING_REQUEST) != 0) { \
        asynchmodal_end_callback_t cb = ux.asynchmodal_end_callback;      \
        ux.asynchmodal_end_callback   = NULL;                             \
        cb(os_ux_get_status(BOLOS_UX_ASYNCHMODAL_PAIRING_REQUEST));       \
    }
#endif  // HAVE_BOLOS

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
#ifdef HAVE_SE_TOUCH
#define UX_BUTTON_PUSH_EVENT(seph_packet)
#else  // HAVE_SE_TOUCH
#define UX_BUTTON_PUSH_EVENT(seph_packet) ux_process_button_event(seph_packet)
#endif  // HAVE_SE_TOUCH

/**
 * forward the finger_event to the os ux handler. if not used by it, it will
 * be used by App controls
 */
#ifdef HAVE_SE_TOUCH
#define UX_FINGER_EVENT(seph_packet) ux_process_finger_event(seph_packet)
#else  // HAVE_SE_TOUCH
#define UX_FINGER_EVENT(seph_packet)
#endif  // HAVE_SE_TOUCH

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

// discriminated from io to allow for different memory placement
typedef struct ux_seph_s {
    unsigned int button_mask;
    unsigned int button_same_mask_counter;
#ifdef HAVE_BOLOS
    unsigned int ux_id;
    unsigned int ux_status;
#endif  // HAVE_BOLOS
} ux_seph_os_and_app_t;

#ifdef HAVE_BACKGROUND_IMG
SYSCALL     PERMISSION(APPLICATION_FLAG_BOLOS_UX)
uint8_t    *fetch_background_img(bool allow_candidate);
SYSCALL     PERMISSION(APPLICATION_FLAG_BOLOS_UX)
bolos_err_t delete_background_img(void);
#endif

extern ux_seph_os_and_app_t G_ux_os;

void io_seproxyhal_request_mcu_status(void);
void io_seproxyhal_power_off(bool criticalBattery);

#if defined(HAVE_LANGUAGE_PACK)
const char *get_ux_loc_string(UX_LOC_STRINGS_INDEX index);
void        bolos_ux_select_language(uint16_t language);
void        bolos_ux_refresh_language(void);

typedef struct ux_loc_language_pack_infos {
    unsigned char available;

} UX_LOC_LANGUAGE_PACK_INFO;

// To populate infos about language packs
SYSCALL PERMISSION(APPLICATION_FLAG_BOLOS_UX) void list_language_packs(
    UX_LOC_LANGUAGE_PACK_INFO *packs PLENGTH(NB_LANG * sizeof(UX_LOC_LANGUAGE_PACK_INFO)));
SYSCALL PERMISSION(APPLICATION_FLAG_BOLOS_UX) const LANGUAGE_PACK *get_language_pack(
    unsigned int language);
#endif  // defined(HAVE_LANGUAGE_PACK)

#include "glyphs.h"
