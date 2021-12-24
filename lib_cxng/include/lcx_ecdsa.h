
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
 * @file    lcx_ecdsa.h
 * @brief   ECDSA (Elliptic Curve Digital Signature Algorithm).
 *
 * ECDSA is a standard digital signature scheme relying on elliptic curves.
 * It provides data integrity and verifiable authenticity. Refer to
 * <a href="https://tools.ietf.org/html/rfc6979"> RFC6979 </a> for more details.
 */

#ifdef HAVE_ECDSA

#ifndef LCX_ECDSA_H
#define LCX_ECDSA_H

#include "lcx_wrappers.h"
#include "lcx_ecfp.h"

/** @internal Backward compatibility */
#define cx_ecdsa_init_public_key cx_ecfp_init_public_key_no_throw
/** @internal Backward compatibility */
#define cx_ecdsa_init_private_key cx_ecfp_init_private_key_no_throw

/**
 * @brief   Sign a message digest according to ECDSA specification
 *
 * @param[in]  pvkey    Private key.
 *                      Shall be initialized with #cx_ecfp_init_private_key_no_throw.
 *
 * @param[in]  mode     Crypto mode flags.
 *                      Supported flags:
 *                        - CX_RND_TRNG
 *                        - CX_RND_RFC6979
 *
 * @param[in]  hashID   Message digest algorithm identifer.
 *                      This parameter is mandatory with the flag CX_RND_RFC6979.
 *
 * @param[in]  hash     Digest of the message to be signed.
 *                      The length of *hash* must be shorter than the curve domain size.
 *
 * @param[in]  hash_len Length of the digest in octets.
 *
 * @param[out] sig      Buffer where to store the signature.
 *                      The signature is encoded in TLV:  **30 || L || 02 || Lr || r || 02 || Ls || s**
 * 
 * @param[in]  sig_len  Length of the buffer in octets.
 *
 * @param[out] info     Set with CX_ECCINFO_PARITY_ODD if the y-coordinate is odd when computing **[k].G**.
 *
 * @return              Error code:
 *                      - CX_OK on success
 *                      - CX_EC_INVALID_CURVE
 *                      - CX_INVALID_PARAMETER
 *                      - CX_INTERNAL_ERROR
 *                      - CX_NOT_UNLOCKED
 *                      - CX_INVALID_PARAMETER_SIZE
 *                      - CX_MEMORY_FULL
 *                      - CX_NOT_LOCKED
 *                      - CX_EC_INVALID_POINT
 *                      - CX_EC_INFINITE_POINT
 *                      - CX_INVALID_PARAMETER_VALUE
 */
cx_err_t cx_ecdsa_sign_no_throw(const cx_ecfp_private_key_t *pvkey,
                       uint32_t                     mode,
                       cx_md_t                      hashID,
                       const uint8_t *              hash,
                       size_t                       hash_len,
                       uint8_t *                    sig,
                       size_t *                     sig_len,
                       uint32_t *                   info);

/**
 * @brief   Sign a message digest according to ECDSA specification.
 * 
 * @details This functions throws an exception if the signature
 *          doesn't succeed.
 *
 * @param[in]  pvkey    Private key.
 *                      Shall be initialized with #cx_ecfp_init_private_key_no_throw.
 *
 * @param[in]  mode     Crypto mode flags.
 *                      Supported flags:
 *                        - CX_RND_TRNG
 *                        - CX_RND_RFC6979
 *
 * @param[in]  hashID   Message digest algorithm identifer.
 *                      This parameter is mandatory with the flag CX_RND_RFC6979.
 *
 * @param[in]  hash     Digest of the message to be signed.
 *                      The length of *hash* must be shorter than the group order size.
 *                      Otherwise it is truncated.
 *
 * @param[in]  hash_len Length of the digest in octets.
 *
 * @param[out] sig      Buffer where to store the signature.
 *                      The signature is encoded in TLV:  **30 || L || 02 || Lr || r || 02 || Ls || s**
 * 
 * @param[in]  sig_len  Length of the buffer in octets.
 *
 * @param[out] info     Set with CX_ECCINFO_PARITY_ODD if the y-coordinate is odd when computing **[k].G**.
 *
 * @return              Length of the signature.
 * 
 * @throws              CX_EC_INVALID_CURVE
 * @throws              CX_INVALID_PARAMETER
 * @throws              CX_INTERNAL_ERROR
 * @throws              CX_NOT_UNLOCKED
 * @throws              CX_INVALID_PARAMETER_SIZE
 * @throws              CX_MEMORY_FULL
 * @throws              CX_NOT_LOCKED
 * @throws              CX_EC_INVALID_POINT
 * @throws              CX_EC_INFINITE_POINT
 * @throws              CX_INVALID_PARAMETER_VALUE
 */
static inline int cx_ecdsa_sign ( const cx_ecfp_private_key_t * pvkey, int mode, cx_md_t hashID, const unsigned char * hash, unsigned int hash_len, unsigned char * sig, unsigned int sig_len, unsigned int * info )
{
  size_t sig_len_ = sig_len;
  uint32_t info_;
  CX_THROW(cx_ecdsa_sign_no_throw(pvkey, mode, hashID, hash, hash_len, sig, &sig_len_, &info_));
  if (info) {
    *info = (uint32_t)info_;
  }
  return sig_len_;
}


/**
 * @brief   Verify an ECDSA signature according to ECDSA specification.
 * 
 * @param[in] pukey    Private key initialized with #cx_ecfp_init_public_key_no_throw.
 * 
 * @param[in] hash     Digest of the message to be verified.
 *                     The length of *hash* must be smaller than the group order size.
 *                     Otherwise it is truncated.
 * 
 * @param[in] hash_len Length of the digest in octets.
 * 
 * @param[in] sig      Pointer to the signature encoded in TLV: **30 || L || 02 || Lr || r || 02 || Ls || s**
 * 
 * @param[in] sig_len  Length of the signature in octets.
 * 
 * @return             1 if the signature is verified, 0 otherwise.
 */
bool cx_ecdsa_verify_no_throw(const cx_ecfp_public_key_t *pukey,
                     const uint8_t *             hash,
                     size_t                      hash_len,
                     const uint8_t *             sig,
                     size_t                      sig_len);

/**
 * @brief   Verify an ECDSA signature according to ECDSA specification.
 * 
 * @param[in] pukey    Private key initialized with #cx_ecfp_init_public_key_no_throw.
 * 
 * @param[in] mode     ECDSA mode. This parameter is not used.
 * 
 * @param[in] hashID   Message digest algorithm identifer.
 *                     This parameter is not used.
 * 
 * @param[in] hash     Digest of the message to be verified.
 *                     The length of *hash* must be smaller than the curve domain size.
 * 
 * @param[in] hash_len Length of the digest in octets.
 * 
 * @param[in] sig      Pointer to the signature encoded in TLV: **30 || L || 02 || Lr || r || 02 || Ls || s**
 * 
 * @param[in] sig_len  Length of the signature in octets.
 * 
 * @return             1 if the signature is verified, 0 otherwise.
 */
static inline bool cx_ecdsa_verify ( const cx_ecfp_public_key_t * pukey, int mode, cx_md_t hashID, const unsigned char * hash, unsigned int hash_len, const unsigned char * sig, unsigned int sig_len)
{
  UNUSED(mode);
  UNUSED(hashID);
  return cx_ecdsa_verify_no_throw(pukey, hash, hash_len, sig, sig_len);
}


#endif

#endif // HAVE_ECDSA
