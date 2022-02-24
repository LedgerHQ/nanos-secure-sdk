
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
 * @file    lcx_ecschnorr.h
 * @brief   ECSDSA (Elliptic Curve-based Schnorr Digital Signature Algorithm).
 *
 * Schnorr signature algorithm is a non-standard alternative to ECDSA.
 * Several implementations of Schnorr signature algorithm are supported here.
 */

#ifdef HAVE_ECSCHNORR

#include "lcx_wrappers.h"

#ifndef LCX_ECSCHNORR_H
#define LCX_ECSCHNORR_H

/**
 * @brief   Sign a digest message according to the given mode.
 *
 * @param[in]  pvkey   Pointer to the private key initialized with 
 *                     #cx_ecfp_init_private_key_no_throw beforehand.
 *
 * @param[in]  mode    Mode. Supported flag:
 *                       - CX_ECSCHNORR_XY
 *                       - CX_ECSCHNORR_ISO14888_X
 *                       - CX_ECSCHNORR_BSI03111
 *                       - CX_ECSCHNORR_LIBSECP
 *                       - CX_ECSCHNORR_Z
 *
 * @param[in]  hashID  Message digest algorithm identifier.
 *                     This parameter is mandatory when
 *                     using the CX_RND_RFC6979 
 *                     pseudorandom number generator.
 *
 * @param[in]  msg     Input data to sign.
 *
 * @param[in]  msg_len Length of input data.
 *
 * @param[out] sig     ECSchnorr signature encoded in TLV: **30 || L || 02 || Lr || r || 02 || Ls || s**.
 *   
 *
 * @param[in]  sig_len Length of the signature.
 *
 * @return             Error code:
 *                     - CX_OK on success
 *                     - CX_EC_INVALID_CURVE
 *                     - CX_INVALID_PARAMETER
 *                     - CX_NOT_UNLOCKED
 *                     - CX_INVALID_PARAMETER_SIZE
 *                     - CX_NOT_LOCKED
 *                     - CX_MEMORY_FULL
 *                     - CX_EC_INVALID_POINT
 *                     - CX_EC_INFINITE_POINT
 *                     - CX_INVALID_PARAMETER_VALUE
 */
cx_err_t cx_ecschnorr_sign_no_throw(const cx_ecfp_private_key_t *pvkey,
                           uint32_t                     mode,
                           cx_md_t                      hashID,
                           const uint8_t *              msg,
                           size_t                       msg_len,
                           uint8_t *                    sig,
                           size_t *                     sig_len);

/**
 * @brief   Sign a digest message according to the given mode.
 * 
 * @details This function throws an exception if the computation
 *          doesn't succeed.
 *
 * @param[in]  pvkey   Pointer to the private key initialized with 
 *                     #cx_ecfp_init_private_key_no_throw beforehand.
 *
 * @param[in]  mode    Mode. Supported flag:
 *                       - CX_ECSCHNORR_XY
 *                       - CX_ECSCHNORR_ISO14888_X
 *                       - CX_ECSCHNORR_BSI03111
 *                       - CX_ECSCHNORR_LIBSECP
 *                       - CX_ECSCHNORR_Z
 *
 * @param[in]  hashID  Message digest algorithm identifier.
 *                     This parameter is mandatory when
 *                     using the CX_RND_RFC6979 
 *                     pseudorandom number generator.
 *
 * @param[in]  msg     Input data to sign.
 *
 * @param[in]  msg_len Length of input data.
 *
 * @param[out] sig     ECSchnorr signature encoded in TLV: **30 || L || 02 || Lr || r || 02 || Ls || s**.
 *   
 *
 * @param[in]  sig_len Length of the signature.
 * 
 * @param[in]  info    Additional information. This parameter is not used.
 *
 * @return             Length of the signature.
 * 
 * @throws             CX_EC_INVALID_CURVE
 * @throws             CX_INVALID_PARAMETER
 * @throws             CX_NOT_UNLOCKED
 * @throws             CX_INVALID_PARAMETER_SIZE
 * @throws             CX_NOT_LOCKED
 * @throws             CX_MEMORY_FULL
 * @throws             CX_EC_INVALID_POINT
 * @throws             CX_EC_INFINITE_POINT
 * @throws             CX_INVALID_PARAMETER_VALUE
 */
static inline int cx_ecschnorr_sign ( const cx_ecfp_private_key_t * pvkey, int mode, cx_md_t hashID, const unsigned char * msg, unsigned int msg_len, unsigned char * sig, size_t sig_len, unsigned int * info )
{
  UNUSED(info);
  CX_THROW(cx_ecschnorr_sign_no_throw(pvkey, mode, hashID, msg, msg_len, sig, &sig_len));
  return sig_len;
}

/**
 * @brief   Verify a hash message signature according to the given mode.
 * 
 * @param[in] pukey   Pointer to the public key initialized with 
 *                    #cx_ecfp_init_private_key_no_throw beforehand.
 *
 * @param[in] mode    Mode. Supported flag:
 *                       - CX_ECSCHNORR_XY
 *                       - CX_ECSCHNORR_ISO14888_X
 *                       - CX_ECSCHNORR_BSI03111
 *                       - CX_ECSCHNORR_LIBSECP
 *                       - CX_ECSCHNORR_Z
 *                       - CX_ECSCHNORR_BIP0340
 *
 * @param[in] hashID  Message digest algorithm identifier used to
 *                    compute the input data.
 *
 * @param[in] msg     Signed input data to verify the signature.
 *   
 *
 * @param[in] msg_len Length of the input data.
 *   
 *
 * @param[in] sig     ECSchnorr signature to verify encoded in
 *                    TLV: **30 || L || 02 || Lr || r || 02 || Ls || s**
 *   
 * 
 * @param[in] sig_len Length of the signature.
 *
 * @return            1 if signature is verified, 0 otherwise.
 */
bool cx_ecschnorr_verify(const cx_ecfp_public_key_t *pukey,
                         uint32_t                    mode,
                         cx_md_t                     hashID,
                         const uint8_t *             msg,
                         size_t                      msg_len,
                         const uint8_t *             sig,
                         size_t                      sig_len);

#endif

#endif // HAVE_ECSHCNORR
