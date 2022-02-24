
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
 * @file    ox_ec.h
 * @brief   Elliptic curve cryptography syscalls.
 *
 * This file contains elliptic curves definitions and functions.
 */

#ifndef OX_EC_H
#define OX_EC_H

#include "decorators.h"
#include "ox_bn.h"
#include <stddef.h>
#include <stdint.h>

/** Largest domain parameters length. */
#define CX_MAX_DOMAIN_LENGTH 66

#ifdef HAVE_SECP_CURVES
/** Enable the Koblitz curve Secp256k1. */
#define HAVE_SECP256K1_CURVE

/** Enable the verifiably random curve Secp256r1. */
#define HAVE_SECP256R1_CURVE

/** Enable the verifiably random curve Secp384r1. */
#define HAVE_SECP384R1_CURVE

/** Enable the verifiably random curve Secp521r1. */
#define HAVE_SECP521R1_CURVE
#endif

#ifdef HAVE_BRAINPOOL_CURVES

/** Enable the curve BrainpoolP256r1. */
#define HAVE_BRAINPOOL_P256R1_CURVE

/** Enable the twisted curve BrainpoolP256t1. */
#define HAVE_BRAINPOOL_P256T1_CURVE

/** Enable the curve BrainpoolP320r1. */
#define HAVE_BRAINPOOL_P320R1_CURVE

/** Enable the twisted curve BrainpoolP320t1. */
#define HAVE_BRAINPOOL_P320T1_CURVE

/** Enable the curve BrainpoolP384r1. */
#define HAVE_BRAINPOOL_P384R1_CURVE

/** Enable the twisted curve BrainpoolP384t1. */
#define HAVE_BRAINPOOL_P384T1_CURVE

/** Enable the curve BrainpoolP512r1. */
#define HAVE_BRAINPOOL_P512R1_CURVE

/** Enable the twisted curve BrainpoolP512t1. */
#define HAVE_BRAINPOOL_P512T1_CURVE
#endif

#ifdef HAVE_ED_CURVES
/** Enable the twisted Edwards curve Ed25519. */
#define HAVE_ED25519_CURVE

/** Enable the twisted Edwards curve Ed448. */
#define HAVE_ED448_CURVE
#endif

#ifdef HAVE_CV_CURVES
/** Enable the Montgomery curve Curve25519. */
#define HAVE_CV25519_CURVE

/** Enable the Montgomery curve Curve448. */
#define HAVE_CV448_CURVE
#endif

#ifdef HAVE_ANSSI_CURVES

/** Enable the curve FRP256v1. */
#define HAVE_FR256V1_CURVE
#endif

#ifdef HAVE_STARK_CURVES

/** Enable the Stark curve. */
#define HAVE_STARK256_CURVE
#endif

/** Indicates the parity of a point coordinate. */
#define CX_ECCINFO_PARITY_ODD 1

#define CX_ECCINFO_xGTn 2

/** List of supported elliptic curves */
enum cx_curve_e {
  /** Undefined curve */
  CX_CURVE_NONE,

#ifdef HAVE_ECC_WEIERSTRASS
  /* ------------------------ */
  /* --- Type Weierstrass --- */
  /* ------------------------ */
  /** Low limit (not included) of Weierstrass curve ID */
  CX_CURVE_WEIERSTRASS_START = 0x20,

#ifdef HAVE_SECP256K1_CURVE
  /** Secp256k1 */
  CX_CURVE_SECP256K1 = 0x21,
/** Allowed identifier for Secp256k1*/
#define CX_CURVE_256K1 CX_CURVE_SECP256K1
#endif

#ifdef HAVE_SECP256R1_CURVE
  /** Secp256r1 */
  CX_CURVE_SECP256R1 = 0x22,
/** Legacy identifier for Secp256r1 */
#define CX_CURVE_256R1 CX_CURVE_SECP256R1
/** Legacy identifier for Secp256r1 */
#define CX_CURVE_NISTP256 CX_CURVE_SECP256R1
#endif

#ifdef HAVE_SECP384R1_CURVE
  /** Secp384r1 */
  CX_CURVE_SECP384R1 = 0x23,
/** Allowed identifier for Secp384r1 */
#define CX_CURVE_NISTP384 CX_CURVE_SECP384R1
#endif

#ifdef HAVE_SECP521R1_CURVE
  /** Secp521r1 */
  CX_CURVE_SECP521R1 = 0x24,
/** Allowed identifier for Secp521r1 */
#define CX_CURVE_NISTP521 CX_CURVE_SECP521R1
#endif

#ifdef HAVE_BRAINPOOL_P256T1_CURVE
  /** BrainpoolP256t1 */
  CX_CURVE_BrainPoolP256T1 = 0x31,
#endif

#ifdef HAVE_BRAINPOOL_P256R1_CURVE
  /** BrainpoolP256r1 */
  CX_CURVE_BrainPoolP256R1 = 0x32,
#endif

#ifdef HAVE_BRAINPOOL_P320T1_CURVE
  /** BrainpoolP320t1 */
  CX_CURVE_BrainPoolP320T1 = 0x33,
#endif

#ifdef HAVE_BRAINPOOL_P320R1_CURVE
  /** BrainpoolP320r1 */
  CX_CURVE_BrainPoolP320R1 = 0x34,
#endif

#ifdef HAVE_BRAINPOOL_P384T1_CURVE
  /** BrainpoolP384t1 */
  CX_CURVE_BrainPoolP384T1 = 0x35,
#endif

#ifdef HAVE_BRAINPOOL_P384R1_CURVE
  /** Brainpool384r1 */
  CX_CURVE_BrainPoolP384R1 = 0x36,
#endif

#ifdef HAVE_BRAINPOOL_P512T1_CURVE
  /** BrainpoolP512t1 */
  CX_CURVE_BrainPoolP512T1 = 0x37,
#endif

#ifdef HAVE_BRAINPOOL_P512R1_CURVE
  /** BrainpoolP512r1 */
  CX_CURVE_BrainPoolP512R1 = 0x38,
#endif

#ifdef HAVE_BLS12_381_G1_CURVE
  /** BLS12-381 G1 */
  CX_CURVE_BLS12_381_G1 = 0x39,
#endif

#ifdef HAVE_FR256V1_CURVE
  /** ANSSI FRP256 */
  CX_CURVE_FRP256V1 = 0x41,
#endif

#ifdef HAVE_STARK256_CURVE
  /** Stark */
  CX_CURVE_Stark256 = 0x51,
#endif

