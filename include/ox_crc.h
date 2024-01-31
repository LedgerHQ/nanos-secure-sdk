
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
 * @file    ox_crc.h
 * @brief   Cyclic Redundancy Check syscall.
 *
 * This file contains the function for calculating a 32-bit cyclic redundancy check.
 */

#ifndef OX_CRC_H
#define OX_CRC_H

#include "decorators.h"

typedef enum crc_type_e {
    CRC_TYPE_CRC16_CCITT_FALSE = 0,
    CRC_TYPE_CRC32             = 1
} crc_type_t;

/**
 * @brief Calculates a cyclic redundancy check.
 *
 * @details
 * Two types are supported:
 * - CRC16-CCITT-FALSE: polynomial = x16 + x12 + x5 + 1
 *                      Init = 0xFFFF
 *                      RefIn = False
 *                      RefOut = False
 *                      XorOut = 0x0000
 * - CRC32: polynomial = x32 + x26 + x23 + x22 + x16 + x12 + x11 + x10 + x8 + x7 + x5 + x4 + x2 + x
 *                      + 1
 *                      Init = 0xFFFFFFFF
 *                      RefIn = True
 *                      RefOut = True
 *                      XorOut = 0xFFFFFFFF
 *
 * @param[in] crc_type  CRC type: either CRC_TYPE_CRC16_CCITT_FALSE
 *                      or CRC_TYPE_CRC32. The function returns 0 for
 *                      other values
 *
 * @param[in] crc_state CRC value for initialization
 *
 * @param[in] buf       Pointer to the buffer to check.
 *
 * @param[in] len       Length of the buffer.
 *
 * @return             Result of CRC calculation: either 16-bit or 32-bit
 *
 */
SYSCALL uint32_t cx_crc_hw(crc_type_t      crc_type,
                           uint32_t        crc_state,
                           const void *buf PLENGTH(len),
                           size_t          len);

#endif  // OX_CRC_H
