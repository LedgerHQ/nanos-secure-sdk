
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

#ifdef HAVE_ECC

#ifndef CX_ECFP_H
#define CX_ECFP_H

#include <stddef.h>
#include <stdint.h>
#include "lcx_ecfp.h"

#define CX_REG_ECC_SIZE 80

#if defined(BOLOS_RELEASE) && defined(HAVE_ECC_WITH_NO_RANDOMIZE)
#error HAVE_ECC_WITH_NO_RANDOMIZE not allowed for release
#endif // HAVE_ECC_WITH_NO_RANDOMIZE

#define CX_BITLEN2BYTELEN(l) (((uint32_t)(l) + 7) >> 3)


/* Encoding/Decoding */

size_t cx_ecfp_encode_sig_der(uint8_t *       sig,
                              size_t          sig_len,
                              const uint8_t  *r,
                              size_t          r_len,
                              const uint8_t  *s,
                              size_t          s_len);
int    cx_ecfp_decode_sig_der(const uint8_t  *sig,
                              size_t          sig_len,
                              size_t          max_size,
                              const uint8_t **r,
                              size_t *        r_len,
                              const uint8_t **s,
                              size_t *        s_len);

#endif // CX_ECFP_H

#endif // HAVE_ECC
