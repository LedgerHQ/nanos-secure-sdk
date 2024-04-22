
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

#include "seproxyhal_protocol.h"
#include "ux.h"
#include "nbgl_buttons.h"
#include "os_io.h"
#ifndef HAVE_BOLOS

/**
 * @brief internal bolos ux event processing
 *
 * @return true if event is to be processed by the application
 */
static bool ux_forward_event(void)
{
    bool app_event = false;

    G_ux_params.ux_id = BOLOS_UX_EVENT;
    G_ux_params.len   = 0;
    os_ux(&G_ux_params);
    G_ux_params.len = os_sched_last_status(TASK_BOLOS_UX);

    switch (G_ux_params.len) {
        case BOLOS_UX_REDRAW: {
            // enable drawing according to UX decision
            nbgl_objAllowDrawing(true);
            nbgl_screenRedraw();
            nbgl_refresh();
            break;
        }
        case BOLOS_UX_IGNORE:
        case BOLOS_UX_CONTINUE: {
            nbgl_objAllowDrawing(false);
            break;
        }
        default: {
            app_event = true;
            nbgl_objAllowDrawing(true);
            break;
        }
    }

    return app_event;
}

/**
 * @brief Process button push event.
 * @note Application's button push/release event handler is called only if the ux app does not deny
 * it (button event caught by BOLOS UX page).
 *
 * @param seph_packet received SEPH packet
 */
void ux_process_button_event(const uint8_t seph_packet[])
{
    bool app_event = ux_forward_event();

    // if the event is not fully consumed by UX, use it for NBGL
    if (app_event) {
        uint8_t buttons_state = seph_packet[3] >> 1;
        nbgl_buttonsHandler(buttons_state);
    }
}

/**
 * @brief Process the ticker_event to the os ux handler. Ticker event callback is always called
 * whatever the return code of the ux app.
 * @note Ticker event interval is assumed to be 100 ms.
 */
void ux_process_ticker_event(void)
{
    // forward to UX
    bool app_event = ux_forward_event();

    // update ticker in NBGL
    nbgl_screenHandler(100);

    if (!app_event) {
        return;
    }

    nbgl_refresh();
}

/**
 * Forwards the event to UX
 */
void ux_process_default_event(void)
{
    // forward to UX
    ux_forward_event();
}

/**
 * Forwards the event to UX
 */

void ux_process_displayed_event(void)
{
    // forward to UX
    bool app_event = ux_forward_event();
    if (app_event) {
        nbgl_processUxDisplayedEvent();
    }
}

#endif  // HAVE_BOLOS
