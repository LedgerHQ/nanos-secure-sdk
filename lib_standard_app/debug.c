/*****************************************************************************
 *   (c) 2020 Ledger SAS.
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
 *****************************************************************************/

#include <stdint.h>  // uint*_t
#include <string.h>  // memset, explicit_bzero

#include "os.h"
#include "io.h"

#ifdef HAVE_NBGL
#include "nbgl_use_case.h"
#endif

#ifdef HAVE_DEBUG_THROWS
static char errordata[20];

WEAK void app_throw_info(unsigned int exception, unsigned int lr_val) {
  snprintf(errordata,
           sizeof(errordata),
           "n%d, LR=0x%08X",
           exception,
           lr_val);
}

static void review_choice(bool confirm) {
    UNUSED(confirm);
    os_sched_exit(-1);
}

#ifdef HAVE_BAGL
UX_STEP_CB(ux_error,
           bnnn_paging,
           review_choice(true),
           {
              .title = "App error",
              .text = errordata,
           });
UX_FLOW(ux_error_flow, &ux_error);
#endif

WEAK void __attribute__((noreturn)) debug_display_throw_error(int exception) {
    UNUSED(exception);

#ifdef HAVE_BAGL
    ux_flow_init(0, ux_error_flow, NULL);
#endif

#ifdef HAVE_NBGL
    nbgl_useCaseChoice(&C_round_warning_64px,
                       "App error",
                       errordata,
                       "Exit app",
                       "Exit app",
                       review_choice);
#endif

    // Block until the user approve and the app is quit
    while (1) {
        io_seproxyhal_io_heartbeat();
    }
}

#endif
