
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

#ifndef LCX_ECDH_H
#define LCX_ECDH_H

/**
 * Compute a shared secret according to ECDH specifiaction
 * Depending on the mode, the shared secret is either the full point or
 * only the x coordinate
 *
 * @param [in] key
 *   A private ecfp key fully inited with 'cx_ecfp_init_private_key'
 *
 * @param [in] mode
 *   Crypto mode flags. See above.
 *   Supported flags:
 *     - CX_ECDH_POINT
 *     - CX_ECDH_X
 *
 * @param [in] P
 *   Other party public point encoded as: 04 x y, where x and y are
 *   encoded as big endian raw value and have bits length equals to
 *   the curve size.
 *
 * @param [out] secret
 *   Generated shared secret.
 *
 *
 * @return size of secret
 *
 * @throws INVALID_PARAMETER
 */
CXCALL int cx_ecdh(const cx_ecfp_private_key_t WIDE *pvkey
                       PLENGTH(scc__cx_scc_struct_size_ecfp_privkey__pvkey),
                   int mode, const unsigned char WIDE *P PLENGTH(P_len),
                   unsigned int P_len,
                   unsigned char *secret PLENGTH(secret_len),
                   unsigned int secret_len);

#endif
