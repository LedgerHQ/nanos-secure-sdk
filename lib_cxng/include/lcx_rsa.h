
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

/**
 * @file    lcx_rsa.h
 * @brief   RSA algorithm
 *
 * RSA is a public key cryptosystem that can be used for encryption and signature schemes.
 */

#ifdef HAVE_RSA

#ifndef LCX_RSA_H
#define LCX_RSA_H

#include "lcx_wrappers.h"
#include "lcx_hash.h"
#include "lcx_sha256.h"
#include "lcx_sha512.h"
#include <stddef.h>
#include <stdint.h>

/** 
 * @brief   Abstract RSA public key.
 *
 * @details This type shall not be instantiate, 
 *          it is only defined to allow unified API
 *           for RSA operations.
 */
struct cx_rsa_public_key_s {
  size_t size;   ///< Key size in bytes
  uint8_t e[4];  ///< 32-bit public exponent
  uint8_t n[1];  ///< Public modulus
};

/**
 * @brief Abstract RSA private key.
 */
struct cx_rsa_private_key_s {
  size_t size;   ///< Key size in bytes
  uint8_t d[1];  ///< Private exponent
  uint8_t n[1];  ///< Public modulus
};
/** Convenience type. See #cx_rsa_public_key_s. */
typedef struct cx_rsa_public_key_s cx_rsa_public_key_t;
/** Convenience type. See #cx_rsa_private_key_s. */
typedef struct cx_rsa_private_key_s cx_rsa_private_key_t;

/** 1024-bit RSA public key */
struct cx_rsa_1024_public_key_s {
  size_t size;    ///< @copydoc cx_rsa_public_key_s::size
  uint8_t e[4];   ///< @copydoc cx_rsa_public_key_s::e
  uint8_t n[128]; ///< @copydoc cx_rsa_public_key_s::n
};
/** 1024-bit RSA private key */
struct cx_rsa_1024_private_key_s {
  size_t size;    ///< @copydoc cx_rsa_private_key_s::size
  uint8_t d[128]; ///< @copydoc cx_rsa_private_key_s::d
  uint8_t n[128]; ///< @copydoc cx_rsa_private_key_s::n
};
/** Convenience type. See #cx_rsa_1024_public_key_s. */
typedef struct cx_rsa_1024_public_key_s cx_rsa_1024_public_key_t;
/** Convenience type. See #cx_rsa_1024_private_key_s. */
typedef struct cx_rsa_1024_private_key_s cx_rsa_1024_private_key_t;

/** 2048-bit RSA public key */
struct cx_rsa_2048_public_key_s {
  size_t size;    ///< @copydoc cx_rsa_public_key_s::size
  uint8_t e[4];   ///< @copydoc cx_rsa_public_key_s::e
  uint8_t n[256]; ///< @copydoc cx_rsa_public_key_s::n
};
/** 2048-bit RSA private key */
struct cx_rsa_2048_private_key_s {
  size_t size;    ///< @copydoc cx_rsa_private_key_s::size
  uint8_t d[256]; ///< @copydoc cx_rsa_private_key_s::d
  uint8_t n[256]; ///< @copydoc cx_rsa_private_key_s::n
};
/** Convenience type. See #cx_rsa_2048_public_key_s. */
typedef struct cx_rsa_2048_public_key_s cx_rsa_2048_public_key_t;
/** Convenience type. See #cx_rsa_2048_private_key_s. */
typedef struct cx_rsa_2048_private_key_s cx_rsa_2048_private_key_t;

/** 3072-bit RSA public key */
struct cx_rsa_3072_public_key_s {
  size_t size;    ///< @copydoc cx_rsa_public_key_s::size
  uint8_t e[4];   ///< @copydoc cx_rsa_public_key_s::e
  uint8_t n[384]; ///< @copydoc cx_rsa_public_key_s::n
};
/** 3072-bit RSA private key */
struct cx_rsa_3072_private_key_s {
  size_t size;    ///< @copydoc cx_rsa_private_key_s::size
  uint8_t d[384]; ///< @copydoc cx_rsa_private_key_s::d
  uint8_t n[384]; ///< @copydoc cx_rsa_private_key_s::n
};
/** Convenience type. See #cx_rsa_3072_public_key_s. */
typedef struct cx_rsa_3072_public_key_s cx_rsa_3072_public_key_t;
/** Convenience type. See #cx_rsa_3072_private_key_s. */
typedef struct cx_rsa_3072_private_key_s cx_rsa_3072_private_key_t;

