
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
 * @file    lcx_ecfp.h
 * @brief   Key pair generation based on elliptic curves.
 *
 * Private and public keys initialization and key pair generation based on elliptic curves.
 */


#ifdef HAVE_ECC
#ifndef LCX_ECFP_H
#define LCX_ECFP_H

#include "lcx_wrappers.h"
#include "lcx_hash.h"
#include "ox_ec.h"
#include <stdbool.h>

/** Elliptic Curve public key */
struct cx_ecfp_public_key_s {
  cx_curve_t curve;  ///< Curve identifier
  size_t W_len;      ///< Public key length in bytes
  uint8_t W[1];      ///< Public key value starting at offset 0
};

/** Elliptic Curve private key */
struct cx_ecfp_private_key_s {
  cx_curve_t curve;  ///< Curve identifier
  size_t d_len;      ///< Private key length in bytes
  uint8_t d[1];      ///< Private key value starting at offset 0
};

/** Up to 256-bit Elliptic Curve public key */
struct cx_ecfp_256_public_key_s {
  cx_curve_t curve;  ///< Curve identifier
  size_t W_len;      ///< Public key length in bytes
  uint8_t W[65];     ///< Public key value starting at offset 0
};
/** Up to 256-bit Elliptic Curve private key */
struct cx_ecfp_256_private_key_s {
  cx_curve_t curve;  ///< Curve identifier
  size_t d_len;      ///< Private key length in bytes
  uint8_t d[32];     ///< Private key value starting at offset 0
};
/** Up to 256-bit Elliptic Curve extended private key */
struct cx_ecfp_256_extended_private_key_s {
  cx_curve_t curve;  ///< Curve identifier
  size_t d_len;      ///< Public key length in bytes
  uint8_t d[64];     ///< Public key value starting at offset 0
};
/** Convenience type. See #cx_ecfp_256_public_key_s. */
typedef struct cx_ecfp_256_public_key_s cx_ecfp_256_public_key_t;
/** temporary def type. See #cx_ecfp_256_private_key_s. */
typedef struct cx_ecfp_256_private_key_s cx_ecfp_256_private_key_t;
/** Convenience type. See #cx_ecfp_256_extended_private_key_s. */
typedef struct cx_ecfp_256_extended_private_key_s cx_ecfp_256_extended_private_key_t;

/* Do not use those types anymore for declaration, they will become abstract */
typedef struct cx_ecfp_256_public_key_s  cx_ecfp_public_key_t;
typedef struct cx_ecfp_256_private_key_s cx_ecfp_private_key_t;

/** Up to 384-bit Elliptic Curve public key */
struct cx_ecfp_384_public_key_s {
  cx_curve_t curve;   ///< Curve identifier
  size_t W_len;       ///< Public key length in bytes
  uint8_t W[97];      ///< Public key value starting at offset 0
};
/** Up to 384-bit Elliptic Curve private key */
struct cx_ecfp_384_private_key_s {
  cx_curve_t curve;   ///< Curve identifier
  size_t d_len;       ///< Private key length in bytes
  uint8_t d[48];      ///< Private key value starting at offset 0
};
/** Convenience type. See #cx_ecfp_384_public_key_s. */
typedef struct cx_ecfp_384_private_key_s cx_ecfp_384_private_key_t;
/** Convenience type. See #cx_ecfp_384_private_key_s. */
typedef struct cx_ecfp_384_public_key_s cx_ecfp_384_public_key_t;

/** Up to 512-bit Elliptic Curve public key */
struct cx_ecfp_512_public_key_s {
  cx_curve_t curve;   ///< Curve identifier
  size_t W_len;       ///< Public key length in bytes
  uint8_t W[129];     ///< Public key value starting at offset 0
};
/** Up to 512-bit Elliptic Curve private key */
struct cx_ecfp_512_private_key_s {
  cx_curve_t curve;   ///< Curve identifier
  size_t d_len;       ///< Private key length in bytes
  uint8_t d[64];      ///< Private key value starting at offset 0
};
/** Up to 512-bit Elliptic Curve extended private key */
struct cx_ecfp_512_extented_private_key_s {
  cx_curve_t curve;   ///< Curve identifier
  size_t d_len;       ///< Private key length in bytes
  uint8_t d[128];     ///< Private key value starting at offset 0
};
/** Convenience type. See #cx_ecfp_512_public_key_s. */
typedef struct cx_ecfp_512_public_key_s cx_ecfp_512_public_key_t;
/** Convenience type. See #cx_ecfp_512_private_key_s. */
typedef struct cx_ecfp_512_private_key_s cx_ecfp_512_private_key_t;
/** Convenience type. See #cx_ecfp_512_extented_private_key_s. */
typedef struct cx_ecfp_512_extented_private_key_s cx_ecfp_512_extented_private_key_t;

