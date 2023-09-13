/* @BANNER@ */

#include "cx_cipher.h"
#include "os_math.h"
#include "os_utils.h"

#include <stddef.h>
#include <string.h>

#define CX_MAX_BLOCK_SIZE 16

static cx_err_t cx_cipher_check_out_len(cx_cipher_context_t *ctx, size_t in_len, size_t out_len)
{
    if (CX_CHAIN_CTR == ctx->mode) {
        if (out_len < in_len) {
            return CX_INVALID_PARAMETER;
        }
        return CX_OK;
    }
    if ((CX_SIGN == ctx->operation) || (CX_VERIFY == ctx->operation)) {
        if (out_len < 8) {
            return CX_INVALID_PARAMETER;
        }
    }
    else if (CX_DECRYPT == ctx->operation) {
        if (out_len < in_len) {
            return CX_INVALID_PARAMETER;
        }
    }
    else if (CX_ENCRYPT == ctx->operation) {
        if (NULL == ctx->add_padding) {
            if (out_len < UPPER_ALIGN(in_len, 8, size_t)) {
                return CX_INVALID_PARAMETER;
            }
        }
        else {
            if (out_len < UPPER_ALIGN(in_len + 7, 8, size_t)) {
                return CX_INVALID_PARAMETER;
            }
        }
    }
    return CX_OK;
}

static const cx_cipher_info_t *cx_cipher_get_info(const cx_cipher_id_t type)
{
    switch (type) {
#ifdef HAVE_AES
        case CX_CIPHER_AES_128:
            return &cx_aes_128_info;
        case CX_CIPHER_AES_192:
            return &cx_aes_192_info;
        case CX_CIPHER_AES_256:
            return &cx_aes_256_info;
#endif  // HAVE_AES
        default:
            return NULL;
    }
}

static cx_err_t get_no_padding(uint8_t *input, size_t in_len, size_t *data_len)
{
    if ((NULL == input) || (NULL == data_len)) {
        return CX_INVALID_PARAMETER;
    }
    *data_len = in_len;
    return CX_OK;
}

static void add_zeros_padding(uint8_t *output, size_t out_len, size_t data_len)
{
    size_t i;

    for (i = data_len; i < out_len; i++) {
        output[i] = 0x00;
    }
}

void add_one_and_zeros_padding(uint8_t *output, size_t out_len, size_t data_len)
{
    size_t padding_len = out_len - data_len;
    size_t i           = 0;

    output[data_len] = 0x80;
    for (i = 1; i < padding_len; i++) {
        output[data_len + i] = 0x00;
    }
}

static cx_err_t get_zeros_padding(uint8_t *input, size_t in_len, size_t *data_len)
{
    if ((NULL == input) || (NULL == data_len)) {
        return CX_INVALID_PARAMETER;
    }
    *data_len = in_len;
    return CX_OK;
}

static inline uint32_t neq_zero(uint32_t x)
{
    return (x | -x) >> 31;
}

static inline uint32_t conditional_choice(uint32_t a, uint32_t b, uint32_t cond)
{
    // Return a if cond is non zero, otherwise return b
    return (a ^ b) ^ (b & (-cond));
}

static cx_err_t get_one_and_zeros_padding(uint8_t *input, size_t in_len, size_t *data_len)
{
    if ((NULL == input) || (NULL == data_len)) {
        return CX_INVALID_PARAMETER;
    }
    int i;
    // keeps track of consecutive zeroes
    uint32_t acc = 0;
    // result[0] is 1 if the padding is correct
    // result[1] serves as a dummy value for constant time purposes
    uint8_t result[2] = {0};
    // padsize[0] will contain the padding length
    // padsize[1] is a dummy value
    uint8_t padsize[2] = {0};

    for (i = in_len - 1; i >= 0; i--) {
        uint32_t is_0x80           = neq_zero(input[i] ^ 0x80);  // 0 if current byte == 0x80
        uint32_t is_zero           = neq_zero(acc);
        result[is_0x80 | is_zero]  = 1;  // result[0] == 1 if t==0 and is_zero==0
        padsize[is_0x80 | is_zero] = i;  // update padded_size the same wa
        acc |= input[i];
    }
    *data_len           = padsize[0];  // this is 0 if padding is invalid
    uint32_t is_correct = neq_zero((uint32_t) result[0]);
    return conditional_choice(CX_OK, CX_INVALID_PARAMETER, is_correct);
}

