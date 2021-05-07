
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
 * Include "ox.h" instead
 */

#ifndef OX_BN_H
#define OX_BN_H

#include "cx_errors.h"
#include "decorators.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// MUST BE A POWER OF 2
#define CX_BN_WORD_ALIGNEMENT 16

/* high level, public and common */

/* ========================================================================= */
/* ===                               TYPES                               === */
/* ========================================================================= */

/** To mark BN as unset/unused in wrapping struct, such as cx_ecpoint_t */
#define CX_BN_FLAG_UNSET 0x80

typedef uint32_t cx_bn_t;

typedef struct {
  /** @private */
  cx_bn_t n;
  /** @private */
  cx_bn_t h;
} cx_bn_mont_ctx_t;

/* ========================================================================= */
/* ===                              CONFIG                               === */
/* ========================================================================= */

/**
 * Acquire lock to BN processor, reset it, set the word size and config
 *
 * @param word_nbytes  word size in byte, each parameter will have a size
 * multiple of this size. This size shall be a multiple of
 * @param flags      logical OR of:
 *                     - CX_BN_SHUFFLE
 *                     - ?
 *
 * @throw  CX_BN_LOCKED if already locked
 */
SYSCALL cx_err_t cx_bn_lock(size_t word_nbytes, uint32_t flags);

/**
 * Release the BN lock and erase all content data
 *
 * This  function intentionnally does not throw exception
 *
 * @return  CX_OK if unlock success
 * @return  CX_BN_NOT_LOCKED is not loecked
 */
SYSCALL uint32_t cx_bn_unlock(void);

/**
 * Tell is BN is currently locked
 *
 * @return >0 if yes, 0 else
 */
SYSCALL bool cx_bn_is_locked(void);

/**
 * Ensure is BN is currently locked
 */
cx_err_t cx_bn_locked(void);

/* ========================================================================= */
/* ===                             ALLOCA                                === */
/* ========================================================================= */

/**
 * Alloc a new bn in the backend and init it to ZERO;
 *
 * The the lengh is the minimal requiered size, the effective size will be
 * round up to next slot-size multiple
 *
 * @param x        bn to alloc
 * @param nbytes     bytes size of x, must be a multiple of slot_size, (or auto
 * round-up?)
 *
 * @throw CX_BN_MEMORY_FULL
 * @throw CX_BN_INVALID_PARAMETER_SIZE
 */
SYSCALL cx_err_t cx_bn_alloc(cx_bn_t *x PLENGTH(sizeof(cx_bn_t)),
                             size_t nbytes);

/**
 * Alloc a new bn in the backend and init it with the specified value.
 *
 * The the lengh is the minimal requiered size, the effective size will be
 * round up to next slot-size multiple
 *
 * @param x        bn to alloc
 * @param nbytes   bytes size of x, must be a multiple of slot_size, (or auto
 * round-up?)
 *
 * @throw CX_BN_MEMORY_FULL
 * @throw CX_BN_INVALID_PARAMETER_SIZE
 */
SYSCALL cx_err_t cx_bn_alloc_init(cx_bn_t *x PLENGTH(sizeof(cx_bn_t)),
                                  size_t nbytes,
                                  const uint8_t *value PLENGTH(value_nbytes),
                                  size_t value_nbytes);

/**
 * Release a bn and give back his attached memory to the system.
 *
 * @param x        bn to release
 *
 * @throw CX_BN_INVALID_PARAMETER_SIZE
 * @throw CX_BN_INVALID_PARAMETER_VALUE
 */
SYSCALL cx_err_t cx_bn_destroy(cx_bn_t *x PLENGTH(sizeof(cx_bn_t)));

/**
 * Returns the size in bytes of bn
 *
 *  @param x        bn to release
 *
 * @throw  CX_BN_INVALID_PARAMETER
 */
SYSCALL cx_err_t cx_bn_nbytes(const cx_bn_t x, size_t *nbytes);

/* ========================================================================= */
/* ===                                R/W                                === */
/* ========================================================================= */
/**
 * Set the unsigned value of the bn
 *
 * @param x        bn to load
 * @param value    unsigned bug-endian value to set
 * @param value_nbytes   bytes length
 *
 * @throw CX_BN_INVALID_PARAMETER_SIZE
 * @throw CX_BN_INVALID_PARAMETER_VALUE
 */
SYSCALL cx_err_t cx_bn_init(cx_bn_t x,
                            const uint8_t *value PLENGTH(value_nbytes),
                            size_t value_nbytes);

