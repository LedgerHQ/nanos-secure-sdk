
/*******************************************************************************
*   Ledger Nano S - Secure firmware
*   (c) 2022 Ledger
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

#ifdef HAVE_EDDSA

#ifndef CX_EDDSA_H
#define CX_EDDSA_H

cx_err_t cx_eddsa_get_public_key_internal(const cx_ecfp_private_key_t *pv_key,
                                          cx_md_t                      hashID,
                                          cx_ecfp_public_key_t *       pu_key,
                                          uint8_t *                    a,
                                          size_t                       a_len,
                                          uint8_t *                    h,
                                          size_t                       h_len,
                                          uint8_t *                    scal /*temp uint8[114]*/);

#endif

#endif
