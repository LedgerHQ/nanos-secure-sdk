
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

#ifdef HAVE_SHA3

#include "cx_sha3.h"
#include "cx_errors.h"
#include "cx_utils.h"
#include "errors.h"
#include <stdbool.h>
#include <string.h>

#ifndef ARCH_LITTLE_ENDIAN
#error "Big endian architectures are not supported. Please abort your project"
#endif

#ifndef NATIVE_64BITS // NO 64BITS natively supported by the compiler
#error sha3 require 64 bits support at compiler level (NATIVE_64BITS option)
#endif

const cx_hash_info_t cx_sha3_info = {
  CX_SHA3,
  0,
  0,
  NULL,
  (cx_err_t (*)(cx_hash_t *ctx, const uint8_t *data, size_t len))cx_sha3_update,
  (cx_err_t (*)(cx_hash_t *ctx, uint8_t *digest))cx_sha3_final,
  (cx_err_t (*)(cx_hash_t *ctx, size_t output_size))cx_sha3_init_no_throw,
  (size_t(*)(const cx_hash_t *ctx))cx_sha3_get_output_size
};

const cx_hash_info_t cx_keccak_info = {
  CX_KECCAK,
  0,
  0,
  NULL,
  (cx_err_t (*)(cx_hash_t *ctx, const uint8_t *data, size_t len))cx_sha3_update,
  (cx_err_t (*)(cx_hash_t *ctx, uint8_t *digest))cx_sha3_final,
  (cx_err_t (*)(cx_hash_t *ctx, size_t output_size))cx_keccak_init_no_throw,
  (size_t(*)(const cx_hash_t *ctx))cx_sha3_get_output_size
};

const cx_hash_info_t cx_shake128_info = {
  CX_SHAKE128,
  0,
  0,
  NULL,
  (cx_err_t (*)(cx_hash_t *ctx, const uint8_t *data, size_t len))cx_sha3_update,
  (cx_err_t (*)(cx_hash_t *ctx, uint8_t *digest))cx_sha3_final,
  (cx_err_t (*)(cx_hash_t *ctx, size_t output_size))cx_shake128_init_no_throw,
  (size_t(*)(const cx_hash_t *ctx))cx_sha3_get_output_size
};

const cx_hash_info_t cx_shake256_info = {
  CX_SHAKE256,
  0,
  0,
  NULL,
  (cx_err_t (*)(cx_hash_t *ctx, const uint8_t *data, size_t len))cx_sha3_update,
  (cx_err_t (*)(cx_hash_t *ctx, uint8_t *digest))cx_sha3_final,
  (cx_err_t (*)(cx_hash_t *ctx, size_t output_size))cx_shake256_init_no_throw,
  (size_t(*)(const cx_hash_t *ctx))cx_sha3_get_output_size
};

// Assume state is a uint64_t array
#define S64(x, y) state[x + 5 * y]
#define ROTL64(x, n) cx_rotl64(x, n)

static void cx_sha3_theta(uint64bits_t state[]) {
  uint64bits_t C[5];
  uint64bits_t D[5];
  int          i, j;

  for (i = 0; i < 5; i++) {
    C[i] = S64(i, 0) ^ S64(i, 1) ^ S64(i, 2) ^ S64(i, 3) ^ S64(i, 4);
  }
  for (i = 0; i < 5; i++) {
    D[i] = C[(i + 4) % 5] ^ ROTL64(C[(i + 1) % 5], 1);
    for (j = 0; j < 5; j++) {
      S64(i, j) ^= D[i];
    }
  }
}

static const uint8_t C_cx_pi_table[] = {10, 7,  11, 17, 18, 3, 5,  16, 8,  21, 24, 4,
                                    15, 23, 19, 13, 12, 2, 20, 14, 22, 9,  6,  1};

static const uint8_t C_cx_rho_table[] = {1,  3,  6,  10, 15, 21, 28, 36, 45, 55, 2,  14,
                                     27, 41, 56, 8,  25, 43, 62, 18, 39, 61, 20, 44};

