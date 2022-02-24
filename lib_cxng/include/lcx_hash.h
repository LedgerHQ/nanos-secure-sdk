
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
 * @file    lcx_hash.h
 * @brief   Hash functions.
 *
 * A hash function maps data of arbitrary size to a bit array of a fixed size,
 * called the message digest. Various hash functions are available:
 *   - BLAKE2B
 *   - GROESTL
 *   - KECCAK (Pre SHA3)
 *   - RIPEMD-160
 *   - SHAKE-128
 *   - SHAKE-256
 *   - SHA224
 *   - SHA256
 *   - SHA3
 *   - SHA384
 *   - SHA3_256
 *   - SHA3_512
 *   - SHA512
 */

#ifdef HAVE_HASH

#ifndef LCX_HASH_H
#define LCX_HASH_H

#include "cx_errors.h"
#include "lcx_wrappers.h"
#include "lcx_common.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/** Message digest algorithm identifiers. */
enum cx_md_e {
  CX_NONE = 0,                          ///< No message digest algorithm
  // 20 bytes
  CX_RIPEMD160 = 1,                     ///< RIPEMD160 digest
  // 28 bytes
  CX_SHA224 = 2,                        ///< SHA224 digest
  // 32 bytes
  CX_SHA256 = 3,                        ///< SHA256 digest
  // 48 bytes 
  CX_SHA384 = 4,                        ///< SHA384 digest
  // 64 bytes 
  CX_SHA512 = 5,                        ///< SHA512 digest
  // 28,32,48,64 bytes
  CX_KECCAK = 6,                        ///< Keccak (pre-SHA3) digest
  // 28,32,48,64 bytes
  CX_SHA3 = 7,                          ///< SHA3 Digest
  CX_GROESTL = 8,                       ///< Groestl digest
  CX_BLAKE2B = 9,                       ///< Blake digest
  // any bytes
  CX_SHAKE128 = 10,                     ///< SHAKE-128 digest
  // any bytes
  CX_SHAKE256 = 11,                     ///< SHAKE-256 digest
  CX_SHA3_256 = 12,                     ///< SHA3-256 digest
  CX_SHA3_512 = 13,                     ///< SHA3-512 digest
};
/** Convenience type. See #cx_md_e. */
typedef enum cx_md_e cx_md_t;

/**
 * @internal
 * Maximum size of message for any digest. The size is given in block.
 */
#define CX_HASH_MAX_BLOCK_COUNT 65535

/** Convenience type. See #cx_hash_header_s. */
typedef struct cx_hash_header_s cx_hash_t;

/**
 * @brief Hash description.
 */
typedef struct {
  cx_md_t md_type;                                                          ///< Message digest algorithm identifier
  size_t  output_size;                                                      ///< Output size
  size_t  block_size;                                                       ///< Block size
  cx_err_t (*init_func)(cx_hash_t *ctx);                                    ///< Pointer to the initialization function
  cx_err_t (*update_func)(cx_hash_t *ctx, const uint8_t *data, size_t len); ///< Pointer to the update function
  cx_err_t (*finish_func)(cx_hash_t *ctx, uint8_t *digest);                 ///< Pointer to the final function
  cx_err_t (*init_ex_func)(cx_hash_t *ctx, size_t output_size);             ///< Pointer to the initialization function for extendable output 
  size_t (*output_size_func)(const cx_hash_t *ctx);                         ///< Pointer to the output size function
} cx_hash_info_t;

/**
 * @brief Common message digest context, used as abstract type.
 */
struct cx_hash_header_s {
  const cx_hash_info_t *info;            ///< Hash description
  uint32_t counter;                      ///< Number of already processed blocks
};

size_t cx_hash_get_size(const cx_hash_t *ctx);

/**
 * @brief   Hash data according to the specified algorithm.
 *
 * @param[in]  hash    Pointer to the hash context.
 *                     Shall be in RAM.
 *                     Should be called with a cast.
 *
 * @param[in]  mode    Crypto flag. Supported flag: CX_LAST. If set:
 *                       - the structure is not modified after finishing
 *                       - if out is not NULL, the message digest is stored in out
 *                       - the context is NOT automatically re-initialized.
 *
 * @param[in]  in      Input data to be hashed.
 *
 * @param[in]  len     Length of the input data.
 *
 * @param[out] out     Buffer where to store the message digest:
 *                       - NULL (ignored) if CX_LAST is NOT set
 *                       - message digest if CX_LAST is set
 *
 * @param[out] out_len The size of the output buffer or 0 if out is NULL.
 *                     If buffer is too small to store the hash a exception is returned.
 *
 * @return             Error code:
 *                     - CX_OK on success
 *                     - INVALID_PARAMETER
 *                     - CX_INVALID_PARAMETER
 */
