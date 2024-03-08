
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

#ifdef HAVE_CRC

#include "lcx_crc.h"
#include "cx_ram.h"

uint16_t cx_crc16_update(uint16_t crc, const void *buf, size_t len)
{
    return cx_crc_hw(CRC_TYPE_CRC16_CCITT_FALSE, crc, buf, len);
}

uint16_t cx_crc16(const void *buf, size_t len)
{
    return cx_crc_hw(CRC_TYPE_CRC16_CCITT_FALSE, CX_CRC16_INIT, buf, len);
}

#endif  // HAVE_CRC
