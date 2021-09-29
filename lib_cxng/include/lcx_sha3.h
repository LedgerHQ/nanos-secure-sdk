
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

#ifdef HAVE_SHA3

#ifndef LCX_SHA3_H
#define LCX_SHA3_H

#include "lcx_common.h"
#include "lcx_hash.h"
#include <stddef.h>

/**
 * KECCAK, SHA3 and SHA3-XOF context
 */
struct cx_sha3_s {
  /** @copydoc cx_ripemd160_s::header */
  struct cx_hash_header_s header;

  /** @internal output digest size*/
  size_t output_size;
  /** @internal input block size*/
  size_t block_size;
  /** @internal @copydoc cx_ripemd160_s::blen */
  size_t blen;
  /** @internal @copydoc cx_ripemd160_s::block */
  uint8_t block[200];
  /** @copydoc cx_ripemd160_s::acc */
  uint64bits_t acc[25];
};
/** Convenience type. See #cx_sha3_s. */
typedef struct cx_sha3_s cx_sha3_t;

/**
 * Initialize a sha3 context.
 *
 * SHA3 family as specified in FIPS 202.
 * Supported output sizes are: 224,256,384,512
 *
 * @param [out] hash  the context to init.
 *    The context shall be in RAM
 *
 * @param [in] size   output sha3 size, in BITS.

 *
 * @return algorithm identifier
 */
cx_err_t cx_sha3_init_no_throw(cx_sha3_t *hash, size_t size);

static inline int cx_sha3_init ( cx_sha3_t * hash, size_t size )
{
  CX_THROW(cx_sha3_init_no_throw(hash, size));
  return CX_SHA3;
}

/**
 * Initialize a sha3 context.
 *
 * SHA3 family as specified in Keccak submission.
 * Supported output sizes are: 224,256,384,512
 *
 * @param [out] hash  the context to init.
 *    The context shall be in RAM
 *
 * @param [in] size   output sha3 size, in BITS.

 *
 * @return algorithm identifier
 */
cx_err_t cx_keccak_init_no_throw(cx_sha3_t *hash, size_t size);

static inline int cx_keccak_init ( cx_sha3_t * hash, size_t size )
{
  CX_THROW(cx_keccak_init_no_throw(hash, size));
  return CX_KECCAK;
}

/**
 * Init a sha3-XOF context.
 *
 * SHA3-XOF family as specified in FIPS 202.
 * Supported output sha3 sizes are: 256,512
 *
 * @param [out] hash        the context to init.
 *    The context shall be in RAM
 *
 * @param [in] out_size         desired output size, in BITS.
 *
 * @return algorithm identifier
 */
cx_err_t cx_shake128_init_no_throw(cx_sha3_t *hash, size_t out_size);

static inline int cx_shake128_init ( cx_sha3_t * hash, unsigned int out_size )
{
  CX_THROW(cx_shake128_init_no_throw(hash, out_size));
  return CX_SHAKE128;
}

/**
 * Init a sha3-XOF context.
 *
 * SHA3-XOF family as specified in FIPS 202.
 * Supported output sha3 sizes are: 256,512
 *
 * @param [out] hash        the context to init.
 *    The context shall be in RAM
 *
 * @param [in] out_size   desired output size, in BITS.
 *
 * @return algorithm identifier
 */
cx_err_t cx_shake256_init_no_throw(cx_sha3_t *hash, size_t out_size);

static inline int cx_shake256_init ( cx_sha3_t * hash, unsigned int out_size )
{
  CX_THROW(cx_shake256_init_no_throw(hash, out_size));
  return CX_SHAKE256;
}

/**
 * @deprecated
 *
 * Init a sha3-XOF context.
 *
 * SHA3-XOF family as specified in FIPS 202.
 * Supported output sha3 sizes are: 256,512
 *
 * @param [out] hash        the context to init.
 *    The context shall be in RAM
 *
 * @param [in] size         output sha3 size, in BITS.
 * @param [in] out_length   desired output size, in BYTES.
 *
 * @return algorithm identifier
 */
cx_err_t cx_sha3_xof_init_no_throw(cx_sha3_t *hash, size_t size, size_t out_length);

static inline int cx_sha3_xof_init ( cx_sha3_t * hash, unsigned int size, unsigned int out_length )
{
  CX_THROW(cx_sha3_xof_init_no_throw(hash, size, out_length));

  if (size == 128) {
    return CX_SHAKE128;
  } else {
    return CX_SHAKE256;
  }
}

#endif

#endif // HAVE_SHA3
