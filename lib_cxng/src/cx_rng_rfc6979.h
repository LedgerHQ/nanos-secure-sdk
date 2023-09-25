
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

#ifndef CX_RNG_RFC6979_H
#define CX_RNG_RFC6979_H

#ifdef HAVE_RNG_RFC6979

#include "libcxng.h"
#include "cx_hash.h"
#include <stddef.h>
#include <stdint.h>

#define CX_RFC6979_BUFFER_LENGTH 64
#define CX_RFC6979_MAX_RLEN      66

typedef struct {
    uint8_t  v[CX_RFC6979_BUFFER_LENGTH + 1];
    uint8_t  k[CX_RFC6979_BUFFER_LENGTH];
    uint8_t  q[CX_RFC6979_MAX_RLEN];
    uint32_t q_len;
    uint32_t r_len;
    uint8_t  tmp[CX_RFC6979_MAX_RLEN];
    cx_md_t  hash_id;
    size_t   md_len;

    union {
#if (!defined(HAVE_SHA512) && !defined(HAVE_SHA384) && !defined(HAVE_SHA256) \
     && !defined(HAVE_SHA224))                                               \
    || !defined(HAVE_HMAC)
#error No hmac defined for rfc6979 support
#endif

        cx_hmac_t hmac;

#if defined(HAVE_SHA512) || defined(HAVE_SHA384)
        cx_hmac_sha512_t hmac_sha512;
#endif

#if defined(HAVE_SHA256) || defined(HAVE_SHA224)
        cx_hmac_sha256_t hmac_sha256;
#endif
    };
} cx_rnd_rfc6979_ctx_t;

WARN_UNUSED_RESULT cx_err_t cx_rng_rfc6979_init(
    cx_rnd_rfc6979_ctx_t *rfc_ctx,
    cx_md_t               hash_id,
    const uint8_t        *x,
    size_t                x_len,
    const uint8_t        *h1,
    size_t                h1_len,
    const uint8_t        *q,
    size_t                q_len
    /*const uint8_t *additional_input, size_t additional_input_len*/);

WARN_UNUSED_RESULT cx_err_t cx_rng_rfc6979_next(cx_rnd_rfc6979_ctx_t *rfc_ctx,
                                                uint8_t              *out,
                                                size_t                out_len);

#endif  // HAVE_RNG_RFC6979

#endif  // CX_RNG_RFC6979_H
