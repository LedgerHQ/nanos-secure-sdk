
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

#ifndef CX_COMPLIANCE_141_H
#define CX_COMPLIANCE_141_H

#ifdef CX_COMPLIANCE_141
#ifndef SYSCALL_STUB

int cx_rng_rfc6979_X(unsigned char *rnd, unsigned int hashID, unsigned char *h1,
                     unsigned char *x, unsigned int x_len, unsigned char *q,
                     unsigned int q_len, unsigned char *V, unsigned int V_len);

int cx_hash_X(cx_hash_t *hash, int mode, const unsigned char WIDE *in,
              unsigned int len, unsigned char *out);
int cx_hash_sha256_X(const unsigned char WIDE *in, unsigned int len,
                     unsigned char *out);
int cx_hash_sha512_X(const unsigned char WIDE *in, unsigned int len,
                     unsigned char *out);

int cx_hmac_sha256_X(const unsigned char *key, unsigned int klen,
                     const unsigned char WIDE *in, unsigned int len,
                     unsigned char *out);
int cx_hmac_sha512_X(const unsigned char *key, unsigned int klen,
                     const unsigned char WIDE *in, unsigned int len,
                     unsigned char *out);
int cx_hmac_X(cx_hmac_t *hmac, int mode, const unsigned char WIDE *in,
              unsigned int len, unsigned char *mac);

int cx_des_init_key_X(const unsigned char WIDE *rawkey, unsigned int key_len,
                      cx_des_key_t *key);
int cx_des_iv_X(cx_des_key_t WIDE *key, int mode, const unsigned char WIDE *iv,
                const unsigned char WIDE *in, unsigned int len,
                unsigned char *out);
int cx_des_X(const cx_des_key_t WIDE *key, int mode,
             const unsigned char WIDE *in, unsigned int len,
             unsigned char *out);

int cx_aes_init_key_X(const unsigned char WIDE *rawkey, unsigned int key_len,
                      cx_des_key_t *key);
int cx_aes_iv_X(const cx_des_key_t WIDE *key, int mode,
                const unsigned char WIDE *iv, const unsigned char WIDE *in,
                unsigned int len, unsigned char *out);
int cx_aes_X(const cx_aes_key_t WIDE *key, int mode,
             const unsigned char WIDE *in, unsigned int len,
             unsigned char *out);

int cx_rsa_init_public_key_X(const unsigned char WIDE *exponent,
                             const unsigned char WIDE *modulus,
                             unsigned int modulus_len,
                             cx_rsa_public_key_t *key);
int cx_rsa_init_private_key_X(const unsigned char WIDE *exponent,
                              const unsigned char WIDE *modulus,
                              unsigned int modulus_len,
                              cx_rsa_private_key_t *key);
int cx_rsa_generate_pair_X(unsigned int modulus_len,
                           cx_rsa_public_key_t *public_key,
                           cx_rsa_private_key_t *private_key,
                           unsigned long int pub_exponent,
                           const unsigned char *externalPQ);

int cx_ecfp_is_valid_point_X(cx_curve_t curve, const unsigned char WIDE *point);
int cx_ecfp_is_cryptographic_point_X(cx_curve_t curve,
                                     const unsigned char WIDE *point);

int cx_ecfp_add_point_X(cx_curve_t curve, unsigned char *R,
                        const unsigned char WIDE *P,
                        const unsigned char WIDE *Q);
int cx_ecfp_scalar_mult_X(cx_curve_t curve, unsigned char *P,
                          const unsigned char WIDE *k, unsigned int k_len);

int cx_ecschnorr_sign_X(const cx_ecfp_private_key_t WIDE *pvkey, int mode,
                        cx_md_t hashID, const unsigned char *msg,
                        unsigned int msg_len, unsigned char *sig,
                        unsigned int *info);

void cx_edward_decompress_point_X(cx_curve_t curve, unsigned char *P);
void cx_edward_compress_point_X(cx_curve_t curve, unsigned char *P);

void cx_eddsa_get_public_key_X(const cx_ecfp_private_key_t WIDE *pvkey,
                               cx_md_t hashID, cx_ecfp_public_key_t *pukey,
                               unsigned char *a, unsigned char *h);
int cx_eddsa_sign_X(const cx_ecfp_private_key_t WIDE *pvkey, int mode,
                    cx_md_t hashID, const unsigned char WIDE *hash,
                    unsigned int hash_len, const unsigned char WIDE *ctx,
                    unsigned int ctx_len, unsigned char *sig,
                    unsigned int *info);
