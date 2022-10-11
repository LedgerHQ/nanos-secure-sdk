/* @BANNER@ */

#ifdef HAVE_AES

#include "cx_cipher.h"
#include "cx_ram.h"

#include <stddef.h>
#include <string.h>

cx_err_t cx_aes_init_key_no_throw(const uint8_t *raw_key, size_t key_len, cx_aes_key_t *key) {
  memset(key, 0, sizeof(cx_aes_key_t));
  switch (key_len) {
  case 16:
  case 24:
  case 32:
    key->size = key_len;
    memmove(key->keys, raw_key, key_len);
    return CX_OK;
  default:
    return CX_INVALID_PARAMETER;
  }
}

cx_err_t cx_aes_enc_block(const cx_aes_key_t *key, const uint8_t *inblock, uint8_t *outblock) {
  cx_err_t error;
  CX_CHECK(cx_aes_set_key_hw(key, CX_ENCRYPT));
  CX_CHECK(cx_aes_block_hw(inblock, outblock));
  cx_aes_reset_hw();
  end:
    return error;
}

cx_err_t cx_aes_dec_block(const cx_aes_key_t *key, const uint8_t *inblock, uint8_t *outblock) {
  cx_err_t error;
  CX_CHECK(cx_aes_set_key_hw(key, CX_DECRYPT));
  CX_CHECK(cx_aes_block_hw(inblock, outblock));
  cx_aes_reset_hw();
  end:
    return error;
}

cx_err_t cx_aes_iv_no_throw(const cx_aes_key_t *key,
                            uint32_t            mode,
                            const uint8_t *     iv,
                            size_t              iv_len,
                            const uint8_t *     in,
                            size_t              in_len,
                            uint8_t *           out,
                            size_t *            out_len) {
  cx_cipher_context_t *ctx;
  cx_cipher_id_t      type;
  cx_err_t            error;
  uint32_t            operation;

  switch (key->size) {
    case 16:
      type = CX_CIPHER_AES_128;
      break;
    case 24:
      type = CX_CIPHER_AES_192;
      break;
    case 32:
      type = CX_CIPHER_AES_256;
      break;
    default:
      return CX_INVALID_PARAMETER;
  }

  ctx = &G_cx.cipher;
  operation = mode & CX_MASK_SIGCRYPT;
  if (   (operation != CX_ENCRYPT) && (operation != CX_DECRYPT)
      && (operation != CX_SIGN) && (operation != CX_VERIFY)) {
      return CX_INVALID_PARAMETER_VALUE;
  }
  CX_CHECK(cx_cipher_init(ctx));
  // Set key
  ctx->key_bitlen = key->size * 8;
  ctx->operation  = operation;
  ctx->cipher_key = (const cipher_key_t*)key;
  CX_CHECK(cx_cipher_setup(ctx, type, mode & CX_MASK_CHAIN));
  CX_CHECK(cx_cipher_set_padding(ctx, mode & CX_MASK_PAD));
  CX_CHECK(cx_cipher_enc_dec(ctx, iv, iv_len, in, in_len, out, out_len));

  end:
    explicit_bzero(ctx, sizeof(cx_cipher_context_t));
    return error;
}

cx_err_t cx_aes_no_throw(const cx_aes_key_t *key, uint32_t mode, const uint8_t *in, size_t in_len, uint8_t *out, size_t *out_len) {
  return cx_aes_iv_no_throw(key, mode, NULL, 0, in, in_len, out, out_len);
}

cx_err_t aes_ctr(cx_aes_key_t *ctx_key, size_t len, size_t *nc_off, uint8_t *nonce_counter, uint8_t *stream_block, 
                 const uint8_t *input, uint8_t *output) {
  uint8_t  c;
  size_t   n     = *nc_off;
  cx_err_t error = CX_INVALID_PARAMETER;

  while (len--) {
    if (n == 0) {
      CX_CHECK(cx_aes_enc_block(ctx_key, nonce_counter, stream_block));
        for (int i = CX_AES_BLOCK_SIZE; i > 0; i--) {
          if (++nonce_counter[i - 1] != 0) {
            break;
          }
        }
    }
    c         = *input++;
    *output++ = c ^ stream_block[n];
    n         = (n + 1) & 0x0F;
  }
  *nc_off = n;
  error   = CX_OK;

  end:
    return error;
}

cx_err_t aes_setkey(cx_aes_key_t *ctx_key, uint32_t operation, const uint8_t *key, uint32_t key_bitlen) {
  cx_err_t error;
  CX_CHECK(cx_aes_init_key_no_throw(key, key_bitlen/8, ctx_key));
  CX_CHECK(cx_aes_set_key_hw(ctx_key, operation));
  end:
    return error;
}

static const cx_cipher_base_t aes_base = {
  (cx_err_t (*) (const cipher_key_t *ctx_key, const uint8_t *inblock, uint8_t *outblock))cx_aes_enc_block,
  (cx_err_t (*) (const cipher_key_t *ctx_key, const uint8_t *inblock, uint8_t *outblock))cx_aes_dec_block,
  (cx_err_t(*) (const cipher_key_t *ctx_key, size_t len, size_t *nc_off, uint8_t *nonce_counter, uint8_t *stream_block, const uint8_t *input, uint8_t *output))aes_ctr,
  (cx_err_t(*) (const cipher_key_t *ctx_key, uint32_t operation, const uint8_t *key, uint32_t key_bitlen))aes_setkey,
  (cx_err_t(*)(void))cx_aes_reset_hw
};

const cx_cipher_info_t cx_aes_128_info = {
  128,
  16,
  CX_AES_BLOCK_SIZE,
  &aes_base
};

const cx_cipher_info_t cx_aes_192_info = {
  192,
  16,
  CX_AES_BLOCK_SIZE,
  &aes_base
};

const cx_cipher_info_t cx_aes_256_info = {
  256,
  16,
  CX_AES_BLOCK_SIZE,
  &aes_base
};

#endif // HAVE_AES
