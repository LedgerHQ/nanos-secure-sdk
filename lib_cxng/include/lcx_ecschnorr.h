
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

#ifdef HAVE_ECSCHNORR

#include "lcx_wrappers.h"

#ifndef LCX_ECSCHNORR_H
#define LCX_ECSCHNORR_H

/**
 * Sign a hash message according to ECSchnorr specification (BSI TR 03111).
 *
 * @param [in] key
 *   A private ecfp key fully inited with 'cx_ecfp_init_private_key'
 *
 * @param [in] mode
 *   Crypto mode flags. See above.
 *   Supported flags:
 *     - CX_ECSCHNORR_XY
 *
 * @param [in] hashID
 *  Hash identifier used to compute the input data.
 *  This parameter is mandatory for rng of type CX_RND_RFC6979.
 *
 * @param [in] msg
 *   Input data to sign.
 *
 * @param [in] msg_len
 *   Length of input to data.
 *
 * @param [out] sig
 *   ECSchnorr signature encoded as TLV:  30 L 02 Lr r 02 Ls s
 *
 * @param [out] info
 *   Set to zero
 *
 * @return
 *   Full length of signature
 *
 * @throws INVALID_PARAMETER
 */
cx_err_t cx_ecschnorr_sign_no_throw(const cx_ecfp_private_key_t *pvkey,
                           uint32_t                     mode,
                           cx_md_t                      hashID,
                           const uint8_t *              msg,
                           size_t                       msg_len,
                           uint8_t *                    sig,
                           size_t *                     sig_len);

static inline int cx_ecschnorr_sign ( const cx_ecfp_private_key_t * pvkey, int mode, cx_md_t hashID, const unsigned char * msg, unsigned int msg_len, unsigned char * sig, unsigned int sig_len, unsigned int * info )
{
  UNUSED(info);
  CX_THROW(cx_ecschnorr_sign_no_throw(pvkey, mode, hashID, msg, msg_len, sig, &sig_len));
  return sig_len;
}

/**
 * Verify a hash message signature according to ECSchnorr specification (BSI TR 03111).
 *
 * @param [in] key
 *   A public ecfp key fully inited with 'cx_ecfp_init_public_key'
 *
 * @param [in] mode
 *   Crypto mode flags. See above.
 *   Supported flags:
 *     - CX_ECSCHNORR_XY
 *
 * @param [in] hashID
 *  Hash identifier used to compute the input data.
 *
 * @param [in] msg
 *   Signed input data to verify the signature.
 *
 * @param [in] msg_len
 *   Length of input to data.
 *
 * @param [in] sig
 *   ECDSA signature to verify encoded as TLV:  30 L 02 Lr r 02 Ls s
 *
 * @return
 *   1 if signature is verified
 *   0 is signature is not verified
 *
 * @throws INVALID_PARAMETER
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
