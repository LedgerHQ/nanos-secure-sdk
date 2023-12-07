
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
 * @file    lcx_chacha_poly.h
 * @brief   CHACHA20_POLY1305 Authenticated Encryption with Additional Data (AEAD)
 *
 * @section Description
 *
 * @author  Ledger
 * @version 1.0
 **/

#ifndef LCX_CHACHA_POLY_H
#define LCX_CHACHA_POLY_H

#if defined(HAVE_CHACHA_POLY)
#if defined(HAVE_POLY1305) && defined(HAVE_CHACHA)

#include "lcx_chacha.h"
#include "lcx_poly1305.h"
#include "ox.h"
#include <stddef.h>

typedef struct {
    cx_chacha_context_t   chacha20_ctx;  ///< The ChaCha20 context.
    cx_poly1305_context_t poly1305_ctx;  ///< The Poly1305 context.
    size_t                aad_len;  ///< The length in bytes of the Additional Authenticated Data.
    size_t                ciphertext_len;  ///< The length in bytes of the ciphertext.
    uint32_t              state;           ///< The current state of the context.
    uint32_t              mode;            ///< Cipher mode (encrypt or decrypt).
} cx_chachapoly_context_t;

void cx_chachapoly_init(cx_chachapoly_context_t *ctx);

WARN_UNUSED_RESULT cx_err_t cx_chachapoly_set_key(cx_chachapoly_context_t *ctx,
                                                  const uint8_t           *key,
                                                  size_t                   key_len);

WARN_UNUSED_RESULT cx_err_t cx_chachapoly_start(cx_chachapoly_context_t *ctx,
                                                uint32_t                 mode,
                                                const uint8_t           *iv,
                                                size_t                   iv_len);

WARN_UNUSED_RESULT cx_err_t cx_chachapoly_update_aad(cx_chachapoly_context_t *ctx,
                                                     const uint8_t           *aad,
                                                     size_t                   aad_len);

WARN_UNUSED_RESULT cx_err_t cx_chachapoly_update(cx_chachapoly_context_t *ctx,
                                                 const uint8_t           *input,
                                                 uint8_t                 *output,
                                                 size_t                   len);

WARN_UNUSED_RESULT cx_err_t cx_chachapoly_finish(cx_chachapoly_context_t *ctx,
                                                 uint8_t                 *tag,
                                                 size_t                   tag_len);

WARN_UNUSED_RESULT cx_err_t cx_chachapoly_encrypt_and_tag(cx_chachapoly_context_t *ctx,
                                                          const uint8_t           *input,
                                                          size_t                   len,
                                                          const uint8_t           *iv,
                                                          size_t                   iv_len,
                                                          const uint8_t           *aad,
                                                          size_t                   aad_len,
                                                          uint8_t                 *output,
                                                          uint8_t                 *tag,
                                                          size_t                   tag_len);

WARN_UNUSED_RESULT cx_err_t cx_chachapoly_decrypt_and_auth(cx_chachapoly_context_t *ctx,
                                                           const uint8_t           *input,
                                                           size_t                   len,
                                                           const uint8_t           *iv,
                                                           size_t                   iv_len,
                                                           const uint8_t           *aad,
                                                           size_t                   aad_len,
                                                           uint8_t                 *output,
                                                           const uint8_t           *tag,
                                                           size_t                   tag_len);

WARN_UNUSED_RESULT cx_err_t cx_chachapoly_check_tag(cx_chachapoly_context_t *ctx,
                                                    const uint8_t           *tag,
                                                    size_t                   tag_len);

#endif  // HAVE_POLY1305 && HAVE_CHACHA
#endif  // HAVE_CHACHA_POLY
#endif  /* LCX_CHACHA_POLY_H */
