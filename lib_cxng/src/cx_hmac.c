
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

#ifdef HAVE_HMAC

#include "cx_hmac.h"
#include "cx_hash.h"
#include "cx_ram.h"
#include "errors.h"
#include "exceptions.h"
#include "lcx_common.h"

#include <string.h>

#define IPAD 0x36u
#define OPAD 0x5cu

static size_t cx_get_block_size(cx_md_t md) {
  const cx_hash_info_t *info = cx_hash_get_info(md);
  if (info == NULL) {
    return 0;
  }
  return info->block_size;
}

static cx_md_t cx_get_algorithm(cx_hmac_t *ctx) {
  return ctx->hash_ctx.info->md_type;
}

static bool cx_is_allowed_digest(cx_md_t md_type) {
  const cx_md_t allowed_algorithms[] = {
#ifdef HAVE_SHA224
    CX_SHA224,
#endif
#ifdef HAVE_SHA256
    CX_SHA256,
#endif
#ifdef HAVE_SHA384
    CX_SHA384,
#endif
#ifdef HAVE_SHA512
    CX_SHA512,
#endif
#ifdef HAVE_RIPEMD160
    CX_RIPEMD160,
#endif
  };
  for (unsigned int i = 0; i < sizeof(allowed_algorithms) / sizeof(allowed_algorithms[0]); i++) {
    if (allowed_algorithms[i] == md_type) {
      return true;
    }
  }
  return false;
}

cx_err_t cx_hmac_init(cx_hmac_t *ctx, cx_md_t hash_id, const uint8_t *key, size_t key_len) {
  cx_hash_t *hash_ctx;

  if ((ctx == NULL) ||
      (!cx_is_allowed_digest(hash_id)) ||
      (key == NULL && key_len != 0)) {
    return CX_INVALID_PARAMETER;
  }

  hash_ctx = &ctx->hash_ctx,
  memset(ctx, 0, sizeof(cx_hmac_t));
  size_t block_size = cx_get_block_size(hash_id);

  if (key) {
    if (key_len > block_size) {
      cx_hash_init(hash_ctx, hash_id);
      cx_hash_update(hash_ctx, key, key_len);
      cx_hash_final(hash_ctx, ctx->key);
    } else {
      memcpy(ctx->key, key, key_len);
    }

    for (unsigned int i = 0; i < block_size; i++) {
      ctx->key[i] ^= IPAD;
    }
  }

  cx_hash_init(hash_ctx, hash_id);
  cx_hash_update(hash_ctx, ctx->key, block_size);
  return CX_OK;
}

cx_err_t cx_hmac_update(cx_hmac_t *ctx, const uint8_t *data, size_t data_len) {
  if (data_len == 0) {
    return CX_OK;
  }
  return cx_hash_update(&ctx->hash_ctx, data, data_len);
}

cx_err_t cx_hmac_final(cx_hmac_t *ctx, uint8_t *out, size_t *out_len) {
  uint8_t inner_hash[MAX_HASH_SIZE];
  uint8_t hkey[MAX_HASH_BLOCK_SIZE];

  cx_hash_t *hash_ctx = &ctx->hash_ctx;

  cx_md_t hash_algorithm   = cx_get_algorithm(ctx);
  size_t  block_size       = cx_get_block_size(hash_algorithm);
  size_t  hash_output_size = cx_hash_get_size(hash_ctx);

  cx_hash_final(hash_ctx, inner_hash);

  // hash key xor 5c (and 36 to remove prepadding at init)
  memcpy(hkey, ctx->key, block_size);
  for (unsigned int i = 0; i < block_size; i++) {
    hkey[i] ^= OPAD ^ IPAD;
  }

  cx_hash_init(hash_ctx, hash_algorithm);
  cx_hash_update(hash_ctx, hkey, block_size);
  cx_hash_update(hash_ctx, inner_hash, hash_output_size);
  cx_hash_final(hash_ctx, hkey);

  // length result
  if (*out_len >= hash_output_size) {
    *out_len = hash_output_size;
  }
  memcpy(out, hkey, *out_len);
  return CX_OK;
}

#ifdef HAVE_SHA224
cx_err_t cx_hmac_sha224_init(cx_hmac_sha256_t *hmac, const uint8_t *key, unsigned int key_len) {
  return cx_hmac_init((cx_hmac_t *)hmac, CX_SHA224, key, key_len);
}
#endif
#ifdef HAVE_SHA256
cx_err_t cx_hmac_sha256_init_no_throw(cx_hmac_sha256_t *hmac, const uint8_t *key, size_t key_len) {
  return cx_hmac_init((cx_hmac_t *)hmac, CX_SHA256, key, key_len);
}
#endif

#ifdef HAVE_SHA384
cx_err_t cx_hmac_sha384_init(cx_hmac_sha512_t *hmac, const uint8_t *key, unsigned int key_len) {
  return cx_hmac_init((cx_hmac_t *)hmac, CX_SHA384, key, key_len);
}
#endif
#ifdef HAVE_SHA512
cx_err_t cx_hmac_sha512_init_no_throw(cx_hmac_sha512_t *hmac, const uint8_t *key, size_t key_len) {
  return cx_hmac_init((cx_hmac_t *)hmac, CX_SHA512, key, key_len);
}
#endif

#ifdef HAVE_RIPEMD160
cx_err_t cx_hmac_ripemd160_init_no_throw(cx_hmac_ripemd160_t *hmac, const uint8_t *key, size_t key_len) {
  return cx_hmac_init((cx_hmac_t *)hmac, CX_RIPEMD160, key, key_len);
}
#endif

cx_err_t cx_hmac_no_throw(cx_hmac_t *hmac, uint32_t mode, const uint8_t *in, size_t len, uint8_t *out, size_t out_len) {
  size_t output_size = 0;
  cx_err_t error = CX_OK;

  if (in == NULL && len != 0) {
    return CX_INVALID_PARAMETER;
  }
  if (out == NULL && out_len != 0) {
    return CX_INVALID_PARAMETER;
  }

  if (in != NULL) {
    CX_CHECK(cx_hmac_update(hmac, in, len));
  }

  if (mode & CX_LAST) {
    output_size = out_len;
    cx_hmac_final(hmac, out, &output_size);

    if (!(mode & CX_NO_REINIT)) {
      cx_hmac_init(hmac, cx_get_algorithm(hmac), NULL, 0);
    }
  }

 end:
  return error;
}

#ifdef HAVE_SHA256
size_t cx_hmac_sha256(const uint8_t *key, size_t key_len, const uint8_t *in, size_t len, uint8_t *out, size_t out_len) {
  size_t mac_len = out_len;
  cx_hmac_init(&G_cx.hmac, CX_SHA256, key, key_len);
  cx_hmac_update(&G_cx.hmac, in, len);
  cx_hmac_final(&G_cx.hmac, out, &mac_len);
  explicit_bzero(&G_cx.hmac, sizeof(cx_hmac_sha256_t));
  return mac_len;
}
#endif

#ifdef HAVE_SHA512
size_t cx_hmac_sha512(const uint8_t *key,
                      size_t         key_len,
                      const uint8_t *in,
                      size_t         len,
                      uint8_t *      out,
                      size_t         out_len) {
  size_t mac_len = out_len;
  cx_hmac_init(&G_cx.hmac, CX_SHA512, key, key_len);
  cx_hmac_update(&G_cx.hmac, in, len);
  cx_hmac_final(&G_cx.hmac, out, &mac_len);
  explicit_bzero(&G_cx.hmac, sizeof(cx_hmac_sha512_t));
  return mac_len;
}
#endif

#endif // HAVE_HMAC
