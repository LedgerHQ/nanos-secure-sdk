
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

#ifndef LCX_DES_H
#define LCX_DES_H

/** @internal */
#define CX_DES_BLOCK_SIZE 8

/** DES key container.
 *  Such container should be initialize with cx_des_init_key to ensure future
 * API compatibility. Indeed, in next API level, the key store format may
 * changed at all. Only 8 bytes (simple DES) and 16 bytes (triple DES with 2
 * keys) are supported.
 */
struct cx_des_key_s {
  /** key size */
  unsigned char size;
  /** key value */
  unsigned char keys[16];
};
/** Convenience type. See #cx_des_key_s. */
typedef struct cx_des_key_s cx_des_key_t;

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
CXCALL int cx_des_init_key(const unsigned char WIDE *rawkey PLENGTH(key_len),
                           unsigned int key_len,
                           cx_des_key_t *key PLENGTH(sizeof(cx_des_key_t)));

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
 *   - In case of VERIFY mode: 0 if signature is false, DES_BLOCK_SIZE if
 * signature is correct
 *
 * @throws INVALID_PARAMETER
 */

CXCALL CXPORT(CXPORT_ED_DES) int cx_des_iv(
    const cx_des_key_t WIDE *key PLENGTH(sizeof(cx_des_key_t)), int mode,
    unsigned char WIDE *iv PLENGTH(iv_len), unsigned int iv_len,
    const unsigned char WIDE *in PLENGTH(in_len), unsigned int in_len,
    unsigned char *out PLENGTH(out_len), unsigned int out_len);

/**
 *  Same as cx_des_iv with initial IV assumed to be heigt zeros.
 */
CXCALL CXPORT(CXPORT_ED_DES) int cx_des(
    const cx_des_key_t WIDE *key PLENGTH(sizeof(cx_des_key_t)), int mode,
    const unsigned char WIDE *in PLENGTH(in_len), unsigned int in_len,
    unsigned char *out PLENGTH(out_len), unsigned int out_len);

#endif
