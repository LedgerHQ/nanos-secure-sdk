
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

#ifdef HAVE_MATH

#include "cx_math.h"
#include "cx_utils.h"


cx_err_t cx_math_cmp_no_throw ( const uint8_t * a, const uint8_t * b, size_t length, int *diff ) {
  cx_err_t error;
  cx_bn_t bn_a, bn_b;

  CX_CHECK(cx_bn_lock(length, 0));
  CX_CHECK(cx_bn_alloc_init(&bn_a, length, a, length));
  CX_CHECK(cx_bn_alloc_init(&bn_b, length, b, length));
  CX_CHECK(cx_bn_cmp(bn_a, bn_b, diff));
  cx_bn_unlock();

end:
  return error;
}

cx_err_t cx_math_add_no_throw( uint8_t * r, const uint8_t * a, const uint8_t * b, size_t len ) {
  cx_bn_t bn_a, bn_b, bn_r;
  cx_err_t error;

  CX_CHECK(cx_bn_lock(len, 0));
  CX_CHECK(cx_bn_alloc(&bn_r, len));
  CX_CHECK(cx_bn_alloc_init(&bn_a, len, a, len));
  CX_CHECK(cx_bn_alloc_init(&bn_b, len, b, len));
  CX_CHECK_IGNORE_CARRY(cx_bn_add(bn_r, bn_a, bn_b));
  CX_CHECK(cx_bn_export(bn_r, r, len));
  cx_bn_unlock();

end:
  return error;
}

cx_err_t cx_math_sub_no_throw( uint8_t * r, const uint8_t * a, const uint8_t * b, size_t len ) {
  cx_bn_t bn_a, bn_b, bn_r;
  cx_err_t error;

  CX_CHECK(cx_bn_lock(len, 0));
  CX_CHECK(cx_bn_alloc(&bn_r, len));
  CX_CHECK(cx_bn_alloc_init(&bn_a, len, a, len));
  CX_CHECK(cx_bn_alloc_init(&bn_b, len, b, len));
  CX_CHECK_IGNORE_CARRY(cx_bn_sub(bn_r, bn_a, bn_b));
  CX_CHECK(cx_bn_export(bn_r, r, len));
  cx_bn_unlock();

end:
  return error;
}

cx_err_t cx_math_mult_no_throw( uint8_t * r, const uint8_t * a, const uint8_t * b, size_t len ) {
  cx_bn_t bn_a, bn_b, bn_r;
  cx_err_t error;

  CX_CHECK(cx_bn_lock(len, 0));
  CX_CHECK(cx_bn_alloc(&bn_r, 2*len));
  CX_CHECK(cx_bn_alloc_init(&bn_a, len, a, len));
  CX_CHECK(cx_bn_alloc_init(&bn_b, len, b, len));
  CX_CHECK(cx_bn_mul(bn_r, bn_a, bn_b));
  CX_CHECK(cx_bn_export(bn_r, r, 2*len));
  cx_bn_unlock();

end:
  return error;
}

cx_err_t cx_math_addm_no_throw( uint8_t * r, const uint8_t * a, const uint8_t * b, const uint8_t * m, size_t len ) {
  cx_bn_t bn_a, bn_b, bn_m, bn_r;
  cx_err_t error;

  CX_CHECK(cx_bn_lock(len, 0));
  CX_CHECK(cx_bn_alloc(&bn_r, len));
  CX_CHECK(cx_bn_alloc_init(&bn_a, len, a, len));
  CX_CHECK(cx_bn_alloc_init(&bn_b, len, b, len));
  CX_CHECK(cx_bn_alloc_init(&bn_m, len, m, len));
  CX_CHECK(cx_bn_mod_add(bn_r, bn_a, bn_b, bn_m));
#ifdef ST33K1M5
  cx_bn_set_u32(bn_a, 0);
  CX_CHECK(cx_bn_mod_sub(bn_r, bn_r, bn_a, bn_m));
#endif // ST33K1M5
  CX_CHECK(cx_bn_export(bn_r, r, len));
  cx_bn_unlock();

end:
  return error;
}

cx_err_t cx_math_subm_no_throw( uint8_t * r, const uint8_t * a, const uint8_t * b, const uint8_t * m, size_t len ) {
  cx_bn_t bn_a, bn_b, bn_m, bn_r;
  cx_err_t error;

  CX_CHECK(cx_bn_lock(len, 0));
  CX_CHECK(cx_bn_alloc(&bn_r, len));
  CX_CHECK(cx_bn_alloc_init(&bn_a, len, a, len));
  CX_CHECK(cx_bn_alloc_init(&bn_b, len, b, len));
  CX_CHECK(cx_bn_alloc_init(&bn_m, len, m, len));
  CX_CHECK(cx_bn_mod_sub(bn_r, bn_a, bn_b, bn_m));
  CX_CHECK(cx_bn_export(bn_r, r, len));
  cx_bn_unlock();

end:
  return error;
}

