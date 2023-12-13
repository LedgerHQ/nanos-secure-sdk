#pragma once

#include "os_app.h"
#include "os_math.h"
#include "os_types.h"
#include "os_utils.h"
#ifdef HAVE_AEM_PIN
#include "os_pin.h"
#endif

/* ----------------------------------------------------------------------- */
/* -                            UX DEFINITIONS                           - */
/* ----------------------------------------------------------------------- */

#if !defined(APP_UX)
#if !defined(HAVE_BOLOS)

// Enumeration of the UX events usable by the UX library.
typedef enum bolos_ux_e {
    BOLOS_UX_INITIALIZE = 0,
    BOLOS_UX_EVENT,
    BOLOS_UX_KEYBOARD,
    BOLOS_UX_WAKE_UP,
    BOLOS_UX_STATUS_BAR,

    BOLOS_UX_VALIDATE_PIN,
    BOLOS_UX_ASYNCHMODAL_PAIRING_REQUEST,  // ask the ux to display a modal to accept/reject the
                                           // current pairing request
    BOLOS_UX_ASYNCHMODAL_PAIRING_CANCEL,
    BOLOS_UX_IO_RESET,
    BOLOS_UX_LAST_ID,
} bolos_ux_t;

// Structure that defines the parameters to exchange with the BOLOS UX application
typedef struct bolos_ux_params_s {
    // Event identifier.
    bolos_ux_t ux_id;
    // length of parameters in the u union to be copied during the syscall.
    unsigned int len;

#if defined(HAVE_BLE) || defined(HAVE_KEYBOARD_UX)
    union {
        // Structure for the lib ux.
#if defined(HAVE_KEYBOARD_UX)
        struct {
            unsigned int keycode;
#define BOLOS_UX_MODE_UPPERCASE 0
#define BOLOS_UX_MODE_LOWERCASE 1
#define BOLOS_UX_MODE_SYMBOLS   2
#define BOLOS_UX_MODE_COUNT     3  // number of keyboard modes
            unsigned int mode;
            // + 1 EOS (0)
#define BOLOS_UX_KEYBOARD_TEXT_BUFFER_SIZE 32
            char entered_text[BOLOS_UX_KEYBOARD_TEXT_BUFFER_SIZE + 1];
        } keyboard;
#endif  // HAVE_KEYBOARD_UX

#if defined(HAVE_BLE)
        struct {
            enum {
                BOLOS_UX_ASYNCHMODAL_PAIRING_REQUEST_PASSKEY,
                BOLOS_UX_ASYNCHMODAL_PAIRING_REQUEST_NUMCOMP,
            } type;
            unsigned int pairing_info_len;
            char         pairing_info[16];
        } pairing_request;

        struct {
            enum {
                BOLOS_UX_ASYNCHMODAL_PAIRING_STATUS_CONFIRM_CODE_YES,
                BOLOS_UX_ASYNCHMODAL_PAIRING_STATUS_CONFIRM_CODE_NO,
                BOLOS_UX_ASYNCHMODAL_PAIRING_STATUS_ACCEPT_PASSKEY,
                BOLOS_UX_ASYNCHMODAL_PAIRING_STATUS_CANCEL_PASSKEY,
                BOLOS_UX_ASYNCHMODAL_PAIRING_STATUS_SUCCESS,
                BOLOS_UX_ASYNCHMODAL_PAIRING_STATUS_TIMEOUT,
                BOLOS_UX_ASYNCHMODAL_PAIRING_STATUS_FAILED,
                BOLOS_UX_ASYNCHMODAL_PAIRING_STATUS_CANCELLED_FROM_REMOTE,
            } pairing_ok;
        } pairing_status;  // sent in BOLOS_UX_ASYNCHMODAL_PAIRING_CANCEL message
#endif                     // HAVE_BLE
    } u;
#endif  // defined(HAVE_BLE) || defined(HAVE_KEYBOARD_UX)
} bolos_ux_params_t;

#endif  // !defined(HAVE_BOLOS)

/* ----------------------------------------------------------------------- */
/* -                             UX-RELATED                              - */
/* ----------------------------------------------------------------------- */
#if defined(BOLOS_TEST_APDUS_UX)
void bolos_test_ux_apdu(void);
#endif  // defined(BOLOS_TEST_APDUS_UX)

// return !0 when ux scenario has ended, else 0
// while not ended, all display/touch/ticker and other events are to be processed by the ux. only io
// is not processed. when returning !0 the application must send a general status (or continue its
// command flow)
SYSCALL TASKSWITCH unsigned int os_ux(bolos_ux_params_t *params PLENGTH(sizeof(bolos_ux_params_t)));
// read parameters back from the UX app. useful to read keyboard type or such
SYSCALL void os_ux_result(bolos_ux_params_t *params PLENGTH(sizeof(bolos_ux_params_t)));

// process all possible messages while waiting for a ux to finish,
// unprocessed messages are replied with a generic general status
// when returning the application must send a general status (or continue its command flow)
unsigned int os_ux_blocking(bolos_ux_params_t *params);
#endif  // !defined(APP_UX)

#ifdef HAVE_BLE
SYSCALL void os_ux_set_status(unsigned int ux_id, unsigned int status);

SYSCALL unsigned int os_ux_get_status(unsigned int ux_id);
#endif  // HAVE_BLE
