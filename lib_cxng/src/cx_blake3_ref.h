#ifdef HAVE_BLAKE3

#ifndef CX_BLAKE3_REF_H
#define CX_BLAKE3_REF_H

#include "cx_blake3.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define INLINE static inline __attribute__((always_inline))

INLINE uint32_t load32(const void *src) {
#if defined(NATIVE_LITTLE_ENDIAN)
  uint32_t w;
  memcpy(&w, src, sizeof w);
  return w;
#else
  const uint8_t *p = (const uint8_t *)src;
  return ((uint32_t)(p[0]) << 0) | ((uint32_t)(p[1]) << 8) | ((uint32_t)(p[2]) << 16) | ((uint32_t)(p[3]) << 24);
#endif
}

INLINE uint32_t rotr32(uint32_t w, uint32_t c) {
  return (w >> c) | (w << (32 - c));
}

INLINE void load_key_words(const uint8_t key[BLAKE3_KEY_LEN],
                           uint32_t key_words[8]) {
  key_words[0] = load32(&key[0 * 4]);
  key_words[1] = load32(&key[1 * 4]);
  key_words[2] = load32(&key[2 * 4]);
  key_words[3] = load32(&key[3 * 4]);
  key_words[4] = load32(&key[4 * 4]);
  key_words[5] = load32(&key[5 * 4]);
  key_words[6] = load32(&key[6 * 4]);
  key_words[7] = load32(&key[7 * 4]);
}

INLINE void store32(void *dst, uint32_t w) {
#if defined(NATIVE_LITTLE_ENDIAN)
  memcpy(dst, &w, sizeof w);
#else
  uint8_t *p = (uint8_t *)dst;
  p[0]       = (uint8_t)(w >> 0);
  p[1]       = (uint8_t)(w >> 8);
  p[2]       = (uint8_t)(w >> 16);
  p[3]       = (uint8_t)(w >> 24);
#endif
}

INLINE void store_cv_words(uint8_t bytes_out[32], uint32_t cv_words[8]) {
  store32(&bytes_out[0 * 4], cv_words[0]);
  store32(&bytes_out[1 * 4], cv_words[1]);
  store32(&bytes_out[2 * 4], cv_words[2]);
  store32(&bytes_out[3 * 4], cv_words[3]);
  store32(&bytes_out[4 * 4], cv_words[4]);
  store32(&bytes_out[5 * 4], cv_words[5]);
  store32(&bytes_out[6 * 4], cv_words[6]);
  store32(&bytes_out[7 * 4], cv_words[7]);
}

static unsigned int highest_one(uint64_t x) {
  unsigned int c = 0;
  if (x & 0xffffffff00000000ULL) {
    x >>= 32;
    c  += 32;
  }
  if (x & 0x00000000ffff0000ULL) {
    x >>= 16;
    c  += 16;
  }
  if (x & 0x000000000000ff00ULL) {
    x >>= 8;
    c  += 8;
  }
  if (x & 0x00000000000000f0ULL) {
    x >>= 4;
     c += 4;
  }
  if (x & 0x000000000000000cULL) {
    x >>= 2;
    c  += 2;
  }
  if (x & 0x0000000000000002ULL) {
    c += 1;
  }

  return c;
}

// The Hamming weight of x, i.e. the number of 1 in the binary representation 
INLINE unsigned int hw(uint64_t x) {
  unsigned int count = 0;
  while (x) {
    count += 1;
    x     &= x - 1;
  }
  return count;
}

/**
 * @brief   Initialize the chunk state.
 *
 * @details The chaining value is set to the value of the key.
 *
 * @param[in] chunk_state Chunk state.
 *
 * @param[in] key         32-byte key-words.
 *
 * @param[in] flags       Flags associated to the chunk.
 */
void blake3_state_init(cx_blake3_state_t *chunk_state, const uint32_t *key, uint8_t flags);

