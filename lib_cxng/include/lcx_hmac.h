
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
 * @file    lcx_hmac.h
 * @brief   HMAC (Keyed-Hash Message Authentication Code)
 *
 * A HMAC is a specific type of message authentication code
 * which involves a hash function and a secret key. It enables
 * the verification of the integrity and the authenticity of a message.
 */
#ifdef HAVE_HMAC

#ifndef LCX_HMAC_H
#define LCX_HMAC_H

#include "lcx_wrappers.h"
#include "lcx_hash.h"
#include "lcx_ripemd160.h"
#include "lcx_sha256.h"
#include "lcx_sha512.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief HMAC context, abstract type
 */
typedef struct {
  uint8_t   key[128];       ///< Key
  cx_hash_t hash_ctx;       ///< Hash context
} cx_hmac_t;


#ifdef HAVE_RIPEMD160

/**
 * @brief HMAC context, concrete type for RIPEMD160
 */
typedef struct {
  uint8_t        key[128];  ///< Key
  cx_ripemd160_t hash_ctx;  ///< Hash context
} cx_hmac_ripemd160_t;


/**
 * @brief   Initialize a HMAC-RIPEMD160 context.
 *
 * @param[out] hmac    Pointer to the HMAC context.
 *                     The context shall be in RAM.
 *
 * @param[in]  key     Pointer to the HMAC key value.
 *                     If a key has been set, passing
 *                     NULL pointeur will reinitialize
 *                     the context with the previously set key.
 *
 * @param [in] key_len Length of the key.
 *                     The key length shall be less than 64 bytes.
 *
 * @return             Error code:
 *                     - CX_OK on success
 *                     - CX_INVALID_PARAMETER
 */
cx_err_t cx_hmac_ripemd160_init_no_throw(cx_hmac_ripemd160_t *hmac, const uint8_t *key, size_t key_len);

/**
 * @brief   Initialize a HMAC-RIPEMD160 context.
 * 
 * @details This function throws an exception if the
 *          initialization fails.
 *
 * @param[out] hmac    Pointer to the HMAC context.
 *                     The context shall be in RAM.
 *
 * @param[in]  key     Pointer to the HMAC key value.
 *                     If a key has been set, passing
 *                     NULL pointeur will reinitialize
 *                     the context with the previously set key.
 *
 * @param [in] key_len Length of the key.
 *                     The key length shall be less than 64 bytes.
 *
 * @return             RIPEMD160 identifier.
 * 
 * @throws             CX_INVALID_PARAMETER
 */
static inline int cx_hmac_ripemd160_init ( cx_hmac_ripemd160_t * hmac, const unsigned char * key, unsigned int key_len )
{
  CX_THROW(cx_hmac_ripemd160_init_no_throw(hmac, key, key_len));
  return CX_RIPEMD160;
}
#endif


#if defined(HAVE_SHA224) || defined(HAVE_SHA256)
/**
 * @brief HMAC context, concrete type for SHA-224/SHA-256
 */
typedef struct {
  uint8_t     key[128];  ///< Key
  cx_sha256_t hash_ctx;  ///< Hash context
} cx_hmac_sha256_t;
#endif

#ifdef HAVE_SHA224

/**
 * @brief   Initialize a HMAC-SHA224 context.
 * 
 * @param[out] hmac    Pointer to the HMAC context.
 *                     The context shall be in RAM.
 *
 * @param[in]  key     Pointer to the HMAC key value.
 *                     If a key has been set, passing
 *                     NULL pointeur will reinitialize
 *                     the context with the previously set key.
 *
 * @param [in] key_len Length of the key.
 *                     The key length shall be less than 64 bytes.
 *
 * @return             Error code:
 *                     - CX_OK on success
 *                     - CX_INVALID_PARAMETER
 */
cx_err_t cx_hmac_sha224_init(cx_hmac_sha256_t *hmac, const uint8_t *key, unsigned int key_len);
#endif

#ifdef HAVE_SHA256

/**
 * @brief   Initialize a HMAC-SHA256 context.
 * 
 * @param[out] hmac    Pointer to the HMAC context.
 *                     The context shall be in RAM.
 *
 * @param[in]  key     Pointer to the HMAC key value.
 *                     If a key has been set, passing
 *                     NULL pointeur will reinitialize
 *                     the context with the previously set key.
 *
 * @param [in] key_len Length of the key.
 *                     The key length shall be less than 64 bytes.
 *
 * @return             Error code:
 *                     - CX_OK on success
 *                     - CX_INVALID_PARAMETER
 */
cx_err_t cx_hmac_sha256_init_no_throw(cx_hmac_sha256_t *hmac, const uint8_t *key, size_t key_len);

/**
 * @brief   Initialize a HMAC-SHA256 context.
 *
 * @details This function throws an exception if
 *          the initialization fails.
 * 
 * @param[out] hmac    Pointer to the HMAC context.
 *                     The context shall be in RAM.
 *
 * @param[in]  key     Pointer to the HMAC key value.
 *                     If a key has been set, passing
 *                     NULL pointeur will reinitialize
 *                     the context with the previously set key.
 *
 * @param [in] key_len Length of the key.
 *                     The key length shall be less than 64 bytes.
 *
 * @return             SHA256 identifier.
 * 
 * @throws             CX_INVALID_PARAMETER
 */
