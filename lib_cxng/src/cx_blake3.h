#ifdef HAVE_BLAKE3

#ifndef CX_BLAKE3_H
#define CX_BLAKE3_H

#include "lcx_blake3.h"
#include <stddef.h>
#include <stdint.h>
#include "cx_errors.h"

#define        BLAKE3_KEY_LEN           32
#define        BLAKE3_CHUNK_LEN         1024

enum blake3_flags {
  CHUNK_START         = 1 << 0,
  CHUNK_END           = 1 << 1,
  PARENT              = 1 << 2,
  ROOT                = 1 << 3,
  KEYED_HASH          = 1 << 4,
  DERIVE_KEY_CONTEXT  = 1 << 5,
  DERIVE_KEY_MATERIAL = 1 << 6,
  LAST                = 1 << 7,
};

typedef struct {
  uint32_t input_cv[BLAKE3_NB_OF_WORDS];
  uint64_t counter;
  uint8_t  block[BLAKE3_BLOCK_LEN];
  uint8_t  block_len;
  uint8_t  d;
} cx_blake3_state_out_t;

// Initialization vectors
static const uint32_t IV[8] = {0x6A09E667UL, 0xBB67AE85UL, 0x3C6EF372UL,
                               0xA54FF53AUL, 0x510E527FUL, 0x9B05688CUL,
                               0x1F83D9ABUL, 0x5BE0CD19UL};
// Permutational key schedule used to permute the message words after each round
// of the compression function
static const uint8_t  MSG_SCHEDULE[7][16] = {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                                             {2, 6, 3, 10, 7, 0, 4, 13, 1, 11, 12, 5, 9, 14, 15, 8},
                                             {3, 4, 10, 12, 13, 2, 7, 14, 6, 5, 9, 0, 11, 15, 8, 1},
                                             {10, 7, 12, 9, 14, 3, 13, 15, 4, 0, 11, 2, 5, 8, 1, 6},
                                             {12, 13, 9, 11, 15, 10, 14, 8, 7, 2, 5, 3, 0, 1, 6, 4},
                                             {9, 14, 11, 5, 8, 12, 15, 1, 13, 3, 0, 10, 2, 6, 4, 7},
                                             {11, 15, 5, 0, 1, 9, 8, 6, 14, 10, 2, 12, 3, 4, 7, 13}};

#endif // CX_BLAKE3_H
#endif // HAVE_BLAKE3
