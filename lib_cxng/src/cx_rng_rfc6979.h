
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

#ifdef HAVE_RNG_RFC6979

#ifndef CX_RNG_RFC6979_H
#define CX_RNG_RFC6979_H

#include "libcxng.h"
#include "cx_hash.h"
#include <stddef.h>
#include <stdint.h>

#define CX_RFC6979_BUFFER_LENGTH                64
#define CX_RFC6979_MAX_RLEN                     66

typedef struct {
  uint8_t  v[CX_RFC6979_BUFFER_LENGTH + 1];
  uint8_t  k[CX_RFC6979_BUFFER_LENGTH];
  uint8_t  q[CX_RFC6979_MAX_RLEN];
  uint32_t q_len;
  uint32_t r_len;
  uint8_t  tmp[CX_RFC6979_MAX_RLEN];
  cx_md_t hash_id;
  size_t md_len;
  cx_hmac_t hmac;
} cx_rnd_rfc6979_ctx_t;

cx_err_t cx_rng_rfc6979_init(cx_rnd_rfc6979_ctx_t *rfc_ctx,
                             cx_md_t hash_id,
                             const uint8_t *x, size_t x_len,
                             const uint8_t *h1, size_t h1_len,
                             const uint8_t *q, size_t q_len
                             /*const uint8_t *additional_input, size_t additional_input_len*/);

cx_err_t cx_rng_rfc6979_next(cx_rnd_rfc6979_ctx_t *rfc_ctx,
                             uint8_t *out, size_t out_len);
#endif //CX_HMAC_DRBG_H

#endif //HAVE_RNG_RFC6979
