
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

#ifndef CX_SHA256_H
#define CX_SHA256_H

#if defined(HAVE_SHA256) || defined(HAVE_SHA224)

#include "lcx_sha256.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef HAVE_SHA224
extern const cx_hash_info_t cx_sha224_info;
#endif  // HAVE_SHA224

#ifdef HAVE_SHA256
extern const cx_hash_info_t cx_sha256_info;
#endif  // HAVE_SHA256

WARN_UNUSED_RESULT cx_err_t cx_sha256_update(cx_sha256_t *ctx, const uint8_t *data, size_t len);
// No need to add WARN_UNUSED_RESULT to cx_sha256_final(), it always returns CX_OK
cx_err_t cx_sha256_final(cx_sha256_t *ctx, uint8_t *digest);

#endif  // HAVE_SHA256

#endif  // CX_SHA256_H