  /** High limit (not included) of Weierstrass curve ID */
  CX_CURVE_WEIERSTRASS_END = 0x6F,

#endif // HAVE_ECC_WEIERSTRASS

/* ---------------------------- */
/* --- Type Twisted Edwards --- */
/* ---------------------------- */
#ifdef HAVE_ECC_TWISTED_EDWARDS
  /** Low limit (not included) of  Twisted Edwards curve ID */
  CX_CURVE_TWISTED_EDWARDS_START = 0x70,

#ifdef HAVE_ED25519_CURVE
  /** Ed25519 */
  CX_CURVE_Ed25519 = 0x71,
#endif

#ifdef HAVE_ED25519_CURVE
  /** Ed448 */
  CX_CURVE_Ed448 = 0x72,
#endif

  /** High limit (not included) of Twisted Edwards curve ID */
  CX_CURVE_TWISTED_EDWARDS_END = 0x7F,

#endif // HAVE_ECC_TWISTED_EDWARDS

/* ----------------------- */
/* --- Type Montgomery --- */
/* ----------------------- */
#ifdef HAVE_ECC_MONTGOMERY
  /** Low limit (not included) of Montgomery curve ID */
  CX_CURVE_MONTGOMERY_START = 0x80,

#ifdef HAVE_CV25519_CURVE
  /** Curve25519 */
  CX_CURVE_Curve25519 = 0x81,
#endif
#ifdef HAVE_CV448_CURVE
  /** Curve448 */
  CX_CURVE_Curve448 = 0x82,
#endif

  /** High limit (not included) of Montgomery curve ID */
  CX_CURVE_MONTGOMERY_END = 0x8F
#endif // HAVE_ECC_MONTGOMERY
};

/** Convenience type. See #cx_curve_e. */
typedef enum cx_curve_e cx_curve_t;

/** Return true if the curve identifier is in the specified range
 * @hideinitializer */
