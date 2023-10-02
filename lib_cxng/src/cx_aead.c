
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

#if defined(HAVE_AEAD)
#include "lcx_aead.h"
#include "lcx_common.h"
#include "cx_utils.h"
#include <string.h>

#if defined(HAVE_AES_GCM)
#include "cx_aes_gcm.h"
#endif  // HAVE_AES
#if defined(HAVE_CHACHA_POLY) && defined(HAVE_CHACHA) && defined(HAVE_POLY1305)
#include "cx_chacha_poly.h"
#endif

const cx_aead_info_t *cx_aead_get_info(cx_aead_type_t type)
{
    switch (type) {
#if defined(HAVE_AES_GCM)
        case CX_AEAD_AES128_GCM:
            return &cx_aes128_gcm_info;
        case CX_AEAD_AES192_GCM:
            return &cx_aes192_gcm_info;
        case CX_AEAD_AES256_GCM:
            return &cx_aes256_gcm_info;
#endif  // HAVE_AES_GCM
#if defined(HAVE_CHACHA_POLY) && defined(HAVE_CHACHA) && defined(HAVE_POLY1305)
        case CX_AEAD_CHACHA20_POLY1305:
            return &cx_chacha20_poly1305_info;
#endif
        default:
            return NULL;
    }
}

cx_err_t cx_aead_init(cx_aead_context_t *ctx)
{
    if (NULL == ctx) {
        return CX_INVALID_PARAMETER;
    }
    memset(ctx, 0, sizeof(cx_aead_context_t));
    return CX_OK;
}

cx_err_t cx_aead_setup(cx_aead_context_t *ctx, cx_aead_type_t type)
{
    const cx_aead_info_t *info = cx_aead_get_info(type);
    if (NULL == info) {
        return CX_INVALID_PARAMETER;
    }
    ctx->info = info;
    ctx->info->func->init(ctx->base_ctx);
    return CX_OK;
}

cx_err_t cx_aead_set_key(cx_aead_context_t *ctx, const uint8_t *key, size_t key_len, uint32_t mode)
{
    if (ctx->info->key_bitlen != key_len * 8) {
        return CX_INVALID_PARAMETER_SIZE;
    }
    if ((mode != CX_ENCRYPT) && (mode != CX_DECRYPT)) {
        return CX_INVALID_PARAMETER_VALUE;
    }
    ctx->mode = mode;
    return ctx->info->func->set_key(ctx->base_ctx, key, key_len);
}

cx_err_t cx_aead_set_iv(cx_aead_context_t *ctx, const uint8_t *iv, size_t iv_len)
{
    if ((NULL == iv) || (iv_len < 1)) {
        return CX_INVALID_PARAMETER;
    }
    if (NULL == ctx->info) {
        return CX_INVALID_PARAMETER;
    }
    return ctx->info->func->start(ctx->base_ctx, ctx->mode, iv, iv_len);
}

cx_err_t cx_aead_update_ad(cx_aead_context_t *ctx, const uint8_t *ad, size_t ad_len)
{
    if (NULL == ctx->info) {
        return CX_INVALID_PARAMETER;
    }
    return ctx->info->func->update_aad(ctx->base_ctx, ad, ad_len);
}

cx_err_t cx_aead_update(cx_aead_context_t *ctx,
                        uint8_t           *in,
                        size_t             in_len,
                        uint8_t           *out,
                        size_t            *out_len)
{
    if ((NULL == ctx->info) || (NULL == out_len)) {
        return CX_INVALID_PARAMETER;
    }
    *out_len = in_len;
    return ctx->info->func->update(ctx->base_ctx, in, out, in_len);
}

cx_err_t cx_aead_write_tag(cx_aead_context_t *ctx, uint8_t *tag, size_t tag_len)
{
    if ((NULL == ctx->info) || (NULL == tag)) {
        return CX_INVALID_PARAMETER;
    }
    return ctx->info->func->finish(ctx->base_ctx, tag, tag_len);
}

cx_err_t cx_aead_check_tag(cx_aead_context_t *ctx, const uint8_t *tag, size_t tag_len)
{
    if ((NULL == ctx->info) || (ctx->mode != CX_DECRYPT)) {
        return CX_INVALID_PARAMETER;
    }

    return ctx->info->func->check_tag(ctx->base_ctx, tag, tag_len);
}

cx_err_t cx_aead_encrypt(cx_aead_context_t *ctx,
                         const uint8_t     *iv,
                         size_t             iv_len,
                         const uint8_t     *ad,
                         size_t             ad_len,
                         uint8_t           *in,
                         size_t             in_len,
                         uint8_t           *out,
                         size_t            *out_len,
                         uint8_t           *tag,
                         size_t             tag_len)
{
    if (NULL == ctx->info) {
        return CX_INVALID_PARAMETER;
    }
    *out_len = in_len;
    return ctx->info->func->encrypt_and_tag(
        ctx->base_ctx, in, in_len, iv, iv_len, ad, ad_len, out, tag, tag_len);
}

cx_err_t cx_aead_decrypt(cx_aead_context_t *ctx,
                         const uint8_t     *iv,
                         size_t             iv_len,
                         const uint8_t     *ad,
                         size_t             ad_len,
                         uint8_t           *in,
                         size_t             in_len,
                         uint8_t           *out,
                         size_t            *out_len,
                         const uint8_t     *tag,
                         size_t             tag_len)
{
    if (NULL == ctx->info) {
        return CX_INVALID_PARAMETER;
    }
    *out_len = in_len;
    return ctx->info->func->auth_decrypt(
        ctx->base_ctx, in, in_len, iv, iv_len, ad, ad_len, out, tag, tag_len);
}

#endif  // HAVE_AEAD
