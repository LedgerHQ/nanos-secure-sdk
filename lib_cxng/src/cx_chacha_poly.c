
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
#if defined(HAVE_CHACHA_POLY)
#if defined(HAVE_CHACHA) && defined(HAVE_POLY1305)
#include "cx_chacha_poly.h"
#include "cx_utils.h"
#include "lcx_common.h"
#include "os_utils.h"
#include <stddef.h>
#include <string.h>

#define CHACHAPOLY_STATE_INIT       0
#define CHACHAPOLY_STATE_AAD        1
#define CHACHAPOLY_STATE_CIPHERTEXT 2 /* Encrypting or decrypting */
#define CHACHAPOLY_STATE_FINISHED   3
#define CHACHAPOLY_N_ROUNDS         20
#define CHACHAPOLY_TAG_LEN          16

#if defined(HAVE_AEAD)
static const cx_aead_base_t cx_chachapoly_functions = {
    (void (*)(void *ctx)) cx_chachapoly_init,
    (cx_err_t(*)(void *ctx, const uint8_t *key, size_t key_len)) cx_chachapoly_set_key,
    (cx_err_t(*)(void *ctx, uint32_t mode, const uint8_t *iv, size_t iv_len)) cx_chachapoly_start,
    (cx_err_t(*)(void *ctx, const uint8_t *aad, size_t aad_len)) cx_chachapoly_update_aad,
    (cx_err_t(*)(void *ctx, const uint8_t *input, uint8_t *output, size_t len))
        cx_chachapoly_update,
    (cx_err_t(*)(void *ctx, uint8_t *tag, size_t tag_len)) cx_chachapoly_finish,
    (cx_err_t(*)(void          *ctx,
                 const uint8_t *input,
                 size_t         len,
                 const uint8_t *iv,
                 size_t         iv_len,
                 const uint8_t *aad,
                 size_t         aad_len,
                 uint8_t       *output,
                 uint8_t       *tag,
                 size_t         tag_len)) cx_chachapoly_encrypt_and_tag,
    (cx_err_t(*)(void          *ctx,
                 const uint8_t *input,
                 size_t         len,
                 const uint8_t *iv,
                 size_t         iv_len,
                 const uint8_t *aad,
                 size_t         aad_len,
                 uint8_t       *output,
                 const uint8_t *tag,
                 size_t         tag_len)) cx_chachapoly_decrypt_and_auth,
    (cx_err_t(*)(void *ctx, const uint8_t *tag, size_t tag_len)) cx_chachapoly_check_tag};

const cx_aead_info_t cx_chacha20_poly1305_info
    = {CX_AEAD_CHACHA20_POLY1305, 256, 512, &cx_chachapoly_functions};
#endif  // HAVE_AEAD

void cx_chachapoly_init(cx_chachapoly_context_t *ctx)
{
    cx_chacha_init(&ctx->chacha20_ctx, CHACHAPOLY_N_ROUNDS);
    cx_poly1305_init(&ctx->poly1305_ctx);
    ctx->aad_len        = 0;
    ctx->ciphertext_len = 0;
    ctx->state          = CHACHAPOLY_STATE_INIT;
    ctx->mode           = CX_ENCRYPT;
}

cx_err_t cx_chachapoly_set_key(cx_chachapoly_context_t *ctx, const uint8_t *key, size_t key_len)
{
    return cx_chacha_set_key(&ctx->chacha20_ctx, key, key_len);
}

cx_err_t cx_chachapoly_start(cx_chachapoly_context_t *ctx,
                             uint32_t                 mode,
                             const uint8_t           *iv,
                             size_t                   iv_len)
{
    cx_err_t error;
    uint8_t  poly1305_key[64];

    /* iv = 0 || nonce */
    CX_CHECK(cx_chacha_start(&ctx->chacha20_ctx, iv, iv_len));

    /* Generate the Poly1305 key by getting the ChaCha20 keystream output with
     * counter = 0.  This is the same as encrypting a buffer of zeroes.
     * Only the first 256-bits (32 bytes) of the key is used for Poly1305.
     * The other 256 bits are discarded.
     */
    memset(poly1305_key, 0, sizeof(poly1305_key));
    CX_CHECK(
        cx_chacha_update(&ctx->chacha20_ctx, poly1305_key, poly1305_key, sizeof(poly1305_key)));

    cx_poly1305_set_key(&ctx->poly1305_ctx, poly1305_key);
    ctx->aad_len        = 0;
    ctx->ciphertext_len = 0;
    ctx->state          = CHACHAPOLY_STATE_AAD;
    ctx->mode           = mode;

end:
    memset(poly1305_key, 0, 64);
    return error;
}

cx_err_t cx_chachapoly_update_aad(cx_chachapoly_context_t *ctx, const uint8_t *aad, size_t aad_len)
{
    if (ctx->state != CHACHAPOLY_STATE_AAD) {
        return CX_INTERNAL_ERROR;
    }

    ctx->aad_len += aad_len;
    return cx_poly1305_update(&ctx->poly1305_ctx, aad, aad_len);
}

static cx_err_t cx_chachapoly_pad(cx_chachapoly_context_t *ctx, size_t in_to_pad_len)
{
    uint32_t      pad_len = in_to_pad_len % 16;
    unsigned char pad_bytes[15];

    if (!pad_len) {
        return CX_OK;
    }

    memset(pad_bytes, 0, sizeof(pad_bytes));
    return cx_poly1305_update(&ctx->poly1305_ctx, pad_bytes, 16 - pad_len);
}

