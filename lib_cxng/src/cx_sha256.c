
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

#if defined(HAVE_SHA256) || defined(HAVE_SHA224)

#include "cx_sha256.h"

#include "cx_ram.h"
#include "cx_utils.h"
#include <stdint.h>
#include <string.h>

#ifdef HAVE_SHA224
const cx_hash_info_t cx_sha224_info = {
  CX_SHA224,
  CX_SHA224_SIZE,
  SHA256_BLOCK_SIZE,
  (cx_err_t(*)(cx_hash_t *ctx))cx_sha224_init_no_throw,
  (cx_err_t (*)(cx_hash_t *ctx, const uint8_t *data, size_t len))cx_sha256_update,
  (cx_err_t (*)(cx_hash_t *ctx, uint8_t *digest))cx_sha256_final,
  NULL,
  NULL
};
#endif // HAVE_SHA224

#ifdef HAVE_SHA256
const cx_hash_info_t cx_sha256_info = {
  CX_SHA256,
  CX_SHA256_SIZE,
  SHA256_BLOCK_SIZE,
  (cx_err_t (*)(cx_hash_t *ctx))cx_sha256_init_no_throw,
  (cx_err_t (*)(cx_hash_t *ctx, const uint8_t *data, size_t len))cx_sha256_update,
  (cx_err_t (*)(cx_hash_t *ctx, uint8_t *digest))cx_sha256_final,
  NULL,
  NULL
};
#endif // HAVE_SHA256

static const uint32_t primeSqrt[] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2,
};

#if defined(HAVE_SHA224)
static const uint32_t hzero_224[] = {0xc1059ed8, 0x367cd507, 0x3070dd17, 0xf70e5939,
                                     0xffc00b31, 0x68581511, 0x64f98fa7, 0xbefa4fa4};
#endif

static const uint32_t hzero[] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
                                 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};


#define sig256(x, a, b, c) (cx_rotr((x), a) ^ cx_rotr((x), b) ^ cx_shr((x), c))
#define sum256(x, a, b, c) (cx_rotr((x), a) ^ cx_rotr((x), b) ^ cx_rotr((x), c))

#define sigma0(x) sig256(x, 7, 18, 3)
#define sigma1(x) sig256(x, 17, 19, 10)
#define sum0(x) sum256(x, 2, 13, 22)
#define sum1(x) sum256(x, 6, 11, 25)

// #define ch(x, y, z) (((x) & (y)) ^ (~(x) & (z)))
// #define maj(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))

#define ch(x, y, z) (z ^ (x & (y ^ z)))
#define maj(x, y, z) ((x & y) | (z & (x | y)))

#if defined(HAVE_SHA224)
cx_err_t cx_sha224_init_no_throw(cx_sha256_t *hash) {
  memset(hash, 0, sizeof(cx_sha256_t));
  hash->header.info = &cx_sha224_info;
  memmove(hash->acc, hzero_224, sizeof(hzero));
  return CX_OK;
}
#endif

cx_err_t cx_sha256_init_no_throw(cx_sha256_t *hash) {
  memset(hash, 0, sizeof(cx_sha256_t));
  hash->header.info = &cx_sha256_info;
  memmove(hash->acc, hzero, sizeof(hzero));
  return CX_OK;
}