/** Up to 640-bit Elliptic Curve public key */
struct cx_ecfp_640_public_key_s {
  cx_curve_t curve;   ///< Curve identifier
  size_t W_len;       ///< Public key length in bytes
  uint8_t W[161];     ///< Public key value starting at offset 0
};
/** Up to 640-bit Elliptic Curve private key */
struct cx_ecfp_640_private_key_s {
  cx_curve_t curve;   ///< Curve identifier
  size_t d_len;       ///< Private key length in bytes
  uint8_t d[80];      ///< Private key value starting at offset 0
};
/** Convenience type. See #cx_ecfp_640_public_key_s. */
typedef struct cx_ecfp_640_public_key_s cx_ecfp_640_public_key_t;
/** Convenience type. See #cx_ecfp_640_private_key_s. */
typedef struct cx_ecfp_640_private_key_s cx_ecfp_640_private_key_t;

/**
 * @brief   Add two points of an elliptic curve.
 *
 * @param[in]  curve Curve identifier.
 * 
 * @param[out] R     Resulting point encoded as **04 || x || y**.
 * 
 * @param[in]  P     First operand: point on curve encoded as **04 || x || y**:
 *                   *x* and *y* are encoded as big endian raw values
 *                   and have a binary length equal to curve domain size.
 * 
 * @param[in]  Q     Second operand: point on curve encoded as **04 || x || y**.
 * 
 * @return           Error code:
 *                   - CX_OK on success
 *                   - CX_EC_INVALID_CURVE
 *                   - CX_NOT_UNLOCKED
 *                   - CX_INVALID_PARAMETER_SIZE
 *                   - CX_EC_INVALID_CURVE
 *                   - CX_MEMORY_FULL
 *                   - CX_NOT_LOCKED
 *                   - CX_INVALID_PARAMETER
 *                   - CX_EC_INVALID_POINT
 *                   - CX_EC_INFINITE_POINT
 */
cx_err_t cx_ecfp_add_point_no_throw(cx_curve_t curve, uint8_t *R, const uint8_t *P, const uint8_t *Q);

/**
 * @brief   Add two points of an elliptic curve.
 * 
 * @details This function throws an exception if the
 *          computation doesn't succeed.
 * 
 * @param[in]  curve Curve identifier.
 * 
 * @param[out] R     Resulting point encoded as **04 || x || y**.
 * 
 * @param[in]  P     First operand: point on curve encoded as **04 || x || y**:
 *                   *x* and *y* are encoded as big endian raw values
 *                   and have a binary length equal to curve domain size.
 * 
 * @param[in]  Q     Second operand: point on curve encoded as **04 || x || y**.
 * 
 * @param[in]  X_len Length of the x-coordinate. This parameter is not used.
 * 
 * @return           Length of the encoded point.

 * @throws           CX_EC_INVALID_CURVE
 * @throws           CX_NOT_UNLOCKED
 * @throws           CX_INVALID_PARAMETER_SIZE
 * @throws           CX_EC_INVALID_CURVE
 * @throws           CX_MEMORY_FULL
 * @throws           CX_NOT_LOCKED
 * @throws           CX_INVALID_PARAMETER
 * @throws           CX_EC_INVALID_POINT
 * @throws           CX_EC_INFINITE_POINT
 */
static inline int cx_ecfp_add_point ( cx_curve_t curve, unsigned char * R, const unsigned char * P, const unsigned char * Q, unsigned int X_len )
{
  UNUSED(X_len);

  CX_THROW(cx_ecfp_add_point_no_throw(curve, R, P, Q));

  size_t size;
  CX_THROW(cx_ecdomain_parameters_length(curve, &size));

  return 1 + 2 * size;
}