/**
 * Randomize the whole bn.
 *
 * @param x        bn to randomize
 *
 * @throw CX_BN_INVALID_PARAMETER_VALUE
 */
SYSCALL cx_err_t cx_bn_rand(cx_bn_t x);

/**
 * Performs a = b
 *
 * @param a        destination
 * @param b        source
 *
 * @throw CX_BN_INVALID_PARAMETER_SIZE
 * @throw CX_BN_INVALID_PARAMETER_VALUE
 */
SYSCALL cx_err_t cx_bn_copy(cx_bn_t a, const cx_bn_t b);

/**
 * Performs x = n
 *
 * @param x        destination
 * @param n        source
 *
 * @return CX_OK on success, CX_NOT_LOCKED or CX_INVALID_PARAMETER on error
 */
SYSCALL cx_err_t cx_bn_set_u32(cx_bn_t x, uint32_t n);

/**
 * @param n        destination
 *
 * @return CX_OK on success, CX_NOT_LOCKED or CX_INVALID_PARAMETER on error
 */
SYSCALL cx_err_t cx_bn_get_u32(const cx_bn_t x, uint32_t *n);

/**
 * Store bn value as unsigned bigendian raw bytes.
 * The whole bn is serialize
 *
 * @param bytes    where to serialize
 * @param x        bn to serialize
 *
 * @return numer of bytes written
 *
 * @throw CX_BN_INVALID_PARAMETER_SIZE
 * @throw CX_BN_INVALID_PARAMETER_VALUE
 */
// int cx_bn_write_binary(uint8_t *bytes, cx_bn_t x);

/**
 * Store/Serialize bn value as unsigned big-endian raw bytes.
 *
 * Only the LSB nbytes of the bn are serialized
 * If nbytes is greater than bn size, x is serialiazed right aligned in x, and
 * zero left padded.
 *
 * @param bytes    where to serialize
 * @param nbytes   bytes length
 *
 * @return numer of bytes written
 *
 * @throw CX_BN_INVALID_PARAMETER_SIZE
 * @throw CX_BN_INVALID_PARAMETER_VALUE
 */
SYSCALL cx_err_t cx_bn_export(const cx_bn_t x, uint8_t *bytes PLENGTH(nbytes),
                              size_t nbytes);

/* ========================================================================= */
/* ===                               TEST                                === */
/* ========================================================================= */

/**
 *
 */
SYSCALL cx_err_t cx_bn_cmp(const cx_bn_t a, const cx_bn_t b, int *diff);

/**
 *
 */
SYSCALL cx_err_t cx_bn_cmp_u32(const cx_bn_t a, uint32_t b, int *diff);

/**
 *
 */
SYSCALL cx_err_t cx_bn_is_odd(const cx_bn_t n, bool *odd);

/* ========================================================================= */
/* ===                         BIT MANIPULATION                          === */
/* ========================================================================= */

/**
 *
 */
SYSCALL cx_err_t cx_bn_xor(cx_bn_t r, const cx_bn_t a, const cx_bn_t b);

/**
 *
 */
SYSCALL cx_err_t cx_bn_or(cx_bn_t r, const cx_bn_t a, const cx_bn_t b);

/**
 *
 */
SYSCALL cx_err_t cx_bn_and(cx_bn_t r, const cx_bn_t a, const cx_bn_t b);

/**
 *
 */
SYSCALL cx_err_t cx_bn_tst_bit(const cx_bn_t x, uint32_t pos, bool *set);

/**
 *
 */
SYSCALL cx_err_t cx_bn_set_bit(cx_bn_t x, uint32_t pos);

/**
 *
 */
SYSCALL cx_err_t cx_bn_clr_bit(cx_bn_t x, uint32_t pos);

/**
 *
 */
SYSCALL cx_err_t cx_bn_shr(cx_bn_t x, uint32_t n);

/**
 *
 */
SYSCALL cx_err_t cx_bn_shl(cx_bn_t x, uint32_t n);

/**
 *
 */
SYSCALL cx_err_t cx_bn_cnt_bits(cx_bn_t n, uint32_t *nbits);

/* ========================================================================= */
/* ===                      NON MODULAR ARITHMETIC                       === */
/* ========================================================================= */

/**
 * Compute r = a + b
 *
 * r,a and b shall have the same bn_size
 *
 * @Return Carry
 *
 * @throw CX_BN_INVALID_PARAMETER_SIZE
 * @throw CX_BN_INVALID_PARAMETER_VALUE
 */
SYSCALL cx_err_t cx_bn_add(cx_bn_t r, const cx_bn_t a, const cx_bn_t b);

