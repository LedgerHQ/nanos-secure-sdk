
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

#ifdef HAVE_RNG

#ifndef LCX_RNG_H
#define LCX_RNG_H

#include "lcx_wrappers.h"
#include "lcx_hash.h"

/**
 * Generate a random buffer, each bytes between 0 and 255*
 *
 * @param [out] buffer to randomize
 * @param [in]  buffer length
 */
void cx_rng_no_throw(uint8_t *buffer, size_t len);

static inline unsigned char *cx_rng(uint8_t *buffer, size_t len)
{
  cx_rng_no_throw(buffer, len);
  return buffer;
}

/**
 * Generate 32 bits of random. This function is implemented as a SUDOCALL to
 * allow calls from constrained tasks (IO sometimes needs random numbers).
 * @return a random 32bits.
 */
static inline uint32_t cx_rng_u32(void) {
  uint32_t r;
  cx_rng_no_throw((uint8_t *)&r, sizeof(uint32_t));
  return r;
}

/**
 * Generate 8 bits of random.
 * @return a random 8bits.
 */
static inline uint8_t cx_rng_u8(void) {
  uint8_t r;
  cx_rng_no_throw((uint8_t *)&r, sizeof(uint8_t));
  return r;
}

typedef uint32_t (*cx_rng_u32_range_randfunc_t)(void);

/**
 * Generate a random 32 bits unsigned integer in range [a;b[ with uniform probability.
 *
 * @param [in] a        inclusive low bound
 * @param [in] b        exclusive high bound
 * @param [in] randfunc function called to generate random u32 value
 *
 * @return generated random.
 */
uint32_t cx_rng_u32_range_func(uint32_t a, uint32_t b, cx_rng_u32_range_randfunc_t randfunc);

/**
 * Generate a random 32 bits unsigned integer in range [a;b[ with unform probality.
 *
 * @param [in] a   inclusive low bound
 * @param [in] b   exclusive high bound
 *
 * @return generated random.
 */
static inline uint32_t cx_rng_u32_range(uint32_t a, uint32_t b) {
  return cx_rng_u32_range_func(a, b, cx_rng_u32);
}

/**
 * Generate a random buffer according to rfc6979
 */
cx_err_t cx_rng_rfc6979(cx_md_t hash_id,
                        const uint8_t *x, size_t x_len,
                        const uint8_t *h1, size_t h1_len,
                        const uint8_t *q, size_t q_len,
                        uint8_t *out, size_t out_len) ;

#endif // LCX_RNG_H

#endif // HAVE_RNG