static inline int cx_hmac_sha256_init ( cx_hmac_sha256_t * hmac, const unsigned char * key, unsigned int key_len )
{
  CX_THROW(cx_hmac_sha256_init_no_throw(hmac, key, key_len));
  return CX_SHA256;
}

/**
 * @brief   Compute a HMAC value using SHA256.
 *
 * @param[in]  key     HMAC key value.
 *
 * @param[in]  key_len Length of the HMAC key.
 *
 * @param[in]  in      Input data.
 *
 * @param[in]  len     Length of the input data.
 *
 * @param[out] mac     Computed HMAC value.
 *
 * @param[in]  mac_len Size of the output buffer.
 *                     The buffer size must be larger
 *                     than the length of the HMAC value.
 * 
 * @return             Length of the HMAC value.
 */
size_t cx_hmac_sha256(const uint8_t *key, size_t key_len, const uint8_t *in, size_t len, uint8_t *mac, size_t mac_len);

#endif


#if defined(HAVE_SHA384) || defined(HAVE_SHA512)
/**
 * @brief HMAC context, concrete type for SHA-384/SHA-512
 */
typedef struct {
  uint8_t     key[128];  ///< Key
  cx_sha512_t hash_ctx;  ///< Hash context
} cx_hmac_sha512_t;
#endif

#ifdef HAVE_SHA384

/**
 * @brief   Initialize a HMAC-SHA384 context.
 * 
 * @param[out] hmac    Pointer to the context.
 *                     The context shall be in RAM.
 *
 * @param[in]  key     Pointer to the HMAC key value.
 *                     If a key has been set, passing
 *                     NULL pointeur will reinitialize
 *                     the context with the previously set key.
 *
 * @param[in]  key_len Length of the key.
 *                     The key length shall be less than 128 bytes.
 *
 * @return             Error code:
 *                     - CX_OK on success
 *                     - CX_INVALID_PARAMETER
 */
cx_err_t cx_hmac_sha384_init(cx_hmac_sha512_t *hmac, const uint8_t *key, unsigned int key_len);
#endif

#ifdef HAVE_SHA512

/**
 * @brief   Initialize a HMAC-SHA512 context.
 * 
 * @param[out] hmac    Pointer to the context.
 *                     The context shall be in RAM.
 *
 * @param[in]  key     Pointer to the HMAC key value.
 *                     If a key has been set, passing
 *                     NULL pointeur will reinitialize
 *                     the context with the previously set key.
 *
 * @param[in]  key_len Length of the key.
 *                     The key length shall be less than 128 bytes.
 *
 * @return             Error code:
 *                     - CX_OK on success
 *                     - CX_INVALID_PARAMETER
 */
cx_err_t cx_hmac_sha512_init_no_throw(cx_hmac_sha512_t *hmac, const uint8_t *key, size_t key_len);

/**
 * @brief   Initialize a HMAC-SHA512 context.
 * 
 * @details This function throws an exception if
 *          if the initialization fails.
 *
 * @param[out] hmac    Pointer to the context.
 *                     The context shall be in RAM.
 *
 * @param[in]  key     Pointer to the HMAC key value.
 *                     If a key has been set, passing
 *                     NULL pointeur will reinitialize
 *                     the context with the previously set key.
 *
 * @param[in]  key_len Length of the key.
 *                     The key length shall be less than 128 bytes.
 *
 * @return             SHA512 identifier.
 * 
 * @throws             CX_INVALID_PARAMETER
 */
static inline int cx_hmac_sha512_init ( cx_hmac_sha512_t * hmac, const unsigned char * key, unsigned int key_len )
{
  CX_THROW(cx_hmac_sha512_init_no_throw(hmac, key, key_len));
  return CX_SHA512;
}

/**
 * @brief   Compute a HMAC value using SHA512.
 *
 * @param[in]  key     HMAC key value.
 *
 * @param[in]  key_len Length of the HMAC key.
 *
 * @param[in]  in      Input data.
 *
 * @param[in]  len     Length of the input data.
 *
 * @param[out] mac     Computed HMAC value.
 *
 * @param[in]  mac_len Size of the output buffer.
 *                     The buffer size must be larger
 *                     than the length of the HMAC value.
 * 
 * @return             Length of the HMAC value.
 */
size_t cx_hmac_sha512(const uint8_t *key, size_t key_len, const uint8_t *in, size_t len, uint8_t *mac, size_t mac_len);

#endif

