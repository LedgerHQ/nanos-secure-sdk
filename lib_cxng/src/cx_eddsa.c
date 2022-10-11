
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

#ifdef HAVE_ECC_TWISTED_EDWARDS

#include "cx_hash.h"
#include "cx_ecfp.h"
#include "cx_eddsa.h"
#include "cx_selftests.h"
#include "cx_utils.h"
#include "cx_ram.h"

#include <string.h>

static void cx_encode_int(uint8_t *v, int len) {
  uint8_t t;
  int     i, j;
  i   = 0;
  j   = len - 1;
  len = len / 2;
  while (len--) {
    t    = v[i];
    v[i] = v[j];
    v[j] = t;
    i++;
    j--;
  }
}

#define cx_decode_int(v, l) cx_encode_int(v, l)


int cx_decode_coord(uint8_t *coord, int len){
  int sign;
  cx_encode_int(coord, len); // little endian
  sign = coord[0] & 0x80; // x_0
  coord[0] &= 0x7F; // y-coordinate
  return sign;
}

void cx_encode_coord(uint8_t *coord, int len, int sign){
  coord[0] |= sign? 0x80:0x00;
  cx_encode_int(coord, len);
}

/* ----------------------------------------------------------------------- */
/*                                                                         */
/* ----------------------------------------------------------------------- */
cx_err_t cx_eddsa_get_public_key_internal(const cx_ecfp_private_key_t *pv_key,
                                          cx_md_t                      hashID,
                                          cx_ecfp_public_key_t *       pu_key,
                                          uint8_t *                    a,
                                          size_t                       a_len,
                                          uint8_t *                    h,
                                          size_t                       h_len,
                                          uint8_t *                    scal /*tmp*/) {
  size_t   size;
  cx_err_t error;

  CX_CHECK(cx_ecdomain_parameters_length(pv_key->curve, &size));

  if (!CX_CURVE_RANGE(pv_key->curve, TWISTED_EDWARDS)) {
    return CX_INVALID_PARAMETER;
  }
  if (!((pv_key->d_len == size) || (pv_key->d_len == (2 * size)))) {
    return CX_INVALID_PARAMETER;
  }
  if (!(((a == NULL) && (a_len == 0)) || ((a_len) && (a_len >= size)))) {
    return CX_INVALID_PARAMETER;
  }
  if (!(((h == NULL) && (h_len == 0)) || ((h_len) && (h_len >= size)))) {
    return CX_INVALID_PARAMETER;
  }

  switch (hashID) {
#if defined(HAVE_SHA512)
  case CX_SHA512:
#endif // HAVE_SHA512

#if defined(HAVE_SHA3)
  case CX_SHAKE256:
  case CX_KECCAK:
  case CX_SHA3:
#endif // HAVE_SHA3

#if defined(HAVE_BLAKE2)
  case CX_BLAKE2B:
    if (cx_hash_init_ex(&G_cx.hash_ctx, hashID, size * 2) != CX_OK) {
      return CX_INVALID_PARAMETER;
    }
    break;
#endif // HAVE_BLAKE2

  default:
    return CX_INVALID_PARAMETER;
  }

  if (pv_key->d_len == size) {
    /* 1. Hash the 32/57-byte private key using SHA-512/shak256-114, storing the digest in
     * a 32/114 bytes large buffer, denoted h.  Only the lower [CME: first] 32/57 bytes are
     * used for generating the public key.
     */
    cx_hash_update(&G_cx.hash_ctx, pv_key->d, pv_key->d_len);
    cx_hash_final(&G_cx.hash_ctx, scal);
    if (pv_key->curve == CX_CURVE_Ed25519) {
      /* 2. Prune the buffer: The lowest 3 bits of the first octet are
       * cleared, the highest bit of the last octet is cleared, and the
       * second highest bit of the last octet is set.
       */
      scal[0] &= 0xF8;
      scal[31] = (scal[31] & 0x7F) | 0x40;
    } else /* CX_CURVE_Ed448 */ {
      /* 2. Prune the buffer: The two least significant bits of the first
       * octet are cleared, all eight bits the last octet are cleared, and
       * the highest bit of the second to last octet is set.
       */
      scal[0] &= 0xFC;
      scal[56] = 0;
      scal[55] |= 0x80;
    }
  } else {
    memmove(scal, pv_key->d, pv_key->d_len);
  }

  /* 3. Interpret the buffer as the little-endian integer, forming a
   * secret scalar a.  Perform a fixed-base scalar multiplication
   * [a]B.
   */
  cx_decode_int(scal, size);
  if (a) {
    memmove(a, scal, size);
  }
  if (h) {
    memmove(h, scal + size, size);
  }
  if (pu_key) {
    cx_ecpoint_t W;
    CX_CHECK(cx_ecpoint_alloc(&W, pv_key->curve));
    CX_CHECK(cx_ecdomain_generator_bn(pv_key->curve, &W));
    CX_CHECK(cx_ecpoint_rnd_scalarmul(&W, scal, size));
    pu_key->curve = pv_key->curve;
    pu_key->W_len = 1 + 2 * size;
    pu_key->W[0]  = 0x04;
    CX_CHECK(cx_ecpoint_export(&W, pu_key->W + 1, size, pu_key->W + 1 + size, size));
    CX_CHECK(cx_ecpoint_destroy(&W));
  }

 end:
  return error;
}

