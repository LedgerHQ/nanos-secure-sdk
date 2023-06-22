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

ux_state_t G_ux;
bolos_ux_params_t G_ux_params;

/**
 * Exit the application and go back to the dashboard.
 */
WEAK void __attribute__((noreturn)) app_exit(void) {
    os_sched_exit(-1);
}

/**
 * Main loop to setup USB, Bluetooth, UI and launch app_main().
 */
WEAK __attribute__((section(".boot"))) int main() {
    __asm volatile("cpsie i");

    os_boot();

    BEGIN_TRY {
        TRY {
            UX_INIT();

            io_seproxyhal_init();

#ifdef HAVE_BLE
            G_io_app.plane_mode = os_setting_get(OS_SETTING_PLANEMODE, NULL, 0);
#endif // HAVE_BLE
            USB_power(0);
            USB_power(1);

#ifdef HAVE_BLE
            BLE_power(0, NULL);
            BLE_power(1, NULL);
#endif // HAVE_BLE

            app_main();
        }
        CATCH_OTHER(e) {
            PRINTF("Exiting following exception: %d\n", e);
        }
        FINALLY {
        }
    }
    END_TRY;

    // Exit the application and go back to the dashboard.
    app_exit();

    return 0;
}
