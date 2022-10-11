
/*******************************************************************************
*   Ledger Nano S - Secure firmware
*   (c) 2022 Ledger
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

#ifdef HAVE_PBKDF2

#ifndef CX_PBKDF2_H
#define CX_PBKDF2_H

#include "lcx_hmac.h"

#include <stddef.h>
#include <stdint.h>

#define PBKDF2_BUFFER_LENGTH                    64

/* ========= PBKDF2 ========= */
struct cx_pbkdf2_s {
  // salt buffer used to initialize each pbkdf2 turn.
  uint8_t   salt[384];
  uint8_t   sha512out[64]; // avoid stack usage in derive_and_set_seed

  uint8_t   work[PBKDF2_BUFFER_LENGTH];
  uint8_t   md1[PBKDF2_BUFFER_LENGTH];
  union {
    #if !defined(HAVE_SHA512) && !defined(HAVE_SHA384) && !defined(HAVE_SHA256) && !defined(HAVE_SHA224)
    #error No hmac defined for pbkdf2 support
    #endif

    cx_hmac_t hmac_ctx;

    #if defined(HAVE_HMAC) && (defined(HAVE_SHA512) || defined(HAVE_SHA384))
    cx_hmac_sha512_t hmac_sha512;
    #endif

    #if defined(HAVE_HMAC) && (defined(HAVE_SHA256) || defined(HAVE_SHA224))
    cx_hmac_sha256_t hmac_sha256;
    #endif
  };
};
typedef struct cx_pbkdf2_s cx_pbkdf2_t;

cx_err_t cx_pbkdf2_hmac(cx_md_t        md_type,
                        const uint8_t *password,
                        size_t         password_len,
                        const uint8_t *salt,
                        size_t         salt_len,
                        uint32_t       iterations,
                        uint8_t *      key,
                        size_t         key_len);

#endif // CX_PBKDF2_H
#endif // HAVE_PBKDF2
