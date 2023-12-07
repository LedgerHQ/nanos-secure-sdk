
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

/**
 * @file    ox_bn.h
 * @brief   Big Number syscalls.
 *
 * This file contains the big numbers definitions and functions:
 * - Lock the memory for further computations
 * - Unlock the memory at the end of the operations
 * - Arithmetic on big numbers
 */

#ifndef OX_BN_H
#define OX_BN_H

#include "decorators.h"
#include "cx_errors.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// MUST BE A POWER OF 2
/**
 * @brief   Minimal word size in bytes.
 *
 * @details A BN size shall be a multiple of this.
 */
#define CX_BN_WORD_ALIGNEMENT 16

/* high level, public and common */

/* ========================================================================= */
/* ===                               TYPES                               === */
/* ========================================================================= */

#define CX_BN_FLAG_UNSET 0x80

/** Index of a big number. */
typedef uint32_t cx_bn_t;

/**
 * @brief Montgomery context.
 */
typedef struct {
    cx_bn_t n;  ///< @private Modulus
    cx_bn_t h;  ///< @private Second Montgomery constant
} cx_bn_mont_ctx_t;

/* ========================================================================= */
/* ===                              CONFIG                               === */
/* ========================================================================= */

/**
 * @brief   Locks the BN processor.
 *
 * @details The memory is reset then the word size is set.
 *          Once locked the memory can be used.
 *
 * @param[in] word_nbytes  Word size in byte, the size of the parameters will be a multiple
 *                         of *word_nbytes*.
 *                         This size must be a multiple of CX_BN_WORD_ALIGNEMENT.
 * @param[in] flags        Flags.
 *
 * @return                 Error code:
 *                         - CX_OK on success
 *                         - CX_BN_LOCKED if already locked.
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t cx_bn_lock(size_t word_nbytes, uint32_t flags);

/**
 * @brief   Releases the BN lock.
 *
 * @details It erases all content data.
 *          Once unlocked the memory cannot be used anymore.
 *
 * @return  Error code:
 *          - CX_OK on success
 *          - CX_BN_NOT_LOCKED if not locked
 */
SYSCALL uint32_t cx_bn_unlock(void);

/**
 * @brief   Checks whether the BN processor is currently locked.
 *
 * @details The memory can be used only if the BN processor is locked.
 *
 * @return  1 if locked, 0 otherwise.
 */
SYSCALL bool cx_bn_is_locked(void);

/**
 * @brief   Ascertains whether the BN processor is currently locked.
 *
 * @details If the BN processor is not locked the memory
 *          cannot be used.
 *
 * @return  Error code:
 *         - CX_OK on success
 *         - CX_NOT_LOCKED
 */
WARN_UNUSED_RESULT cx_err_t cx_bn_locked(void);

/* ========================================================================= */
/* ===                             ALLOCA                                === */
/* ========================================================================= */

