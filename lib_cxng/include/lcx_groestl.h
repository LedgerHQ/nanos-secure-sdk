
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

/**
 * @file    lcx_groestl.h
 * @brief   GROESTL hash function.
 *
 * Refer to <a href = "https://www.groestl.info/"> GROESTL info </a> for more details.
 */

#ifdef HAVE_GROESTL

#ifndef LCX_GROESTL_H
#define LCX_GROESTL_H

#include <stdint.h>

#include "lcx_wrappers.h"

#define ROWS 8
#define COLS1024 16
#define SIZE1024 (ROWS * COLS1024)
typedef unsigned char BitSequence;
/**  @private Hash state */
struct hashState_s {
  uint8_t      chaining[ROWS][COLS1024]; ///< Actual state
  uint64_t     block_counter;            ///< Block counter
  unsigned int hashlen;                  ///< Output length
  BitSequence  buffer[SIZE1024];         ///< Block buffer
  unsigned int buf_ptr;                  ///< Buffer pointer
  unsigned int columns;                  ///< Number of columns in a state
  unsigned int rounds;                   ///< Number of rounds in P and Q
  unsigned int statesize;                ///< Size of the state
};
/** @private */
typedef struct hashState_s hashState;

/**
 * @brief Groestl context
 */
struct cx_groestl_s {
  unsigned int output_size;             ///< Output digest size
  struct hashState_s ctx;               ///< Hash state
};
/** Convenience type.*/
typedef struct cx_groestl_s cx_groestl_t;

size_t cx_groestl_get_output_size(const cx_groestl_t *ctx);

/**
 * @brief   Initializes a GROESTL context.
 *
 * @param[out] hash Pointer to the context to init.ialize.
 * 
 * @param[in]  size Length of the digest.
 *
 * @return          Error code:
 *                  - CX_OK on success
 *                  - CX_INVALID_PARAMETER
 */
  cx_err_t cx_groestl_init_no_throw(cx_groestl_t *hash, size_t size);

/**
 * @brief   Initializes a GROESTL context.
 * 
 * @details Throws an exception if the
 *          initialization fails.
 *
 * @param[out] hash Pointer to the context to initialize.
 * 
 * @param[in]  size Length of the digest.
 *
 * @return          GROESTL identifier.
 * 
 * @throws          CX_INVALID_PARAMETER
 */
static inline void cx_groestl_init ( cx_groestl_t * hash, unsigned int size )
{
  CX_THROW(cx_groestl_init_no_throw(hash, size));
}

/**
 * @brief   Hashes data with Groestl algorithm.
 *
 * @param[in]  hash    Pointer to the hash context.
 *                     Shall be in RAM.
 *                     Should be called with a cast.
 *
 * @param[in]  mode    Crypto flag. Supported flag: CX_LAST. If set:
 *                       - the structure is not modified after finishing
 *                       - if out is not NULL, the message digest is stored in out
 *                       - the context is NOT automatically re-initialized.
 *
 * @param[in]  in      Input data to be hashed.
 *
 * @param[in]  len     Length of the input data.
 *
 * @param[out] out     Buffer where to store the message digest:
 *                       - NULL (ignored) if CX_LAST is NOT set
 *                       - message digest if CX_LAST is set
 *
 * @param[out] out_len The size of the output buffer or 0 if out is NULL.
 *                     If buffer is too small to store the hash a exception is returned.
 *
 * @return             Error code:
 *                     - CX_OK on success
 *                     - CX_INVALID_PARAMETER
 */
cx_err_t cx_groestl(cx_groestl_t *hash, uint32_t mode, const uint8_t *in, size_t len, uint8_t *out, size_t out_len);

/**
 * @brief   Adds more data to hash.
 *
 * @details A call to this function is equivalent to:
 *          *cx_groestl_no_throw(hash, 0, in, in_len, NULL, 0)*.
 *
 * @param[out] hash   Pointer to the groest context.
 *
 * @param[in]  in     Input data to add to the context.
 *
 * @param[in]  in_len Length of the input data.
 *
 * @return            Error code:
 *                    - CX_OK on success
 *                    - CX_INVALID_PARAMETER
 */
cx_err_t cx_groestl_update(cx_groestl_t *ctx, const uint8_t *data, size_t len);

/**
 * @brief   Finalizes the hash.
 *
 * @details A call to this function is equivalent to:
 *          *cx_groestl_no_throw(hash, CX_LAST, NULL, 0, out, out_len)*.
 *
 * @param[in]  hash   Pointer to the groestl context.
 *
 * @param[out] digest The message digest.
 *
 * @return            Error code:
 *                    - CX_OK on success
 */
cx_err_t cx_groestl_final(cx_groestl_t *ctx, uint8_t *digest);

#endif

#endif // HAVE_GROESTL