cx_err_t cx_eddsa_get_public_key_no_throw(const cx_ecfp_private_key_t *pv_key,
                                 cx_md_t                      hashID,
                                 cx_ecfp_public_key_t *       pu_key,
                                 uint8_t *                    a,
                                 size_t                       a_len,
                                 uint8_t *                    h,
                                 size_t                       h_len) {
  uint8_t scal[114];
  size_t size;
  cx_err_t error;

  CX_CHECK(cx_ecdomain_parameters_length(pv_key->curve, &size));
  CX_CHECK(cx_bn_lock(size, 0));
  CX_CHECK(cx_eddsa_get_public_key_internal(pv_key, hashID, pu_key, a, a_len, h, h_len, scal));

 end:
  cx_bn_unlock();
  return error;
}

#ifdef HAVE_EDDSA

static uint8_t const C_cx_siged448[] = {'S', 'i', 'g', 'E', 'd', '4', '4', '8'};

/* ----------------------------------------------------------------------- */
/*                                                                         */
/* ----------------------------------------------------------------------- */
cx_err_t cx_eddsa_sign_no_throw(const cx_ecfp_private_key_t *pv_key,
                       cx_md_t                      hashID,
                       const uint8_t *              hash,
                       size_t                       hash_len,
                       uint8_t *                    sig,
                       size_t                       sig_len) {
  size_t                     size, hsize;
  cx_bn_t                    bn_h, bn_a, bn_r, bn_s, bn_n;
  cx_ecpoint_t               Q;
  cx_err_t                   error;

  uint32_t                   sign;
  uint8_t a[57]; // a
  uint8_t r[57]; // r
  uint8_t scal[114];

  CX_CHECK(cx_ecdomain_parameters_length(pv_key->curve, &size));

  if (!CX_CURVE_RANGE(pv_key->curve, TWISTED_EDWARDS)) {
    return CX_INVALID_PARAMETER;
  }
  if (!((pv_key->d_len == size) || (pv_key->d_len == 2 * size))) {
    return CX_INVALID_PARAMETER;
  }
  if (sig_len < 2 * size) {
    return CX_INVALID_PARAMETER;
  }

  // check hashID as H function
  switch (hashID) {
#if (defined(HAVE_SHA512) || defined(HAVE_SHA3))
# if defined(HAVE_SHA512)
  case CX_SHA512:
# endif // HAVE_SHA512

# if defined(HAVE_SHA3)
  case CX_KECCAK:
  case CX_SHA3:
# endif // HAVE_SHA3
    if (size * 2 != 512 / 8) {
      return INVALID_PARAMETER;
    }
    break;
#endif // (defined(HAVE_SHA512) || defined(HAVE_SHA3))

#if (defined(HAVE_SHA3) || defined(HAVE_BLAKE2))
# if defined(HAVE_SHA3)
  case CX_SHAKE256:
# endif // HAVE_SHA3

# if defined(HAVE_BLAKE2)
  case CX_BLAKE2B:
# endif // HAVE_BLAKE2
    break;
#endif // (defined(HAVE_SHA3) || defined(HAVE_BLAKE2))

  default:
    return INVALID_PARAMETER;
  }
  hsize = 2 * size;

  CX_CHECK(cx_bn_lock(size, 0));
  CX_CHECK(cx_ecpoint_alloc(&Q, pv_key->curve));
  CX_CHECK(cx_bn_alloc(&bn_n, size));
  CX_CHECK(cx_ecdomain_parameter_bn(pv_key->curve, CX_CURVE_PARAM_Order, bn_n));

  // retrieve private scalar a, and private prefix h (stored in r)
  CX_CHECK(cx_eddsa_get_public_key_internal(pv_key, hashID, NULL, a, sizeof(a), r, sizeof(r), scal));

  // compute r
  // - last size (32/57) bytes of H(sk), h,  as big endian bytes ordered. stored in r
  // - r = H(h,M) as little endian
  cx_hash_init_ex(&G_cx.hash_ctx, hashID, hsize);

  if (pv_key->curve == CX_CURVE_Ed448) {
    cx_hash_update(&G_cx.hash_ctx, C_cx_siged448, sizeof(C_cx_siged448));
    scal[0] = 0; // no ph
    scal[1] = 0; // no ctx
    cx_hash_update(&G_cx.hash_ctx, scal, 2);
  }
  cx_hash_update(&G_cx.hash_ctx, r, size);
  cx_hash_update(&G_cx.hash_ctx, hash, hash_len);
  cx_hash_final(&G_cx.hash_ctx, scal);
  cx_encode_int(scal, hsize);

  CX_CHECK(cx_bn_alloc_init(&bn_h, hsize, scal, hsize));
  CX_CHECK(cx_bn_alloc(&bn_r, size));
  CX_CHECK(cx_bn_reduce(bn_r, bn_h, bn_n));
  CX_CHECK(cx_bn_export(bn_r, r, size));
  CX_CHECK(cx_bn_destroy(&bn_h));

  // compute R = r.B
  CX_CHECK(cx_ecdomain_generator_bn(pv_key->curve, &Q));
  // Scalar multiplication with random projective coordinates and additive splitting
  CX_CHECK(cx_ecpoint_rnd_scalarmul(&Q, r, size));
  CX_CHECK(cx_ecpoint_compress(&Q, sig, size, &sign));
  cx_encode_coord(sig, size, sign);

  // compute S = r+H(R,A,M).a
  // - compute and compress public_key A
  CX_CHECK(cx_ecdomain_generator_bn(pv_key->curve, &Q));
  // Scalar multiplication with random projective coordinates and additive splitting
  CX_CHECK(cx_ecpoint_rnd_scalarmul(&Q, a, size));
  CX_CHECK(cx_ecpoint_compress(&Q, sig + size, size, &sign));
  cx_encode_coord(sig+size, size, sign);

  // - compute H(R,A,M)
  cx_hash_init_ex(&G_cx.hash_ctx, hashID, hsize);
  if (pv_key->curve == CX_CURVE_Ed448) {
    cx_hash_update(&G_cx.hash_ctx, C_cx_siged448, sizeof(C_cx_siged448));
    scal[0] = 0; // no ph
    scal[1] = 0; // no ctx
    cx_hash_update(&G_cx.hash_ctx, scal, 2);
  }
  cx_hash_update(&G_cx.hash_ctx, sig, size);
  cx_hash_update(&G_cx.hash_ctx, sig + size, size);
  cx_hash_update(&G_cx.hash_ctx, hash, hash_len);
  cx_hash_final(&G_cx.hash_ctx, scal);
  cx_encode_int(scal, hsize);

  // - compute S = r+H(.)a

  //-- H(R,A,M).a
  CX_CHECK(cx_bn_alloc(&bn_s, size));
  CX_CHECK(cx_bn_alloc_init(&bn_h, hsize, scal, hsize));
  CX_CHECK(cx_bn_reduce(bn_r, bn_h, bn_n));
  CX_CHECK(cx_bn_alloc_init(&bn_a, size, a, size));
  CX_CHECK(cx_bn_mod_mul(bn_s, bn_r, bn_a, bn_n));
  //-- r +
  CX_CHECK(cx_bn_init(bn_r, r, size));
  CX_CHECK(cx_bn_mod_add(bn_s, bn_s, bn_r, bn_n));
  cx_bn_set_u32(bn_r, 0);
  CX_CHECK(cx_bn_mod_sub(bn_s, bn_s, bn_r, bn_n));

  //- encode
  CX_CHECK(cx_bn_export(bn_s, sig + size, size));
  cx_encode_int(sig + size, size);

 end:
  cx_bn_unlock();
  return error;
}

