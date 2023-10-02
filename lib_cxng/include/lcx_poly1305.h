
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
#if defined(HAVE_POLY1305)

/**
 * @file    lcx_poly1305.h
 * @brief   POLY1305 Message Authentication Code
 *
 * @section Description
 * 
 * POLY1305 generates a 16-byte tag which authenticates a message given a 32-byte key.
 * The 32-byte key can be made of two 16-byte keys, e.g. the first 16-byte key is an AES key
 * and the remaining 16-byte come from a random string. POLY1305 has been designed with the AES
 * but it can be used with any other secure cipher, e.g. Chacha20.
 *
 * @author  Ledger
 * @version 1.0
 **/


#ifndef LCX_POLY1305_H
#define LCX_POLY1305_H

#include "ox.h"
#include <stddef.h>

/**
 * @brief Poly1305 context
 */
typedef struct {
    uint32_t r[4];       ///< The value for 'r' (low 128 bits of the key)
    uint32_t s[4];       ///< The value for 's' (high 128 bits of the key)
    uint32_t acc[5];     ///< The accumulator number
    uint8_t  block[16];  ///< The current partial block of data
    size_t   block_len;  ///< The number of bytes stored in 'block'
} cx_poly1305_context_t;

#endif /* LCX_POLY1305_H */
#endif // HAVE_POLY1305
