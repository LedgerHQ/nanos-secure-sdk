
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
 * @file    lcx_sha256.h
 * @brief   SHA-2 (Secure Hash Algorithm 2)
 *
 * SHA-224 and SHA-256 are secure hash functions belonging to the SHA-2 family
 * with a digest length of 224 and 256 bits, respectively. The message length should
 * be less than 2<sup align = right>64</sup> bits.
 * Refer to <a href="https://csrc.nist.gov/publications/detail/fips/180/4/final">  FIPS 180-4 </a>
 * for more details.
 */

#if defined(HAVE_SHA256) || defined(HAVE_SHA224)

#ifndef LCX_SHA256_H
#define LCX_SHA256_H

#include "lcx_wrappers.h"
#include "lcx_hash.h"
#include <stddef.h>
#include <stdint.h>

#if defined(HAVE_SHA224)
/** SHA-224 message digest size */
#define CX_SHA224_SIZE 28
#endif

/** SHA-256 message digest size */
#define CX_SHA256_SIZE 32

/**
 * @brief SHA-224 and SHA-256 context
 */
struct cx_sha256_s {
  struct cx_hash_header_s header;  ///< @copydoc cx_ripemd160_s::header
  size_t blen;                     ///< @copydoc cx_ripemd160_s::blen
  uint8_t block[64];               ///< @copydoc cx_ripemd160_s::block
  uint8_t acc[8 * 4];              ///< @copydoc cx_ripemd160_s::acc
};
/** Convenience type. See #cx_sha256_s. */
typedef struct cx_sha256_s cx_sha256_t;

#if defined(HAVE_SHA224)
/**
 * @brief   Initialize a SHA-224 context.
 *
 * @param[out] hash Pointer to the context.
 *                  The context shall be in RAM.
 *
 * @return          Error code:
 *                  - CX_OK on success
 */
cx_err_t cx_sha224_init_no_throw(cx_sha256_t *hash);

static inline int cx_sha224_init ( cx_sha256_t * hash )
{
  cx_sha224_init_no_throw(hash);
  return CX_SHA224;
}
#endif

/**
 * @brief   Initialize a SHA-256 context.
 *
 * @param[out] hash Pointer to the context.
 *                  The context shall be in RAM.
 *
 * @return          Error code:
 *                  - CX_OK on success
 */
cx_err_t cx_sha256_init_no_throw(cx_sha256_t *hash);

/**
 * @brief   Initialize a SHA-256 context.
 *
 * @param[out] hash Pointer to the context.
 *                  The context shall be in RAM.
 *
 * @return          SHA256 identifier.
 */
static inline int cx_sha256_init ( cx_sha256_t * hash )
{
  cx_sha256_init_no_throw(hash);
  return CX_SHA256;
}

/**
 * @brief   Compute a one shot SHA-256 digest.
 *
 * @param[in]  in      Input data.
 *
 * @param[in]  len     Length of the input data.
 *
 * @param[out] out     Buffer where to store the digest.
 * 
 * @param[in]  out_len Length of the output.
 *                     This is actually 256 bits.
 *
 */
  size_t cx_hash_sha256(const uint8_t *in, size_t len, uint8_t *out, size_t out_len);

#endif

#endif // defined(HAVE_SHA256) || defined(HAVE_SHA224)
