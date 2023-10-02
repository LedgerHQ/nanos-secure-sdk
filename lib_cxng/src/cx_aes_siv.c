/* @BANNER@ */

#ifdef HAVE_AES_SIV
#if defined(HAVE_AES) && defined(HAVE_CMAC)
#include "cx_cmac.h"
#include "cx_utils.h"
#include "lcx_cmac.h"
#include "lcx_aes_siv.h"

static void cx_clear_bits(uint8_t *input) {
  input[8]  &= 0x7f;
  input[12] &= 0x7f;
}

cx_err_t cx_aes_siv_init(cx_aes_siv_context_t *ctx) {
  cx_err_t error;
  CX_CHECK(cx_cipher_init(ctx->cipher_ctx));

  end:
    return error;
}

cx_err_t cx_aes_siv_set_key(cx_aes_siv_context_t *ctx,
                            const uint8_t *key, size_t key_bitlen) {

  // AES SIV uses two keys of either 128, 192 or 256 bits each
  if (key_bitlen > AES_SIV_MAX_KEY_LEN * AES_SIV_KEY_NUMBER * 8) {
    return CX_INVALID_PARAMETER_SIZE;
  }

  ctx->key_len = key_bitlen/2;
  memcpy(ctx->key1, key, ctx->key_len/8);
  memcpy(ctx->key2, key + ctx->key_len/8, ctx->key_len/8);
  return CX_OK;
}

cx_err_t cx_aes_siv_start(cx_aes_siv_context_t *ctx,
                          uint32_t mode,
                          const uint8_t *iv,
                          size_t iv_len) {
  uint8_t iv_used[CX_AES_BLOCK_SIZE] = {0};
  cx_err_t error;

  // Set up cipher to be used for CMAC computation
  CX_CHECK(cx_cipher_setup(ctx->cipher_ctx, ctx->cipher_type, CX_CHAIN_ECB));
  CX_CHECK(cx_cmac_start(ctx->cipher_ctx, ctx->key1, ctx->key_len));
  CX_CHECK(cx_cmac_update(ctx->cipher_ctx, iv_used, CX_AES_BLOCK_SIZE));
  CX_CHECK(cx_cmac_finish(ctx->cipher_ctx, ctx->tag_state));

  if (CX_DECRYPT == mode) {
    // Prepare for the AES-CTR computation
    memcpy(iv_used, iv, iv_len);
    cx_clear_bits(iv_used);
    CX_CHECK(cx_cipher_setup(ctx->cipher_ctx, ctx->cipher_type, CX_CHAIN_CTR));
    CX_CHECK(cx_cipher_setkey(ctx->cipher_ctx, ctx->key2, ctx->key_len, CX_ENCRYPT));
    CX_CHECK(cx_cipher_setiv(ctx->cipher_ctx, iv_used, CX_AES_BLOCK_SIZE));
  }
  ctx->mode = mode;

  end:
    return error;
}

cx_err_t cx_aes_siv_update_aad(cx_aes_siv_context_t *ctx,
                               const uint8_t *aad,
                               size_t aad_len) {
  uint8_t  tmp[CX_AES_BLOCK_SIZE] = {0};
  cx_err_t error;

  if (NULL == aad) {
    return CX_OK;
  }

  CX_CHECK(cx_cipher_setup(ctx->cipher_ctx, ctx->cipher_type, CX_CHAIN_ECB));
  CX_CHECK(cx_cmac_shift_and_xor(tmp, ctx->tag_state, CX_AES_BLOCK_SIZE));
  CX_CHECK(cx_cmac_start(ctx->cipher_ctx, ctx->key1, ctx->key_len));
  CX_CHECK(cx_cmac_update(ctx->cipher_ctx, aad, aad_len));
  CX_CHECK(cx_cmac_finish(ctx->cipher_ctx, ctx->tag_state));
  cx_memxor(ctx->tag_state, tmp, CX_AES_BLOCK_SIZE);

  end:
    return error;
}

cx_err_t cx_aes_siv_update(cx_aes_siv_context_t *ctx,
                           const uint8_t *input,
                           uint8_t *output, size_t len) {
  size_t out_len = len;
  cx_err_t error;
  CX_CHECK(cx_cipher_update(ctx->cipher_ctx, input, len, output, &out_len));
  cx_cipher_reset(ctx->cipher_ctx);

  end:
    return error;
}

