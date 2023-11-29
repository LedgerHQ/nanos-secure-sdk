#pragma once

#include "bolos_target.h"
#include "decorators.h"

/* ----------------------------------------------------------------------- */
/* -                         ENDORSEMENT FEATURE                         - */
/* ----------------------------------------------------------------------- */

typedef enum endorsement_revoke_id_e {
    ENDORSEMENT_REVOKE_ID_ALL   = 0,
    ENDORSEMENT_REVOKE_ID_SLOT1 = 1,
    ENDORSEMENT_REVOKE_ID_SLOT2 = 2,
    ENDORSEMENT_REVOKE_ID_LAST  = 3,
} endorsement_revoke_id_t;

#define ENDORSEMENT_MAX_ASN1_LENGTH (1 + 1 + 2 * (1 + 1 + 33))

SYSCALL unsigned int os_endorsement_get_code_hash(unsigned char *buffer PLENGTH(32));
SYSCALL unsigned int os_endorsement_get_public_key(unsigned char         index,
                                                   unsigned char *buffer PLENGTH(65),
                                                   unsigned char *length PLENGTH(1));
SYSCALL unsigned int os_endorsement_get_public_key_certificate(
    unsigned char         index,
    unsigned char *buffer PLENGTH(ENDORSEMENT_MAX_ASN1_LENGTH),
    unsigned char *length PLENGTH(1));
SYSCALL unsigned int os_endorsement_key1_get_app_secret(unsigned char *buffer PLENGTH(64));
SYSCALL unsigned int os_endorsement_key1_sign_data(unsigned char *src PLENGTH(srcLength),
                                                   unsigned int       srcLength,
                                                   unsigned char *signature
                                                       PLENGTH(ENDORSEMENT_MAX_ASN1_LENGTH));
SYSCALL unsigned int os_endorsement_key1_sign_without_code_hash(
    unsigned char *src       PLENGTH(srcLength),
    unsigned int             srcLength,
    unsigned char *signature PLENGTH(ENDORSEMENT_MAX_ASN1_LENGTH));
SYSCALL unsigned int os_endorsement_key2_derive_sign_data(unsigned char *src PLENGTH(srcLength),
                                                          unsigned int       srcLength,
                                                          unsigned char *signature
                                                              PLENGTH(ENDORSEMENT_MAX_ASN1_LENGTH));

SYSCALL unsigned int os_endorsement_get_metadata(unsigned char         index,
                                                 unsigned char *buffer PLENGTH(8));

SYSCALL void os_endorsement_revoke_slot1(void);
SYSCALL void os_endorsement_revoke_slot2(void);
SYSCALL void os_endorsement_revoke_all(void);
