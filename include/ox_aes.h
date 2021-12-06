
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
 * @file    ox_aes.h
 * @brief   Advanced Encryption Standard syscalls.
 *
 * This file contains AES definitions and functions:
 * - Set the AES key in memory
 * - Encrypt a 128-bit block
 * - Reset the AES context
 */

/**
 * @mainpage
 *
 * @author  Ledger
 *
 * @section Introduction
 *
 * This documentation describes the cryptography API and
 * the syscalls that can be invoked to the operating system
 * to use basic arithmetic and cryptographic functions.
 * It is basically divided into:
 * - **cryptography API** which consists of signature algorithms, hash
 * functions, message authentication codes and encryption algorithms
 * - **syscalls** which enable computations for GF(p) and GF(2<sup align =
 * right>n</sup>) arithmetic and efficient implementation of elliptic curves.
 *
 * Here is a non-exhaustive list of supported algorithms:
 * - AES and DES in different modes
 * - ECDSA with a random or deterministic nonce
 * - EDDSA
 * - ECDH
 * - Schnorr signature with different implementations, especially the one used
 * for Zilliqa
 * - Multiple hash functions from SHA-2 and SHA-3 families as well as extendable
 * output functions (SHAKE-128 and SHAKE-256)
 * - GROESTL and RIPEMD-160
 * - Keyed-hash Message Authentication Code
 */

#ifndef OX_AES_H
#define OX_AES_H

/** @internal */

#include <stddef.h>
#include <stdint.h>

#include "cx_errors.h"
#include "decorators.h"

/** Block size of the AES in bytes. */
#define CX_AES_BLOCK_SIZE 16

/**
 * @brief   AES key container.
 *
 * @details Such container should be initialized with cx_ees_init_key to ensure
 * future API compatibility. Indeed, in next API level, the key store format may
 * changed at all. Only 16-byte key (AES128) is supported .
 */
struct cx_aes_key_s {
  size_t size;      ///< key size
  uint8_t keys[32]; ///< key value
};
/** Convenience type. See #cx_aes_key_s. */
typedef struct cx_aes_key_s cx_aes_key_t;

/**
 * @brief   Set an AES key in hardware.
 *
 * @param[in] key AES key.
 *
 * @param[in] mode Operation for which the key will be used.
 *
 * @return         Error code:
 *                 - CX_OK on success
 *                 - CX_INVALID_PARAMETER
 */
SYSCALL cx_err_t cx_aes_set_key_hw(
    const cx_aes_key_t *key PLENGTH(sizeof(cx_aes_key_t)), uint32_t mode);

/**
 * @brief   Reset AES context.
 */
SYSCALL void cx_aes_reset_hw(void);

/**
 * @brief   Encrypt or decrypt a block with AES.
 *
 * @param[in]  inblock  Pointer to the block.
 *
 * @param[out] outblock Buffer for the output.
 *
 * @return              Error code:
 *                      - CX_OK on success
 *                      - INVALID_PARAMETER
 */
SYSCALL cx_err_t cx_aes_block_hw(const unsigned char *inblock PLENGTH(16),
                                 unsigned char *outblock PLENGTH(16));

#endif
