
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

#ifdef HAVE_HASH

#include <string.h>

#include "cx_hash.h"
#include "cx_utils.h"
#include "cx_ram.h"

#include "cx_blake2b.h"
#include "cx_ripemd160.h"
#include "cx_sha256.h"
#include "cx_sha3.h"
#include "cx_sha512.h"

#include "errors.h"
#include "exceptions.h"

const cx_hash_info_t *cx_hash_get_info(cx_md_t md_type) {
  switch (md_type) {
#ifdef HAVE_SHA256
  case CX_SHA256:
    return &cx_sha256_info;
#endif

#ifdef HAVE_RIPEMD160
  case CX_RIPEMD160:
    return &cx_ripemd160_info;
#endif

#ifdef HAVE_SHA224
  case CX_SHA224:
    return &cx_sha224_info;
#endif

#ifdef HAVE_SHA384
  case CX_SHA384:
    return &cx_sha384_info;
#endif

#ifdef HAVE_SHA512
  case CX_SHA512:
    return &cx_sha512_info;
#endif

#ifdef HAVE_SHA3
  case CX_SHA3:
  case CX_SHA3_256:
  case CX_SHA3_512:
    return &cx_sha3_info;
  case CX_KECCAK:
    return &cx_keccak_info;
  case CX_SHAKE128:
    return &cx_shake128_info;
  case CX_SHAKE256:
    return &cx_shake256_info;
#endif

#ifdef HAVE_BLAKE2
  case CX_BLAKE2B:
    return &cx_blake2b_info;
#endif

  default:
    return NULL;
  }
}

size_t cx_hash_get_size(const cx_hash_t *ctx) {
  const cx_hash_info_t *info = ctx->info;
  if (info->output_size) {
    return info->output_size;
  }
  return info->output_size_func(ctx);
}

cx_err_t cx_hash_init(cx_hash_t *ctx, cx_md_t md_type) {
  const cx_hash_info_t *info = cx_hash_get_info(md_type);
  if (info == NULL) {
    return CX_INVALID_PARAMETER;
  }
  if (info->output_size == 0) { /* variable output size, must use cx_hash_init_ex */
    return CX_INVALID_PARAMETER;
  }
  info->init_func(ctx);
  return CX_OK;
}

cx_err_t cx_hash_init_ex(cx_hash_t *ctx, cx_md_t md_type, size_t output_size) {
  const cx_hash_info_t *info = cx_hash_get_info(md_type);
  if (info == NULL) {
    return CX_INVALID_PARAMETER;
  }
  if (info->output_size != 0) {
    if (info->output_size != output_size) {
      return CX_INVALID_PARAMETER;
    }
    return cx_hash_init(ctx, md_type);
  }
  return info->init_ex_func(ctx, output_size * 8);
}

cx_err_t cx_hash_update(cx_hash_t *ctx, const uint8_t *data, size_t len) {
  const cx_hash_info_t *info = ctx->info;
  return info->update_func(ctx, data, len);
}

cx_err_t cx_hash_final(cx_hash_t *ctx, uint8_t *digest) {
  const cx_hash_info_t *info = ctx->info;
  info->finish_func(ctx, digest);
  return CX_OK;
}

cx_err_t cx_hash_no_throw(cx_hash_t *hash, uint32_t mode, const uint8_t *in, size_t len, uint8_t *out, size_t out_len) {
  unsigned int digest_len;
  cx_err_t error;

  // --- init locals ---
  digest_len = (unsigned int)cx_hash_get_size(hash);
  CX_CHECK(cx_hash_update(hash, in, len));

  if (mode & CX_LAST) {
    if (out_len < digest_len) {
      return INVALID_PARAMETER;
    }
    CX_CHECK(cx_hash_final(hash, out));
  }

 end:
  return error;
}

#endif // HAVE_HASH