/**
 * Compute r = a - b
 *
 * r,a and b shall have the same bn_size
 *
 * @Return Carry
 *
 * @throw CX_BN_INVALID_PARAMETER_SIZE
 * @throw CX_BN_INVALID_PARAMETER_VALUE
 */
SYSCALL cx_err_t cx_bn_sub(cx_bn_t r, const cx_bn_t a, const cx_bn_t b);

/**
 * Compute r = a * b
 *
 * a and b shall have the same bn_size
 * r shall have a size greater that twice the size of a (and so b)
 *
 *
 * @throw CX_BN_INVALID_PARAMETER_SIZE
 * @throw CX_BN_INVALID_PARAMETER_VALUE
 */
SYSCALL cx_err_t cx_bn_mul(cx_bn_t r, const cx_bn_t a, const cx_bn_t b);

/* ========================================================================= */
/* ===                     Z/pZ  MODULAR ARITHMETIC                      === */
/* ========================================================================= */

/**
 * Compute r = a + b mod n
 *
 * r,a b and n shall have the same bn_size
 *
 * @Return Carry
 *
 * @throw CX_BN_INVALID_PARAMETER_SIZE
 * @throw CX_BN_INVALID_PARAMETER_VALUE
 */
SYSCALL cx_err_t cx_bn_mod_add(cx_bn_t r, const cx_bn_t a, const cx_bn_t b,
                               const cx_bn_t n);

/**
 * Compute r = a - b mod n
 *
 * r,a, b and n shall have the same bn_size
 *
 * @Return Carry
 *
 * @throw CX_BN_INVALID_PARAMETER_SIZE
 * @throw CX_BN_INVALID_PARAMETER_VALUE
 */
SYSCALL cx_err_t cx_bn_mod_sub(cx_bn_t r, const cx_bn_t a, const cx_bn_t b,
                               const cx_bn_t n);

/**
 * Compute r = a * b mod n
 *
 * r,a, b and n shall have the same bn_size
 *
 * @Return Carry
 *
 * @throw CX_BN_INVALID_PARAMETER_SIZE
 * @throw CX_BN_INVALID_PARAMETER_VALUE
 */
SYSCALL cx_err_t cx_bn_mod_mul(cx_bn_t r, const cx_bn_t a, const cx_bn_t b,
                               const cx_bn_t n);

/**
 * Compute r = d mod n
 *
 * r, n shall have the same bn_size
 *
 * @Return Carry
 *
 * @throw CX_BN_INVALID_PARAMETER_SIZE
 * @throw CX_BN_INVALID_PARAMETER_VALUE
 */
SYSCALL cx_err_t cx_bn_reduce(cx_bn_t r, const cx_bn_t d, const cx_bn_t n);

/**
 * Compute r, such r² == a mod n
 *
 * r, a and n shall have the same bn_size
 *
 * @throw CX_BN_INVALID_PARAMETER_SIZE
 * @throw CX_BN_INVALID_PARAMETER_VALUE
 * @throw CX_BN_NO RESIDUE
 */
SYSCALL cx_err_t cx_bn_mod_sqrt(cx_bn_t bn_r, const cx_bn_t bn_a,
                                const cx_bn_t bn_n, uint32_t sign);

/**
 * Compute r = a pow e mod n
 *
 * r, a and n shall have the same bn_size
 *
 * @throw CX_BN_INVALID_PARAMETER_SIZE
 * @throw CX_BN_INVALID_PARAMETER_VALUE
 */
SYSCALL cx_err_t cx_bn_mod_pow_bn(cx_bn_t r, const cx_bn_t a, const cx_bn_t e,
                                  const cx_bn_t n);

/**
 * Compute r = a pow e mod n
 *
 * r, a and n shall have the same bn_size
 * r, a and n must be different
 *
 * @throw CX_BN_INVALID_PARAMETER_SIZE
 * @throw CX_BN_INVALID_PARAMETER_VALUE
 */
SYSCALL cx_err_t cx_bn_mod_pow(cx_bn_t r, const cx_bn_t a,
                               const uint8_t *e PLENGTH(e_len), uint32_t e_len,
                               const cx_bn_t n);

/**
 * Compute r = a pow e mod n
 *
 * This function requires less memory (and may avoid a CX_BN_MEMORY_FULL error).
 * It destroys the a value, as a is used in internally for intermediate
 * computation.
 *
 * r, a and n shall have the same bn_size
 * r, a and n must be different
 *
 * @throw CX_BN_INVALID_PARAMETER_SIZE
 * @throw CX_BN_INVALID_PARAMETER_VALUE
 */
