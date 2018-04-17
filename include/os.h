/*******************************************************************************
*   Ledger Nano S - Secure firmware
*   (c) 2016, 2017, 2018 Ledger
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

// -----------------------------------------------------------------------
// - BASIC MATHS
// -----------------------------------------------------------------------
#define U2(hi, lo) ((((hi)&0xFF) << 8) | ((lo)&0xFF))
#define U4(hi3, hi2, lo1, lo0)                                                 \
    ((((hi3)&0xFF) << 24) | (((hi2)&0xFF) << 16) | (((lo1)&0xFF) << 8) |       \
     ((lo0)&0xFF))
#define U2BE(buf, off) ((((buf)[off] & 0xFF) << 8) | ((buf)[off + 1] & 0xFF))
#define U2LE(buf, off) ((((buf)[off + 1] & 0xFF) << 8) | ((buf)[off] & 0xFF))
#define U4BE(buf, off) ((U2BE(buf, off) << 16) | (U2BE(buf, off + 2) & 0xFFFF))
#define U4LE(buf, off) ((U2LE(buf, off + 2) << 16) | (U2LE(buf, off) & 0xFFFF))
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define IS_POW2(x) (((x) & ((x)-1)) == 0)

#ifdef macro_offsetof
#define offsetof(type, field) ((unsigned int)&(((type *)NULL)->field))
#endif

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

// error type definition
typedef unsigned short exception_t;

// Arch definitions

//#define macro_offsetof // already defined in stddef.h
#define OS_LITTLE_ENDIAN
#define NATIVE_64BITS

#define WIDE // const // don't !!
#define WIDE_AS_INT unsigned long int
#define REENTRANT(x) x //

//#include <setjmp.h>
// GCC/LLVM declare way too big jmp context, reduce them to what is used on CM0+
typedef struct try_context_s try_context_t;

typedef unsigned int jmp_buf[10];

struct try_context_s {
    // jmp context to backup (in increasing order address: r4, r5, r6, r7, r8,
    // r9, r10, r11, SP, setjmpcallPC)
    jmp_buf jmp_buf;

    // link to the previous jmp_buf context
    // in r9 // try_context_t* previous;

    // current exception
    exception_t ex;
};

// borrowed from setjmp.h

#ifdef __GNUC__
void longjmp(jmp_buf __jmpb, int __retval) __attribute__((__noreturn__));
#else
void longjmp(jmp_buf __jmpb, int __retval);
#endif
int setjmp(jmp_buf __jmpb);

#include "stddef.h"
#include "stdint.h"

#define UNUSED(x) (void)x


#include "os_apilevel.h"

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef WIDE_NULL
#define WIDE_NULL ((void WIDE *)0)
#endif

// Placement Independence Code reference
// function that align the deferenced value in a rom struct to use it depending
// on the execution address
// can be used even if code is executing at the same place where it had been
// linked
#ifndef PIC
#define PIC(x) pic((unsigned int)x)
unsigned int pic(unsigned int linked_address);
#endif

#ifndef SYSCALL
// #define SYSCALL syscall
#define SYSCALL
#endif

#ifndef TASKSWITCH
// #define TASKSWITCH taskswitch
#define TASKSWITCH
#endif

#ifndef LIBCALL
// #define LIBCALL libcall
#define LIBCALL
#endif

#ifndef SHARED
// #define SHARED shared
#define SHARED
#endif

#ifndef PERMISSION
#define PERMISSION(name)
#endif

#ifndef PLENGTH
#define PLENGTH(len)
#endif

#ifndef CXPORT
#define CXPORT(name)
#endif

/* ----------------------------------------------------------------------- */
/* -                            APPLICATION PRIVILEGES                   - */
/* ----------------------------------------------------------------------- */

/**
 * Application has been loaded using a secure channel opened using the
 * bootloader's issuer
 * public key. This application is ledger legit.
 */
#define APPLICATION_FLAG_ISSUER 0x1

/**
 * Flag which combined with ::APPLICATION_FLAG_ISSUER.
 * The application is given full nvram access after the global seed has been
 * destroyed.
 */
#define APPLICATION_FLAG_BOLOS_UPGRADE 0x2

// this flag is set when a valid signature of the loaded application is
// presented at the end of the bolos application load.
#define APPLICATION_FLAG_SIGNED 0x4

// must be set on one application in the registry which is used
#define APPLICATION_FLAG_BOLOS_UX 0x8

// application is allowed to use the raw master seed, if not set, at least a
// level of derivation is required.
#define APPLICATION_FLAG_DERIVE_MASTER 0x10

#define APPLICATION_FLAG_SHARED_NVRAM 0x20
#define APPLICATION_FLAG_GLOBAL_PIN 0x40

// This flag means the application is meant to be debugged and allows for dump
// or core ARM register in
// case of a fault detection
#define APPLICATION_FLAG_DEBUG 0x80

/**
 * Mark this application as defaultly booting along with the bootloader (no
 * application menu displayed)
 * Only one application can have this at a time. It is managed by the bootloader
 * interface.
 */
#define APPLICATION_FLAG_AUTOBOOT 0x100

/**
 * Application is allowed to change the settings
 */
