
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

#if defined(HAVE_SHA256) || defined(HAVE_SHA224)

#ifndef LCX_SHA256_H
#define LCX_SHA256_H

#include "lcx_wrappers.h"
#include "lcx_hash.h"
#include <stddef.h>
#include <stdint.h>

#if defined(HAVE_SHA224)
/** SHA224 message digest size */
#define CX_SHA224_SIZE 28
#endif

/** SHA256 message digest size */
#define CX_SHA256_SIZE 32

/**
 * SHA-224 and SHA-256 context
 */
struct cx_sha256_s {
  /** @copydoc cx_ripemd160_s::header */
  struct cx_hash_header_s header;
  /** @internal @copydoc cx_ripemd160_s::blen */
  size_t blen;
  /** @internal @copydoc cx_ripemd160_s::block */
  uint8_t block[64];
  /** @copydoc cx_ripemd160_s::acc */
  uint8_t acc[8 * 4];
};
/** Convenience type. See #cx_sha256_s. */
typedef struct cx_sha256_s cx_sha256_t;

#if defined(HAVE_SHA224)
/**
 * Initialize a SHA-224 context.
 *
 * @param [out] hash the context to init.
 *    The context shall be in RAM
 *
 * @return algorithm identifier
 */
cx_err_t cx_sha224_init_no_throw(cx_sha256_t *hash);

static inline int cx_sha224_init ( cx_sha256_t * hash )
{
  cx_sha224_init_no_throw(hash);
  return CX_SHA224;
}
#endif

/**
 * Initialize a SHA-256 context.
 *
 * @param [out] hash the context to init.
 *    The context shall be in RAM
 *
 * @return algorithm identifier
 */
cx_err_t cx_sha256_init_no_throw(cx_sha256_t *hash);

static inline int cx_sha256_init ( cx_sha256_t * hash )
{
  cx_sha256_init_no_throw(hash);
  return CX_SHA256;
}

/**
 * One shot SHA-256 digest
 *
 * @param  [in] in
 *   Input data to compute the hash
 *
 * @param  [in] len
 *   Length of input data.
 *
 * @param [out] out
 *   'out' length is implicit
 *
 */
  size_t cx_hash_sha256(const uint8_t *in, size_t len, uint8_t *out, size_t out_len);

#endif

#endif // defined(HAVE_SHA256) || defined(HAVE_SHA224)