/**
 * @brief   Compute a HMAC value according to the specified
 *          hash function.
 * 
 * @param[in]  hmac    Pointer to the HMAC context.
 *                     The context shall be initialized with
 *                     one of the initialization functions.
 *                     The context shall be in RAM.
 *                     The function shall be called with the cast
 *                     (cx_hmac_t *).
 *
 * @param[in]  mode    Crypto mode flags. Supported flags:
 *                       - CX_LAST
 *                       - CX_NO_REINIT
 *                     If CX_LAST is set and CX_NO_REINIT is not set, the context is
 *                     reinitialized.
 *
 * @param[in]  in      Input data to add to the context.
 *
 * @param[in]  len     Length of the input data.
 *
 * @param[out] mac     Pointer to the computed HMAC or NULL pointer (if CX_LAST is not set).
 *
 * @param[in]  mac_len Length of the output buffer if not NULL, 0 otherwise.
 *                     The buffer size must be larger
 *                     than the length of the HMAC value.
 *
 * @return             Error code:
 *                     - CX_OK on success
 *                     - CX_INVALID_PARAMETER
 */
cx_err_t cx_hmac_no_throw(cx_hmac_t *hmac, uint32_t mode, const uint8_t *in, size_t len, uint8_t *mac, size_t mac_len);

/**
 * @brief   Compute a HMAC value according to the specified
 *          hash function.
 * 
 * @details This function throws an exception
 *          if the computation doesn't succeed.
 * 
 * @param[in]  hmac    Pointer to the HMAC context.
 *                     The context shall be initialized with
 *                     one of the initialization functions.
 *                     The context shall be in RAM.
 *                     The function shall be called with the cast
 *                     (cx_hmac_t *).
 *
 * @param[in]  mode    Crypto mode flags. Supported flags:
 *                       - CX_LAST
 *                       - CX_NO_REINIT
 *                     If CX_LAST is set and CX_NO_REINIT is not set, the context is
 *                     reinitialized.
 *
 * @param[in]  in      Input data to add to the context.
 *
 * @param[in]  len     Length of the input data.
 *
 * @param[out] mac     Pointer to the computed HMAC or NULL pointer (if CX_LAST is not set).
 *
 * @param[in]  mac_len Length of the output buffer if not NULL, 0 otherwise.
 *                     The buffer size must be larger
 *                     than the length of the HMAC value.
 *
 * @return             Identifier of the hash function.
 * 
 * @throws             CX_INVALID_PARAMETER
 */
static inline int cx_hmac ( cx_hmac_t * hmac, int mode, const unsigned char * in, unsigned int len, unsigned char * mac, unsigned int mac_len )
{
  CX_THROW(cx_hmac_no_throw(hmac, mode, in, len, mac, mac_len));

  switch (hmac->hash_ctx.info->md_type) {
#ifdef HAVE_SHA224
  case CX_SHA224: return CX_SHA224_SIZE;
#endif
#ifdef HAVE_SHA256
  case CX_SHA256: return CX_SHA256_SIZE;
#endif
#ifdef HAVE_SHA384
  case CX_SHA384: return CX_SHA384_SIZE;
#endif
#ifdef HAVE_SHA512
  case CX_SHA512: return CX_SHA512_SIZE;
#endif
#ifdef HAVE_RIPEMD160
  case CX_RIPEMD160: return CX_RIPEMD160_SIZE;
#endif
  default:
    CX_THROW(CX_INVALID_PARAMETER);
    return 0;
  }
}

/**
 * @brief   Initialize a HMAC context.
 * 
 * @param[out] hmac    Pointer to the context.
 *                     The context shall be in RAM.
 *
 * @param[in]  hash_id The message digest algorithm identifier
 *
 * @param[in]  key     Pointer to the HMAC key value.
 *                     If a key has been set, passing
 *                     NULL pointeur will reinitialize
 *                     the context with the previously set key.
 *
 * @param[in]  key_len Length of the key.
 *                     The key length shall be less than 128 bytes.
 *
 * @return             Error code:
 *                     - CX_OK on success
 *                     - CX_INVALID_PARAMETER
 */
cx_err_t cx_hmac_init(cx_hmac_t *hmac, cx_md_t hash_id, const uint8_t *key, size_t key_len);

/**
 * @brief   Add more data to compute the HMAC.
 * 
 * @details A call to this function is equivalent to:
 *          *cx_hmac_no_throw(hmac, 0, in, in_len, NULL, 0)*.
 *
 * @param[out] hmac   Pointer to the HMAC context.
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
cx_err_t cx_hmac_update(cx_hmac_t *hmac, const uint8_t *in, size_t in_len);

/**
 * @brief   Finalize the HMAC algorithm.
 * 
 * @details A call to this function is
 *          equivalent to *cx_hmac_no_throw(hash, CX_LAST, NULL, 0, out, out_len)*.
 *
 * @param[in]  ctx     Pointer to the HMAC context.
 * @param[out] out     Computed HMAC value is CX_LAST is set.
 *
 * @param[in]  out_len Length of the output (the most significant bytes).
 *
 * @return             Error code:
 *                     - CX_OK on success
 */
cx_err_t cx_hmac_final(cx_hmac_t *ctx, uint8_t *out, size_t *out_len) ;

#endif

#endif // HAVE_HMAC
