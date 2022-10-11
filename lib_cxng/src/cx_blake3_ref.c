#ifdef HAVE_BLAKE3

#include "cx_blake3_ref.h"
#include "cx_utils.h"
#include "cx_ram.h"

#include <string.h>

/* ---------------------------------------------------------------------------------------------------*/
/*                             Functions from the reference implementation                            */
/* ---------------------------------------------------------------------------------------------------*/

/*********************  Compression function  ************************/

// The 'quarter-round' function G_i(a,b,c,d)
static void g(uint32_t *state, size_t a, size_t b, size_t c, size_t d,
              uint32_t x, uint32_t y) {
  state[a] = state[a] + state[b] + x;
  state[d] = rotr32(state[d] ^ state[a], 16);
  state[c] = state[c] + state[d];
  state[b] = rotr32(state[b] ^ state[c], 12);
  state[a] = state[a] + state[b] + y;
  state[d] = rotr32(state[d] ^ state[a], 8);
  state[c] = state[c] + state[d];
  state[b] = rotr32(state[b] ^ state[c], 7);
}

// The round function F = |G_0(0,4,8,12)  G_1(1,5,9,13)  G_2(2,6,10,14) G_3(3,7,11,15)|
//                        |G_4(0,5,10,15) G_5(1,6,11,12) G_6(2,7,8,13)  G_7(3,4,9,14) |
static void round_fn(uint32_t state[16], const uint32_t *msg, size_t round) {
  // Select the message schedule based on the round.
  const uint8_t *schedule = MSG_SCHEDULE[round];

  // Mix the columns.
  g(state, 0, 4, 8, 12, msg[schedule[0]], msg[schedule[1]]);
  g(state, 1, 5, 9, 13, msg[schedule[2]], msg[schedule[3]]);
  g(state, 2, 6, 10, 14, msg[schedule[4]], msg[schedule[5]]);
  g(state, 3, 7, 11, 15, msg[schedule[6]], msg[schedule[7]]);

  // Mix the rows.
  g(state, 0, 5, 10, 15, msg[schedule[8]], msg[schedule[9]]);
  g(state, 1, 6, 11, 12, msg[schedule[10]], msg[schedule[11]]);
  g(state, 2, 7, 8, 13, msg[schedule[12]], msg[schedule[13]]);
  g(state, 3, 4, 9, 14, msg[schedule[14]], msg[schedule[15]]);
}

// Compressing a block along with a chaining value
static void compress_pre(uint32_t *state, const uint32_t *cv,
                         const uint8_t *block, uint8_t block_len,
                         uint64_t counter, uint8_t flags) {
  uint32_t block_words[16];
  size_t   round;
  // First convert the block into 32-byte words
  for (int i = 0; i < 16; i++) {
    block_words[i]  = load32(block + 4 * i);
  }

  // Initialize the state
  state[0]        = cv[0];
  state[1]        = cv[1];
  state[2]        = cv[2];
  state[3]        = cv[3];
  state[4]        = cv[4];
  state[5]        = cv[5];
  state[6]        = cv[6];
  state[7]        = cv[7];
  state[8]        = IV[0];
  state[9]        = IV[1];
  state[10]       = IV[2];
  state[11]       = IV[3];
  state[12]       = (uint32_t)counter;
  state[13]       = (uint32_t)(counter >> 32);
  state[14]       = (uint32_t)block_len;
  state[15]       = (uint32_t)flags;

  for (round = 0; round < 7; round++) {
    round_fn(state, &block_words[0], round);
  }
}

/**
 * @brief   Compress a block to get a chaining value.
 *
 * @details The chaining value corresponds to the 8 first 32-byte words
 *          of the output of the compression function.
 *
 * @param[out] cv        Computed chaining value.
 *
 * @param[in]  block     Block to be compressed.
 *
 * @param[in]  block_len Length of the block.
 *
 * @param[in]  counter   Counter of already compressed blocks.
 *
 * @param[in]  flags     Flags associated to the block.
 *
 */
static void blake3_compress_in_place(uint32_t *cv, const uint8_t *block,
                                     uint8_t block_len, uint64_t counter,
                                     uint8_t flags) {
  uint32_t state[16];
  compress_pre(&state[0], cv, block, block_len, counter, flags);
  cv[0] = state[0] ^ state[8];
  cv[1] = state[1] ^ state[9];
  cv[2] = state[2] ^ state[10];
  cv[3] = state[3] ^ state[11];
  cv[4] = state[4] ^ state[12];
  cv[5] = state[5] ^ state[13];
  cv[6] = state[6] ^ state[14];
  cv[7] = state[7] ^ state[15];
}

