
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
 * @file    lcx_aead.h
 * @brief   Authenticated Encryption with Associated Data (AEAD)
 *
 * @section Description
 *
 * Authenticated encryption provides confidentiality for the plaintext
 * and integrity and authenticity checks. Authentication Encryption with
 * Associated Data enables to check the integrity and authenticity of
 * some associated data called "additional authenticated data".
 *
 * @author  Ledger
 * @version 1.0
 **/

#ifndef LCX_AEAD_H
#define LCX_AEAD_H

#if defined(HAVE_AEAD)

#include "cx_errors.h"
#include <stddef.h>
#if defined(HAVE_AES_GCM)
#include "lcx_aes_gcm.h"
#endif
#if defined(HAVE_CHACHA_POLY) && defined(HAVE_CHACHA) && defined(HAVE_POLY1305)
#include "lcx_chacha_poly.h"
#endif

#define GCM_MAX_IV_LENGTH 64
#define MAX_TAG_LENGTH    16

/**
 * @brief Supported AEAD algorithms.
 */
typedef enum {
    CX_AEAD_AES128_GCM,        ///< AES-GCM with a 128-bit key
    CX_AEAD_AES192_GCM,        ///< AES-GCM with a 192-bit key
    CX_AEAD_AES256_GCM,        ///< AES-GCM with a 256-bit key
    CX_AEAD_CHACHA20_POLY1305  ///< CHACHA20-POLY1305 with a 256-bit key
} cx_aead_type_t;

/**
 * @brief AEAD generic functions
 *
 */
typedef struct {
    void (*init)(void *ctx);  ///< Initializes the context of the base algorithm
    cx_err_t (*set_key)(void *ctx, const uint8_t *key, size_t key_len);  ///< Sets the key
    cx_err_t (*start)(void          *ctx,
                      uint32_t       mode,
                      const uint8_t *iv,
                      size_t         iv_len);  ///< Sets the initialization vector
    cx_err_t (*update_aad)(void          *ctx,
                           const uint8_t *aad,
                           size_t aad_len);  ///< Processes the additional authenticated data
    cx_err_t (*update)(void          *ctx,
                       const uint8_t *input,
                       uint8_t       *output,
                       size_t         len);  ///< Processes the input data
    cx_err_t (*finish)(void    *ctx,
                       uint8_t *tag,
                       size_t   tag_len);  ///< Computes the message authenticated code
    cx_err_t (*encrypt_and_tag)(void          *ctx,
                                const uint8_t *input,
                                size_t         len,
                                const uint8_t *iv,
                                size_t         iv_len,
                                const uint8_t *aad,
                                size_t         aad_len,
                                uint8_t       *output,
                                uint8_t       *tag,
                                size_t tag_len);  ///< Encrypts the message and computes the MAC

    cx_err_t (*auth_decrypt)(void          *ctx,
                             const uint8_t *input,
                             size_t         len,
                             const uint8_t *iv,
                             size_t         iv_len,
                             const uint8_t *aad,
                             size_t         aad_len,
                             uint8_t       *output,
                             const uint8_t *tag,
                             size_t tag_len);  ///< Decrypts the message and verifies the MAC
    cx_err_t (*check_tag)(void *ctx, const uint8_t *tag, size_t tag_len);  ///< Checks the MAC
} cx_aead_base_t;

/**
 * @brief AEAD information.
 */
typedef struct {
    cx_aead_type_t        type;        ///< AEAD algorithm
    size_t                key_bitlen;  ///< Bit length of the key
    size_t                block_size;  ///< Block size
    const cx_aead_base_t *func;        ///< Pointer to the generic functions
} cx_aead_info_t;

/**
 * @brief AEAD context.
 */

typedef struct {
    const cx_aead_info_t *info;  ///< Pointer to the AEAD information
    uint32_t              mode;  ///< Encrypt or decrypt
    void *base_ctx;  ///< Pointer to the context of the base algorithm: either AES-GCM or
                     ///< Chacha20-Poly1305
} cx_aead_context_t;

