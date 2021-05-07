#pragma once

#ifndef HAVE_BOLOS_NO_CUSTOMCA
#include "bolos_target.h"
#include "decorators.h"

/* ----------------------------------------------------------------------- */
/* -                         CUSTOM CERTIFICATE AUTHORITY                - */
/* ----------------------------------------------------------------------- */

// Verify the signature is issued from the custom certificate authority
SYSCALL unsigned int
os_customca_verify(unsigned char *hash PLENGTH(32),
                   unsigned char *sign PLENGTH(sign_length),
                   unsigned int sign_length);
#endif // HAVE_BOLOS_NO_CUSTOMCA
