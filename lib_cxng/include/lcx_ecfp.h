
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

#ifdef HAVE_ECC
#ifndef LCX_ECFP_H
#define LCX_ECFP_H

#include "lcx_wrappers.h"
#include "lcx_hash.h"
#include "ox_ec.h"
#include <stdbool.h>

/** Elliptic Curve public key */
struct cx_ecfp_public_key_s {
  /** curve ID #cx_curve_e */
  cx_curve_t curve;
  /** Public key length in bytes */
  size_t W_len;
  /** Public key value starting at offset 0 */
  uint8_t W[1];
};
/** Elliptic Curve private key */
struct cx_ecfp_private_key_s {
  /** curve ID #cx_curve_e */
  cx_curve_t curve; 
  /** Private key length in bytes */
  size_t d_len; 
  /** Private key value starting at offset 0 */
  uint8_t d[1]; 
};

/** Up to 256 bits Elliptic Curve public key */
struct cx_ecfp_256_public_key_s {
  /** curve ID #cx_curve_e */
  cx_curve_t curve;
  /** Public key length in bytes */
  size_t W_len;
  /** Public key value starting at offset 0 */
  uint8_t W[65];
};
/** Up to 256 bits Elliptic Curve private key */
struct cx_ecfp_256_private_key_s {
  /** curve ID #cx_curve_e */
  cx_curve_t curve;
  /** Private key length in bytes */
  size_t d_len;
  /** Private key value starting at offset 0 */
  uint8_t d[32];
};
/** Up to 256 bits Elliptic Curve extended private key */
struct cx_ecfp_256_extended_private_key_s {
  /** curve ID #cx_curve_e */
  cx_curve_t curve;
  /** Public key length in bytes */
  size_t d_len;
  /** Public key value starting at offset 0 */
  uint8_t d[64];
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

/** Up to 384 bits Public Elliptic Curve key */
struct cx_ecfp_384_public_key_s {
  /** curve ID #cx_curve_e */
  cx_curve_t curve;
  /** Public key length in bytes */
  size_t W_len;
  /** Public key value starting at offset 0 */
  uint8_t W[97];
};
/** Up to 384 bits Private Elliptic Curve key */
struct cx_ecfp_384_private_key_s {
  /** curve ID #cx_curve_e */
  cx_curve_t curve;
  /** Private key length in bytes */
  size_t d_len;
  /** Private key value starting at offset 0 */
  uint8_t d[48];
};
/** Convenience type. See #cx_ecfp_384_public_key_s. */
typedef struct cx_ecfp_384_private_key_s cx_ecfp_384_private_key_t;
/** Convenience type. See #cx_ecfp_384_private_key_s. */
typedef struct cx_ecfp_384_public_key_s cx_ecfp_384_public_key_t;

/** Up to 512 bits Public Elliptic Curve key */
struct cx_ecfp_512_public_key_s {
  /** curve ID #cx_curve_e */
  cx_curve_t curve;
  /** Public key length in bytes */
  size_t W_len;
  /** Public key value starting at offset 0 */
  uint8_t W[129];
};
/** Up to 512 bits Private Elliptic Curve key */
struct cx_ecfp_512_private_key_s {
  /** curve ID #cx_curve_e */
  cx_curve_t curve;
  /** Private key length in bytes */
  size_t d_len;
  /** Private key value starting at offset 0 */
  uint8_t d[64];
};
/** Up to 512 bits Extended Private Elliptic Curve key */
struct cx_ecfp_512_extented_private_key_s {
  /** curve ID #cx_curve_e */
  cx_curve_t curve;
  /** Private key length in bytes */
  size_t d_len;
  /** Private key value starting at offset 0 */
  uint8_t d[128];
};
/** Convenience type. See #cx_ecfp_512_public_key_s. */
typedef struct cx_ecfp_512_public_key_s cx_ecfp_512_public_key_t;
/** Convenience type. See #cx_ecfp_512_private_key_s. */
typedef struct cx_ecfp_512_private_key_s cx_ecfp_512_private_key_t;
/** Convenience type. See #cx_ecfp_512_extented_private_key_s. */
typedef struct cx_ecfp_512_extented_private_key_s cx_ecfp_512_extented_private_key_t;

/** Up to 640 bits Public Elliptic Curve key */
struct cx_ecfp_640_public_key_s {
  /** curve ID #cx_curve_e */
  cx_curve_t curve;
  /** Public key length in bytes */
  size_t W_len;
  /** Public key value starting at offset 0 */
  uint8_t W[161];
};
/** Up to 640 bits Private Elliptic Curve key */
struct cx_ecfp_640_private_key_s {
  /** curve ID #cx_curve_e */
  cx_curve_t curve;
  /** Private key length in bytes */
  size_t d_len;
  /** Private key value starting at offset 0 */
  uint8_t d[80];
};
/** Convenience type. See #cx_ecfp_640_public_key_s. */
typedef struct cx_ecfp_640_public_key_s cx_ecfp_640_public_key_t;
/** Convenience type. See #cx_ecfp_640_private_key_s. */
typedef struct cx_ecfp_640_private_key_s cx_ecfp_640_private_key_t;

/**
 * Add two affine points
 *
 * This routine only support Weierstrass and twisted Edwards curves.
 *
 * @param [in] domain
 *   The curve domain parameters to work with.
 *
 * @param [out] R
 *   P+Q encoded as: 04 x y, where x and y are
 *   encoded as  big endian raw value and have bits length equals to
 *   the curve size.
 *
 * @param [in] P
 *   First point to add *
 *   The value shall be a point encoded as: 04 x y, where x and y are
 *   encoded as  big endian raw value and have bits length equals to
 *   the curve size.
 *
 * @param [in] Q
 *   Second point to add
 *
 * @param [in]  public_point
 *   The point to test  encoded as: 04 x y
 *
 * @return
 *   R encoding length, if add success
 *   0 if result is infinity
 *
 * @throws INVALID_PARAMETER
 */
cx_err_t cx_ecfp_add_point_no_throw(cx_curve_t curve, uint8_t *R, const uint8_t *P, const uint8_t *Q);

static inline int cx_ecfp_add_point ( cx_curve_t curve, unsigned char * R, const unsigned char * P, const unsigned char * Q, unsigned int X_len )
{
  UNUSED(X_len);

  CX_THROW(cx_ecfp_add_point_no_throw(curve, R, P, Q));

  size_t size;
  CX_THROW(cx_ecdomain_parameters_length(curve, &size));

  return 1 + 2 * size;
}

/**
 * Multiply an affine point
 *
 * @param [in] domain
 *   The curve domain parameters to work with.
 *
 * @param [out] R
 *   R = k.P encoded as: 04 x y, where x and y are
 *   encoded as  big endian raw value and have bits length equals to
 *   the curve size.
 *
 * @param [in] P
 *   Point to multiply *
 *   The value shall be a point encoded as: 04 x y, where x and y are
 *   encoded as  big endian raw value and have bits length equals to
 *   the curve size.
 *
 * @param [in] k
 *   scalar to multiply, encoded as big endian integer
 *
 * @param [in] k_len
 *   byte length of scalar to multiply
 *
 * @return
 *   R encoding length, if mult success
 *   0 if result is infinity
 *
 * @throws INVALID_PARAMETER
 */
cx_err_t cx_ecfp_scalar_mult_no_throw(cx_curve_t curve, uint8_t *P, const uint8_t *k, size_t k_len);

static inline int cx_ecfp_scalar_mult ( cx_curve_t curve, unsigned char * P, unsigned int P_len, const unsigned char * k, unsigned int k_len )
{
  UNUSED(P_len);

  CX_THROW(cx_ecfp_scalar_mult_no_throw(curve, P, k, k_len));

  size_t size;
  CX_THROW(cx_ecdomain_parameters_length(curve, &size));

  return 1 + 2 * size;
}

/**
 * Initialize a public ECFP Key.
 * Once initialized, the key may be stored in non-volatile memory
 * an reused 'as-is' for any ECDSA/25519 processing
 * Passing NULL as raw key initializes the key without value. The key may be used
 * as parameter for cx_ecfp_generate_pair.

 * @param [in] curve
 *   The curve domain parameters to work with.
 *
 * @param [in] rawkey
 *   Raw key value or NULL.
 *   The value shall be the public point encoded as:
 *     - '04 x y' for Weiertrass curve
 *     - '04 x y'  or '02 y' (plus sign) for twisted Edwards curves
 *     - '04 x y'  or '02 x' for Montgomery curves
 *    where x and y are encoded as big endian raw value and have bits length
 *    equals to the curve size. Any specific integer decoding from binary,
 *    such as specified in RFC7748 and RFC eddsa-draft, is up to caller.
 *
 * @param [in] key_len
 *   Key bytes lenght
 *
 * @param [out] key
 *   Public ecfp key to init.
 *
 * @param key
 *   Ready to use key to init
 *
 * @return something
 *
 * @throws INVALID_PARAMETER
 */

cx_err_t cx_ecfp_init_public_key_no_throw(cx_curve_t            curve,
                                 const uint8_t *       rawkey,
                                 size_t                key_len,
                                 cx_ecfp_public_key_t *key);

static inline int cx_ecfp_init_public_key ( cx_curve_t curve, const unsigned char * rawkey, unsigned int key_len, cx_ecfp_public_key_t * key )
{
  CX_THROW(cx_ecfp_init_public_key_no_throw(curve, rawkey, key_len, key));
  return key_len;
}

/**
 * Initialize a private ECFP Key.
 * Once initialized, the key may be  stored in non-volatile memory
 * and reused 'as-is' for any ECDSA/EC25519 processing
 * Passing NULL as raw key initializes the key without value. The key may be used
 * as parameter for cx_ecfp_generate_pair.
 *
 * @param [in] curve
 *   The curve domain parameters to work with.
 *
 * @param [in] rawkey
 *   Raw key value or NULL.
 *   The value shall be the private key big endian raw value.
 *
 * @param [in] key_len
 *   Key bytes lenght
 *
 * @param [out] pvkey
 *   Private ecfp key to init.
 *
 * @param key
 *   Ready to use key to init
 *
 * @return something
 *
 * @throws INVALID_PARAMETER
 */
cx_err_t cx_ecfp_init_private_key_no_throw(cx_curve_t             curve,
                                  const uint8_t *        rawkey,
                                  size_t                 key_len,
                                  cx_ecfp_private_key_t *pvkey);

static inline int cx_ecfp_init_private_key ( cx_curve_t curve, const unsigned char * rawkey, unsigned int key_len, cx_ecfp_private_key_t * pvkey )
{
  CX_THROW(cx_ecfp_init_private_key_no_throw(curve, rawkey, key_len, pvkey));
  return key_len;
}

/**
 * Generate a ecfp key pair.
 * This function call cx_ecfp_generate_pair2 with hashID equals to CX_SHA512.
 *
 * @param [in] curve
 *   The curve domain parameters to work with.
 *
 * @param [out] pubkey
 *   A public ecfp public key to generate.
 *
 * @param [in,out] privkey
 *   A private ecfp private key to generate.
 *   Either:
 *     - if the private ecfp key is fully inited, i.e  parameter 'rawkey' of
 *       'cx_ecfp_init_private_key' is NOT null, the private key value is kept
 *       if the 'keep_private' parameter is non zero
 *     - else a new private key is generated.
 *
 * @param [in] keepprivate if set to non zero, keep the private key value if set.
 *             Else generate a new random one
 *
 * @return zero
 *
 * @throws INVALID_PARAMETER
 */
cx_err_t cx_ecfp_generate_pair_no_throw(cx_curve_t             curve,
                               cx_ecfp_public_key_t * pubkey,
                               cx_ecfp_private_key_t *privkey,
                               bool                   keepprivate);

static inline int cx_ecfp_generate_pair ( cx_curve_t curve, cx_ecfp_public_key_t * pubkey, cx_ecfp_private_key_t * privkey, int keepprivate )
{
  CX_THROW(cx_ecfp_generate_pair_no_throw(curve, pubkey, privkey, keepprivate));
  return 0;
}

/**
 * Generate a ecfp key pair
 *
 * @param [in] curve
 *   The curve domain parameters to work with.
 *
 * @param [out] pubkey
 *   A public ecfp public key to generate.
 *
 * @param [in,out] privkey
 *   A private ecfp private key to generate.
 *   Either:
 *     - if the private ecfp key is fully inited, i.e  parameter 'rawkey' of
 *       'cx_ecfp_init_private_key' is NOT null, the private key value is kept
 *       if the 'keep_private' parameter is non zero
 *     - else a new private key is generated.
 *
 * @param [in] keepprivate if set to non zero, keep the private key value if set.
 *             Else generate a new random one
 *
 *  @param [in] hashID Hash to use for eddsa (SHA512, SHA3 and Keccak are supported)
 *
 *
 * @throws INVALID_PARAMETER
 */
cx_err_t cx_ecfp_generate_pair2_no_throw(cx_curve_t             curve,
                                cx_ecfp_public_key_t * pubkey,
                                cx_ecfp_private_key_t *privkey,
                                bool                   keepprivate,
                                cx_md_t                hashID);

static inline int cx_ecfp_generate_pair2 ( cx_curve_t curve, cx_ecfp_public_key_t * pubkey, cx_ecfp_private_key_t * privkey, int keepprivate, cx_md_t hashID )
{
  CX_THROW(cx_ecfp_generate_pair2_no_throw(curve, pubkey, privkey, keepprivate, hashID));
  return 0;
}

#ifdef HAVE_ECC_TWISTED_EDWARDS

/**
 *  Retrieve (a,h) = (Kr, Kl), such (Kr, Kl) = Hash(pv_key) as specified in RFC8032
 *
 * @param [in] pv_key
 *   A private ecfp key fully inited with 'cx_ecfp_init_private_key'.
 *
 * @param [in] hashID
 *  Hash identifier used to compute the input data. SHA512, SHA3 and Keccak are supported.
 *
 * @param [out] pu_key
 *   A public null-inited ecfp key container for retrieving public key A.
 *
 * @param [out] a
 *   private scalar such A = a.B
 *
 * @param [out] h
 *   prefix signature
 *
 * @param [in] hashID
 *  Hash identifier used to compute the input data. SHA512, SHA3 and Keccak are supported.
 *
 */
cx_err_t cx_eddsa_get_public_key_no_throw(const cx_ecfp_private_key_t *pvkey,
                                 cx_md_t                      hashID,
                                 cx_ecfp_public_key_t *       pukey,
                                 uint8_t *                    a,
                                 size_t                       a_len,
                                 uint8_t *                    h,
                                 size_t                       h_len);

static inline void cx_eddsa_get_public_key ( const cx_ecfp_private_key_t * pvkey, cx_md_t hashID, cx_ecfp_public_key_t * pukey, unsigned char * a, unsigned int a_len, unsigned char * h, unsigned int h_len )
{
  CX_THROW(cx_eddsa_get_public_key_no_throw(pvkey, hashID, pukey, a, a_len, h, h_len));
}

/**
 *  Compress point according to RFC8032.
 *
 * @param [in]     domain
 * @param [in,out] P
 */
cx_err_t cx_edwards_compress_point_no_throw(cx_curve_t curve, uint8_t *p, size_t p_len);

static inline void cx_edwards_compress_point(cx_curve_t curve, uint8_t *p, size_t p_len)
{
  CX_THROW(cx_edwards_compress_point_no_throw(curve, p, p_len));
}

/**
 *  Decompress point according to draft-irtf-cfrg-eddsa-05.
 *
 * @param [in]     domain
 * @param [in,out] P
 */
cx_err_t cx_edwards_decompress_point_no_throw(cx_curve_t curve, uint8_t *p, size_t p_len);

static inline void cx_edwards_decompress_point(cx_curve_t curve, uint8_t *p, size_t p_len)
{
  CX_THROW(cx_edwards_decompress_point_no_throw(curve, p, p_len));
}

__attribute__((deprecated)) static inline void cx_edward_compress_point(cx_curve_t curve, uint8_t *p, size_t p_len) {
  CX_THROW(cx_edwards_compress_point_no_throw(curve, p, p_len));
}

__attribute__((deprecated)) static inline void cx_edward_decompress_point(cx_curve_t curve, uint8_t *p, size_t p_len) {
  CX_THROW(cx_edwards_decompress_point_no_throw(curve, p, p_len));
}

#endif // HAVE_ECC_TWISTED_EDWARDS

#endif

#endif // HAVE_ECC