#define APPLICATION_FLAG_BOLOS_SETTINGS 0x200

#define APPLICATION_FLAG_CUSTOM_CA 0x400

/**
 * The application main can be called in two ways:
 *  - with first arg (stored in r0) set to 0: The application is called from the
 * dashboard
 *  - with first arg (stored in r0) set to != 0 (ram address likely): The
 * application is used as a library from another app.
 */
#define APPLICATION_FLAG_LIBRARY 0x800

/**
 * The application won't be shown on the dashboard (somewhat reasonable for pure
 * library)
 */
#define APPLICATION_FLAG_NO_RUN 0x1000

/**
 * Application is disabled (during its upgrade or whatever)
 */

//#define APPLICATION_FLAG_DISABLED         0x8000
#define APPLICATION_FLAG_ENABLED 0x8000

#define APPLICATION_FLAG_NEG_MASK 0xFFFF0000UL

/* ----------------------------------------------------------------------- */
/* -                            SYSCALL CRYPTO EXPORT                    - */
/* ----------------------------------------------------------------------- */

#define CXPORT_ED_DES 0x0001UL
#define CXPORT_ED_AES 0x0002UL
#define CXPORT_ED_RSA 0x0004UL

/* ----------------------------------------------------------------------- */
/* -                            TYPES                                    - */
/* ----------------------------------------------------------------------- */

/* ----------------------------------------------------------------------- */
/* -                            GLOBALS                                  - */
/* ----------------------------------------------------------------------- */

// the global apdu buffer
#ifdef HAVE_IO_U2F
#define IMPL_IO_APDU_BUFFER_SIZE (3 + 32 + 32 + 15 + 255)
#else
#define IMPL_IO_APDU_BUFFER_SIZE (5 + 255)
#endif

#ifdef CUSTOM_IO_APDU_BUFFER_SIZE
#define IO_APDU_BUFFER_SIZE                                                    \
    MAX(IMPL_IO_APDU_BUFFER_SIZE, CUSTOM_IO_APDU_BUFFER_SIZE)
#else
#define IO_APDU_BUFFER_SIZE IMPL_IO_APDU_BUFFER_SIZE
#endif
extern unsigned char G_io_apdu_buffer[IO_APDU_BUFFER_SIZE];

#define CUSTOMCA_MAXLEN 64

/* ----------------------------------------------------------------------- */
/* -                            ENTRY POINT                              - */
/* ----------------------------------------------------------------------- */

// os entry point
void app_main(void);

// os initialization function to be called by application entry point
void os_boot();

/* ----------------------------------------------------------------------- */
/* -                            OS FUNCTIONS                             - */
/* ----------------------------------------------------------------------- */
#define os_swap_u16(u16)                                                       \
    ((((unsigned short)(u16) << 8) & 0xFF00U) |                                \
     (((unsigned short)(u16) >> 8) & 0x00FFU))

#define os_swap_u32(u32)                                                       \
    (((unsigned long int)(u32) >> 24) |                                        \
     (((unsigned long int)(u32) << 8) & 0x00FF0000UL) |                        \
     (((unsigned long int)(u32) >> 8) & 0x0000FF00UL) |                        \
     ((unsigned long int)(u32) << 24))

REENTRANT(void os_memmove(void *dst, const void WIDE *src,
                          unsigned int length));
#define os_memcpy os_memmove

void os_memset(void *dst, unsigned char c, unsigned int length);

char os_memcmp(const void WIDE *buf1, const void WIDE *buf2,
               unsigned int length);

void os_xor(void *dst, void WIDE *src1, void WIDE *src2, unsigned int length);

// Secure memory comparison
char os_secure_memcmp(void WIDE *src1, void WIDE *src2, unsigned int length);

// patch point, address used to dispatch, no index
REENTRANT(void patch(void));

// check API level
SYSCALL void check_api_level(unsigned int apiLevel);

// reset the chip
SYSCALL REENTRANT(void reset(void));

// send tx_len bytes (atr or rapdu) and retrieve the length of the next command
// apdu (over the requested channel)
#define CHANNEL_APDU 0
#define CHANNEL_KEYBOARD 1
#define CHANNEL_SPI 2
#define IO_RESET_AFTER_REPLIED 0x80
#define IO_RECEIVE_DATA 0x40
#define IO_RETURN_AFTER_TX 0x20
#define IO_ASYNCH_REPLY                                                        \
    0x10 // avoid apdu state reset if tx_len == 0 when we're expected to reply
#define IO_FLAGS 0xF0
unsigned short io_exchange(unsigned char channel_and_flags,
                           unsigned short tx_len);

typedef enum {
    IO_APDU_MEDIA_NONE = 0, // not correctly in an apdu exchange
    IO_APDU_MEDIA_USB_HID = 1,
    IO_APDU_MEDIA_BLE,
    IO_APDU_MEDIA_NFC,
    IO_APDU_MEDIA_USB_CCID,
    IO_APDU_MEDIA_RAW,
    IO_APDU_MEDIA_U2F,
} io_apdu_media_t;

extern volatile io_apdu_media_t G_io_apdu_media;

extern unsigned int usb_ep_xfer_len[7];

