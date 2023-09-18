/*******************************************************************************
 *   (c) 2023 Ledger
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

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

extern volatile bool G_called_from_swap;
extern volatile bool G_swap_response_ready;

bool swap_str_to_u64(const uint8_t *src, size_t length, uint64_t *result);
bool swap_parse_config(const uint8_t *config,
                       uint8_t        config_len,
                       char          *ticker,
                       uint8_t        ticker_buf_len,
                       uint8_t       *decimals);