/**
 * @brief Update a chunk state
 *
 * @param[in] chunk_state Chunk state.
 *
 * @param[in] input       Input used for this chunk.
 *
 * @param[in] input_len   Length of the input.
 */
void blake3_state_update(cx_blake3_state_t *chunk_state, const uint8_t *input, size_t input_len);

/**
 * @brief Set the state buffer to zero before going to the next chunk.
 *
 * @param[in] chunk_state   Chunk state.
 *
 * @param[in] key           Key used to set the chaining value
 *                          when resetting the state.
 *
 * @param[in] chunk_counter Number of already compressed chunks.
 */
void blake3_state_reset(cx_blake3_state_t *chunk_state, const uint32_t *key, uint64_t chunk_counter);

/**
 * @brief   Output the chunk state.
 *
 * @details The returned structure consists of the last chaining value,
 *          the buffer which contains the last block of the chunk,
 *          the chunk counter and the chunk flag.
 *          The last block of the chunk sets the CHUNK_END flag. However, if a chunk
 *          contains only one block then both CHUNK_START and CHUNK_END are set.
 *          Also, if the chunk is the root, then the last block sets the ROOT flag.
 *
 * @param[in] chunk_state Chunk state.
 *
 * @return                Output of a chunk.
 */
cx_blake3_state_out_t blake3_state_output(const cx_blake3_state_t *chunk_state);

/**
 * @brief   Pick the output chaining value.
 *
 * @details Pick the the chaining value of the output structure.
 *          The 32-byte words obtained after the compression is
 *          converted into byte array.
 *
 * @param[in]  out Chunk state output.
 *
 * @param[out] cv  Chaining value of a chunk output.
 */
void blake3_output_chain(const cx_blake3_state_out_t *out, uint8_t *cv);

/**
 * @brief   Compress a subtree to a parent node.
 *
 * @details The output contains the concatenated chaining values of the children.
 *
 * @param[in]  input         Several chunks.
 *
 * @param[in]  input_len     Length of the input.
 *
 * @param[in]  key           Key used for the compression.
 *
 * @param[in]  chunk_counter Number of compressed chunks.
 *
 * @param[in]  flags         Flags used for the compression.
 *
 * @param[out] out           Output of the compression: the chaining value of a parent node.
 */
void blake3_compress_subtree_to_parent(const uint8_t *input, size_t input_len, const uint32_t *key,
                                       uint64_t chunk_counter, uint8_t flags, uint8_t *out);

/**
 * @brief Merge the chaining values stack.
 *
 * @param[in] hash      Pointer to the blake3 context.
 *
 * @param[in] total_len Total length of the stack.
 */
void blake3_hasher_merge_cv(cx_blake3_t *hash, uint64_t total_len);

/**
 * @brief Push a chaining value into the chaining values stack.
 *
 * @param[in] hash          Pointer to the blake3 context.
 *
 * @param[in] new_cv        Chaining value to be pushed onto the stack.
 *
 * @param[in] chunk_counter Number of chunks.
 */
void blake3_hasher_push_cv(cx_blake3_t *hash, uint8_t *new_cv, uint64_t chunk_counter);

/**
 * @brief Output the hash of the root node.
 *
 * @param[in] chunk_out Chunk output.
 *
 * @param[in]  out_len  Length of the output.
 *
 * @param[out] out      Output of the compression when the chunk is the root node.
 *
 */
void blake3_output_root_bytes(const cx_blake3_state_out_t *chunk_out, uint8_t *out, size_t out_len);

/**
 * @brief Initialize the context.
 *
 * @param[in] hash Pointer to the blake3 context.
 *
 * @param[in] key  Key used for the initialization.
 *
 * @param[in] mode Mode to the used. Supported modes:
 *                   - hash
 *                   - keyed hash
 *                   - key derivation
 */
void blake3_init_ctx(cx_blake3_t *hash, const uint32_t *key, uint8_t mode);

#endif // CX_BLAKE3_REF_H
#endif // HAVE_BLAKE3