/** 4096-bit RSA public key */
struct cx_rsa_4096_public_key_s {
  size_t size;    ///< @copydoc cx_rsa_public_key_s::size
  uint8_t e[4];   ///< @copydoc cx_rsa_public_key_s::e
  uint8_t n[512]; ///< @copydoc cx_rsa_public_key_s::n
};
/** 4096-bit RSA private key */
struct cx_rsa_4096_private_key_s {
  size_t size;    ///< @copydoc cx_rsa_private_key_s::size
  uint8_t d[512]; ///< @copydoc cx_rsa_private_key_s::d
  uint8_t n[512]; ///< @copydoc cx_rsa_private_key_s::n
};
/** Convenience type. See #cx_rsa_4096_public_key_s. */
typedef struct cx_rsa_4096_public_key_s cx_rsa_4096_public_key_t;
/** Convenience type. See #cx_rsa_4096_private_key_s. */
typedef struct cx_rsa_4096_private_key_s cx_rsa_4096_private_key_t;

/**
 * @brief   Initialize a RSA public key.
 * 
 * @details Once initialized, the key may be stored in non-volatile memory
 *          and used for any RSA processing.
 *
 *          Passing NULL as raw key initializes the key without any value.
 *          The key can not be used.
 *
 * @param[in]  exponent     Public exponent: pointer to a raw key value (4 bytes) or NULL.
 * 
 * @param[in]  exponent_len Length of the exponent.
 *
 * @param[in]  modulus      Modulus: pointer to a raw key as big endian value or NULL.
 *
 * @param[in]  modulus_len  Length of the modulus.
 *
 * @param[out] key          Pointer to the RSA public key.
 *
 * @return                  Error code:
 *                          - CX_OK on success
 *                          - CX_INVALID_PARAMETER
 */
cx_err_t cx_rsa_init_public_key_no_throw(const uint8_t *      exponent,
                                size_t               exponent_len,
                                const uint8_t *      modulus,
                                size_t               modulus_len,
                                cx_rsa_public_key_t *key);

/**
 * @brief   Initialize a RSA public key.
 * 
 * @details Once initialized, the key may be stored in non-volatile memory
 *          and used for any RSA processing. 
 *
 *          Passing NULL as raw key initializes the key without any value.
 *          The key can not be used.
 *          This function throws an exception if the initialization fails.
 *
 * @param[in]  exponent     Public exponent: pointer to a raw key value (4 bytes) or NULL.
 * 
 * @param[in]  exponent_len Length of the exponent.
 *
 * @param[in]  modulus      Modulus: pointer to a raw key as big endian value or NULL.
 *
 * @param[in]  modulus_len  Length of the modulus.
 *
 * @param[out] key          Pointer to the RSA public key.
 *
 * @return                  Length of the modulus.
 * 
 * @throws                  CX_INVALID_PARAMETER
 */
static inline int cx_rsa_init_public_key ( const unsigned char * exponent, unsigned int exponent_len, const unsigned char * modulus, unsigned int modulus_len, cx_rsa_public_key_t * key )
{
  CX_THROW(cx_rsa_init_public_key_no_throw(exponent, exponent_len, modulus, modulus_len, key));
  return modulus_len;
}

/**
 * @brief  Initialize a RSA private key.
 * 
 * @details Once initialized, the key may be stored in non-volatile memory
 *          and used for any RSA processing.
 *
 *          Passing NULL as raw key initializes the key without any value.
 *          The key cannot be used.
 *
 * @param[in]  exponent     Private exponent: pointer to a raw key value (4 bytes) or NULL.
 * 
 * @param[in]  exponent_len Length of the exponent.
 *
 * @param[in]  modulus      Modulus: pointer to a raw key as big endian value or NULL.
 *
 * @param[in]  modulus_len  Length of the modulus.
 *
 * @param[out] key          Pointer to the RSA private key.
 *
 * @return                  Error code:
 *                          - CX_OK on success
 *                          - CX_INVALID_PARAMETER
 */