/**
 * @brief   Perform a scalar multiplication over an elliptic curve.
 * 
 * @param[in]      curve Curve identifier.
 * 
 * @param[in, out] P     Point on curve encoded as **04 || x || y**:
 *                       x and y are encoded as big endian raw values
 *                       and have a binary length equal to curve domain size.
 *                       This is also used for the result.
 * 
 * @param[in]      k     Scalar encoded as big endian integer.
 * 
 * @param[in]      k_len Length of the scalar. This should be equal to
 *                       the curve domain length.
 * 
 * @return               Error code:
 *                       - CX_OK on success
 *                       - CX_EC_INVALID_CURVE
 *                       - CX_NOT_UNLOCKED
 *                       - CX_EC_INVALID_CURVE
 *                       - CX_MEMORY_FULL
 *                       - CX_NOT_LOCKED
 *                       - CX_INVALID_PARAMETER
 *                       - CX_EC_INFINITE_POINT
 */
cx_err_t cx_ecfp_scalar_mult_no_throw(cx_curve_t curve, uint8_t *P, const uint8_t *k, size_t k_len);

/**
 * @brief   Perform a scalar multiplication over an elliptic curve.
 * 
 * @details This function throws an exception if the computation
 *          doesn't succeed.
 * 
 * @param[in]      curve Curve identifier.
 * 
 * @param[in, out] P     Point on curve encoded as **04 || x || y**:
 *                       x and y are encoded as big endian raw values
 *                       and have a binary length equal to curve domain size.
 *                       This is also used for the result.
 * 
 * @param[in]      P_len Length of the input point. This parameter is not used.
 * 
 * @param[in]      k     Scalar encoded as big endian integer.
 * 
 * @param[in]      k_len Length of the scalar. This should be equal to
 *                       the curve domain length.
 * 
 * @return               Length of the encoded point.
 *
 * @throws               CX_EC_INVALID_CURVE
 * @throws               CX_NOT_UNLOCKED
 * @throws               CX_EC_INVALID_CURVE
 * @throws               CX_MEMORY_FULL
 * @throws               CX_NOT_LOCKED
 * @throws               CX_INVALID_PARAMETER
 * @throws               CX_EC_INFINITE_POINT
 */
static inline int cx_ecfp_scalar_mult ( cx_curve_t curve, unsigned char * P, unsigned int P_len, const unsigned char * k, unsigned int k_len )
{
  UNUSED(P_len);

  CX_THROW(cx_ecfp_scalar_mult_no_throw(curve, P, k, k_len));

  size_t size;
  CX_THROW(cx_ecdomain_parameters_length(curve, &size));

  return 1 + 2 * size;
}

/**
 * @brief   Initialize a public key.
 * 
 * @param[in]  curve   Curve identifier.
 * 
 * @param[in]  rawkey  Pointer to a raw key value or NULL pointer
 *                     The value shall be the public point encoded as:
 *                      - **04 || x || y** for Weiertrass curves
 *                      - **04 || x || y**  or **02 || y** (plus sign) for Twisted Edward curves
 *                      - **04 || x || y**  or **02 || x** for Montgomery curves
 * 
 *                     where *x* and *y* are encoded as big endian raw values and have a
 *                     binary length equal to the curve domain size.
 * 
 * @param[in]  key_len Length of the key.
 * 
 * @param[out] key     Public key to initialize.
 * 
 * @return             Error code:
 *                     - CX_OK on success
 *                     - CX_EC_INVALID_CURVE
 *                     - INVALID_PARAMETER
 */
cx_err_t cx_ecfp_init_public_key_no_throw(cx_curve_t            curve,
                                 const uint8_t *       rawkey,
                                 size_t                key_len,
                                 cx_ecfp_public_key_t *key);

