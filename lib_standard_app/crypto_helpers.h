#pragma once

#include <stdint.h>  // uint*_t

#include "os.h"
#include "cx.h"

/**
 * @brief   Gets the private key from the device seed using the specified bip32 path and seed key.
 *
 * @param[in]  derivation_mode Derivation mode, one of HDW_NORMAL / HDW_ED25519_SLIP10 / HDW_SLIP21.
 *
 * @param[in]  curve           Curve identifier.
 *
 * @param[in]  path            Bip32 path to use for derivation.
 *
 * @param[in]  path_len        Bip32 path length.
 *
 * @param[out] privkey         Generated private key.
 *
 * @param[out] chain_code      Buffer where to store the chain code. Can be NULL.
 *
 * @param[in]  seed            Seed key to use for derivation.
 *
 * @param[in]  seed_len        Seed key length.
 *
 * @return                     Error code:
 *                             - CX_OK on success
 *                             - CX_EC_INVALID_CURVE
 *                             - CX_INTERNAL_ERROR
 */
WARN_UNUSED_RESULT cx_err_t
bip32_derive_with_seed_init_privkey_256(unsigned int               derivation_mode,
                                        cx_curve_t                 curve,
                                        const uint32_t            *path,
                                        size_t                     path_len,
                                        cx_ecfp_256_private_key_t *privkey,
                                        uint8_t                   *chain_code,
                                        unsigned char             *seed,
                                        size_t                     seed_len);

/**
 * @brief   Gets the private key from the device seed using the specified bip32 path.
 *
 * @param[in]  curve           Curve identifier.
 *
 * @param[in]  path            Bip32 path to use for derivation.
 *
 * @param[in]  path_len        Bip32 path length.
 *
 * @param[out] privkey         Generated private key.
 *
 * @param[out] chain_code      Buffer where to store the chain code. Can be NULL.
 *
 * @return                     Error code:
 *                             - CX_OK on success
 *                             - CX_EC_INVALID_CURVE
 *                             - CX_INTERNAL_ERROR
 */
WARN_UNUSED_RESULT static inline cx_err_t bip32_derive_init_privkey_256(
    cx_curve_t                 curve,
    const uint32_t            *path,
    size_t                     path_len,
    cx_ecfp_256_private_key_t *privkey,
    uint8_t                   *chain_code)
{
    return bip32_derive_with_seed_init_privkey_256(
        HDW_NORMAL, curve, path, path_len, privkey, chain_code, NULL, 0);
}

/**
 * @brief   Gets the public key from the device seed using the specified bip32 path and seed key.
 *
 * @param[in]  derivation_mode Derivation mode, one of HDW_NORMAL / HDW_ED25519_SLIP10 / HDW_SLIP21.
 *
 * @param[in]  curve           Curve identifier.
 *
 * @param[in]  path            Bip32 path to use for derivation.
 *
 * @param[in]  path_len        Bip32 path length.
 *
 * @param[out] raw_pubkey      Buffer where to store the public key.
 *
 * @param[out] chain_code      Buffer where to store the chain code. Can be NULL.
 *
 * @param[in]  hashID          Message digest algorithm identifier.
 *
 * @param[in]  seed            Seed key to use for derivation.
 *
 * @param[in]  seed_len        Seed key length.
 *
 * @return                     Error code:
 *                             - CX_OK on success
 *                             - CX_EC_INVALID_CURVE
 *                             - CX_INTERNAL_ERROR
 */
WARN_UNUSED_RESULT cx_err_t bip32_derive_with_seed_get_pubkey_256(unsigned int    derivation_mode,
                                                                  cx_curve_t      curve,
                                                                  const uint32_t *path,
                                                                  size_t          path_len,
                                                                  uint8_t  raw_pubkey[static 65],
                                                                  uint8_t *chain_code,
                                                                  cx_md_t  hashID,
                                                                  unsigned char *seed,
                                                                  size_t         seed_len);

