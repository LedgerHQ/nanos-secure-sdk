
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

#if defined(HAVE_SHA512) || defined(HAVE_SHA384)

#include "cx_ram.h"
#include "cx_sha512.h"
#include "cx_utils.h"

#include <string.h>

#ifdef HAVE_SHA384
const cx_hash_info_t cx_sha384_info = {
  CX_SHA384,
  CX_SHA384_SIZE,
  SHA512_BLOCK_SIZE,
  (cx_err_t (*)(cx_hash_t *ctx))cx_sha384_init_no_throw,
  (cx_err_t (*)(cx_hash_t *ctx, const uint8_t *data, size_t len))cx_sha512_update,
  (cx_err_t (*)(cx_hash_t *ctx, uint8_t *digest))cx_sha512_final,
  NULL,
  NULL
};
#endif // HAVE_SHA384

#ifdef HAVE_SHA512
const cx_hash_info_t cx_sha512_info = {
  CX_SHA512,
  CX_SHA512_SIZE,
  SHA512_BLOCK_SIZE,
  (cx_err_t (*)(cx_hash_t *ctx))cx_sha512_init_no_throw,
  (cx_err_t (*)(cx_hash_t *ctx, const uint8_t *data, size_t len))cx_sha512_update,
  (cx_err_t (*)(cx_hash_t *ctx, uint8_t *digest))cx_sha512_final,
  NULL,
  NULL
};
#endif // HAVE_SHA512

#ifndef HAVE_SHA512_WITH_INIT_ALT_METHOD

#if defined(HAVE_SHA384)
static const uint64bits_t hzero_384[] = {_64BITS(0xcbbb9d5d, 0xc1059ed8), _64BITS(0x629a292a, 0x367cd507),
                                         _64BITS(0x9159015a, 0x3070dd17), _64BITS(0x152fecd8, 0xf70e5939),
                                         _64BITS(0x67332667, 0xffc00b31), _64BITS(0x8eb44a87, 0x68581511),
                                         _64BITS(0xdb0c2e0d, 0x64f98fa7), _64BITS(0x47b5481d, 0xbefa4fa4)};
#endif
#if defined(HAVE_SHA512)
static const uint64bits_t hzero[] = {_64BITS(0x6a09e667, 0xf3bcc908), _64BITS(0xbb67ae85, 0x84caa73b),
                                     _64BITS(0x3c6ef372, 0xfe94f82b), _64BITS(0xa54ff53a, 0x5f1d36f1),
                                     _64BITS(0x510e527f, 0xade682d1), _64BITS(0x9b05688c, 0x2b3e6c1f),
                                     _64BITS(0x1f83d9ab, 0xfb41bd6b), _64BITS(0x5be0cd19, 0x137e2179)};
#endif

#if defined(HAVE_SHA384)
cx_err_t cx_sha384_init_no_throw(cx_sha512_t *hash) {
  memset(hash, 0, sizeof(cx_sha512_t));
  hash->header.info = &cx_sha384_info;
  memmove(hash->acc, hzero_384, sizeof(hzero_384));
  return CX_OK;
}
#endif

#if defined(HAVE_SHA512)
cx_err_t cx_sha512_init_no_throw(cx_sha512_t *hash) {
  memset(hash, 0, sizeof(cx_sha512_t));
  hash->header.info = &cx_sha512_info;
  memmove(hash->acc, hzero, sizeof(hzero));
  return CX_OK;
}
#endif
#endif // HAVE_SHA512_WITH_INIT_ALT_METHOD


#ifndef HAVE_SHA512_WITH_BLOCK_ALT_METHOD

#ifndef NATIVE_64BITS // NO 64BITS natively supported by the compiler

#define rotR64(x, n) cx_rotr64(&x, n)
#define shR64(x, n) cx_shr64(&x, n)

