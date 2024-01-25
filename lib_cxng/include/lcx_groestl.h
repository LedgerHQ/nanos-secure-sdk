
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

#define ROWS     8
#define COLS1024 16
#define SIZE1024 (ROWS * COLS1024)
typedef unsigned char BitSequence;
/**  @private Hash state */
struct hashState_s {
    uint8_t      chaining[ROWS][COLS1024];  ///< Actual state
    uint64_t     block_counter;             ///< Block counter
    unsigned int hashlen;                   ///< Output length
    BitSequence  buffer[SIZE1024];          ///< Block buffer
    unsigned int buf_ptr;                   ///< Buffer pointer
    unsigned int columns;                   ///< Number of columns in a state
    unsigned int rounds;                    ///< Number of rounds in P and Q
    unsigned int statesize;                 ///< Size of the state
};
/** @private */
typedef struct hashState_s hashState;

/**
 * @brief Groestl context
 */
struct cx_groestl_s {
    struct cx_hash_header_s header;       ///< @copydoc cx_ripemd160_s::header
    unsigned int            output_size;  ///< Output digest size
    struct hashState_s      ctx;          ///< Hash state
};
/** Convenience type.*/
typedef struct cx_groestl_s cx_groestl_t;

/**
 * @brief   Initializes a GROESTL context.
 *
 * @param[out] hash Pointer to the context to initialize.
 *
 * @param[in]  size Length of the digest.
 *
 * @return          Error code:
 *                  - CX_OK on success
 *                  - CX_INVALID_PARAMETER
 */
WARN_UNUSED_RESULT cx_err_t cx_groestl_init_no_throw(cx_groestl_t *hash, size_t size);

/**
 * @deprecated
 * See #cx_groestl_init_no_throw
 */
DEPRECATED static inline int cx_groestl_init(cx_groestl_t *hash, unsigned int size)
{
    CX_THROW(cx_groestl_init_no_throw(hash, size));
    return CX_GROESTL;
}

#endif

#endif  // HAVE_GROESTL
