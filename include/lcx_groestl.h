
/*******************************************************************************
*   Ledger Nano S - Secure firmware
*   (c) 2019 Ledger
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

#ifndef LCX_GROESTL_H
#define LCX_GROESTL_H

#include <stddef.h>
#include <stdint.h>

#define ROWS 8
#define COLS1024 16
#define SIZE1024 (ROWS * COLS1024)
typedef unsigned char BitSequence;
/**  @Private */
struct hashState_s {
  uint8_t chaining[ROWS][COLS1024]; /* the actual state */
  uint64_t block_counter;           /* block counter */
  unsigned int hashlen;             /* output length */
  BitSequence buffer[SIZE1024];     /* block buffer */
  unsigned int buf_ptr;             /* buffer pointer */
  unsigned int columns;             /* number of columns in state */
  unsigned int rounds;              /* number of rounds in P and Q */
  unsigned int statesize;           /* size of state (ROWS*columns) */
};
/** @Private */
typedef struct hashState_s hashState;

/**
 * Groestl context
 */
struct cx_groestl_s {
  /** @copydoc cx_ripemd160_s::header */
  struct cx_hash_header_s header;
  /** @internal output digest size*/
  unsigned int output_size;

  struct hashState_s ctx;
};
/** Convenience type. See #cx_groestl512_s. */
typedef struct cx_groestl_s cx_groestl_t;

/**
 * Init a groestl224 context.
 *
 * @param [out] hash the context to init.
 *    The context shall be in RAM
 *
 * @return algorithm identifier
 */
CXCALL int cx_groestl_init(cx_groestl_t *hash PLENGTH(sizeof(cx_groestl_t)),
                           unsigned int size);

#endif