#ifndef USB_SEGMENT_SIZE
#ifdef IO_HID_EP_LENGTH
#define USB_SEGMENT_SIZE IO_HID_EP_LENGTH
#else
#error IO_HID_EP_LENGTH and USB_SEGMENT_SIZE not defined
#endif
#endif
#ifndef BLE_SEGMENT_SIZE
#define BLE_SEGMENT_SIZE USB_SEGMENT_SIZE
#endif

// common usb endpoint buffer
extern unsigned char
    G_io_usb_ep_buffer[MAX(USB_SEGMENT_SIZE, BLE_SEGMENT_SIZE)];

/**
 * Return 1 when the event has been processed, 0 else
 */
// io callback in the application called when an interrupt based channel has
// received data to be processed
unsigned char io_event(unsigned char channel);

/**
 * Function takes 0 for first call. Returns 0 when timeout has occured. Returned
 * value is passed as argument for next call, acting as a timeout context.
 */
unsigned short io_timeout(unsigned short last_timeout);
// write in persistent memory, to make things easy keep a layout of the memory
// in a structure and update fields upon needs
// NOTE: accept copy from far memory to another far memory.
// @param src_adr NULL to fill with 00's
SYSCALL void nvm_write(void WIDE *dst_adr PLENGTH(src_len),
                       void WIDE *src_adr PLENGTH(src_len),
                       unsigned int src_len);

// program a page with the content of the nvm_page_buffer
// HAL for the high level NVM management functions
void nvm_write_page(unsigned char WIDE *page_adr);

/* ----------------------------------------------------------------------- */
/* -                            EXCEPTIONS                               - */
/* ----------------------------------------------------------------------- */

// workaround to make sure defines are replaced by their value for example
#define CPP_CONCAT(x, y) CPP_CONCAT_x(x, y)
#define CPP_CONCAT_x(x, y) x##y

// -----------------------------------------------------------------------
// - BEGIN TRY
// -----------------------------------------------------------------------

