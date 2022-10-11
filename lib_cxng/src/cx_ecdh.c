
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

#if defined(HAVE_ECDH) || defined(HAVE_X25519) || defined(HAVE_X448)

#include "cx_ecfp.h"
#include "cx_utils.h"
#include "cx_ram.h"

#include <string.h>

#if defined(HAVE_ECDH)
cx_err_t cx_ecdh_no_throw(const cx_ecfp_private_key_t *key,
                 uint32_t                     mode,
                 const uint8_t *              public_point,
                 size_t                       P_len,
                 uint8_t *                    secret,
                 size_t                       secret_len) {
  size_t                   sz;
  cx_curve_t               curve;
  cx_err_t                 error;
  cx_ecpoint_t             W;

  curve = key->curve;
  CX_CHECK(cx_ecdomain_parameters_length(curve, &sz));

  if (false
#ifdef HAVE_ECC_WEIERSTRASS
   || CX_CURVE_RANGE(curve, WEIERSTRASS)
#endif
#ifdef HAVE_ECC_MONTGOMERY
   || CX_CURVE_RANGE(curve, MONTGOMERY)
#endif
   ) {
    if (P_len != (1 + sz * 2)) {
      error = CX_INVALID_PARAMETER;
      goto end;
    }
    if (!(((mode & CX_MASK_EC) == CX_ECDH_X) || ((mode & CX_MASK_EC) == CX_ECDH_POINT))) {
      error = CX_INVALID_PARAMETER;
      goto end;
    }
  }
  else {
    error = INVALID_PARAMETER;
    goto end;
  }

  switch (mode & CX_MASK_EC) {
  case CX_ECDH_POINT:
    if (secret_len < P_len) {
      error = INVALID_PARAMETER;
      goto end;
    }
    break;
  case CX_ECDH_X:
    if (secret_len < sz) {
      error = INVALID_PARAMETER;
      goto end;
    }
    break;
  default:
    error = INVALID_PARAMETER;
    goto end;
    break;
  }

  CX_CHECK(cx_bn_lock(sz, 0));
  CX_CHECK(cx_ecpoint_alloc(&W, curve));
  CX_CHECK(cx_ecpoint_init(&W, public_point + 1, sz, public_point + 1 + sz, sz));
  // Scalar multiplication with random projective coordinates and additive splitting
  CX_CHECK(cx_ecpoint_rnd_scalarmul(&W, key->d, key->d_len));
  switch (mode & CX_MASK_EC) {
  case CX_ECDH_POINT:
    secret[0] = 0x04;
    CX_CHECK(cx_ecpoint_export(&W, secret + 1, sz, secret + 1 + sz, sz));
    break;
  case CX_ECDH_X:
    CX_CHECK(cx_ecpoint_export(&W, secret, sz, NULL, 0));
    break;
  }

 end:
  cx_bn_unlock();
  return error;
}

#endif // HAVE_ECDH

#if defined(HAVE_X25519)
cx_err_t cx_x25519(uint8_t *u, const uint8_t *k, size_t k_len) {
  cx_bn_t  bn_u;
  size_t   domain_length;
  cx_err_t error;

  CX_CHECK(cx_ecdomain_parameters_length(CX_CURVE_Curve25519, &domain_length));
  CX_CHECK(cx_bn_lock(domain_length, 0));
  CX_CHECK(cx_bn_alloc(&bn_u, domain_length));
  CX_CHECK(cx_bn_init(bn_u, u, domain_length));
  CX_CHECK(cx_ecpoint_x25519(bn_u, k, k_len));
  CX_CHECK(cx_bn_export(bn_u, u, domain_length));

  end:
    cx_bn_unlock();
    return error;
}
#endif // HAVE_X25519

#if defined(HAVE_X448)
cx_err_t cx_x448(uint8_t *u, const uint8_t *k, size_t k_len) {
  cx_bn_t  bn_u;
  size_t   domain_length;
  cx_err_t error;

  CX_CHECK(cx_ecdomain_parameters_length(CX_CURVE_Curve448, &domain_length));
  CX_CHECK(cx_bn_lock(domain_length, 0));
  CX_CHECK(cx_bn_alloc(&bn_u, domain_length));
  CX_CHECK(cx_bn_init(bn_u, u, domain_length));
  CX_CHECK(cx_ecpoint_x448(bn_u, k, k_len));
  CX_CHECK(cx_bn_export(bn_u, u, domain_length));

  end:
    cx_bn_unlock();
    return error;
}
#endif // HAVE_X448

#endif // HAVE_ECDH || HAVE_X25519 || HAVE_X448
