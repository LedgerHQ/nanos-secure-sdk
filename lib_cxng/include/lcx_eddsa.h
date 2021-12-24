
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
 * @file    lcx_eddsa.h
 * @brief   EDDSA (Edwards Curve Digital Signature Algorithm)
 *
 * EDDSA is a digital signature scheme relying on Edwards curves, especially
 * Ed25519 and Ed448. Refer to <a href="https://tools.ietf.org/html/rfc8032"> RFC8032 </a>
 * for more details.
 */

#ifdef HAVE_EDDSA

#include "lcx_ecfp.h"
#include "lcx_wrappers.h"

#ifndef LCX_EDDSA_H
#define LCX_EDDSA_H

/**
 * @brief   Sign a message digest.
 * 
 * @details Sign a message digest according to the EDDSA specification
 *          <a href="https://tools.ietf.org/html/rfc8032"> RFC8032 </a>.
 * 
 * @param[in]  pvkey    Private key.
 *                      This shall be initialized with #cx_ecfp_init_private_key_no_throw.
 * 
 * @param[in]  hashID   Message digest agorithm identifier.
 *                      Algorithms supported: 
 *                        - SHA512
 *                        - SHA3
 *                        - Keccak
 * 
 * @param[in]  hash     Pointer to the message digest.
 * 
 * @param[in]  hash_len Length of the digest.
 * 
 * @param[out] sig      Buffer where to store the signature.
 * 
 * @param[in]  sig_len  Length of the signature.
 * 
 * @return              Error code:
 *                      - CX_OK on success
 *                      - CX_EC_INVALID_CURVE
 *                      - CX_INVALID_PARAMETER
 *                      - INVALID_PARAMETER
 *                      - CX_NOT_UNLOCKED
 *                      - CX_INVALID_PARAMETER_SIZE
 *                      - CX_MEMORY_FULL
 *                      - CX_NOT_LOCKED
 *                      - CX_INVALID_PARAMETER_SIZE
 *                      - CX_EC_INVALID_POINT
 *                      - CX_EC_INFINITE_POINT
 *                      - CX_INTERNAL_ERROR
 *                      - CX_INVALID_PARAMETER_VALUE
 */
cx_err_t cx_eddsa_sign_no_throw(const cx_ecfp_private_key_t *pvkey,
                       cx_md_t                      hashID,
                       const uint8_t *              hash,
                       size_t                       hash_len,
                       uint8_t *                    sig,
                       size_t                       sig_len);

/**
 * @brief   Sign a message digest.
 * 
 * @details Sign a message digest according to the EDDSA specification
 *          <a href="https://tools.ietf.org/html/rfc8032"> RFC8032 </a>.
 *          This function throws an exception if the computation doesn't
 *          succeed.
 * 
 * @param[in]  pvkey    Private key.
 *                      This shall be initialized with #cx_ecfp_init_private_key_no_throw.
 * 
 * @param[in]  mode     Mode. This parameter is not used.
 * 
 * @param[in]  hashID   Message digest agorithm identifier.
 *                      Algorithms supported: 
 *                        - SHA512
 *                        - SHA3
 *                        - Keccak
 * 
 * @param[in]  hash     Pointer to the message digest.
 * 
 * @param[in]  hash_len Length of the digest.
 * 
 * @param[in]  ctx      Pointer to the context. This parameter is not used.
 * 
 * @param[in]  ctx_len  Length of *ctx*. This parameter is not used.
 * 
 * @param[out] sig      Buffer where to store the signature.
 * 
 * @param[in]  sig_len  Length of the signature.
 * 
 * @param[in]  info     Additional information. This parameter is not used.
 * 
 * @return              Length of the signature.
 * 
 * @throws              CX_EC_INVALID_CURVE
 * @throws              CX_INVALID_PARAMETER
 * @throws              INVALID_PARAMETER
 * @throws              CX_NOT_UNLOCKED
 * @throws              CX_INVALID_PARAMETER_SIZE
 * @throws              CX_MEMORY_FULL
 * @throws              CX_NOT_LOCKED
 * @throws              CX_INVALID_PARAMETER_SIZE
 * @throws              CX_EC_INVALID_POINT
 * @throws              CX_EC_INFINITE_POINT
 * @throws              CX_INTERNAL_ERROR
 * @throws              CX_INVALID_PARAMETER_VALUE
 */