static cx_err_t ecb_func(cx_cipher_context_t *ctx,
                         uint32_t             operation,
                         size_t               len,
                         const uint8_t       *input,
                         uint8_t             *output)
{
    uint32_t block_size = ctx->cipher_info->block_size;
    cx_err_t error      = CX_INTERNAL_ERROR;

    if (len % block_size) {
        return CX_INVALID_PARAMETER_VALUE;
    }
    while (len > 0) {
        if (CX_ENCRYPT == operation) {
            CX_CHECK(ctx->cipher_info->base->enc_func(input, output));
        }
        else if (CX_DECRYPT == operation) {
            CX_CHECK(ctx->cipher_info->base->dec_func(input, output));
        }
        else {
            return CX_INVALID_PARAMETER_VALUE;
        }
        input += block_size;
        output += block_size;
        len -= block_size;
    }
    error = CX_OK;

end:
    return error;
}

static cx_err_t cbc_func(cx_cipher_context_t *ctx,
                         uint32_t             operation,
                         size_t               len,
                         const uint8_t       *input,
                         uint8_t             *output)
{
    uint8_t  block[CX_MAX_BLOCK_SIZE];
    uint32_t block_size = ctx->cipher_info->block_size;
    cx_err_t error      = CX_INTERNAL_ERROR;

    if (len % block_size) {
        return CX_INVALID_PARAMETER_VALUE;
    }
    while (len > 0) {
        if (CX_DECRYPT == operation) {
            CX_CHECK(ctx->cipher_info->base->dec_func(input, output));
            output += block_size;
        }
        else {
            CX_CHECK(ctx->cipher_info->base->enc_func(input, block));
            if (CX_ENCRYPT == operation) {
                memcpy(output, block, block_size);
                output += block_size;
            }
        }
        input += block_size;
        len -= block_size;
    }
    memcpy(ctx->sig, block, block_size);
    error = CX_OK;

end:
    return error;
}

static size_t cx_cipher_verify_block(uint8_t *block, uint8_t *block_ref, size_t block_size)
{
    return (memcmp(block, block_ref, block_size) == 0) ? block_size : 0;
}

cx_err_t cx_cipher_init(cx_cipher_context_t *ctx)
{
    if (NULL == ctx) {
        return CX_INVALID_PARAMETER;
    }
    memset(ctx, 0, sizeof(cx_cipher_context_t));
    return CX_OK;
}

cx_err_t cx_cipher_setup(cx_cipher_context_t *ctx, const cx_cipher_id_t type, uint32_t mode)
{
    if (NULL == ctx) {
        return CX_INVALID_PARAMETER;
    }
    const cx_cipher_info_t *info = cx_cipher_get_info(type);
    if (NULL == info) {
        return CX_INVALID_PARAMETER;
    }
    if (NULL == ctx->cipher_key) {
        return CX_INVALID_PARAMETER;
    }
    switch (mode) {
        case CX_CHAIN_CTR:
            switch (type) {
                case CX_CIPHER_AES_128:
                case CX_CIPHER_AES_192:
                case CX_CIPHER_AES_256:
                    ctx->mode = mode;
                    break;
                default:
                    return CX_INVALID_PARAMETER_VALUE;
            }
            break;
        case CX_CHAIN_ECB:
        case CX_CHAIN_CBC:
            ctx->mode = mode;
            break;
        default:
            return CX_INVALID_PARAMETER_VALUE;
    }
    ctx->cipher_info = info;
    return CX_OK;
}

