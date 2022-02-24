
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
 * @file    lcx_aes.h
 * @brief   AES (Advanced Encryption Standard).
 *
 * AES is an encryption standard based on Rijndael algorithm, a symmetric block
 * cipher that can process data blocks of 128 bits. The key length is either 
 * 128, 192 or 256 bits.
 * 
 * Refer to <a href = " https://csrc.nist.gov/publications/detail/fips/197/final"> FIPS 197
 * </a> for more details.
 */

#ifdef HAVE_AES

#ifndef LCX_AES_H
#define LCX_AES_H

#include "cx_errors.h"
#include "lcx_wrappers.h"
#include "lcx_common.h"
#include "ox_aes.h"

/**
 * @brief   Initialize an AES Key.
 * 
 * @details Once initialized, the key can be stored in non-volatile memory
 *          and directly used for any AES processing.
 *
 * @param[in]  rawkey  Pointer to the supplied key.
 * 
 * @param[in]  key_len Length of the key: 16, 24 or 32 octets.
 * 
 * @param[out] key     Pointer to the key.
 * 
 * @return             Error code:
 *                     - CX_OK on success
 *                     - CX_INVALID_PARAMETER
 */
cx_err_t cx_aes_init_key_no_throw(const uint8_t *rawkey, size_t key_len, cx_aes_key_t *key);

/**
 * @brief   Initialize an AES Key.
 * 
 * @details Once initialized, the key can be stored in non-volatile memory
 *          and directly used for any AES processing.
 *          This function throws an exception if the initialization fails.
 *
 * @param[in]  rawkey  Pointer to the supplied key.
 * 
 * @param[in]  key_len Length of the key: 16, 24 or 32 octets.
 * 
 * @param[out] key     Pointer to the key.
 * 
 * @return             Length of the key.
 * 
 * @throw              CX_INVALID_PARAMETER
 */
static inline int cx_aes_init_key ( const unsigned char * rawkey, unsigned int key_len, cx_aes_key_t * key )
{
  CX_THROW(cx_aes_init_key_no_throw(rawkey, key_len, key));
  return key_len;
}

/**
 * @brief   Encrypt, Decrypt, Sign or Verify data with AES algorithm.
 *
 * @param[in] key     Pointer to the key initialized with #cx_aes_init_key_no_throw.
 * 
 * @param[in] mode    Crypto mode flags
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
 * @param[in] iv      Initialization vector.
 * 
 * @param[in] iv_len  Length of the initialization vector.
 * 
 * @param[in] in      Input data.
 * 
 * @param[in] in_len  Length of the input data.
 *                    If CX_LAST is set, padding is automatically done according to the *mode*.
 *                    Otherwise, *in_len* shall be a multiple of AES_BLOCK_SIZE.
 *
 * @param[out] out    Output data according to the mode:
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
cx_err_t cx_aes_iv_no_throw(const cx_aes_key_t *key,
                            uint32_t            mode,
                            const uint8_t *     iv,
                            size_t              iv_len,
                            const uint8_t *     in,
                            size_t              in_len,
                            uint8_t *           out,
                            size_t *            out_len);

/**
 * @brief   Encrypt, Decrypt, Sign or Verify data with AES algorithm.
 * 
 * @details This functions throws an exception if the computation
 *          doesn't succeed.
 *
 * @param[in] key     Pointer to the key initialized with #cx_aes_init_key_no_throw.
 * 
 * @param[in] mode    Crypto mode flags
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
 * @param[in] iv      Initialization vector.
 * 
 * @param[in] iv_len  Length of the initialization vector.
 * 
 * @param[in] in      Input data.
 * 
 * @param[in] in_len  Length of the input data.
 *                    If CX_LAST is set, padding is automatically done according to the *mode*.
 *                    Otherwise, *in_len* shall be a multiple of AES_BLOCK_SIZE.
 *
 * @param[out] out    Output data according to the mode:
 *                     - encrypted/decrypted output data
 *                     - generated signature
 *                     - signature to be verified
 *
 * @param[in] out_len Length of the output data.
 *
 * @return            Length of the output.
 * 
 * @throws            CX_INVALID_PARAMETER
 * @throws            INVALID_PARAMETER
 */
static inline int cx_aes_iv ( const cx_aes_key_t * key, int mode, unsigned char * iv, unsigned int iv_len, const unsigned char * in, unsigned int in_len, unsigned char * out, unsigned int out_len )
{
  size_t out_len_ = out_len;
  CX_THROW(cx_aes_iv_no_throw(key, mode, iv, iv_len, in, in_len, out, &out_len_));
  return out_len_;
}

/**
 * @brief   Encrypt, Decrypt, Sign or Verify data with AES algorithm.
 * 
 * @details Same as #cx_aes_iv_no_throw with initial IV assumed to be sixteen zeros.
 *
 * @param[in] key     Pointer to the key initialized with 
 *                    #cx_aes_init_key_no_throw.
 * 
 * @param[in] mode    Crypto mode flags
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
 * @param[in] in      Input data.
 * 
 * @param[in] in_len  Length of the input data.
 *                    If CX_LAST is set, padding is automatically done according to the *mode*.
 *                    Otherwise, *in_len* shall be a multiple of AES_BLOCK_SIZE.
 *
 * @param[out] out    Output data according to the mode:
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
cx_err_t cx_aes_no_throw(const cx_aes_key_t *key, uint32_t mode, const uint8_t *in, size_t in_len, uint8_t *out, size_t *out_len);

/**
 * @brief   Encrypt, Decrypt, Sign or Verify data with AES algorithm.
 * 
 * @details Same as #cx_aes_iv_no_throw with initial IV assumed to be sixteen zeros.
 *          This function throws an exception if the computation
 *          doesn't succeed.
 *
 * @param[in] key     Pointer to the key initialized with 
 *                    #cx_aes_init_key_no_throw.
 * 
 * @param[in] mode    Crypto mode flags
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
 * @param[in] in      Input data.
 * 
 * @param[in] in_len  Length of the input data.
 *                    If CX_LAST is set, padding is automatically done according to the *mode*.
 *                    Otherwise, *in_len* shall be a multiple of AES_BLOCK_SIZE.
 *
 * @param[out] out    Output data according to the mode:
 *                     - encrypted/decrypted output data
 *                     - generated signature
 *                     - signature to be verified
 *
 * @param[in] out_len Length of the output data.
 *
 * @return            Length of the output.
 * 
 * @throws            CX_INVALID_PARAMETER
 * @throws            INVALID_PARAMETER
 */
static inline int cx_aes ( const cx_aes_key_t * key, int mode, const unsigned char * in, unsigned int in_len, unsigned char * out, unsigned int out_len )
{
  size_t out_len_ = out_len;
  CX_THROW(cx_aes_no_throw(key, mode, in, in_len, out, &out_len_));
  return out_len_;
}

/**
 * @brief   Encrypt a 16-byte block using AES algorithm.
 *
 * @param[in]  key      Pointer to the AES key.
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
cx_err_t cx_aes_enc_block(const cx_aes_key_t *key, const uint8_t *inblock, uint8_t *outblock);

/**
 * @brief   Decrypt a 16-byte block using AES algorithm.
 * 
 * @param[in]  key      Pointer to the AES key.
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
cx_err_t cx_aes_dec_block(const cx_aes_key_t *key, const uint8_t *inblock, uint8_t *outblock);

#endif

#endif // HAVE_AES
