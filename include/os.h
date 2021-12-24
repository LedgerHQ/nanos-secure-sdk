
/*******************************************************************************
*   Ledger Nano S - Secure firmware
*   (c) 2021 Ledger
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

#ifndef OS_H
#define OS_H

#include "bolos_target.h"
#include "os_hal.h"

// FIXME: for backward compatibility. To be removed.
#include "os_debug.h"
#include "os_endorsement.h"
#include "os_halt.h"
#include "os_helpers.h"
#include "os_id.h"
#include "os_io.h"
#include "os_lib.h"
#include "os_math.h"
#include "os_memory.h"
#include "os_nvm.h"
#include "os_pic.h"
#include "os_pin.h"
#include "os_print.h"
#include "os_random.h"
#include "os_registry.h"
#include "os_screen.h"
#include "os_seed.h"
#include "os_settings.h"
#include "os_task.h"
#include "os_utils.h"

// Keep these includes atm.
#include "os_types.h"

/**
 * Quality development guidelines:
 * - NO header defined per arch and included in common if needed per arch,
 * define below
 * - exception model
 * - G_ prefix for RAM vars
 * - N_ prefix for NVRAM vars (mandatory for x86 link script to operate
 * correctly)
 * - C_ prefix for ROM   constants (mandatory for x86 link script to operate
 * correctly)
 * - extensive use of * and arch specific C modifier
 */

#include "os_apilevel.h"

#include "arch.h"

#include "decorators.h"

/* ----------------------------------------------------------------------- */
/* -                            APPLICATION PRIVILEGES                   - */
/* ----------------------------------------------------------------------- */

#include "appflags.h"

/* ----------------------------------------------------------------------- */
/* -                            SYSCALL CRYPTO EXPORT                    - */
/* ----------------------------------------------------------------------- */

#define CXPORT_ED_DES 0x0001UL
#define CXPORT_ED_AES 0x0002UL
#define CXPORT_ED_RSA 0x0004UL

/* ----------------------------------------------------------------------- */
/* -                            ENTRY POINT                              - */
/* ----------------------------------------------------------------------- */

// os entry point
void app_main(void);

// os initialization function to be called by application entry point
void os_boot();

/**
 * Function takes 0 for first call. Returns 0 when timeout has occured. Returned
 * value is passed as argument for next call, acting as a timeout context.
 */
unsigned short io_timeout(unsigned short last_timeout);

/* ----------------------------------------------------------------------- */
/* -                            EXCEPTIONS                               - */
/* ----------------------------------------------------------------------- */

#include "exceptions.h"

/* ----------------------------------------------------------------------- */
/* -                             ERROR CODES                             - */
/* ----------------------------------------------------------------------- */
#include "errors.h"

/**
BOLOS RAM LAYOUT
            msp                          psp                   psp
| bolos ram <-os stack-| bolos ux ram <-ux_stack-| app ram <-app stack-|

ux and app are seen as applications.
os is not an application (it calls ux upon user inputs)
**/

/* ----------------------------------------------------------------------- */
/* -                          DEBUG FUNCTIONS                           - */
/* ----------------------------------------------------------------------- */
#ifdef HAVE_PRINTF
void screen_printf(const char *format, ...);
void mcu_usb_printf(const char *format, ...);
#else // !HAVE_PRINTF
#define PRINTF(...)
#endif // !HAVE_PRINTF

// redefined if string.h not included
#ifdef HAVE_SPRINTF
int snprintf(char *str, size_t str_size, const char *format, ...);
#endif // HAVE_SPRINTF

// syscall test
// SYSCALL void dummy_1(unsigned int* p PLENGTH(2+len+15+ len + 16 +
// sizeof(io_send_t) + 1 ), unsigned int len);

#ifdef BOLOS_DEBUG
#ifdef TARGET_NANOX
SYSCALL void trigger_gpio3(unsigned int val);
#endif // TARGET_NANOX
#endif // BOLOS_DEBUG

/* ----------------------------------------------------------------------- */
/*   -                            I/O I2C                                - */
/* ----------------------------------------------------------------------- */

#ifdef HAVE_IO_I2C

#define IO_I2C_SPEED_STD 0
#define IO_I2C_SPEED_FAST 1
#define IO_I2C_SPEED_FASTPLUS 2
#define IO_I2C_SPEED_HS 3
#define IO_I2C_MASTER 0x80
/**
 * Configure the I2C peripheral.
 * @param speed_and_master enables to set the bus speed. And to select if the
 * peripheral will act as master (issuing Start and Stop condition upon need) or
 * slave mode.
 * @param address In master mode, this parameter sets the target I2C device's
 * address. In slave mode, the address is the desired I2C bus address for the
 * interface. The address is always a 7bit address (excluding the transfer
 * direction bit).
 */
SYSCALL void io_i2c_setmode(unsigned int speed_and_master,
                            unsigned int address);

/**
 * Setup the I2C peripheral for:
 * - In slave mode, receiving a WRITE transaction of maxlength bytes at most.
 * Upon WRITE transaction end, an SEPROXYHAL_TAG_I2C_EVENT is issued with the
 * received data. It has to be received through ::io_seph_recv.
 * - In master mode, this call is nop.
 */
SYSCALL void io_i2c_prepare(unsigned int maxlength);

#define IO_I2C_FLAGS_READ 0
#define IO_I2C_FLAGS_WRITE 1
#define IO_I2C_FLAGS_START 2
#define IO_I2C_FLAGS_STOP 4
/**
 * Request to execute a transfer:
 * - In slave mode, this call is non-blocking. It only enables to reply to a
 * READ transaction of at most length bytes. After the Stop condition is issued
 * from the master, a SEPROXYHAL_TAG_I2C_EVENT event containing the effectively
 * transferred length is issued and can be retrieved through ::io_seph_recv. To
 * restart or continue the transfer requires another call to ::io_i2c_xfer.
 * - In master mode, this call is blocking and triggers the transaction as
 * requested through the flags parameter. The READ or WRITE transaction will
 * place or transmit data from the given buffer and length. Depending on the
 * passed start/stop flags, corresponding bus condition are executed.
 */
SYSCALL void io_i2c_xfer(void *buffer PLENGTH(length), unsigned int length,
                         unsigned int flags);

#ifndef BOLOS_RELEASE
#ifdef BOLOS_DEBUG
SYSCALL void io_i2c_dumpstate(void);
SYSCALL void io_debug(char *chars, unsigned int len);
#endif // BOLOS_DEBUG
#endif // BOLOS_RELEASE

#endif // HAVE_IO_I2C

#ifndef SYSCALL_GENERATE
#include "syscalls.h"
#endif // SYSCALL_GENERATE

#endif // OS_H
