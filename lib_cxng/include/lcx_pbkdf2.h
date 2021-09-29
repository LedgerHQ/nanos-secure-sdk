
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

#ifdef HAVE_PBKDF2

#ifndef LCX_PBKDF2_H
#define LCX_PBKDF2_H

#include "lcx_wrappers.h"
#include "lcx_hash.h"

/**
 * Compute pbkdf2 bytes sequence as specified by RFC 2898.
 *
 * @param [in]  md_type
 *    The hash algo (CX_SHA512, CX_SHA256, ...)
 *
 * @param [in]  password
 *    The hmac key
 *
 * @param [in]  passwordlen
 *    The hmac key bytes length
 *
 * @param [in]  salt
 *    The initial salt.
 *
 * @param [in]  saltlen
 *    The salt key bytes length
 *
 * @param [in]  iterations
 *    Per block iteration.
 *
 * @param [in]  out
 *    Where to put result.
 *
 * @param [in] outLength
 *    How many bytes to generate.
 *
 */

cx_err_t cx_pbkdf2_no_throw(cx_md_t md_type,
                   const uint8_t *password,
                   size_t         passwordlen,
                   uint8_t *      salt,
                   size_t         saltlen,
                   uint32_t       iterations,
                   uint8_t *      out,
                   size_t         outLength);

static inline void cx_pbkdf2 ( cx_md_t md_type, const unsigned char * password, unsigned short passwordlen, unsigned char * salt, unsigned short saltlen, unsigned int iterations, unsigned char * out, unsigned int outLength )
{
  CX_THROW(cx_pbkdf2_no_throw(md_type, password, passwordlen, salt, saltlen, iterations, out, outLength));
}

/**
 * Compute pbkdf2 bytes sequence as specified by RFC 2898.
 * The undelying hash function is SHA512
 *
 * @param [in]  password
 *    The hmac key
 *
 * @param [in]  passwordlen
 *    The hmac key bytes length
 *
 * @param [in]  salt
 *    The initial salt.
 *
 * @param [in]  saltlen
 *    The salt key bytes length
 *
 * @param [in]  iterations
 *    Per block iteration.
 *
 * @param [in]  out
 *    Where to put result.
 *
 * @param [in] outLength
 *    How many bytes to generate.
 *
 */
#define cx_pbkdf2_sha512(password, password_len, salt, salt_len, iterations, out, out_len) \
    cx_pbkdf2_no_throw(CX_SHA512, password, password_len, salt, salt_len, iterations, out, out_len)

#endif  //LCX_PBKDF2_H

#endif // HAVE_PBKDF2
