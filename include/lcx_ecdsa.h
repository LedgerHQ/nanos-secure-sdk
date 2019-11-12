
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

#ifndef LCX_ECDSA_H
#define LCX_ECDSA_H
/** @internal backward compatibility */
#define cx_ecdsa_init_public_key cx_ecfp_init_public_key
/** @internal backward compatibility */
#define cx_ecdsa_init_private_key cx_ecfp_init_private_key

/**
 * Sign a hash message according to ECDSA specification.
 *
 * @param [in] pvkey
 *   A private ecfp key fully inited with 'cx_ecfp_init_private_key'
 *
 * @param [in] mode
 *   Crypto mode flags. See above.
 *   Supported flags:
 *     - CX_RND_TRNG
 *     - CX_RND_RFC6979
 *
 * @param [in] hashID
 *  Hash identifier used to compute the input data.
 *  This parameter is mandatory for rng of type CX_RND_RFC6979.
 *
 * @param [in] hash
 *   Input data to sign.
 *   The data should be the hash of the original message.
 *   The data length must be lesser than the curve size.
 *
 * @param [in] hash_len
 *   Length of input to data.
 *
 * @param [out] sig
 *   ECDSA signature encoded as TLV:  30 L 02 Lr r 02 Ls s
 *
 * @param [out] info
 *   Set CX_ECCINFO_PARITY_ODD if Y is odd when computing k.G
 *
 * @return
 *   Full length of signature
 *
 * @throws INVALID_PARAMETER
 */
CXCALL int cx_ecdsa_sign(const cx_ecfp_private_key_t WIDE *pvkey PLENGTH(
                             scc__cx_scc_struct_size_ecfp_privkey__pvkey),
                         int mode, cx_md_t hashID,
                         const unsigned char WIDE *hash PLENGTH(hash_len),
                         unsigned int hash_len,
                         unsigned char *sig PLENGTH(sig_len),
                         unsigned int sig_len,
                         unsigned int *info PLENGTH(sizeof(unsigned int)));

/**
 * Verify a hash message signature according to ECDSA specification.
 *
 * @param [in] key
 *   A public ecfp key fully inited with 'cx_ecfp_init_public_key'
 *
 * @param [in] mode
 *   Crypto mode flags. See above.
 *   Supported flags:
 *     - CX_LAST
 *
 * @param [in] hashID
 *  Hash identifier used to compute the input data.
 *
 * @param [in] hash
 *   Signed input data to verify the signature.
 *   The data should be the hash of the original message.
 *   The data length must be lesser than the curve size.
 *
 * @param [in] hash_len
 *   Length of input to data.
 *
 * @param [in] sig
 *   ECDSA signature to verify encoded as TLV:  30 L 02 Lr r 02 Ls s
 *
 * @return
 *   1 if signature is verified
 *   0 is signarure is not verified
 *
 * @throws INVALID_PARAMETER
 */
CXCALL int cx_ecdsa_verify(const cx_ecfp_public_key_t WIDE *pukey PLENGTH(
                               scc__cx_scc_struct_size_ecfp_pubkey__pukey),
                           int mode, cx_md_t hashID,
                           const unsigned char WIDE *hash PLENGTH(hash_len),
                           unsigned int hash_len,
                           const unsigned char WIDE *sig PLENGTH(sig_len),
                           unsigned int sig_len);

#endif
