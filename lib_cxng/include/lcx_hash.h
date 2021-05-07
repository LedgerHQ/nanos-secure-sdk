
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

/*
 * This file is not intended to be included directly.
 * Include "lbcxng.h" instead
 */

#ifdef HAVE_HASH

#ifndef LCX_HASH_H
#define LCX_HASH_H

#include "cx_errors.h"
#include "lcx_wrappers.h"
#include "lcx_common.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/** Message Digest algorithm identifiers. */
enum cx_md_e {
  /** NONE Digest */
  CX_NONE = 0,
  /** RIPEMD160 Digest */
  CX_RIPEMD160 = 1, // 20 bytes
  /** SHA224 Digest */
  CX_SHA224 = 2, // 28 bytes
  /** SHA256 Digest */
  CX_SHA256 = 3, // 32 bytes
  /** SHA384 Digest */
  CX_SHA384 = 4, // 48 bytes
  /** SHA512 Digest */
  CX_SHA512 = 5, // 64 bytes
  /** Keccak (pre-SHA3) Digest */
  CX_KECCAK = 6, // 28,32,48,64 bytes
  /** SHA3 Digest */
  CX_SHA3 = 7, // 28,32,48,64 bytes
  /** Groestl Digest */
  CX_GROESTL = 8,
  /** Blake Digest */
  CX_BLAKE2B = 9,
  /** SHAKE-128 Digest */
  CX_SHAKE128 = 10, // any bytes
  /** SHAKE-128 Digest */
  CX_SHAKE256 = 11, // any bytes
  /** SHA3-256 Digest */
  CX_SHA3_256 = 12,
  /** SHA3-512 Digest */
  CX_SHA3_512 = 13,
};
/** Convenience type. See #cx_md_e. */
typedef enum cx_md_e cx_md_t;

/**
 * @internal
 * Maximum size of message for any digest. The size is given in block,
 */
#define CX_HASH_MAX_BLOCK_COUNT 65535

/** Convenience type. See #cx_hash_header_s. */
typedef struct cx_hash_header_s cx_hash_t;

/* Generic API */
typedef struct {
  cx_md_t md_type;
  size_t  output_size;
  size_t  block_size;
  cx_err_t (*init_func)(cx_hash_t *ctx);
  cx_err_t (*update_func)(cx_hash_t *ctx, const uint8_t *data, size_t len);
  cx_err_t (*finish_func)(cx_hash_t *ctx, uint8_t *digest);
  cx_err_t (*init_ex_func)(cx_hash_t *ctx, size_t output_size);
  size_t (*output_size_func)(const cx_hash_t *ctx);
} cx_hash_info_t;

/**
 * Common Message Digest context, used as abstract type.
 */
struct cx_hash_header_s {
  /** Hash description. */
  const cx_hash_info_t *info;
  /** Number of block already processed */
  uint32_t counter;
};

size_t cx_hash_get_size(const cx_hash_t *ctx);

/**
 * Add more data to hash.
 *
 * @param  [in/out] hash
 *   Univers Continuation Blob.
 *   The hash context pointer shall point to a well-defined hash struct cx_ripemd160_t, cx_sha256_t, ....
 *   The hash context shall be inited with 'cx_xxx_init'
 *   The hash context shall be in RAM
 *   The function should be called with a nice cast.
 *
 * @param  [in] mode
 *   Supported flags: CX_LAST
 *   If CX_LAST is set,
 *     - The struct is left not modified after finishing
 *     - if out is provided, hash is copied in out
 *     - context is NOT automatically re-inited.
 *
 * @param  [in] in
 *   Input data to add to current hash
 *
 * @param  [in] len
 *   Length of input to data.
 *
 * @param [out] out
 *   Either:
 *     - NULL (ignored) if CX_LAST is NOT set
 *     - produced hash  if CX_LAST is set
 *
 * @param [out] out_len
 *   Either:
 *     - O, if out is NULL
 *     - out buffer size, if buffer is too small to store the hash a exception is thrown
 *
 */
cx_err_t cx_hash_no_throw(cx_hash_t *hash, uint32_t mode, const uint8_t *in, size_t len, uint8_t *out, size_t out_len);

static inline int cx_hash ( cx_hash_t * hash, int mode, const unsigned char * in, unsigned int len, unsigned char * out, unsigned int out_len )
{
  CX_THROW(cx_hash_no_throw(hash, mode, in, len, out, out_len));
  return cx_hash_get_size(hash);
}

/**
 * Generic Initialization of hash context.
 *
 * @param [out] hash the context to init.
 *    The context shall be in RAM
 *
 * @param [in] hash_id the hash identifier.
 *
 * @return algorithm identifier
 */
cx_err_t cx_hash_init(cx_hash_t *hash, cx_md_t hash_id);

/**
 * Generic Initialization of hash context with parameter
 *
 * @param [out]
 *    hash the context to init.
 *    The context shall be in RAM
 *
 * @param [in] hash_id
 *    the hash identifier.
 *
 * @param [in] output_size
 *    for blake, groestl, shake.
 *
 * @return algorithm identifier, or 0 if the 'hash_id'
 *    identifier is not supported.
 */
cx_err_t cx_hash_init_ex(cx_hash_t *hash, cx_md_t hash_id, size_t output_size);

/**
 * Add more data to hash.
 *
 * Equivalent to cx_hash_no_throw(hash, 0, in, in_len NULL, 0);
 *
 * @param  [in/out] hash
 *   Univers Continuation Blob.
 *   The hash context pointer shall point to a well-defined hash struct cx_ripemd160_t, cx_sha256_t, ....
 *   The hash context shall be inited with 'cx_xxx_init'
 *   The hash context shall be in RAM
 *   The function should be called with a nice cast.
 *
 * @param  [in] in
 *   Input data to add to current hash
 *
 * @param  [in] in_len
 *   Length of input to data.
 */
cx_err_t cx_hash_update(cx_hash_t *hash, const uint8_t *in, size_t in_len);

/**
 * Finalize hash and get result
 *
 * Equivalent to cx_hash_no_throw(hash, CX_LAST, NULL, 0, out, <LEN>);
 *
 * @param [out] out
*     produced hash
 */
cx_err_t cx_hash_final(cx_hash_t *hash, uint8_t *digest);

#endif

#endif // HAVE_HASH