static void sig512(uint64bits_t *x, uint8_t a, uint8_t b, uint8_t c) {
  uint64bits_t x1, x2, x3;

  ASSIGN64(x1, *x);
  ASSIGN64(x2, *x);
  ASSIGN64(x3, *x);
  rotR64(x1, a);
  rotR64(x2, b);
  shR64(x3, c);
  x->l = x1.l ^ x2.l ^ x3.l;
  x->h = x1.h ^ x2.h ^ x3.h;
}

static void sum512(uint64bits_t *x, uint8_t a, uint8_t b, uint8_t c) {
  uint64bits_t x1, x2, x3;

  ASSIGN64(x1, *x);
  ASSIGN64(x2, *x);
  ASSIGN64(x3, *x);
  rotR64((x1), a);
  rotR64((x2), b);
  rotR64((x3), c);
  x->l = x1.l ^ x2.l ^ x3.l;
  x->h = x1.h ^ x2.h ^ x3.h;
}
#define sigma0(x) sig512(&x, 1, 8, 7)
#define sigma1(x) sig512(&x, 19, 61, 6)
#define sum0(x) sum512(&x, 28, 34, 39)
#define sum1(x) sum512(&x, 14, 18, 41)

// ( ((x) & (y)) ^ (~(x) & (z)) )
#define ch(r, x, y, z)                                                                                                 \
  r.l = ((x.l) & (y.l)) ^ (~(x.l) & (z.l));                                                                            \
  r.h = ((x.h) & (y.h)) ^ (~(x.h) & (z.h))

//( ((x) & (y)) ^ ( (x) & (z)) ^ ((y) & (z)) )
#define maj(r, x, y, z)                                                                                                \
  r.l = ((x.l) & (y.l)) ^ ((x.l) & (z.l)) ^ ((y.l) & (z.l));                                                           \
  r.h = ((x.h) & (y.h)) ^ ((x.h) & (z.h)) ^ ((y.h) & (z.h))

#else // NATIVE_64BITS

#define rotR64(x, n) cx_rotr64(x, n)
#define shR64(x, n) cx_shr64(x, n)

#define sig512(x, a, b, c) (rotR64((x), a) ^ rotR64((x), b) ^ shR64((x), c))

#define sum512(x, a, b, c) (rotR64((x), a) ^ rotR64((x), b) ^ rotR64((x), c))

#define sigma0(x) sig512(x, 1, 8, 7)
#define sigma1(x) sig512(x, 19, 61, 6)
#define sum0(x) sum512(x, 28, 34, 39)
#define sum1(x) sum512(x, 14, 18, 41)

#define ch(x, y, z) (((x) & (y)) ^ (~(x) & (z)))
#define maj(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))

#endif // NATIVE_64BITS?