static void cx_sha3_rho_pi(uint64bits_t state[]) {
  int          i, j;
  uint64bits_t A;
  uint64bits_t tmp;

  A = state[1];
  for (i = 0; i < 24; i++) {
    j        = C_cx_pi_table[i];
    tmp      = state[j];
    state[j] = ROTL64(A, C_cx_rho_table[i]);
    A        = tmp;
  }
}

static void cx_sha3_chi(uint64bits_t state[]) {
  uint64bits_t C[5];

  int i, j;
  for (j = 0; j < 5; j++) {
    for (i = 0; i < 5; i++) {
      C[i] = S64(i, j);
    }
    for (i = 0; i < 5; i++) {
      S64(i, j) ^= (~C[(i + 1) % 5]) & C[(i + 2) % 5];
    }
  }
}

static const uint64bits_t C_cx_iota_RC[24] = {
    _64BITS(0x00000000, 0x00000001), _64BITS(0x00000000, 0x00008082), _64BITS(0x80000000, 0x0000808A),
    _64BITS(0x80000000, 0x80008000), _64BITS(0x00000000, 0x0000808B), _64BITS(0x00000000, 0x80000001),
    _64BITS(0x80000000, 0x80008081), _64BITS(0x80000000, 0x00008009), _64BITS(0x00000000, 0x0000008A),
    _64BITS(0x00000000, 0x00000088), _64BITS(0x00000000, 0x80008009), _64BITS(0x00000000, 0x8000000A),
    _64BITS(0x00000000, 0x8000808B), _64BITS(0x80000000, 0x0000008B), _64BITS(0x80000000, 0x00008089),
    _64BITS(0x80000000, 0x00008003), _64BITS(0x80000000, 0x00008002), _64BITS(0x80000000, 0x00000080),
    _64BITS(0x00000000, 0x0000800A), _64BITS(0x80000000, 0x8000000A), _64BITS(0x80000000, 0x80008081),
    _64BITS(0x80000000, 0x00008080), _64BITS(0x00000000, 0x80000001), _64BITS(0x80000000, 0x80008008)};

static void cx_sha3_iota(uint64bits_t state[], int round) {
  S64(0, 0) ^= C_cx_iota_RC[round];
}

static bool check_hash_out_size(size_t size) {
  switch (size) {
  case 128:
  case 224:
  case 256:
  case 384:
  case 512:
    return true;
  default:
    return false;
  }
}

cx_err_t cx_sha3_init_no_throw(cx_sha3_t *hash PLENGTH(sizeof(cx_sha3_t)), size_t size) {
  if (!check_hash_out_size(size)) {
    return CX_INVALID_PARAMETER;
  }
  memset(hash, 0, sizeof(cx_sha3_t));
  hash->header.info = &cx_sha3_info;
  hash->output_size = size >> 3;
  hash->block_size  = (1600 - 2 * size) >> 3;
  return CX_OK;
}

cx_err_t cx_keccak_init_no_throw(cx_sha3_t *hash PLENGTH(sizeof(cx_sha3_t)), size_t size) {
  if (!check_hash_out_size(size)) {
    return CX_INVALID_PARAMETER;
  }
  cx_sha3_init_no_throw(hash, size);
  hash->header.info = &cx_keccak_info;
  return CX_OK;
}

cx_err_t cx_shake128_init_no_throw(cx_sha3_t *hash, size_t size) {
  memset(hash, 0, sizeof(cx_sha3_t));
  if (size % 8 != 0) {
    return CX_INVALID_PARAMETER;
  }
  hash->header.info = &cx_shake128_info;
  hash->output_size = size / 8;
  hash->block_size  = (1600 - 2 * 128) >> 3;
  return CX_OK;
}

cx_err_t cx_shake256_init_no_throw(cx_sha3_t *hash, size_t size) {
  memset(hash, 0, sizeof(cx_sha3_t));
  if (size % 8 != 0) {
    return CX_INVALID_PARAMETER;
  }
  hash->header.info = &cx_shake256_info;
  hash->output_size = size / 8;
  hash->block_size  = (1600 - 2 * 256) >> 3;
  return CX_OK;
}

