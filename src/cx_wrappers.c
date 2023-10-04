
/*******************************************************************************
 *   (c) 2023 Ledger SAS.
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
#include <stdint.h>   // uint*_t
#include <string.h>   // memset, explicit_bzero
#include <stdbool.h>  // bool

#include "cx.h"
#include "lib_cxng/src/cx_ecfp.h"

// At some point this could/should be migrated to a cx func included in the cx_lib
cx_err_t cx_ecdsa_sign_rs_no_throw(const cx_ecfp_private_key_t *key,
                                   uint32_t                     mode,
                                   cx_md_t                      hashID,
                                   const uint8_t               *hash,
                                   size_t                       hash_len,
                                   size_t                       rs_size,
                                   uint8_t                     *sig_r,
                                   uint8_t                     *sig_s,
                                   uint32_t                    *info)
{
    cx_err_t       error = CX_OK;
    uint8_t        sig_der[100];
    size_t         sig_der_len = sizeof(sig_der);
    const uint8_t *r;
    const uint8_t *s;
    size_t         r_len, s_len;

    explicit_bzero(sig_r, rs_size);
    explicit_bzero(sig_s, rs_size);

    CX_CHECK(
        cx_ecdsa_sign_no_throw(key, mode, hashID, hash, hash_len, sig_der, &sig_der_len, info));

    if (cx_ecfp_decode_sig_der(sig_der, sig_der_len, rs_size, &r, &r_len, &s, &s_len) != 1) {
        error = CX_INVALID_PARAMETER;
        goto end;
    }

    // Copy R and S. No need to pad as the full buffer has been memset
    memcpy(sig_r + rs_size - r_len, r, r_len);
    memcpy(sig_s + rs_size - s_len, s, s_len);

end:
    if (error != CX_OK) {
        *info = 0;
    }
    return error;
}
