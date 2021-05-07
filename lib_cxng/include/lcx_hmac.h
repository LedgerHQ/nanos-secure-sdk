
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

/*
 * This file is not intended to be included directly.
 * Include "lbcxng.h" instead
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
 * HMAC context, abstract type
 */
typedef struct {
  uint8_t   key[128];
  cx_hash_t hash_ctx;
} cx_hmac_t;


#ifdef HAVE_RIPEMD160

/**
 * HMAC context, concrete type for ripemd160
 */
typedef struct {
  uint8_t        key[128];
  cx_ripemd160_t hash_ctx;
} cx_hmac_ripemd160_t;


/**
 * Init a hmac sha512 context.
 *
 * @param  [out] hash        the context to init.
 *    The context shall be in RAM
 *
 * @param  [in] key         hmac key value
 *    Passing a NULL pointeur, will reinit the context with the previously set key.
 *    If no key has already been set, passing NULL will lead into an undefined behavior.
 *
 * @param  [in] key_len     hmac key length
 *    The key length shall be less than 64 bytes
 *
 * @return algorithm  identifier
 */
cx_err_t cx_hmac_ripemd160_init_no_throw(cx_hmac_ripemd160_t *hmac, const uint8_t *key, size_t key_len);

static inline int cx_hmac_ripemd160_init ( cx_hmac_ripemd160_t * hmac, const unsigned char * key, unsigned int key_len )
{
  CX_THROW(cx_hmac_ripemd160_init_no_throw(hmac, key, key_len));
  return CX_RIPEMD160;
}
#endif


#if defined(HAVE_SHA224) || defined(HAVE_SHA256)
/**
 * HMAC context, concrete type for sha224/sha256
 */
typedef struct {
  uint8_t     key[128];
  cx_sha256_t hash_ctx;
} cx_hmac_sha256_t;
#endif

#ifdef HAVE_SHA224
/**
 * Init a hmac sha224 context.
 *
 * @param [out] hash        the context to init.
 *    The context shall be in RAM
 *
 * @param [in] key         hmac key value
 *    Passing a NULL pointeur, will reinit the context with the previously set key.
 *    If no key has already been set, passing NULL will lead into an undefined behavior.
 *
 * @param [in] key_len     hmac key length
 *    The key length shall be less than 64 bytes
 *
 * @return algorithm  identifier
 */
cx_err_t cx_hmac_sha224_init(cx_hmac_sha256_t *hmac, const uint8_t *key, unsigned int key_len);
#endif

#ifdef HAVE_SHA256
/**
 * Init a hmac sha256 context.
 *
 * @param [out] hash        the context to init.
 *    The context shall be in RAM
 *
 * @param [in] key         hmac key value
 *    Passing a NULL pointeur, will reinit the context with the previously set key.
 *    If no key has already been set, passing NULL will lead into an undefined behavior.
 *
 * @param [in] key_len     hmac key length
 *    The key length shall be less than 64 bytes
 *
 * @return algorithm  identifier
 */
cx_err_t cx_hmac_sha256_init_no_throw(cx_hmac_sha256_t *hmac, const uint8_t *key, size_t key_len);

static inline int cx_hmac_sha256_init ( cx_hmac_sha256_t * hmac, const unsigned char * key, unsigned int key_len )
{
  CX_THROW(cx_hmac_sha256_init_no_throw(hmac, key, key_len));
  return CX_SHA256;
}

/**
 * One shot hmac sha256 digest
 *
 * @param  [in] key_in
 *   hmac key value
 *
 * @param  [in] key_len
 *   Length of the hmac key
 *
 * @param  [in] in
 *   Input data to compute the hash
 *
 * @param  [in] len
 *   Length of input to data.
 *
 * @param [out] out
 *   Produced hmac
 *
 * @param [in] mac_len
 *    mac buffer size, if buffer is too small to store the mac an exception is thrown
 */
size_t cx_hmac_sha256(const uint8_t *key, size_t key_len, const uint8_t *in, size_t len, uint8_t *mac, size_t mac_len);

#endif


#if defined(HAVE_SHA384) || defined(HAVE_SHA512)
/**
 * HMAC context, concrete type for sha384/sha512
 */
typedef struct {
  uint8_t     key[128];
  cx_sha512_t hash_ctx;
} cx_hmac_sha512_t;
#endif

#ifdef HAVE_SHA384
/**
 * Init a hmac sha384 context.
 *
 * @param [out] hash        the context to init.
 *    The context shall be in RAM
 *
 * @param [in] key         hmac key value
 *    Passing a NULL pointeur, will reinit the context with the previously set key.
 *    If no key has already been set, passing NULL will lead into an undefined behavior.
 *
 * @param [in] key_len     hmac key length
 *    The key length shall be less than 128 bytes
 *
 * @return algorithm  identifier
 */