cx_err_t cx_math_multm_no_throw( uint8_t * r, const uint8_t * a, const uint8_t * b, const uint8_t * m, size_t len ) {
  cx_bn_t bn_a, bn_b, bn_m, bn_r;
  cx_err_t error;

  CX_CHECK(cx_bn_lock(len, 0));
  CX_CHECK(cx_bn_alloc(&bn_r, len));
  CX_CHECK(cx_bn_alloc_init(&bn_a, len, a, len));
  CX_CHECK(cx_bn_alloc_init(&bn_b, len, b, len));
  CX_CHECK(cx_bn_alloc_init(&bn_m, len, m, len));
  CX_CHECK(cx_bn_mod_mul(bn_r, bn_a, bn_b, bn_m));
  CX_CHECK(cx_bn_export(bn_r, r, len));
  cx_bn_unlock();

end:
  return error;
}

cx_err_t cx_math_modm_no_throw( uint8_t * v, size_t len_v, const uint8_t * m, size_t len_m ) {
  cx_bn_t bn_v, bn_m, bn_r;
  cx_err_t error;

  CX_CHECK(cx_bn_lock(len_v, 0));
  CX_CHECK(cx_bn_alloc(&bn_r, len_v));
  CX_CHECK(cx_bn_alloc_init(&bn_v, len_v, v, len_v));
  CX_CHECK(cx_bn_alloc_init(&bn_m, len_m, m, len_m));
  CX_CHECK(cx_bn_reduce(bn_r, bn_v, bn_m));
  CX_CHECK(cx_bn_export(bn_r, v, len_v));
  cx_bn_unlock();

end:
  return error;
}

cx_err_t cx_math_powm_no_throw( uint8_t * r, const uint8_t * a, const uint8_t * e, size_t len_e, const uint8_t * m, size_t len ) {
  cx_bn_t bn_a, bn_m, bn_r;
  cx_err_t error;

  CX_CHECK(cx_bn_lock(len, 0));
  CX_CHECK(cx_bn_alloc_init(&bn_a, len, a, len));
  CX_CHECK(cx_bn_alloc_init(&bn_m, len, m, len));
  CX_CHECK(cx_bn_alloc(&bn_r, len));
  CX_CHECK(cx_bn_mod_pow2(bn_r, bn_a, e, len_e, bn_m));
  CX_CHECK(cx_bn_export(bn_r, r, len));
  cx_bn_unlock();

end:
  return error;
}

cx_err_t cx_math_invprimem_no_throw( uint8_t * r, const uint8_t * a, const uint8_t * m, size_t len ) {
  cx_bn_t bn_a, bn_r, bn_m;
  cx_err_t error;

  CX_CHECK(cx_bn_lock(len, 0));
  CX_CHECK(cx_bn_alloc(&bn_r, len));
  CX_CHECK(cx_bn_alloc_init(&bn_a, len, a, len));
  CX_CHECK(cx_bn_alloc_init(&bn_m, len, m, len));
  CX_CHECK(cx_bn_mod_invert_nprime(bn_r, bn_a, bn_m));
  CX_CHECK(cx_bn_export(bn_r, r, len));
  cx_bn_unlock();

end:
  return error;
}

cx_err_t cx_math_invintm_no_throw( uint8_t * r, uint32_t a, const uint8_t * m, size_t len ) {
  cx_bn_t bn_r, bn_m;
  cx_err_t error;

  CX_CHECK(cx_bn_lock(len, 0));
  CX_CHECK(cx_bn_alloc(&bn_r, len));
  CX_CHECK(cx_bn_alloc_init(&bn_m, len, m, len));
  CX_CHECK(cx_bn_mod_u32_invert(bn_r, a, bn_m));
  CX_CHECK(cx_bn_export(bn_r, r, len));
  cx_bn_unlock();

end:
  return error;
}

cx_err_t cx_math_is_prime_no_throw( const uint8_t * r, size_t len, bool *prime) {
  cx_bn_t bn_r;
  cx_err_t error;

  CX_CHECK(cx_bn_lock(len, 0));
  CX_CHECK(cx_bn_alloc_init(&bn_r, len, r, len));
  CX_CHECK(cx_bn_is_prime(bn_r, prime));
  cx_bn_unlock();

end:
  return error;
}

cx_err_t cx_math_next_prime_no_throw( uint8_t * r, uint32_t len ) {
  cx_bn_t bn_r;
  cx_err_t error;

  CX_CHECK(cx_bn_lock(len, 0));
  CX_CHECK(cx_bn_alloc_init(&bn_r, len, r, len));
  CX_CHECK(cx_bn_next_prime(bn_r));
  CX_CHECK(cx_bn_export(bn_r, r, len));
  cx_bn_unlock();

end:
  return error;
}

#endif // HAVE_MATH