/**
 * @brief   Initialize a public key.
 * 
 * @details This function throws an exception
 *          if the computation doesn't succeed.
 * 
 * @param[in]  curve   Curve identifier.
 * 
 * @param[in]  rawkey  Pointer to a raw key value or NULL pointer
 *                     The value shall be the public point encoded as:
 *                      - **04 || x || y** for Weiertrass curves
 *                      - **04 || x || y**  or **02 || y** (plus sign) for Twisted Edward curves
 *                      - **04 || x || y**  or **02 || x** for Montgomery curves
 * 
 *                     where *x* and *y* are encoded as big endian raw values and have a
 *                     binary length equal to the curve domain size.
 * 
 * @param[in]  key_len Length of the key.
 * 
 * @param[out] key     Public key to initialize.
 * 
 * @return             Length of the key.
 * 
 * @throws             CX_EC_INVALID_CURVE
 * @throws             INVALID_PARAMETER
 */
static inline int cx_ecfp_init_public_key ( cx_curve_t curve, const unsigned char * rawkey, unsigned int key_len, cx_ecfp_public_key_t * key )
{
  CX_THROW(cx_ecfp_init_public_key_no_throw(curve, rawkey, key_len, key));
  return key_len;
}

/**
 * @brief   Initialize a private key.
 * 
 * @details The key can be stored in non-volatile memory and
 *          used for ECDSA or ECDH processing.
 * 
 * @param[in]  curve   Curve identifier.
 * 
 * @param[in]  rawkey  Pointer to a raw key value or NULL pointer.
 *                     The value shall be in big endian order.
 * 
 * @param[in]  key_len Length of the key.
 * 
 * @param[out] pvkey   Private key to initialize.
 * 
 * @return             Error code:
 *                     - CX_OK on success
 *                     - CX_EC_INVALID_CURVE
 *                     - CX_INVALID_PARAMETER
 */
cx_err_t cx_ecfp_init_private_key_no_throw(cx_curve_t             curve,
                                  const uint8_t *        rawkey,
                                  size_t                 key_len,
                                  cx_ecfp_private_key_t *pvkey);

/**
 * @brief   Initialize a private key.
 * 
 * @details The key can be stored in non-volatile memory and
 *          used for ECDSA or ECDH processing.
 *          This function throws an exception if the computation
 *          doesn't succeed.
 * 
 * @param[in]  curve   Curve identifier.
 * 
 * @param[in]  rawkey  Pointer to a raw key value or NULL pointer.
 *                     The value shall be in big endian order.
 * 
 * @param[in]  key_len Length of the key.
 * 
 * @param[out] pvkey   Private key to initialize.
 * 
 * @return             Length of the key.
 * 
 * @throws             CX_EC_INVALID_CURVE
 * @throws             CX_INVALID_PARAMETER
 */
static inline int cx_ecfp_init_private_key ( cx_curve_t curve, const unsigned char * rawkey, unsigned int key_len, cx_ecfp_private_key_t * pvkey )
{
  CX_THROW(cx_ecfp_init_private_key_no_throw(curve, rawkey, key_len, pvkey));
  return key_len;
}

/**
 * @brief   Generate a key pair with SHA-512 hash function.
 * 
 * @param[in]  curve       Curve identifier.
 * 
 * @param[out] pubkey      Generated public key.
 * 
 * @param[out] privkey     Generated private key.
 * 
 * @param[in]  keepprivate If set, the private key is the one initialized
 *                         with #cx_ecfp_init_private_key_no_throw. Otherwise,
 *                         a new private key is generated.
 * 
 * @return                 Error code:
 *                         - CX_OK on success
 *                         - CX_EC_INVALID_CURVE
 *                         - CX_NOT_UNLOCKED
 *                         - CX_INVALID_PARAMETER_SIZE
 *                         - CX_MEMORY_FULL
 *                         - CX_NOT_LOCKED
 *                         - CX_INVALID_PARAMETER
 *                         - CX_INTERNAL_ERROR
 *                         - CX_EC_INVALID_POINT
 *                         - CX_EC_INFINITE_POINT
 */
cx_err_t cx_ecfp_generate_pair_no_throw(cx_curve_t             curve,
                               cx_ecfp_public_key_t * pubkey,
                               cx_ecfp_private_key_t *privkey,
                               bool                   keepprivate);

