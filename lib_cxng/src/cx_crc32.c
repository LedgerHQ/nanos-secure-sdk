
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

#include "cx_crc.h"
#include "cx_ram.h"

static uint32_t reverse_32_bits(uint32_t value)
{
    uint32_t reverse_val = 0;

    for (uint8_t i = 0; i < 32; i++) {
        if ((value & (1 << i))) {
            reverse_val |= 1 << ((32 - 1) - i);
        }
    }
    return reverse_val;
}

// x^32 + x^26 + x^23 + x^22 + x^16 + x^12 + x^11 + x^10 + x^8 + x^7 + x^5 + x^4 + x^2 + x + 1
uint32_t cx_crc32(const void *buf, size_t len)
{
    return cx_crc_hw(CRC_TYPE_CRC32, CX_CRC32_INIT, buf, len);
}

uint32_t cx_crc32_update(uint32_t crc_state, const void *buf, size_t len)
{
    crc_state = reverse_32_bits(crc_state);
    crc_state ^= 0xFFFFFFFF;
    return cx_crc_hw(CRC_TYPE_CRC32, crc_state, buf, len);
}
#endif  // HAVE_CRC