int cx_ecdsa_sign_X(const cx_ecfp_private_key_t WIDE *pvkey, int mode,
                    cx_md_t hashID, const unsigned char WIDE *hash,
                    unsigned int hash_len, unsigned char *sig,
                    unsigned int *info);

int cx_ecdh_X(const cx_ecfp_private_key_t WIDE *key, int mode,
              const unsigned char WIDE *P, unsigned char *secret);

#define cx_rng_rfc6979(rnd, hashID, h1, x, x_len, q, q_len, V, V_len)          \
  cx_rng_rfc6979_X(rnd, hashID, h1, x, x_len, q, q_len, V, V_len)

#define cx_hash(hash, mode, in, len, out) cx_hash_X(hash, mode, in, len, out)

#define cx_hash_sha256(in, len, out) cx_hash_sha256_X(in, len, out)
#define cx_hash_sha512(in, len, out) cx_hash_sha512_X(in, len, out)

#define cx_hmac(hmac, mode, in, len, mac) cx_hmac_X(hmac, mode, in, len, mac)
#define cx_hmac_sha256(key, klen, in, len, out)                                \
  cx_hmac_sha256_X(key, klen, in, len, out)
#define cx_hmac_sha512(key, klen, in, len, out)                                \
  cx_hmac_sha512_X(key, klen, in, len, out)

#define cx_des_iv(key, mode, iv, in, len, out)                                 \
  cx_des_iv_X(key, mode, iv, in, len, out)
#define cx_des(key, mode, in, len, out) cx_des_X(key, mode, in, len, out)

#define cx_aes_iv(key, mode, iv, in, len, out)                                 \
  cx_aes_iv_X(key, mode, iv, in, len, out)
#define cx_aes(key, mode, in, len, out) cx_aes_X(key, mode, in, len, out)

#define cx_rsa_init_public_key(exponent, modulus, modulus_len, key)            \
  cx_rsa_init_public_key_X(exponent, modulus, modulus_len, key)
#define cx_rsa_init_private_key(exponent, modulus, modulus_len, key)           \
  cx_rsa_init_private_key_X(exponent, modulus, modulus_len, key)
#define cx_rsa_generate_pair(modulus_len, public_key, private_key,             \
                             pub_exponent, externalPQ)                         \
  cx_rsa_generate_pair_X(modulus_len, public_key, private_key, pub_exponent,   \
                         externalPQ)

#define cx_ecfp_is_valid_point(curve, point)                                   \
  cx_ecfp_is_valid_point_X(curve, point)
#define cx_ecfp_is_cryptographic_point(curve, point)                           \
  cx_ecfp_is_cryptographic_point_X(curve, point)

#define cx_ecfp_add_point(curve, R, P, Q) cx_ecfp_add_point_X(curve, R, P, Q)
#define cx_ecfp_scalar_mult(curve, P, k, k_len)                                \
  cx_ecfp_scalar_mult_X(curve, P, k, k_len)

#define cx_ecschnorr_sign(pvkey, mode, hashID, msg, msg_len, sig, info)        \
  cx_ecschnorr_sign_X(pvkey, mode, hashID, msg, msg_len, sig, info)

#define cx_edward_decompress_point(curve, P)                                   \
  cx_edward_decompress_point_X(curve, P)
#define cx_edward_compress_point(curve, P) cx_edward_compress_point_X(curve, P)

#define cx_eddsa_get_public_key(pvkey, hashID, pukey, a, h)                    \
  cx_eddsa_get_public_key_X(pvkey, hashID, pukey, a, h)
#define cx_eddsa_sign(pvkey, mode, hashID, hash, hash_len, ctx, ctx_len, sig,  \
                      info)                                                    \
  cx_eddsa_sign_X(pvkey, mode, hashID, hash, hash_len, ctx, ctx_len, sig, info)

#define cx_ecdsa_sign(pvkey, mode, hashID, hash, hash_len, sig, info)          \
  cx_ecdsa_sign_X(pvkey, mode, hashID, hash, hash_len, sig, info)

#define cx_ecdh(key, mode, P, secret) cx_ecdh_X(key, mode, P, secret)

#endif // SYSCALL_STUB
#endif // CX_COMPLIANCE_141

#endif // CX_COMPLIANCE_141_H
