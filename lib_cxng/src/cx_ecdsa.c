
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

#ifdef HAVE_ECDSA

#include "cx_rng.h"
#include "cx_rng_rfc6979.h"
#include "cx_ecfp.h"
#include "cx_ecdsa.h"
#include "cx_utils.h"
#include "cx_ram.h"
#include "os_math.h"

#include <string.h>

static cx_err_t truernd(uint8_t *rnd, cx_curve_t cv) {
  cx_bn_t       r, t, n;
  uint8_t domain_order[CX_ECDSA_MAX_ORDER_LEN];
  size_t domain_length;
  cx_err_t error;

  CX_CHECK(cx_ecdomain_parameters_length(cv, &domain_length));
  CX_CHECK(cx_ecdomain_parameter(cv, CX_CURVE_PARAM_Order, domain_order, sizeof(domain_order)));

  CX_CHECK(cx_bn_lock(domain_length, 0));
  CX_CHECK(cx_bn_alloc(&r, domain_length));
  CX_CHECK(cx_bn_alloc(&t, 2 * domain_length));
  CX_CHECK(cx_bn_alloc_init(&n, domain_length, domain_order, domain_length));
  CX_CHECK(cx_bn_rand(t));
  CX_CHECK(cx_bn_reduce(r, t, n));
  CX_CHECK(cx_bn_export(r, rnd, domain_length));
  cx_bn_unlock();

 end:
  return error;
}

static cx_err_t rfcrnd(uint8_t *rnd, uint32_t skip,
                       cx_curve_t cv,
                       const cx_ecfp_private_key_t *key,
                       cx_md_t hashID, const uint8_t *hash, size_t hash_len) {
  uint8_t domain_order[CX_ECDSA_MAX_ORDER_LEN];
  size_t domain_length;
  cx_err_t error;

  CX_CHECK(cx_ecdomain_parameters_length(cv, &domain_length));
  CX_CHECK(cx_ecdomain_parameter(cv, CX_CURVE_PARAM_Order, domain_order, sizeof(domain_order)));

  CX_CHECK(cx_rng_rfc6979_init(&G_cx.rfc6979, hashID, key->d, key->d_len, hash, hash_len, domain_order, domain_length/*, NULL, 0*/));
  skip++;
  for (uint32_t i = 0; i<skip; i++) {
    CX_CHECK(cx_rng_rfc6979_next(&G_cx.rfc6979, rnd, domain_length));
  }

 end:
  return error;
}


// This function is used to convert the incoming hash into its bn
// representation, depending on its length compared to the domain's
// associated lengths.
// The v parameter must already point to an initialized RAM area, of length
// domain_length.
static cx_err_t initialize_hash(const uint8_t *hash,
                                cx_bn_t v,
                                size_t domain_bit_length,
                                size_t hash_length,
                                size_t domain_length) {
  cx_err_t error;
  
  // If the hash length is greater than the domain length, we only
  // consider the 'domain bit length' leftmost bits of the hash
  // for the operation.
  CX_CHECK(cx_bn_init(v, hash, MIN(hash_length, domain_length)));

  if (domain_bit_length < hash_length * 8) {
    // We shift the hash if necessary.
    if (domain_bit_length % 8) {
      CX_CHECK(cx_bn_shr(v, 8 - (domain_bit_length % 8)));
    }
  }

end:
  return error;
}

