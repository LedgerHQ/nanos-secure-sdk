/* @BANNER@ */
#if defined(HAVE_VSS)
/**
 * @file    ox_vss.h
 * @brief   Verifiable seret sharing over SECP384R1
 *
 * This file contains the functions to share then reconstruct a secret over SECP384R1.
 */

#ifndef OX_VSS_H
#define OX_VSS_H

#include "ox.h"
#include <stddef.h>

/** Size of one share value */
#define CX_VSS_SECRET_SIZE (2 * 48)

/**
 * @brief Structure to hold a share of a secret over SECP384R1.
 *
 */
typedef struct {
    uint8_t  share[CX_VSS_SECRET_SIZE];  ///< The evaluation of two secret polynomials
    uint32_t index;                      ///< The value to evaluate the polynomials on
} cx_vss_share_t;

/**
 * @brief Structure to hold a commitment over SECP384R1.
 *
 */
typedef struct {
    uint8_t commitment[CX_VSS_SECRET_SIZE];  ///< A point on SECP384R1 which serves as a commitment.
} cx_vss_commitment_t;

/**
 * @brief This function generates some shares along with the commitments.
 *
 * @param[out] shares        Pointer to the shares.
 * @param[out] commits       Pointer to the commitments.
 * @param[in]  point         Point on SECP384R1 used to compute the commitments.
 * @param[in]  point_len     Length of the point. This must be 2 * 48 bytes.
 * @param[in]  seed          Pointer to a seed used to derive the secret coefficients of the
 * polynomials.
 * @param[in]  seed_len      Length of the seed.
 * @param[in]  secret        Pointer to the secret to share.
 * @param[in]  secret_len    Length of the secret. This must be less than 48 bytes.
 * @param[in]  shares_number Number of shares to return.
 * @param[in]  threshold     Threshold of the secret sharing.
 * @return Error code
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t
cx_vss_generate_shares(cx_vss_share_t *shares       PLENGTH(shares_number),
                       cx_vss_commitment_t *commits PLENGTH(threshold),
                       const uint8_t               *point,
                       size_t                       point_len,
                       const uint8_t *seed          PLENGTH(seed_len),
                       size_t                       seed_len,
                       const uint8_t *secret        PLENGTH(secret_len),
                       size_t                       secret_len,
                       uint8_t                      shares_number,
                       uint8_t                      threshold);

/**
 * @brief This function combines the given shares to get a secret.
 *
 * @param[out] secret     Pointer to the reconstructed secret.
 * @param[in]  secret_len Length of the expected secret. This must not exceed 48 bytes.
 * @param[in]  shares     Pointer to the shares.
 * @param[in]  threshold  Threshold of the secret sharing.
 * @return Error code
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t
cx_vss_combine_shares(uint8_t *secret        PLENGTH(secret_len),
                      size_t                 secret_len,
                      cx_vss_share_t *shares PLENGTH(sizeof(cx_vss_share_t)),
                      uint8_t                threshold);

/**
 * @brief This function verifies whether the coefficients commitments match
 *        the given share's commitment.
 *
 * @param[in]  commitments      Pointer to the coefficients commitments.
 * @param[in]  threshold        Threshold parameter of the scheme. It corresponds
 *                              to the number of coefficients commitments.
 * @param[in]  share_commitment Pointer to the share's commitment.
 * @param[in]  share_index      Index of the share corresponding to 'share_commitment'.
 * @param[out] verified         The commitments match.
 * @return Error code.
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t cx_vss_verify_commits(cx_vss_commitment_t *commitments,
                                                          uint8_t              threshold,
                                                          cx_vss_commitment_t *share_commitment,
                                                          uint32_t             share_index,
                                                          bool                *verified);

#endif  /* OX_VSS_H */
#endif  // HAVE_VSS
