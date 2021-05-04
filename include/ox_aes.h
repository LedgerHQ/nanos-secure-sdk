
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

#ifndef OX_AES_H
#define OX_AES_H

/*
 * This file is not intended to be included directly.
 * Include "ox.h" instead
 */

/** @internal */

#include <stddef.h>
#include <stdint.h>

#include "cx_errors.h"
#include "decorators.h"

#define CX_AES_BLOCK_SIZE 16

/** DES key container.
 *  Such container should be initialize with cx_des_init_key to ensure future
 * API compatibility. Indeed, in next API level, the key store format may
 * changed at all. Only 16 bytes key (AES128) are supported .
 */
struct cx_aes_key_s {
  /** key size */
  size_t size;
  /** key value */
  uint8_t keys[32];
};
/** Convenience type. See #cx_aes_key_s. */
typedef struct cx_aes_key_s cx_aes_key_t;

/**
 *
 */
SYSCALL cx_err_t cx_aes_set_key_hw(
    const cx_aes_key_t *key PLENGTH(sizeof(cx_aes_key_t)), uint32_t mode);

/**
 *
 */
SYSCALL void cx_aes_reset_hw(void);

/**
 *
 */
SYSCALL cx_err_t cx_aes_block_hw(const unsigned char *inblock PLENGTH(16),
                                 unsigned char *outblock PLENGTH(16));

#endif
