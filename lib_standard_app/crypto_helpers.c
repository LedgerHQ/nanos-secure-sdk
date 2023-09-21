/*****************************************************************************
 *   Ledger SDK.
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
 *****************************************************************************/
#include <stdint.h>   // uint*_t
#include <string.h>   // memset, explicit_bzero
#include <stdbool.h>  // bool

#include "cx.h"
#include "os.h"

WARN_UNUSED_RESULT cx_err_t
bip32_derive_with_seed_init_privkey_256(unsigned int               derivation_mode,
                                        cx_curve_t                 curve,
                                        const uint32_t            *path,
                                        size_t                     path_len,
                                        cx_ecfp_256_private_key_t *privkey,
                                        uint8_t                   *chain_code,
                                        unsigned char             *seed,
                                        size_t                     seed_len)
{
    cx_err_t error = CX_OK;
    uint8_t  raw_privkey[64];  // Allocate 64 bytes to respect Syscall API but only 32 will be used
    size_t   length;

    // Check curve key length
    CX_CHECK(cx_ecdomain_parameters_length(curve, &length));
    if (length != 32) {
        error = CX_EC_INVALID_CURVE;
        goto end;
    }

    // Derive private key according to BIP32 path
    CX_CHECK(os_derive_bip32_with_seed_no_throw(
        derivation_mode, curve, path, path_len, raw_privkey, chain_code, seed, seed_len));

    // Init privkey from raw
    CX_CHECK(cx_ecfp_init_private_key_no_throw(curve, raw_privkey, length, privkey));

end:
    explicit_bzero(raw_privkey, sizeof(raw_privkey));

    if (error != CX_OK) {
        // Make sure the caller doesn't use uninitialized data in case
        // the return code is not checked.
        explicit_bzero(privkey, sizeof(cx_ecfp_256_private_key_t));
    }
    return error;
}

WARN_UNUSED_RESULT cx_err_t bip32_derive_with_seed_get_pubkey_256(unsigned int    derivation_mode,
                                                                  cx_curve_t      curve,
                                                                  const uint32_t *path,
                                                                  size_t          path_len,
                                                                  uint8_t  raw_pubkey[static 65],
                                                                  uint8_t *chain_code,
                                                                  cx_md_t  hashID,
                                                                  unsigned char *seed,
                                                                  size_t         seed_len)
{
    cx_err_t error = CX_OK;

    cx_ecfp_256_private_key_t privkey;
    cx_ecfp_256_public_key_t  pubkey;

    // Derive private key according to BIP32 path
    CX_CHECK(bip32_derive_with_seed_init_privkey_256(
        derivation_mode, curve, path, path_len, &privkey, chain_code, seed, seed_len));

    // Generate associated pubkey
    CX_CHECK(cx_ecfp_generate_pair2_no_throw(curve, &pubkey, &privkey, true, hashID));

    // Check pubkey length then copy it to raw_pubkey
    if (pubkey.W_len != 65) {
        error = CX_EC_INVALID_CURVE;
        goto end;
    }
    memmove(raw_pubkey, pubkey.W, pubkey.W_len);

end:
    explicit_bzero(&privkey, sizeof(privkey));

    if (error != CX_OK) {
        // Make sure the caller doesn't use uninitialized data in case
        // the return code is not checked.
        explicit_bzero(raw_pubkey, 65);
    }
    return error;
}

WARN_UNUSED_RESULT cx_err_t bip32_derive_with_seed_ecdsa_sign_hash_256(unsigned int derivation_mode,
                                                                       cx_curve_t   curve,
                                                                       const uint32_t *path,
                                                                       size_t          path_len,
                                                                       uint32_t        sign_mode,
                                                                       cx_md_t         hashID,
                                                                       const uint8_t  *hash,
                                                                       size_t          hash_len,
                                                                       uint8_t        *sig,
                                                                       size_t         *sig_len,
                                                                       uint32_t       *info,
                                                                       unsigned char  *seed,
                                                                       size_t          seed_len)
{
    cx_err_t                  error = CX_OK;
    cx_ecfp_256_private_key_t privkey;
    size_t                    buf_len = *sig_len;

    // Derive private key according to BIP32 path
    CX_CHECK(bip32_derive_with_seed_init_privkey_256(
        derivation_mode, curve, path, path_len, &privkey, NULL, seed, seed_len));

    CX_CHECK(
        cx_ecdsa_sign_no_throw(&privkey, sign_mode, hashID, hash, hash_len, sig, sig_len, info));

end:
    explicit_bzero(&privkey, sizeof(privkey));

    if (error != CX_OK) {
        // Make sure the caller doesn't use uninitialized data in case
        // the return code is not checked.
        explicit_bzero(sig, buf_len);
    }
    return error;
}

WARN_UNUSED_RESULT cx_err_t
bip32_derive_with_seed_ecdsa_sign_rs_hash_256(unsigned int    derivation_mode,
                                              cx_curve_t      curve,
                                              const uint32_t *path,
                                              size_t          path_len,
                                              uint32_t        sign_mode,
                                              cx_md_t         hashID,
                                              const uint8_t  *hash,
                                              size_t          hash_len,
                                              uint8_t         sig_r[static 32],
                                              uint8_t         sig_s[static 32],
                                              uint32_t       *info,
                                              unsigned char  *seed,
                                              size_t          seed_len)
{
    cx_err_t                  error = CX_OK;
    cx_ecfp_256_private_key_t privkey;

    // Derive private key according to BIP32 path
    CX_CHECK(bip32_derive_with_seed_init_privkey_256(
        derivation_mode, curve, path, path_len, &privkey, NULL, seed, seed_len));

    CX_CHECK(cx_ecdsa_sign_rs_no_throw(
        &privkey, sign_mode, hashID, hash, hash_len, 32, sig_r, sig_s, info));

end:
    explicit_bzero(&privkey, sizeof(privkey));

    return error;
}

WARN_UNUSED_RESULT cx_err_t bip32_derive_with_seed_eddsa_sign_hash_256(unsigned int derivation_mode,
                                                                       cx_curve_t   curve,
                                                                       const uint32_t *path,
                                                                       size_t          path_len,
                                                                       cx_md_t         hashID,
                                                                       const uint8_t  *hash,
                                                                       size_t          hash_len,
                                                                       uint8_t        *sig,
                                                                       size_t         *sig_len,
                                                                       unsigned char  *seed,
                                                                       size_t          seed_len)
{
    cx_err_t                  error = CX_OK;
    cx_ecfp_256_private_key_t privkey;
    size_t                    size;
    size_t                    buf_len = *sig_len;

    if (sig_len == NULL) {
        error = CX_INVALID_PARAMETER_VALUE;
        goto end;
    }

    // Derive private key according to BIP32 path
    CX_CHECK(bip32_derive_with_seed_init_privkey_256(
        derivation_mode, curve, path, path_len, &privkey, NULL, seed, seed_len));

    CX_CHECK(cx_eddsa_sign_no_throw(&privkey, hashID, hash, hash_len, sig, *sig_len));

    CX_CHECK(cx_ecdomain_parameters_length(curve, &size));
    *sig_len = size * 2;

end:
    explicit_bzero(&privkey, sizeof(privkey));

    if (error != CX_OK) {
        // Make sure the caller doesn't use uninitialized data in case
        // the return code is not checked.
        explicit_bzero(sig, buf_len);
    }
    return error;
}