cx_err_t cx_rsa_init_private_key_no_throw(const uint8_t *       exponent,
                                 size_t                exponent_len,
                                 const uint8_t *       modulus,
                                 size_t                modulus_len,
                                 cx_rsa_private_key_t *key);

/**
 * @brief  Initialize a RSA private key.
 * 
 * @details Once initialized, the key may be stored in non-volatile memory
 *          and used for any RSA processing.
 *
 *          Passing NULL as raw key initializes the key without any value.
 *          The key cannot be used.
 *          This function throws an exception if the initialization fails.
 *
 * @param[in]  exponent     Private exponent: pointer to a raw key value (4 bytes) or NULL.
 * 
 * @param[in]  exponent_len Length of the exponent.
 *
 * @param[in]  modulus      Modulus: pointer to a raw key as big endian value or NULL.
 *
 * @param[in]  modulus_len  Length of the modulus.
 *
 * @param[out] key          Pointer to the RSA private key.
 *
 * @return                  Length of the modulus.
 * 
 * @throws                  CX_INVALID_PARAMETER
 */
static inline int cx_rsa_init_private_key ( const unsigned char * exponent, unsigned int exponent_len, const unsigned char * modulus, unsigned int modulus_len, cx_rsa_private_key_t * key )
{
  CX_THROW(cx_rsa_init_private_key_no_throw(exponent, exponent_len, modulus, modulus_len, key));
  return modulus_len;
}

/**
 * @brief   Generate a RSA key pair.
 *
 * @param[in]  modulus_len  Size of the modulus in bytes. Expected sizes:
 *                           - 256
 *                           - 384
 *                           - 512
 *
 * @param[out] public_key   Pointer to the RSA public key. The structure shall match
 *                          *modulus_len*.
 *
 * @param[out] private_key  Pointer to the RSA private key. The structure shall match
 *                          *modulus_len*.
 *
 * @param[in]  pub_exponent Public exponent. ZERO means default value: 0x010001 (65337).
 *                          The public exponent shall be less than 0x0FFFFFFF.
 *                          No verification is done on the public exponent value except its range.
 *
 * @param[in]  exponent_len Length of the exponent.
 * 
 * @param[in]  externalPQ   Pointer to the prime factors of the modulus or NULL pointer. Each prime consists of modulus_len/2
 *                          bytes in big endian order.
 *                          P =  externalPQ[0:modulus_len/2-1], Q = externalPQ[modulus_len/2 : modulus_len-1]
 *                          There is no verification on provided P and Q.
 *
 * @return                  Error code:
 *                          - CX_OK on success
 *                          - CX_INVALID_PARAMETER
 *                          - CX_NOT_UNLOCKED
 *                          - CX_INVALID_PARAMETER_SIZE
 *                          - CX_MEMORY_FULL
 *                          - CX_NOT_LOCKED
 *                          - CX_INTERNAL_ERROR
 *                          - CX_OVERFLOW
 */
cx_err_t cx_rsa_generate_pair_no_throw(size_t       modulus_len,
                              cx_rsa_public_key_t * public_key,
                              cx_rsa_private_key_t *private_key,
                              const uint8_t *       pub_exponent,
                              size_t                exponent_len,
                              const uint8_t *       externalPQ);

/**
 * @brief   Generate a RSA key pair.
 *
 * @param[in]  modulus_len  Size of the modulus in bytes. Expected sizes:
 *                           - 256
 *                           - 384
 *                           - 512
 *
 * @param[out] public_key   Pointer to the RSA public key. The structure shall match
 *                          *modulus_len*.
 *
 * @param[out] private_key  Pointer to the RSA private key. The structure shall match
 *                          *modulus_len*.
 *
 * @param[in]  pub_exponent Public exponent. ZERO means default value: 0x010001 (65337).
 *                          The public exponent shall be less than 0x0FFFFFFF.
 *                          No verification is done on the public exponent value except its range.
 *
 * @param[in]  exponent_len Length of the exponent.
 * 
 * @param[in]  externalPQ   Pointer to the prime factors of the modulus or NULL pointer. Each prime consists of modulus_len/2
 *                          bytes in big endian order.
 *                          P =  externalPQ[0:modulus_len/2-1], Q = externalPQ[modulus_len/2 : modulus_len-1]
 *                          There is no verification on provided P and Q.
 *
 * @return                  Length of the modulus.
 * 
 * @throws                  CX_INVALID_PARAMETER
 * @throws                  CX_NOT_UNLOCKED
 * @throws                  CX_INVALID_PARAMETER_SIZE
 * @throws                  CX_MEMORY_FULL
 * @throws                  CX_NOT_LOCKED
 * @throws                  CX_INTERNAL_ERROR
 * @throws                  CX_OVERFLOW
 */
