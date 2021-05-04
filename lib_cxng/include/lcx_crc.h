
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

#ifdef HAVE_CRC

#ifndef LCX_CRC_H
#define LCX_CRC_H

#include <stddef.h>
#include <stdint.h>

/**
 * Compute a 16 bits checksum value.
 * The 16 bits value is computed according to the CRC16 CCITT definition.
 *
 * @param [in] buffer
 *   The buffer to compute the crc over.
 *
 * @param [in]
 *   Bytes Length of the 'buffer'
 *
 * @return current crc value
 *
 */
  uint16_t cx_crc16(const void *buffer, size_t len);

#define CX_CRC16_INIT 0xFFFF

/** Accumulate more data to crc */
  uint16_t cx_crc16_update(uint16_t crc, const void *buffer, size_t len);

#endif

#endif // HAVE_CRC