/**
 * @brief   Allocates memory for a new BN.
 *
 * @details The specified number of bytes is the minimal required bytes,
 *          the number of words allocated will be automatically a multiple
 *          of the configured word size. At this moment the BN value is set
 *          to 0.
 *
 * @param[in] x        Pointer to a BN.
 *
 * @param[in] nbytes   Number of bytes of x.
 *
 * @return             Error code:
 *                     - CX_OK on success
 *                     - CX_BN_MEMORY_FULL
 *                     - CX_BN_INVALID_PARAMETER_SIZE
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t cx_bn_alloc(cx_bn_t *x PLENGTH(sizeof(cx_bn_t)), size_t nbytes);

/**
 * @brief   Allocates memory for a new BN and
 *          initializes it with the specified value.
 *
 * @details The specified number of bytes is the minimal required bytes,
 *          the number of words allocated will be automatically a multiple
 *          of the configured word size.
 *
 * @param[in] x            Pointer to a BN.
 *
 * @param[in] nbytes       Number of bytes of x.
 *
 * @param[in] value        Pointer to the value used to initialize the BN.
 *
 * @param[in] value_nbytes Number of bytes of value.
 *
 * @return                 Error code:
 *                         - CX_OK on success
 *                         - CX_BN_MEMORY_FULL
 *                         - CX_BN_INVALID_PARAMETER_SIZE
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t cx_bn_alloc_init(cx_bn_t *x           PLENGTH(sizeof(cx_bn_t)),
                                                     size_t               nbytes,
                                                     const uint8_t *value PLENGTH(value_nbytes),
                                                     size_t               value_nbytes);

/**
 * @brief   Releases a BN and gives back its attached memory to the system.
 *
 * @param[in] x BN to release. If NULL, nothing is done.
 *
 * @return      Error code:
 *              - CX_OK on success
 *              - CX_BN_INVALID_PARAMETER_SIZE
 *              - CX_BN_INVALID_PARAMETER_VALUE
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t cx_bn_destroy(cx_bn_t *x PLENGTH(sizeof(cx_bn_t)));

/**
 * @brief   Gets the size in bytes of a BN.
 *
 * @param[in]  x      BN index.
 *
 * @param[out] nbytes Returned number of bytes.
 *
 * @return            Error code:
 *                    - CX_OK on success
 *                    - CX_BN_INVALID_PARAMETER
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t cx_bn_nbytes(const cx_bn_t x, size_t *nbytes);

/* ========================================================================= */
/* ===                                R/W                                === */
/* ========================================================================= */
/**
 * @brief   Iinitializes a BN with an unsigned value.
 *
 * @param[in] x              BN index.
 *
 * @param[in] value          Pointer to the value in big-endian order.
 *
 * @param[in] value_nbytes   Number of bytes of the value.
 *
 * @return                   Error code:
 *                           - CX_OK on success
 *                           - CX_BN_INVALID_PARAMETER_SIZE
 *                           - CX_BN_INVALID_PARAMETER_VALUE
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t cx_bn_init(cx_bn_t              x,
                                               const uint8_t *value PLENGTH(value_nbytes),
                                               size_t               value_nbytes);

/**
 * @brief   Generates a random number and
 *          stores it in the given index.
 *
 * @param[in] x BN index.
 *
 * @return      Error code:
 *              - CX_OK on success
 *              - CX_BN_INVALID_PARAMETER_VALUE
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t cx_bn_rand(cx_bn_t x);

/**
 * @brief   Copies the BN value.
 *
 * @param[out] a BN destination index.
 *
 * @param[in]  b BN source index.
 *
 * @return       Error code:
 *               - CX_OK on success
 *               - CX_BN_INVALID_PARAMETER_SIZE
 *               - CX_BN_INVALID_PARAMETER_VALUE
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t cx_bn_copy(cx_bn_t a, const cx_bn_t b);

/**
 * @brief   Sets the value of a BN with a 32-bit unsigned value.
 *
 * @param[in] x BN index.
 *
 * @param[in] n 32-bit value to be assigned.
 *
 * @return      Error code:
 *              - CX_OK on success
 *              - CX_NOT_LOCKED
 *              - CX_INVALID_PARAMETER
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t cx_bn_set_u32(cx_bn_t x, uint32_t n);

/**
 * @brief   Gets the 32-bit value corresponding to a BN.
 *
 * @param[in]  x BN index.
 *
 * @param[out] n Stored 32-bit unsigned value.
 *
 * @return       Error code:
 *               - CX_OK on success
 *               - CX_NOT_LOCKED
 *               - CX_INVALID_PARAMETER
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t cx_bn_get_u32(const cx_bn_t x, uint32_t *n);

/**
 * @brief   Stores (serializes) a BN value as
 *          unsigned raw bytes in big-endian order.
 *
 * @details Only the least significant *nbytes* bytes of the BN are serialized
 *          If *nbytes* is greater than the BN size, *x* is serialized right aligned
 *          and zero left-padded.
 *
 * @param[in]  x       BN index.
 *
 * @param[out] bytes   Buffer where to store the serialized number.
 *
 * @param[in]  nbytes  Number of bytes to store into the buffer.
 *
 * @return             Error code:
 *                     - CX_OK on success
 *                     - CX_BN_INVALID_PARAMETER_SIZE
 *                     - CX_BN_INVALID_PARAMETER_VALUE
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t cx_bn_export(const cx_bn_t  x,
                                                 uint8_t *bytes PLENGTH(nbytes),
                                                 size_t         nbytes);

/* ========================================================================= */
/* ===                               TEST                                === */
/* ========================================================================= */

