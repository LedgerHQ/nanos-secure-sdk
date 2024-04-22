/*******************************************************************************
 *   (c) 2023 Ledger
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

#include <string.h>

#include "ledger_assert.h"
#include "os.h"
#if defined(HAVE_BAGL) || defined(HAVE_NBGL)
#include "ux.h"
#endif
#include "os_io_seproxyhal.h"

#ifdef HAVE_NBGL
#include "nbgl_use_case.h"
#endif

#if defined(HAVE_LEDGER_ASSERT_DISPLAY) || defined(HAVE_DEBUG_THROWS)
#ifndef ASSERT_BUFFER_LEN
#define ASSERT_BUFFER_LEN 24
#endif

char assert_buffer[ASSERT_BUFFER_LEN];
#endif

/************************************************************
 * Define behavior when LEDGER_ASSERT_CONFIG_LR_AND_PC_INFO *
 ***********************************************************/
#ifdef LEDGER_ASSERT_CONFIG_LR_AND_PC_INFO
#ifdef HAVE_LEDGER_ASSERT_DISPLAY
void assert_display_lr_and_pc(int lr, int pc)
{
    char buff[LR_AND_PC_SIZE];

    lr = compute_address_location(lr);
    pc = compute_address_location(pc);
    snprintf(buff, LR_AND_PC_SIZE, "LR=0x%08X\n PC=0x%08X\n", lr, pc);
    strncat(assert_buffer, buff, LR_AND_PC_SIZE);
}
#endif

#ifdef HAVE_PRINTF
void assert_print_lr_and_pc(int lr, int pc)
{
    lr = compute_address_location(lr);
    pc = compute_address_location(pc);
    PRINTF("=> LR: 0x%08X \n", lr);
    PRINTF("=> PC: 0x%08X \n", pc);
}
#endif
#endif  // LEDGER_ASSERT_CONFIG_LR_AND_PC_INFO

/*******************************************************
 * Define behavior when LEDGER_ASSERT_CONFIG_FILE_INFO *
 ******************************************************/
#ifdef LEDGER_ASSERT_CONFIG_FILE_INFO
#ifdef HAVE_LEDGER_ASSERT_DISPLAY
void assert_display_file_info(const char *file, unsigned int line)
{
    char buff[FILE_SIZE];

    snprintf(buff, FILE_SIZE, "%s::%d\n", file, line);
    strncat(assert_buffer, buff, FILE_SIZE);
}
#endif

#ifdef HAVE_PRINTF
void assert_print_file_info(const char *file, int line)
{
    PRINTF("%s::%d \n", file, line);
}
#endif
#endif  // LEDGER_ASSERT_CONFIG_FILE_INFO

/*************************************
 * Specific mechanism to debug THROW *
 ************************************/
#ifdef HAVE_DEBUG_THROWS
void throw_display_lr(int e, int lr)
{
    lr = compute_address_location(lr);
    snprintf(assert_buffer, ASSERT_BUFFER_LEN, "e=0x%04X\n LR=0x%08X\n", e, lr);
}

#ifdef HAVE_PRINTF
void throw_print_lr(int e, int lr)
{
    lr = compute_address_location(lr);
    PRINTF("exception[0x%04X]: LR=0x%08X\n", e, lr);
}
#endif
#endif  // HAVE_DEBUG_THROWS

/*******************
 * Common app exit *
 ******************/
void assert_exit(bool confirm)
{
    UNUSED(confirm);
    os_sched_exit(-1);
}

/************************************
 * Display info on screen mechanism *
 ***********************************/
#if defined(HAVE_LEDGER_ASSERT_DISPLAY) || defined(HAVE_DEBUG_THROWS)
#ifdef HAVE_BAGL
UX_STEP_CB(ux_error,
           bnnn_paging,
           assert_exit(true),
           {
               .title = "App error",
               .text  = assert_buffer,
           });
UX_FLOW(ux_error_flow, &ux_error);
#endif

void __attribute__((noreturn)) assert_display_exit(void)
{
#ifdef HAVE_BAGL
    ux_flow_init(0, ux_error_flow, NULL);
#endif

#ifdef HAVE_NBGL
#if defined(TARGET_NANOS) || defined(TARGET_NANOX) || defined(TARGET_NANOS2)
#define ICON_APP_WARNING C_icon_warning
#elif defined(TARGET_STAX) || defined(TARGET_FLEX)
#define ICON_APP_WARNING C_round_warning_64px
#endif

    nbgl_useCaseChoice(
        &ICON_APP_WARNING, "App error", assert_buffer, "Exit app", "Exit app", assert_exit);
#endif

    // Block until the user approve and the app is quit
    while (1) {
        io_seproxyhal_io_heartbeat();
    }
}
#endif
