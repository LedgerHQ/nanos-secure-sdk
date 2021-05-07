
/*******************************************************************************
*   Ledger Nano S - Secure firmware
*   (c) 2021 Ledger
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

/*
 * This file is not intended to be included directly.
 * Include "lbcxng.h" instead
 */

#ifdef HAVE_MATH

#ifndef LCX_MATH_H
#define LCX_MATH_H

#include "lcx_wrappers.h"
#include "ox_bn.h"

/**
 * Compare two operands
 *
 * @param [in] a
 *   Operand a
 *
 * @param [in] b
 *   Operand b
 *
 * @param [in] length
 *   Length on which the operands should be compared in bytes
 *
 * @return
 *   Result of the comparison ie:
 *   0 if a and b are identical
 *   < 0 if a is less than b
 *   > 0 if a is greater than b
 */
cx_err_t cx_math_cmp_no_throw(const uint8_t *a, const uint8_t *b, size_t length, int *diff);

static inline int32_t cx_math_cmp(const uint8_t *a, const uint8_t *b, size_t length) {
  int diff;
  CX_THROW(cx_math_cmp_no_throw(a, b, length, &diff));
  return diff;
}

/**
 * Add two operands
 *
 * @param [in] r
 *   Result of the operation
 *
 * @param [in] a
 *   Operand a
 *
 * @param [in] b
 *   Operand b
 *
 * @param [in] length
 *   Length on which the operands should be added in bytes
 *
 * @return
 *   a bool defining whether the operation has a carry or not:
 *   0 : has no carry
 *   1 : has a carry
 */

cx_err_t cx_math_add_no_throw(uint8_t *r, const uint8_t *a, const uint8_t *b, size_t len);

static inline uint32_t cx_math_add(uint8_t *r, const uint8_t *a, const uint8_t *b, size_t len) {
  cx_err_t error = cx_math_add_no_throw(r, a, b, len);
  if (error && error != CX_CARRY) {
    THROW(error);
  }
  return (error == CX_CARRY);
}

/**
 * Substract two operands
 *
 * @param [in] r
 *   Result of the operation
 *
 * @param [in] a
 *   Operand a
 *
 * @param [in] b
 *   Operand b
 *
 * @param [in] length
 *   Length on which the operands should be substracted in bytes
 *
 * @return
 *   a bool defining whether the operation has a carry or not (< 0):
 *   0 : has no carry
 *   1 : has a carry
 */

cx_err_t cx_math_sub_no_throw(uint8_t *r, const uint8_t *a, const uint8_t *b, size_t len);

static inline uint32_t cx_math_sub(uint8_t *r, const uint8_t *a, const uint8_t *b, size_t len) {
  cx_err_t error = cx_math_sub_no_throw(r, a, b, len);
  if (error && error != CX_CARRY) {
    THROW(error);
  }
  return (error == CX_CARRY);
}

/**
 * Multiply two operands
 *
 * @param [in] r
 *   Result of the operation
 *
 * @param [in] a
 *   Operand a
 *
 * @param [in] b
 *   Operand b
 *
 * @param [in] length
 *   Length on which the operands should be multiplied in bytes
 */

cx_err_t cx_math_mult_no_throw(uint8_t *r, const uint8_t *a, const uint8_t *b, size_t len);

static inline void cx_math_mult(uint8_t *r, const uint8_t *a, const uint8_t *b, size_t len) {
  CX_THROW(cx_math_mult_no_throw(r, a, b, len));
}

/**
 * Modular Addition of two operands
 *
 * @param [in] r
 *   Result of the operation
 *
 * @param [in] a
 *   Operand a
 *
 * @param [in] b
 *   Operand b
 *
 * @param [in] m
 *   Modulo
 *
 * @param [in] length
 *   Length on which the operands should be mod-added in bytes
 */

cx_err_t cx_math_addm_no_throw(uint8_t *r, const uint8_t *a, const uint8_t *b, const uint8_t *m, size_t len);

static inline void cx_math_addm(uint8_t *r, const uint8_t *a, const uint8_t *b, const uint8_t *m, size_t len) {
  CX_THROW(cx_math_addm_no_throw(r, a, b, m, len));
}

/**
 * Modular Substraction of two operands
 *
 * @param [in] r
 *   Result of the operation
 *
 * @param [in] a
 *   Operand a
 *
 * @param [in] b
 *   Operand b
 *
 * @param [in] m
 *   Modulo
 *
 * @param [in] length
 *   Length on which the operands should be mod-subbed in bytes
 */

cx_err_t cx_math_subm_no_throw(uint8_t *r, const uint8_t *a, const uint8_t *b, const uint8_t *m, size_t len);

static inline void cx_math_subm(uint8_t *r, const uint8_t *a, const uint8_t *b, const uint8_t *m, size_t len) {
  CX_THROW(cx_math_subm_no_throw(r, a, b, m, len));
}

