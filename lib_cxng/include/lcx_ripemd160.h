
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

/**
 * @file    lcx_ripemd160.h
 * @brief   RIPEMD-160 hash function
 *
 * RIPEMD-160 is a 160-bit cryptographic hash function. Refer to <a href = "https://homes.esat.kuleuven.be/~bosselae/ripemd160.html"> 
 * RIPEMD-160 </a> for more details.
 */

#ifdef HAVE_RIPEMD160

#ifndef LCX_RIPEMD160_H
#define LCX_RIPEMD160_H

#include "lcx_wrappers.h"
#include "lcx_hash.h"

/** RIPEMD160 message digest size */
#define CX_RIPEMD160_SIZE 20

/**
 * @brief RIPEMD-160 context.
 */
struct cx_ripemd160_s {
  struct cx_hash_header_s header;   ///< See #cx_hash_header_s
  size_t blen;                      ///< Pending partial block length
  uint8_t block[64];                ///< Pending partial block
  // After finishing the digest,
  // contains the digest if correct 
  // parameters are passed.
  uint8_t acc[5 * 4];               ///< Current digest state
};
/** Convenience type. See #cx_ripemd160_s. */
typedef struct cx_ripemd160_s cx_ripemd160_t;

/**
 * @brief   Initialize a RIPEMD-160 context.
 *
 * @param[out] hash Pointer to the context.
 *                  The context shall be in RAM.
 *
 * @return          Error code:
 *                  - CX_OK on success
 */
cx_err_t cx_ripemd160_init_no_throw(cx_ripemd160_t *hash);

/**
 * @brief   Initialize a RIPEMD-160 context.
 *
 * @param[out] hash Pointer to the context.
 *                  The context shall be in RAM.
 *
 * @return          RIPEMD160 identifier.
 */
static inline int cx_ripemd160_init ( cx_ripemd160_t * hash )
{
  cx_ripemd160_init_no_throw(hash);
  return CX_RIPEMD160;
}

#endif

#endif // HAVE_RIPEMD160
