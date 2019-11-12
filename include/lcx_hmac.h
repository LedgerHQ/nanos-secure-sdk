
/*******************************************************************************
*   Ledger Nano S - Secure firmware
*   (c) 2019 Ledger
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

#ifndef LCX_HMAC_H
#define LCX_HMAC_H

typedef union {
  cx_hash_t header;
  cx_sha256_t sha256;
  cx_sha512_t sha512;
  cx_ripemd160_t ripemd160;
} cx_hash_for_hmac_ctx;

/**
 * HMAC context.
 */
typedef struct {
  cx_hash_for_hmac_ctx hash_ctx;
  uint8_t key[128];
} cx_hmac_ctx;

/* Aliases for compatibility with the old SDK */
typedef cx_hmac_ctx cx_hmac_t;
typedef cx_hmac_ctx cx_hmac_ripemd160_t;
typedef cx_hmac_ctx cx_hmac_sha256_t;
typedef cx_hmac_ctx cx_hmac_sha512_t;

/**
 * Init a hmac sha512 context.
 *
 * @param  [out] hash        the context to init.
 *    The context shall be in RAM
 *
 * @param  [in] key         hmac key value
 *    Passing a NULL pointeur, will reinit the context with the previously set
 * key. If no key has already been set, passing NULL will lead into an undefined
 * behavior.
 *
 * @param  [in] key_len     hmac key length
 *    The key length shall be less than 64 bytes
 *
 * @return algorithm  identifier
 */
CXCALL int cx_hmac_ripemd160_init(
    cx_hmac_ripemd160_t *hmac PLENGTH(sizeof(cx_hmac_ripemd160_t)),
    const unsigned char WIDE *key PLENGTH(key_len), unsigned int key_len);

/**
 * Init a hmac sha256 context.
 *
 * @param [out] hash        the context to init.
 *    The context shall be in RAM
 *
 * @param [in] key         hmac key value
 *    Passing a NULL pointeur, will reinit the context with the previously set
 * key. If no key has already been set, passing NULL will lead into an undefined
 * behavior.
 *
 * @param [in] key_len     hmac key length
 *    The key length shall be less than 64 bytes
 *
 * @return algorithm  identifier
 */
CXCALL int
cx_hmac_sha256_init(cx_hmac_sha256_t *hmac PLENGTH(sizeof(cx_hmac_sha256_t)),
                    const unsigned char WIDE *key PLENGTH(key_len),
                    unsigned int key_len);

/**
 * Init a hmac sha512 context.
 *
 * @param [out] hash        the context to init.
 *    The context shall be in RAM
 *
 * @param [in] key         hmac key value
 *    Passing a NULL pointeur, will reinit the context with the previously set
 * key. If no key has already been set, passing NULL will lead into an undefined
 * behavior.
 *
 * @param [in] key_len     hmac key length
 *    The key length shall be less than 128 bytes
 *
 * @return algorithm  identifier
 */
CXCALL int
cx_hmac_sha512_init(cx_hmac_sha512_t *hmac PLENGTH(sizeof(cx_hmac_sha512_t)),
                    const unsigned char WIDE *key PLENGTH(key_len),
                    unsigned int key_len);

/**
 * @param [in,out] hmac
 *   Univers Continuation Blob.
 *   The hmac context pointer shall point to  either a cx_ripemd160_t, either a
 * cx_sha256_t  or cx_sha512_t . The hmac context shall be inited with
 * 'cx_xxx_init' The hmac context shall be in RAM The function should be called
 * with a nice cast.
 *
 * @param [in] mode
 *   Crypto mode flags. See Above.
 *   If CX_LAST is set and CX_NO_REINIT is not set, context is automatically
 * re-inited. Supported flags:
 *     - CX_LAST
 *     - CX_NO_REINIT
 *
 * @param [in] in
 *   Input data to add to current hmac
 *
 * @param [in] len
 *   Length of input to data.
 *
 * @param [out] mac
 *   Either:
 *     - NULL (ignored) if CX_LAST is NOT set
 *     - produced hmac  if CX_LAST is set
 *
 * @param [in] mac_len
 *   Either:
 *     - O, if mac is NULL
 *     - mac buffer size, if buffer is too small to store the mac an exception
 * is thrown
 *
 */
CXCALL int cx_hmac(cx_hmac_t *hmac PLENGTH(scc__cx_scc_struct_size_hmac__hmac),
                   int mode, const unsigned char WIDE *in PLENGTH(len),
                   unsigned int len, unsigned char *mac PLENGTH(mac_len),
                   unsigned int mac_len);
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
 *    mac buffer size, if buffer is too small to store the mac an exception is
 * thrown
 */
CXCALL int cx_hmac_sha512(const unsigned char WIDE *key PLENGTH(key_len),
                          unsigned int key_len,
                          const unsigned char WIDE *in PLENGTH(len),
                          unsigned int len, unsigned char *mac PLENGTH(mac_len),
                          unsigned int mac_len);

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
 *    mac buffer size, if buffer is too small to store the mac an exception is
 * thrown
 */
CXCALL int cx_hmac_sha256(const unsigned char WIDE *key PLENGTH(key_len),
                          unsigned int key_len,
                          const unsigned char WIDE *in PLENGTH(len),
                          unsigned int len, unsigned char *mac PLENGTH(mac_len),
                          unsigned int mac_len);

#endif