cx_err_t cx_sha3_xof_init_no_throw(cx_sha3_t *hash PLENGTH(sizeof(cx_sha3_t)), size_t size, size_t out_length) {
  if ((size != 128) && (size != 256)) {
    return CX_INVALID_PARAMETER;
  }
  memset(hash, 0, sizeof(cx_sha3_t));
  if (size == 128) {
    hash->header.info = &cx_shake128_info;
  } else {
    hash->header.info = &cx_shake256_info;
  }
  hash->output_size = out_length;
  hash->block_size  = (1600 - 2 * size) >> 3;
  return CX_OK;
}

void cx_sha3_block(cx_sha3_t *hash) {
  uint64bits_t *block;
  uint64bits_t *acc;
  int           r, i, n;

  block = (uint64bits_t *)hash->block;
  acc   = (uint64bits_t *)hash->acc;

  if (hash->block_size > 144) {
    n = 21;
  } else if (hash->block_size > 136) {
    n = 18;
  } else if (hash->block_size > 104) {
    n = 17;
  } else if (hash->block_size > 72) {
    n = 13;
  } else {
    n = 9;
  }
  for (i = 0; i < n; i++) {
    acc[i] ^= block[i];
  }

  for (r = 0; r < 24; r++) {
    cx_sha3_theta(acc);
    cx_sha3_rho_pi(acc);
    cx_sha3_chi(acc);
    cx_sha3_iota(acc, r);
  }
}

cx_err_t cx_sha3_update(cx_sha3_t *ctx, const uint8_t *data, size_t len) {
  size_t   r;
  size_t   block_size;
  uint8_t *block;
  size_t   blen;

  if (ctx == NULL) {
    return CX_INVALID_PARAMETER;
  }
  if (data == NULL) {
    return len == 0 ? CX_OK : CX_INVALID_PARAMETER;
  }

  block_size = ctx->block_size;
  if (block_size > 200) {
    return CX_INVALID_PARAMETER;
  }

  block     = ctx->block;
  blen      = ctx->blen;
  ctx->blen = 0;

  if (blen >= block_size) {
    return CX_INVALID_PARAMETER;
  }

  // --- append input data and process all blocks ---
  if ((blen + len) >= block_size) {
    r = block_size - blen;
    do {
      if (ctx->header.counter == CX_HASH_MAX_BLOCK_COUNT) {
        return INVALID_PARAMETER;
      }
      memcpy(block + blen, data, r);
      cx_sha3_block(ctx);

      blen = 0;
      ctx->header.counter++;
      data += r;
      len -= r;
      r = block_size;
    } while (len >= block_size);
  }

  // --- remind rest data---
  memcpy(block + blen, data, len);
  blen += len;
  ctx->blen = blen;
  return CX_OK;
}

cx_err_t cx_sha3_final(cx_sha3_t *hash, uint8_t *digest) {
  size_t   block_size;
  uint8_t *block;
  size_t   blen;
  size_t   len;

  block      = hash->block;
  block_size = hash->block_size;
  blen       = hash->blen;

  // one more block?
  if (hash->header.info->md_type == CX_KECCAK || hash->header.info->md_type == CX_SHA3) {
    // last block!
    memset(block + blen, 0, (200 - blen));

    if (hash->header.info->md_type == CX_KECCAK) {
      block[blen] |= 01;
    } else {
      block[blen] |= 06;
    }
    block[block_size - 1] |= 0x80;
    cx_sha3_block(hash);

    // provide result
    len = (hash)->output_size;
    memcpy(digest, hash->acc, len);
  } else {
    // CX_SHA3_XOF
    memset(block + blen, 0, (200 - blen));
    block[blen] |= 0x1F;
    block[block_size - 1] |= 0x80;
    cx_sha3_block(hash);
    // provide result
    len  = hash->output_size;
    blen = len;

    memset(block, 0, 200);

    while (blen > block_size) {
      memcpy(digest, hash->acc, block_size);
      blen -= block_size;
      digest += block_size;
      cx_sha3_block(hash);
    }
    memcpy(digest, hash->acc, blen);
  }
  return CX_OK;
}

size_t cx_sha3_get_output_size(const cx_sha3_t *ctx) {
  return ctx->output_size;
}

#endif // HAVE_SHA3
