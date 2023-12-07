
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
 * @file    lcx_aes_gcm.h
 * @brief   AES in Galois/Counter Mode (AES-GCM)
 *
 * @section Description
 *
 * The Galois/Counter Mode (GCM) is an authenticated encryption algorithm
 * designed to provide both data authenticity (integrity) and confidentiality.
 * Refer to SP 800-38D for more details.
 *
 * @author  Ledger
 * @version 1.0
 **/

#ifndef LCX_AES_GCM_H
#define LCX_AES_GCM_H

#if defined(HAVE_AES) && defined(HAVE_AES_GCM)

#include "ox.h"
#include <stddef.h>

/**
 * @brief AES-GCM context
 */
typedef struct {
    cx_aes_key_t key;            ///< AES key
    size_t       len;            ///< Input length
    size_t       aad_len;        ///< Additional data length
    uint8_t      enc_block[16];  ///< First encrypted block used to compute the tag
    uint8_t      J0[16];         ///< Counter
    uint8_t      processed[16];  ///< Processed data
    uint8_t      hash_key[16];   ///< Ghash key
    uint32_t     mode;           ///< Encrypt or decrypt
    uint8_t      flag;           ///< Indicates either the IV has already been processed or not
} cx_aes_gcm_context_t;

void                        cx_aes_gcm_init(cx_aes_gcm_context_t *ctx);
WARN_UNUSED_RESULT cx_err_t cx_aes_gcm_set_key(cx_aes_gcm_context_t *ctx,
                                               const uint8_t        *raw_key,
                                               size_t                key_len);
WARN_UNUSED_RESULT cx_err_t cx_aes_gcm_start(cx_aes_gcm_context_t *ctx,
                                             uint32_t              mode,
                                             const uint8_t        *iv,
                                             size_t                iv_len);
WARN_UNUSED_RESULT cx_err_t cx_aes_gcm_update_aad(cx_aes_gcm_context_t *ctx,
                                                  const uint8_t        *aad,
                                                  size_t                aad_len);
WARN_UNUSED_RESULT cx_err_t cx_aes_gcm_update(cx_aes_gcm_context_t *ctx,
                                              const uint8_t        *in,
                                              uint8_t              *out,
                                              size_t                len);
WARN_UNUSED_RESULT cx_err_t cx_aes_gcm_finish(cx_aes_gcm_context_t *ctx,
                                              uint8_t              *tag,
                                              size_t                tag_len);
WARN_UNUSED_RESULT cx_err_t cx_aes_gcm_encrypt_and_tag(cx_aes_gcm_context_t *ctx,
                                                       uint8_t              *in,
                                                       size_t                len,
                                                       const uint8_t        *iv,
                                                       size_t                iv_len,
                                                       const uint8_t        *aad,
                                                       size_t                aad_len,
                                                       uint8_t              *out,
                                                       uint8_t              *tag,
                                                       size_t                tag_len);
WARN_UNUSED_RESULT cx_err_t cx_aes_gcm_decrypt_and_auth(cx_aes_gcm_context_t *ctx,
                                                        uint8_t              *in,
                                                        size_t                len,
                                                        const uint8_t        *iv,
                                                        size_t                iv_len,
                                                        const uint8_t        *aad,
                                                        size_t                aad_len,
                                                        uint8_t              *out,
                                                        const uint8_t        *tag,
                                                        size_t                tag_len);
WARN_UNUSED_RESULT cx_err_t cx_aes_gcm_check_tag(cx_aes_gcm_context_t *ctx,
                                                 const uint8_t        *tag,
                                                 size_t                tag_len);

#endif  // HAVE_AES && HAVE_AES_GCM

#endif  // LCX_AES_GCM_H
