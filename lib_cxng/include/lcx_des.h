
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

/**
 * @file    lcx_des.h
 * @brief   DES (Data Encryption Standard).
 *
 * DES is an encryption algorithm designed to encipher and decipher blocks of
 * 64 bits under control of a 56-bit key. However, the key is represented with 64 bits.
 *
 * Triple DES variant supports either a 128-bit (two 64-bit keys) or 192-bit key
 * (three 64-bit keys).
 */

#ifdef HAVE_DES

#ifndef LCX_DES_H
#define LCX_DES_H

#include "lcx_wrappers.h"
#include "lcx_common.h"
#include "ox_des.h"
#include <stddef.h>
#include <stdint.h>

/** Simple DES key length in bits */
#define CX_DES_KEY_LENGTH      64

/** Key length in bits of a triple DES with 2 keys */
#define CX_3DES_2_KEY_LENGTH  128

/** Key length in bits of a triple DES with 3 keys */
#define CX_3DES_3_KEY_LENGTH  192

/**
 * @brief   Initializes a DES key.
 *
 * @details Once initialized, the key can be stored in non-volatile memory
 *          and directly used for any DES processing.
 *
 * @param[in]  rawkey  Pointer to the supplied key.
 *
 * @param[in]  key_len Length of the key: 8, 16 or 24 octets.
 *
 * @param[out] key     Pointer to the key structure. This must not be NULL.
 *
 * @return             Error code:
 *                     - CX_OK on success
 *                     - CX_INVALID_PARAMETER
 */
cx_err_t cx_des_init_key_no_throw(const uint8_t *rawkey, size_t key_len, cx_des_key_t *key);

/**
 * @deprecated
 * See #cx_des_init_key_no_throw
 */
DEPRECATED static inline size_t cx_des_init_key ( const unsigned char * rawkey, unsigned int key_len, cx_des_key_t * key )
{
  CX_THROW(cx_des_init_key_no_throw(rawkey, key_len, key));
  return key_len;
}

/**
 * @brief   Encrypts, decrypts, signs or verifies data with DES algorithm.
 *
 * @param[in] key    Pointer to the key initialized with
 *                   #cx_des_init_key_no_throw.
 *
 * @param[in] iv     Initialization vector.
 *
 * @param[in] iv_len Length of the initialization vector.
 *
 * @param[in] mode   Crypto mode flags.
 *                    Supported flags:
 *                     - CX_LAST
 *                     - CX_ENCRYPT
 *                     - CX_DECRYPT
 *                     - CX_SIGN
 *                     - CX_VERIFY
 *                     - CX_PAD_NONE
 *                     - CX_PAD_ISO9797M1
 *                     - CX_PAD_ISO9797M2
 *                     - CX_CHAIN_ECB
 *                     - CX_CHAIN_CBC
 *                     - CX_CHAIN_CTR
 *
 * @param[in] in     Input data.
 *
 * @param[in] in_len Length of the input data.
 *                    If CX_LAST is set, padding is automatically done according to the *mode*.
 *                    Otherwise, *in_len* shall be a multiple of DES_BLOCK_SIZE.
 *
 * @param[out] out   Output data according to the mode:
 *                     - encrypted/decrypted output data
 *                     - generated signature
 *                     - signature to be verified
 *
 * @param[in] out_len Length of the output data.
 *
 * @return            Error code:
 *                    - CX_OK on success
 *                    - CX_INVALID_PARAMETER
 *                    - INVALID_PARAMETER
 */
cx_err_t cx_des_iv_no_throw(const cx_des_key_t *key,
                   uint32_t            mode,
                   const uint8_t *     iv,
                   size_t              iv_len,
                   const uint8_t *     in,
                   size_t              in_len,
                   uint8_t *           out,
                   size_t *            out_len);

/**
 * @deprecated
 * See #cx_des_iv_no_throw
 */
DEPRECATED static inline size_t cx_des_iv ( const cx_des_key_t * key, uint32_t mode, unsigned char * iv, unsigned int iv_len, const unsigned char * in, unsigned int in_len, unsigned char * out, unsigned int out_len )
{
  size_t out_len_ = out_len;
  CX_THROW(cx_des_iv_no_throw(key, mode, iv, iv_len, in, in_len, out, &out_len_));
  return out_len_;
}

/**
 * @brief   Encrypts, decrypts, signs or verifies data with DES algorithm.
 *
 * @param[in] key    Pointer to the key initialized with
 *                   #cx_des_init_key_no_throw.
 *
 * @param[in] mode   Crypto mode flags.
 *                    Supported flags:
 *                     - CX_LAST
 *                     - CX_ENCRYPT
 *                     - CX_DECRYPT
 *                     - CX_SIGN
 *                     - CX_VERIFY
 *                     - CX_PAD_NONE
 *                     - CX_PAD_ISO9797M1
 *                     - CX_PAD_ISO9797M2
 *                     - CX_CHAIN_ECB
 *                     - CX_CHAIN_CBC
 *                     - CX_CHAIN_CTR
 *
 * @param[in] in     Input data.
 *
 * @param[in] in_len Length of the input data.
 *                    If CX_LAST is set, padding is automatically done according to the *mode*.
 *                    Otherwise, *in_len* shall be a multiple of DES_BLOCK_SIZE.
 *
 * @param[out] out   Output data according to the mode:
 *                     - encrypted/decrypted output data
 *                     - generated signature
 *                     - signature to be verified
 *
 * @param[in] out_len Length of the output data.
 *
 * @return            Error code:
 *                    - CX_OK on success
 *                    - CX_INVALID_PARAMETER
 *                    - INVALID_PARAMETER
 */
cx_err_t cx_des_no_throw(const cx_des_key_t *key, uint32_t mode, const uint8_t *in, size_t in_len, uint8_t *out, size_t *out_len);

/**
 * @deprecated
 * See #cx_des_no_throw
 */
DEPRECATED static inline size_t cx_des ( const cx_des_key_t * key, uint32_t mode, const unsigned char * in, unsigned int in_len, unsigned char * out, unsigned int out_len )
{
  size_t out_len_ = out_len;
  CX_THROW(cx_des_no_throw(key, mode, in, in_len, out, &out_len_));
  return out_len_;
}

/**
 * @brief   Encrypts a 8-byte block using DES/3-DES algorithm.
 *
 * @param[in]  key      Pointer to the DES key.
 *
 * @param[in]  inblock  Plaintext block to encrypt.
 *
 * @param[out] outblock Ciphertext block.
 *
 * @return              Error code:
 *                      - CX_OK
 *                      - CX_INVALID_PARAMETER
 *                      - INVALID_PARAMETER
 */
  cx_err_t cx_des_enc_block(const cx_des_key_t *key, const uint8_t *inblock, uint8_t *outblock);

/**
 * @brief   Decrypts a 8-byte block using DES/3-DES algorithm.
 *
 * @param[in]  key      Pointer to the DES key.
 *
 * @param[in]  inblock  Ciphertext block to decrypt.
 *
 * @param[out] outblock Plaintext block.
 *
 * @return              Error code:
 *                      - CX_OK
 *                      - CX_INVALID_PARAMETER
 *                      - INVALID_PARAMETER
 */
  cx_err_t cx_des_dec_block(const cx_des_key_t *key, const uint8_t *inblock, uint8_t *outblock);

#endif // HAVE_DES

#endif