/**
 * @brief   Generate a key pair with SHA-512 hash function.
 * 
 * @details This function throws an exception if the computation
 *          doesn't succeed.
 * 
 * @param[in]  curve       Curve identifier.
 * 
 * @param[out] pubkey      Generated public key.
 * 
 * @param[out] privkey     Generated private key.
 * 
 * @param[in]  keepprivate If set, the private key is the one initialized
 *                         with #cx_ecfp_init_private_key_no_throw. Otherwise,
 *                         a new private key is generated.
 * 
 * @return                 0
 *
 * @throws                 CX_EC_INVALID_CURVE
 * @throws                 CX_NOT_UNLOCKED
 * @throws                 CX_INVALID_PARAMETER_SIZE
 * @throws                 CX_MEMORY_FULL
 * @throws                 CX_NOT_LOCKED
 * @throws                 CX_INVALID_PARAMETER
 * @throws                 CX_INTERNAL_ERROR
 * @throws                 CX_EC_INVALID_POINT
 * @throws                 CX_EC_INFINITE_POINT
 */
static inline int cx_ecfp_generate_pair ( cx_curve_t curve, cx_ecfp_public_key_t * pubkey, cx_ecfp_private_key_t * privkey, int keepprivate )
{
  CX_THROW(cx_ecfp_generate_pair_no_throw(curve, pubkey, privkey, keepprivate));
  return 0;
}

/**
 * @brief   Generate a key pair.
 * 
 * @param[in]  curve       Curve identifier.
 * 
 * @param[out] pubkey      Generated public key.
 * 
 * @param[out] privkey     Generated private key.
 * 
 * @param[in]  keepprivate If set, the private key is the one initialized with
 *                         #cx_ecfp_init_private_key_no_throw.
 *                         Otherwise, a new private key is generated.
 * 
 * @param[in]  hashID      Message digest algorithm identifer.
 * 
 * @return                 Error code:
 *                         - CX_OK on success
 *                         - CX_EC_INVALID_CURVE
 *                         - CX_NOT_UNLOCKED
 *                         - CX_INVALID_PARAMETER_SIZE
 *                         - CX_MEMORY_FULL
 *                         - CX_NOT_LOCKED
 *                         - CX_INVALID_PARAMETER
 *                         - CX_INTERNAL_ERROR
 *                         - CX_EC_INVALID_POINT
 *                         - CX_EC_INFINITE_POINT
 */
cx_err_t cx_ecfp_generate_pair2_no_throw(cx_curve_t             curve,
                                cx_ecfp_public_key_t * pubkey,
                                cx_ecfp_private_key_t *privkey,
                                bool                   keepprivate,
                                cx_md_t                hashID);

/**
 * @brief   Generate a key pair.
 * 
 * @details This function throws an exception if
 *          the computation doesn't succeed.
 * 
 * @param[in]  curve       Curve identifier.
 * 
 * @param[out] pubkey      Generated public key.
 * 
 * @param[out] privkey     Generated private key.
 * 
 * @param[in]  keepprivate If set, the private key is the one initialized with
 *                         #cx_ecfp_init_private_key_no_throw.
 *                         Otherwise, a new private key is generated.
 * 
 * @param[in]  hashID      Message digest algorithm identifer.
 * 
 * @return                 0
 *
 * @throws                 CX_EC_INVALID_CURVE
 * @throws                 CX_NOT_UNLOCKED
 * @throws                 CX_INVALID_PARAMETER_SIZE
 * @throws                 CX_MEMORY_FULL
 * @throws                 CX_NOT_LOCKED
 * @throws                 CX_INVALID_PARAMETER
 * @throws                 CX_INTERNAL_ERROR
 * @throws                 CX_EC_INVALID_POINT
 * @throws                 CX_EC_INFINITE_POINT
 */
static inline int cx_ecfp_generate_pair2 ( cx_curve_t curve, cx_ecfp_public_key_t * pubkey, cx_ecfp_private_key_t * privkey, int keepprivate, cx_md_t hashID )
{
  CX_THROW(cx_ecfp_generate_pair2_no_throw(curve, pubkey, privkey, keepprivate, hashID));
  return 0;
}

#ifdef HAVE_ECC_TWISTED_EDWARDS

