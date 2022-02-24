
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
 * @file    lcx_sha3.h
 * @brief   SHA-3 (Secure Hash Algorithm 3)
 *
 * SHA-3 specifies a family of secure hash functions
 * based on an instance of the KECCAK algorithm.
 * Refer to <a href="https://csrc.nist.gov/publications/detail/fips/202/final">  FIPS 202 </a>
 * for more details.
 */

#ifdef HAVE_SHA3

#ifndef LCX_SHA3_H
#define LCX_SHA3_H

#include "lcx_common.h"
#include "lcx_hash.h"
#include <stddef.h>

/**
 * @brief KECCAK, SHA3 and SHA3-XOF context
 */
struct cx_sha3_s {
  struct cx_hash_header_s header;  ///< @copydoc cx_ripemd160_s::header
  size_t output_size;              ///< Output digest size
  size_t block_size;               ///< Input block size
  size_t blen;                     ///< @copydoc cx_ripemd160_s::blen
  uint8_t block[200];              ///< @copydoc cx_ripemd160_s::block
  uint64bits_t acc[25];            ///< @copydoc cx_ripemd160_s::acc
};
/** Convenience type. See #cx_sha3_s. */
typedef struct cx_sha3_s cx_sha3_t;

/**
 * @brief   Initialize a SHA3 context.
 * 
 * @details Supported output sizes in bits:
 *            - 224
 *            - 256
 *            - 384
 *            - 512
 *
 * @param[out] hash Pointer to the SHA3 context.
 *                  The context shall be in RAM.
 *
 * @param[in]  size Length of the hash output in bits.
 * 
 * @return          Error code:
 *                  - CX_OK on success
 *                  - CX_INVALID_PARAMETER
 */
cx_err_t cx_sha3_init_no_throw(cx_sha3_t *hash, size_t size);

/**
 * @brief   Initialize a SHA3 context.
 * 
 * @details Supported output sizes in bits:
 *            - 224
 *            - 256
 *            - 384
 *            - 512
 *          
 *          This function throws an exception if the
 *          initialization fails.
 *
 * @param[out] hash Pointer to the SHA3 context.
 *                  The context shall be in RAM.
 *
 * @param[in]  size Length of the hash output in bits.
 * 
 * @return          SHA3 identifier.
 * 
 * @throws          CX_INVALID_PARAMETER
 */
static inline int cx_sha3_init ( cx_sha3_t * hash, size_t size )
{
  CX_THROW(cx_sha3_init_no_throw(hash, size));
  return CX_SHA3;
}

/**
 * @brief Initialize a KECCAK context.
 * 
 * @details Supported output sizes in bits:
 *            - 224
 *            - 256
 *            - 384
 *            - 512
 *
 *
 * @param[out] hash  Pointer to the KECCAK context.
 *                   The context shall be in RAM.
 *
 * @param[in]  size  Length of the hash output in bits.
 *
 * @return           Error code:
 *                   - CX_OK on success
 *                   - CX_INVALID_PARAMETER
 */
cx_err_t cx_keccak_init_no_throw(cx_sha3_t *hash, size_t size);

/**
 * @brief Initialize a KECCAK context.
 * 
 * @details Supported output sizes in bits:
 *            - 224
 *            - 256
 *            - 384
 *            - 512
 *          
 *          This function throws an exception if the
 *          initialization fails.
 *
 * @param[out] hash  Pointer to the KECCAK context.
 *                   The context shall be in RAM.
 *
 * @param[in]  size  Length of the hash output in bits.
 *
 * @return           KECCAK identifier.
 * 
 * @throws           CX_INVALID_PARAMETER
 */
static inline int cx_keccak_init ( cx_sha3_t * hash, size_t size )
{
  CX_THROW(cx_keccak_init_no_throw(hash, size));
  return CX_KECCAK;
}

/**
 * @brief   Initialize a SHA3-XOF context.
 * 
 * @details SHAKE128 is a SHA3-XOF (Extendable Output Function
 *          based on SHA3) with a 128-bit security.
 *          Supported output sizes in bits:
 *            - 256
 *            - 512
 *
 * @param[out] hash      Pointer to the context.
 *                       The context shall be in RAM.
 *
 * @param[in]  out_size  Length of the output in bits.
 *
 * @return               Error code:
 *                       - CX_OK on success
 *                       - CX_INVALID_PARAMETER
 */