static const uint64bits_t primeSqrt[] = {

    _64BITS(0x428a2f98, 0xd728ae22), _64BITS(0x71374491, 0x23ef65cd), _64BITS(0xb5c0fbcf, 0xec4d3b2f),
    _64BITS(0xe9b5dba5, 0x8189dbbc), _64BITS(0x3956c25b, 0xf348b538), _64BITS(0x59f111f1, 0xb605d019),
    _64BITS(0x923f82a4, 0xaf194f9b), _64BITS(0xab1c5ed5, 0xda6d8118), _64BITS(0xd807aa98, 0xa3030242),
    _64BITS(0x12835b01, 0x45706fbe), _64BITS(0x243185be, 0x4ee4b28c), _64BITS(0x550c7dc3, 0xd5ffb4e2),
    _64BITS(0x72be5d74, 0xf27b896f), _64BITS(0x80deb1fe, 0x3b1696b1), _64BITS(0x9bdc06a7, 0x25c71235),
    _64BITS(0xc19bf174, 0xcf692694), _64BITS(0xe49b69c1, 0x9ef14ad2), _64BITS(0xefbe4786, 0x384f25e3),
    _64BITS(0x0fc19dc6, 0x8b8cd5b5), _64BITS(0x240ca1cc, 0x77ac9c65), _64BITS(0x2de92c6f, 0x592b0275),
    _64BITS(0x4a7484aa, 0x6ea6e483), _64BITS(0x5cb0a9dc, 0xbd41fbd4), _64BITS(0x76f988da, 0x831153b5),
    _64BITS(0x983e5152, 0xee66dfab), _64BITS(0xa831c66d, 0x2db43210), _64BITS(0xb00327c8, 0x98fb213f),
    _64BITS(0xbf597fc7, 0xbeef0ee4), _64BITS(0xc6e00bf3, 0x3da88fc2), _64BITS(0xd5a79147, 0x930aa725),
    _64BITS(0x06ca6351, 0xe003826f), _64BITS(0x14292967, 0x0a0e6e70), _64BITS(0x27b70a85, 0x46d22ffc),
    _64BITS(0x2e1b2138, 0x5c26c926), _64BITS(0x4d2c6dfc, 0x5ac42aed), _64BITS(0x53380d13, 0x9d95b3df),
    _64BITS(0x650a7354, 0x8baf63de), _64BITS(0x766a0abb, 0x3c77b2a8), _64BITS(0x81c2c92e, 0x47edaee6),
    _64BITS(0x92722c85, 0x1482353b), _64BITS(0xa2bfe8a1, 0x4cf10364), _64BITS(0xa81a664b, 0xbc423001),
    _64BITS(0xc24b8b70, 0xd0f89791), _64BITS(0xc76c51a3, 0x0654be30), _64BITS(0xd192e819, 0xd6ef5218),
    _64BITS(0xd6990624, 0x5565a910), _64BITS(0xf40e3585, 0x5771202a), _64BITS(0x106aa070, 0x32bbd1b8),
    _64BITS(0x19a4c116, 0xb8d2d0c8), _64BITS(0x1e376c08, 0x5141ab53), _64BITS(0x2748774c, 0xdf8eeb99),
    _64BITS(0x34b0bcb5, 0xe19b48a8), _64BITS(0x391c0cb3, 0xc5c95a63), _64BITS(0x4ed8aa4a, 0xe3418acb),
    _64BITS(0x5b9cca4f, 0x7763e373), _64BITS(0x682e6ff3, 0xd6b2b8a3), _64BITS(0x748f82ee, 0x5defb2fc),
    _64BITS(0x78a5636f, 0x43172f60), _64BITS(0x84c87814, 0xa1f0ab72), _64BITS(0x8cc70208, 0x1a6439ec),
    _64BITS(0x90befffa, 0x23631e28), _64BITS(0xa4506ceb, 0xde82bde9), _64BITS(0xbef9a3f7, 0xb2c67915),
    _64BITS(0xc67178f2, 0xe372532b), _64BITS(0xca273ece, 0xea26619c), _64BITS(0xd186b8c7, 0x21c0c207),
    _64BITS(0xeada7dd6, 0xcde0eb1e), _64BITS(0xf57d4f7f, 0xee6ed178), _64BITS(0x06f067aa, 0x72176fba),
    _64BITS(0x0a637dc5, 0xa2c898a6), _64BITS(0x113f9804, 0xbef90dae), _64BITS(0x1b710b35, 0x131c471b),
    _64BITS(0x28db77f5, 0x23047d84), _64BITS(0x32caab7b, 0x40c72493), _64BITS(0x3c9ebe0a, 0x15c9bebc),
    _64BITS(0x431d67c4, 0x9c100d4c), _64BITS(0x4cc5d4be, 0xcb3e42b6), _64BITS(0x597f299c, 0xfc657e2a),
    _64BITS(0x5fcb6fab, 0x3ad6faec), _64BITS(0x6c44198c, 0x4a475817)};