/**
 * Modular Multiplication of two operands
 *
 * @param [in] r
 *   Result of the operation
 *
 * @param [in] a
 *   Operand a
 *
 * @param [in] b
 *   Operand b
 *
 * @param [in] m
 *   Modulo
 *
 * @param [in] length
 *   Length on which the operands should be mod-mult in bytes
 */

cx_err_t cx_math_multm_no_throw(uint8_t *r, const uint8_t *a, const uint8_t *b, const uint8_t *m, size_t len);

static inline void cx_math_multm(uint8_t *r, const uint8_t *a, const uint8_t *b, const uint8_t *m, size_t len) {
  CX_THROW(cx_math_multm_no_throw(r, a, b, m, len));
}

/**
 * Modulo Operation. (v % m)
 *
 * @param [in] v
 *   First operand (and also the result)
 *
 * @param [in] len_v
 *   Size of the first operand in bytes
 *
 * @param [in] m
 *   Second operand aka the modulo
 *
 * @param [in] len_m
 *   Size of the second operand in bytes
 */

cx_err_t cx_math_modm_no_throw(uint8_t *v, size_t len_v, const uint8_t *m, size_t len_m);

static inline void cx_math_modm(uint8_t *v, size_t len_v, const uint8_t *m, size_t len_m) {
  CX_THROW(cx_math_modm_no_throw(v, len_v, m, len_m));
}

/**
 * Modular Exponentiation of a number
 *
 * @param [in] r
 *   Result of the operation
 *
 * @param [in] a
 *   Operand a
 *
 * @param [in] e
 *   Operand e aka the exponent
 *
 * @param [in] len_e
 *   Size of the exponent in bytes
 *
 * @param [in] m
 *   Modulo
 *
 * @param [in] length
 *   Length of the result in bytes
 */

cx_err_t cx_math_powm_no_throw(uint8_t *r, const uint8_t *a, const uint8_t *e, size_t len_e, const uint8_t *m, size_t len);

static inline void cx_math_powm(uint8_t *r, const uint8_t *a, const uint8_t *e, size_t len_e, const uint8_t *m, size_t len) {
  CX_THROW(cx_math_powm_no_throw(r, a, e, len_e, m, len));
}

/**
 * Modular Inverse of a prime number
 *
 * @param [in] r
 *   Result of the operation
 *
 * @param [in] a
 *   Operand a
 *
 * @param [in] m
 *   Modulo
 *
 * @param [in] length
 *   Length of the result in bytes
 */

cx_err_t cx_math_invprimem_no_throw(uint8_t *r, const uint8_t *a, const uint8_t *m, size_t len);

static inline void cx_math_invprimem(uint8_t *r, const uint8_t *a, const uint8_t *m, size_t len) {
  CX_THROW(cx_math_invprimem_no_throw(r, a, m, len));
}

/**
 * Modular Inverse of a number
 *
 * @param [in] r
 *   Result of the operation
 *
 * @param [in] a
 *   Operand a
 *
 * @param [in] m
 *   Modulo
 *
 * @param [in] length
 *   Length of the result in bytes
 */

cx_err_t cx_math_invintm_no_throw(uint8_t *r, uint32_t a, const uint8_t *m, size_t len);

static inline void cx_math_invintm(uint8_t *r, uint32_t a, const uint8_t *m, size_t len) {
  CX_THROW(cx_math_invintm_no_throw(r, a, m, len));
}

/**
 * Is prime function
 *
 * @param [in] r
 *   Operand to be tested
 *
 * @param [in] length
 *   Length of the operand r in bytes
 *
 * @return
 *   a bool defining whether r is prime or not:
 *   0 : not prime
 *   1 : prime
 */

cx_err_t cx_math_is_prime_no_throw(const uint8_t *r, size_t len, bool *prime);

static inline bool cx_math_is_prime(const uint8_t *r, size_t len) {
  bool prime;
  CX_THROW(cx_math_is_prime_no_throw(r, len, &prime));
  return prime;
}

/**
 * Next prime function
 *
 * @param [in] r
 *   Operand to be tested (and also the result)
 *
 * @param [in] length
 *   Length of the operand r in bytes
 *
 */

cx_err_t cx_math_next_prime_no_throw(uint8_t *r, uint32_t len);

static inline void cx_math_next_prime(uint8_t *r, uint32_t len) {
  CX_THROW(cx_math_next_prime_no_throw(r, len));
}

/**
 * Is zero function
 *
 * @param [in] a
 *   Buffer to be tested
 *
 * @param [in] length
 *   Length of the buffer a to test in bytes
 *
 * @return
 *   a bool defining whether a is all null or not:
 *   0 : not null
 *   1 : all null
 */

static inline bool cx_math_is_zero(const uint8_t *a, size_t len) {
  uint32_t i;
  for (i=0; i<len; i++) {
    if (a[i] != 0) {
      return 0;
    }
  }
  return 1;
}

#endif // LCX_MATH_H

#endif // HAVE_MATH
