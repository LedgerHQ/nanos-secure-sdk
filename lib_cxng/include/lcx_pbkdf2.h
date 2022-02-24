
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

/**
 * @file    lcx_pbkdf2.h
 * @brief   PBKDF2 (Password-Based Key Derivation Function)
 *
 * PBKDF2 is a key derivation function i.e. it produces a key
 * from a base key (a password) and other parameters (a salt
 * and an iteration counter).
 * It consists in iteratively deriving HMAC.
 */

#ifdef HAVE_PBKDF2

#ifndef LCX_PBKDF2_H
#define LCX_PBKDF2_H

#include "lcx_wrappers.h"
#include "lcx_hash.h"

/**
 * @brief   Compute PBKDF2 bytes sequence.
 * 
 * @details Compute PBKDF2 bytes sequence according to
 *          <a href="https://tools.ietf.org/html/rfc2898"> RFC 2898 </a>.
 *
 * @param[in]  md_type     Message digest algorithm identifier.
 *
 * @param[in]  password    Password used as a base key to compute
 *                         the HMAC.
 *
 * @param[in]  passwordlen Length of the password i.e. the length
 *                         of the HMAC key.
 *
 * @param[in]  salt        Initial salt.
 *
 * @param[in]  saltlen     Length of the salt.
 *
 * @param[in]  iterations  Per block iteration.
 *
 * @param[out] out         Buffer where to store the output.
 *
 * @param[in]  outLength   Lengh of the output.
 *
 * @return                 Error code:
 *                         - CX_OK
 *                         - CX_INVALID_PARAMETER
 */
cx_err_t cx_pbkdf2_no_throw(cx_md_t md_type,
                   const uint8_t *password,
                   size_t         passwordlen,
                   uint8_t *      salt,
                   size_t         saltlen,
                   uint32_t       iterations,
                   uint8_t *      out,
                   size_t         outLength);

/**
 * @brief   Compute PBKDF2 bytes sequence.
 * 
 * @details Compute PBKDF2 bytes sequence according to
 *          <a href="https://tools.ietf.org/html/rfc2898"> RFC 2898 </a>.
 *          This function throws an exception if the computation doesn't succeed.
 *
 * @param[in]  md_type     Message digest algorithm identifier.
 *
 * @param[in]  password    Password used as a base key to compute
 *                         the HMAC.
 *
 * @param[in]  passwordlen Length of the password i.e. the length
 *                         of the HMAC key.
 *
 * @param[in]  salt        Initial salt.
 *
 * @param[in]  saltlen     Length of the salt.
 *
 * @param[in]  iterations  Per block iteration.
 *
 * @param[out] out         Buffer where to store the output.
 *
 * @param[in]  outLength   Lengh of the output.
 *
 * @throws                 CX_INVALID_PARAMETER
 */
static inline void cx_pbkdf2 ( cx_md_t md_type, const unsigned char * password, unsigned short passwordlen, unsigned char * salt, unsigned short saltlen, unsigned int iterations, unsigned char * out, unsigned int outLength )
{
  CX_THROW(cx_pbkdf2_no_throw(md_type, password, passwordlen, salt, saltlen, iterations, out, outLength));
}

/**
 * @brief   Compute PBKDF2 bytes sequence with SHA512.
 * 
 * @details Compute PBKDF2 bytes sequence according to
 *          <a href="https://tools.ietf.org/html/rfc2898">  RFC 2898 </a>
 *          with SHA512 as the underlying hash function.
 *
 * @param[in]  password     Password used as a base key to compute
 *                          the HMAC.
 *
 * @param[in]  password_len Length of the password i.e. the length
 *                          of the HMAC key.
 *
 * @param[in]  salt         Initial salt.
 *
 * @param[in]  salt_len     Length of the salt.
 *
 * @param[in]  iterations   Per block iteration.
 *
 * @param[out] out          Buffer where to store the output.
 *
 * @param[in]  out_len      Lengh of the output.
 *
 * @return                  Error code:
 *                          - CX_OK
 *                          - CX_INVALID_PARAMETER
 */
#define cx_pbkdf2_sha512(password, password_len, salt, salt_len, iterations, out, out_len) \
    cx_pbkdf2_no_throw(CX_SHA512, password, password_len, salt, salt_len, iterations, out, out_len)

#endif  // LCX_PBKDF2_H

#endif // HAVE_PBKDF2