SYSCALL cx_err_t cx_bn_mod_pow2(cx_bn_t r, const cx_bn_t a,
                                const uint8_t *e PLENGTH(e_len), uint32_t e_len,
                                const cx_bn_t n);

/**
 * Compute r = inv(a) mod n
 *
 * r,a and n shall have the same bn_size. n must be prime.
 *
 *
 * @throw CX_BN_INVALID_PARAMETER_SIZE
 * @throw CX_BN_INVALID_PARAMETER_VALUE
 * @throw CX_NOT_INVERTIBLE
 */
SYSCALL cx_err_t cx_bn_mod_invert_nprime(cx_bn_t r, const cx_bn_t a,
                                         const cx_bn_t n);

/**
 * Compute r = inv(a) mod n
 *
 * r,a and n shall have the same bn_size
 *
 * This method add some constraints on its parameters:
 *   - the parameters n is detroyed and contains zero after the function returns
 *   - the return invert may be negative, and a *plus n* corection must be
 * applied by the caller in that case
 *
 * @return 1 if correction +n must be done, zero else
 *
 * @throw CX_BN_INVALID_PARAMETER_SIZE
 * @throw CX_BN_INVALID_PARAMETER_VALUE
 * @throw CX_NOT_INVERTIBLE
 */
SYSCALL cx_err_t cx_bn_mod_u32_invert(cx_bn_t r, uint32_t a, cx_bn_t n);

/* ========================================================================= */
/* ===                   MONTGOMERY  MODULAR ARITHMETIC                  === */
/* ========================================================================= */

/**
 *
 */
SYSCALL cx_err_t cx_mont_alloc(
    cx_bn_mont_ctx_t *ctx PLENGTH(sizeof(cx_bn_mont_ctx_t)), size_t length);

/**
 *
 */
SYSCALL cx_err_t cx_mont_init(
    cx_bn_mont_ctx_t *ctx PLENGTH(sizeof(cx_bn_mont_ctx_t)), const cx_bn_t n);

/**
 *
 */
SYSCALL cx_err_t
cx_mont_init2(cx_bn_mont_ctx_t *ctx PLENGTH(sizeof(cx_bn_mont_ctx_t)),
              const cx_bn_t n, const cx_bn_t h);

/**
 *
 */
SYSCALL cx_err_t cx_mont_to_montgomery(cx_bn_t x, const cx_bn_t z,
                                       const cx_bn_mont_ctx_t *ctx
                                           PLENGTH(sizeof(cx_bn_mont_ctx_t)));

/**
 *
 */
SYSCALL cx_err_t cx_mont_from_montgomery(cx_bn_t z, const cx_bn_t x,
                                         const cx_bn_mont_ctx_t *ctx
                                             PLENGTH(sizeof(cx_bn_mont_ctx_t)));

/**
 *
 */
cx_err_t
cx_mont_mul(cx_bn_t r, const cx_bn_t a, const cx_bn_t b,
            const cx_bn_mont_ctx_t *ctx PLENGTH(sizeof(cx_bn_mont_ctx_t)));

/**
 *
 */
SYSCALL cx_err_t cx_mont_pow(
    cx_bn_t r, const cx_bn_t a, const uint8_t *e PLENGTH(e_len), uint32_t e_len,
    const cx_bn_mont_ctx_t *ctx PLENGTH(sizeof(cx_bn_mont_ctx_t)));

/**
 *
 */
SYSCALL cx_err_t
cx_mont_pow_bn(cx_bn_t r, const cx_bn_t a, const cx_bn_t e,
               const cx_bn_mont_ctx_t *ctx PLENGTH(sizeof(cx_bn_mont_ctx_t)));

/**
 *
 */
SYSCALL cx_err_t cx_mont_invert_nprime(cx_bn_t r, const cx_bn_t a,
                                       const cx_bn_mont_ctx_t *ctx
                                           PLENGTH(sizeof(cx_bn_mont_ctx_t)));

/* ========================================================================= */
/* ===                               PRIME                               === */
/* ========================================================================= */

/**
 *
 */
SYSCALL cx_err_t cx_bn_is_prime(const cx_bn_t n, bool *prime);

/**
 *
 */
SYSCALL cx_err_t cx_bn_next_prime(cx_bn_t n);

SYSCALL cx_err_t cx_bn_rng(cx_bn_t bn_r, const cx_bn_t bn_n);

#endif /* CX_BN_H */
