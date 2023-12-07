
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

#ifndef CX_SHA3_H
#define CX_SHA3_H

#ifdef HAVE_SHA3

#include "lcx_sha3.h"

extern const cx_hash_info_t cx_sha3_info;
extern const cx_hash_info_t cx_keccak_info;
extern const cx_hash_info_t cx_shake128_info;
extern const cx_hash_info_t cx_shake256_info;

WARN_UNUSED_RESULT cx_err_t cx_sha3_update(cx_sha3_t *ctx, const uint8_t *data, size_t len);
// No need to add WARN_UNUSED_RESULT to cx_sha3_final(), it always returns CX_OK
cx_err_t cx_sha3_final(cx_sha3_t *ctx, uint8_t *digest);
size_t   cx_sha3_get_output_size(const cx_sha3_t *ctx);

#endif  // HAVE_SHA3

#endif  // CX_SHA3_H