/**
 * @brief AEAD initialization.
 *
 * @details This function must be the first called and
 *          is usually followed by a call to
 *          #cx_aead_setup, #cx_aead_set_iv, and
 *          #cx_aead_set_key, then one or more calls to
 *          #cx_aead_update_ad and #cx_aead_update.
 *          Finally, #cx_aead_write_tag computes the message
 *          authentication code in the case of encryption and
 *          #cx_aead_check_tag verifies a given MAC in the case
 *          of decryption.
 *
 * @param[in] ctx Pointer to the AEAD context.
 *
 * @return        Error code.
 */
WARN_UNUSED_RESULT cx_err_t cx_aead_init(cx_aead_context_t *ctx);

/**
 * @brief   AEAD set up.
 *
 * @details The AEAD context must be initialized. This initializes the specific AEAD algorithm
 * context. Supported AEAD algorithms:
 *            - AES-GCM
 *            - Chacha20-Poly1305
 *          This must be called after #cx_aead_init and before #cx_aead_set_key.
 *
 * @param[in] ctx  Pointer to the AEAD context.
 *
 * @param[in] type AEAD algorithm.
 *
 * @return         Error code
 */
WARN_UNUSED_RESULT cx_err_t cx_aead_setup(cx_aead_context_t *ctx, cx_aead_type_t type);

/**
 * @brief   Sets the cipher key.
 *
 * @details The AEAD context must be set up. This is called after
 *          #cx_aead_setup and should be followed by #cx_aead_set_iv.
 *
 * @param[in] ctx     Pointer to the AEAD context.
 *
 * @param[in] key     Cipher key.
 *
 * @param[in] key_len Length of the key.
 *
 * @param[in] mode    Operation the key will be used for: encryption or decryption.
 *
 * @return            Error code
 */
WARN_UNUSED_RESULT cx_err_t cx_aead_set_key(cx_aead_context_t *ctx,
                                            const uint8_t     *key,
                                            size_t             key_len,
                                            uint32_t           mode);

/**
 * @brief   Sets the initialization vector.
 *
 * @details The AEAD context must be set up.
 *
 * @param[in] ctx    Pointer to the AEAD context.
 *
 * @param[in] iv     Initialization vector.
 *
 * @param[in] iv_len Length of the initialization vector.
 *
 * @return           Error code.
 */
WARN_UNUSED_RESULT cx_err_t cx_aead_set_iv(cx_aead_context_t *ctx,
                                           const uint8_t     *iv,
                                           size_t             iv_len);

/**
 * @brief   Adds associated data to the context.
 *
 * @details The AEAD context must be set up. The data length and the processed data
 *          are kept within the context.
 *
 * @param[in] ctx    Pointer to the AEAD context.
 *
 * @param[in] ad     Associated data to add.
 *
 * @param[in] ad_len Length of the associated data.
 *
 * @return           Error code.
 */
WARN_UNUSED_RESULT cx_err_t cx_aead_update_ad(cx_aead_context_t *ctx,
                                              const uint8_t     *ad,
                                              size_t             ad_len);

/**
 * @brief   Updates the data to encrypt or decrypt.
 *
 * @details The AEAD context must be set up. The input length and the processed input
 *          are kept within the context.
 *
 * @param[in]  ctx     Pointer to the AEAD context.
 *
 * @param[in]  in      Input data.
 *
 * @param[in]  in_len  Length of the input.
 *
 * @param[out] out     Buffer the output data. This must be able to hold at least in_len +
 * block_size. This must be not the same buffer as in.
 *
 * @param[out] out_len The length of the output data. This must not be NULL.
 *
 * @return             Error code.
 *
 */
WARN_UNUSED_RESULT cx_err_t
cx_aead_update(cx_aead_context_t *ctx, uint8_t *in, size_t in_len, uint8_t *out, size_t *out_len);

