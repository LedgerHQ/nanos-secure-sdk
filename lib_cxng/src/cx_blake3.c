#ifdef HAVE_BLAKE3

#include "cx_blake3_ref.h"
#include "cx_utils.h"
#include "cx_ram.h"

#include <string.h>

/**
 * Initializing the hash context in the default hashing mode.
 * The key words are the initialization vectors.
 */
cx_err_t cx_blake3_init_default(cx_blake3_t *hash) {
  blake3_init_ctx(hash, IV, 0);
  return CX_OK;
}

/**
 * Initializing the hash context in the keyed hashing mode.
 */
cx_err_t cx_blake3_init_keyed(cx_blake3_t *hash, const uint8_t *key) {
  uint32_t key_words[BLAKE3_NB_OF_WORDS];
  load_key_words(key, key_words);
  blake3_init_ctx(hash, key_words, KEYED_HASH);
  return CX_OK;
}

/**
 * Initialize the hash context in the key derivation mode
 * context is the context string used as an initialization parameter,
 * it should be hardcoded, globally unique and application-specific
 */
cx_err_t cx_blake3_init_derive_key(cx_blake3_t *hash, const void *context, size_t context_len) {
  uint8_t     context_key[BLAKE3_KEY_LEN];
  uint32_t    context_key_words[BLAKE3_NB_OF_WORDS];

  // The context string context is hashed with the key words set to IV_0,...,IV_7
  // The DERIVE_KEY_CONTEXT flag is set for every compression
  // Then the key material context_key_words is hashed with the key words set to the
  // the first 8 output words of the first stage (context_key)

  blake3_init_ctx(hash, IV, DERIVE_KEY_CONTEXT);
  cx_blake3_update(hash, context, context_len);
  cx_blake3_final(hash, context_key, BLAKE3_KEY_LEN);
  load_key_words(context_key, context_key_words);
  blake3_init_ctx(hash, context_key_words, DERIVE_KEY_MATERIAL);

  return CX_OK;
}

cx_err_t cx_blake3_init(cx_blake3_t *hash, uint8_t mode, const uint8_t *key,
                        const void *context, size_t context_len) {
  switch (mode) {
  case 0:
    cx_blake3_init_default(hash);
    break;
  case KEYED_HASH:
    if (NULL == key) {
      return CX_INVALID_PARAMETER;
    }
    cx_blake3_init_keyed(hash, key);
    break;
  case DERIVE_KEY_CONTEXT:
    if ((NULL == context) || (!context_len)) {
      return CX_INVALID_PARAMETER;
    }
    cx_blake3_init_derive_key(hash, context, context_len);
    break;
  default:
    return CX_INVALID_PARAMETER;
  }
  return CX_OK;
}

cx_err_t cx_blake3_update(cx_blake3_t *hash, const void *input, size_t input_len) {
  const uint8_t        *input_bytes = (const uint8_t *)input;
  size_t                state_len;
  size_t                nb_bytes;

  if (!input_len) {
    return CX_OK;
  }

  state_len = BLAKE3_BLOCK_LEN * (hash->chunk).blocks_compressed + (hash->chunk).buffer_len;

  if (state_len > 0) {
    nb_bytes = BLAKE3_CHUNK_LEN - state_len;
    if (nb_bytes > input_len) {
      nb_bytes = input_len;
    }
    blake3_state_update(&hash->chunk, input_bytes, nb_bytes);
    input_bytes += nb_bytes;
    input_len   -= nb_bytes;

    if (input_len > 0) {
      cx_blake3_state_out_t output = blake3_state_output(&hash->chunk);
      uint8_t               chunk_cv[32];
      blake3_output_chain(&output, chunk_cv);
      blake3_hasher_push_cv(hash, chunk_cv, (hash->chunk).t);
      blake3_state_reset(&hash->chunk, hash->key, (hash->chunk).t + 1);
    } else {
      return CX_OK;
    }
  }

  while (input_len > BLAKE3_CHUNK_LEN) {
    size_t   subtree_len;
    uint64_t count;
    uint64_t subtree_chunks;
    subtree_len = 1ULL << highest_one(input_len | 1);
    count       = (hash->chunk).t * BLAKE3_CHUNK_LEN;

    while ((((uint64_t)(subtree_len - 1)) & count) != 0) {
      subtree_len /= 2;
    }

    subtree_chunks = subtree_len / BLAKE3_CHUNK_LEN;
    if (subtree_len <= BLAKE3_CHUNK_LEN) {
      cx_blake3_state_t    chunk_state;
      uint8_t              cv[BLAKE3_OUT_LEN];
      blake3_state_init(&chunk_state, hash->key, (hash->chunk).d);
      chunk_state.t = (hash->chunk).t;
      blake3_state_update(&chunk_state, input_bytes, subtree_len);
      cx_blake3_state_out_t output = blake3_state_output(&chunk_state);
      blake3_output_chain(&output, cv);
      blake3_hasher_push_cv(hash, cv, chunk_state.t);
    }
    else {

      uint8_t cv_pair[2 * BLAKE3_OUT_LEN];
      blake3_compress_subtree_to_parent(input_bytes, subtree_len, hash->key,
                                       (hash->chunk).t,
                                       (hash->chunk).d, cv_pair);
      blake3_hasher_push_cv(hash, cv_pair, (hash->chunk).t);
      blake3_hasher_push_cv(hash, cv_pair + BLAKE3_OUT_LEN,
                           (hash->chunk).t + (subtree_chunks / 2));
    }
    (hash->chunk).t += subtree_chunks;
    input_bytes     += subtree_len;
    input_len       -= subtree_len;
  }

  if (input_len > 0) {
    blake3_state_update(&hash->chunk, input_bytes, input_len);
    blake3_hasher_merge_cv(hash, (hash->chunk).t);
  }
  return CX_OK;
}