/**
 * @brief   Compares two BN values.
 *
 * @param[in]  a    BN index to the first value to be compared.
 *
 * @param[in]  b    BN index to the second value to be compared.
 *
 * @param[out] diff Result of the comparison:
 *                  - 0 if the numbers are equal.
 *                  - > 0 if the first number is greater than the second
 *                  - < 0 if the first number is smaller than the second
 *
 * @return          Error code:
 *                  - CX_OK on success
 *                  - CX_NOT_LOCKED
 *                  - CX_INVALID_PARAMETER
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t cx_bn_cmp(const cx_bn_t a, const cx_bn_t b, int *diff);

/**
 * @brief   Compares a BN value with an unsigned integer.
 *
 * @param[in]  a    BN index to the value to be compared.
 *
 * @param[in]  b    Integer to be compared.
 *
 * @param[out] diff Result of the comparison:
 *                  - 0 if the numbers are equal.
 *                  - > 0 if the BN value is greater
 *                  - < 0 if the BN value is smaller
 *
 * @return          Error code:
 *                  - CX_OK on success
 *                  - CX_NOT_LOCKED
 *                  - CX_INVALID_PARAMETER
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t cx_bn_cmp_u32(const cx_bn_t a, uint32_t b, int *diff);

/**
 * @brief   Tests whether a BN value is odd.
 *
 * @param[in]  n    BN index.
 *
 * @param[out] odd  Boolean which indicates the parity of the BN value:
 *                  - 1 if odd
 *                  - 0 if even
 *
 * @return          Error code:
 *                  - CX_OK on success
 *                  - CX_NOT_LOCKED
 *                  - CX_INVALID_PARAMETER
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t cx_bn_is_odd(const cx_bn_t n, bool *odd);

/* ========================================================================= */
/* ===                         BIT MANIPULATION                          === */
/* ========================================================================= */

/**
 * @brief   Performs the bitwise 'exclusive-OR' of two BN values.
 *
 * @details *r* must be distinct from *a* and *b*.
 *
 * @param[out] r BN index for the result.
 *
 * @param[in]  a BN index of the first operand.
 *
 * @param[in]  b BN index of the second operand.
 *
 * @return       Error code:
 *               - CX_OK on success
 *               - CX_NOT_LOCKED
 *               - CX_INVALID_PARAMETER
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t cx_bn_xor(cx_bn_t r, const cx_bn_t a, const cx_bn_t b);

/**
 * @brief   Performs the bitwise 'OR' of two BN values.
 *
 * @details *r* must be distinct from *a* and *b*.
 *
 * @param[out] r BN index for the result.
 *
 * @param[in]  a BN index of the first operand.
 *
 * @param[in]  b BN index of the second operand.
 *
 * @return       Error code:
 *               - CX_OK on success
 *               - CX_NOT_LOCKED
 *               - CX_INVALID_PARAMETER
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t cx_bn_or(cx_bn_t r, const cx_bn_t a, const cx_bn_t b);

/**
 * @brief   Performs the bitwise 'AND' of two BN values.
 *
 * @details *r* must be distinct from *a* and *b*.
 *
 * @param[out] r BN index for the result.
 *
 * @param[in]  a BN index of the first operand.
 *
 * @param[in]  b BN index of the second operand.
 *
 * @return       Error code:
 *               - CX_OK on success
 *               - CX_NOT_LOCKED
 *               - CX_INVALID_PARAMETER
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t cx_bn_and(cx_bn_t r, const cx_bn_t a, const cx_bn_t b);

/**
 * @brief   Tests the bit value at the specified index.
 *
 * @details The BN value is in big endian order, thus the
 *          position 0 corresponds to the least significant bit.
 *
 * @param[in]  x    BN index.
 *
 * @param[in]  pos  Position of the bit.
 *
 * @param[out] set  Boolean which indicates the bit value
 *                  - 1 if the bit is set
 *                  - 0 otherwise
 *
 * @return          Error code:
 *                  - CX_OK on success
 *                  - CX_NOT_LOCKED
 *                  - CX_INVALID_PARAMETER
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t cx_bn_tst_bit(const cx_bn_t x, uint32_t pos, bool *set);

/**
 * @brief   Sets the bit value at the specified index.
 *
 * @details The BN value is in big endian order, thus the
 *          position 0 corresponds to the least significant bit.
 *
 * @param[in]  x    BN index.
 *
 * @param[in]  pos  Position of the bit.
 *
 * @return          Error code:
 *                  - CX_OK on success
 *                  - CX_NOT_LOCKED
 *                  - CX_INVALID_PARAMETER
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t cx_bn_set_bit(cx_bn_t x, uint32_t pos);

/**
 * @brief   Clears the bit value at the specified index.
 *
 * @details The BN value is in big endian order, thus the
 *          position 0 corresponds to the least significant bit.
 *
 * @param[in]  x    BN index.
 *
 * @param[in]  pos  Position of the bit.
 *
 * @return          Error code:
 *                  - CX_OK on success
 *                  - CX_NOT_LOCKED
 *                  - CX_INVALID_PARAMETER
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t cx_bn_clr_bit(cx_bn_t x, uint32_t pos);

/**
 * @brief   Performs a right shift.
 *
 * @param[in]  x  BN index.
 *
 * @param[in]  n  Number of bits to shift.
 *
 * @return        Error code:
 *                - CX_OK on success
 *                - CX_NOT_LOCKED
 *                - CX_INVALID_PARAMETER
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t cx_bn_shr(cx_bn_t x, uint32_t n);

/**
 * @brief   Performs a left shift.
 *
 * @param[in]  x  BN index.
 *
 * @param[in]  n  Number of bits to shift.
 *
 * @return        Error code:
 *                - CX_OK on success
 *                - CX_NOT_LOCKED
 *                - CX_INVALID_PARAMETER
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t cx_bn_shl(cx_bn_t x, uint32_t n);

/**
 * @brief   Counts the number of bits set to 1 of the BN value.
 *
 * @param[in]  n      BN index.
 *
 * @param[out] nbits  Number of bits set.
 *
 * @return            Error code:
 *                    - CX_OK on success
 *                    - CX_NOT_LOCKED
 *                    - CX_INVALID_PARAMETER
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t cx_bn_cnt_bits(cx_bn_t n, uint32_t *nbits);

/* ========================================================================= */
/* ===                      NON MODULAR ARITHMETIC                       === */
/* ========================================================================= */

