/**
 * @file    cx_errors.h
 * @brief   Error codes related to cryptography and arithmetic operations.
 */

#pragma once

#include <stdint.h>

/**
 * Check the error code of a function.
 * @hideinitializer
 */
#define CX_CHECK(call)                                                         \
  do {                                                                         \
    error = call;                                                              \
    if (error) {                                                               \
      goto end;                                                                \
    }                                                                          \
  } while (0)

/**
 * Check the error code of a function and ignore
 * it if CX_CARRY.
 * @hideinitializer
 */
#define CX_CHECK_IGNORE_CARRY(call)                                            \
  do {                                                                         \
    error = call;                                                              \
    if (error && error != CX_CARRY) {                                          \
      goto end;                                                                \
    }                                                                          \
  } while (0)

/** Success. */
#define CX_OK 0x00000000

/** There exists a carry at the end of the operation. */
#define CX_CARRY 0xFFFFFF21

/**
 *  Multi Precision Integer processor is locked:
 *  operations can be done.
 */
#define CX_LOCKED 0xFFFFFF81

/**
 * Multi Precision Integer processor is unlocked:
 * operations can't be done.
 */
#define CX_UNLOCKED 0xFFFFFF82

/**
 * Multi Precision Integer processor is not locked:
 * it cannot be unlocked.
 */
#define CX_NOT_LOCKED 0xFFFFFF83

/**
 *  Multi Precision Integer processor is already locked:
 *  it cannot be locked.
 */
#define CX_NOT_UNLOCKED 0xFFFFFF84

/** Internal error */
#define CX_INTERNAL_ERROR 0xFFFFFF85

/**
 * A parameter has an invalid size.
 */
#define CX_INVALID_PARAMETER_SIZE 0xFFFFFF86

/**
 * A parameter has an invalid value.
 * This error is returned if the given modulus is not odd.
 */
#define CX_INVALID_PARAMETER_VALUE 0xFFFFFF87

/** A parameter is invalid. */
#define CX_INVALID_PARAMETER 0xFFFFFF88

/**
 * A value is not invertible.
 */
#define CX_NOT_INVERTIBLE 0xFFFFFF89

/** A value overflow occurred. */
#define CX_OVERFLOW 0xFFFFFF8A

/** Memory is full: allocation is not possible anymore. */
#define CX_MEMORY_FULL 0xFFFFFF8B

/** A quadratic residue cannot be computed. */
#define CX_NO_RESIDUE 0xFFFFFF8C

/** Point at infinity is hit. */
#define CX_EC_INFINITE_POINT 0xFFFFFF41

/** Point is invalid: it does not belong to the curve. */
#define CX_EC_INVALID_POINT 0xFFFFFFA2

/** Curve is invalid. */
#define CX_EC_INVALID_CURVE 0xFFFFFFA3

/** Type of error code */
typedef uint32_t cx_err_t;