cx_err_t cx_blake3_final(cx_blake3_t *hash, uint8_t *output, size_t out_len) {

  if (!out_len) {
    return CX_INVALID_PARAMETER;
  }
  cx_blake3_state_out_t chunk_out;

  // If the subtree stack is empty, then the current chunk is the root.
  if (!hash->cv_stack_len) {
    chunk_out = blake3_state_output(&hash->chunk);
    blake3_output_root_bytes(&chunk_out, output, out_len);
    return CX_OK;
  }

  size_t  cvs_remaining;
  size_t  state_len = BLAKE3_BLOCK_LEN * (hash->chunk).blocks_compressed + (hash->chunk).buffer_len;
  uint8_t parent_block[BLAKE3_BLOCK_LEN];

  if (state_len > 0) {
    cvs_remaining = hash->cv_stack_len;
    chunk_out     = blake3_state_output(&hash->chunk);
  }
  else {
    // There are always at least 2 CVs in the stack in this case.
    cvs_remaining = hash->cv_stack_len - 2;
    memcpy(chunk_out.input_cv, hash->key, BLAKE3_OUT_LEN);
    memcpy(chunk_out.block, hash->cv_stack + cvs_remaining * BLAKE3_OUT_LEN, BLAKE3_BLOCK_LEN);
    chunk_out.block_len = BLAKE3_BLOCK_LEN;
    chunk_out.counter   = 0;
    chunk_out.d         = (hash->chunk).d | PARENT;

  }
  while (cvs_remaining > 0) {
    cvs_remaining -= 1;
    memcpy(parent_block, hash->cv_stack + cvs_remaining * BLAKE3_OUT_LEN, BLAKE3_OUT_LEN);
    blake3_output_chain(&chunk_out, parent_block + BLAKE3_OUT_LEN);
    memcpy(chunk_out.input_cv, hash->key, BLAKE3_OUT_LEN);
    memcpy(chunk_out.block, parent_block, BLAKE3_BLOCK_LEN);
    chunk_out.block_len = BLAKE3_BLOCK_LEN;
    chunk_out.counter   = 0;
    chunk_out.d         = (hash->chunk).d | PARENT;
  }
  blake3_output_root_bytes(&chunk_out, output, out_len);

  return CX_OK;
}

static bool cx_blake3_validate_context(const cx_blake3_t *hash) {
  if ((NULL == hash) || !hash->is_init) {
    return false;
  }
  return true;
}

cx_err_t cx_blake3(cx_blake3_t *hash, uint8_t mode, const void *input,
                   size_t input_len, uint8_t *out, size_t out_len) {
  cx_err_t error;
  // Check the context
  if (!cx_blake3_validate_context(hash)) {
    return CX_INTERNAL_ERROR;
  }
  CX_CHECK(cx_blake3_update(hash, input, input_len));

  if (mode & LAST) {
    CX_CHECK(cx_blake3_final(hash, out, out_len));
  }
  error = CX_OK;
  end:
    return error;
}

#endif // HAVE_BLAKE3
