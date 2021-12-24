
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

#pragma once

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported enumerations -----------------------------------------------------*/
enum {
  APDU_STATUS_WAITING,
  APDU_STATUS_NEED_MORE_DATA,
  APDU_STATUS_COMPLETE,
};

/* Exported types, structures, unions ----------------------------------------*/
typedef struct ledger_protocol_s {
  uint8_t *tx_apdu_buffer;
  uint16_t tx_apdu_length;
  uint16_t tx_apdu_sequence_number;
  uint16_t tx_apdu_offset;

  uint8_t chunk[156 + 2];
  uint8_t chunk_length;

  uint8_t *rx_apdu_buffer;
  uint16_t rx_apdu_buffer_max_length;
  uint8_t rx_apdu_status;
  uint16_t rx_apdu_sequence_number;
  uint16_t rx_apdu_length;
  uint16_t rx_apdu_offset;
  uint16_t mtu;
  uint8_t mtu_negotiated;
} ledger_protocol_t;

/* Exported defines   --------------------------------------------------------*/

/* Exported macros------------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions prototypes--------------------------------------------- */
void LEDGER_PROTOCOL_init(ledger_protocol_t *data);
void LEDGER_PROTOCOL_rx(uint8_t *buffer, uint16_t length);
void LEDGER_PROTOCOL_tx(uint8_t *buffer, uint16_t length);