static inline int cx_eddsa_sign ( const cx_ecfp_private_key_t * pvkey, int mode, cx_md_t hashID, const unsigned char * hash, unsigned int hash_len, const unsigned char * ctx, unsigned int ctx_len, unsigned char * sig, unsigned int sig_len, unsigned int * info )
{
  UNUSED(ctx);
  UNUSED(ctx_len);
  UNUSED(mode);
  UNUSED(info);

  CX_THROW(cx_eddsa_sign_no_throw(pvkey, hashID, hash, hash_len, sig, sig_len));

  size_t size;
  CX_THROW(cx_ecdomain_parameters_length(pvkey->curve, &size));

  return 2 * size;
}

/**
 * @brief   Verify a signature.
 * 
 * @details Verify a signature according to the specification
 *          <a href="https://tools.ietf.org/html/rfc8032"> RFC8032 </a>.
 * 
 * @param[in]  pukey    Public key.
 *                      This shall be initialized with #cx_ecfp_init_public_key_no_throw.
 * 
 * @param[in]  hashID   Message digest agorithm identifier.
 *                      Algorithms supported: 
 *                        - SHA512
 *                        - SHA3
 *                        - Keccak
 * 
 * @param[in]  hash     Pointer to the message digest.
 * 
 * @param[in]  hash_len Length of the digest.
 * 
 * @param[out] sig      Pointer to the signature.
 * 
 * @param[in]  sig_len  Length of the signature.
 * 
 * @return              1 if the signature is verified, otherwise 0.
 */
bool cx_eddsa_verify_no_throw(const cx_ecfp_public_key_t *pukey,
                     cx_md_t                     hashID,
                     const uint8_t *             hash,
                     size_t                      hash_len,
                     const uint8_t *             sig,
                     size_t                      sig_len);

/**
 * @brief   Verify a signature.
 * 
 * @details Verify a signature according to the specification
 *          <a href="https://tools.ietf.org/html/rfc8032"> RFC8032 </a>.
 *          This function throws an exception if the computation doesn't
 *          succeed.
 * 
 * @param[in]  pukey    Public key.
 *                      THis shall be initialized with #cx_ecfp_init_public_key_no_throw.
 * 
 * @param[in]  mode     Mode. This parameter is not used.
 * 
 * @param[in]  hashID   Message digest agorithm identifier.
 *                      Algorithms supported: 
 *                        - SHA512
 *                        - SHA3
 *                        - Keccak
 * 
 * @param[in]  hash     Pointer to the message digest.
 * 
 * @param[in]  hash_len Length of the digest.
 * 
 * @param[in]  ctx      Pointer to the context. This parameter is not used.
 * 
 * @param[in]  ctx_len  Length of the context. This parameter is not used.
 * 
 * @param[out] sig      Pointer to the signature.
 * 
 * @param[in]  sig_len  Length of the signature.
 * 
 * @return              1 if the signature is verified, otherwise 0.
 */
static inline int cx_eddsa_verify ( const cx_ecfp_public_key_t * pukey, int mode, cx_md_t hashID, const unsigned char * hash, unsigned int hash_len, const unsigned char * ctx, unsigned int ctx_len, const unsigned char * sig, unsigned int sig_len )
{
  UNUSED(mode);
  UNUSED(ctx);
  UNUSED(ctx_len);

  return cx_eddsa_verify_no_throw(pukey, hashID, hash, hash_len, sig, sig_len);
}


/**
 * @brief   Encode the curve point coordinates.
 *
 * @param[in] coord A pointer to the point coordinates in the form x|y.
 *
 * @param[in] len   Length of the coordinates.
 *
 * @param[in] sign  Sign of the x-coordinate.
 *
 */
  void cx_encode_coord(uint8_t * coord,
                            int len,
                            int sign);

/**
 * @brief   Decode the curve point coordinates.
 *
 * @param[in] coord A pointer to the point encoded coordinates.
 *
 * @param[in] len   Length of the encoded coordinates.
 *
 * @return Sign of the x-coordinate.
 */
  int cx_decode_coord(uint8_t * coord,
                        int len);

#endif

#endif // HAVE_EDDSA