cx_err_t cx_cipher_setkey(cx_cipher_context_t *ctx,
                          const uint8_t       *key,
                          uint32_t             key_bitlen,
                          uint32_t             operation)
{
    uint32_t op_mode;

    if ((NULL == ctx) || (NULL == key) || (NULL == ctx->cipher_info)) {
        return CX_INVALID_PARAMETER;
    }
    if (ctx->cipher_info->key_bitlen != key_bitlen) {
        return CX_INVALID_PARAMETER_SIZE;
    }
    if ((operation != CX_ENCRYPT) && (operation != CX_DECRYPT) && (operation != CX_SIGN)
        && (operation != CX_VERIFY)) {
        return CX_INVALID_PARAMETER_VALUE;
    }
    ctx->key_bitlen = key_bitlen;
    ctx->operation  = operation;
    op_mode         = operation | ctx->mode;
    return ctx->cipher_info->base->setkey_func(ctx->cipher_key, op_mode, key, key_bitlen);
}

cx_err_t cx_cipher_setiv(cx_cipher_context_t *ctx, const uint8_t *iv, size_t iv_len)
{
    if ((NULL == ctx) || (NULL == ctx->cipher_info)) {
        return CX_INVALID_PARAMETER;
    }
    if (CX_CHAIN_ECB == ctx->mode) {
        return CX_OK;
    }
    if ((NULL == iv) && (0 != iv_len)) {
        return CX_INVALID_PARAMETER_VALUE;
    }

    if ((0 != iv_len) && (ctx->cipher_info->iv_size != iv_len)) {
        return CX_INVALID_PARAMETER_VALUE;
    }

    memcpy(ctx->iv, iv, iv_len);
    ctx->iv_size = iv_len;

    if (CX_CHAIN_CBC == ctx->mode) {
        ctx->cipher_info->base->enc_func(ctx->iv, ctx->iv);
    }
    return CX_OK;
}

cx_err_t cx_cipher_set_padding(cx_cipher_context_t *ctx, uint32_t padding)
{
    if ((NULL == ctx) || (NULL == ctx->cipher_info)) {
        return CX_INVALID_PARAMETER;
    }
    switch (padding) {
        case CX_PAD_NONE:
            ctx->add_padding = NULL;
            ctx->get_padding = get_no_padding;
            return CX_OK;
        case CX_PAD_ISO9797M1:
            ctx->add_padding = add_zeros_padding;
            ctx->get_padding = get_zeros_padding;
            return CX_OK;
        case CX_PAD_ISO9797M2:
            ctx->add_padding = add_one_and_zeros_padding;
            ctx->get_padding = get_one_and_zeros_padding;
            return CX_OK;
        default:
            return CX_INVALID_PARAMETER_VALUE;
    }
}

cx_err_t cx_cipher_update(cx_cipher_context_t *ctx,
                          const uint8_t       *input,
                          size_t               in_len,
                          uint8_t             *output,
                          size_t              *out_len)
{
    size_t   remain_len;
    size_t   block_size;
    cx_err_t error = CX_INTERNAL_ERROR;

    if ((NULL == ctx) || (NULL == ctx->cipher_info) || (NULL == out_len)) {
        return CX_INVALID_PARAMETER;
    }

    if (NULL == input) {
        return CX_INVALID_PARAMETER_VALUE;
    }

    CX_CHECK(cx_cipher_check_out_len(ctx, in_len, *out_len));

    *out_len   = 0;
    block_size = ctx->cipher_info->block_size;

    switch (ctx->mode) {
        case CX_CHAIN_ECB:
        case CX_CHAIN_CBC:
            if (in_len + ctx->unprocessed_len < block_size) {
                memcpy(&(ctx->unprocessed_data[ctx->unprocessed_len]), input, in_len);
                ctx->unprocessed_len += in_len;
                return CX_OK;
            }
            if (ctx->unprocessed_len != 0) {
                remain_len = block_size - ctx->unprocessed_len;
                memcpy(&(ctx->unprocessed_data[ctx->unprocessed_len]), input, remain_len);
                if (CX_CHAIN_ECB == ctx->mode) {
                    CX_CHECK(
                        ecb_func(ctx, ctx->operation, block_size, ctx->unprocessed_data, output));
                }
                else {
                    CX_CHECK(
                        cbc_func(ctx, ctx->operation, block_size, ctx->unprocessed_data, output));
                }
                if (ctx->operation != CX_SIGN) {
                    output += block_size;
                }
                *out_len += block_size;
                ctx->unprocessed_len = 0;
                input += remain_len;
                in_len -= remain_len;
            }
            if (in_len != 0) {
                remain_len = in_len % block_size;
                if ((remain_len == 0) && (ctx->operation == CX_DECRYPT)
                    && (ctx->add_padding != NULL)) {
                    remain_len = block_size;
                }
                memcpy(ctx->unprocessed_data, &(input[in_len - remain_len]), remain_len);
                ctx->unprocessed_len += remain_len;
                in_len -= remain_len;
            }
            if (in_len) {
                if (CX_CHAIN_ECB == ctx->mode) {
                    CX_CHECK(ecb_func(ctx, ctx->operation, in_len, input, output));
                }
                else {
                    CX_CHECK(cbc_func(ctx, ctx->operation, in_len, input, output));
                }
            }
            *out_len = in_len;
            return CX_OK;
        case CX_CHAIN_CTR:
            CX_CHECK(ctx->cipher_info->base->ctr_func(ctx->cipher_key,
                                                      in_len,
                                                      &ctx->unprocessed_len,
                                                      ctx->iv,
                                                      ctx->unprocessed_data,
                                                      input,
                                                      output));
            *out_len = in_len;
            return CX_OK;
        default:
            return CX_INVALID_PARAMETER_VALUE;
    }
end:
    return error;
}

