#pragma once

#include "appflags.h"
#include "decorators.h"
#include "lcx_ecfp.h"
#include "os_types.h"

/* ----------------------------------------------------------------------- */
/* -                             SEED FEATURE                            - */
/* ----------------------------------------------------------------------- */

#define BOLOS_UX_ONBOARDING_ALGORITHM_BIP39 1
#define BOLOS_UX_ONBOARDING_ALGORITHM_ELECTRUM 2
#if defined(HAVE_VAULT_RECOVERY_ALGO)
#define BOLOS_UX_ONBOARDING_ALGORITHM_BIP39_VAULT_REC_MSK 3
#endif // HAVE_VAULT_RECOVERY_ALGO

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

#if defined(HAVE_VAULT_RECOVERY_ALGO)
SYSCALL
    PERMISSION(APPLICATION_FLAG_BOLOS_UX) void os_perso_derive_and_prepare_seed(
        const char *words, unsigned int words_length);
SYSCALL PERMISSION(APPLICATION_FLAG_BOLOS_UX) void os_perso_derive_and_xor_seed(
    void);
SYSCALL PERMISSION(
    APPLICATION_FLAG_BOLOS_UX) unsigned char os_perso_get_seed_algorithm(void);
#endif // HAVE_VAULT_RECOVERY_ALGO

SYSCALL PERMISSION(APPLICATION_FLAG_BOLOS_UX) void os_perso_set_words(
    const unsigned char *words PLENGTH(length), unsigned int length);
SYSCALL PERMISSION(APPLICATION_FLAG_BOLOS_UX) void os_perso_finalize(void);

// checked in the ux flow to avoid asking the pin for example
// NBA : could also be checked by applications running in unsecure mode - thus
// unprivilegied
// @return BOLOS_UX_OK when perso is onboarded.
SYSCALL bolos_bool_t os_perso_isonboarded(void);

SYSCALL void os_perso_set_onboarding_status(unsigned int kind,
                                            unsigned int count,
                                            unsigned int maxCount,
                                            unsigned int isConfirming);

// derive the seed for the requested BIP32 path
SYSCALL void os_perso_derive_node_bip32(
    cx_curve_t curve,
    const unsigned int *path PLENGTH(4 * (pathLength & 0x0FFFFFFFu)),
    unsigned int pathLength, unsigned char *privateKey PLENGTH(64),
    unsigned char *chain PLENGTH(32));

#define HDW_NORMAL 0
#define HDW_ED25519_SLIP10 1
// symmetric key derivation according to SLIP-0021
// this only supports derivation of the master node (level 1)
// the beginning of the authorized path is to be provided in the authorized
// derivation tag of the registry starting with a \x00 Note: for SLIP21, the
// path is a string and the pathLength is the number of chars including the
// starting \0 byte. However, firewall checks are processing a number of
// integers, therefore, take care not to locate the buffer too far in memory to
// pass the firewall check.
#define HDW_SLIP21 2
// derive the seed for the requested BIP32 path, with the custom provided
// seed_key for the sha512 hmac ("Bitcoin Seed", "Nist256p1 Seed", "ed25519
// seed", ...)
SYSCALL void os_perso_derive_node_with_seed_key(
    unsigned int mode, cx_curve_t curve,
    const unsigned int *path PLENGTH(4 * (pathLength & 0x0FFFFFFFu)),
    unsigned int pathLength, unsigned char *privateKey PLENGTH(64),
    unsigned char *chain PLENGTH(32),
    unsigned char *seed_key PLENGTH(seed_key_length),
    unsigned int seed_key_length);
#define os_perso_derive_node_bip32_seed_key(mode, curve, path, pathLength,     \
                                            privateKey, chain, seed_key,       \
                                            seed_key_length)                   \
  os_perso_derive_node_with_seed_key(mode, curve, path, pathLength,            \
                                     privateKey, chain, seed_key,              \
                                     seed_key_length)

SYSCALL void os_perso_derive_eip2333(
    cx_curve_t curve,
    const unsigned int *path PLENGTH(4 * (pathLength & 0x0FFFFFFFu)),
    unsigned int pathLength, unsigned char *privateKey PLENGTH(32));
/**
 * Generate a seed based cookie
 * seed => derivation (path 0xda7aba5e/0xc1a551c5) => priv key =SECP256K1=>
 * pubkey => sha512 => cookie
 */

#if defined(HAVE_SEED_COOKIE)
SYSCALL unsigned int
os_perso_seed_cookie(unsigned char *seed_cookie PLENGTH(seed_cookie_length),
                     unsigned int seed_cookie_length);
#endif // HAVE_SEED_COOKIE
