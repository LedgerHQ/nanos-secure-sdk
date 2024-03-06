
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

#ifndef CX_CRC_H
#define CX_CRC_H

#ifdef HAVE_CRC

#include <stddef.h>
#include <stdint.h>

/** CRC32 initial value */
#define CX_CRC32_INIT 0xFFFFFFFF

uint32_t cx_crc32(const void *buf, size_t len);
uint32_t cx_crc32_update(uint32_t crc_state, const void *buf, size_t len);

#endif  // HAVE_CRC

#endif  // CX_CRC_H