/**
 * @brief   Gets the public key from the device seed using the specified bip32 path.
 *
 * @param[in]  curve           Curve identifier.
 *
 * @param[in]  path            Bip32 path to use for derivation.
 *
 * @param[in]  path_len        Bip32 path length.
 *
 * @param[out] raw_pubkey      Buffer where to store the public key.
 *
 * @param[out] chain_code      Buffer where to store the chain code. Can be NULL.
 *
 * @param[in]  hashID          Message digest algorithm identifier.
 *
 * @return                     Error code:
 *                             - CX_OK on success
 *                             - CX_EC_INVALID_CURVE
 *                             - CX_INTERNAL_ERROR
 */
WARN_UNUSED_RESULT static inline cx_err_t bip32_derive_get_pubkey_256(cx_curve_t      curve,
                                                                      const uint32_t *path,
                                                                      size_t          path_len,
                                                                      uint8_t raw_pubkey[static 65],
                                                                      uint8_t *chain_code,
                                                                      cx_md_t  hashID)
{
    return bip32_derive_with_seed_get_pubkey_256(
        HDW_NORMAL, curve, path, path_len, raw_pubkey, chain_code, hashID, NULL, 0);
}

/**
 * @brief   Sign a hash with ecdsa using the device seed derived from the specified bip32 path and
 * seed key.
 *
 * @param[in]  derivation_mode Derivation mode, one of HDW_NORMAL / HDW_ED25519_SLIP10 / HDW_SLIP21.
 *
 * @param[in]  curve           Curve identifier.
 *
 * @param[in]  path            Bip32 path to use for derivation.
 *
 * @param[in]  path_len        Bip32 path length.
 *
 * @param[in]  hashID          Message digest algorithm identifier.
 *
 * @param[in]  hash            Digest of the message to be signed.
 *                             The length of *hash* must be shorter than the group order size.
 *                             Otherwise it is truncated.
 *
 * @param[in]  hash_len        Length of the digest in octets.
 *
 * @param[out] sig             Buffer where to store the signature.
 *                             The signature is encoded in TLV:  **30 || L || 02 || Lr || r || 02 ||
 * Ls || s**
 *
 * @param[in]  sig_len         Length of the signature buffer, updated with signature length.
 *
 * @param[out] info            Set with CX_ECCINFO_PARITY_ODD if the y-coordinate is odd when
 * computing **[k].G**.
 *
 * @param[in]  seed            Seed key to use for derivation.
 *
 * @param[in]  seed_len        Seed key length.
 *
 * @return                     Error code:
 *                             - CX_OK on success
 *                             - CX_EC_INVALID_CURVE
 *                             - CX_INTERNAL_ERROR
 */
WARN_UNUSED_RESULT cx_err_t bip32_derive_with_seed_ecdsa_sign_hash_256(unsigned int derivation_mode,
                                                                       cx_curve_t   curve,
                                                                       const uint32_t *path,
                                                                       size_t          path_len,
                                                                       uint32_t        sign_mode,
                                                                       cx_md_t         hashID,
                                                                       const uint8_t  *hash,
                                                                       size_t          hash_len,
                                                                       uint8_t        *sig,
                                                                       size_t         *sig_len,
                                                                       uint32_t       *info,
                                                                       unsigned char  *seed,
                                                                       size_t          seed_len);

/**
 * @brief   Sign a hash with ecdsa using the device seed derived from the specified bip32 path.
 *
 * @param[in]  derivation_mode Derivation mode, one of HDW_NORMAL / HDW_ED25519_SLIP10 / HDW_SLIP21.
 *
 * @param[in]  curve           Curve identifier.
 *
 * @param[in]  path            Bip32 path to use for derivation.
 *
 * @param[in]  path_len        Bip32 path length.
 *
 * @param[in]  hashID          Message digest algorithm identifier.
 *
 * @param[in]  hash            Digest of the message to be signed.
 *                             The length of *hash* must be shorter than the group order size.
 *                             Otherwise it is truncated.
 *
 * @param[in]  hash_len        Length of the digest in octets.
 *
 * @param[out] sig             Buffer where to store the signature.
 *                             The signature is encoded in TLV:  **30 || L || 02 || Lr || r || 02 ||
 * Ls || s**
 *
 * @param[in]  sig_len         Length of the signature buffer, updated with signature length.
 *
 * @param[out] info            Set with CX_ECCINFO_PARITY_ODD if the y-coordinate is odd when
 * computing **[k].G**.
 *
 * @return                     Error code:
 *                             - CX_OK on success
 *                             - CX_EC_INVALID_CURVE
 *                             - CX_INTERNAL_ERROR
 */