#define BEGIN_TRY_L(L)                                                         \
    {                                                                          \
        try_context_t __try##L;

try_context_t *try_context_get(void);
try_context_t *try_context_get_previous(void);
void try_context_set(try_context_t *context);

// -----------------------------------------------------------------------
// - TRY
// -----------------------------------------------------------------------
#define TRY_L(L)                                                               \
    /* previous exception context chain is saved within the setjmp r9 save */  \
    __try                                                                      \
        ##L.ex = setjmp(__try##L.jmp_buf);                                     \
    if (__try##L.ex == 0) {                                                    \
        try_context_set(&__try##L);

// -----------------------------------------------------------------------
// - EXCEPTION CATCH
// -----------------------------------------------------------------------
#define CATCH_L(L, x)                                                          \
    goto CPP_CONCAT(__FINALLY, L);                                             \
    }                                                                          \
    else if (__try##L.ex == x) {                                               \
        __try                                                                  \
            ##L.ex = 0;

// -----------------------------------------------------------------------
// - EXCEPTION CATCH OTHER
// -----------------------------------------------------------------------
#define CATCH_OTHER_L(L, e)                                                    \
    goto CPP_CONCAT(__FINALLY, L);                                             \
    }                                                                          \
    else {                                                                     \
        exception_t e;                                                         \
        e = __try##L.ex;                                                       \
        __try                                                                  \
            ##L.ex = 0;

// -----------------------------------------------------------------------
// - EXCEPTION CATCH ALL
// -----------------------------------------------------------------------
#define CATCH_ALL_L(L)                                                         \
    goto CPP_CONCAT(__FINALLY, L);                                             \
    }                                                                          \
    else {                                                                     \
        __try                                                                  \
            ##L.ex = 0;

// -----------------------------------------------------------------------
// - FINALLY
// -----------------------------------------------------------------------
#define FINALLY_L(L)                                                           \
    goto CPP_CONCAT(__FINALLY, L);                                             \
    }                                                                          \
    CPP_CONCAT(__FINALLY, L)                                                   \
        : /* has TRY clause ended without nested throw ? */                    \
          if (try_context_get() == &__try##L) {                                \
        /* restore previous context manually (as a throw would have) */        \
        try_context_set(try_context_get_previous());                           \
    }

// -----------------------------------------------------------------------
// - END TRY
// -----------------------------------------------------------------------
#define END_TRY_L(L)                                                           \
    /* nested throw not consumed ? (by CATCH* clause) */                       \
    if (__try##L.ex != 0) {                                                    \
        /* rethrow */                                                          \
        THROW_L(L, __try##L.ex);                                               \
    }                                                                          \
    }

// -----------------------------------------------------------------------
// - CLOSE TRY
// -----------------------------------------------------------------------
/**
 * Forced finally like clause.
 */
#define CLOSE_TRY_L(L) try_context_set(try_context_get_previous())

// -----------------------------------------------------------------------
// - EXCEPTION THROW
// -----------------------------------------------------------------------

// longjmp is marked as no return to avoid too much generated code
void os_longjmp(unsigned int exception) __attribute__((noreturn));
#define THROW_L(L, x) os_longjmp(x)

// Default macros when nesting is not used.
#define THROW(x) THROW_L(EX, x)
#define BEGIN_TRY BEGIN_TRY_L(EX)
#define TRY TRY_L(EX)
#define CATCH(x) CATCH_L(EX, x)
#define CATCH_OTHER(e) CATCH_OTHER_L(EX, e)
#define CATCH_ALL CATCH_ALL_L(EX)
#define FINALLY FINALLY_L(EX)
#define CLOSE_TRY CLOSE_TRY_L(EX)
#define END_TRY END_TRY_L(EX)

#define EXCEPTION 1
#define INVALID_PARAMETER 2
#define EXCEPTION_OVERFLOW 3
#define EXCEPTION_SECURITY 4
#define INVALID_CRC 5
#define INVALID_CHECKSUM 6
#define INVALID_COUNTER 7
#define NOT_SUPPORTED 8
#define INVALID_STATE 9
#define TIMEOUT 10
#define EXCEPTION_PIC 11
#define EXCEPTION_APPEXIT 12
#define EXCEPTION_IO_OVERFLOW 13
#define EXCEPTION_IO_HEADER 14
#define EXCEPTION_IO_STATE 15
#define EXCEPTION_IO_RESET 16
#define EXCEPTION_CXPORT 17
#define EXCEPTION_SYSTEM 18

/* ----------------------------------------------------------------------- */
/* -                          CRYPTO FUNCTIONS                           - */
/* ----------------------------------------------------------------------- */
#include "cx.h"

/**
 BOLOS RAM LAYOUT
                msp                          psp                   psp
 | bolos ram <-os stack-| bolos ux ram <-ux_stack-| app ram <-app stack-|

 ux and app are seen as applications.
 os is not an application (it calls ux upon user inputs)
**/

/* ----------------------------------------------------------------------- */
/* -                        BOLOS UX DEFINITIONS                         - */
/* ----------------------------------------------------------------------- */
typedef enum bolos_ux_e {
    BOLOS_UX_INITIALIZE =
        0, // tag to be processed by the UX from the serial line

    BOLOS_UX_EVENT, // tag to be processed by the UX from the serial line
    BOLOS_UX_KEYBOARD,
    BOLOS_UX_WAKE_UP, // the application/os asks for the screen to be waked up
                      // (asking pin if the lock period has been exceeded)
    BOLOS_UX_STATUS_BAR,

    BOLOS_UX_BOOT, // will never be presented to loaded UX app, this is for
                   // failsafe UX only
    BOLOS_UX_BOOT_NOT_PERSONALIZED,
    BOLOS_UX_BOOT_ONBOARDING,
    BOLOS_UX_BOOT_UNSAFE_WIPE,
    BOLOS_UX_BOOT_UX_NOT_SIGNED,
    BOLOS_UX_BOLOS_START, // called in before dashboard displays
    BOLOS_UX_DASHBOARD,
    BOLOS_UX_PROCESSING,

    BOLOS_UX_LOADER, // display loader screen or advance it

    BOLOS_UX_VALIDATE_PIN,
    BOLOS_UX_CONSENT_UPGRADE,
    BOLOS_UX_CONSENT_APP_ADD,
    BOLOS_UX_CONSENT_APP_DEL,
    BOLOS_UX_CONSENT_APP_UPG,
    BOLOS_UX_CONSENT_ISSUER_KEY,
    BOLOS_UX_CONSENT_CUSTOMCA_KEY,
    BOLOS_UX_CONSENT_FOREIGN_KEY,
    BOLOS_UX_CONSENT_GET_DEVICE_NAME,
    BOLOS_UX_CONSENT_SET_DEVICE_NAME,
    BOLOS_UX_CONSENT_RESET_CUSTOMCA_KEY,
    BOLOS_UX_CONSENT_SETUP_CUSTOMCA_KEY,
    BOLOS_UX_APP_ACTIVITY, // special code when application is processing data
                           // but not displaying UI. It prevent going poweroff,
                           // while not avoiding screen locking
    BOLOS_UX_CONSENT_NOT_INTERACTIVE_ONBOARD,
    BOLOS_UX_PREPARE_RUN_APP, // called before the os runs an application, the
                              // os ux must be cleared and not display anything
                              // on upcoming display processed events

    BOLOS_UX_MCU_UPGRADE_REQUIRED,
    BOLOS_UX_CONSENT_RUN_APP,

} bolos_ux_t;

#define BOLOS_UX_OK 0xB0105011
#define BOLOS_UX_CANCEL 0xB0105022
#define BOLOS_UX_ERROR 0xB0105033

/* Value returned by os_ux to notify the application that the processed event
 * must be discarded and not processed by the application. Generally due to
 * handling of power management/dim/locking */
#define BOLOS_UX_IGNORE 0xB0105044
// a modal has destroyed the display, app needs to redraw its screen
#define BOLOS_UX_REDRAW 0xB0105055
// ux has not finished processing yet (not a final status)
#define BOLOS_UX_CONTINUE 0

/* ----------------------------------------------------------------------- */
/* -                       APPLICATION FUNCTIONS                         - */
/* ----------------------------------------------------------------------- */

typedef void (*appmain_t)(void);

#define BOLOS_TAG_APPNAME 0x01
#define BOLOS_TAG_APPVERSION 0x02
#define BOLOS_TAG_ICON 0x03
#define BOLOS_TAG_DERIVEPATH 0x04
#define BOLOS_TAG_DATA_SIZE                                                    \
    0x05 // meta tag to retrieve the size of the data section for the
         // application
// Library Dependencies are a tuple of 2 LV, the lib appname and the lib version
// (only exact for now). When lib version is not specified, it is not check,
// only name is asserted
// The DEPENDENCY tag may have several occurences, one for each dependency (by
// name). Malformed (multiple dep to the same lib with different version is is
// not ORed but ANDed, and then considered bogus)
#define BOLOS_TAG_DEPENDENCY 0x06
// first autorised tag value for user data
#define BOLOS_TAG_USER_TAG 0x20

// application slot description
typedef struct application_s {
    // nvram start address for this application (to check overlap when loading,
    // and mpu lock)
    unsigned char *nvram_begin;
    // nvram stop address (exclusive) for this application (to check overlap
    // when loading, and mpu lock)
    unsigned char *nvram_end;

    // address of the main address, must be set according to BLX spec ( ORed
    // with 1 when jumping into Thumb code
    appmain_t main;

    // special flags for this application
    unsigned int flags;

    // Memory organization: [ code (RX) |alignpage| data (RW) |alignpage|
    // install params (R) ]

    // length of the code section of the application (RX)
    unsigned int code_length;

    // NOTE: code_length+params_length must be a multiple of PAGE_SIZE
    // Length of the DATA section of the application. (RW)
    unsigned int data_length;

    // NOTE: code_length+params_length must be a multiple of PAGE_SIZE
    // length of the parameters sections of the application (R)
    unsigned int params_length;

    // Intermediate hash of the application's loaded code and data segments
    unsigned char sha256_code_data[32];
    // Hash of the application's loaded code, data and instantiation parameters
    unsigned char sha256_full[32];

} application_t;

// Structure that defines the parameters to exchange with the BOLOS UX
// application
typedef struct bolos_ux_params_s {
    bolos_ux_t ux_id;
    // length of parameters in the u union to be copied during the syscall
    unsigned int len;

    // structure to overlay parameters for each BOLOS_UX
    union {
        struct {
            unsigned int currently_onboarded;
            unsigned char hash[32];
        } boot_unsafe;

        struct {
            unsigned int app_idx;
        } appexitb;

        struct {
            unsigned int app_idx;
            application_t appentry;
        } appdel;

        struct {
            unsigned int app_idx;
            application_t appentry;
        } appadd;

        // pass the whole load application to that the os version and hash are
        // available
        struct {
            unsigned int app_idx;
            application_t upgrade;
        } upgrade;

        struct {
            application_t ux_app;
        } ux_not_signed;

        struct {
            unsigned int app_idx;
            application_t app;
        } run_app;

        struct {
            unsigned char name[CUSTOMCA_MAXLEN + 1];
            cx_ecfp_public_key_t public;
        } customca_key;

        struct {
            cx_ecfp_public_key_t host_pubkey;
        } foreign_key;

        struct {
            unsigned char name[CUSTOMCA_MAXLEN + 1];
            cx_ecfp_public_key_t public;
        } reset_customca;

        struct {
            unsigned char name[CUSTOMCA_MAXLEN + 1];
            cx_ecfp_public_key_t public;
        } setup_customca;

        struct {
            unsigned int keycode;
#define BOLOS_UX_MODE_UPPERCASE 0
#define BOLOS_UX_MODE_LOWERCASE 1
#define BOLOS_UX_MODE_SYMBOLS 2
#define BOLOS_UX_MODE_COUNT 3 // number of keyboard modes
            unsigned int mode;
// + 1 EOS (0)
#define BOLOS_UX_KEYBOARD_TEXT_BUFFER_SIZE 32
            unsigned char entered_text[BOLOS_UX_KEYBOARD_TEXT_BUFFER_SIZE + 1];
        } keyboard;

        struct {
            unsigned int cancellable;
        } validate_pin;

        struct {
            unsigned int keycode;
        } pin_keyboard;

        struct {
            unsigned int fgcolor;
            unsigned int bgcolor;
        } status_bar;

        struct {
            unsigned int x;
            unsigned int y;
            unsigned int width;
            unsigned int height;
        } loader;

        struct {
            unsigned int id;
        } onboard;

    } u;

} bolos_ux_params_t;

// any application can wipe the global pin, global seed, user's keys
// disabled for security reasons // SYSCALL void           os_perso_wipe(void);
// erase seed, settings AND applications
SYSCALL void os_perso_erase_all(void);

/* set_pin can update the pin if the perso is onboarded (tearing leads to perso
 * wipe though) */
SYSCALL PERMISSION(APPLICATION_FLAG_BOLOS_UX) void os_perso_set_pin(
    unsigned int identity, unsigned char *pin PLENGTH(length),
    unsigned int length);
// set the currently unlocked identity pin. (change pin feature)
SYSCALL
    PERMISSION(APPLICATION_FLAG_BOLOS_UX) void os_perso_set_current_identity_pin(
        unsigned char *pin PLENGTH(length), unsigned int length);

#define BOLOS_UX_ONBOARDING_ALGORITHM_BIP39 1
#define BOLOS_UX_ONBOARDING_ALGORITHM_ELECTRUM 2

/**
 * Set the persisted seed if none yet, else override the volatile seed (in RAM)
 */
SYSCALL PERMISSION(APPLICATION_FLAG_BOLOS_UX) void os_perso_set_seed(
    unsigned int identity, unsigned int algorithm,
    unsigned char *seed PLENGTH(length), unsigned int length);

SYSCALL PERMISSION(APPLICATION_FLAG_BOLOS_UX) void os_perso_derive_and_set_seed(
    unsigned char identity, const char *prefix PLENGTH(prefix_length),
    unsigned int prefix_length,
    const char *passphrase PLENGTH(passphrase_length),
    unsigned int passphrase_length, const char *words PLENGTH(words_length),
    unsigned int words_length);

// SYSCALL PERMISSION(APPLICATION_FLAG_BOLOS_UX) void
// os_perso_set_alternate_pin(unsigned char* pin PLENGTH(pinLength), unsigned
// int pinLength);
// SYSCALL PERMISSION(APPLICATION_FLAG_BOLOS_UX) void
// os_perso_set_alternate_seed(unsigned char* seed PLENGTH(seedLength), unsigned
// int seedLength);
SYSCALL PERMISSION(APPLICATION_FLAG_BOLOS_UX) void os_perso_set_words(
    const unsigned char *words PLENGTH(length), unsigned int length);
SYSCALL PERMISSION(APPLICATION_FLAG_BOLOS_UX) void os_perso_finalize(void);

// checked in the ux flow to avoid asking the pin for example
// NBA : could also be checked by applications running in unsecure mode - thus
// unprivilegied
SYSCALL unsigned int os_perso_isonboarded(void);


// derive the seed for the requested BIP32 path
SYSCALL void os_perso_derive_node_bip32(
    cx_curve_t curve, const unsigned int *path PLENGTH(4 * pathLength),
    unsigned int pathLength, unsigned char *privateKey PLENGTH(32),
    unsigned char *chain PLENGTH(32));
// derive the seed for the requested BIP32 path, with the custom provided
// seed_key for the sha512 hmac ("Bitcoin Seed", "Nist256p1 Seed", "ed25519
// seed", ...)
SYSCALL void os_perso_derive_node_bip32_seed_key(
    cx_curve_t curve, const unsigned int *path PLENGTH(4 * pathLength),
    unsigned int pathLength, unsigned char *privateKey PLENGTH(32),
    unsigned char *chain PLENGTH(32),
    unsigned char *seed_key PLENGTH(seed_key_length),
    unsigned int seed_key_length);

// endorsement APIs
SYSCALL unsigned int
os_endorsement_get_code_hash(unsigned char *buffer PLENGTH(32));
SYSCALL unsigned int
os_endorsement_get_public_key(unsigned char index,
                              unsigned char *buffer PLENGTH(65));
SYSCALL unsigned int os_endorsement_get_public_key_certificate(
    unsigned char index,
    unsigned char *buffer PLENGTH(1 + 1 + 2 * (1 + 1 + 33)));
SYSCALL unsigned int
os_endorsement_key1_get_app_secret(unsigned char *buffer PLENGTH(64));
SYSCALL unsigned int os_endorsement_key1_sign_data(
    unsigned char *src PLENGTH(srcLength), unsigned int srcLength,
    unsigned char *signature PLENGTH(1 + 1 + 2 * (1 + 1 + 33)));
SYSCALL unsigned int os_endorsement_key2_derive_sign_data(
    unsigned char *src PLENGTH(srcLength), unsigned int srcLength,
    unsigned char *signature PLENGTH(1 + 1 + 2 * (1 + 1 + 33)));

// nvram shared zone access right => MPU opening at application switch, using a
// flags in the registry

// Global PIN
#define DEFAULT_PIN_RETRIES 3
SYSCALL PERMISSION(
    APPLICATION_FLAG_GLOBAL_PIN) unsigned int os_global_pin_is_validated(void);
/**
 * Validating the pin also setup the identity linked with this pin (normal or
 * alternate)
 * @return 1 if pin validated
 */
SYSCALL
    PERMISSION(APPLICATION_FLAG_GLOBAL_PIN) unsigned int os_global_pin_check(
        unsigned char *pin_buffer PLENGTH(pin_length),
        unsigned char pin_length);
SYSCALL
    PERMISSION(APPLICATION_FLAG_GLOBAL_PIN) void os_global_pin_invalidate(void);
SYSCALL PERMISSION(
    APPLICATION_FLAG_GLOBAL_PIN) unsigned int os_global_pin_retries(void);

SYSCALL
    PERMISSION(APPLICATION_FLAG_BOLOS_UX) unsigned int os_registry_count(void);
// return any entry, activated or not, enabled or not, empty or not. to enable
// full control
SYSCALL PERMISSION(APPLICATION_FLAG_BOLOS_UX) void os_registry_get(
    unsigned int index,
    application_t *out_application_entry PLENGTH(sizeof(application_t)));

// execute the given application index in the registry, this function kills the
// current task
TASKSWITCH PERMISSION(APPLICATION_FLAG_BOLOS_UX) unsigned int os_sched_exec(
    unsigned int application_index);
// exit the current task
SYSCALL void os_sched_exit(unsigned int exit_code);

// the bolos_ux declares the buffer to be used to store the ux parameter to be
// displayed
SYSCALL PERMISSION(APPLICATION_FLAG_BOLOS_UX) void os_ux_register(
    bolos_ux_params_t *parameter_ram_pointer
        PLENGTH(sizeof(bolos_ux_params_t)));

// return !0 when ux scenario has ended, else 0
// while not ended, all display/touch/ticker and other events are to be
// processed by the ux. only io is not processed.
// when returning !0 the application must send a general status (or continue its
// command flow)
TASKSWITCH unsigned int
os_ux(bolos_ux_params_t *params PLENGTH(sizeof(bolos_ux_params_t)));

// process all possible messages while waiting for a ux to finish,
// unprocessed messages are replied with a generic general status
// when returning the application must send a general status (or continue its
// command flow)
unsigned int os_ux_blocking(bolos_ux_params_t *params);

/* ----------------------------------------------------------------------- */
/* -                            LIB FUNCTIONS                            - */
/* ----------------------------------------------------------------------- */
/**
 * Library call function.
 * call_parameters[0] = library name string pointer (const)
 * call_parameters[1] = library call identifier (0 = init, ...)
 * call_parameters[2+] = called function parameters
 */
SYSCALL unsigned int os_lib_call(unsigned int *call_parameters);
SYSCALL void os_lib_end(unsigned int returnvalue);
SYSCALL void os_lib_throw(unsigned int exception);

/* ----------------------------------------------------------------------- */
/* -                            IO FUNCTIONS                             - */
/* ----------------------------------------------------------------------- */
typedef REENTRANT(void (*io_send_t)(unsigned char *buffer,
                                    unsigned short length));

typedef REENTRANT(unsigned short (*io_recv_t)(unsigned char *buffer,
                                              unsigned short maxlenth));

typedef enum io_usb_hid_receive_status_e {
    IO_USB_APDU_RESET,
    IO_USB_APDU_MORE_DATA,
    IO_USB_APDU_RECEIVED,
} io_usb_hid_receive_status_t;

extern volatile unsigned int G_io_usb_hid_total_length;

void io_usb_hid_init(void);

io_usb_hid_receive_status_t
io_usb_hid_receive(io_send_t sndfct, unsigned char *buffer, unsigned short l);

unsigned short io_usb_hid_exchange(io_send_t sndfct, unsigned short sndlength,
                                   io_recv_t rcvfct, unsigned char flags);

/* ----------------------------------------------------------------------- */
/* -                            ID FUNCTIONS                             - */
/* ----------------------------------------------------------------------- */
#define OS_FLAG_RECOVERY 1
#define OS_FLAG_SIGNED_MCU_CODE 2
//#define OS_FLAG_CUSTOM_UX       4
/* Enable application to retrieve OS current running options */
SYSCALL unsigned int os_flags(void);
SYSCALL unsigned int os_version(unsigned char *version PLENGTH(maxlength),
                                unsigned int maxlength);
/* Grab the SEPROXYHAL's feature set */
SYSCALL unsigned int os_seph_features(void);
/* Grab the SEPROXYHAL's version */
SYSCALL unsigned int os_seph_version(unsigned char *version PLENGTH(maxlength),
                                     unsigned int maxlength);

/*
 * Copy the serial number in the given buffer and return its length
 */
unsigned int os_get_sn(unsigned char *buffer);

/* ----------------------------------------------------------------------- */
/* -                         SETTINGS FUNCTIONS                          - */
/* ----------------------------------------------------------------------- */
typedef enum os_setting_e {
    OS_SETTING_BRIGHTNESS,
    OS_SETTING_INVERT,
    OS_SETTING_ROTATION,
#ifdef HAVE_BOLOS_NOT_SHUFFLED_PIN
    OS_SETTING_NOSHUFFLE_PIN,
#endif // HAVE_BOLOS_NOT_SHUFFLED_PIN
    OS_SETTING_AUTO_LOCK_DELAY,
    OS_SETTING_POWER_OFF_DELAY,

    // before that value, all settings are only making use of the length value
    // with a null buffer to be set, and are returned through the return value
    // with a maxlength = 0 in the get.
    OS_SETTING_LAST_INT,

    // screen saver string to display
    OS_SETTING_SAVER_STRING = OS_SETTING_LAST_INT,
    OS_SETTING_DEVICENAME,

    OS_SETTING_LAST,
} os_setting_t;

/**
 * Retrieve the value of a setting in a user specified buffer, with a max
 * length, and return the effective returned length.
 */
SYSCALL PERMISSION(APPLICATION_FLAG_BOLOS_SETTINGS) unsigned int os_setting_get(
    unsigned int setting_id, unsigned char *value PLENGTH(maxlen),
    unsigned int maxlen);

/**
 * Define a setting's value from a user buffer and its length. In case of error,
 * a throw is executed.
 */
SYSCALL PERMISSION(APPLICATION_FLAG_BOLOS_SETTINGS) void os_setting_set(
    unsigned int setting_id, unsigned char *value PLENGTH(length),
    unsigned int length);

/* ----------------------------------------------------------------------- */
/* -                          DEBUG FUNCTIONS                           - */
/* ----------------------------------------------------------------------- */
void screen_printf(const char *format, ...);

// emit a single byte
void screen_printc(unsigned char const c);

// redefined if string.h not included
int snprintf(char *str, size_t str_size, const char *format, ...);

// syscall test
// SYSCALL void dummy_1(unsigned int* p PLENGTH(2+len+15+ len + 16 +
// sizeof(io_send_t) + 1 ), unsigned int len);

typedef struct meminfo_s {
    unsigned int free_nvram_size;
    unsigned int appMemory;
    unsigned int systemSize;
    unsigned int slots;
} meminfo_t;

SYSCALL PERMISSION(APPLICATION_FLAG_BOLOS_UX) void os_get_memory_info(
    meminfo_t *meminfo);

// arbitraty max size for application names.
#define BOLOS_APPNAME_MAX_SIZE_B 32

// read from the application's install parameter TLV (if present), else 0 is
// returned (no exception thrown).
// takes into account the currently loaded application
#define OS_REGISTRY_GET_TAG_OFFSET_COMPARE_WITH_BUFFER (0x80000000UL)
#define OS_REGISTRY_GET_TAG_OFFSET_GET_LENGTH (0x40000000UL)

/**
 * @param appidx The application entry index in the registry (raw, not filtering
 * ux or whatever). If the entry index correspond to the application being
 * installed then RAM structure content is used instead of the NVRAM registry.
 * @param tlvoffset The offset within the install parameters memory area, in
 * bytes. Useful if tag is present multiple times. Can be null. The tlv offset
 * is the offset of the tag in the install parameters area when a tag is
 * matched. This way long tag can be read in multiple time without the need to
 * play with the tlvoffset. Add +1 to skip to the next one when seraching for
 * multiple tag occurences.
 * @param tag The tag to be searched for
 * @param value_offset The offset within the value for this occurence of the
 * tag. The OS_REGISTRY_GET_TAG_OFFSET_COMPARE_WITH_BUFFER or
 * OS_REGISTRY_GET_TAG_OFFSET_GET_LENGTH can be ORed to perform meta operation
 * on the TLV occurence.
 * @param buffer The user buffer for comparison or to retrieve the value of the
 * tag at the given offset.
 * @param maxlength Size of the buffer to be compared OR to be retrieved
 * (trimmed depending the TLV effective length).
 */
SYSCALL PERMISSION(APPLICATION_FLAG_BOLOS_UX) unsigned int os_registry_get_tag(
    unsigned int appidx, unsigned int *tlvoffset, unsigned int tag,
    unsigned int value_offset, void *buffer PLENGTH(maxlength),
    unsigned int maxlength);

/* ----------------------------------------------------------------------- */
/* -                         CUSTOM CERTIFICATE AUTHORITY                - */
/* ----------------------------------------------------------------------- */

// Verify the signature is issued from the custom certificate authority
SYSCALL unsigned int
os_customca_verify(unsigned char *hash PLENGTH(32),
                   unsigned char *sign PLENGTH(sign_length),
                   unsigned int sign_length);

/* ----------------------------------------------------------------------- */
/* -                         PRECISE WATCHDOG                            - */
/* ----------------------------------------------------------------------- */

typedef enum {
    /* Watchdog consumption lead to no action being taken, overflowed value is
       accounted and can be retrieved by the application */
    OS_WATCHDOG_NOACTION = 0,
    /* Request a platform reset when the watchdog set value is completely
       consumed */
    OS_WATCHDOG_RESET = 1,
    /* Request a wipe of the user data when the watchdog times out */
    OS_WATCHDOG_WIPE = 2,
} os_watchdog_behavior_t;

/**
 * This function arm a low level watchdog, when the value is consumed, and
 * depending on the requested behavior,
 * an action can be taken.
 * @throw INVALID_PARAMETER when the useconds value overflows the possible
 * value.
 */
void os_watchdog_arm(unsigned int useconds, os_watchdog_behavior_t behavior);

/**
 * This function returns the number of useconds to be still consumed by the
 * watchdog (when > 0), or the overflowed useconds after the watchdog has timed
 * out (when < 0)
 */
int os_watchdog_value(void);

/**
 * Stop background watchdog and avoid its action if any is set.
 */
void os_watchdog_stop(void);

/* ----------------------------------------------------------------------- */
/* -                         MEMORY PROTECTION UNIT                      - */
/* ----------------------------------------------------------------------- */

/**
 * Enable/disable the seed container FLASH region memory protection for
 * read/write operations
 * @return the previous state of protection before applying the new value
 */
unsigned int os_mpu_protect_flash(unsigned int prot_enabled);
/**
 * Enable/disable the os private RAM region memory protection for read/write
 * operations
 * @return the previous state of protection before applying the new value
 */
unsigned int os_mpu_protect_ram(unsigned int prot_enabled);

#endif // OS_H
