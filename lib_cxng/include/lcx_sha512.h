
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
 * @file    lcx_sha512.h
 * @brief   SHA-2 (Secure Hash Algorithm 2)
 *
 * SHA-384 and SHA-512 are secure hash functions belonging to the SHA-2 family
 * with a digest length of 384 and 512 bits, respectively. The message length should
 * be less than 2<sup align = right>128</sup> bits.
 * Refer to <a href="https://csrc.nist.gov/publications/detail/fips/180/4/final">  FIPS 180-4 </a>
 * for more details.
 */

#if defined(HAVE_SHA384) || defined(HAVE_SHA512)

#ifndef LCX_SHA512_H
#define LCX_SHA512_H

/** SHA-384 message digest size */
#define CX_SHA384_SIZE 48
/** SHA-512 message digest size */
#define CX_SHA512_SIZE 64

/**
 * @brief SHA-384 and SHA-512 context
 */
struct cx_sha512_s {
  struct cx_hash_header_s header;  ///< @copydoc cx_ripemd160_s::header
  size_t blen;                     ///< @copydoc cx_ripemd160_s::blen
  uint8_t block[128];              ///< @copydoc cx_ripemd160_s::block
  uint8_t acc[8 * 8];              ///< @copydoc cx_ripemd160_s::acc
};
/** Convenience type. See #cx_sha512_s. */
typedef struct cx_sha512_s cx_sha512_t;

/**
 * @brief   Initialize a SHA-384 context.
 *
 * @param[out] hash Pointer to the context.
 *                  The context shall be in RAM.
 *
 * @return          Error code:
 *                  - CX_OK on success
 */
cx_err_t cx_sha384_init_no_throw(cx_sha512_t *hash);

/**
 * @brief   Initialize a SHA-384 context.
 *
 * @param[out] hash Pointer to the context.
 *                  The context shall be in RAM.
 *
 * @return          SHA384 identifier.
 */
static inline int cx_sha384_init ( cx_sha512_t * hash )
{
  cx_sha384_init_no_throw(hash);
  return CX_SHA384;
}

/**
 * @brief   Initialize a SHA-512 context.
 *
 * @param[out] hash Pointer to the context.
 *                  The context shall be in RAM.
 *
 * @return          Error code:
 *                  - CX_OK on success
 */
cx_err_t cx_sha512_init_no_throw(cx_sha512_t *hash);

/**
 * @brief   Initialize a SHA-512 context.
 *
 * @param[out] hash Pointer to the context.
 *                  The context shall be in RAM.
 *
 * @return          SHA512 identifier.
 */
static inline int cx_sha512_init ( cx_sha512_t * hash )
{
  cx_sha512_init_no_throw(hash);
  return CX_SHA512;
}

/**
 * @brief   Compute a one shot SHA-512 digest.
 *
 * @param[in]  in      Input data.
 *
 * @param[in]  in_len  Length of the input data.
 *
 * @param[out] out     Buffer where to store the output.
 *
 * @param[out] out_len Length of the output.
 *                     This is actually 512 bits.
 */
  size_t cx_hash_sha512(const uint8_t *in, size_t in_len, uint8_t *out, size_t out_len);

#endif

#endif // defined(HAVE_SHA384) || defined(HAVE_SHA512)