WARN_UNUSED_RESULT static inline cx_err_t bip32_derive_ecdsa_sign_hash_256(cx_curve_t      curve,
                                                                           const uint32_t *path,
                                                                           size_t          path_len,
                                                                           uint32_t       sign_mode,
                                                                           cx_md_t        hashID,
                                                                           const uint8_t *hash,
                                                                           size_t         hash_len,
                                                                           uint8_t       *sig,
                                                                           size_t        *sig_len,
                                                                           uint32_t      *info)
{
    return bip32_derive_with_seed_ecdsa_sign_hash_256(HDW_NORMAL,
                                                      curve,
                                                      path,
                                                      path_len,
                                                      sign_mode,
                                                      hashID,
                                                      hash,
                                                      hash_len,
                                                      sig,
                                                      sig_len,
                                                      info,
                                                      NULL,
                                                      0);
}

/**
 * @brief   Sign a hash with ecdsa using the device seed derived from the specified bip32 path and
 * seed key.
 *
 * @param[in]  derivation_mode Derivation mode, one of HDW_NORMAL / HDW_ED25519_SLIP10 / HDW_SLIP21.
 *
 * @param[in]  curve           Curve identifier.
 *
 * @param[in]  path            Bip32 path to use for derivation.
 *
 * @param[in]  path_len        Bip32 path length.
 *
 * @param[in]  hashID          Message digest algorithm identifier.
 *
 * @param[in]  hash            Digest of the message to be signed.
 *                             The length of *hash* must be shorter than the group order size.
 *                             Otherwise it is truncated.
 *
 * @param[in]  hash_len        Length of the digest in octets.
 *
 * @param[out] sig_r               Buffer where to store the signature r value.
 *
 * @param[out] sig_s               Buffer where to store the signature s value.
 *
 * @param[out] info            Set with CX_ECCINFO_PARITY_ODD if the y-coordinate is odd when
 * computing **[k].G**.
 *
 * @param[in]  seed            Seed key to use for derivation.
 *
 * @param[in]  seed_len        Seed key length.
 *
 * @return                     Error code:
 *                             - CX_OK on success
 *                             - CX_EC_INVALID_CURVE
 *                             - CX_INTERNAL_ERROR
 */
WARN_UNUSED_RESULT cx_err_t
bip32_derive_with_seed_ecdsa_sign_rs_hash_256(unsigned int    derivation_mode,
                                              cx_curve_t      curve,
                                              const uint32_t *path,
                                              size_t          path_len,
                                              uint32_t        sign_mode,
                                              cx_md_t         hashID,
                                              const uint8_t  *hash,
                                              size_t          hash_len,
                                              uint8_t         sig_r[static 32],
                                              uint8_t         sig_s[static 32],
                                              uint32_t       *info,
                                              unsigned char  *seed,
                                              size_t          seed_len);

/**
 * @brief   Sign a hash with ecdsa using the device seed derived from the specified bip32 path.
 *
 * @param[in]  curve           Curve identifier.
 *
 * @param[in]  path            Bip32 path to use for derivation.
 *
 * @param[in]  path_len        Bip32 path length.
 *
 * @param[in]  hashID          Message digest algorithm identifier.
 *
 * @param[in]  hash            Digest of the message to be signed.
 *                             The length of *hash* must be shorter than the group order size.
 *                             Otherwise it is truncated.
 *
 * @param[in]  hash_len        Length of the digest in octets.
 *
 * @param[out] sig_r           Buffer where to store the signature r value.
 *
 * @param[out] sig_s           Buffer where to store the signature s value.
 *
 * @param[out] info            Set with CX_ECCINFO_PARITY_ODD if the y-coordinate is odd when
 * computing **[k].G**.
 *
 * @return                     Error code:
 *                             - CX_OK on success
 *                             - CX_EC_INVALID_CURVE
 *                             - CX_INTERNAL_ERROR
 */