/**
 * @brief   Performs an addition **r = a + b**.
 *
 * @details *r*, *a* and *b* shall have the same BN size.
 *
 * @param[out] r BN index for the result.
 *
 * @param[in]  a BN index of the first operand.
 *
 * @param[in]  b BN index of the second operand.
 *
 * @return       Error code:
 *               - CX_OK or CX_CARRY on success
 *               - CX_NOT_LOCKED
 *               - CX_INVALID_PARAMETER
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t cx_bn_add(cx_bn_t r, const cx_bn_t a, const cx_bn_t b);

/**
 * @brief   Performs a subtraction **r = a - b**.
 *
 * @details *r*, *a* and *b* shall have the same BN size.
 *
 * @param[out] r BN index for the result.
 *
 * @param[in]  a BN index of the first operand.
 *
 * @param[in]  b BN index of the second operand.
 *
 * @return       Error code:
 *               - CX_OK or CX_CARRY on success
 *               - CX_NOT_LOCKED
 *               - CX_INVALID_PARAMETER
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t cx_bn_sub(cx_bn_t r, const cx_bn_t a, const cx_bn_t b);

/**
 * @brief   Performs a multiplication **r = a * b**.
 *
 * @details *a* and *b* shall have the same BN size.
 *          The size of *r* must be the size of *a*
 *          + the size of *b*.
 *
 * @param[out] r BN index for the result.
 *
 * @param[in]  a BN index of the first operand.
 *
 * @param[in]  b BN index of the second operand.
 *
 * @return       Error code:
 *               - CX_OK on success
 *               - CX_NOT_LOCKED
 *               - CX_INVALID_PARAMETER
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t cx_bn_mul(cx_bn_t r, const cx_bn_t a, const cx_bn_t b);

/* ========================================================================= */
/* ===                     Z/pZ  MODULAR ARITHMETIC                      === */
/* ========================================================================= */

