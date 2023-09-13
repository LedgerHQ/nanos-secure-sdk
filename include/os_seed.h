#pragma once

#include <string.h>

#include "appflags.h"
#include "decorators.h"
#include "lcx_ecfp.h"
#include "os_types.h"

/* ----------------------------------------------------------------------- */
/* -                             SEED FEATURE                            - */
/* ----------------------------------------------------------------------- */

#define BOLOS_UX_ONBOARDING_ALGORITHM_BIP39    1
#define BOLOS_UX_ONBOARDING_ALGORITHM_ELECTRUM 2
#if defined(HAVE_VAULT_RECOVERY_ALGO)
#define BOLOS_UX_ONBOARDING_ALGORITHM_BIP39_VAULT_REC_MSK 3
#define BOLOS_VAULT_RECOVERY_WORK_BUFFER_SIZE             64
#endif  // HAVE_VAULT_RECOVERY_ALGO
#define BOLOS_MASTER_SEED_LEN (32)

typedef enum {
    SET_SEED = 0,
    SET_STATE,
    GET_STATE
} os_action_t;

/**
 * Set the persisted seed if none yet, else override the volatile seed (in RAM)
 */
SYSCALL PERMISSION(APPLICATION_FLAG_BOLOS_UX) void os_perso_set_seed(unsigned int identity,
                                                                     unsigned int algorithm,
                                                                     unsigned char *seed
                                                                                  PLENGTH(length),
                                                                     unsigned int length);

SYSCALL PERMISSION(APPLICATION_FLAG_BOLOS_UX) void os_perso_derive_and_set_seed(
    unsigned char          identity,
    const char *prefix     PLENGTH(prefix_length),
    unsigned int           prefix_length,
    const char *passphrase PLENGTH(passphrase_length),
    unsigned int           passphrase_length,
    const char *words      PLENGTH(words_length),
    unsigned int           words_length);

#if defined(HAVE_VAULT_RECOVERY_ALGO)
SYSCALL PERMISSION(APPLICATION_FLAG_BOLOS_UX) void os_perso_derive_and_prepare_seed(
    const char  *words,
    unsigned int words_length,
    uint8_t     *vault_recovery_work_buffer);
SYSCALL PERMISSION(APPLICATION_FLAG_BOLOS_UX) void os_perso_derive_and_xor_seed(
    uint8_t *vault_recovery_work_buffer);
SYSCALL       PERMISSION(APPLICATION_FLAG_BOLOS_UX)
unsigned char os_perso_get_seed_algorithm(void);
#endif  // HAVE_VAULT_RECOVERY_ALGO

SYSCALL PERMISSION(APPLICATION_FLAG_BOLOS_UX) void os_perso_set_words(const unsigned char *words
                                                                                   PLENGTH(length),
                                                                      unsigned int length);
SYSCALL PERMISSION(APPLICATION_FLAG_BOLOS_UX) void os_perso_finalize(void);
#if defined(HAVE_RECOVER)
SYSCALL PERMISSION(APPLICATION_FLAG_BOLOS_UX) void os_perso_master_seed(uint8_t *master_seed
                                                                                    PLENGTH(length),
                                                                        size_t      length,
                                                                        os_action_t action);
SYSCALL PERMISSION(APPLICATION_FLAG_BOLOS_UX) void os_perso_recover_state(uint8_t    *state,
                                                                          os_action_t action);
#endif  // HAVE_RECOVER

// checked in the ux flow to avoid asking the pin for example
// NBA : could also be checked by applications running in insecure mode - thus unprivilegied
// @return BOLOS_UX_OK when perso is onboarded.
SYSCALL bolos_bool_t os_perso_isonboarded(void);

