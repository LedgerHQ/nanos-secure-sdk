
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
 * @file    ox_crc.h
 * @brief   Cyclic Redundancy Check syscall.
 *
 * This file contains the function for calculating a 32-bit cyclic redundancy
 * check.
 */

#ifndef OX_CRC_H
#define OX_CRC_H

#include "decorators.h"

/**
 * @brief Calculate a 32-bit cyclic redundancy check.
 *
 * @param[in] buf Pointer to the buffer to check.
 *
 * @param[in] len Length of the buffer.
 *
 * @return        Result of the 32-bit CRC calculation.
 *
 */
SYSCALL uint32_t cx_crc32_hw(const void *buf PLENGTH(len), size_t len);

#endif // OX_CRC_H