static inline int cx_rsa_generate_pair ( unsigned int modulus_len, cx_rsa_public_key_t * public_key, cx_rsa_private_key_t * private_key, const unsigned char * pub_exponent, unsigned int exponent_len, const unsigned char * externalPQ )
{
  CX_THROW(cx_rsa_generate_pair_no_throw(modulus_len, public_key, private_key, pub_exponent, exponent_len, externalPQ));
  return modulus_len;
}

/**
 * @brief   Compute a message digest signature according to RSA specification.
 * 
 * @details When using PSS padding, the salt length is fixed to the hash output length.
 *          The MGF1 function is the one descrided in PKCS1 v2.0 specification, 
 *          using the same hash algorithm as specified by hashID.
 *
 * @param[in] key      RSA private key initialized with #cx_rsa_init_private_key_no_throw.
 *
 * @param[in] mode     Crypto mode flags. Supported flags: 
 *                       - CX_PAD_PKCS1_1o5
 *                       - CX_PAD_PKCS1_PSS
 *
 * @param[in] hashID   Message digest algorithm identifier. Expected identifiers:
 *                       - CX_SHA224
 *                       - CX_SHA256
 *                       - CX_SHA384
 *                       - CX_SHA512
 *                       - CX_SHA3_256 (for CX_PAD_PKCS1_1o5 mode only)
 *                       - CX_SHA3_512 (for CX_PAD_PKCS1_1o5 mode only)
 *
 * @param[in] hash     Input data to be signed.
 *
 * @param[in] hash_len Length of the input data.
 *
 * @param[in] sig      Buffer where to store the signature.
 *
 * @param[in] sig_len  Length of the output buffer.
 *
 * @param[in] salt_len Length of the salt.
 *
 * @return             Error code:
 *                     - CX_OK on success
 *                     - CX_INVALID_PARAMETER
 *                     - CX_NOT_UNLOCKED
 *                     - CX_INVALID_PARAMETER_SIZE
 *                     - CX_MEMORY_FULL
 *                     - CX_NOT_LOCKED
 */
cx_err_t cx_rsa_sign_with_salt_len(const cx_rsa_private_key_t *key,
                                   uint32_t                    mode,
                                   cx_md_t                     hashID,
                                   const uint8_t *             hash,
                                   size_t                      hash_len,
                                   uint8_t *                   sig,
                                   size_t                      sig_len,
                                   size_t                      salt_len);

/**
 * @brief   Compute a message digest signature according to RSA specification.
 * 
 * @details When using PSS padding, the salt length is fixed to the hash output length.
 *          If another salt length is used, call #cx_rsa_sign_with_salt_len instead.
 *          The MGF1 function is the one descrided in PKCS1 v2.0 specification, using the
 *          the same hash algorithm as specified by hashID.
 *
 * @param[in] key      RSA private key initialized with #cx_rsa_init_private_key_no_throw.
 *
 * @param[in] mode     Crypto mode flags. Supported flags: 
 *                       - CX_PAD_PKCS1_1o5
 *                       - CX_PAD_PKCS1_PSS
 *
 * @param[in] hashID   Message digest algorithm identifier. Expected identifiers:
 *                       - CX_SHA224
 *                       - CX_SHA256
 *                       - CX_SHA384
 *                       - CX_SHA512
 *                       - CX_SHA3_256 (for CX_PAD_PKCS1_1o5 mode only)
 *                       - CX_SHA3_512 (for CX_PAD_PKCS1_1o5 mode only)
 *
 * @param[in] hash     Input data to be signed.
 *
 * @param[in] hash_len Length of the input data.
 *
 * @param[in] sig      Buffer where to store the signature.
 *
 * @param[in] sig_len  Length of the output buffer.
 *
 * @return             Error code:
 *                     - CX_OK on success
 *                     - CX_INVALID_PARAMETER
 *                     - CX_NOT_UNLOCKED
 *                     - CX_INVALID_PARAMETER_SIZE
 *                     - CX_MEMORY_FULL
 *                     - CX_NOT_LOCKED
 */