static cx_err_t cx_aes_siv_check_tag(cx_aes_siv_context_t *ctx,
                                     const uint8_t *tag) {
  uint8_t  diff;
  diff  = cx_constant_time_eq(tag, ctx->tag_state, CX_AES_BLOCK_SIZE);
  return diff * CX_INVALID_PARAMETER_VALUE + (1 - diff) * CX_OK;
}

cx_err_t cx_aes_siv_finish(cx_aes_siv_context_t *ctx,
                           const uint8_t *input, size_t in_len,
                           uint8_t *tag) {
  uint8_t  tmp[CX_AES_BLOCK_SIZE] = {0};
  cx_err_t error;

  CX_CHECK(cx_cipher_setup(ctx->cipher_ctx, ctx->cipher_type, CX_CHAIN_ECB));
  if (in_len < CX_AES_BLOCK_SIZE) {
    CX_CHECK(cx_cmac_shift_and_xor(tmp, ctx->tag_state, CX_AES_BLOCK_SIZE));
    memset(ctx->tag_state, 0, CX_AES_BLOCK_SIZE);
    memcpy(ctx->tag_state, input, in_len);
    add_one_and_zeros_padding(ctx->tag_state, CX_AES_BLOCK_SIZE, in_len);
    cx_memxor(tmp, ctx->tag_state, CX_AES_BLOCK_SIZE);
    CX_CHECK(cx_cmac_start(ctx->cipher_ctx, ctx->key1, ctx->key_len));
    CX_CHECK(cx_cmac_update(ctx->cipher_ctx, tmp, CX_AES_BLOCK_SIZE));
    CX_CHECK(cx_cmac_finish(ctx->cipher_ctx, ctx->tag_state));
  }
  else {
    CX_CHECK(cx_cmac_start(ctx->cipher_ctx, ctx->key1, ctx->key_len));
    CX_CHECK(cx_cmac_update(ctx->cipher_ctx, input, in_len - CX_AES_BLOCK_SIZE));
    cx_memxor(ctx->tag_state, input + in_len - CX_AES_BLOCK_SIZE, CX_AES_BLOCK_SIZE);
    CX_CHECK(cx_cmac_update(ctx->cipher_ctx, ctx->tag_state, CX_AES_BLOCK_SIZE));
    CX_CHECK(cx_cmac_finish(ctx->cipher_ctx, ctx->tag_state));
  }
  if (CX_DECRYPT == ctx->mode) {
    return cx_aes_siv_check_tag(ctx, tag);
  }
  memcpy(tag, ctx->tag_state, CX_AES_BLOCK_SIZE);
  // Prepare for the AES-CTR computation
  cx_clear_bits(ctx->tag_state);
  CX_CHECK(cx_cipher_setup(ctx->cipher_ctx, ctx->cipher_type, CX_CHAIN_CTR));
  CX_CHECK(cx_cipher_setkey(ctx->cipher_ctx, ctx->key2, ctx->key_len, CX_ENCRYPT));
  CX_CHECK(cx_cipher_setiv(ctx->cipher_ctx, ctx->tag_state, CX_AES_BLOCK_SIZE));

  end:
    return error;

}

cx_err_t cx_aes_siv_encrypt(cx_aes_siv_context_t *ctx,
                            const uint8_t *input,
                            size_t in_len,
                            const uint8_t *aad,
                            size_t aad_len,
                            uint8_t *output,
                            uint8_t *tag) {
  cx_err_t error;
  CX_CHECK(cx_aes_siv_start(ctx, CX_ENCRYPT, NULL, 0));
  CX_CHECK(cx_aes_siv_update_aad(ctx, aad, aad_len));
  CX_CHECK(cx_aes_siv_finish(ctx, input, in_len, tag));
  CX_CHECK(cx_aes_siv_update(ctx, input, output, in_len));
  end:
    return error;
}

cx_err_t cx_aes_siv_decrypt(cx_aes_siv_context_t *ctx,
                            const uint8_t *input,
                            size_t in_len,
                            const uint8_t *aad,
                            size_t aad_len,
                            uint8_t *output,
                            uint8_t *tag) {
  cx_err_t error;
  CX_CHECK(cx_aes_siv_start(ctx, CX_DECRYPT, tag, CX_AES_BLOCK_SIZE));
  CX_CHECK(cx_aes_siv_update(ctx, input, output, in_len));
  CX_CHECK(cx_aes_siv_update_aad(ctx, aad, aad_len));
  CX_CHECK(cx_aes_siv_finish(ctx, output, in_len, tag));

  end:
    return error;
}

#endif // HAVE_AES && HAVE_CMAC
#endif // HAVE_AES_SIV
