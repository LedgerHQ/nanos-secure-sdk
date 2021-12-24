
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
 * @file    lcx_blake2.h
 * @brief   BLAKE2 crypographic hash function.
 * 
 * BLAKE2b is a cryptographic hash function optimized for 64-bit platforms that
 * produces digests of any size between 1 and 64 bytes. It is specified at https://blake2.net.
 */

#ifdef HAVE_BLAKE2
#ifndef LCX_BLAKE2_H
#define LCX_BLAKE2_H

#include "lcx_wrappers.h"
#include "lcx_hash.h"
#include <stddef.h>
#include <stdint.h>

/**  @private BLAKE2b constants */
enum blake2b_constant {
  BLAKE2B_BLOCKBYTES    = 128,       ///< Size of a block
  BLAKE2B_OUTBYTES      = 64,        ///< Size of the output
  BLAKE2B_KEYBYTES      = 64,        ///< Size of the key
  BLAKE2B_SALTBYTES     = 16,        ///< Size of the salt
  BLAKE2B_PERSONALBYTES = 16         ///< Size of the personalization string
};

/**  @private BLAKE2b state members */
struct blake2b_state__ {
  uint64_t h[8];                     ///< Internal state of the hash
  uint64_t t[2];                     ///< Message byte offset at the end of the current block
  uint64_t f[2];                     ///< Flag indicating the last block
  uint8_t  buf[BLAKE2B_BLOCKBYTES];  ///< Buffer for the processed data
  size_t   buflen;                   ///< Length of the buffer
  size_t   outlen;                   ///< Length of the output
  uint8_t  last_node;                ///< Last node
};
/** @private BLAKE2b state */
typedef struct blake2b_state__ blake2b_state;

/**
 * @brief BLAKE2b context
 */
struct cx_blake2b_s {
  struct cx_hash_header_s header;     ///< @copydoc cx_ripemd160_s::header
  size_t                 output_size; ///< Output digest size
  struct blake2b_state__ ctx;         ///< BLAKE2B state
};
/** Convenience type. See #cx_blake2b_s. */
typedef struct cx_blake2b_s cx_blake2b_t;

/**
 * @brief   Initialize BLAKE2b message digest context.
 *
 * @param[out] hash    Pointer to the BLAKE2b context to initialize.
 *                     The context shall be in RAM.
 *
 * @param[in]  out_len Digest size in bits.
 *
 * @return             Error code:
 *                     - CX_OK
 *                     - CX_INVALID_PARAMETER
 */
cx_err_t cx_blake2b_init_no_throw(cx_blake2b_t *hash, size_t out_len);

/**
 * @brief   Initialize BLAKE2b message digest context.
 * 
 * @details This function throws an exception if the
 *          initialization fails.
 *
 * @param[out] hash    Pointer to the BLAKE2b context to initialize.
 *                     The context shall be in RAM.
 *
 * @param[in]  out_len Digest size in bits.
 *
 * @return             BLAKE2b identifier.
 * 
 * @throws             CX_INVALID_PARAMETER
 */
static inline int cx_blake2b_init ( cx_blake2b_t * hash, unsigned int out_len )
{
  CX_THROW(cx_blake2b_init_no_throw(hash, out_len));
  return CX_BLAKE2B;
}

/**
 * @brief   Initialize BLAKE2b message digest context with
 *          salt and personnalization string.
 * 
 * @param[out] hash     Pointer to the BLAKE2b context to initialize.
 *                      The context shall be in RAM.
 * 
 * @param[in] out_len   Digest size in bits.
 * 
 * @param[in] salt      Pointer to a salt (optional).
 * 
 * @param[in] salt_len  Length of the salt.
 * 
 * @param[in] perso     Pointer to a personalization string (optional).
 * 
 * @param[in] perso_len Length of the personalization string.
 * 
 * @return              Error code:
 *                      - CX_OK on success
 *                      - CX_INVALID_PARAMETER
 */
cx_err_t cx_blake2b_init2_no_throw(cx_blake2b_t *hash,
                                    size_t        out_len,
                                    uint8_t *     salt,
                                    size_t        salt_len,
                                    uint8_t *     perso,
                                    size_t        perso_len);

/**
 * @brief   Initialize BLAKE2b message digest context with
 *          salt and personnalization string.
 * 
 * @details This function throws an exception if the initialization
 *          fails.
 * 
 * @param[out] hash     Pointer to the BLAKE2b context to initialize.
 *                      The context shall be in RAM.
 * 
 * @param[in] out_len   Digest size in bits.
 * 
 * @param[in] salt      Pointer to a salt (optional).
 * 
 * @param[in] salt_len  Length of the salt.
 * 
 * @param[in] perso     Pointer to a personalization string (optional).
 * 
 * @param[in] perso_len Length of the personalization string.
 * 
 * @return              BLAKE2b identifier.
 * 
 * @throws              CX_INVALID_PARAMETER
 */
static inline int cx_blake2b_init2 ( cx_blake2b_t * hash, unsigned int out_len, unsigned char * salt, unsigned int salt_len, unsigned char * perso, unsigned int perso_len )
{
  CX_THROW(cx_blake2b_init2_no_throw(hash, out_len, salt, salt_len, perso, perso_len));
  return CX_BLAKE2B;
}

#endif

#endif // HAVE_BLAKE2
