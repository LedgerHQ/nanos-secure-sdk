
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
#ifdef HAVE_RNG

#ifndef CX_RNG_H
#define CX_RNG_H

#include <stdint.h>

void cx_rng_init(void);

/// @brief function to be called each time random data is needed
uint32_t cx_trng_u32(void);

#ifdef CX_EMU_SELFTESTS
void cx_trng_lengthtest(void);
#endif

#endif

#endif // HAVE_RNG
