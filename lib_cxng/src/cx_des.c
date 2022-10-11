/* @BANNER@ */

#ifdef HAVE_DES

#include "cx_cipher.h"
#include "cx_ram.h"

#include <stddef.h>
#include <string.h>

cx_err_t cx_des_init_key_no_throw(const uint8_t *raw_key, size_t key_len, cx_des_key_t *key) {
  memset(key, 0, sizeof(cx_des_key_t));
  switch (key_len) {
  case 8:
  case 16:
  case 24:
    key->size = (uint8_t)key_len;
    memmove(key->keys, raw_key, key_len);
    return CX_OK;

  default:
    return CX_INVALID_PARAMETER;
  }
}

cx_err_t cx_des_enc_block(const cx_des_key_t *key, const uint8_t *inblock, uint8_t *outblock) {
  cx_err_t error;
  CX_CHECK(cx_des_set_key_hw(key, CX_ENCRYPT));
  cx_des_block_hw(inblock, outblock);
  cx_des_reset_hw();
  end:
    return error;
}

cx_err_t cx_des_dec_block(const cx_des_key_t *key, const uint8_t *inblock, uint8_t *outblock) {
  cx_err_t error;
  CX_CHECK(cx_des_set_key_hw(key, CX_DECRYPT));
  cx_des_block_hw(inblock, outblock);
  cx_des_reset_hw();
  end:
    return error;
}

cx_err_t cx_des_iv_no_throw(const cx_des_key_t *key,
                            uint32_t            mode,
                            const uint8_t *     iv,
                            size_t              iv_len,
                            const uint8_t *     in,
                            size_t              len,
                            uint8_t *           out,
                            size_t *            out_len) {
  cx_cipher_context_t *ctx;
  cx_cipher_id_t      type;
  cx_err_t            error;
  uint32_t            operation;

  switch (key->size) {
    case 8:
      type = CX_CIPHER_DES_64;
      break;
    case 16:
      type = CX_CIPHER_3DES_128;
      break;
    case 24:
      type = CX_CIPHER_3DES_192;
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
  CX_CHECK(cx_cipher_enc_dec(ctx, iv, iv_len, in, len, out, out_len));

  end:
    explicit_bzero(ctx, sizeof(cx_cipher_context_t));
    return error;

}
cx_err_t cx_des_no_throw(const cx_des_key_t *key, uint32_t mode, const uint8_t *in, size_t len, uint8_t *out, size_t *out_len) {
  return cx_des_iv_no_throw(key, mode, NULL, 0, in, len, out, out_len);
}

cx_err_t des_setkey(cx_des_key_t *ctx_key, uint32_t operation, const uint8_t *key, uint32_t key_bitlen) {
  cx_err_t error;
  CX_CHECK(cx_des_init_key_no_throw(key, key_bitlen/8, ctx_key));
  CX_CHECK(cx_des_set_key_hw(ctx_key, operation));
  end:
    return error;
}

static const cx_cipher_base_t des_base = {
  (cx_err_t (*) (const cipher_key_t *ctx_key, const uint8_t *inblock, uint8_t *outblock))cx_des_enc_block,
  (cx_err_t (*) (const cipher_key_t *ctx_key, const uint8_t *inblock, uint8_t *outblock))cx_des_dec_block,
  NULL,
  (cx_err_t(*) (const cipher_key_t *ctx_key, uint32_t operation, const uint8_t *key, uint32_t key_bitlen))des_setkey,
  (cx_err_t(*)(void))cx_des_reset_hw
};

const cx_cipher_info_t cx_des_64_info = {
  CX_DES_KEY_LENGTH,
  8,
  CX_DES_BLOCK_SIZE,
  &des_base
};

const cx_cipher_info_t cx_3des_128_info = {
  CX_3DES_2_KEY_LENGTH,
  8,
  CX_DES_BLOCK_SIZE,
  &des_base
};

const cx_cipher_info_t cx_3des_192_info = {
  CX_3DES_3_KEY_LENGTH,
  8,
  CX_DES_BLOCK_SIZE,
  &des_base
};

#endif // HAVE_DES
