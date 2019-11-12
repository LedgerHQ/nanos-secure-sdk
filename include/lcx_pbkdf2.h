
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
#ifndef LCX_PBKDF2_H
#define LCX_PBKDF2_H

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
CXCALL void cx_pbkdf2(cx_md_t md_type,
                      const unsigned char WIDE *password PLENGTH(passwordlen),
                      unsigned short passwordlen,
                      unsigned char *salt PLENGTH(saltlen),
                      unsigned short saltlen, unsigned int iterations,
                      unsigned char *out PLENGTH(outLength),
                      unsigned int outLength);

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
#define cx_pbkdf2_sha512(password, password_len, salt, salt_len, iterations,   \
                         out, out_len)                                         \
  cx_pbkdf2(CX_SHA512, password, password_len, salt, salt_len, iterations,     \
            out, out_len)

#endif