/* ----------------------------------------------------------------------- */
/*                                                                         */
/* ----------------------------------------------------------------------- */
cx_err_t cx_ecdsa_sign_no_throw(const cx_ecfp_private_key_t *key,
                       uint32_t                     mode,
                       cx_md_t                      hashID,
                       const uint8_t *              hash,
                       size_t                       hash_len,
                       uint8_t *                    sig,
                       size_t *                     sig_len,
                       uint32_t *                   info) {
  #ifndef HAVE_RNG_RFC6979
  (void)hashID;
  #endif

#define CX_MAX_TRIES 100

  cx_err_t error;
  uint32_t volatile out_sig_len = 0;
  size_t domain_length;
  size_t domain_bit_length;
  cx_bn_t       r, s;
  int diff;
  union {
    struct {
      uint32_t _tries;
      uint8_t _rnd[CX_ECDSA_MAX_ORDER_LEN];
      cx_ecpoint_t _Q;
      cx_bn_t _n, _t, _t1, _t2, _v;
    };
    uint8_t _rs[CX_ECDSA_MAX_ORDER_LEN * 2];
  } u;
  #define tries  u._tries
  #define rnd    u._rnd
  #define Q      u._Q
  #define n      u._n
  #define t      u._t
  #define t1     u._t1
  #define t2     u._t2
  #define v      u._v
  #define rs     u._rs

  // get dom
  CX_CHECK(cx_ecdomain_parameters_length(key->curve, &domain_length));
  CX_CHECK(cx_ecdomain_size(key->curve, &domain_bit_length));

  if (!CX_CURVE_RANGE(key->curve, WEIERSTRASS) ||
      *sig_len < 6 + 2 * (domain_length + 1) ||
      key->d_len != domain_length) {
    error = CX_INVALID_PARAMETER;
    goto end;
  }

  if (info) {
    *info = 0;
  }

  out_sig_len = *sig_len;

  // generate random
  tries = 0;
 RETRY:
  if (tries == CX_MAX_TRIES) {
    out_sig_len = 0;
    error = CX_INTERNAL_ERROR;
    goto end;
  }

  switch (mode & CX_MASK_RND) {
  default:
    error = CX_INVALID_PARAMETER;
    goto end;

  case CX_RND_PROVIDED:
    if (tries) {
      out_sig_len = 0;
      error = CX_INTERNAL_ERROR;
      goto end;
    }
    memmove(rnd, sig, domain_length);
    break;

  case CX_RND_TRNG:
    CX_CHECK(truernd(rnd, key->curve));
    break;

#ifdef HAVE_RNG_RFC6979
  case CX_RND_RFC6979:
    // If the hash length is greater than the domain length, we only consider
    // the domain length's leftmost bytes of the hash for the operation.
    // This optimisation works so long as (hash_len - domain_length) is a
    // multiple of 8, when hash_len > domain_length. Otherwise, the hash
    // needs to be shifted by (hash_len - domain_length) bits to fit into
    // domain_length bytes.
    CX_CHECK(rfcrnd(rnd, tries, key->curve, key, hashID, hash, MIN(hash_len, domain_length)));
    break;
#endif //HAVE_RNG_RFC6979
  }
  tries++;

  // compute the sig
  CX_CHECK(cx_bn_lock(domain_length, 0));

  // --> compute Q = k.G
  CX_CHECK(cx_ecpoint_alloc(&Q, key->curve));
  CX_CHECK(cx_ecdomain_generator_bn(key->curve, &Q));

#ifdef HAVE_FIXED_SCALAR_LENGTH
  // Additive splitting with random projective coordinates
  // The length of the scalar is fixed to not leak information
  CX_CHECK(cx_ecpoint_rnd_fixed_scalarmul(&Q, rnd, domain_length));
#else
  CX_CHECK(cx_ecpoint_rnd_scalarmul(&Q, rnd, domain_length));
#endif // HAVE_FIXED_SCALAR_LENGTH

  bool odd;
  CX_CHECK(cx_bn_is_odd(Q.y, &odd));

  // load order
  CX_CHECK(cx_bn_alloc(&n, domain_length));
  CX_CHECK(cx_ecdomain_parameter_bn(key->curve, CX_CURVE_PARAM_Order, n));

  // compute r
  CX_CHECK(cx_bn_alloc(&r, domain_length));
  CX_CHECK(cx_ecpoint_export_bn(&Q, &r, NULL));
  CX_CHECK(cx_ecpoint_destroy(&Q));
  CX_CHECK(cx_bn_cmp(r, n, &diff));
  if (diff >= 0) {
    CX_CHECK_IGNORE_CARRY(cx_bn_sub(r, r, n));
    if (info) {
      *info |= CX_ECCINFO_xGTn;
    }
  }

  //check r non zero
  CX_CHECK(cx_bn_cmp_u32(r, 0, &diff));
  if (diff == 0) {
    goto RETRY;
  }

  //some allocs
  CX_CHECK(cx_bn_alloc(&s, domain_length));
  CX_CHECK(cx_bn_alloc(&t, domain_length));
  CX_CHECK(cx_bn_alloc(&v, domain_length));
  CX_CHECK(cx_bn_alloc(&t1, domain_length));
  CX_CHECK(cx_bn_alloc(&t2, domain_length));

  // compute s = kinv(h+d.x)
  //
  // t random, 0 <= t < n
  // v = d - t
  // u = h + (d-t)*x +t*x  = h + v*x + t*x
  // s = k_inv*u
  //

  CX_CHECK(cx_bn_rng(t, n));
  CX_CHECK(cx_bn_init(t1, key->d, key->d_len));
  CX_CHECK(cx_bn_mod_sub(v, t1, t, n));                         // v
  CX_CHECK(cx_bn_mod_mul(t2, v, r, n));                         // v.x
  CX_CHECK(cx_bn_mod_mul(t1, t, r, n));                         // t.x

  CX_CHECK(initialize_hash(hash, v, domain_bit_length, hash_len, domain_length));
                                                                          // v = h (or domain bit length's leftmost bits of h)
  CX_CHECK(cx_bn_mod_add(v, v, t1, n));                         // v += t.x
  CX_CHECK(cx_bn_mod_add(v, v, t2, n));                         // v += v.x
  CX_CHECK(cx_bn_init(t1, rnd, domain_length));                    // k
  CX_CHECK(cx_bn_mod_invert_nprime(t2, t1, n));                  // k_inv
  CX_CHECK(cx_bn_mod_mul(s, v, t2, n));                         // s = k_inv*u
  //check s non zero
  CX_CHECK(cx_bn_cmp_u32(s, 0, &diff));
  if (diff == 0) {
    goto RETRY;
  }

  // "Sainte Canonisation"
  if ((mode & CX_NO_CANONICAL) == 0) {
    // if s > order/2, s = -s = order-s
    CX_CHECK_IGNORE_CARRY(cx_bn_sub(t1, n, s));
    CX_CHECK(cx_bn_shr(n, 1));
    CX_CHECK(cx_bn_cmp(s, n, &diff));
    if (diff > 0) {
      CX_CHECK(cx_bn_copy(s, t1));
      odd = !odd;
    }
  }

  //WARN:  from here only args and  locals out_sig_len, r, s, rs, domain_length are valid

  CX_CHECK(cx_bn_export(r, rs, domain_length));
  CX_CHECK(cx_bn_export(s, rs + domain_length, domain_length));

  //  --> build the signature in TLV:   T  L     T   L   r     T   L   s
  //                                 30 ll    02  ll  X1    02  ll  Y1
  // r,s == X2,Y2
  out_sig_len = cx_ecfp_encode_sig_der(sig, out_sig_len, rs, domain_length, rs + domain_length, domain_length);
  if (info) {
    *info |= odd ? CX_ECCINFO_PARITY_ODD : 0;
  }

 end:
  *sig_len = out_sig_len;
  cx_bn_unlock();
  return error;

  #undef CX_MAX_TRIES
  #undef tries
  #undef rnd
  #undef Q
  #undef n
  #undef t
  #undef t1
  #undef t2
  #undef v
  #undef rs
}

