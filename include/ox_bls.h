/* @BANNER@ */

/**
 * @file    ox_bls.h
 * @brief   BLS signature.
 *
 */

#ifndef OX_BLS_H
#define OX_BLS_H

#ifdef HAVE_BLS
#include "lcx_ecfp.h"

#define CX_BLS_BLS12381_PARAM_LEN (48)
#define CX_BLS_PROVE              (0x9)
#define CX_BLS_BASE               (0x0)
#define CX_BLS_SIGN_AUG           (0xA)
#define CX_BLS_SIGN_POP           (0xF)

/**
 * @brief BLS12-381 key generation.
 *
 * @details If the raw value of the private key is pointed by *secret*, it is used to initialize
 *          the private key structure. Otherwise, the private key is first derived then the private
 *          key structure is initialized.
 *
 * @param[in] mode Signature scheme:
 *                 - Basic
 *                 - Message augmentation
 *                 - Proof of possession
 *                 The first bit of mode indicates whether the secret should be derived
 *                 or used as is for the private key.
 *
 * @param[in] secret         Pointer to either random bytes used to derive the private key or the
 * private key itself.
 *
 * @param[in] secret_len     Length of the secret which should be at most 32 bytes.
 *
 * @param[in] salt           Pointer to a salt.
 *
 * @param[in] salt_len       Length of the salt.
 *
 * @param[in] key_info       Pointer to an optional key information. If no key information is used,
 *                           this buffer must be allocated to hold at least 2 bytes.
 *
 * @param[in] key_info_len   Length of the key information.
 *
 * @param[in] private_key    Pointer to the private key structure.
 *
 * @param[in] public_key     Pointer to the public key. This is NULL for 'basic' and 'proof of
 * possession' schemes.
 *
 * @param[in] public_key_len Length of the public key.
 *
 * @return                   Error code:
 *                           - CX_OK on success
 *                           - CX_INVALID_PARAMETER
 *                           - CX_MEMORY_FULL
 *                           - CX_INVALID_PARAMETER_SIZE
 *                           - CX_EC_INVALID_POINT
 *                           - CX_EC_INFINITE_POINT
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t cx_bls12381_key_gen(uint8_t                    mode,
                                                        const uint8_t             *secret,
                                                        size_t                     secret_len,
                                                        const uint8_t             *salt,
                                                        size_t                     salt_len,
                                                        uint8_t                   *key_info,
                                                        size_t                     key_info_len,
                                                        cx_ecfp_384_private_key_t *private_key,
                                                        uint8_t                   *public_key,
                                                        size_t                     public_key_len);

/**
 * @brief   Hash a message to be signed with BLS12-381 signature scheme.
 *
 * @details The hash is represented by (h_0, h_1, h_2, h_3) where each h_i is
 *          48-byte long. Thus, the whole hash is 192-byte long.
 *
 * @param[in]  msg      Message to hash.
 *
 * @param[in]  msg_len  Length of the message.
 *
 * @param[in]  dst      Domain Separation Tag
 *
 * @param[in]  dst_len  Length of the Domain Separation Tag, at most 255 bytes.
 *
 * @param[out] hash     Hash of the message: a concatenation of field elements.
 *
 * @param[in]  hash_len Curve identifier.
 *
 *
 * @return              Error code:
 *                      - CX_OK on success
 *                      - CX_INVALID_PARAMETER
 *                      - CX_MEMORY_FULL
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t cx_hash_to_field(const uint8_t *msg,
                                                     size_t         msg_len,
                                                     const uint8_t *dst,
                                                     size_t         dst_len,
                                                     uint8_t       *hash,
                                                     size_t         hash_len);

/**
 * @brief   Sign the hash of a message.
 *
 * @details BLS signature with the curve BLS12-381-G2. The message must be hash beforehand with
 * #cx_hash_to_field.
 *
 * @param[in]  key           Private key. The private key must be initialized with
 * #cx_ecfp_init_private_key_no_throw given a 32-byte raw key.
 *
 * @param[in]  message       Hash of the message, result of #cx_hash_to_field.
 *
 * @param[in]  message_len   Length of the hash. This must be 192 bytes.
 *
 * @param[out] signature     Compressed signature: only the x-coordinate of the point is returned.
 *                           The most-significant three bits of the signature are used to
 * unambiguously determine the y-coordinate among the two possible choices. The fist bit is always 1
 * to indicate that the signature is in compressed form. The second bit is set iff the result point
 * is the point at infinity, i.e. all of the bits of signature must be 0. The third bit is set iff
 * the y-coordinate is greater than (p-1)/2 where p =
 * 0x1a0111ea397fe69a4b1ba7b6434bacd764774b84f38512bf6730d2a0f6b0f6241eabfffeb153ffffb9feffffffffaaab.
 *
 * @param[in]  signature_len Length of the signature. This must be 96 bytes.
 *
 * @return                   Error code:
 *                           - CX_OK on success
 *                           - CX_INTERNAL_ERROR
 *                           - CX_INVALID_PARAMETER
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t ox_bls12381_sign(const cx_ecfp_384_private_key_t *key,
                                                     const uint8_t                   *message,
                                                     size_t                           message_len,
                                                     uint8_t                         *signature,
                                                     size_t signature_len);

/**
 * @brief   Aggregate multiple signatures.
 *
 * @details BLS signature aggregation with the curve BLS12-381-G2. This function should be called
 *          for each signature to aggregate, with the first parameter set to 'true' for the first
 *          signature and false for the next signatures.
 *
 * @param[in]  in                   Signature to aggregate.
 *
 * @param[in]  in_len               Length of the input. This must be the length of one signature.
 *
 * @param[in]  first                Indicates if this is the first signature to aggregate.
 *
 * @param[out] aggregated_signature Aggregated signature in compressed form.
 *
 * @param[in]  signature_len        Length of the aggregated signature. This must be 96 bytes.
 *
 * @return                          Error code:
 *                                  - CX_OK on success
 *                                  - CX_INTERNAL_ERROR
 *                                  - CX_INVALID_PARAMETER
 */
SYSCALL WARN_UNUSED_RESULT cx_err_t cx_bls12381_aggregate(const uint8_t *in,
                                                          size_t         in_len,
                                                          bool           first,
                                                          uint8_t       *aggregated_signature,
                                                          size_t         signature_len);

#endif  // HAVE_BLS
#endif  // OX_BLS_H