static void cx_sha256_block(cx_sha256_t *hash) {
  uint32_t t1, t2;

  uint32_t  ACC[8];
  uint32_t *accumulator;
  uint32_t *X;

#define A ACC[0]
#define B ACC[1]
#define C ACC[2]
#define D ACC[3]
#define E ACC[4]
#define F ACC[5]
#define G ACC[6]
#define H ACC[7]

  // init
  X           = ((uint32_t *)&hash->block[0]);
  accumulator = (uint32_t *)&hash->acc[0];
  memmove(ACC, accumulator, sizeof(ACC));

#ifdef ARCH_LITTLE_ENDIAN
  cx_swap_buffer32(X, 16);
#endif

  /*
   * T1 = Sum_1_256(e) + Chg(e,f,g) + K_t_256 + Wt
   * T2 = Sum_0_256(a) + Maj(abc)
   * h = g ;
   * g = f;
   * f = e;
   * e = d + T1;
   * d = c;
   * c = b;
   * b = a;
   * a = T1 + T2;
   */
  for (int j = 0; j < 64; j++) {
    /* for j in 16 to 63, Xj <- (Sigma_1_256( Xj-2) + Xj-7 + Sigma_0_256(Xj-15)
     * + Xj-16 ). */
    if (j >= 16) {
      X[j & 0xF] = (sigma1(X[(j - 2) & 0xF]) + X[(j - 7) & 0xF] + sigma0(X[(j - 15) & 0xF]) + X[(j - 16) & 0xF]);
    }

    t1 = H + sum1(E) + ch(E, F, G) + primeSqrt[j] + X[j & 0xF];
    t2 = sum0(A) + maj(A, B, C);
    /*
    H = G ;
    G = F;
    F = E;
    E = D+t1;
    D = C;
    C = B;
    B = A;
    A = t1+t2;
    */
    memmove(&ACC[1], &ACC[0], sizeof(ACC) - sizeof(uint32_t));
    E += t1;
    A = t1 + t2;
  }

  //(update chaining values) (H1 , H2 , H3 , H4 ) <- (H1 + A, H2 + B, H3 + C, H4
  //+ D...)
  accumulator[0] += A;
  accumulator[1] += B;
  accumulator[2] += C;
  accumulator[3] += D;
  accumulator[4] += E;
  accumulator[5] += F;
  accumulator[6] += G;
  accumulator[7] += H;
}

cx_err_t cx_sha256_update(cx_sha256_t *ctx, const uint8_t *data, size_t len) {
  size_t r;
  size_t blen;

  if (ctx == NULL) {
    return CX_INVALID_PARAMETER;
  }
  if (data == NULL && len != 0) {
    return CX_INVALID_PARAMETER;
  }

  // --- init locals ---
  blen      = ctx->blen;
  ctx->blen = 0;

  if (blen >= 64) {
    return CX_INVALID_PARAMETER;
  }

  // --- append input data and process all blocks ---
  if (blen + len >= 64) {
    r = 64 - blen;
    do {
      // if (ctx->header.counter == CX_HASH_MAX_BLOCK_COUNT) {
      //   return CX_INVALID_PARAMETER;
      // }
      memcpy(ctx->block + blen, data, r);
      cx_sha256_block(ctx);

      blen = 0;
      ctx->header.counter++;
      data += r;
      len -= r;
      r = 64;
    } while (len >= 64);
  }

  // --- remind rest data---
  memcpy(ctx->block + blen, data, len);
  blen += len;
  ctx->blen = blen;
  return CX_OK;
}

cx_err_t cx_sha256_final(cx_sha256_t *ctx, uint8_t *digest) {
  uint64_t bitlen;

  // --- init locals ---
  uint8_t *block = ctx->block;

  block[ctx->blen] = 0x80;
  ctx->blen++;

  bitlen = (((uint64_t)ctx->header.counter) * 64UL + (uint64_t)ctx->blen - 1UL) * 8UL;
  // one more block?
  if (64 - ctx->blen < 8) {
    memset(block + ctx->blen, 0, 64 - ctx->blen);
    cx_sha256_block(ctx);
    ctx->blen = 0;
  }
  // last block!
  memset(block + ctx->blen, 0, 64 - ctx->blen);
#ifdef ARCH_LITTLE_ENDIAN
  *(uint64_t *)&block[64 - 8] = cx_swap_uint64(bitlen);
#else
  (*(uint64_t *)&block[64 - 8]) = bitlen;
#endif
  cx_sha256_block(ctx);
  // provide result
#ifdef ARCH_LITTLE_ENDIAN
  cx_swap_buffer32((uint32_t *)ctx->acc, 8);
#endif

#if defined(HAVE_SHA224)
  if (ctx->header.info->md_type == CX_SHA224) {
    memcpy(digest, ctx->acc, CX_SHA224_SIZE);
  } else
#endif
  {
    memcpy(digest, ctx->acc, CX_SHA256_SIZE);
  }
  return CX_OK;
}

size_t cx_hash_sha256(const uint8_t *in, size_t in_len, uint8_t *out, size_t out_len) {
  if (out_len < CX_SHA256_SIZE) {
    return 0;
  }
  cx_sha256_init_no_throw(&G_cx.sha256);
  cx_sha256_update(&G_cx.sha256, in, in_len);
  cx_sha256_final(&G_cx.sha256, out);
  explicit_bzero(&G_cx.sha256, sizeof(cx_sha256_t));
  return CX_SHA256_SIZE;
}

#endif // defined(HAVE_SHA255) || defined(HAVE_SHA224)