#define CX_CURVE_RANGE(i, dom)                                                 \
  (((i) > (CX_CURVE_##dom##_START)) && ((i) < (CX_CURVE_##dom##_END)))

#ifdef HAVE_ECC_WEIERSTRASS
/** Return true if the curve is a short Weierstrass curve @hideinitializer */
#define CX_CURVE_IS_WEIERSTRASS(c)                                             \
  (((c) > CX_CURVE_WEIERSTRASS_START) && ((c) < CX_CURVE_WEIERSTRASS_END))
#endif

#ifdef HAVE_ECC_TWISTED_EDWARDS
/** Return true if the curve is a twisted Edwards curve @hideinitializer */
#define CX_CURVE_IS_TWISTED_EDWARDS(c)                                         \
  (((c) > CX_CURVE_TWISTED_EDWARDS_START) &&                                   \
   ((c) < CX_CURVE_TWISTED_EDWARDS_END))
#endif

#ifdef HAVE_ECC_MONTGOMERY
/** Return true if the curve is a Montgomery curve @hideinitializer */
#define CX_CURVE_IS_MONTGOMERY(c)                                              \
  (((c) > CX_CURVE_MONTGOMERY_START) && ((c) < CX_CURVE_MONTGOMERY_END))
#endif

/**
 * @brief   Curve domain parameters.
 *
 * @details The parameters are common to #cx_curve_weierstrass_s,
 * #cx_curve_twisted_edwards_s, and #cx_curve_montgomery_s.
 *
 *  @arg @c curve:    Curve identifier. See #cx_curve_e
 *  @arg @c bit_size: Curve size in bits
 *  @arg @c length:   Component lenth in bytes
 *  @arg @c a:        a coefficient of the curve equation
 *  @arg @c b:        b (Weierstrass or Montgomery) or d (twisted Edwards)
 * coefficient of the curve equation
 *  @arg @c p:        Prime specifying the base field
 *  @arg @c Gx:       x-coordinate of the base point
 *  @arg @c Gy:       y-coordinate of the base point
 *  @arg @c n:        Curve order: order of the group generated by G
 *  @arg @c h:        Cofactor i.e. **h = |E(GF(p))|/n**
 *  @arg @c Hn:       Second Montgomery constant for the curve order
 *  @arg @c Hp:       Second Montgomery constant for the field characteristic p
 */
#define CX_CURVE_HEADER                                                        \
  cx_curve_t curve;                                                            \
  unsigned int bit_size;                                                       \
  unsigned int length;                                                         \
  const uint8_t *a;                                                            \
  const uint8_t *b;                                                            \
  const uint8_t *p;                                                            \
  const uint8_t *Gx;                                                           \
  const uint8_t *Gy;                                                           \
  const uint8_t *n;                                                            \
  const uint8_t *h;                                                            \
  const uint8_t *Hn;                                                           \
  const uint8_t *Hp;

/**
 * @brief   Weierstrass curve defined by **y^3 = x^2 + a*x + b over GF(p)**.
 *
 * @details See #CX_CURVE_HEADER for the structure members.
 */
struct cx_curve_weierstrass_s {
  CX_CURVE_HEADER
};

/** Convenience type. See #cx_curve_weierstrass_s. */
typedef struct cx_curve_weierstrass_s cx_curve_weierstrass_t;

/**
 * @brief   Twisted Edwards curve defined by **a*x^2 + y^2 = 1 + d*x2*y2 over
 * GF(q)**.
 *
 * @details See #CX_CURVE_HEADER for the structure members.
 */
struct cx_curve_twisted_edwards_s {
  CX_CURVE_HEADER
};

/** Convenience type. See #cx_curve_twisted_edwards_s. */
typedef struct cx_curve_twisted_edwards_s cx_curve_twisted_edwards_t;

/**
 * @brief   Montgomery curve defined by **B*y^2= x^3 + A*x^2 + x over GF(q)**.
 *
 * @details See #CX_CURVE_HEADER for the structure members.
 */
struct cx_curve_montgomery_s {
  CX_CURVE_HEADER
};

/** Convenience type. See #cx_curve_montgomery_s. */
typedef struct cx_curve_montgomery_s cx_curve_montgomery_t;

/**
 * @brief   Abstract type for elliptic curve domain.
 *
 * @details See #CX_CURVE_HEADER for the structure members.
 */
struct cx_curve_domain_s {
  CX_CURVE_HEADER
};

/** Convenience type. See #cx_curve_domain_s. */
typedef struct cx_curve_domain_s cx_curve_domain_t;

/**
 * @brief Elliptic curve point.
 */
struct cx_ec_point_s {
  cx_curve_t curve; ///< Point's curve
  cx_bn_t x;        ///< x-coordinate in affine representation
  cx_bn_t y;        ///< y-coordinate in affine representation
  cx_bn_t z;        ///< z-coordinate = 1 in affine representation
};

/** Convenience type. See #cx_ec_point_s. */
typedef struct cx_ec_point_s cx_ecpoint_t;

/**
 * @brief Identifiers of the domain parameters.
 */

enum cx_curve_dom_param_s {
  CX_CURVE_PARAM_NONE = 0,     ///< No parameter
  CX_CURVE_PARAM_A = 1,        ///< First coefficient of the curve
  CX_CURVE_PARAM_B = 2,        ///< Second coefficient of the curve
  CX_CURVE_PARAM_Field = 3,    ///< Curve field
  CX_CURVE_PARAM_Gx = 4,       ///< x-coordinate of the curve's generator
  CX_CURVE_PARAM_Gy = 5,       ///< y-coordinate of the curve's generator
  CX_CURVE_PARAM_Order = 6,    ///< Order of the generator
  CX_CURVE_PARAM_Cofactor = 7, ///< Cofactor
};

typedef enum cx_curve_dom_param_s cx_curve_dom_param_t;

/**
 * @brief   Return the bit length of each parameter of the curve.
 *
 * @param[in] curve   Curve identifier.
 *
 * @param[out] length Bit length of each parameter.
 *
 * @return            Error code:
 *                    - CX_OK on success
 *                    - CX_EC_INVALID_CURVE
 */
SYSCALL cx_err_t cx_ecdomain_size(cx_curve_t curve, size_t *length);

/**
 * @brief   Return the byte length of each parameter of the curve.
 *
 * @param[in]  cv     Curve identifier.
 *
 * @param[out] length Byte length of each parameter.
 *
 * @return            Error code:
 *                    - CX_OK on success
 *                    - CX_EC_INVALID_CURVE
 */
SYSCALL cx_err_t cx_ecdomain_parameters_length(cx_curve_t cv, size_t *length);

/**
 * @brief   Get a specific parameter of the curve.
 *
 * @param[in]  cv    Curve identifier.
 *
 * @param[in]  id    Parameter identifier.
 *
 * @param[out] p     Buffer where to store the parameter.
 *
 * @param[in]  p_len Length of the buffer.
 *
 * @return           Error code:
 *                   - CX_OK on success
 *                   - CX_EC_INVALID_CURVE
 *                   - CX_INVALID_PARAMETER
 */
SYSCALL cx_err_t cx_ecdomain_parameter(cx_curve_t cv, cx_curve_dom_param_t id,
                                       uint8_t *p PLENGTH(p_len),
                                       uint32_t p_len);

/**
 * @brief   Store a specific parameter of the curve as a BN.
 *
 * @param[in]  cv Curve identifier.
 *
 * @param[in]  id Parameter identifier.
 *
 * @param[out] p  BN where to store the parameter.
 *
 * @return        Error code:
 *                - CX_OK on success
 *                - CX_EC_INVALID_CURVE
 *                - CX_NOT_LOCKED
 *                - CX_INVALID_PARAMETER
 *                - CX_INVALID_PARAMETER_SIZE
 */
SYSCALL cx_err_t cx_ecdomain_parameter_bn(cx_curve_t cv,
                                          cx_curve_dom_param_t id, cx_bn_t p);

/**
 * @brief   Get the generator of the curve.
 *
 * @param[in] cv   Curve identifier.
 *
 * @param[out] Gx  Buffer to store the x-coordinate of the generator.
 *
 * @param[out] Gy  Buffer to store the y-coordinate of the generator.
 *
 * @param[in]  len Byte length of each coordinate.
 *
 * @return         Error code:
 *                 - CX_OK on success
 *                 - CX_EC_INVALID_CURVE
 *                 - CX_INVALID_PARAMETER
 */
SYSCALL cx_err_t cx_ecdomain_generator(cx_curve_t cv, uint8_t *Gx PLENGTH(len),
                                       uint8_t *Gy PLENGTH(len), size_t len);

/**
 * @brief   Get the generator of the curve and store it in the point structure.
 *
 * @param[in]  cv Curve identifier.
 *
 * @param[out] P  Pointer to the structure where to store the generator.
 *
 * @return        Error code:
 *                - CX_OK on success
 *                - CX_EC_INVALID_CURVE
 *                - CX_NOT_LOCKED
 *                - CX_INVALID_PARAMETER
 *                - CX_INVALID_PARAMETER_SIZE
 *                - CX_EC_INVALID_POINT
 */
SYSCALL cx_err_t cx_ecdomain_generator_bn(
    cx_curve_t cv, cx_ecpoint_t *P PLENGTH(sizeof(cx_ecpoint_t)));

/**
 * @brief   Allocate memory for a point on the curve.
 *
 * @param[in] P  Pointer to a point.
 *
 * @param[in] cv Curve on which the point is defined.
 *
 * @return       Error code:
 *               - CX_OK on success
 *               - CX_EC_INVALID_CURVE
 *               - CX_NOT_LOCKED
 *               - CX_INVALID_PARAMETER
 *               - CX_MEMORY_FULL
 */
SYSCALL cx_err_t cx_ecpoint_alloc(cx_ecpoint_t *P PLENGTH(sizeof(cx_ecpoint_t)),
                                  cx_curve_t cv);

/**
 * @brief   Destroy a point on the curve.
 *
 * @param[in] P Pointer to the point to destroy. If the pointer is NULL,
 *              nothing is done.
 *
 * @return      Error code:
 *              - CX_OK on success
 *              - CX_NOT_LOCKED
 *              - CX_INVALID_PARAMETER
 *              - CX_INTERNAL_ERROR
 */
SYSCALL cx_err_t
cx_ecpoint_destroy(cx_ecpoint_t *P PLENGTH(sizeof(cx_ecpoint_t)));

/**
 * @brief   Initialize a point on the curve.
 *
 * @param[in] P     Pointer to the point to initialize.
 *
 * @param[in] x     x-coordinate of the point. This must belong to the curve
 * field.
 *
 * @param[in] x_len Length of the x-coordinate. This must be at most equal to
 * the curve's domain number of bytes.
 *
 * @param[in] y     y-coordinate of the point. This must belong to the curve
 * field.
 *
 * @param[in] y_len Length of the y-coordinate. This must be at most equal to
 * the curve's domain number of bytes.
 *
 * @return          Error code:
 *                  - CX_OK on success
 *                  - CX_NOT_LOCKED
 *                  - CX_INVALID_PARAMETER
 *                  - CX_EC_INVALID_CURVE
 */
SYSCALL cx_err_t cx_ecpoint_init(cx_ecpoint_t *P PLENGTH(sizeof(cx_ecpoint_t)),
                                 const uint8_t *x PLENGTH(x_len), size_t x_len,
                                 const uint8_t *y PLENGTH(y_len), size_t y_len);

/**
 * @brief   Initialize a point on the curve with the BN indexes of the
 * coordinates.
 *
 * @param[in] P Pointer to the point to initialize.
 *
 * @param[in] x BN index of the x-coordinate. The coordinate must
 *              belong to the base field.
 *
 * @param[in] y BN index of the y-coordinate. The coordinate must
 *              belong to the base field.
 *
 * @return      Error code:
 *              - CX_OK on success
 *              - CX_NOT_LOCKED
 *              - CX_INVALID_PARAMETER
 *              - CX_EC_INVALID_CURVE
 */
SYSCALL cx_err_t
cx_ecpoint_init_bn(cx_ecpoint_t *P PLENGTH(sizeof(cx_ecpoint_t)),
                   const cx_bn_t x, const cx_bn_t y);

/**
 * @brief   Export a point.
 *
 * @details Fill two distinct buffers with the x-coordinate and the y-coordinate
 *          of the point. If the point is not in affine representation, it will
 *          be normalized first.
 *
 * @param[in]  P     Pointer to the point to export.
 *
 * @param[out] x     Buffer for the x-coordinate.
 *
 * @param[in]  x_len Length of the *x* buffer.
 *
 * @param[out] y     Buffer for the y-coordinate.
 *
 * @param[in]  y_len Length of the *y* buffer.
 *
 * @return           Error code:
 *                   - CX_OK on success
 *                   - CX_NOT_LOCKED
 *                   - CX_INVALID_PARAMETER
 *                   - CX_EC_INVALID_CURVE
 *                   - CX_EC_INFINITE_POINT
 *                   - CX_MEMORY_FULL
 */
SYSCALL cx_err_t
cx_ecpoint_export(const cx_ecpoint_t *P PLENGTH(sizeof(cx_ecpoint_t)),
                  uint8_t *x PLENGTH(x_len), size_t x_len,
                  uint8_t *y PLENGTH(y_len), size_t y_len);

/**
 * @brief   Export a point using BN indexes of the coordinates.
 *
 * @param[in]  P Pointer to the point to export.
 *
 * @param[out] x Pointer to the BN index of the x-coordinate.
 *
 * @param[out] y Pointer to the BN index of the y-coordinate.
 *
 * @return       Error code:
 *               - CX_OK on success
 *               - CX_NOT_LOCKED
 *               - CX_INVALID_PARAMETER
 *               - CX_EC_INVALID_CURVE
 *               - CX_EC_INFINITE_POINT
 *               - CX_MEMORY_FULL
 */
SYSCALL cx_err_t cx_ecpoint_export_bn(
    const cx_ecpoint_t *P PLENGTH(sizeof(cx_ecpoint_t)),
    cx_bn_t *x PLENGTH(sizeof(cx_bn_t)), cx_bn_t *y PLENGTH(sizeof(cx_bn_t)));

/**
 * @brief   Compute the compressed form of a point.
 *
 * @details The compressed form depends on the curve type.
 *          For a Weierstrass or a Montgomery curve, the
 *          compressed form consists of the x-coordinate and
 *          a prefix. For a Twisted Edwards curve the compressed
 *          form consists of a y-coordinate and a prefix.
 *
 * @param[in]  P Pointer to the point to be compressed.
 *
 * @param[out] xy_compressed     Buffer to hold the compressed
 *                               coordinate.
 *
 * @param[in]  xy_compressed_len Length of the compressed coordinate in bytes.
 *                               This should be equal to twice of the length of
 * one coordinate plus one byte for the prefix.
 *
 * @param[out] sign              Pointer to the sign of the hidden coordinate:
 *                               correspond to the least significant bit of the
 *                               y-coordinate for a Weierstrass or Montgomery
 * curve and of the x-coordinate for a Twisted Edwards curve.
 *
 * @return                       Error code:
 *                               - CX_OK on success
 *                               - CX_NOT_LOCKED
 *                               - CX_INVALID_PARAMETER
 *                               - CX_EC_INVALID_CURVE
 *                               - CX_EC_INFINITE_POINT
 *                               - CX_MEMORY_FULL
 */
SYSCALL cx_err_t
cx_ecpoint_compress(const cx_ecpoint_t *P PLENGTH(sizeof(cx_ecpoint_t)),
                    uint8_t *xy_compressed PLENGTH(xy_compressed_len),
                    size_t xy_compressed_len, uint32_t *sign);

/**
 * @brief   Compute the affine coordinates of a point given its compressed form.
 *
 * @param[out] P Pointer to the point.
 *
 * @param[in]  xy_compressed     Pointer to the buffer holding the compressed
 *                               coordinate.
 *
 * @param[in]  xy_compressed_len Length of the compressed coordinate in bytes.
 *                               This should be equal to twice of the length of
 * one coordinate plus one byte for the prefix.
 *
 * @param[in]  sign              Sign of the coordinate to recover.
 *
 * @return                       Error code:
 *                               - CX_OK on success
 *                               - CX_NOT_LOCKED
 *                               - CX_INVALID_PARAMETER
 *                               - CX_EC_INVALID_CURVE
 *                               - CX_MEMORY_FULL
 *                               - CX_NO_RESIDUE
 *
 */
SYSCALL cx_err_t
cx_ecpoint_decompress(cx_ecpoint_t *P PLENGTH(sizeof(cx_ecpoint_t)),
                      const uint8_t *xy_compressed PLENGTH(xy_compressed_len),
                      size_t xy_compressed_len, uint32_t sign);

/**
 * @brief   Add two points on a curve.
 *
 * @details Each point should not be the point at infinity.
 *          If one of the point is the point at infinity then
 *          the function returns a CX_EC_INFINITE_POINT error.
 *
 * @param[out] R Pointer to the result point.
 *
 * @param[in]  P Pointer to the first point to add.
 *               The point must be on the curve.
 *
 * @param[in]  Q Pointer to the second point to add.
 *               The point must be on the curve.
 *
 * @return       Error code:
 *               - CX_OK on success
 *               - CX_NOT_LOCKED
 *               - CX_INVALID_PARAMETER
 *               - CX_EC_INVALID_CURVE
 *               - CX_EC_INVALID_POINT
 *               - CX_EC_INFINITE_POINT
 *               - CX_MEMORY_FULL
 */
SYSCALL cx_err_t
cx_ecpoint_add(cx_ecpoint_t *R PLENGTH(sizeof(cx_ecpoint_t)),
               const cx_ecpoint_t *P PLENGTH(sizeof(cx_ecpoint_t)),
               const cx_ecpoint_t *Q PLENGTH(sizeof(cx_ecpoint_t)));

/**
 * @brief   Compute the opposite of a point.
 *
 * @details The point should not be the point at infinity,
 *          otherwise the function returns a CX_EC_INFINITE_POINT
 *          error.
 *
 * @param[in, out] P Pointer to a point of the curve.
 *                   and will hold the result.
 *
 * @return           Error code:
 *                   - CX_OK on success
 *                   - CX_NOT_LOCKED
 *                   - CX_INVALID_PARAMETER
 *                   - CX_EC_INVALID_CURVE
 *                   - CX_EC_INVALID_POINT
 *                   - CX_MEMORY_FULL
 *                   - CX_EC_INFINITE_POINT
 */
SYSCALL cx_err_t cx_ecpoint_neg(cx_ecpoint_t *P PLENGTH(sizeof(cx_ecpoint_t)));

/**
 * @brief   Perform a secure scalar multiplication.
 *
 * @param[in, out] P     Pointer to a point on a curve. This will hold the
 * result.
 *
 * @param[in]      k     Pointer to the scalar. The scalar is an integer at
 * least equal to 0 and at most equal to the order of the curve minus 1.
 *
 * @param[in]      k_len Length of the scalar. This should be equal to the
 * domain length.
 *
 * @return               Error code:
 *                       - CX_OK on success
 *                       - CX_NOT_LOCKED
 *                       - CX_INVALID_PARAMETER
 *                       - CX_EC_INVALID_POINT
 *                       - CX_EC_INVALID_CURVE
 *                       - CX_EC_INFINITE_POINT
 *                       - CX_MEMORY_FULL
 */
SYSCALL cx_err_t
cx_ecpoint_rnd_scalarmul(cx_ecpoint_t *P PLENGTH(sizeof(cx_ecpoint_t)),
                         const uint8_t *k PLENGTH(k_len), size_t k_len);

/**
 * @brief   Perform a secure scalar multiplication given the BN index of the
 * scalar.
 *
 * @param[in, out] P    Pointer to a point on a curve. This will hold the
 * result.
 *
 * @param[in]      bn_k BN index of the scalar. The scalar is an integer at
 * least equal to 0 and at most equal to the order of the curve minus 1.
 *
 * @return           Error code:
 *                   - CX_OK on success
 *                   - CX_NOT_LOCKED
 *                   - CX_INVALID_PARAMETER
 *                   - CX_EC_INVALID_POINT
 *                   - CX_EC_INVALID_CURVE
 *                   - CX_EC_INFINITE_POINT
 *                   - CX_MEMORY_FULL
 */
SYSCALL cx_err_t
cx_ecpoint_rnd_scalarmul_bn(cx_ecpoint_t *P PLENGTH(sizeof(cx_ecpoint_t)),
                            const cx_bn_t bn_k PLENGTH(sizeof(cx_bn_t)));

/**
 * @brief   Perform a secure scalar multiplication with a fixed scalar length.
 *
 * @param[in, out] P     Pointer to a point on a curve. This will hold the
 * result.
 *
 * @param[in]      k     Pointer to the scalar. The scalar is an integer at
 * least equal to 0 and at most equal to the order of the curve minus 1.
 *
 * @param[in]      k_len Length of the scalar. This should be equal to the
 * domain length.
 *
 * @return               Error code:
 *                       - CX_OK on success
 *                       - CX_NOT_LOCKED
 *                       - CX_INVALID_PARAMETER
 *                       - CX_EC_INVALID_POINT
 *                       - CX_EC_INVALID_CURVE
 *                       - CX_EC_INFINITE_POINT
 *                       - CX_MEMORY_FULL
 */
SYSCALL cx_err_t
cx_ecpoint_rnd_fixed_scalarmul(cx_ecpoint_t *P PLENGTH(sizeof(cx_ecpoint_t)),
                               const uint8_t *k PLENGTH(k_len), size_t k_len);

/**
 * @brief   Perform a scalar multiplication.
 *
 * @details This should be called only for non critical purposes.
 *
 * @param[in, out] P      Pointer to a point on a curve. This will hold the
 * result.
 *
 * @param[in]      k      Pointer to the scalar. The scalar is an integer at
 * least equal to 0 and at most equal to the order of the curve minus 1.
 *
 * @param[in]      k_len  Length of the scalar.
 *
 * @return                Error code:
 *                        - CX_OK on success
 *                        - CX_NOT_LOCKED
 *                        - CX_INVALID_PARAMETER
 *                        - CX_EC_INVALID_POINT
 *                        - CX_EC_INVALID_CURVE
 *                        - CX_EC_INFINITE_POINT
 *                        - CX_MEMORY_FULL
 */
SYSCALL cx_err_t
cx_ecpoint_scalarmul(cx_ecpoint_t *P PLENGTH(sizeof(cx_ecpoint_t)),
                     const uint8_t *k PLENGTH(k_len), size_t k_len);

/**
 * @brief   Perform a scalar multiplication given the BN index of the scalar.
 *
 * @details This should be called only for non critical purposes.
 *
 * @param[in, out] P    Pointer to a point on a curve. This will hold the
 * result.
 *
 * @param[in]      bn_k BN index of the scalar. The scalar is an integer at
 * least equal to 0 and at most equal to the order of the curve minus 1.
 *
 * @return              Error code:
 *                      - CX_OK on success
 *                      - CX_NOT_LOCKED
 *                      - CX_INVALID_PARAMETER
 *                      - CX_EC_INVALID_POINT
 *                      - CX_EC_INVALID_CURVE
 *                      - CX_EC_INFINITE_POINT
 *                      - CX_MEMORY_FULL
 */
SYSCALL cx_err_t
cx_ecpoint_scalarmul_bn(cx_ecpoint_t *P PLENGTH(sizeof(cx_ecpoint_t)),
                        const cx_bn_t bn_k PLENGTH(sizeof(cx_bn_t)));

/**
 * @brief   Perform a double scalar multiplication.
 *
 * @details This implements the Straus-Shamir algorithm for computing **R = [k]P
 * + [r]Q**. This should be used only for non-secret computations.
 *
 * @param[out] R     Pointer to the result.
 *
 * @param[in]  P     Pointer to the first point.
 *
 * @param[in]  Q     Pointer to the second point.
 *
 * @param[in]  k     Pointer to the first scalar.
 *
 * @param[in]  k_len Length of the first scalar.
 *
 * @param[in]  r     Pointer to the second scalar.
 *
 * @param[in]  r_len Length of the second scalar.
 *
 * @return           Error code:
 *                   - CX_OK on success
 *                   - CX_NOT_LOCKED
 *                   - CX_INVALID_PARAMETER
 *                   - CX_EC_INVALID_POINT
 *                   - CX_EC_INVALID_CURVE
 *                   - CX_MEMORY_FULL
 *                   - CX_EC_INFINITE_POINT
 */
SYSCALL cx_err_t
cx_ecpoint_double_scalarmul(cx_ecpoint_t *R PLENGTH(sizeof(cx_ecpoint_t)),
                            cx_ecpoint_t *P PLENGTH(sizeof(cx_ecpoint_t)),
                            cx_ecpoint_t *Q PLENGTH(sizeof(cx_ecpoint_t)),
                            const uint8_t *k PLENGTH(k_len), size_t k_len,
                            const uint8_t *r PLENGTH(r_len), size_t r_len);

/**
 * @brief   Perform a double scalar multiplication given the BN indexes of the
 * scalars.
 *
 * @details This implements the Straus-Shamir algorithm for computing **R = [k]P
 * + [r]Q**. This should be used only for non-secret computations.
 *
 * @param[out] R     Pointer to the result.
 *
 * @param[in]  P     Pointer to the first point.
 *
 * @param[in]  Q     Pointer to the second point.
 *
 * @param[in]  bn_k  BN index of the first scalar.
 *
 * @param[in]  bn_r  BN index of the second scalar.
 *
 * @return           Error code:
 *                  - CX_OK on success
 *                  - CX_NOT_LOCKED
 *                  - CX_INVALID_PARAMETER
 *                  - CX_EC_INVALID_POINT
 *                  - CX_EC_INVALID_CURVE
 *                  - CX_MEMORY_FULL
 *                  - CX_EC_INFINITE_POINT
 */
SYSCALL cx_err_t
cx_ecpoint_double_scalarmul_bn(cx_ecpoint_t *R PLENGTH(sizeof(cx_ecpoint_t)),
                               cx_ecpoint_t *P PLENGTH(sizeof(cx_ecpoint_t)),
                               cx_ecpoint_t *Q PLENGTH(sizeof(cx_ecpoint_t)),
                               const cx_bn_t bn_k, const cx_bn_t bn_r);

/**
 * @brief   Compare two points on the same curve.
 *
 * @param[in]  P        First point to compare.
 *
 * @param[in]  Q        Second point to compare.
 *
 * @param[out] is_equal Boolean which indicates whether the two points are equal
 * or not:
 *                      - 1 if the points are equal
 *                      - 0 otherwise
 *
 * @return              Error code:
 *                      - CX_OK on success
 *                      - CX_NOT_LOCKED
 *                      - CX_INVALID_PARAMETER
 *                      - CX_EC_INVALID_CURVE
 *                      - CX_EC_INFINITE_POINT
 *                      - CX_MEMORY_FULL
 */
SYSCALL cx_err_t cx_ecpoint_cmp(
    const cx_ecpoint_t *P PLENGTH(sizeof(cx_ecpoint_t)),
    const cx_ecpoint_t *Q PLENGTH(sizeof(cx_ecpoint_t)), bool *is_equal);

/**
 * @brief   Check if a given point is on the curve.
 *
 * @param[in]  R           Pointer to the point to check.
 *
 * @param[out] is_on_curve Boolean which indicates whether the point is on the
 * curve or not:
 *                         - 1 if the point is on the curve
 *                         - 0 otherwise
 *
 * @return                 Error code:
 *                         - CX_OK on success
 *                         - CX_NOT_LOCKED
 *                         - CX_INVALID_PARAMETER
 *                         - CX_EC_INVALID_CURVE
 *                         - CX_EC_INFINITE_POINT
 *                         - CX_MEMORY_FULL
 */
SYSCALL cx_err_t cx_ecpoint_is_on_curve(
    const cx_ecpoint_t *R PLENGTH(sizeof(cx_ecpoint_t)), bool *is_on_curve);

/**
 * @brief   Check if a given point is the point at infinity.
 *
 * @details The point at infinity has a z-coordinate equal to 0.
 *
 * @param[in]  R              Pointer to the point to check.
 *
 * @param[out] is_at_infinity Boolean which indicates whether the point is at
 * infinity or not:
 *                            - 1 if the point is at infinity
 *                            - 0 otherwise
 *
 * @return                    Error code:
 *                            - CX_OK on success
 *                            - CX_NOT_LOCKED
 *                            - CX_INVALID_PARAMETER
 *                            - CX_EC_INVALID_CURVE
 */
SYSCALL cx_err_t cx_ecpoint_is_at_infinity(
    const cx_ecpoint_t *R PLENGTH(sizeof(cx_ecpoint_t)), bool *is_at_infinity);

#ifdef HAVE_X25519
/**
 * @brief   x25519 function: scalar multiplication on Curve25519.
 *
 * @details x25519 performs the scalar multiplication on Curve25519
 *          with the u-coordinate only. The function returns
 *          only the u-coordinate of the result point.
 *
 * @param[in, out] bn_u  BN index of the u-coordinate. This will hold
 *                       the result.
 *
 * @param[in]      k     Pointer to the scalar.
 *
 * @param[in]      k_len Length of the scalar.
 *
 * @return               Error code:
 *                       - CX_OK on success
 *                       - CX_NOT_LOCKED
 *                       - CX_INVALID_PARAMETER
 *                       - CX_EC_INFINITE_POINT
 *                       - CX_MEMORY_FULL
 *
 */
SYSCALL cx_err_t cx_ecpoint_x25519(const cx_bn_t bn_u, const uint8_t *k,
                                   size_t k_len);
#endif // HAVE_X25519

#ifdef HAVE_X448
/**
 * @brief   x448 function: scalar multiplication on Curve448.
 *
 * @details x448 performs the scalar multiplication on Curve448
 *          with the u-coordinate only. The function returns
 *          only the u-coordinate of the result point.
 *
 * @param[in, out] bn_u  BN index of the u-coordinate. This will hold
 *                       the result.
 *
 * @param[in]      k     Pointer to the scalar.
 *
 * @param[in]      k_len Length of the scalar.
 *
 * @return               Error code:
 *                       - CX_OK on success
 *                       - CX_NOT_LOCKED
 *                       - CX_INVALID_PARAMETER
 *                       - CX_EC_INFINITE_POINT
 *                       - CX_MEMORY_FULL
 *
 */
SYSCALL cx_err_t cx_ecpoint_x448(const cx_bn_t bn_u, const uint8_t *k,
                                 size_t k_len);
#endif // HAVE_X448

#endif