/* ----------------------------------------------------------------------- */
/*                                                                         */
/* ----------------------------------------------------------------------- */
bool cx_eddsa_verify_no_throw(const cx_ecfp_public_key_t *pu_key,
                     cx_md_t                     hashID,
                     const uint8_t *             hash,
                     size_t                      hash_len,
                     const uint8_t *             sig,
                     size_t                      sig_len) {
  size_t                     size, hsize;
  uint8_t                    left[115];
  uint8_t                    scal[57];
  // Second scalar for double scalar multiplication
  uint8_t                    scal_left[57];
  bool                       verified, are_equal;

  uint32_t                   sign;
  cx_bn_t      bn_h, bn_rs, bn_n;
  cx_bn_t bn_p, bn_y;
  cx_ecpoint_t Q, R, Right, Left;
  cx_err_t error;
  int diff;

  CX_CHECK(cx_ecdomain_parameters_length(pu_key->curve, &size));

  if (!CX_CURVE_RANGE(pu_key->curve, TWISTED_EDWARDS)) {
    error = CX_INVALID_PARAMETER;
    goto end;
  }
  if (!((pu_key->W_len == 1 + size) || (pu_key->W_len == 1 + 2 * size))) {
    error = CX_INVALID_PARAMETER;
    goto end;
  }
  if (sig_len != 2 * size) {
    error = CX_INVALID_PARAMETER;
    goto end;
  }

  switch (hashID) {
#if (defined(HAVE_SHA512) || defined(HAVE_SHA3))
# if defined(HAVE_SHA512)
  case CX_SHA512:
# endif // HAVE_SHA512

# if defined(HAVE_SHA3)
  case CX_KECCAK:
  case CX_SHA3:
# endif // HAVE_SHA3
    if (size * 2 != 512 / 8) {
      error = CX_INVALID_PARAMETER;
      goto end;
    }
    break;
#endif // (defined(HAVE_SHA512) || defined(HAVE_SHA3))

#if (defined(HAVE_SHA3) || defined(HAVE_BLAKE2))
# if defined(HAVE_SHA3)
  case CX_SHAKE256:
# endif // HAVE_SHA3

# if defined(HAVE_BLAKE2)
  case CX_BLAKE2B:
# endif // HAVE_BLAKE2
    break;
#endif // (defined(HAVE_SHA3) || defined(HAVE_BLAKE2))

  default:
    error = CX_INVALID_PARAMETER;
    goto end;
  }

  verified = 0;

  hsize = 2 * size;
  memset(left, 0, sizeof(left));
  memset(scal, 0, sizeof(scal));
  // set scal_left
  memset(scal_left, 0, sizeof(scal_left));

  CX_CHECK(cx_bn_lock(size, 0));
  CX_CHECK(cx_ecpoint_alloc(&Q, pu_key->curve));

  // Compute H(R || A || M),
  cx_hash_init_ex(&G_cx.hash_ctx, hashID, hsize);
  // -prefix for Ed448
  if (pu_key->curve == CX_CURVE_Ed448) {
    cx_hash_update(&G_cx.hash_ctx, C_cx_siged448, sizeof(C_cx_siged448));
    scal[0] = 0; // no ph
    scal[1] = 0; // no ctx
    cx_hash_update(&G_cx.hash_ctx, scal, 2);
  }
  // -R
  cx_hash_update(&G_cx.hash_ctx, sig, size);
  // -A, compress public key
  if (pu_key->W[0] == 0x04) {
    CX_CHECK(cx_ecpoint_init(&Q, &pu_key->W[1], size, &pu_key->W[1 + size], size));
    CX_CHECK(cx_ecpoint_compress(&Q, scal, size, &sign));
    cx_encode_coord(scal, size, sign);
  } else {
    memmove(scal, &pu_key->W[1], size);
  }
  cx_hash_update(&G_cx.hash_ctx, scal, size);
  // -M
  cx_hash_update(&G_cx.hash_ctx, hash, hash_len);
  cx_hash_final(&G_cx.hash_ctx, left);
  cx_encode_int(left, hsize);

  CX_CHECK(cx_bn_alloc(&bn_n, size));
  CX_CHECK(cx_ecdomain_parameter_bn(pu_key->curve, CX_CURVE_PARAM_Order, bn_n));
  CX_CHECK(cx_bn_alloc(&bn_rs, size));

  // Use double scalar multiplication and check that R = sB - H(...).A

  // Init the first component of the double scalar: - H(...).A
  CX_CHECK(cx_bn_alloc_init(&bn_h, hsize, left, hsize));
  CX_CHECK(cx_bn_reduce(bn_rs, bn_h, bn_n));
  CX_CHECK(cx_bn_export(bn_rs, scal, size));
  CX_CHECK(cx_ecpoint_init(&Q, &pu_key->W[1], size, &pu_key->W[1 + size], size));
  CX_CHECK(cx_ecpoint_neg(&Q));

  // Init the second component of the double scalar:  s.B
  // -> decode s
  memmove(scal_left, sig + size, size);
  cx_decode_int(scal_left, size);

  // The second half of the signature s must be in range 0 <= s < L to prevent
  // signature malleability.
  CX_CHECK(cx_bn_alloc_init(&bn_rs, size, scal_left, size));
  CX_CHECK(cx_bn_cmp(bn_rs, bn_n, &diff));
  if (diff >= 0) {
    goto end;
  }

  CX_CHECK(cx_ecpoint_alloc(&Left, pu_key->curve));
  CX_CHECK(cx_ecdomain_generator_bn(pu_key->curve, &Left));

  CX_CHECK(cx_ecpoint_alloc(&Right, pu_key->curve));

  // P = Q
  CX_CHECK(cx_ecpoint_cmp(&Left, &Q, &are_equal));
  if (are_equal) {
    CX_CHECK(cx_ecpoint_scalarmul(&Left, scal_left, size));
    CX_CHECK(cx_ecpoint_scalarmul(&Q, scal, size));
    CX_CHECK(cx_ecpoint_add(&Right, &Left, &Q));
  }
  else{
    // double scalar multiplication sB - H(...)A iff P != +/-Q
    CX_CHECK(cx_ecpoint_double_scalarmul(&Right, &Left, &Q, scal_left, size, scal, size));
  }

  // -> decompress R
  CX_CHECK(cx_ecpoint_alloc(&R, pu_key->curve));
  memmove(scal, sig, size);
  sign = cx_decode_coord(scal, size);

  // field characteristic
  CX_CHECK(cx_bn_alloc(&bn_p, size));
  CX_CHECK(cx_ecdomain_parameter_bn(pu_key->curve, CX_CURVE_PARAM_Field, bn_p));

  // If the y coordinate is >= p then the decoding fails
  CX_CHECK(cx_bn_alloc(&bn_y, size));
  CX_CHECK(cx_bn_init(bn_y, scal, size));

  CX_CHECK(cx_bn_cmp(bn_y, bn_p, &diff));
  if (diff >= 0) {
    goto end;
  }

  CX_CHECK(cx_ecpoint_decompress(&R, scal, size, sign));
  CX_CHECK(cx_ecpoint_destroy(&Left));
  CX_CHECK(cx_ecpoint_destroy(&Q));

  // Check the signature
  CX_CHECK(cx_ecpoint_cmp(&R, &Right, &verified));

 end:
  cx_bn_unlock();
  return error == CX_OK && verified;
}

#endif // HAVE_EDDSA
#endif // HAVE_ECC_TWISTED_EDWARDS