cx_err_t cx_chachapoly_update(cx_chachapoly_context_t *ctx,
                              const uint8_t           *input,
                              uint8_t                 *output,
                              size_t                   len)
{
    cx_err_t error;

    if ((ctx->state != CHACHAPOLY_STATE_AAD) && (ctx->state != CHACHAPOLY_STATE_CIPHERTEXT)) {
        return CX_INTERNAL_ERROR;
    }
    if (CHACHAPOLY_STATE_AAD == ctx->state) {
        ctx->state = CHACHAPOLY_STATE_CIPHERTEXT;
        CX_CHECK(cx_chachapoly_pad(ctx, ctx->aad_len));
    }
    ctx->ciphertext_len += len;
    if (CX_ENCRYPT == ctx->mode) {
        CX_CHECK(cx_chacha_update(&ctx->chacha20_ctx, input, output, len));
        CX_CHECK(cx_poly1305_update(&ctx->poly1305_ctx, output, len));
    }
    else { /* DECRYPT */
        CX_CHECK(cx_poly1305_update(&ctx->poly1305_ctx, input, len));
        CX_CHECK(cx_chacha_update(&ctx->chacha20_ctx, input, output, len));
    }
end:
    return error;
}

cx_err_t cx_chachapoly_finish(cx_chachapoly_context_t *ctx, uint8_t *tag, size_t tag_len)
{
    cx_err_t error;
    uint8_t  len_block[16];

    if (tag_len != CHACHAPOLY_TAG_LEN) {
        return CX_INVALID_PARAMETER_VALUE;
    }

    if (CHACHAPOLY_STATE_INIT == ctx->state) {
        return CX_INTERNAL_ERROR;
    }
    if (CHACHAPOLY_STATE_AAD == ctx->state) {
        CX_CHECK(cx_chachapoly_pad(ctx, ctx->aad_len));
    }
    else if (CHACHAPOLY_STATE_CIPHERTEXT == ctx->state) {
        CX_CHECK(cx_chachapoly_pad(ctx, ctx->ciphertext_len));
    }
    ctx->state = CHACHAPOLY_STATE_FINISHED;
    /* The lengths of the AAD and ciphertext are processed by
     * Poly1305 as the final 128-bit block, encoded as little-endian integers.
     */
    memset(len_block, 0, 16);
    U4LE_ENCODE(len_block, 0, ctx->aad_len);
    U4LE_ENCODE(len_block, 8, ctx->ciphertext_len);
    CX_CHECK(cx_poly1305_update(&ctx->poly1305_ctx, len_block, 16));
    CX_CHECK(cx_poly1305_finish(&ctx->poly1305_ctx, tag));

end:
    return error;
}

static cx_err_t cx_chachapoly_process_and_tag(cx_chachapoly_context_t *ctx,
                                              uint32_t                 mode,
                                              const uint8_t           *input,
                                              size_t                   len,
                                              const uint8_t           *iv,
                                              size_t                   iv_len,
                                              const uint8_t           *aad,
                                              size_t                   aad_len,
                                              uint8_t                 *output,
                                              uint8_t                 *tag,
                                              size_t                   tag_len)
{
    cx_err_t error;
    CX_CHECK(cx_chachapoly_start(ctx, mode, iv, iv_len));
    CX_CHECK(cx_chachapoly_update_aad(ctx, aad, aad_len));
    CX_CHECK(cx_chachapoly_update(ctx, input, output, len));
    CX_CHECK(cx_chachapoly_finish(ctx, tag, tag_len));
end:
    return error;
}

cx_err_t cx_chachapoly_encrypt_and_tag(cx_chachapoly_context_t *ctx,
                                       const uint8_t           *input,
                                       size_t                   len,
                                       const uint8_t           *iv,
                                       size_t                   iv_len,
                                       const uint8_t           *aad,
                                       size_t                   aad_len,
                                       uint8_t                 *output,
                                       uint8_t                 *tag,
                                       size_t                   tag_len)
{
    return cx_chachapoly_process_and_tag(
        ctx, CX_ENCRYPT, input, len, iv, iv_len, aad, aad_len, output, tag, tag_len);
}

cx_err_t cx_chachapoly_decrypt_and_auth(cx_chachapoly_context_t *ctx,
                                        const uint8_t           *input,
                                        size_t                   len,
                                        const uint8_t           *iv,
                                        size_t                   iv_len,
                                        const uint8_t           *aad,
                                        size_t                   aad_len,
                                        uint8_t                 *output,
                                        const uint8_t           *tag,
                                        size_t                   tag_len)
{
    uint8_t  check_tag[CHACHAPOLY_TAG_LEN];
    uint8_t  diff;
    cx_err_t error;

    memset(check_tag, 0, CHACHAPOLY_TAG_LEN);
    CX_CHECK(cx_chachapoly_process_and_tag(
        ctx, CX_DECRYPT, input, len, iv, iv_len, aad, aad_len, output, check_tag, tag_len));

    diff  = cx_constant_time_eq(tag, check_tag, CHACHAPOLY_TAG_LEN);
    error = ((diff == 0) ? CX_OK : CX_INVALID_PARAMETER_VALUE);
    if (error) {
        memset(check_tag, 0, CHACHAPOLY_TAG_LEN);
    }

end:
    return error;
}

cx_err_t cx_chachapoly_check_tag(cx_chachapoly_context_t *ctx, const uint8_t *tag, size_t tag_len)
{
    cx_err_t error;
    uint8_t  diff;
    uint8_t  check_tag[CHACHAPOLY_TAG_LEN];

    CX_CHECK(cx_chachapoly_finish(ctx, check_tag, tag_len));

    diff  = cx_constant_time_eq(tag, check_tag, tag_len);
    error = diff * CX_INVALID_PARAMETER_VALUE + (1 - diff) * CX_OK;
end:
    return error;
}

#endif  // HAVE_CHACHA && HAVE_POLY1305
#endif  // HAVE_CHACHA_POLY
