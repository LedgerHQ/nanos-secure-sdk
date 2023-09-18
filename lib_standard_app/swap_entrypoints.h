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

#include "swap_lib_calls.h"

/*
 * These functions must be defined by the application
 */

/* Check check_address_parameters_t.address_to_check against specified parameters.
 *
 * Must set params.result to 0 on error, 1 otherwise */
void swap_handle_check_address(check_address_parameters_t *params);

/* Format printable amount including the ticker from specified parameters.
 *
 * Must set empty printable_amount on error, printable amount otherwise */
void swap_handle_get_printable_amount(get_printable_amount_parameters_t *params);

/* Backup up transaction parameters and wipe BSS to avoid collusion with
 * app-exchange BSS data.
 *
 * return false on error, true otherwise */
bool swap_copy_transaction_parameters(create_transaction_parameters_t *sign_transaction_params);

/* Set create_transaction.result and call os_lib_end().
 *
 * Doesn't return */
void __attribute__((noreturn)) swap_finalize_exchange_sign_transaction(bool is_success);