cx_err_t cx_hmac_sha384_init(cx_hmac_sha512_t *hmac, const uint8_t *key, unsigned int key_len);
#endif

#ifdef HAVE_SHA512
/**
 * Init a hmac sha512 context.
 *
 * @param [out] hash        the context to init.
 *    The context shall be in RAM
 *
 * @param [in] key         hmac key value
 *    Passing a NULL pointeur, will reinit the context with the previously set key.
 *    If no key has already been set, passing NULL will lead into an undefined behavior.
 *
 * @param [in] key_len     hmac key length
 *    The key length shall be less than 128 bytes
 *
 * @return algorithm  identifier
 */
cx_err_t cx_hmac_sha512_init_no_throw(cx_hmac_sha512_t *hmac, const uint8_t *key, size_t key_len);

static inline int cx_hmac_sha512_init ( cx_hmac_sha512_t * hmac, const unsigned char * key, unsigned int key_len )
{
  CX_THROW(cx_hmac_sha512_init_no_throw(hmac, key, key_len));
  return CX_SHA512;
}

/**
 * One shot hmac sha512 digest
 *
 * @param  [in] key_in
 *   hmac key value
 *
 * @param  [in] key_len
 *   Length of the hmac key
 *
 * @param  [in] in
 *   Input data to compute the hash
 *
 * @param  [in] len
 *   Length of input to data.
 *
 * @param [out] out
 *   Produced hmac
 *
 * @param [in] mac_len
 *    mac buffer size, if buffer is too small to store the mac an exception is thrown
 */
size_t cx_hmac_sha512(const uint8_t *key, size_t key_len, const uint8_t *in, size_t len, uint8_t *mac, size_t mac_len);

#endif

/**
 * @param [in,out] hmac
 *    Univers Continuation Blob.
 *    The hmac context pointer shall point to  either a cx_ripemd160_t, either a cx_sha256_t  or cx_sha512_t .
 *    The hmac context shall be inited with 'cx_xxx_init'
 *    The hmac context shall be in RAM
 *    The function should be called with a nice cast.
 *
 * @param [in] mode
 *    Crypto mode flags. See Above.
 *    If CX_LAST is set and CX_NO_REINIT is not set, context is automatically re-inited.
 *    Supported flags:
 *      - CX_LAST
 *     - CX_NO_REINIT
 *
 * @param [in] in
 *    Input data to add to current hmac
 *
 * @param [in] len
 *    Length of input to data.
 *
 * @param [out] mac
 *    Either:
 *      - NULL (ignored) if CX_LAST is NOT set
 *      - produced hmac  if CX_LAST is set
 *
 * @param [in] mac_len
 *    Either:
 *      - O, if mac is NULL
 *      - mac buffer size, if buffer is too small to store the mac an exception is thrown
 *
 */
cx_err_t cx_hmac_no_throw(cx_hmac_t *hmac, uint32_t mode, const uint8_t *in, size_t len, uint8_t *mac, size_t mac_len);

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
 * Generic Initialization of hashmac context with parameter
 *
 * @param [out] hmac
 *    The context to init.
 *    The context shall be in RAM
 *
 * @param [in] hash_id the hash identifier.
 *
 * @param [in] key
 *    hmac key value
 *    Passing a NULL pointeur, will reinit the context with the previously set key.
 *    If no key has already been set, passing NULL will lead into an undefined behavior.
 *
 * @param [in] key_len     hmac key length
 *    The key length shall be less than 128 bytes
 *
 * @return algorithm  identifier
 */
cx_err_t cx_hmac_init(cx_hmac_t *hmac, cx_md_t hash_id, const uint8_t *key, size_t key_len);

/**
 ** Add more data to hashmac.
 *
 * Equivalent to cx_hmac_no_throw(hmac, 0, in, len,  NULL, 0);
 *
 * @param [in,out] hmac
 *    Univers Continuation Blob.
 *    The hmac context pointer shall point to  either a cx_ripemd160_t, either a cx_sha256_t  or cx_sha512_t .
 *    The hmac context shall be inited with 'cx_xxx_init'
 *    The hmac context shall be in RAM
 *    The function should be called with a nice cast.
 *
 * @param [in] in
 *    Input data to add to current hmac
 *
 * @param [in] len
 *    Length of input to data.
 *
 *
 */
cx_err_t cx_hmac_update(cx_hmac_t *hmac, const uint8_t *in, size_t in_len);

/**
 * Finalize hashmac and get result
 *
 * Equivalent to cx_hmac_no_throw(hash, CX_LAST, NULL, 0, out, out_len);
 *
 * @param [out] out
 *    produced hash  if CX_LAST is set
 *
 * @param [in] out_len
 *    max (most significant) bytes of hashmac to retrieve
 */
cx_err_t cx_hmac_final(cx_hmac_t *ctx, uint8_t *out, size_t *out_len) ;

#endif

#endif // HAVE_HMAC
