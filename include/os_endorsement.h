#pragma once

#include "bolos_target.h"
#include "decorators.h"

/* ----------------------------------------------------------------------- */
/* -                         ENDORSEMENT FEATURE                         - */
/* ----------------------------------------------------------------------- */

#define ENDORSEMENT_MAX_ASN1_LENGTH (1 + 1 + 2 * (1 + 1 + 33))

SYSCALL unsigned int
os_endorsement_get_code_hash(unsigned char *buffer PLENGTH(32));
SYSCALL unsigned int
os_endorsement_get_public_key(unsigned char index,
                              unsigned char *buffer PLENGTH(65));
SYSCALL unsigned int os_endorsement_get_public_key_certificate(
    unsigned char index,
    unsigned char *buffer PLENGTH(ENDORSEMENT_MAX_ASN1_LENGTH));
SYSCALL unsigned int
os_endorsement_key1_get_app_secret(unsigned char *buffer PLENGTH(64));
SYSCALL unsigned int os_endorsement_key1_sign_data(
    unsigned char *src PLENGTH(srcLength), unsigned int srcLength,
    unsigned char *signature PLENGTH(ENDORSEMENT_MAX_ASN1_LENGTH));
SYSCALL unsigned int os_endorsement_key2_derive_sign_data(
    unsigned char *src PLENGTH(srcLength), unsigned int srcLength,
    unsigned char *signature PLENGTH(ENDORSEMENT_MAX_ASN1_LENGTH));

#if (defined(HAVE_BOLOS_NOTWIPED_ENDORSEMENT) &&                               \
     defined(HAVE_ENDORSEMENTS_DISPLAY))
SYSCALL unsigned int
os_endorsement_get_metadata(unsigned char index,
                            unsigned char *buffer PLENGTH(8));
#endif // (defined(HAVE_BOLOS_NOTWIPED_ENDORSEMENT) &&
       // defined(HAVE_ENDORSEMENTS_DISPLAY))