cx_err_t cx_shake128_init_no_throw(cx_sha3_t *hash, size_t out_size);

/**
 * @brief   Initialize a SHA3-XOF context.
 * 
 * @details SHAKE128 is a SHA3-XOF (Extendable Output Function
 *          based on SHA3) with a 128-bit security.
 *          Supported output sizes in bits:
 *            - 256
 *            - 512
 *          
 *          This function throws an exception if the computation
 *          doesn't succeed.
 *
 * @param[out] hash      Pointer to the context.
 *                       The context shall be in RAM.
 *
 * @param[in]  out_size  Length of the output in bits.
 *
 * @return               SHAKE128 identifier.
 * 
 * @throws               CX_INVALID_PARAMETER
 */
static inline int cx_shake128_init ( cx_sha3_t * hash, unsigned int out_size )
{
  CX_THROW(cx_shake128_init_no_throw(hash, out_size));
  return CX_SHAKE128;
}

/**
 * @brief   Initialize a SHA3-XOF context.
 * 
 * @details SHAKE256 is a SHA3-XOF (Extendable Output Function
 *          based on SHA3) with a 256-bit security.
 *          Supported output sizes in bits:
 *            - 256
 *            - 512
 *
 * @param[out] hash      Pointer to the context.
 *                       The context shall be in RAM.
 *
 * @param[in]  out_size  Length of the output in bits.
 *
 * @return               Error code:
 *                       - CX_OK on success
 *                       - CX_INVALID_PARAMETER
 */
cx_err_t cx_shake256_init_no_throw(cx_sha3_t *hash, size_t out_size);

/**
 * @brief   Initialize a SHA3-XOF context.
 * 
 * @details SHAKE256 is a SHA3-XOF (Extendable Output Function
 *          based on SHA3) with a 256-bit security.
 *          Supported output sizes in bits:
 *            - 256
 *            - 512
 *          
 *          This function throws an exception if the computation
 *          doesn't succeed.
 *
 * @param[out] hash      Pointer to the context.
 *                       The context shall be in RAM.
 *
 * @param[in]  out_size  Length of the output in bits.
 *
 * @return               SHA256 identifier.
 * 
 * @throws               CX_INVALID_PARAMETER
 */
static inline int cx_shake256_init ( cx_sha3_t * hash, unsigned int out_size )
{
  CX_THROW(cx_shake256_init_no_throw(hash, out_size));
  return CX_SHAKE256;
}

/**
 * @brief   Initialize a SHA3-XOF context.
 * 
 * @details This can be used to initialize either SHAKE128
 *          or SHAKE256.
 *          Supported output sizes in bits:
 *            - 256
 *            - 512
 *
 * @param[out] hash         Pointer to the context.
 *                          The context shall be in RAM.
 *
 * @param[in]  size         Length of SHA3 digest in bits.
 * 
 * @param[in]  out_length   Length of the output in bytes.
 *
 * @return                  Error code:
 *                          - CX_OK on success
 *                          - CX_INVALID_PARAMETER
 */
cx_err_t cx_sha3_xof_init_no_throw(cx_sha3_t *hash, size_t size, size_t out_length);

/**
 * @brief   Initialize a SHA3-XOF context.
 * 
 * @details This can be used to initialize either SHAKE128
 *          or SHAKE256.
 *          Supported output sizes in bits:
 *            - 256
 *            - 512
 *          
 *          This function throws an exception if the computation
 *          doesn't succeed.
 *
 * @param[out] hash         Pointer to the context.
 *                          The context shall be in RAM.
 *
 * @param[in]  size         Length of SHA3 digest in bits.
 * 
 * @param[in]  out_length   Length of the output in bytes.
 *
 * @return                  Either SHAKE128 or SHAKE256 identifier.
 * 
 * @throws                  CX_INVALID_PARAMETER
 */
static inline int cx_sha3_xof_init ( cx_sha3_t * hash, unsigned int size, unsigned int out_length )
{
  CX_THROW(cx_sha3_xof_init_no_throw(hash, size, out_length));

  if (size == 128) {
    return CX_SHAKE128;
  } else {
    return CX_SHAKE256;
  }
}

#endif

#endif // HAVE_SHA3
