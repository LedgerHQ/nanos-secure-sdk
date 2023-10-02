
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
 * @file  lcx_blake3.h
 * @brief BLAKE3 hash function.
 *
 * @section Description
 *
 * BLAKE3 is a cryptographic algorithm which is an evolution of
 * BLAKE2 cryptographic hash function. It provides keyed hashing and
 * extendable output. The default output size is 256 bit.
 *
 * @author  Ledger
 *
 * @version 1.0
 **/

#ifdef HAVE_BLAKE3
#ifndef LCX_BLAKE3_H
#define LCX_BLAKE3_H

#include "lcx_hash.h"
#include "lcx_wrappers.h"
#include <stddef.h>
#include <stdint.h>

// Not used within the source code but defines the stack depth.
// The stack depth is equal to log(BLAKE3_MAX_INPUT_LEN / BLAKE3_CHUNK_LEN)
// i.e. it is the maximal number of chunks that can be stored within the cv stack.
// The default input lenght is at most (2**64 - 1) but due to memory constraints
// it is set to (2**20 - 1)
#define        BLAKE3_MAX_INPUT_LEN_POWER   20
#define        BLAKE3_MAX_INPUT_LEN         (1 << BLAKE3_MAX_INPUT_LEN_POWER)

#define        BLAKE3_BLOCK_LEN             64
#define        BLAKE3_OUT_LEN               32
#define        BLAKE3_MAX_DEPTH             (BLAKE3_MAX_INPUT_LEN_POWER - 10)
#define        BLAKE3_MAX_STACK_SIZE        (BLAKE3_MAX_DEPTH + 1) * BLAKE3_OUT_LEN
#define        BLAKE3_WORD_SIZE             32
#define        BLAKE3_NB_OF_WORDS           8

/**
 * @brief Blake3 chunk state
 */
typedef struct {
  uint32_t cv[BLAKE3_NB_OF_WORDS];   ///< Chaining value
  uint64_t t;                        ///< Chunk counter
  uint8_t  buffer[BLAKE3_BLOCK_LEN]; ///< Buffer for the next block
  uint8_t  buffer_len;               ///< Number of bytes of the buffer
  uint8_t  blocks_compressed;        ///< Number of compressed blocks
  uint8_t  d;                        ///< Flags
} cx_blake3_state_t;

/**
 * @brief Blake3 context
 */
typedef struct {
  uint32_t           key[BLAKE3_NB_OF_WORDS];         ///< Key words
  // the stack contains at most (BLAKE3_MAX_DEPTH + 1)
  // 32-byte chaining values
  uint8_t            cv_stack[BLAKE3_MAX_STACK_SIZE]; ///< Stack of chaining values
  uint8_t            cv_stack_len;                    ///< Current size of the CV stack
  // contains the 32-byte chaining value (CV)
  // of the previous block,
  // the 64-byte input buffer for the next block
  // the counter t and the flags d
  cx_blake3_state_t  chunk;                           ///< Chunk state
  bool               is_init;                         ///< Context initialization
} cx_blake3_t;

/**
 * @brief Initializes a blake3 context.
 *
 * @param[in] hash       Pointer to the blake3 context.
 *
 * @param[in] mode       Mode. Supported modes are:
 *                         - default hash
 *                         - keyed hash
 *                         - key derivation
 *
 * @param[in] key         Key used in the keyed hash mode.
 *
 * @param[in] context     Context string used in the key derivation mode.
 *
 * @param[in] context_len Length of the context.
 *
 * @return                Error code.
 */
cx_err_t cx_blake3_init(cx_blake3_t *hash, uint8_t mode, const unsigned char *key,
                        const void *context, unsigned int context_len);

/**
 * @brief Computes the digest of a message using blake3.
 *
 * @param[in]  hash      Pointer to the blake3 context.
 *
 * @param[in]  mode      Indicates if the input is the last one or not.
 *
 * @param[in]  input     Input to process.
 *
 * @param[in]  input_len Length of the input.
 *
 * @param[out] out       Buffer for the output.
 *
 * @param[in]  out_len   Desired length of the output.
 *
 * @return               Error code
 */
cx_err_t cx_blake3(cx_blake3_t *hash, uint8_t mode, const void *input,
                   size_t input_len, uint8_t *out, size_t out_len);

/**
 * @brief Adds more data to process to the context.
 *
 * @param[in] hash      Pointer to the blake3 context.
 *
 * @param[in] input     Input to process.
 *
 * @param[in] input_len Length of the input.
 *
 * @return              Error code.
 */
cx_err_t cx_blake3_update(cx_blake3_t *hash, const void *input, size_t input_len);

/**
 * @brief Finalizes the hash.
 *
 * @param[in]  hash    Pointer to the blake3 context.
 *
 * @param[out] output  Buffer for the output
 *
 * @param[in]  out_len Length of the output.
 *
 * @return             Error code.
 */
cx_err_t cx_blake3_final(cx_blake3_t *hash, uint8_t *output, size_t out_len);

#endif // LCX_BLAKE3_H
#endif // HAVE_BLAKE3
