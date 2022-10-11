
/*******************************************************************************
*   Ledger Nano S - Secure firmware
*   (c) 2022 Ledger
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
#if defined(HAVE_POLY1305)


#ifndef CX_POLY1305_H
#define CX_POLY1305_H

#include "lcx_poly1305.h"
#include "ox.h"
#include <stddef.h>


/**
 * @brief           Initialize the specified Poly1305 context.
 *
 * @details         It must be the first API called before using
 *                  the context.
 *
 *                  It is usually followed by a call to
 *                  #cx_poly1305_set_key, then one or more calls to
 *                  #cx_poly1305_update, then one call to
 *                  #cx_poly1305_finish
 *
 * @param ctx       The Poly1305 context to initialize. This must
 *                  not be \c NULL.
 */
void cx_poly1305_init(cx_poly1305_context_t *ctx);

/**
 * @brief           Set the one-time authentication key.
 *
 * @warning         The key must be unique and unpredictable for each
 *                  invocation of Poly1305.
 *
 * @param ctx       The Poly1305 context to which the key should be bound.
 *                  This must be initialized.
 *
 * @param key       Pointer to the buffer containing the \c 32 byte (\c 256 bit) key.
 *
 */
void cx_poly1305_set_key(cx_poly1305_context_t *ctx, const uint8_t *key);

/**
 * @brief           Update the Poly1305 computation given an input buffer.
 *
 * @details         It is called between #cx_poly1305_set_key() and
 *                  #cx_poly1305_finish().
 *                  It can be called repeatedly to process a stream of data.
 *
 * @param ctx       The Poly1305 context to use for the Poly1305 operation.
 *                  This must be initialized and bound to a key.
 * 
 * @param input     Pointer to the buffer holding the input data.
 *                  This pointer can be \c NULL if `in_len == 0`.
 *
 * @param in_len    The length of the input data in bytes.
 *
 * @return          Error code
 */
cx_err_t cx_poly1305_update(cx_poly1305_context_t *ctx, const uint8_t *input, size_t in_len);

/**
 * @brief           Generate the Poly1305 Message
 *                  Authentication Code (MAC).
 *
 * @param ctx       The Poly1305 context to use for the Poly1305 operation.
 *                  This must be initialized and bound to a key.
 *
 * @param tag       Pointer to the buffer to where the MAC is written. This must
 *                  be a writable buffer of length \c 16 bytes.
 *
 * @return          Error code.
 */
cx_err_t cx_poly1305_finish(cx_poly1305_context_t *ctx, uint8_t *tag);

/**
 * @brief           Calculate the Poly1305 MAC of the input
 *                  buffer with the provided key.
 *
 * @warning         The key must be unique and unpredictable for each
 *                  invocation of Poly1305.
 *
 * @param key       Pointer to the buffer containing the \c 32 byte (\c 256 bit) key.
 *
 * @param input     Pointer to the buffer holding the input data.
 *                  This pointer can be \c NULL if `in_len == 0`.
 *
 * @param in_len    Length of the input data in bytes.
 *
 * @param tag       Pointer to the buffer to where the MAC is written.
 *                  This must be a writable buffer of length \c 16 bytes.
 *
 * @return          Error code
 */
cx_err_t cx_poly1305_mac(const uint8_t *key,
                         const uint8_t *input, size_t in_len,
                         uint8_t *tag);

#endif /* CX_POLY1305_H */
#endif // HAVE_POLY1305