/* ----------------------------------------------------------------------- */
/*                                                                         */
/* ----------------------------------------------------------------------- */
bool cx_ecdsa_verify_no_throw(const cx_ecfp_public_key_t *key,
                     const uint8_t *             hash,
                     size_t                      hash_len,
                     const uint8_t *             sig,
                     size_t                      sig_len) {
  bool                     verified;
  size_t                   domain_length, domain_bit_length;
  cx_bn_t                  c, n, r, s, u1, u2, h;
  cx_ecpoint_t             R, Q, G;
  cx_err_t                 error;
  int                      diff;
  bool                     is_infinite;

  verified = false;

  CX_CHECK(cx_ecdomain_parameters_length(key->curve, &domain_length));
  CX_CHECK(cx_ecdomain_size(key->curve, &domain_bit_length));

  if (!CX_CURVE_RANGE(key->curve, WEIERSTRASS) || key->W_len != 1 + 2 * domain_length) {
    error = CX_INVALID_PARAMETER;
    goto end;
  }

  // verify
  //. check sig format
  if (hash_len > (8 + 2 * domain_length)) {
    error = CX_INVALID_PARAMETER;
    goto end;
  }

  const uint8_t *sig_r, *sig_s;
  size_t         sig_rlen, sig_slen;

  // decode
  if (!cx_ecfp_decode_sig_der(sig, sig_len, domain_length, &sig_r, &sig_rlen, &sig_s, &sig_slen)) {
    error = CX_INVALID_PARAMETER;
    goto end;
  }

  // setup
  CX_CHECK(cx_bn_lock(domain_length, 0));
  CX_CHECK(cx_bn_alloc(&n, domain_length));
  CX_CHECK(cx_ecdomain_parameter_bn(key->curve, CX_CURVE_PARAM_Order, n));
  CX_CHECK(cx_bn_alloc_init(&r, domain_length, sig_r, sig_rlen));
  CX_CHECK(cx_bn_alloc_init(&s, domain_length, sig_s, sig_slen));
  CX_CHECK(cx_bn_alloc(&h, domain_length));
  CX_CHECK(initialize_hash(hash, h, domain_bit_length, hash_len, domain_length));
  CX_CHECK(cx_bn_alloc(&u1, domain_length));
  CX_CHECK(cx_bn_alloc(&u2, domain_length));
  CX_CHECK(cx_bn_alloc(&c, domain_length));

  // 1 check 0 < r <= N-1 and 0 < s <= N-1
  CX_CHECK(cx_bn_set_u32(u2, 0));
  CX_CHECK(cx_bn_cmp(r, u2, &diff));
  if (diff == 0) {
    goto end;
  }
  CX_CHECK(cx_bn_cmp(r, n, &diff));
  if (diff >= 0) {
    goto end;
  }
  CX_CHECK(cx_bn_cmp(s, u2, &diff));
  if (diff == 0) {
    goto end;
  }
  CX_CHECK(cx_bn_cmp(s, n, &diff));
  if (diff >= 0) {
    goto end;
  }

  // 2 verify r/s
  //.compute c = inv(s) mod N ;
  CX_CHECK(cx_bn_mod_invert_nprime(c, s, n));
  //.compute u1 = hash*c mod N *
  CX_CHECK(cx_bn_mod_mul(u1, h, c, n));
  //.compute u2 = r*c mod N
  CX_CHECK(cx_bn_mod_mul(u2, r, c, n));
  // uG+vQ
  CX_CHECK(cx_bn_destroy(&h));
  CX_CHECK(cx_bn_destroy(&c));
  CX_CHECK(cx_bn_destroy(&s));
  CX_CHECK(cx_ecpoint_alloc(&G, key->curve));
  CX_CHECK(cx_ecdomain_generator_bn(key->curve, &G));
  CX_CHECK(cx_ecpoint_alloc(&Q, key->curve));
  CX_CHECK(cx_ecpoint_init(&Q, &key->W[1], domain_length, &key->W[1 + domain_length], domain_length));

  CX_CHECK(cx_ecpoint_alloc(&R,key->curve));

  // Double scalar multiplication using Straus-Shamir 's trick
  CX_CHECK(cx_ecpoint_double_scalarmul_bn(&R, &G , &Q , u1, u2));

  // Check if R is infinite point
  CX_CHECK(cx_ecpoint_is_at_infinity(&R, &is_infinite));
  if (is_infinite) {
    goto end;
  }
  //.compute  v = x1 mod N
  CX_CHECK(cx_ecpoint_export_bn(&R, &u1, NULL));
  CX_CHECK(cx_bn_reduce(u2, u1, n));
  //.verify value
  CX_CHECK(cx_bn_cmp(u2, r, &diff));
  if (diff) {
    goto end;
  }

  verified = true;

 end:
  cx_bn_unlock();
  return (error == CX_OK) && verified;
}

#endif // HAVE_ECDSA
