
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
 * This file is not intended to be included direcly.
 * Include "lbcxng.h" instead
 */
#ifdef HAVE_DES

#ifndef LCX_DES_H
#define LCX_DES_H

#include "lcx_wrappers.h"
#include "lcx_common.h"
#include "ox_des.h"
#include <stddef.h>
#include <stdint.h>

/**
 * Initialize a DES Key.
 *
 * Once initialized, the key may be stored in non-volatile memory
 * an reused 'as-is' for any DES processing
 *
 * @param [in] rawkey
 *   raw key value
 *
 * @param [in] key_len
 *   key bytes lenght: 8,16 or 24
 *
 * @param [out] key
 *   DES key to init
 *
 * @param key
 *   ready to use key to init
 */
cx_err_t cx_des_init_key_no_throw(const uint8_t *rawkey, size_t key_len, cx_des_key_t *key);

static inline int cx_des_init_key ( const unsigned char * rawkey, unsigned int key_len, cx_des_key_t * key )
{
  CX_THROW(cx_des_init_key_no_throw(rawkey, key_len, key));
  return key_len;
}

/**
 * Encrypt, Decrypt, Sign or Verify data with DES algorithm.
 *
 * @param [in] key
 *   A des key fully inited with 'cx_des_init_key'
 *
 * @param [in] mode
 *   Crypto mode flags. See above.
 *   Supported flags:
 *     - CX_LAST
 *     - CX_ENCRYPT
 *     - CX_DECRYPT
 *     - CX_SIGN
 *     - CX_VERIFY
 *     - CX_PAD_NONE
 *     - CX_PAD_ISO9797M1
 *     - CX_PAD_ISO9797M2
 *     - CX_CHAIN_ECB
 *     - CX_CHAIN_CBC
 *     - CX_CHAIN_CTR
 *
 * @param [in] in
 *   Input data to encrypt/decrypt
 *
 * @param [in] len
 *   Length of input to data.
 *   If CX_LAST is set, padding is automatically done according to  'mode'.
 *   Else  'len' shall be a multiple of DES_BLOCK_SIZE.
 *
 * @param [in] iv
 *   Initial IV for chaining mode
 *
 * @param [out] out
 *   Either:
 *     - encrypted/decrypted ouput data
 *     - produced signature
 *     - signature to check
 *
 * @param [in] out_len
 *     size of output buffer
 *
 * @return
 *   - In case of ENCRYPT, DECRYPT or SIGN mode: output lenght data
 *   - In case of VERIFY mode: 0 if signature is false, DES_BLOCK_SIZE if signature is correct
 *
 * @throws INVALID_PARAMETER
 */

cx_err_t cx_des_iv_no_throw(const cx_des_key_t *key,
                   uint32_t            mode,
                   const uint8_t *     iv,
                   size_t              iv_len,
                   const uint8_t *     in,
                   size_t              in_len,
                   uint8_t *           out,
                   size_t *            out_len);

static inline int cx_des_iv ( const cx_des_key_t * key, int mode, unsigned char * iv, unsigned int iv_len, const unsigned char * in, unsigned int in_len, unsigned char * out, unsigned int out_len )
{
  size_t out_len_ = out_len;
  CX_THROW(cx_des_iv_no_throw(key, mode, iv, iv_len, in, in_len, out, &out_len_));
  return out_len_;
}

/**
 *  Same as cx_des_iv with initial IV assumed to be heigt zeros.
 */
cx_err_t cx_des_no_throw(const cx_des_key_t *key, uint32_t mode, const uint8_t *in, size_t in_len, uint8_t *out, size_t *out_len);

static inline int cx_des ( const cx_des_key_t * key, int mode, const unsigned char * in, unsigned int in_len, unsigned char * out, unsigned int out_len )
{
  size_t out_len_ = out_len;
  CX_THROW(cx_des_no_throw(key, mode, in, in_len, out, &out_len_));
  return out_len_;
}

  void cx_des_enc_block(const cx_des_key_t *key, const uint8_t *inblock, uint8_t *outblock);

  void cx_des_dec_block(const cx_des_key_t *key, const uint8_t *inblock, uint8_t *outblock);

#endif // HAVE_DES

#endif