/**
 * @brief   Writes the tag of the AEAD cipher.
 *
 * @details The AEAD context must be set up.
 *
 * @param[in]  ctx     Pointer to the AEAD context.
 *
 * @param[out] tag     Buffer to write the tag to.
 *
 * @param[in]  tag_len Length of the tag.
 *
 * @return             Error code.
 */
WARN_UNUSED_RESULT cx_err_t cx_aead_write_tag(cx_aead_context_t *ctx, uint8_t *tag, size_t tag_len);

/**
 * @brief   Checks the tag of the AEAD cipher.
 *
 * @details The AEAD context must be set up.
 *
 * @param[in] ctx     Pointer to the AEAD context.
 *
 * @param[in] tag     Buffer holding the tag.
 *
 * @param[in] tag_len Length of the tag.
 *
 * @return            Error code.
 */
WARN_UNUSED_RESULT cx_err_t cx_aead_check_tag(cx_aead_context_t *ctx,
                                              const uint8_t     *tag,
                                              size_t             tag_len);

/**
 * @brief   All-in-one authenticated encryption.
 *
 * @details Performs an authenticated encryption according to the specified algorithm.
 *          The AEAD context must be set up.
 *
 * @param[in]  ctx     Pointer to the AEAD context.
 *
 * @param[in]  iv      Initialization vector.
 *
 * @param[in]  iv_len  Length of the initialization vector.
 *
 * @param[in]  ad      Associated data to authenticate.
 *
 * @param[in]  ad_len  Length of the associated data.
 *
 * @param[in]  in      Input data.
 *
 * @param[in]  in_len  Length of the input data.
 *
 * @param[out] out     Buffer for the output data.
 *
 * @param[out] out_len Length of the output data. This must not be NULL.
 *
 * @param[out] tag     Buffer for the authentication tag.
 *
 * @param[in]  tag_len Length of the tag.
 *
 * @return             Error code.
 */
WARN_UNUSED_RESULT cx_err_t cx_aead_encrypt(cx_aead_context_t *ctx,
                                            const uint8_t     *iv,
                                            size_t             iv_len,
                                            const uint8_t     *ad,
                                            size_t             ad_len,
                                            uint8_t           *in,
                                            size_t             in_len,
                                            uint8_t           *out,
                                            size_t            *out_len,
                                            uint8_t           *tag,
                                            size_t             tag_len);

/**
 * @brief   All-in-one authenticated decryption.
 *
 * @details Performs an authenticated decryption according to the specified algorithm.
 *          The AEAD context must be set up. If the data is not authentic then the
 *          output buffer is zeroed.
 *
 * @param[in]  ctx     Pointer to the AEAD context.
 *
 * @param[in]  iv      Initialization vector.
 *
 * @param[in]  iv_len  Length of the initialization vector.
 *
 * @param[in]  ad      Associated data to authenticate.
 *
 * @param[in]  ad_len  Length of the associated data.
 *
 * @param[in]  in      Input data.
 *
 * @param[in]  in_len  Length of the input data.
 *
 * @param[out] out     Buffer for the output data.
 *
 * @param[out] out_len Length of the output data. This must not be NULL.
 *
 * @param[out] tag     Authentication tag to verify.
 *
 * @param[in]  tag_len Length of the tag.
 *
 * @return             Error code.
 */
WARN_UNUSED_RESULT cx_err_t cx_aead_decrypt(cx_aead_context_t *ctx,
                                            const uint8_t     *iv,
                                            size_t             iv_len,
                                            const uint8_t     *ad,
                                            size_t             ad_len,
                                            uint8_t           *in,
                                            size_t             in_len,
                                            uint8_t           *out,
                                            size_t            *out_len,
                                            const uint8_t     *tag,
                                            size_t             tag_len);

#endif  // HAVE_AEAD

#endif  // LCX_AED_H
