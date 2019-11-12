
/*******************************************************************************
*   Ledger Nano S - Secure firmware
*   (c) 2019 Ledger
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

#ifndef LCX_EDDSA_H
#define LCX_EDDSA_H

/**
 *  Compress point according to RFC8032.
 *
 * @param [in]     domain
 * @param [in,out] P
 */
CXCALL void cx_edward_compress_point(cx_curve_t curve,
                                     unsigned char *P PLENGTH(P_len),
                                     unsigned int P_len);

/**
 *  Decompress point according to draft-irtf-cfrg-eddsa-05.
 *
 * @param [in]     domain
 * @param [in,out] P
 */
CXCALL void cx_edward_decompress_point(cx_curve_t curve,
                                       unsigned char *P PLENGTH(P_len),
                                       unsigned int P_len);

/**
 *  Retrieve (a,h) = (Kr, Kl), such (Kr, Kl) = Hash(pv_key) as specified in
 * RFC8032
 *
 * @param [in] pv_key
 *   A private ecfp key fully inited with 'cx_ecfp_init_private_key'.
 *
 * @param [in] hashID
 *  Hash identifier used to compute the input data. SHA512, SHA3 and Keccak are
 * supported.
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
 *  Hash identifier used to compute the input data. SHA512, SHA3 and Keccak are
 * supported.
 *
 */
CXCALL void cx_eddsa_get_public_key(
    const cx_ecfp_private_key_t WIDE *pvkey
        PLENGTH(scc__cx_scc_struct_size_ecfp_privkey__pvkey),
    cx_md_t hashID,
    cx_ecfp_public_key_t *pukey
        PLENGTH(scc__cx_scc_struct_size_ecfp_pubkey_from_pvkey__pvkey),
    unsigned char *a PLENGTH(a_len), unsigned int a_len,
    unsigned char *h PLENGTH(h_len), unsigned int h_len);

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
 *  Hash identifier used to compute the input data. SHA512, SHA3 and Keccak are
 * supported.
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
CXCALL int cx_eddsa_sign(const cx_ecfp_private_key_t WIDE *pvkey PLENGTH(
                             scc__cx_scc_struct_size_ecfp_privkey__pvkey),
                         int mode, cx_md_t hashID,
                         const unsigned char WIDE *hash PLENGTH(hash_len),
                         unsigned int hash_len,
                         const unsigned char WIDE *ctx PLENGTH(ctx_len),
                         unsigned int ctx_len,
                         unsigned char *sig PLENGTH(sig_len),
                         unsigned int sig_len, unsigned int *info);

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
 *  Hash identifier used to compute the input data.  SHA512, SHA3 and Keccak are
 * supported.
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
CXCALL int cx_eddsa_verify(const cx_ecfp_public_key_t WIDE *pukey PLENGTH(
                               scc__cx_scc_struct_size_ecfp_pubkey__pukey),
                           int mode, cx_md_t hashID,
                           const unsigned char WIDE *hash PLENGTH(hash_len),
                           unsigned int hash_len,
                           const unsigned char WIDE *ctx PLENGTH(ctx_len),
                           unsigned int ctx_len,
                           const unsigned char WIDE *sig PLENGTH(sig_len),
                           unsigned int sig_len);

#endif
