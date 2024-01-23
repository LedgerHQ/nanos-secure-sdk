#pragma once

#include "ledger_assert_internals.h"

/*****************
 * LEDGER_ASSERT *
 ****************/

/**
 * LEDGER_ASSERT - exit the app if assertion is false.
 *
 * Description:
 * This is a C macro that can be used similarly of the libc `assert` macro.
 * Therefore, it can help programmers find bugs in their programs, or handle
 * exceptional cases via a crash that will produce limited debugging output.
 *
 *
 * Important note:
 * Note that contrary to the libc `assert`, the `LEDGER_ASSERT` macro will
 * still end the app execution even if build with DEBUG=0 or NDEBUG.
 * However, there are configurations explained below that shall be used to
 * reduce the code size impact of the `LEDGER_ASSERT` macro to the bare minimum.
 *
 *
 * Examples of usage:
 *
 * 1/ Simple example always raising with simple message:
 * - `LEDGER_ASSERT(false, "Always assert");`
 *
 * 2/ More complex examples:
 * - `LEDGER_ASSERT(len <= sizeof(buf), "Len too long");`
 * - `LEDGER_ASSERT(check_value(value) == 0, "check_value failed");`
 *
 * 3/ Examples showcasing advance message format:
 * - `LEDGER_ASSERT(len <= sizeof(buf), "Len too long %d <= %d", len, sizeof(buf));`
 * - `int err = check_value(value); LEDGER_ASSERT(err == 0, "check_value failed (%d)", err);`
 *
 *
 * Configuration:
 *
 * I/ Type of info to expose in PRINTF and screen if enabled.
 *
 * There are 4 types of information that can be displayed:
 *
 * 1/ A simple info displaying "LEDGER_ASSERT FAILED"
 *    => This is always enabled
 *
 * 2/ An info containing the PC and LR of the instruction that failed.
 *    This can be used to locate the failing code and flow using the following
 *    command: `arm-none-eabi-addr2line --exe bin/app.elf -pf 0xC0DE586B`
 *    => This is enabled when `LEDGER_ASSERT_CONFIG_LR_AND_PC_INFO` is defined.
 *
 * 3/ An info displaying the `LEDGER_ASSERT` message passed as parameter.
 *    => This is enabled when `LEDGER_ASSERT_CONFIG_MESSAGE_INFO` is defined.
 *
 * 4/ An info displaying the file and line position of the failing
 *    `LEDGER_ASSERT`.
 *    => This is enabled when `LEDGER_ASSERT_CONFIG_FILE_INFO` is defined.
 *
 * By default, when an info level X is enabled, all info level below are enabled.
 *
 * When using `Makefile.standard_app` and building with `DEBUG=1` all info are
 * enabled.
 *
 * Note that enabling these info increase the app code size and is only
 * recommended to ease the debugging.
 *
 *
 * II/ Display info on device screen.
 *
 * Control whether or not a specific screen displaying assert info is shown
 * to the app user before exiting the app due to an assert failure.
 *
 * To enable it, add `DEFINES+=HAVE_LEDGER_ASSERT_DISPLAY` in your app Makefile.
 *
 * This is enabled by default when using `Makefile.standard_app` and building
 * with `DEBUG=1`. It can still be disabled with using
 * `DISABLE_DEBUG_LEDGER_ASSERT=1`.
 *
 * Note that this is increasing the app code size and exposes a non-user
 * friendly screen in case of assert. therefore this should not be enabled in
 * app release build but only kept for debug purposes.
 *
 */
#define LEDGER_ASSERT(test, format, ...)                  \
    do {                                                  \
        if (!(test)) {                                    \
            LEDGER_ASSERT_LR_AND_PC_PREAMBLE();           \
            PRINTF("LEDGER_ASSERT FAILED\n");             \
            LEDGER_ASSERT_MESSAGE(format, ##__VA_ARGS__); \
            LEDGER_ASSERT_FILE_INFO();                    \
            LEDGER_ASSERT_LR_AND_PC();                    \
            LEDGER_ASSERT_EXIT();                         \
        }                                                 \
    } while (0)
