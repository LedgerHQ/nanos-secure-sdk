
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

#ifdef HAVE_RSA

#ifndef CX_RSA_H
#define CX_RSA_H

#include "lcx_rsa.h"
#include "lcx_hash.h"

/*
 * @param  [in]  hID       Hash identifier
 *
 * @return Output hash length in bytes.
 */
size_t cx_pkcs1_get_hash_len(cx_md_t hID);

/*
 * @param  [in]  hID       underlaid hash (ignored, sha256 forced)
 * @param  [out] em        encoded output
 * @param  [in]  em_len    requested output length
 * @param  [in]  mHash     hash to encode
 * @param  [in]  mHashLen  hash length
 *
 * @return em_len, if encoded 0 else
 */
cx_err_t cx_pkcs1_emsa_v1o5_encode(cx_md_t hID, uint8_t *em, size_t em_len, const uint8_t *mHash, size_t mHashLen);
/*
 * @param  [in]  hID       underlaid hash (ignored, sha256 forced)
 * @param  [in]  em        encoded message to verify
 * @param  [in]  em_len    requested output length
 * @param  [in]  mHash     hash to verify
 * @param  [in]  mHashLen  hash length
 *
 * @return 1 if verifed, 0 else
 */
bool cx_pkcs1_emsa_v1o5_verify(cx_md_t hID, uint8_t *em, size_t em_len, const uint8_t *mHash, size_t mHashLen);

/* The salt length is equal to the mHashLen value.
 * If another salt length is to be used, the
 * cx_pkcs1_emsa_pss_encode_with_salt_len function must
 * be used instead.
 * @param  [in]  hID       underlaid hash (ignored, sha256 forced)
 * @param  [out] em        encoded output
 * @param  [in]  em_len    requested output length
 * @param  [in]  mHash     hash to encode
 * @param  [in]  mHashLen  hash length
 * @param  [in]  mSaltLen  Salt length
 *
 * @return em_len, if encoded 0 else
 */
cx_err_t cx_pkcs1_emsa_pss_encode(cx_md_t hID, uint8_t *em, size_t em_len, const uint8_t *mHash, size_t mHashLen, size_t *size);

/*
 * @param  [in]  hID       underlaid hash (ignored, sha256 forced)
 * @param  [out] em        encoded output
 * @param  [in]  em_len    requested output length
 * @param  [in]  mHash     hash to encode
 * @param  [in]  mHashLen  hash length
 * @param  [in]  mSaltLen  Salt length
 *
 * @return em_len, if encoded 0 else
 */
cx_err_t cx_pkcs1_emsa_pss_encode_with_salt_len(cx_md_t hID, uint8_t *em, size_t em_len, const uint8_t *mHash, size_t mHashLen, size_t mSaltLen, size_t *size);

/* The salt length is equal to the mHashLen value.
 * If another salt length is to be used, the
 * cx_pkcs1_emsa_pss_verify_with_salt_len function must
 * be used instead.
 * @param  [in]  hID       underlaid hash
 * @param  [in]  em        encoded message to verify
 * @param  [in]  em_len    requested output length
 * @param  [in]  mHash     hash to verify
 * @param  [in]  mHashLen  hash length
 *
 * @return 1 if verifed, 0 else
 */
bool cx_pkcs1_emsa_pss_verify(cx_md_t hID, uint8_t *em, size_t em_len, const uint8_t *mHash, size_t mHashLen);

/*
 * @param  [in]  hID       underlaid hash
 * @param  [in]  em        encoded message to verify
 * @param  [in]  em_len    requested output length
 * @param  [in]  mHash     hash to verify
 * @param  [in]  mHashLen  hash length
 * @param  [in]  mSaltLen  salt length
 *
 * @return 1 if verifed, 0 else
 */
bool cx_pkcs1_emsa_pss_verify_with_salt_len(cx_md_t hID, uint8_t *em, size_t em_len, const uint8_t *mHash, size_t mHashLen, size_t mSaltLen);

/*
 * @param  [in]  hID       underlaid hash (ignored, sha256 forced)
 * @param  [out] em        encoded output
 * @param  [in]  em_len    requested output length
 * @param  [in]  m         message to encode
 * @param  [in]  mLen      message length
 *
 * @return em_len, if encoded 0 else
 */