/**
 * @brief   Compute the output of the compression function.
 *
 * @details Store the 32-byte words as byte arrays.
 *
 * @param[in]  cv        Computed chaining value.
 *
 * @param[in]  block     Block to be compressed.
 *
 * @param[in]  block_len Length of the block.
 *
 * @param[in]  counter   Counter of already compressed blocks.
 *
 * @param[in]  flags     Flags associated to the block.
 *
 * @param[out] out       Full output of the compression function (64 bytes).
 */
static void blake3_compress_xof(const uint32_t *cv, const uint8_t *block,
                                uint8_t block_len, uint64_t counter,
                                uint8_t flags, uint8_t *out) {
  uint32_t state[16];
  compress_pre(state, cv, block, block_len, counter, flags);

  store32(&out[0 * 4], state[0] ^ state[8]);
  store32(&out[1 * 4], state[1] ^ state[9]);
  store32(&out[2 * 4], state[2] ^ state[10]);
  store32(&out[3 * 4], state[3] ^ state[11]);
  store32(&out[4 * 4], state[4] ^ state[12]);
  store32(&out[5 * 4], state[5] ^ state[13]);
  store32(&out[6 * 4], state[6] ^ state[14]);
  store32(&out[7 * 4], state[7] ^ state[15]);
  store32(&out[8 * 4], state[8] ^ cv[0]);
  store32(&out[9 * 4], state[9] ^ cv[1]);
  store32(&out[10 * 4], state[10] ^ cv[2]);
  store32(&out[11 * 4], state[11] ^ cv[3]);
  store32(&out[12 * 4], state[12] ^ cv[4]);
  store32(&out[13 * 4], state[13] ^ cv[5]);
  store32(&out[14 * 4], state[14] ^ cv[6]);
  store32(&out[15 * 4], state[15] ^ cv[7]);
}

/********************  Chunk state functions  *********************/
/**
 *  @brief Fill the chunk state buffer with the input bytes.
 *
 * @param[in] chunk_state Chunk state
 *
 * @param[in] input       Input used to fill the buffer.
 *
 * @param[in] input_len   Length of the input.
 *
 * @return                Current size of the buffer.
 */
static size_t blake3_fill_buffer(cx_blake3_state_t *chunk_state, const uint8_t *input, size_t input_len) {
  size_t nb_bytes = BLAKE3_BLOCK_LEN - ((size_t)chunk_state->buffer_len);
  if (nb_bytes > input_len) {
    nb_bytes = input_len;
  }
  memcpy(chunk_state->buffer + ((size_t)chunk_state->buffer_len), input, nb_bytes);
  chunk_state->buffer_len += (uint8_t)nb_bytes;

  return nb_bytes;
}

void blake3_state_init(cx_blake3_state_t *chunk_state, const uint32_t *key,
                       uint8_t flags) {
  memcpy(chunk_state->cv, key, BLAKE3_KEY_LEN);
  chunk_state->t                 = 0;
  memset(chunk_state->buffer, 0, BLAKE3_BLOCK_LEN);
  chunk_state->buffer_len        = 0;
  chunk_state->blocks_compressed = 0;
  chunk_state->d                 = flags;
}

void blake3_state_update(cx_blake3_state_t *chunk_state, const uint8_t *input,
                         size_t input_len) {
  size_t  nb_bytes;
  uint8_t is_start_flag = 0;
  if (chunk_state->buffer_len > 0) {
    nb_bytes   = blake3_fill_buffer(chunk_state, input, input_len);
    input     += nb_bytes;
    input_len -= nb_bytes;
    if (input_len > 0) {
      if (!chunk_state->blocks_compressed) {
        is_start_flag = CHUNK_START;
      }
      blake3_compress_in_place(chunk_state->cv, chunk_state->buffer,
                               BLAKE3_BLOCK_LEN, chunk_state->t,
                               chunk_state->d | is_start_flag);
      chunk_state->blocks_compressed += 1;
      chunk_state->buffer_len         = 0;
      memset(chunk_state->buffer, 0, BLAKE3_BLOCK_LEN);
    }
  }

  while (input_len > BLAKE3_BLOCK_LEN) {
    if (!chunk_state->blocks_compressed) {
      is_start_flag = CHUNK_START;
    }
    else {
      is_start_flag = 0;
    }
    blake3_compress_in_place(chunk_state->cv, input, BLAKE3_BLOCK_LEN,
                             chunk_state->t,
                             chunk_state->d | is_start_flag);
    chunk_state->blocks_compressed += 1;
    input                          += BLAKE3_BLOCK_LEN;
    input_len                      -= BLAKE3_BLOCK_LEN;
  }

  nb_bytes   = blake3_fill_buffer(chunk_state, input, input_len);
}