cx_err_t cx_rsa_sign_no_throw(const cx_rsa_private_key_t *key,
                     uint32_t                    mode,
                     cx_md_t                     hashID,
                     const uint8_t *             hash,
                     size_t                      hash_len,
                     uint8_t *                   sig,
                     size_t                      sig_len);

/**
 * @brief   Compute a message digest signature according to RSA specification.
 * 
 * @details When using PSS padding, the salt length is fixed to the hash output length.
 *          If another salt length is used, call #cx_rsa_sign_with_salt_len instead.
 *          The MGF1 function is the one descrided in PKCS1 v2.0 specification, using the
 *          the same hash algorithm as specified by hashID.
 *          This function throws an exception if the computation doesn't succeed.
 *
 * @param[in] key      RSA private key initialized with #cx_rsa_init_private_key_no_throw.
 *
 * @param[in] mode     Crypto mode flags. Supported flags: 
 *                       - CX_PAD_PKCS1_1o5
 *                       - CX_PAD_PKCS1_PSS
 *
 * @param[in] hashID   Message digest algorithm identifier. Expected identifiers:
 *                       - CX_SHA224
 *                       - CX_SHA256
 *                       - CX_SHA384
 *                       - CX_SHA512
 *                       - CX_SHA3_256 (for CX_PAD_PKCS1_1o5 mode only)
 *                       - CX_SHA3_512 (for CX_PAD_PKCS1_1o5 mode only)
 *
 * @param[in] hash     Input data to be signed.
 *
 * @param[in] hash_len Length of the input data.
 *
 * @param[in] sig      Buffer where to store the signature.
 *
 * @param[in] sig_len  Length of the output buffer.
 *
 * @return             Length of the key.
 *
 * @throws             CX_INVALID_PARAMETER
 * @throws             CX_NOT_UNLOCKED
 * @throws             CX_INVALID_PARAMETER_SIZE
 * @throws             CX_MEMORY_FULL
 * @throws             CX_NOT_LOCKED
 */
static inline int cx_rsa_sign ( const cx_rsa_private_key_t * key, int mode, cx_md_t hashID, const unsigned char * hash, unsigned int hash_len, unsigned char * sig, unsigned int sig_len )
{
  CX_THROW(cx_rsa_sign_no_throw(key, mode, hashID, hash, hash_len, sig, sig_len));
  return key->size;
}

/**
 * @brief   Verify a message digest signature.
 * 
 * @details Verify a message digest signature according to RSA specification
 *          with a specified salt length.
 *
 * @param[in] key      RSA public key initialized with #cx_rsa_init_public_key_no_throw.
 *
 * @param[in] mode     Crypto mode flags. Supported flags: 
 *                       - CX_PAD_PKCS1_1o5
 *                       - CX_PAD_PKCS1_PSS
 *
 * @param[in] hashID   Message digest algorithm identifier. Expected identifiers:
 *                       - CX_SHA224
 *                       - CX_SHA256
 *                       - CX_SHA384
 *                       - CX_SHA512
 *                       - CX_SHA3_256 (for CX_PAD_PKCS1_1o5 mode only)
 *                       - CX_SHA3_512 (for CX_PAD_PKCS1_1o5 mode only)
 *
 * @param[in] hash     Input data to be signed.
 *
 * @param[in] hash_len Length of the input data.
 *
 * @param[in] sig      RSA signature encoded as raw bytes.
 *                     This is used as a temporary buffer.
 * 
 * @param[in] sig_len  Length of the signature.
 * 
 * @param[in] salt_len Length of the salt.
 *
 * @return             1 if the signature is verified, 0 otherwise.
 */