WARN_UNUSED_RESULT static inline cx_err_t bip32_derive_ecdsa_sign_rs_hash_256(
    cx_curve_t      curve,
    const uint32_t *path,
    size_t          path_len,
    uint32_t        sign_mode,
    cx_md_t         hashID,
    const uint8_t  *hash,
    size_t          hash_len,
    uint8_t         sig_r[static 32],
    uint8_t         sig_s[static 32],
    uint32_t       *info)
{
    return bip32_derive_with_seed_ecdsa_sign_rs_hash_256(HDW_NORMAL,
                                                         curve,
                                                         path,
                                                         path_len,
                                                         sign_mode,
                                                         hashID,
                                                         hash,
                                                         hash_len,
                                                         sig_r,
                                                         sig_s,
                                                         info,
                                                         NULL,
                                                         0);
}

/**
 * @brief   Sign a hash with eddsa using the device seed derived from the specified bip32 path and
 * seed key.
 *
 * @param[in]  derivation_mode Derivation mode, one of HDW_NORMAL / HDW_ED25519_SLIP10 / HDW_SLIP21.
 *
 * @param[in]  curve           Curve identifier.
 *
 * @param[in]  path            Bip32 path to use for derivation.
 *
 * @param[in]  path_len        Bip32 path length.
 *
 * @param[in]  hashID          Message digest algorithm identifier.
 *
 * @param[in]  hash            Digest of the message to be signed.
 *                             The length of *hash* must be shorter than the group order size.
 *                             Otherwise it is truncated.
 *
 * @param[in]  hash_len        Length of the digest in octets.
 *
 * @param[out] sig             Buffer where to store the signature.
 *
 * @param[in]  sig_len         Length of the signature buffer, updated with signature length.
 *
 * @param[in]  seed            Seed key to use for derivation.
 *
 * @param[in]  seed_len        Seed key length.
 *
 * @return                     Error code:
 *                             - CX_OK on success
 *                             - CX_EC_INVALID_CURVE
 *                             - CX_INTERNAL_ERROR
 */
WARN_UNUSED_RESULT cx_err_t bip32_derive_with_seed_eddsa_sign_hash_256(unsigned int derivation_mode,
                                                                       cx_curve_t   curve,
                                                                       const uint32_t *path,
                                                                       size_t          path_len,
                                                                       cx_md_t         hashID,
                                                                       const uint8_t  *hash,
                                                                       size_t          hash_len,
                                                                       uint8_t        *sig,
                                                                       size_t         *sig_len,
                                                                       unsigned char  *seed,
                                                                       size_t          seed_len);

/**
 * @brief   Sign a hash with eddsa using the device seed derived from the specified bip32 path.
 *
 * @param[in]  curve           Curve identifier.
 *
 * @param[in]  path            Bip32 path to use for derivation.
 *
 * @param[in]  path_len        Bip32 path length.
 *
 * @param[in]  hashID          Message digest algorithm identifier.
 *
 * @param[in]  hash            Digest of the message to be signed.
 *                             The length of *hash* must be shorter than the group order size.
 *                             Otherwise it is truncated.
 *
 * @param[in]  hash_len        Length of the digest in octets.
 *
 * @param[out] sig             Buffer where to store the signature.
 *
 * @param[in]  sig_len         Length of the signature buffer, updated with signature length.
 *
 * @return                     Error code:
 *                             - CX_OK on success
 *                             - CX_EC_INVALID_CURVE
 *                             - CX_INTERNAL_ERROR
 */
WARN_UNUSED_RESULT static inline cx_err_t bip32_derive_eddsa_sign_hash_256(cx_curve_t      curve,
                                                                           const uint32_t *path,
                                                                           size_t          path_len,
                                                                           cx_md_t         hashID,
                                                                           const uint8_t  *hash,
                                                                           size_t          hash_len,
                                                                           uint8_t        *sig,
                                                                           size_t         *sig_len)
{
    return bip32_derive_with_seed_eddsa_sign_hash_256(
        HDW_NORMAL, curve, path, path_len, hashID, hash, hash_len, sig, sig_len, NULL, 0);
}