/**
 * @brief   Performs a modular addition **r = a + b mod n**.
 *
 * @details *r*, *a*, *b* and *n* shall have the same BN size.
 *          The values of *a* and *b* must be strictly smaller
 *          than modulus value.
 *
 * @param[out] r BN index for the result.
 *
 * @param[in]  a BN index of the first operand.
 *
 * @param[in]  b BN index of the second operand.
 *
 * @param[in]  n BN index of the modulus.
 *
 * @return       Error code:
 *               - CX_OK on success
 *               - CX_NOT_LOCKED
 *               - CX_INVALID_PARAMETER
 *               - CX_MEMORY_FULL
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t cx_bn_mod_add(cx_bn_t       r,
                                                  const cx_bn_t a,
                                                  const cx_bn_t b,
                                                  const cx_bn_t n);

/**
 * @brief   Performs a modular subtraction **r = a - b mod n**.
 *
 * @details *r*, *a*, *b* and *n* shall have the same BN size.
 *          The values of *a* and *b* must be strictly smaller
 *          than modulus value.
 *
 * @param[out] r BN index for the result.
 *
 * @param[in]  a BN index of the first operand.
 *
 * @param[in]  b BN index of the second operand.
 *
 * @param[in]  n BN index of the modulus.
 *
 * @return       Error code:
 *               - CX_OK on success
 *               - CX_NOT_LOCKED
 *               - CX_INVALID_PARAMETER
 *               - CX_MEMORY_FULL
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t cx_bn_mod_sub(cx_bn_t       r,
                                                  const cx_bn_t a,
                                                  const cx_bn_t b,
                                                  const cx_bn_t n);

/**
 * @brief   Performs a modular multiplication **r = a * b mod n**.
 *
 * @details *r*, *a*, *b* and *n* shall have the same BN size.
 *          The value of b must be strictly smaller
 *          than modulus value.
 *
 * @param[out] r BN index for the result.
 *
 * @param[in]  a BN index of the first operand.
 *
 * @param[in]  b BN index of the second operand.
 *
 * @param[in]  n BN index of the modulus.
 *
 * @return       Error code:
 *               - CX_OK on success
 *               - CX_NOT_LOCKED
 *               - CX_INVALID_PARAMETER
 *               - CX_INVALID_PARAMETER_VALUE
 *               - CX_MEMORY_FULL
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t cx_bn_mod_mul(cx_bn_t       r,
                                                  const cx_bn_t a,
                                                  const cx_bn_t b,
                                                  const cx_bn_t n);

/**
 * @brief   Performs a reduction **r = d mod n**.
 *
 * @details *r* and *n* shall have the same BN size.
 *
 * @param[out] r BN index for the result.
 *
 * @param[in]  d BN index of the value to be reduced.
 *
 * @param[in]  n BN index of the modulus.
 *
 * @return       Error code:
 *               - CX_OK on success
 *               - CX_NOT_LOCKED
 *               - CX_INVALID_PARAMETER
 *               - CX_MEMORY_FULL
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t cx_bn_reduce(cx_bn_t r, const cx_bn_t d, const cx_bn_t n);

/**
 * @brief   Computes *r* such that **r² = a mod n** if *a* is a quadratic residue.
 *
 * @details This returns an error if the given number is not a quadratic residue.
 *          *r*, *a* and *n* shall have the same BN size.
 *
 * @param[out] r    BN index for the result.
 *
 * @param[in]  a    BN index of the quadratic residue or quadratic non residue.
 *
 * @param[in]  n    BN index of the modulus.
 *
 * @param[in]  sign Sign of the result.
 *
 * @return          Error code:
 *                  - CX_OK on success
 *                  - CX_NOT_LOCKED
 *                  - CX_INVALID_PARAMETER
 *                  - CX_MEMORY_FULL
 *                  - CX_NO_RESIDUE
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t cx_bn_mod_sqrt(cx_bn_t       r,
                                                   const cx_bn_t a,
                                                   const cx_bn_t n,
                                                   uint32_t      sign);

/**
 * @brief   Performs a modular exponentiation **r = a^e mod n**.
 *
 * @details *r*, *a* and *n* shall have the same BN size.
 *          *r*, *a* and *n* must be different.
 *
 * @param[out] r BN index for the result.
 *
 * @param[in]  a BN index of the base of the exponentiation.
 *
 * @param[in]  e BN index of the exponent.
 *
 * @param[in]  n BN index of the modulus.
 *
 * @return       Error code:
 *               - CX_OK on success
 *               - CX_NOT_LOCKED
 *               - CX_INVALID_PARAMETER
 *               - CX_MEMORY_FULL
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t cx_bn_mod_pow_bn(cx_bn_t       r,
                                                     const cx_bn_t a,
                                                     const cx_bn_t e,
                                                     const cx_bn_t n);

/**
 * @brief   Performs a modular exponentiation **r = a^e mod n**.
 *
 * @details *r*, *a* and *n* shall have the same BN size.
 *          *r*, *a* and *n* must be different.
 *
 * @param[out] r     BN index for the result.
 *
 * @param[in]  a     BN index of the base of the exponentiation.
 *
 * @param[in]  e     Pointer to the exponent.
 *
 * @param[in]  e_len Length of the exponent buffer.
 *
 * @param[in]  n     BN index of the modulus.
 *
 * @return           Error code:
 *                   - CX_OK on success
 *                   - CX_NOT_LOCKED
 *                   - CX_INVALID_PARAMETER
 *                   - CX_MEMORY_FULL
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t cx_bn_mod_pow(cx_bn_t          r,
                                                  const cx_bn_t    a,
                                                  const uint8_t *e PLENGTH(e_len),
                                                  uint32_t         e_len,
                                                  const cx_bn_t    n);

/**
 * @brief   Performs a modular exponentiation **r = a^e mod n**.
 *
 * @details This function reuses the parameter *a* for intermediate computations,
 *          hence requires less memory.
 *          *r*, *a* and *n* shall have the same BN size.
 *          *r*, *a* and *n* must be different.
 *
 * @param[out] r     BN index for the result.
 *
 * @param[in]  a     BN index of the base of the exponentiation.
 *                   The BN value is modified during the
 *                   computations.
 *
 * @param[in]  e     Pointer to the exponent.
 *
 * @param[in]  e_len Length of the exponent buffer.
 *
 * @param[in]  n     BN index of the modulus.
 *
 * @return           Error code:
 *                   - CX_OK on success
 *                   - CX_NOT_LOCKED
 *                   - CX_INVALID_PARAMETER
 *                   - CX_MEMORY_FULL
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t cx_bn_mod_pow2(cx_bn_t          r,
                                                   const cx_bn_t    a,
                                                   const uint8_t *e PLENGTH(e_len),
                                                   uint32_t         e_len,
                                                   const cx_bn_t    n);

/**
 * @brief   Computes the modular inverse **r = a^(-1) mod n**,
 *          for a prime *n*.
 *
 * @details *r*, *a* and *n* shall have the same BN size. *n* must be prime.
 *
 * @param[out] r BN index for the result.
 *
 * @param[in]  a BN index of the value to be inverted.
 *
 * @param[in]  n BN index of the modulus.
 *
 * @return       Error code:
 *               - CX_OK on success
 *               - CX_NOT_LOCKED
 *               - CX_INVALID_PARAMETER
 *               - CX_MEMORY_FULL
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t cx_bn_mod_invert_nprime(cx_bn_t       r,
                                                            const cx_bn_t a,
                                                            const cx_bn_t n);

/**
 * @brief   Computes the modular inverse **r = a^(-1) mod n**,
 *          of a 32-bit value.
 *
 * @details *r* and *n* shall have the same BN size.
 *          The parameter *n* is destroyed and contains zero
 *          after the function returns.
 *
 * @param[out] r BN index for the result.
 *
 * @param[in]  a 32-bit value to be inverted.
 *
 * @param[in]  n BN index of the modulus.
 *
 * @return       Error code:
 *               - CX_OK on success
 *               - CX_NOT_LOCKED
 *               - CX_INVALID_PARAMETER
 *               - CX_MEMORY_FULL
 *               - CX_INTERNAL_ERROR
 *               - CX_NOT_INVERTIBLE
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t cx_bn_mod_u32_invert(cx_bn_t r, uint32_t a, cx_bn_t n);

/* ========================================================================= */
/* ===                   MONTGOMERY  MODULAR ARITHMETIC                  === */
/* ========================================================================= */

