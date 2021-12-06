
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
 * @file    lcx_ecdh.h
 * @brief   ECDH (Elliptic Curve Diffie Hellman) key exchange.
 *
 * ECDH is a key agreement protocol that allows two parties to calculate
 * a shared secret over an insecure channel. The public and private keys
 * are elements of a chosen elliptic curve.
 */

#ifdef HAVE_ECDH

#ifndef LCX_ECDH_H
#define LCX_ECDH_H

#if defined(HAVE_ECDH) || defined(HAVE_X25519) || defined(HAVE_X448)

#include "lcx_wrappers.h"
#include "lcx_ecfp.h"
#include <stddef.h>

#if defined(HAVE_ECDH)
/**
 * @brief   Compute an ECDH shared secret.
 * 
 * @details Depending on the mode, the shared secret is either the full point or
 *          only the *x* coordinate.
 * 
 * @param[in]  pvkey        Private key.
 *                          Shall be initialized with #cx_ecfp_init_private_key_no_throw.
 * 
 * @param[in]  mode         Crypto mode flags. 
 *                          Supported flags:
 *                           - CX_ECDH_POINT
 *                           - CX_ECDH_X
 * 
 * @param[in]  P            Pointer to the public key encoded as **04 || x || y**:
 *                          *x* and *y* are encoded as big endian raw values
 *                          and have a binary length equal to curve domain size.
 * 
 * @param[in]  P_len        Length of the public key in octets.
 * 
 * @param[out] secret       Buffer where to store the shared secret (full or compressed).
 * 
 * @param[in]  secret_len   Length of the buffer in octets.
 * 
 * @return                  Error code:
 *                          - CX_OK on success
 *                          - CX_INVALID_PARAMETER
 *                          - INVALID_PARAMETER
 *                          - CX_EC_INVALID_CURVE
 *                          - CX_MEMORY_FULL
 *                          - CX_NOT_LOCKED
 *                          - CX_EC_INVALID_POINT
 *                          - CX_INVALID_PARAMETER_SIZE
 *                          - CX_EC_INFINITE_POINT
 */
cx_err_t cx_ecdh_no_throw(const cx_ecfp_private_key_t *pvkey,
                 uint32_t                     mode,
                 const uint8_t *              P,
                 size_t                       P_len,
                 uint8_t *                    secret,
                 size_t                       secret_len);

/**
 * @brief   Compute an ECDH shared secret.
 * 
 * @details Depending on the mode, the shared secret is either the full point or
 *          only the *x* coordinate.
 *          This function throws an exception if the computation doesn't succeed.
 * 
 * @param[in]  pvkey        Private key.
 *                          Shall be initialized with #cx_ecfp_init_private_key_no_throw.
 * 
 * @param[in]  mode         Crypto mode flags. 
 *                          Supported flags:
 *                           - CX_ECDH_POINT
 *                           - CX_ECDH_X
 * 
 * @param[in]  P            Pointer to the public key encoded as **04 || x || y**:
 *                          *x* and *y* are encoded as big endian raw values
 *                          and have a binary length equal to curve domain size.
 * 
 * @param[in]  P_len        Length of the public key in octets.
 * 
 * @param[out] secret       Buffer where to store the shared secret (full or compressed).
 * 
 * @param[in]  secret_len   Length of the buffer in octets.
 * 
 * @return                  Length of the shared secret.
 
 * @throws                  CX_INVALID_PARAMETER
 * @throws                  INVALID_PARAMETER
 * @throws                  CX_EC_INVALID_CURVE
 * @throws                  CX_MEMORY_FULL
 * @throws                  CX_NOT_LOCKED
 * @throws                  CX_EC_INVALID_POINT
 * @throws                  CX_INVALID_PARAMETER_SIZE
 * @throws                  CX_EC_INFINITE_POINT
 */
static inline int cx_ecdh ( const cx_ecfp_private_key_t * pvkey, int mode, const unsigned char * P, unsigned int P_len, unsigned char * secret, unsigned int secret_len )
{
  CX_THROW(cx_ecdh_no_throw(pvkey, mode, P, P_len, secret, secret_len));

  size_t size;

  CX_THROW(cx_ecdomain_parameters_length(pvkey->curve, &size));
  if ((mode & CX_MASK_EC) == CX_ECDH_POINT) {
    return 1 + 2 * size;
  } else {
    return size;
  }
}

#endif // HAVE_ECDH

#if defined(HAVE_X25519)
/**
 * @brief   Perform a scalar multiplication on Curve25519 with u-coordinate only.
 *
 * @details The notation (u, v) is used for Montgomery curves while (x, y)
 *          is used for the birational equivalents (Edwards curves).
 *
 * @param[in]  u     The u-coordinate of the point.
 *
 * @param[in]  k     The scalar.
 *
 * @param[in]  k_len Length of the scalar.
 *
 * @param[out] u     The u-coordinate of the resulting point.
 *
 * @return           Error code:
 *                   - CX_OK on success
 *                   - CX_EC_INVALID_CURVE
 *                   - CX_NOT_UNLOCKED
 *                   - CX_MEMORY_FULL
 *                   - CX_INVALID_PARAMETER
 *                   - CX_NOT_LOCKED
 *                   - CX_INVALID_PARAMETER_SIZE
 *                   - CX_EC_INFINITE_POINT
 *                   - CX_INVALID_PARAMETER_VALUE
 */
cx_err_t cx_x25519(uint8_t *u, const uint8_t *k, size_t k_len);
#endif // HAVE_X25519

#if defined(HAVE_X448)
/**
 * @brief   Perform a scalar multiplication on Curve448 with u-coordinate only.
 *
 * @details The notation (u, v) is used for Montgomery curves while (x, y)
 *          is used for the birational equivalents (Edwards curves).
 *
 * @param[in]  u     The u-coordinate of the point.
 *
 * @param[in]  k     The scalar.
 *
 * @param[in]  k_len Length of the scalar.
 *
 * @param[out] u     The u-coordinate of the resulting point.
 *
 * @return           Error code:
 *                   - CX_OK on success
 *                   - CX_EC_INVALID_CURVE
 *                   - CX_NOT_UNLOCKED
 *                   - CX_MEMORY_FULL
 *                   - CX_INVALID_PARAMETER
 *                   - CX_NOT_LOCKED
 *                   - CX_INVALID_PARAMETER_SIZE
 *                   - CX_EC_INFINITE_POINT
 *                   - CX_INVALID_PARAMETER_VALUE
 */
cx_err_t cx_x448(uint8_t *u, const uint8_t *k, size_t k_len);
#endif // HAVE_X448

#endif // HAVE_ECDH || HAVE_X25519 || HAVE_X448

#endif // HAVE_ECDH

#endif // LCX_ECDH_H
