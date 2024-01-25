
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

#ifndef CX_SHA512_H
#define CX_SHA512_H

#if defined(HAVE_SHA512) || defined(HAVE_SHA384)

#include <stdbool.h>
#include <stddef.h>
#include "lcx_sha512.h"

#ifdef HAVE_SHA384
extern const cx_hash_info_t cx_sha384_info;
#endif  // HAVE_SHA384

#ifdef HAVE_SHA512
extern const cx_hash_info_t cx_sha512_info;
#endif  // HAVE_SHA512

WARN_UNUSED_RESULT cx_err_t cx_sha512_update(cx_sha512_t *ctx, const uint8_t *data, size_t len);
// No need to add WARN_UNUSED_RESULT to cx_sha512_final(), it always returns CX_OK
cx_err_t cx_sha512_final(cx_sha512_t *ctx, uint8_t *digest);

#endif  // defined(HAVE_SHA512) || defined(HAVE_SHA384)

#endif  // CX_SHA512_H