cx_err_t cx_hash_no_throw(cx_hash_t *hash, uint32_t mode, const uint8_t *in, size_t len, uint8_t *out, size_t out_len);

/**
 * @brief   Hash data according to the specified algorithm.
 * 
 * @details This function throws an exception if the computation
 *          doesn't succeed.
 *
 * @param[in]  hash    Pointer to the hash context.
 *                     Shall be in RAM.
 *                     Should be called with a cast.
 *
 * @param[in]  mode    Crypto flag. Supported flag: CX_LAST. If set:
 *                       - the structure is not modified after finishing
 *                       - if out is not NULL, the message digest is stored in out
 *                       - the context is NOT automatically re-initialized.
 *
 * @param[in]  in      Input data to be hashed.
 *
 * @param[in]  len     Length of the input data.
 *
 * @param[out] out     Buffer where to store the message digest:
 *                       - NULL (ignored) if CX_LAST is NOT set
 *                       - message digest if CX_LAST is set
 *
 * @param[out] out_len The size of the output buffer or 0 if out is NULL.
 *                     If buffer is too small to store the hash a exception is returned.
 *
 * @return             Length of the digest.
 * 
 * @throws             INVALID_PARAMETER
 * @throws             CX_INVALID_PARAMETER
 */
static inline int cx_hash ( cx_hash_t * hash, int mode, const unsigned char * in, unsigned int len, unsigned char * out, unsigned int out_len )
{
  CX_THROW(cx_hash_no_throw(hash, mode, in, len, out, out_len));
  return cx_hash_get_size(hash);
}

/**
 * @brief   Initialize a hash context.
 *
 * @param[out] hash    Pointer to the context to be initialized.
 *                     The context shall be in RAM.
 *
 * @param[in]  hash_id Message digest algorithm identifier.
 *
 * @return             Error code:
 *                     - CX_OK on success
 *                     - CX_INVALID_PARAMETER
 */
cx_err_t cx_hash_init(cx_hash_t *hash, cx_md_t hash_id);

/**
 * @brief   Initialize a hash context.
 * 
 * @details Initialize a hash context with a chosen output length
 *          (typically for eXtendable Output Functions (XOF)).
 *
 * @param[out] hash        Pointer to the context to be initialized.
 *                         The context shall be in RAM.
 *
 * @param [in] hash_id     Hash algorithm identifier. Typically:
 *                           - CX_BLAKE2B
 *                           - CX_GROESTL
 *                           - CX_SHAKE128
 *                           - CX_SHAKE256
 *
 * @param [in] output_size Length of the output.
 * 
 * @return                 Error code:
 *                         - CX_OK on success
 *                         - CX_INVALID_PARAMETER
 */
cx_err_t cx_hash_init_ex(cx_hash_t *hash, cx_md_t hash_id, size_t output_size);

/**
 * @brief   Add more data to hash.
 * 
 * @details A call to this function is equivalent to:
 *          *cx_hash_no_throw(hash, 0, in, in_len, NULL, 0)*.
 *
 * @param[out] hash   Pointer to the hash context.
 *
 * @param[in]  in     Input data to add to the context.
 *
 * @param[in]  in_len Length of the input data.
 *
 * @return            Error code:
 *                    - CX_OK on success
 *                    - CX_INVALID_PARAMETER
 *                    - INVALID_PARAMETER
 */
cx_err_t cx_hash_update(cx_hash_t *hash, const uint8_t *in, size_t in_len);

/**
 * @brief   Finalize the hash. 
 * 
 * @details A call to this function is equivalent to:
 *          *cx_hash_no_throw(hash, CX_LAST, NULL, 0, out, out_len)*.
 *
 * @param[in]  hash   Pointer to the hash context.
 *
 * @param[out] digest The message digest.
 * 
 * @return            Error code:
 *                    - CX_OK on success
 */
cx_err_t cx_hash_final(cx_hash_t *hash, uint8_t *digest);

#endif

#endif // HAVE_HASH
