
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

#ifndef LCX_BLAKE2_H
#define LCX_BLAKE2_H

#include <stddef.h>
#include <stdint.h>

/**  @private */
enum blake2b_constant {
  BLAKE2B_BLOCKBYTES = 128,
  BLAKE2B_OUTBYTES = 64,
  BLAKE2B_KEYBYTES = 64,
  BLAKE2B_SALTBYTES = 16,
  BLAKE2B_PERSONALBYTES = 16
};

/**  @private */
struct blake2b_state__ {
  uint64_t h[8];
  uint64_t t[2];
  uint64_t f[2];
  uint8_t buf[BLAKE2B_BLOCKBYTES];
  size_t buflen;
  size_t outlen;
  uint8_t last_node;
};
/** @private */
typedef struct blake2b_state__ blake2b_state;

/**
 * Blake2b context
 */
struct cx_blake2b_s {
  /** @copydoc cx_ripemd160_s::header */
  struct cx_hash_header_s header;
  /** @internal output digest size*/
  unsigned int output_size;
  struct blake2b_state__ ctx;
};
/** Convenience type. See #cx_blake2b_s. */
typedef struct cx_blake2b_s cx_blake2b_t;

/**
 * Init a blake2b context.
 *
 * Blake2b as specified at https://blake2.net.
 *
 * @param [out] hash  the context to init.
 *    The context shall be in RAM
 *
 * @param [in] size   output blake2b size, in BITS.
 *
 *
 * @return algorithm identifier
 */
CXCALL int cx_blake2b_init(cx_blake2b_t *hash PLENGTH(sizeof(cx_blake2b_t)),
                           unsigned int out_len);

/**
 * Init a blake2b context with salt and personalization string.
 *
 * Blake2b as specified at https://blake2.net.
 *
 * @param [out] hash  the context to init.
 *    The context shall be in RAM
 *
 * @param [in] size   output blake2b size, in BITS.
 *
 *
 * @return algorithm identifier
 */
CXCALL int cx_blake2b_init2(cx_blake2b_t *hash PLENGTH(sizeof(cx_blake2b_t)),
                            unsigned int out_len,
                            unsigned char *salt PLENGTH(salt_len),
                            unsigned int salt_len,
                            unsigned char *perso PLENGTH(perso_len),
                            unsigned int perso_len);

#endif