/**
 * @brief   Allocates memory for the Montgomery context.
 *
 * @param[in] ctx    Pointer to the Montgomery context.
 *
 * @param[in] length BN size for the context fields.
 *
 * @return           Error code:
 *                   - CX_OK on success
 *                   - CX_NOT_LOCKED
 *                   - CX_MEMORY_FULL
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t
cx_mont_alloc(cx_bn_mont_ctx_t *ctx PLENGTH(sizeof(cx_bn_mont_ctx_t)), size_t length);

/**
 * @brief   Initializes a Montgomery context with the modulus.
 *
 * @details Calculate and set up the second Montgomery constant.
 *
 * @param[in] ctx Pointer to a Montgomery context.
 *
 * @param[in] n   BN index of the modulus.
 *
 * @return        Error code:
 *                - CX_OK on success
 *                - CX_NOT_LOCKED
 *                - CX_INVALID_PARAMETER
 *                - CX_MEMORY_FULL
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t
cx_mont_init(cx_bn_mont_ctx_t *ctx PLENGTH(sizeof(cx_bn_mont_ctx_t)), const cx_bn_t n);

/**
 * @brief   Initializes a Montgomery context with the modulus and
 *          the second Montgomery constant.
 *
 * @details Set up the second Montgomery constant with the given
 *          parameter.
 *          The caller should make sure that the given
 *          second Montgomery constant is correct.
 *
 * @param[in] ctx Pointer to a Montgomery context.
 *
 * @param[in] n   BN index of the modulus.
 *
 * @param[in] h   BN index of the pre calculated second Montgomery constant.
 *
 * @return        Error code:
 *                - CX_OK on success
 *                - CX_NOT_LOCKED
 *                - CX_INVALID_PARAMETER
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t
cx_mont_init2(cx_bn_mont_ctx_t *ctx PLENGTH(sizeof(cx_bn_mont_ctx_t)),
              const cx_bn_t         n,
              const cx_bn_t         h);

/**
 * @brief   Computes the Montgomery representation of a BN value.
 *
 * @details The context must be initialized.
 *
 * @param[out] x   BN index for the result.
 *
 * @param[in]  z   BN index of the value to convert into Montgomery representation.
 *
 * @param[in]  ctx Pointer to the Montgomery context, initialized
 *                 with the modulus and the second Montgomery constant.
 *
 * @return         Error code:
 *                 - CX_OK on success
 *                 - CX_NOT_LOCKED
 *                 - CX_INVALID_PARAMETER
 *                 - CX_MEMORY_FULL
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t
cx_mont_to_montgomery(cx_bn_t                     x,
                      const cx_bn_t               z,
                      const cx_bn_mont_ctx_t *ctx PLENGTH(sizeof(cx_bn_mont_ctx_t)));

/**
 * @brief   Computes the normal representation of a BN value
 *          given a Montgomery representation.
 *
 * @details The context must be initialized.
 *
 * @param[out] x   BN index for the result.
 *
 * @param[in]  z   BN index of the value to be converted.
 *                 The value should be in Montgomery representation.
 *
 * @param[in]  ctx Pointer to the Montgomery context, initialized
 *                 with the modulus and the second Montgomery constant.
 *
 * @return         Error code:
 *                 - CX_OK on success
 *                 - CX_NOT_LOCKED
 *                 - CX_INVALID_PARAMETER
 *                 - CX_MEMORY_FULL
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t
cx_mont_from_montgomery(cx_bn_t                     z,
                        const cx_bn_t               x,
                        const cx_bn_mont_ctx_t *ctx PLENGTH(sizeof(cx_bn_mont_ctx_t)));

/**
 * @brief   Performs a Montgomery multiplication.
 *
 * @details The context must be initialized.
 *
 * @param[out] r   BN index for the result.
 *
 * @param[in]  a   BN index of the first operand in Montgomery representation.
 *
 * @param[in]  b   BN index of the second operand in Montgomery representation.
 *
 * @param[in]  ctx Pointer to the Montgomery context, initialized
 *                 with the modulus and the second Montgomery constant.
 *
 * @return         Error code:
 *                 - CX_OK on success
 *                 - CX_NOT_LOCKED
 *                 - CX_INVALID_PARAMETER
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t
cx_mont_mul(cx_bn_t                     r,
            const cx_bn_t               a,
            const cx_bn_t               b,
            const cx_bn_mont_ctx_t *ctx PLENGTH(sizeof(cx_bn_mont_ctx_t)));

/**
 * @brief   Performs a modular exponentiation **r = a^e mod n**.
 *
 * @details The context must be initialized. The BN value *a*
 *          is in Montgomery representation.
 *
 * @param[out] r     BN index for the result. The result is in
 *                   Montgomery representation.
 *
 * @param[in]  a     BN index of the exponentiation base in Montgomery
 *                   representation.
 *
 * @param[in]  e     Pointer to the exponent.
 *
 * @param[in]  e_len Length of the exponent in bytes.
 *
 * @param[in]  ctx   Pointer to the Montgomery context, initialized
 *                   with the modulus and the second Montgomery constant.
 *
 * @return           Error code:
 *                   - CX_OK on success
 *                   - CX_NOT_LOCKED
 *                   - CX_INVALID_PARAMETER
 *                   - CX_MEMORY_FULL
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t
cx_mont_pow(cx_bn_t                     r,
            const cx_bn_t               a,
            const uint8_t *e            PLENGTH(e_len),
            uint32_t                    e_len,
            const cx_bn_mont_ctx_t *ctx PLENGTH(sizeof(cx_bn_mont_ctx_t)));

/**
 * @brief   Performs a modular exponentiation **r = a^e mod n**.
 *
 * @details The context must be initialized. The BN value *a*
 *          is in Montgomery representation.
 *
 * @param[out] r     BN index for the result. The result is in
 *                   Montgomery representation.
 *
 * @param[in]  a     BN index of the exponentiation base in Montgomery
 *                   representation.
 *
 * @param[in]  e     BN index of the exponent.
 *
 * @param[in]  ctx   Pointer to the Montgomery context, initialized
 *                   with the modulus and the second Montgomery constant.
 *
 * @return           Error code:
 *                   - CX_OK on success
 *                   - CX_NOT_LOCKED
 *                   - CX_INVALID_PARAMETER
 *                   - CX_MEMORY_FULL
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t
cx_mont_pow_bn(cx_bn_t                     r,
               const cx_bn_t               a,
               const cx_bn_t               e,
               const cx_bn_mont_ctx_t *ctx PLENGTH(sizeof(cx_bn_mont_ctx_t)));

/**
 * @brief   Computes the modular inverse **r = a^(-1) mod n** for
 *          a prime number *n*.
 *
 * @details The context must be initialized.
 *
 * @param[out] r   BN index for the result. The result is in
 *                 Montgomery representation.
 *
 * @param[in]  a   BN index of the value to be inverted. The value
 *                 is in Montgomery representation.
 *
 * @param[in]  ctx Pointer to the Montgomery context, initialized
 *                 with the modulus and the second Montgomery constant
 *
 * @return         Error code:
 *                 - CX_OK on success
 *                 - CX_NOT_LOCKED
 *                 - CX_INVALID_PARAMETER
 *                 - CX_MEMORY_FULL
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t
cx_mont_invert_nprime(cx_bn_t                     r,
                      const cx_bn_t               a,
                      const cx_bn_mont_ctx_t *ctx PLENGTH(sizeof(cx_bn_mont_ctx_t)));

/* ========================================================================= */
/* ===                               PRIME                               === */
/* ========================================================================= */

