
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

#ifdef HAVE_EDDSA

#include "lcx_ecfp.h"
#include "lcx_wrappers.h"

#ifndef LCX_EDDSA_H
#define LCX_EDDSA_H
/**
 * Sign a hash message according to EdDSA specification RFC8032.
 *
 * @param [in] pv_key
 *   A private ecfp key fully inited with 'cx_ecfp_init_private_key'.
 *
 *
 * @param [in] mode
 *   Crypto mode flags. See above.
 *   Supported flags:
 *      <none>
 *
 * @param [in] hashID
 *  Hash identifier used to compute the input data. SHA512, SHA3 and Keccak are supported.
 *
 * @param [in] hash
 *   Input data to sign.
 *   The data should be the hash of the original message.
 *   The data length must be lesser than the curve size.
 *
 * @param [in] hash_len
 *   Length of input to data.
 *
 * @param [in] ctx
 *   UNUSED, SHALL BE NULL
 *
 * @param [in] ctx_len
 *   UNUSED, SHALL BE ZERO
 *
 * @param [out] sig
 *   EdDSA signature encoded as : R|S
 *
 * @param [out] info
 *   Set to zero
 *
 * @return
 *   Full length of signature
 *
 * @throws INVALID_PARAMETER
 */
cx_err_t cx_eddsa_sign_no_throw(const cx_ecfp_private_key_t *pvkey,
                       cx_md_t                      hashID,
                       const uint8_t *              hash,
                       size_t                       hash_len,
                       uint8_t *                    sig,
                       size_t                       sig_len);

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
 * Verify a hash message signature according to EDDSA specification RFC8032.
 *
 * @param [in] key
 *   A public ecfp key fully inited with 'cx_ecfp_init_public_key'
 *
 * @param [in] mode
 *   Crypto mode flags. See above.
 *   Supported flags:
 *     - <none>
 *
 * @param [in] hashID
 *  Hash identifier used to compute the input data.  SHA512, SHA3 and Keccak are supported.
 *
 * @param [in] hash
 *   Signed input data to verify the signature.
 *   The data should be the hash of the original message.
 *   The data length must be lesser than the curve size.
 *
 * @param [in] hash_len
 *   Length of input to data.
 *
 * @param [in] ctx
 *   UNUSED, SHALL BE NULL
 *
 * @param [in] ctx_len
 *   UNUSED, SHALL BE ZERO
 *
 * @param [in] sig
 *   EDDSA signature to verify encoded as : R|S
 *
 * @param [in] sig_len
 *   sig length in bytes
 *
 * @return
 *   1 if signature is verified
 *   0 is signarure is not verified
 *
 * @throws INVALID_PARAMETER
 */
bool cx_eddsa_verify_no_throw(const cx_ecfp_public_key_t *pukey,
                     cx_md_t                     hashID,
                     const uint8_t *             hash,
                     size_t                      hash_len,
                     const uint8_t *             sig,
                     size_t                      sig_len);

static inline int cx_eddsa_verify ( const cx_ecfp_public_key_t * pukey, int mode, cx_md_t hashID, const unsigned char * hash, unsigned int hash_len, const unsigned char * ctx, unsigned int ctx_len, const unsigned char * sig, unsigned int sig_len )
{
  UNUSED(mode);
  UNUSED(ctx);
  UNUSED(ctx_len);

  return cx_eddsa_verify_no_throw(pukey, hashID, hash, hash_len, sig, sig_len);
}


/**
 * Encode coordinates
 *
 * @param [in] coord
 *   A pointer to the ec point coordinates in the form x|y
 *
 * @param [in] len
 *   Length of the input coordinates
 *
 * @param [in] sign
 *  Sign of the coordinates
 *
 */
  void cx_encode_coord(uint8_t * coord,
                            int len,
                            int sign);

/**
 * Decode coordinates.
 *
 * @param [in] coord
 *   A pointer to the ec point coordinates in the form x|y
 *
 * @param [in] len
 *   Length of the input coordinates
 *
 * @return [in] sign
 *  Sign of the coordinates
 *
 */
  int cx_decode_coord(uint8_t * coord,
                        int len);

#endif

#endif // HAVE_EDDSA
