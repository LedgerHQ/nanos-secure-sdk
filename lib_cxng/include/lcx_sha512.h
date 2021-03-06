
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

#if defined(HAVE_SHA384) || defined(HAVE_SHA512)

#ifndef LCX_SHA512_H
#define LCX_SHA512_H

/** SHA384 message digest size */
#define CX_SHA384_SIZE 48
/** SHA512 message digest size */
#define CX_SHA512_SIZE 64

/**
 * SHA-384 and SHA-512 context
 */
struct cx_sha512_s {
  /** @copydoc cx_ripemd160_s::header */
  struct cx_hash_header_s header;
  /** @internal @copydoc cx_ripemd160_s::blen */
  size_t blen;
  /** @internal @copydoc cx_ripemd160_s::block */
  uint8_t block[128];
  /** @copydoc cx_ripemd160_s::acc */
  uint8_t acc[8 * 8];
};
/** Convenience type. See #cx_sha512_s. */
typedef struct cx_sha512_s cx_sha512_t;

/**
 * Initialize a SHA-384 context.
 *
 * @param [out] hash the context to initialize.
 *    The context shall be in RAM
 *
 * @return algorithm identifier
 */
cx_err_t cx_sha384_init_no_throw(cx_sha512_t *hash);

static inline int cx_sha384_init ( cx_sha512_t * hash )
{
  cx_sha384_init_no_throw(hash);
  return CX_SHA384;
}

/**
 * Initialize a SHA-512 context.
 *
 * @param [out] hash the context to initialize.
 *    The context shall be in RAM
 *
 * @return algorithm identifier
 */
cx_err_t cx_sha512_init_no_throw(cx_sha512_t *hash);

static inline int cx_sha512_init ( cx_sha512_t * hash )
{
  cx_sha512_init_no_throw(hash);
  return CX_SHA512;
}

/**
 * One shot SHA-512 digest
 *
 * @param  [in] in
 *   Input data to compute the hash
 *
 * @param  [in] in_len
 *   Length of input data.
 *
 * @param [out] out
 *   'out' buffer
 *
 * @param [out] out_len
 *   max 'out' length
 */
  size_t cx_hash_sha512(const uint8_t *in, size_t in_len, uint8_t *out, size_t out_len);

#endif

#endif // defined(HAVE_SHA384) || defined(HAVE_SHA512)
