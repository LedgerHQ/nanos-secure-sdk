
/*******************************************************************************
*   Ledger Nano S - Secure firmware
*   (c) 2019 Ledger
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

#ifndef LCX_RNG_H
#define LCX_RNG_H

/**
 * Generate a random unsigned byte between 0 and 255
 *
 * @return a random byte.
 */
CXCALL unsigned char cx_rng_u8(void);

typedef uint32_t(cx_rng_u32_range_randfunc_t)(void);

/**
 * Generate a random 32 bits unsigned integer in range [a;b[ with uniform
 * probability.
 *
 * @param [in] a        inclusive low bound
 * @param [in] b        exclusive high bound
 * @param [in] randfunc function called to generate random u32 value
 *
 * @return generated random.
 */
uint32_t cx_rng_u32_range_func(uint32_t a, uint32_t b,
                               cx_rng_u32_range_randfunc_t randfunc);

/**
 * Generate a random 32 bits unsigned integer in range [a;b[ with unform
 * probality.
 *
 * @param [in] a   inclusive low bound
 * @param [in] b   exclusive high bound
 *
 * @return generated random.
 */
uint32_t cx_rng_u32_range(uint32_t a, uint32_t b);

/**
 * Generate 32 bits of random. This function is implemented as a SUDOCALL to
 * allow calls from constrained tasks (IO sometimes needs random numbers).
 * @return a random 32bits.
 */
SUDOCALL PERMISSION(APPLICATION_FLAG_NONE) unsigned int cx_rng_u32(void);
unsigned int svc_cx_rng_u32(void);

/**
 * Generate a random buffer, each bytes between 0 and 255*
 *
 * @param [out] buffer to randomize
 * @param [in]  buffer length
 */
CXCALL unsigned char *cx_rng(unsigned char *buffer PLENGTH(len),
                             unsigned int len);

#endif