/**
 * @brief   Retrieve an EDDSA public key.
 * 
 * @details Retrieve (a,h) = (Kr, Kl), such that (Kr, Kl) = Hash(pv_key)
 *          as specified in <a href="https://tools.ietf.org/html/rfc8032"> RFC8032 </a>.
 *
 * @param[in]  pvkey  A private key fully initialized with #cx_ecfp_init_private_key_no_throw.
 *
 * @param[in]  hashID Message digest algorithm identifier used to compute the input data. 
 *                    SHA512, SHA3 and Keccak are supported.
 *
 * @param[out] pukey  Key container for retrieving the public key A.
 *
 * @param[out] a      Private scalar such that A = a.B.
 * 
 * @param[in]  a_len  Length of the scalar a.
 *
 * @param[out] h      Signature prefix.
 * 
 * @param[in]  h_len  Length of the prefix h.
 *
 * @return            Error code:
 *                    - CX_OK on success
 *                    - CX_EC_INVALID_CURVE
 *                    - CX_NOT_UNLOCKED
 *                    - CX_INVALID_PARAMETER_SIZE
 *                    - CX_INVALID_PARAMETER
 *                    - CX_NOT_LOCKED
 *                    - CX_MEMORY_FULL
 *                    - CX_EC_INVALID_POINT
 *                    - CX_EC_INFINITE_POINT
 *                    - CX_INTERNAL_ERROR
 *
 */
cx_err_t cx_eddsa_get_public_key_no_throw(const cx_ecfp_private_key_t *pvkey,
                                 cx_md_t                      hashID,
                                 cx_ecfp_public_key_t *       pukey,
                                 uint8_t *                    a,
                                 size_t                       a_len,
                                 uint8_t *                    h,
                                 size_t                       h_len);

/**
 * @brief   Retrieve an EDDSA public key.
 * 
 * @details Retrieve (a,h) = (Kr, Kl), such that (Kr, Kl) = Hash(pv_key)
 *          as specified in <a href="https://tools.ietf.org/html/rfc8032"> RFC8032 </a>.
 *          This function throws an exception if the computation doesn't succeed.
 *
 * @param[in]  pvkey  A private key fully initialized with #cx_ecfp_init_private_key_no_throw.
 *
 * @param[in]  hashID Message digest algorithm identifier used to compute the input data. 
 *                    SHA512, SHA3 and Keccak are supported.
 *
 * @param[out] pukey  Key container for retrieving the public key A.
 *
 * @param[out] a      Private scalar such that A = a.B.
 * 
 * @param[in]  a_len  Length of the scalar a.
 *
 * @param[out] h      Signature prefix.
 * 
 * @param[in]  h_len  Length of the prefix h.
 * 
 * @throws            CX_EC_INVALID_CURVE
 * @throws            CX_NOT_UNLOCKED
 * @throws            CX_INVALID_PARAMETER_SIZE
 * @throws            CX_INVALID_PARAMETER
 * @throws            CX_NOT_LOCKED
 * @throws            CX_MEMORY_FULL
 * @throws            CX_EC_INVALID_POINT
 * @throws            CX_EC_INFINITE_POINT
 * @throws            CX_INTERNAL_ERROR
 *
 */
static inline void cx_eddsa_get_public_key ( const cx_ecfp_private_key_t * pvkey, cx_md_t hashID, cx_ecfp_public_key_t * pukey, unsigned char * a, unsigned int a_len, unsigned char * h, unsigned int h_len )
{
  CX_THROW(cx_eddsa_get_public_key_no_throw(pvkey, hashID, pukey, a, a_len, h, h_len));
}

/**
 * @brief   Compress a point according to <a href="https://tools.ietf.org/html/rfc8032"> RFC8032 </a>.
 *
 * @param[in]      curve Curve identifier. The curve must be
 *                       a Twisted Edwards curve.
 * 
 * @param[in, out] p     Pointer to the point to compress.
 * 
 * @param[in]      p_len Length of the point buffer.
 * 
 * @return               Error code:
 *                       - CX_OK on success
 *                       - CX_EC_INVALID_CURVE
 *                       - CX_NOT_UNLOCKED
 *                       - CX_INVALID_PARAMETER_SIZE
 *                       - CX_EC_INVALID_CURVE
 *                       - CX_MEMORY_FULL
 *                       - CX_NOT_LOCKED
 *                       - CX_INVALID_PARAMETER
 *                       - CX_EC_INFINITE_POINT
 */
