
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
#if defined(HAVE_CHACHA)

/**
 * @file    lcx_chacha.h
 * @brief   Chacha cipher
 *
 * @section Description
 *
 * The Chacha cipher is a stream cipher which generates a key stream of at most 2^70 bytes
 * composed of blocks of size 512 bits. The possible key sizes are 128 bits and 256 bits and
 * the possible numbers of rounds are 8,12 and, 20. 
 *
 * @author  Ledger
 * @version 1.0
 **/

#ifndef LCX_CHACHA_H
#define LCX_CHACHA_H

#include "ox.h"
#include <stddef.h>

/**
 * @brief Chacha context
 */
typedef struct {
  uint32_t nrounds;    ///< Number of rounds: 8, 12 or 20
  uint32_t state[16];  ///< Initial state array
  uint32_t block[16];  ///< State array after block processing
  uint32_t pos;        ///< Block count
} cx_chacha_context_t;

/**
 * @brief         Initialize the specified Chacha context.
 *
 * @details       It must be the first API called before using the context.
 *                It is usually followed by a call to
 *                #cx_chacha_set_key and
 *                #cx_chacha_start, then one or more calls to
 *                #cx_chacha_update
 * 
 * @param ctx     The Chacha context to initialize. This must not be \c NULL.
 *
 * @param nrounds The number of rounds: 8, 12 or 20.
 *
 */
void cx_chacha_init(cx_chacha_context_t *ctx, uint32_t nrounds);

/**
 * @brief         Set the encryption or decryption key.
 * 
 * @details       This function must be followed by #cx_chacha_start to set the nonce
 *                and the counter (the initialization vector) and by #cx_chacha_update
 *                to encrypt or decrypt.
 * 
 * @param ctx     The Chacha context to which the key should be bound. It must be initialized.
 *
 * @param key     Pointer to the key. This must not be \c NULL.
 *
 * @param key_len Length of the key. This must be \c 32 bytes.
 *                The \c 16 bytes key is not supported as it is not recommended.
 *
 * @return        Error code
 */
cx_err_t cx_chacha_set_key(cx_chacha_context_t *ctx, const uint8_t *key, size_t key_len);

/**
 * @brief        Set the nonce and initial counter value.
 * 
 * @details      The counter value *counter* and the nonce *nonce* are
 *               concatenated to form the initialization vector *iv*:
 *               **iv = counter || nonce**.
 *               The nonce is either 8 or 12 bytes. If the nonce is 8 bytes
 *               then the counter is represented with 8 bytes. If the nonce is
 *               12 bytes then the counter is represented with 4 bytes.
 * 
 * @param ctx    The Chacha context to which the nonce should be bound.
 *               It must be initialized and bound to a key.
 *
 * @param iv     Pointer to the initialization vector composed of the counter and the nonce.
 *               This must not be \c NULL.
 *
 * @param iv_len Length of the initialization vector. This must be \c 16 bytes.
 *
 * @return       Error code
 */
cx_err_t cx_chacha_start(cx_chacha_context_t *ctx, const uint8_t *iv, size_t iv_len);

/**
 * @brief        Update the stream: encrypt or decrypt data.
 * 
 * @details      This function must be called after #cx_chacha_set_key and #cx_chacha_start.
 *               It can be called multiple times for long input data. The result is then the
 *               concatenation of all the ouputs of this function.
 * 
 * @param ctx    The Chacha context to use for encryption or decryption. It must be initialized
 *               and bound to a key and a nonce.
 *
 * @param input  Pointer to the input data. This must not be \c NULL.
 *
 * @param output Pointer to the buffer holding the output data. The size of the buffer
 *               must be at least *len* bytes. This must not be \c NULL.
 *
 * @param len    Length of the input data in bytes.
 *
 * @return       Error code
 */
cx_err_t cx_chacha_update(cx_chacha_context_t *ctx, const uint8_t *input, uint8_t *output, size_t len);

/**
 * @brief Encrypt or decrypt data with Chacha and a given key and nonce.
 * 
 * @param nrounds Number of rounds: 8, 12 or 20.
 *
 * @param key     Pointer to the key. This must not be \c NULL.
 *
 * @param key_len Length of the key. This must be 32 bytes.
 *
 * @param iv      Pointer to the initialization vector composed of the
 *                nonce and the counter.
 *
 * @param iv_len  Length of the initialization vector. This must be \c 16 bytes.
 *
 * @param input   Pointer to the input data. This must not be \c NULL.
 *
 * @param output  Pointer to the buffer holding the output data. The size of the buffer
 *                must be at least *len* bytes. This must not be \c NULL.
 *
 * @param len     Length of the input data in bytes.
 *
 * @return        Error code.
 */
cx_err_t cx_chacha_cipher(uint32_t nrounds,
                          const uint8_t *key, size_t key_len,
                          const uint8_t *iv, size_t iv_len,
                          const uint8_t *input,
                          uint8_t *output, size_t len);

#endif /* LCX_CHACHA_H */
#endif // HAVE_CHACHA