/**
 * @brief   Tests whether a BN value is a probable prime.
 *
 * @param[in] n      BN index of the value.
 *
 * @param[out] prime Boolean which indicates whether the number is a prime:
 *                   - 1 if it is a prime
 *                   - 0 otherwise
 *
 * @return           Error code:
 *                   - CX_OK on success
 *                   - CX_NOT_LOCKED
 *                   - CX_INVALID_PARAMETER
 *                   - CX_MEMORY_FULL
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t cx_bn_is_prime(const cx_bn_t n, bool *prime);

/**
 * @brief   Gets the first prime number after a
 *          given BN value.
 *
 * @param[in, out] n BN index of the value and the result.
 *
 * @return           Error code:
 *                   - CX_OK on success
 *                   - CX_NOT_LOCKED
 *                   - CX_INVALID_PARAMETER
 *                   - CX_MEMORY_FULL
 *                   - CX_OVERFLOW
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t cx_bn_next_prime(cx_bn_t n);

/**
 * @brief Generates a random number *r* in the range ]0,n[.
 *
 * @details *r* is such that: **0 < r < n**.
 *
 * @param[out] r BN index for the result.
 *
 * @param[in]  n BN index of the upper bound.
 *
 * @return       Error code:
 *               - CX_OK on success
 *               - CX_NOT_LOCKED
 *               - CX_INVALID_PARAMETER
 *               - CX_MEMORY_FULL
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t cx_bn_rng(cx_bn_t r, const cx_bn_t n);

/**
 * @brief Performs a multiplication over GF(2^n).
 *
 * @param[out] bn_r BN index for the result.
 *
 * @param[in]  bn_a BN index of the first operand.
 *
 * @param[in]  bn_b BN index of the second operand.
 *
 * @param[in]  bn_n BN index of the modulus.
 *                  The modulus must be an irreducible polynomial over GF(2)
 *                  of degree n.
 *
 * @param[in]  bn_h BN index of the second montgomery constant.
 *
 * @return          Error code:
 *                  - CX_OK on success
 *                  - CX_NOT_LOCKED
 *                  - CX_INVALID_PARAMETER
 *                  - CX_MEMORY_FULL
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t cx_bn_gf2_n_mul(cx_bn_t       bn_r,
                                                    const cx_bn_t bn_a,
                                                    const cx_bn_t bn_b,
                                                    const cx_bn_t bn_n,
                                                    const cx_bn_t bn_h);

#endif /* CX_BN_H */