cx_err_t cx_cipher_finish(cx_cipher_context_t *ctx, uint8_t *output, size_t *out_len)
{
    cx_err_t error = CX_INTERNAL_ERROR;
    if ((NULL == ctx) || (NULL == ctx->cipher_info) || (NULL == out_len)) {
        return CX_INVALID_PARAMETER;
    }

    *out_len = 0;
    switch (ctx->mode) {
        case CX_CHAIN_CTR:
            error = CX_OK;
            break;
        case CX_CHAIN_ECB:
        case CX_CHAIN_CBC:
            if ((CX_ENCRYPT == ctx->operation) || (CX_SIGN == ctx->operation)
                || (CX_VERIFY == ctx->operation)) {
                if (NULL == ctx->add_padding) {
                    if (ctx->unprocessed_len != 0) {
                        goto end;
                    }
                    error = CX_OK;
                    goto end;
                }
                if ((add_zeros_padding == ctx->add_padding) && (ctx->unprocessed_len == 0)) {
                    *out_len = 0;
                    error    = CX_OK;
                    goto end;
                }
                ctx->add_padding(
                    ctx->unprocessed_data, ctx->cipher_info->block_size, ctx->unprocessed_len);
            }
            if (get_no_padding != ctx->get_padding) {
                ctx->unprocessed_len = ctx->cipher_info->block_size;
            }
            if (CX_CHAIN_ECB == ctx->mode) {
                CX_CHECK(ecb_func(
                    ctx, ctx->operation, ctx->unprocessed_len, ctx->unprocessed_data, output));
            }
            else {
                CX_CHECK(cbc_func(
                    ctx, ctx->operation, ctx->unprocessed_len, ctx->unprocessed_data, output));
            }

            if ((CX_DECRYPT == ctx->operation) && (NULL != ctx->get_padding)) {
                error = ctx->get_padding(output, ctx->unprocessed_len, out_len);
                goto end;
            }

            *out_len = ctx->unprocessed_len;
            error    = CX_OK;
            break;
        default:
            error = CX_INVALID_PARAMETER_VALUE;
            break;
    }
end:
    ctx->cipher_info->base->ctx_reset();
    return error;
}

cx_err_t cx_cipher_mac(cx_cipher_context_t *ctx,
                       uint8_t             *output,
                       size_t              *out_len,
                       size_t              *finish_len)
{
    // Only in CBC
    if (CX_VERIFY == ctx->operation) {
        *finish_len = cx_cipher_verify_block(ctx->sig, output, ctx->cipher_info->block_size);
        *out_len    = 0;
    }
    else if (CX_SIGN == ctx->operation) {
        memcpy(output, ctx->sig, ctx->cipher_info->block_size);
        *finish_len = ctx->cipher_info->block_size;
        *out_len    = 0;
    }
    else {
        return CX_OK;
    }
    return CX_OK;
}