enum {
    ONBOARDING_STATUS_WELCOME = 0,
    ONBOARDING_STATUS_WELCOME_STEP2,
    ONBOARDING_STATUS_WELCOME_STEP5,
    ONBOARDING_STATUS_WELCOME_STEP6,
    ONBOARDING_STATUS_WELCOME_REMEMBER,
    ONBOARDING_STATUS_SETUP_CHOICE,
    ONBOARDING_STATUS_PIN,
    ONBOARDING_STATUS_NEW_DEVICE,
    ONBOARDING_STATUS_NEW_DEVICE_CONFIRMING,
    ONBOARDING_STATUS_RESTORE_SEED,
    ONBOARDING_STATUS_SAFETY_WARNINGS,
    ONBOARDING_STATUS_READY,
    ONBOARDING_STATUS_CHOOSE_NAME,
    ONBOARDING_STATUS_RECOVER_RESTORE_SEED,
    ONBOARDING_STATUS_SETUP_CHOICE_RESTORE_SEED,
    ONBOARDING_STATUS_CHECKING
};
SYSCALL void os_perso_set_onboarding_status(unsigned int state,
                                            unsigned int count,
                                            unsigned int total);

// derive the seed for the requested BIP32 path
// Deprecated : see "os_derive_bip32_no_throw"
#ifndef HAVE_BOLOS
DEPRECATED
#endif
SYSCALL void os_perso_derive_node_bip32(cx_curve_t curve,
                                        const unsigned int *path
                                                     PLENGTH(4 * (pathLength & 0x0FFFFFFFu)),
                                        unsigned int pathLength,
                                        unsigned char *privateKey PLENGTH(64),
                                        unsigned char *chain      PLENGTH(32));

#define HDW_NORMAL         0
#define HDW_ED25519_SLIP10 1
// symmetric key derivation according to SLIP-0021
// this only supports derivation of the master node (level 1)
// the beginning of the authorized path is to be provided in the authorized derivation tag of the
// registry starting with a \x00 Note: for SLIP21, the path is a string and the pathLength is the
// number of chars including the starting \0 byte. However, firewall checks are processing a number
// of integers, therefore, take care not to locate the buffer too far in memory to pass the firewall
// check.
#define HDW_SLIP21         2

// derive the seed for the requested BIP32 path, with the custom provided seed_key for the sha512
// hmac ("Bitcoin Seed", "Nist256p1 Seed", "ed25519 seed", ...) Deprecated : see
// "os_derive_bip32_with_seed_no_throw"
#ifndef HAVE_BOLOS
DEPRECATED
#endif
SYSCALL void os_perso_derive_node_with_seed_key(unsigned int mode,
                                                cx_curve_t   curve,
                                                const unsigned int *path
                                                    PLENGTH(4 * (pathLength & 0x0FFFFFFFu)),
                                                unsigned int              pathLength,
                                                unsigned char *privateKey PLENGTH(64),
                                                unsigned char *chain      PLENGTH(32),
                                                unsigned char *seed_key   PLENGTH(seed_key_length),
                                                unsigned int              seed_key_length);

#define os_perso_derive_node_bip32_seed_key os_perso_derive_node_with_seed_key

/**
 * @brief   Gets the private key from the device seed using the specified bip32 path and seed key.
 *
 * @param[in]  derivation_mode Derivation mode, one of HDW_NORMAL / HDW_ED25519_SLIP10 / HDW_SLIP21.
 *
 * @param[in]  curve           Curve identifier.
 *
 * @param[in]  path            Bip32 path to use for derivation.
 *
 * @param[in]  path_len        Bip32 path length.
 *
 * @param[out] raw_privkey     Buffer where to store the private key.
 *
 * @param[out] chain_code      Buffer where to store the chain code. Can be NULL.
 *
 * @param[in]  seed            Seed key to use for derivation.
 *
 * @param[in]  seed_len        Seed key length.
 *
 * @return                     Error code:
 *                             - CX_OK on success
 *                             - CX_INTERNAL_ERROR
 */