bool cx_rsa_verify_with_salt_len(const cx_rsa_public_key_t *key,
                                 uint32_t                   mode,
                                 cx_md_t                    hashID,
                                 const uint8_t *            hash,
                                 size_t                     hash_len,
                                 uint8_t *                  sig,
                                 size_t                     sig_len,
                                 size_t                     salt_len);

/**
 * @brief   Verify a message digest signature.
 *
 * @details Verify a message digest signature according to RSA specification.
 *          Please note that if the mode is set to CX_PAD_PKCS1_PSS, then
 *          the underlying salt length is by convention equal to the hash length.
 *          If another salt length is used, please call #cx_rsa_verify_with_salt_len
 *          instead.
 *
 * @param[in] key      RSA public key initialized with #cx_rsa_init_public_key_no_throw.
 *
 * @param[in] mode     Crypto mode flags. Supported flags: 
 *                       - CX_PAD_PKCS1_1o5
 *                       - CX_PAD_PKCS1_PSS
 *
 * @param[in] hashID   Message digest algorithm identifier. Expected identifiers:
 *                       - CX_SHA224
 *                       - CX_SHA256
 *                       - CX_SHA384
 *                       - CX_SHA512
 *                       - CX_SHA3_256 (for CX_PAD_PKCS1_1o5 mode only)
 *                       - CX_SHA3_512 (for CX_PAD_PKCS1_1o5 mode only)
 *
 * @param[in] hash     Input data to be signed.
 *
 * @param[in] hash_len Length of the input data.
 *
 * @param[in] sig      RSA signature encoded as raw bytes.
 *                     This is used as a temporary buffer.
 * 
 * @param[in] sig_len  Length of the signature.
 *
 * @return             1 if the signature is verified, 0 otherwise.
 *
 */
  bool cx_rsa_verify(const cx_rsa_public_key_t *key,
                              uint32_t                   mode,
                              cx_md_t                    hashID,
                              const uint8_t *            hash,
                              size_t                     hash_len,
                              uint8_t *                  sig,
                              size_t                     sig_len);

/**
 * @brief   Encrypt a message according to RSA specification.
 *
 * @param[in] key     RSA public key initialized with #cx_rsa_init_public_key_no_throw.
 *
 * @param[in] mode    Crypto mode flags. Supported flags:
 *                       - CX_PAD_PKCS1_1o5
 *                       - CX_PAD_PKCS1_OAEP
 *
 * @param[in] hashID   Message digest algorithm identifier. Expected identifiers
 *                     used in OAEP padding:
 *                       - CX_SHA224
 *                       - CX_SHA256
 *                       - CX_SHA384
 *                       - CX_SHA512
 *
 * @param[in] mesg     Input data to encrypt.
 *
 * @param[in] mesg_len Length of the input.
 *
 * @param[in] enc      Buffer where to store the ciphertext.
 * 
 * @param[in] enc_len  Length of the ciphertext.
 *
 * @return             Error code:
 *                     - CX_OK on success
 *                     - CX_INVALID_PARAMETER
 *                     - CX_NOT_UNLOCKED
 *                     - CX_INVALID_PARAMETER_SIZE
 *                     - CX_MEMORY_FULL
 *                     - CX_NOT_LOCKED
 */
cx_err_t cx_rsa_encrypt_no_throw(const cx_rsa_public_key_t *key,
                        uint32_t                   mode,
                        cx_md_t                    hashID,
                        const uint8_t *            mesg,
                        size_t                     mesg_len,
                        uint8_t *                  enc,
                        size_t                     enc_len);

/**
 * @brief   Encrypt a message according to RSA specification.
 * 
 * @details This function throws an exception if the computation doesn't succeed.
 *
 * @param[in] key     RSA public key initialized with #cx_rsa_init_public_key_no_throw.
 *
 * @param[in] mode    Crypto mode flags. Supported flags:
 *                       - CX_PAD_PKCS1_1o5
 *                       - CX_PAD_PKCS1_OAEP
 *
 * @param[in] hashID   Message digest algorithm identifier. Expected identifiers
 *                     used in OAEP padding:
 *                       - CX_SHA224
 *                       - CX_SHA256
 *                       - CX_SHA384
 *                       - CX_SHA512
 *
 * @param[in] mesg     Input data to encrypt.
 *
 * @param[in] mesg_len Length of the input.
 *
 * @param[in] enc      Buffer where to store the ciphertext.
 * 
 * @param[in] enc_len  Length of the ciphertext.
 *
 * @return             Length of the key.
 * 
 * @throws             CX_INVALID_PARAMETER
 * @throws             CX_NOT_UNLOCKED
 * @throws             CX_INVALID_PARAMETER_SIZE
 * @throws             CX_MEMORY_FULL
 * @throws             CX_NOT_LOCKED
 */