cx_err_t cx_cipher_enc_dec(cx_cipher_context_t *ctx,
                           const uint8_t       *iv,
                           size_t               iv_len,
                           const uint8_t       *input,
                           size_t               in_len,
                           uint8_t             *output,
                           size_t              *out_len)
{
    cx_err_t error      = CX_INTERNAL_ERROR;
    size_t   finish_len = 0;

    CX_CHECK(cx_cipher_setiv(ctx, iv, iv_len));
    CX_CHECK(cx_cipher_update(ctx, input, in_len, output, out_len));
    if (ctx->add_padding != NULL) {
        finish_len = *out_len;
    }
    CX_CHECK(cx_cipher_finish(ctx, output + finish_len, &finish_len));
    CX_CHECK(cx_cipher_mac(ctx, output, out_len, &finish_len));
    *out_len += finish_len;

end:
    return error;
}

void cx_cipher_reset(cx_cipher_context_t *ctx)
{
    memset(ctx->iv, 0, MAX_IV_LENGTH);
    memset(ctx->unprocessed_data, 0, MAX_BLOCK_LENGTH);
    ctx->unprocessed_len = 0;
    ctx->iv_size         = 0;
}

#ifdef UNITTEST
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>
static void test_iso9797_method2_unpad(void **state)
{
    uint8_t buf1[8]       = {0, 1, 2, 3, 4, 5, 6, 0x80};
    size_t  unpadded_size = 0;

    assert_int_equal(get_one_and_zeros_padding(buf1, 8, &unpadded_size), CX_OK);
    assert_int_equal(unpadded_size, 7);

    uint8_t buf2[8] = {0, 1, 2, 3, 0x80, 0, 0, 0};
    unpadded_size   = 0;

    assert_int_equal(get_one_and_zeros_padding(buf2, 8, &unpadded_size), CX_OK);
    assert_int_equal(unpadded_size, 4);

    unpadded_size   = 0;
    uint8_t buf3[8] = {0, 1, 2, 3, 4, 5, 6, 7};

    assert_int_equal(get_one_and_zeros_padding(buf3, 8, &unpadded_size), CX_INVALID_PARAMETER);

    uint8_t buf4[8] = {0, 1, 2, 3, 4, 5, 0x80, 7};
    unpadded_size   = 0;

    assert_int_equal(get_one_and_zeros_padding(buf4, 8, &unpadded_size), CX_INVALID_PARAMETER);

    uint8_t buf5[8] = {0x80, 0, 0, 0, 0, 0, 0, 0};
    unpadded_size   = 0;

    assert_int_equal(get_one_and_zeros_padding(buf5, 8, &unpadded_size), CX_OK);
    assert_int_equal(unpadded_size, 0);
}

static void test_iso9797_method2_pad(void **state)
{
    uint8_t       buf1[8]        = {0};
    const uint8_t buf1_padded[8] = {0x80, 0, 0, 0, 0, 0, 0, 0};

    add_one_and_zeros_padding(buf1, 8, 0);
    assert_memory_equal(buf1, buf1_padded, sizeof(buf1_padded));

    uint8_t       buf2[8]        = {0xaa, 0xbb, 0xcc};
    const uint8_t buf2_padded[8] = {0xaa, 0xbb, 0xcc, 0x80, 0, 0, 0, 0};

    add_one_and_zeros_padding(buf2, 8, 3);
    assert_memory_equal(buf2, buf2_padded, sizeof(buf2_padded));

    uint8_t       buf3[16]        = {0, 1, 2, 3, 4, 5, 6, 7};
    const uint8_t buf3_padded[16] = {0, 1, 2, 3, 4, 5, 6, 7, 0x80, 0, 0, 0, 0, 0, 0, 0};

    add_one_and_zeros_padding(buf3, 16, 8);
    assert_memory_equal(buf3, buf3_padded, sizeof(buf3_padded));
}

int main()
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_iso9797_method2_pad),
        cmocka_unit_test(test_iso9797_method2_unpad),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
#endif  // UNITTEST