void blake3_state_reset(cx_blake3_state_t *chunk_state, const uint32_t *key,
                        uint64_t chunk_counter) {
  memcpy(chunk_state->cv, key, BLAKE3_KEY_LEN);
  chunk_state->t                 = chunk_counter;
  chunk_state->blocks_compressed = 0;
  memset(chunk_state->buffer, 0, BLAKE3_BLOCK_LEN);
  chunk_state->buffer_len        = 0;
}

cx_blake3_state_out_t blake3_state_output(const cx_blake3_state_t *chunk_state) {
  uint8_t               is_start_flag = 0;
  uint8_t               block_flags;
  cx_blake3_state_out_t chunk_output;

  if (!chunk_state->blocks_compressed) {
    is_start_flag = CHUNK_START;
  }
  block_flags = chunk_state->d | is_start_flag | CHUNK_END;
  memcpy(chunk_output.input_cv, chunk_state->cv, BLAKE3_OUT_LEN);
  memcpy(chunk_output.block, chunk_state->buffer, BLAKE3_BLOCK_LEN);
  chunk_output.block_len = chunk_state->buffer_len;
  chunk_output.counter   = chunk_state->t;
  chunk_output.d         = block_flags;

  return chunk_output;
}

void blake3_output_chain(const cx_blake3_state_out_t *out, uint8_t *cv) {
  uint32_t cv_words[BLAKE3_NB_OF_WORDS];
  memcpy(cv_words, out->input_cv, BLAKE3_WORD_SIZE);
  blake3_compress_in_place(cv_words, out->block, out->block_len,
                           out->counter, out->d);
  store_cv_words(cv, cv_words);
}


/*********************  Hashing  ************************/
/**
 * @brief   Hash one chunk.
 *
 * @details Each block of the chunk is compressed.
 *
 * @param[in]  input       Chunk to be compressed.
 *
 * @param[in]  blocks      Number of blocks.
 *
 * @param[in]  key         Key used for the compressed.
 *
 * @param[in]  counter     Number of compressed blocks.
 *
 * @param[in]  flags       Flags associated to the chunk.
 *
 * @param[in]  flags_start Indicates if the start flag is set.
 *
 * @param[in]  flags_end   Indicates if the end flag is set.
 *
 * @param[out] out         Output of the compression: a 32-byte chaining value.
 */
static void blake3_hash_one(const uint8_t *input, size_t blocks,
                            const uint32_t *key, uint64_t counter,
                            uint8_t flags, uint8_t flags_start,
                            uint8_t flags_end, uint8_t *out) {
  uint32_t cv[BLAKE3_NB_OF_WORDS];
  memcpy(cv, key, BLAKE3_KEY_LEN);
  uint8_t block_flags = flags | flags_start;
  while (blocks > 0) {
    if (1 == blocks) {
      block_flags |= flags_end;
    }
    blake3_compress_in_place(cv, input, BLAKE3_BLOCK_LEN,
                             counter, block_flags);
    input       = &input[BLAKE3_BLOCK_LEN];
    blocks     -= 1;
    block_flags = flags;
  }
  store_cv_words(out, cv);
}

/**
 * @brief Hash several chunks when it is possible.
 *
 * @param[in]  inputs            Chunks to be compressed.
 *
 * @param[in]  num_inputs        Number of chunks.
 *
 * @param[in]  blocks            Number of blocks.
 *
 * @param[in]  key               Key used for the compression.
 *
 * @param[in]  counter           Number of compressed blocks.
 *
 * @param[in]  increment_counter Enables to increment the counter.
 *
 * @param[in]  flags             Flags associated to the chunks.
 *
 * @param[in]  flags_start       Indicates if the start flag is set.
 *
 * @param[in]  flags_end         Indicates if the end flag is set.
 *
 * @param[out] out               Outputs of the compression.
 */
static void blake3_hash_many(const uint8_t *const *inputs, size_t num_inputs,
                             size_t blocks, const uint32_t *key,
                             uint64_t counter, bool increment_counter,
                             uint8_t flags, uint8_t flags_start,
                             uint8_t flags_end, uint8_t *out) {
  while (num_inputs > 0) {
    blake3_hash_one(inputs[0], blocks, key, counter, flags, flags_start,
                    flags_end, out);
    if (increment_counter) {
      counter  += 1;
    }
    inputs     += 1;
    num_inputs -= 1;
    out         = &out[BLAKE3_OUT_LEN];
  }
}