void cx_sha512_block(cx_sha512_t *hash) {
  uint8_t      j;
  uint64bits_t t1, t2;

  uint64bits_t *accumulator;
  uint64bits_t *X;
  struct {
    uint64bits_t a, b, c, d, e, f, g, h;
  } ACC;
#define A ACC.a
#define B ACC.b
#define C ACC.c
#define D ACC.d
#define E ACC.e
#define F ACC.f
#define G ACC.g
#define H ACC.h

  X           = (uint64bits_t *)(&hash->block[0]);
  accumulator = (uint64bits_t *)(&hash->acc[0]); // only work because of indexing zero!
  A           = accumulator[0];
  B           = accumulator[1];
  C           = accumulator[2];
  D           = accumulator[3];
  E           = accumulator[4];
  F           = accumulator[5];
  G           = accumulator[6];
  H           = accumulator[7];

#ifdef ARCH_LITTLE_ENDIAN
  cx_swap_buffer64(X, 16);
#endif

  // init
  /*
   * T1 = Sum_1_512(e) + Chg(e,f,g) + K_t_512 + Wt
   * T2 = Sum_0_512(a) + Maj(abc)
   * h = g ;
   * g = f;
   * f = e;
   * e = d + T1;
   * d = c;
   * c = b;
   * b = a;
   * a = T1 + T2;
   */
  for (j = 0; j < 80; j++) {
#ifndef NATIVE_64BITS
    uint64bits_t r;

    /* for j in 16 to 63, Xj <- (Sigma_1_512( Xj-2) + Xj-7 + Sigma_0_512(Xj-15)
     * + Xj-16 ). */
    if (j >= 16) {
      // sigma1(X[(j-2)  & 0xF])
      ASSIGN64(t2, X[(j - 2) & 0xF]);
      sigma1(t2);
      //+ X[(j-7)  & 0xF]
      ADD64(t2, X[(j - 7) & 0xF]);
      //+ sigma0(X[(j-15) & 0xF]
      ASSIGN64(t1, X[(j - 15) & 0xF]);
      sigma0(t1);
      ADD64(t2, t1);
      //+ X[(j-16) & 0xF])
      ADD64(t2, X[(j - 16) & 0xF]);
      // assign
      ASSIGN64(X[j & 0xF], t2);
    }

    /// t1 =  H + sum1(E) + ch(E,F,G) + primeSqrt[j] + X[j&0xF];
    // H
    ASSIGN64(t1, H);
    //+sum1(E)
    ASSIGN64(r, E);
    sum1(r);
    ADD64(t1, r);
    //+ch(E,F,G)
    ch(r, E, F, G);
    ADD64(t1, r);
    //+primeSqrt[j]
    ASSIGN64(r, primeSqrt[j]);
    ADD64(t1, r); // ADD64(t1,primeSqrt[j]);
    //+X[j&0xF]
    ADD64(t1, X[j & 0xF]);

    /// t2 = sum0(A) + maj(A,B,C);
    // sum0(A)
    ASSIGN64(t2, A);
    sum0(t2);
    //+maj(A,B,C);
    maj(r, A, B, C);
    ADD64(t2, r);

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
    memmove(&ACC[1], &ACC[0], sizeof(ACC) - sizeof(uint64bits_t));
    // E += t1;
    ADD64(E, t1);
    // A  = t1+t2;
    ASSIGN64(A, t1);
    ADD64(A, t2);
#else // no NATIVE_64BITS
    /* for j in 16 to 63, Xj <- (Sigma_1_512( Xj-2) + Xj-7 + Sigma_0_512(Xj-15)
     * + Xj-16 ). */
    if (j >= 16) {
      uint64bits_t xj2, xj15;
      xj2        = X[(j - 2) & 0xF];
      xj15       = X[(j - 15) & 0xF];
      X[j & 0xF] = (sigma1(xj2) + X[(j - 7) & 0xF] + sigma0(xj15) + X[(j - 16) & 0xF]);
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
    H = G;
    G = F;
    F = E;
    E = D;
    D = C;
    C = B;
    B = A;
    E += t1;
    A = t1 + t2;

#endif // NATIVE_64BITS?
  }

  //(update chaining values) (H1 , H2 , H3 , H4 ) <- (H1 + A, H2 + B, H3 + C, H4
  //+ D...)

#ifndef NATIVE_64BITS
  ADD64(accumulator[0], A);
  ADD64(accumulator[1], B);
  ADD64(accumulator[2], C);
  ADD64(accumulator[3], D);
  ADD64(accumulator[4], E);
  ADD64(accumulator[5], F);
  ADD64(accumulator[6], G);
  ADD64(accumulator[7], H);
#else
  accumulator[0] += A;
  accumulator[1] += B;
  accumulator[2] += C;
  accumulator[3] += D;
  accumulator[4] += E;
  accumulator[5] += F;
  accumulator[6] += G;
  accumulator[7] += H;

#endif
}
#else
void cx_sha512_block(cx_sha512_t *ctx);
#endif //! HAVE_SHA512_WITH_BLOCK_ALT_METHOD

cx_err_t cx_sha512_update(cx_sha512_t *ctx, const uint8_t *data, size_t len) {
  size_t   r;
  uint8_t  block_size;
  uint8_t *block;
  size_t   blen;

  if (ctx == NULL) {
    return CX_INVALID_PARAMETER;
  }

  if (data == NULL) {
    if (len == 0) {
      return CX_OK;
    } else {
      return CX_INVALID_PARAMETER;
    }
  }

  block_size = 128;
  block      = ctx->block;
  blen       = ctx->blen;
  ctx->blen  = 0;

  // --- append input data and process all blocks ---
  if ((blen + len) >= block_size) {
    r = block_size - blen;
    do {
      if (ctx->header.counter == CX_HASH_MAX_BLOCK_COUNT) {
        return CX_INVALID_PARAMETER;
      }
      memcpy(block + blen, data, r);
      cx_sha512_block(ctx);
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

cx_err_t cx_sha512_final(cx_sha512_t *ctx, uint8_t *digest) {
  uint64_t bitlen;
  size_t   len;

  uint8_t *    block = ctx->block;
  unsigned int blen  = ctx->blen;
  uint8_t *    acc   = ctx->acc;

  // one more block?
  block[blen] = 0x80;
  blen++;

  bitlen = (((uint64_t)ctx->header.counter) * 128UL + (uint64_t)blen - 1UL) * 8UL;
  // one more block?
  if ((128 - blen) < 16) {
    memset(block + blen, 0, 128 - blen);
    cx_sha512_block(ctx);
    blen = 0;
  }
  // last block!
  memset(block + blen, 0, 128 - blen);
#ifdef ARCH_LITTLE_ENDIAN
  (*(uint64_t *)(&block[128 - 8])) = cx_swap_uint64(bitlen);
#else
  (*(uint64_t *)(&block[128 - 8])) = bitlen;
#endif
  cx_sha512_block(ctx);
  // provide result
  len = (ctx->header.info->md_type == CX_SHA512) ? 512 >> 3 : 384 >> 3;
#ifdef ARCH_LITTLE_ENDIAN
  cx_swap_buffer64((uint64bits_t *)acc, 8);
#endif
  memcpy(digest, acc, len);
  return CX_OK;
}

#if defined(HAVE_SHA512)
size_t cx_hash_sha512(const uint8_t *in, size_t in_len, uint8_t *out, size_t out_len) {
  if (out_len < CX_SHA512_SIZE) {
    return 0;
  }
  cx_sha512_init_no_throw(&G_cx.sha512);
  cx_sha512_update(&G_cx.sha512, in, in_len);
  cx_sha512_final(&G_cx.sha512, out);
  explicit_bzero(&G_cx.sha512, sizeof(cx_sha512_t));
  return CX_SHA512_SIZE;
}
#endif

#endif // defined (HAVE_SHA512) || defined(HAVE_SHA384)
