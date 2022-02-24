
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

#ifndef CX_ADES_H
#define CX_ADES_H

#ifdef HAVE_AES
#include "lcx_aes.h"

/** HW support */
cx_err_t cx_aes_set_key_hw(const cx_aes_key_t *keys, uint32_t mode);
cx_err_t cx_aes_block_hw(const uint8_t *inblock, uint8_t *outblock);
void cx_aes_reset_hw(void);

#endif

/** HW support */
#ifdef HAVE_DES
#include "lcx_des.h"

void cx_des_set_hw_key(const cx_des_key_t *keys, uint32_t mode);
cx_err_t cx_des_hw_block(const uint8_t *inblock, uint8_t *outblock);
void cx_des_reset_hw(void);
#endif

#endif
