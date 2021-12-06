
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

/**
 * @file    ox_rng.h
 * @brief   Random number generation syscall.
 *
 * This file contains the function for getting random data from the TRNG.
 */

#ifndef OX_RNG_H
#define OX_RNG_H

#include <stdint.h>

#include "decorators.h"

/**
 * @brief   Get random data from the True Random Number Generation.
 *
 * @param[out] buf  Buffer where to store the random data.
 *
 * @param[in]  size Size of the random data in bytes.
 *
 */
SYSCALL void cx_trng_get_random_data(uint8_t *buf PLENGTH(size), size_t size);

void cx_trng_selftest(void);

void cx_trng_init(void);

#endif // OX_RNG_H
