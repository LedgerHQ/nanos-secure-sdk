
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

#pragma once

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "os_id.h"
#include "lcx_crc.h"

/* Exported enumerations -----------------------------------------------------*/

/* Exported types, structures, unions ----------------------------------------*/

/* Exported defines   --------------------------------------------------------*/

/* Exported macros------------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions prototypes--------------------------------------------- */
void LEDGER_BLE_init(void);
void LEDGER_BLE_send(const uint8_t *packet, uint16_t packet_length);
void LEDGER_BLE_receive(const uint8_t *spi_buffer);
void LEDGER_BLE_set_recv_buffer(uint8_t *buffer, uint16_t buffer_length);
void LEDGER_BLE_enable_advertising(uint8_t enable);
void LEDGER_BLE_reset_pairings(void);
void LEDGER_BLE_accept_pairing(uint8_t status);

#define LEDGER_BLE_get_mac_address(address)            \
    {                                                  \
        unsigned char se_serial[8] = {0};              \
        os_serial(se_serial, sizeof(se_serial));       \
        unsigned int uid = cx_crc16(se_serial, 4);     \
        address[0]       = uid;                        \
        address[1]       = uid >> 8;                   \
        uid              = cx_crc16(se_serial + 4, 4); \
        address[2]       = uid;                        \
        address[3]       = uid >> 8;                   \
        address[4]       = 0xF1;                       \
        address[5]       = 0xDE;                       \
    }