/**
 * @brief   Compress chunks.
 *
 * @details In contrary to the reference implementation, it compresses one chunk at a time.
 *
 * @param[in]  input         One or several chunks.
 *
 * @param[in]  input_len     Length of the input.
 *
 * @param[in]  key           Key used for the compression.
 *
 * @param[in]  chunk_counter Number of chunks.
 *
 * @param[in]  flags         Flags associated to the chunks.
 *
 * @param[out] out           Output of the compression.
 *
 * @return                   Length of the chunk array
 *                          (if several chunks have been compressed in parallel).
 */
static size_t blake3_compress_chunks(const uint8_t *input, size_t input_len,
                                     const uint32_t *key, uint64_t chunk_counter,
                                     uint8_t flags, uint8_t *out) {

  const uint8_t        *chunks_array[1];
  size_t                input_position   = 0;
  size_t                chunks_array_len = 0;
  cx_blake3_state_out_t output;

  while (input_len - input_position >= BLAKE3_CHUNK_LEN) {
    chunks_array[chunks_array_len] = &input[input_position];
    input_position                += BLAKE3_CHUNK_LEN;
    chunks_array_len              += 1;
  }

  blake3_hash_many(chunks_array, chunks_array_len,
                   BLAKE3_CHUNK_LEN / BLAKE3_BLOCK_LEN, key, chunk_counter,
                   true, flags, CHUNK_START, CHUNK_END, out);

  // Hash the remaining partial chunk, if there is one. Note that the empty
  // chunk (meaning the empty message) is a different codepath.
  if (input_len > input_position) {
    uint64_t counter = chunk_counter + (uint64_t)chunks_array_len;
    cx_blake3_state_t chunk_state;
    blake3_state_init(&chunk_state, key, flags);
    chunk_state.t = counter;
    blake3_state_update(&chunk_state, input + input_position, input_len - input_position);
    output = blake3_state_output(&chunk_state);
    blake3_output_chain(&output, out + chunks_array_len * BLAKE3_OUT_LEN);
    return chunks_array_len + 1;
  }
  else {
    return chunks_array_len;
  }
}

/**
 * @brief Compress parents.
 *
 * @param[in]  child_chaining_values Chaining values of nodes children.
 *
 * @param[in]  num_chaining_values   Number of chaining values.
 *
 * @param[in]  key                   Key used for the compression.
 *
 * @param[in]  flags                 Flags used for the compression.
 *
 * @param[out] out                   Output of the compression (the chaining value of a parent node).
 *
 * @return                           Length of the parents array, i.e. the number of compressed parent nodes
 */
static size_t blake3_compress_parents(const uint8_t *child_chaining_values,
                                      size_t num_chaining_values,
                                      const uint32_t *key, uint8_t flags,
                                      uint8_t *out) {

  const uint8_t *parents_array[2];
  size_t         parents_array_len = 0;

  while (num_chaining_values - (2 * parents_array_len) >= 2) {
    parents_array[parents_array_len] = &child_chaining_values[2 * parents_array_len * BLAKE3_OUT_LEN];
    parents_array_len               += 1;
  }

  blake3_hash_many(parents_array, parents_array_len, 1, key,
                   0, // Parents always use counter 0.
                   false, flags | PARENT,
                   0, // Parents have no start flags.
                   0, // Parents have no end flags.
                   out);

  // If there's an odd child left over, it becomes an output.
  if (num_chaining_values > 2 * parents_array_len) {
    memcpy(out + parents_array_len * BLAKE3_OUT_LEN,
           child_chaining_values + 2 * parents_array_len * BLAKE3_OUT_LEN,
           BLAKE3_OUT_LEN);
    return parents_array_len + 1;
  }
  else {
    return parents_array_len;
  }
}

/**
 * @brief Compress a subtree.
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
 * @param[out] out           Output of the compression.
 *
 * @return                   Number of compressed nodes.
 */
