
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

#ifdef HAVE_BLAKE2

#ifndef CX_BLAKE2B_H
#define CX_BLAKE2B_H

#include "lcx_wrappers.h"
#include "lcx_blake2.h"
#include <stdbool.h>
#include <stddef.h>

extern const cx_hash_info_t cx_blake2b_info;

cx_err_t cx_blake2b_update(cx_blake2b_t *ctx, const uint8_t *data, size_t len);
cx_err_t cx_blake2b_final(cx_blake2b_t *ctx, uint8_t *digest);
size_t cx_blake2b_get_output_size(const cx_blake2b_t *ctx);

struct cx_xblake_s {
    cx_blake2b_t blake2b;
    uint64_t     m[16];
    uint64_t     v[16];
    uint8_t      buffer[BLAKE2B_OUTBYTES];
    uint8_t      block1[BLAKE2B_BLOCKBYTES];
} ;
typedef struct cx_xblake_s cx_xblake_t;

#endif // CX_BLAKE2B_H

#endif // HAVE_BLAKE2
