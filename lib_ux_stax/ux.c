
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
#include "nbgl_touch.h"
#include "os_io.h"
#ifndef HAVE_BOLOS
// number of 100ms ticks since the start-up of the app
static uint32_t nbTicks;

/**
 * @brief internal bolos ux event processing with callback in case event is to be processed by the application
 *
 * @param ignoring_app_if_ux_busy if set to false, function returns true if no REDRAW needed
 * @return true if ignoring_app_if_ux_busy is false or UX is not busy
 */
static bool ux_forward_event(bool ignoring_app_if_ux_busy) {
  G_ux_params.ux_id = BOLOS_UX_EVENT;
  G_ux_params.len = 0;
  os_ux(&G_ux_params);
  G_ux_params.len = os_sched_last_status(TASK_BOLOS_UX);
#ifdef HAVE_BLE
  if (G_ux.asynchmodal_end_callback &&
      (os_ux_get_status(BOLOS_UX_ASYNCHMODAL_PAIRING_REQUEST) != 0)) {
	asynchmodal_end_callback_t cb = G_ux.asynchmodal_end_callback;
	// reset G_ux.asynchmodal_end_callback for next time
	G_ux.asynchmodal_end_callback = NULL;
	cb(os_ux_get_status(BOLOS_UX_ASYNCHMODAL_PAIRING_REQUEST));
	// for app to redraw/refresh itself
	G_ux_params.len = BOLOS_UX_REDRAW;
  }
#endif // HAVE_BLE
  if (G_ux_params.len == BOLOS_UX_REDRAW) {
    // enable drawing according to UX decision
    nbgl_objAllowDrawing(true);
    nbgl_screenRedraw();
    nbgl_refresh();
  }
  else if (!ignoring_app_if_ux_busy ||
          ((G_ux_params.len != BOLOS_UX_IGNORE) && (G_ux_params.len != BOLOS_UX_CONTINUE))) {
    return true;
  }
  return false;
}

/**
 * @brief Process finger event.
 * @note Application's finger event handler is called only if the ux app does not deny it (finger event caught by BOLOS UX page).
 *
 * @param seph_packet received SEPH packet
 * @param nbTicks number of 100ms ticks from the launch of the app
 */

static nbgl_touchStatePosition_t pos;

void ux_process_finger_event(uint8_t seph_packet[]) {
  bool displayEnabled = ux_forward_event(true);
  // enable/disable drawing according to UX decision
  nbgl_objAllowDrawing(displayEnabled);

  // if the event is not fully consumed by UX, use it for NBGL
  if (displayEnabled) {
    pos.state = (seph_packet[3] == SEPROXYHAL_TAG_FINGER_EVENT_TOUCH) ? PRESSED : RELEASED;
    pos.x = (seph_packet[4] << 8) + seph_packet[5];
    pos.y = (seph_packet[6] << 8) + seph_packet[7];
    nbgl_touchHandler(&pos, nbTicks * 100);
    nbgl_refresh();
  }
}

/**
 * @brief Process the ticker_event to the os ux handler. Ticker event callback is always called whatever the return code of the ux app.
 * @note Ticker event interval is assumed to be 100 ms.
 */
void ux_process_ticker_event(void) {
  nbTicks++;
  // forward to UX
  bool displayEnabled = ux_forward_event(true);

  // enable/disable drawing according to UX decision
  nbgl_objAllowDrawing(displayEnabled);
  // update ticker in NBGL
  nbgl_screenHandler(100);

  if (!displayEnabled) {
    return;
  }

  // handle touch only if detected as pressed in last touch message
  if (pos.state == PRESSED) {
    io_touch_info_t touch_info;
    touch_get_last_info(&touch_info);
    pos.state = (touch_info.state == SEPROXYHAL_TAG_FINGER_EVENT_TOUCH)? PRESSED : RELEASED;
    pos.x = touch_info.x;
    pos.y = touch_info.y;
    // Send current touch position to nbgl
    nbgl_touchHandler(&pos, nbTicks * 100);
  }
  nbgl_refresh();
}

/**
 * Forwards the event to UX
 */
void ux_process_default_event(void) {
  // forward to UX
  ux_forward_event(false);
}

#endif // HAVE_BOLOS
