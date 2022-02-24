
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
 * @file    ox_des.h
 * @brief   Data Encryption Standard syscalls.
 *
 * This file contains DES definitions and functions:
 * - Set the DES key in memory
 * - Encrypt a 64-bit block
 * - Reset the DES context
 */

#ifndef OX_DES_H
#define OX_DES_H

#include "cx_errors.h"
#include "decorators.h"
#include <stddef.h>
#include <stdint.h>

/** Block size of the DES in bytes */
#define CX_DES_BLOCK_SIZE 8

/** @brief   DES key container.
 *
 *  @details DES key container.
 *           Such container should be initialized with **cx_des_init_key** to
 * ensure future API compatibility. Indeed, in next API level, the key store
 * format may changed at all. 8 bytes (simple DES), 16 bytes (triple DES with 2
 * keys) and 24 bytes (triple DES with 3 keys) are supported.
 */
struct cx_des_key_s {
  uint8_t size;     ///< key size
  uint8_t keys[24]; ///< key value
};
/** Convenience type. See #cx_des_key_s. */
typedef struct cx_des_key_s cx_des_key_t;

/**
 * @brief   Set a DES key in hardware.
 *
 * @param[in] keys DES key.
 *
 * @param[in] mode Operation for which the key will be used.
 *
 * @return         Error code:
 *                 - CX_OK on success
 *                 - INVALID_PARAMETER
 */
SYSCALL cx_err_t cx_des_set_key_hw(
    const cx_des_key_t *keys PLENGTH(sizeof(cx_des_key_t)), uint32_t mode);

/**
 * @brief   Reset DES context.
 */
SYSCALL void cx_des_reset_hw(void);

/**
 * @brief   Encrypt or decrypt a block with DES.
 *
 * @param[in]  inblock  Pointer to the block.
 *
 * @param[out] outblock Buffer for the output.
 */
SYSCALL void cx_des_block_hw(const unsigned char *inblock PLENGTH(8),
                             unsigned char *outblock PLENGTH(8));

#endif