cx_err_t cx_edwards_compress_point_no_throw(cx_curve_t curve, uint8_t *p, size_t p_len);

/**
 * @brief   Compress a point according to <a href="https://tools.ietf.org/html/rfc8032"> RFC8032 </a>.
 * 
 * @details This function throws an exception if the
 *          computation doesn't succeed.
 *
 * @param[in]      curve Curve identifier. The curve must be
 *                       a Twisted Edwards curve.
 * 
 * @param[in, out] p     Pointer to the point to compress.
 * 
 * @param[in]      p_len Length of the point buffer.
 * 
 * @throws               CX_EC_INVALID_CURVE
 * @throws               CX_NOT_UNLOCKED
 * @throws               CX_INVALID_PARAMETER_SIZE
 * @throws               CX_EC_INVALID_CURVE
 * @throws               CX_MEMORY_FULL
 * @throws               CX_NOT_LOCKED
 * @throws               CX_INVALID_PARAMETER
 * @throws               CX_EC_INFINITE_POINT
 */
static inline void cx_edwards_compress_point(cx_curve_t curve, uint8_t *p, size_t p_len)
{
  CX_THROW(cx_edwards_compress_point_no_throw(curve, p, p_len));
}

/**
 * @brief   Decompress a point according to <a href="https://tools.ietf.org/html/rfc8032"> RFC8032 </a>.
 *
 * @param[in]      curve Curve identifier. The curve must be
 *                       a Twisted Edwards curve.
 * 
 * @param[in, out] p     Pointer to the point to decompress.
 * 
 * @param[in]      p_len Length of the point buffer.
 * 
 * @return               Error code:
 *                       - CX_OK on success
 *                       - CX_EC_INVALID_CURVE
 *                       - CX_NOT_UNLOCKED
 *                       - CX_INVALID_PARAMETER_SIZE
 *                       - CX_EC_INVALID_CURVE
 *                       - CX_MEMORY_FULL
 *                       - CX_NOT_LOCKED
 *                       - CX_INVALID_PARAMETER
 *                       - CX_EC_INFINITE_POINT
 *                       - CX_NO_RESIDUE
 *                       - INVALID_PARAMETER
 */
cx_err_t cx_edwards_decompress_point_no_throw(cx_curve_t curve, uint8_t *p, size_t p_len);

/**
 * @brief   Decompress a point according to <a href="https://tools.ietf.org/html/rfc8032"> RFC8032 </a>.
 * 
 * @details This function throws an exception if the computation
 *          doesn't succeed.
 *
 * @param[in]      curve Curve identifier. The curve must be
 *                       a Twisted Edwards curve.
 * 
 * @param[in, out] p     Pointer to the point to decompress.
 * 
 * @param[in]      p_len Length of the point buffer.
 * 
 * @throws               CX_EC_INVALID_CURVE
 * @throws               CX_NOT_UNLOCKED
 * @throws               CX_INVALID_PARAMETER_SIZE
 * @throws               CX_EC_INVALID_CURVE
 * @throws               CX_MEMORY_FULL
 * @throws               CX_NOT_LOCKED
 * @throws               CX_INVALID_PARAMETER
 * @throws               CX_EC_INFINITE_POINT
 * @throws               CX_NO_RESIDUE
 * @throws               INVALID_PARAMETER
 */
static inline void cx_edwards_decompress_point(cx_curve_t curve, uint8_t *p, size_t p_len)
{
  CX_THROW(cx_edwards_decompress_point_no_throw(curve, p, p_len));
}

/**
 * @deprecated
 * See #cx_edwards_compress_point_no_throw
 */
__attribute__((deprecated)) static inline void cx_edward_compress_point(cx_curve_t curve, uint8_t *p, size_t p_len) {
  CX_THROW(cx_edwards_compress_point_no_throw(curve, p, p_len));
}

/**
 * @deprecated
 * See #cx_edwards_decompress_point_no_throw
 */
__attribute__((deprecated)) static inline void cx_edward_decompress_point(cx_curve_t curve, uint8_t *p, size_t p_len) {
  CX_THROW(cx_edwards_decompress_point_no_throw(curve, p, p_len));
}

#endif // HAVE_ECC_TWISTED_EDWARDS

#endif

#endif // HAVE_ECC