cx_err_t cx_pkcs1_eme_v1o5_encode(cx_md_t hID, uint8_t *em, size_t em_len, const uint8_t *mHash, size_t mHashLen);
/*
 * @param  [in]  hID       underlaid hash (ignored, sha256 forced)
 * @param  [in]  em        encoded message
 * @param  [in]  em_len    requested output length
 * @param  [out]  m        message to encode
 * @param  [in]  mLen      max message length acceptable in m
 *
 * @return decoded message length, -1 if decoding fail
 */
size_t cx_pkcs1_eme_v1o5_decode(cx_md_t hID, uint8_t *em, size_t em_len, uint8_t *mHash, size_t mHashLen);

/*
 * @param  [in]  hID       underlaid hash (ignored, sha256 forced)
 * @param  [out] em        encoded output
 * @param  [in]  em_len    requested output length
 * @param  [in]  m         message to encode
 * @param  [in]  mLen      message length
 *
 * @return em_len, if encoded 0 else
 */
cx_err_t cx_pkcs1_eme_oaep_encode(cx_md_t hID, uint8_t *em, size_t em_len, const uint8_t *m, size_t mLen);
/*
 * @param  [in]  hID       underlaid hash (ignored, sha256 forced)
 * @param  [in]  em        encoded message
 * @param  [in]  em_len    requested output length
 * @param  [out]  m        message to encode
 * @param  [in]  mLen      max message length acceptable in m
 *
 * @return decoded message length, -1 if decoding fail
 */
cx_err_t cx_pkcs1_eme_oaep_decode(cx_md_t hID, uint8_t *em, size_t em_len, uint8_t *m, size_t *mLen);

// For PKCS1.5
#define PKCS1_DIGEST_BUFFER_LENGTH              64

struct cx_pkcs1_s {
    union {
        cx_hash_t   hash;
#if defined(HAVE_SHA256)
        cx_sha256_t sha256;
#endif // HAVE_SHA256

#if defined(HAVE_SHA512)
        cx_sha512_t sha512;
#endif // HAVE_SHA512
    } hash_ctx;
    uint8_t       digest[PKCS1_DIGEST_BUFFER_LENGTH];
    uint8_t       MGF1[512];
};
typedef  struct cx_pkcs1_s cx_pkcs1_t;

#if defined(HAVE_SHA224)
#define CX_OID_SHA224_LENGTH    19
extern uint8_t const C_cx_oid_sha224[CX_OID_SHA224_LENGTH];
#endif // HAVE_SHA224

#if defined(HAVE_SHA256)
#define CX_OID_SHA256_LENGTH    19
extern uint8_t const C_cx_oid_sha256[CX_OID_SHA256_LENGTH];
#endif // HAVE_SHA256

#if defined(HAVE_SHA384)
#define CX_OID_SHA384_LENGTH    19
extern uint8_t const C_cx_oid_sha384[CX_OID_SHA384_LENGTH];
#endif // HAVE_SHA384

#if defined(HAVE_SHA512)
#define CX_OID_SHA512_LENGTH    19
extern uint8_t const C_cx_oid_sha512[CX_OID_SHA512_LENGTH];
#endif // HAVE_SHA512

#if defined(HAVE_SHA3)
#define CX_OID_SHA3_256_LENGTH  19
extern uint8_t const C_cx_oid_sha3_256[CX_OID_SHA3_256_LENGTH];

#define CX_OID_SHA3_512_LENGTH  19
extern uint8_t const C_cx_oid_sha3_512[CX_OID_SHA3_512_LENGTH];
#endif // HAVE_SHA3

cx_err_t cx_rsa_get_public_components(const cx_rsa_public_key_t *key, uint8_t **e, uint8_t **n);
cx_err_t cx_rsa_get_private_components(const cx_rsa_private_key_t *key, uint8_t **d, uint8_t **n);

cx_err_t cx_rsa_private_key_ctx_size(const cx_rsa_private_key_t *key, size_t *size);

#endif // CX_RSA_H
#endif // HAVE_RSA