static size_t blake3_compress_subtree(const uint8_t *input, size_t input_len,
                                      const uint32_t *key, uint64_t chunk_counter,
                                      uint8_t flags, uint8_t *out) {

  // One chunk
  if (input_len <= BLAKE3_CHUNK_LEN) {
    return blake3_compress_chunks(input, input_len, key, chunk_counter, flags, out);
  }

  // At least two chunks
  // The left subtree consits of the first 2^(10+log((n-1)/1024)), where n is the input length
  // and he right substree consists of the remainder
  size_t left_input_len        = 1ULL << highest_one(((input_len - 1) / BLAKE3_CHUNK_LEN) | 1);
  left_input_len               = left_input_len * BLAKE3_CHUNK_LEN;
  size_t right_input_len       = input_len - left_input_len;
  const uint8_t *right_input   = &input[left_input_len];
  uint64_t right_chunk_counter = chunk_counter + (uint64_t)(left_input_len / BLAKE3_CHUNK_LEN);

  uint8_t  cv_array[2 * 2 * BLAKE3_OUT_LEN];
  size_t   degree = 1;
  uint8_t *right_cvs;
  size_t   left_n, right_n;

  if (left_input_len > BLAKE3_CHUNK_LEN) {
    degree = 2;
  }
  right_cvs = &cv_array[degree * BLAKE3_OUT_LEN];

  left_n    = blake3_compress_subtree(input, left_input_len, key,
                                      chunk_counter, flags, cv_array);
  right_n   = blake3_compress_subtree(right_input, right_input_len, key,
                                      right_chunk_counter, flags, right_cvs);

  if (1 == left_n) {
    memcpy(out, cv_array, 2 * BLAKE3_OUT_LEN);
    return 2;
  }

  // Otherwise, do one layer of parent node compression.
  size_t num_chaining_values = left_n + right_n;
  return blake3_compress_parents(cv_array, num_chaining_values, key, flags, out);
}

void blake3_compress_subtree_to_parent(const uint8_t *input, size_t input_len, const uint32_t *key,
                                       uint64_t chunk_counter, uint8_t flags, uint8_t *out) {

  uint8_t cv_array[2 * BLAKE3_OUT_LEN];
  size_t  num_cvs = blake3_compress_subtree(input, input_len, key,
                                           chunk_counter, flags, cv_array);

  uint8_t out_array[BLAKE3_OUT_LEN];
  while (num_cvs > 2) {
    num_cvs = blake3_compress_parents(cv_array, num_cvs, key, flags, out_array);
    memcpy(cv_array, out_array, num_cvs * BLAKE3_OUT_LEN);
  }
  memcpy(out, cv_array, 2 * BLAKE3_OUT_LEN);
}

void blake3_hasher_merge_cv(cx_blake3_t *hash, uint64_t total_len) {
  size_t                 post_merge_stack_len = (size_t)hw(total_len);
  cx_blake3_state_out_t  output;
  uint8_t               *parent_node;

  while (hash->cv_stack_len > post_merge_stack_len) {
    parent_node = hash->cv_stack + (hash->cv_stack_len - 2) * BLAKE3_OUT_LEN;
    memcpy(output.input_cv, hash->key, BLAKE3_OUT_LEN);
    memcpy(output.block, parent_node, BLAKE3_BLOCK_LEN);
    output.block_len = BLAKE3_BLOCK_LEN;
    output.counter   = 0;
    output.d         = (hash->chunk).d | PARENT;
    blake3_output_chain(&output, parent_node);
    hash->cv_stack_len -= 1;
  }
}

void blake3_hasher_push_cv(cx_blake3_t *hash, uint8_t *new_cv,
                           uint64_t chunk_counter) {
  blake3_hasher_merge_cv(hash, chunk_counter);
  memcpy(hash->cv_stack + hash->cv_stack_len * BLAKE3_OUT_LEN, new_cv, BLAKE3_OUT_LEN);
  hash->cv_stack_len += 1;
}

void blake3_output_root_bytes(const cx_blake3_state_out_t *chunk_out, uint8_t *out,
                              size_t out_len) {
  uint64_t output_block_counter = 0;
  size_t   offset_within_block  = 0;
  uint8_t  wide_buf[BLAKE3_BLOCK_LEN];

  while (out_len > 0) {
    blake3_compress_xof(chunk_out->input_cv, chunk_out->block, chunk_out->block_len,
                        output_block_counter, chunk_out->d | ROOT, wide_buf);
    size_t available_bytes = BLAKE3_BLOCK_LEN - offset_within_block;
    size_t memcpy_len;
    if (out_len > available_bytes) {
      memcpy_len = available_bytes;
    } else {
      memcpy_len = out_len;
    }
    memcpy(out, wide_buf + offset_within_block, memcpy_len);
    out                  += memcpy_len;
    out_len              -= memcpy_len;
    output_block_counter += 1;
    offset_within_block   = 0;
  }
}

void blake3_init_ctx(cx_blake3_t *hash, const uint32_t *key, uint8_t mode) {
  memcpy(hash->key, key, BLAKE3_KEY_LEN);
  blake3_state_init(&hash->chunk, key, mode);
  hash->cv_stack_len = 0;
  hash->is_init      = true;
}

#endif // HAVE_BLAKE3