WARN_UNUSED_RESULT static inline cx_err_t os_derive_bip32_with_seed_no_throw(
    unsigned int        derivation_mode,
    cx_curve_t          curve,
    const unsigned int *path,
    unsigned int        path_len,
    unsigned char       raw_privkey[static 64],
    unsigned char      *chain_code,
    unsigned char      *seed,
    unsigned int        seed_len)
{
    cx_err_t error = CX_OK;

    BEGIN_TRY
    {
        TRY
        {
// ignore the deprecated warning, pragma to remove when the "no throw" OS function will be available
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
            // Derive the seed with path
            os_perso_derive_node_bip32_seed_key(
                derivation_mode, curve, path, path_len, raw_privkey, chain_code, seed, seed_len);
#pragma GCC diagnostic pop
        }
        CATCH_OTHER(e)
        {
            error = e;

            // Make sure the caller doesn't use uninitialized data in case
            // the return code is not checked.
            explicit_bzero(raw_privkey, 64);
        }
        FINALLY {}
    }
    END_TRY;

    return error;
}

/**
 * @brief   Gets the private key from the device seed using the specified bip32 path.
 *
 * @param[in]  curve           Curve identifier.
 *
 * @param[in]  path            Bip32 path to use for derivation.
 *
 * @param[in]  path_len        Bip32 path length.
 *
 * @param[out] raw_privkey     Buffer where to store the private key.
 *
 * @param[out] chain_code      Buffer where to store the chain code. Can be NULL.
 *
 * @return                     Error code:
 *                             - CX_OK on success
 *                             - CX_INTERNAL_ERROR
 */
WARN_UNUSED_RESULT static inline cx_err_t os_derive_bip32_no_throw(
    cx_curve_t          curve,
    const unsigned int *path,
    unsigned int        path_len,
    unsigned char       raw_privkey[static 64],
    unsigned char      *chain_code)
{
    return os_derive_bip32_with_seed_no_throw(
        HDW_NORMAL, curve, path, path_len, raw_privkey, chain_code, NULL, 0);
}

// Deprecated : see "os_derive_eip2333_no_throw"
#ifndef HAVE_BOLOS
DEPRECATED
#endif
SYSCALL void os_perso_derive_eip2333(cx_curve_t                curve,
                                     const unsigned int *path  PLENGTH(4
                                                                      * (pathLength & 0x0FFFFFFFu)),
                                     unsigned int              pathLength,
                                     unsigned char *privateKey PLENGTH(32));

/**
 * @brief   Gets the private key from the device seed using the specified eip2333 path.
 *
 * @param[in]  curve           Curve identifier.
 *
 * @param[in]  path            Eip2333 path to use for derivation.
 *
 * @param[in]  path_len        Eip2333 path length.
 *
 * @param[out] raw_privkey     Buffer where to store the private key.
 *
 * @return                     Error code:
 *                             - CX_OK on success
 *                             - CX_INTERNAL_ERROR
 */
WARN_UNUSED_RESULT static inline cx_err_t os_derive_eip2333_no_throw(
    cx_curve_t          curve,
    const unsigned int *path,
    unsigned int        path_len,
    unsigned char       raw_privkey[static 64])
{
    cx_err_t error = CX_OK;

    BEGIN_TRY
    {
        TRY
        {
// ignore the deprecated warning, pragma to remove when the "no throw" OS function will be available
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
            // Derive the seed with path
            os_perso_derive_eip2333(curve, path, path_len, raw_privkey);
#pragma GCC diagnostic pop
        }
        CATCH_OTHER(e)
        {
            error = e;

            // Make sure the caller doesn't use uninitialized data in case
            // the return code is not checked.
            explicit_bzero(raw_privkey, 64);
        }
        FINALLY {}
    }
    END_TRY;

    return error;
}

/**
 * Generate a seed based cookie
 * seed => derivation (path 0xda7aba5e/0xc1a551c5) => priv key =SECP256K1=> pubkey => sha512 =>
 * cookie
 */

#if defined(HAVE_SEED_COOKIE)
// seed_cookie length has to be CX_SHA512_SIZE.
// return BOLOS_TRUE if the seed has been generated, return BOLOS_FALSE otherwise.
SYSCALL bolos_bool_t os_perso_seed_cookie(unsigned char *seed_cookie PLENGTH(CX_SHA512_SIZE));
#endif  // HAVE_SEED_COOKIE
