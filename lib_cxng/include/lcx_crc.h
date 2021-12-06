
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
 * @file    lcx_crc.h
 * @brief   CRC (Cyclic Redundancy Check).
 *
 * CRC-16 is a variant of CRC, an error-detecting code, with a 16-bit long check value.
 */

#ifdef HAVE_CRC

#ifndef LCX_CRC_H
#define LCX_CRC_H

#include <stddef.h>
#include <stdint.h>

/** CRC16 initial value */
#define CX_CRC16_INIT 0xFFFF

/**
 * @brief   Compute a 16-bit checksum value.
 * 
 * @details The 16-bit value is computed according to the CRC16 CCITT definition.
 *
 * @param[in] buffer The buffer to compute the CRC over.
 *
 * @param[in] len    Bytes length of the buffer.
 *
 * @return           Current CRC value.
 */
  uint16_t cx_crc16(const void *buffer, size_t len);

/**
 * @brief   Accumulate more data to CRC.
 * 
 * @param[in] crc    CRC value to be updated.
 * 
 * @param[in] buffer The buffer to compute the CRC over.
 * 
 * @param[in] len    Bytes length of the buffer.
 * 
 * @return           Updated CRC value.
 */
  uint16_t cx_crc16_update(uint16_t crc, const void *buffer, size_t len);

#endif

#endif // HAVE_CRC