static inline int cx_rsa_encrypt ( const cx_rsa_public_key_t * key, int mode, cx_md_t hashID, const unsigned char * mesg, unsigned int mesg_len, unsigned char * enc, unsigned int enc_len )
{
  CX_THROW(cx_rsa_encrypt_no_throw(key, mode, hashID, mesg, mesg_len, enc, enc_len));
  return key->size;
}

/**
 * @brief   Decrypt a message according to RSA specification.
 *
 * @param[in] key     RSA private key initialized with #cx_rsa_init_private_key_no_throw.
 *
 * @param[in] mode    Crypto mode flags. Supported flags:
 *                       - CX_PAD_PKCS1_1o5
 *                       - CX_PAD_PKCS1_OAEP
 *
 * @param[in] hashID   Message digest algorithm identifier. Expected identifiers
 *                     used in OAEP padding:
 *                       - CX_SHA224
 *                       - CX_SHA256
 *                       - CX_SHA384
 *                       - CX_SHA512
 *
 * @param[in] mesg     Input data to decrypt.
 *
 * @param[in] mesg_len Length of the input.
 *
 * @param[in] dec      Buffer where to store the decrypted data.
 * 
 * @param[in] dec_len  Length of the output.
 *
 * @return             Error code:
 *                     - CX_OK on success
 *                     - CX_INVALID_PARAMETER
 *                     - CX_NOT_UNLOCKED
 *                     - CX_INVALID_PARAMETER_SIZE
 *                     - CX_MEMORY_FULL
 *                     - CX_NOT_LOCKED
 */
cx_err_t cx_rsa_decrypt_no_throw(const cx_rsa_private_key_t *key,
                        uint32_t                    mode,
                        cx_md_t                     hashID,
                        const uint8_t *             mesg,
                        size_t                      mesg_len,
                        uint8_t *                   dec,
                        size_t *                    dec_len);

/**
 * @brief   Decrypt a message according to RSA specification.
 * 
 * @details This function throws an exception if the computation doesn't succeed.
 *
 * @param[in] key     RSA private key initialized with #cx_rsa_init_private_key_no_throw.
 *
 * @param[in] mode    Crypto mode flags. Supported flags:
 *                       - CX_PAD_PKCS1_1o5
 *                       - CX_PAD_PKCS1_OAEP
 *
 * @param[in] hashID   Message digest algorithm identifier. Expected identifiers
 *                     used in OAEP padding:
 *                       - CX_SHA224
 *                       - CX_SHA256
 *                       - CX_SHA384
 *                       - CX_SHA512
 *
 * @param[in] mesg     Input data to decrypt.
 *
 * @param[in] mesg_len Length of the input.
 *
 * @param[in] dec      Buffer where to store the decrypted data.
 * 
 * @param[in] dec_len  Length of the output.
 *
 * @return             Length of the decrypted message.
 *
 * @throws             CX_INVALID_PARAMETER
 * @throws             CX_NOT_UNLOCKED
 * @throws             CX_INVALID_PARAMETER_SIZE
 * @throws             CX_MEMORY_FULL
 * @throws             CX_NOT_LOCKED
 */
static inline int cx_rsa_decrypt ( const cx_rsa_private_key_t * key, int mode, cx_md_t hashID, const unsigned char * mesg, unsigned int mesg_len, unsigned char * dec, unsigned int dec_len )
{
  size_t dec_len_ = dec_len;
  CX_THROW(cx_rsa_decrypt_no_throw(key, mode, hashID, mesg, mesg_len, dec, &dec_len_));
  return dec_len_;
}

#endif

#endif // HAVE_RSA
