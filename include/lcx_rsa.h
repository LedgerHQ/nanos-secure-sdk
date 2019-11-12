
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

#ifndef LCX_RSA_H
#define LCX_RSA_H

/** Abstract RSA public key.
 *
 * This type shall not be instantiate, it is only defined to allow unified API
 * for RSA operations
 */
struct cx_rsa_public_key_s {
  /** Key size in bytes */
  unsigned int size;
  /** 32 bits public exponent */
  unsigned char e[4];
  /** public modulus */
  unsigned char n[1];
};
struct cx_rsa_private_key_s {
  /** Key size in bytes */
  unsigned int size;
  /** private exponent */
  unsigned char d[1];
  /** public modulus */
  unsigned char n[1];
};
/** Convenience type. See #cx_rsa_public_key_s. */
typedef struct cx_rsa_public_key_s cx_rsa_public_key_t;
/** Convenience type. See #cx_rsa_private_key_s. */
typedef struct cx_rsa_private_key_s cx_rsa_private_key_t;

/** 1024 bits RSA public key */
struct cx_rsa_1024_public_key_s {
  /** @copydoc cx_rsa_public_key_s::size */
  unsigned int size;
  /** @copydoc cx_rsa_public_key_s::e */
  unsigned char e[4];
  /** @copydoc cx_rsa_public_key_s::n */
  unsigned char n[128];
};
/** 1024 bits RSA private key */
struct cx_rsa_1024_private_key_s {
  /** @copydoc cx_rsa_private_key_s::size */
  unsigned int size;
  /** @copydoc cx_rsa_private_key_s::e */
  unsigned char d[128];
  /** @copydoc cx_rsa_private_key_s::n */
  unsigned char n[128];
};
/** Convenience type. See #cx_rsa_1024_public_key_s. */
typedef struct cx_rsa_1024_public_key_s cx_rsa_1024_public_key_t;
/** Convenience type. See #cx_rsa_1024_private_key_s. */
typedef struct cx_rsa_1024_private_key_s cx_rsa_1024_private_key_t;

/** 2048 bits RSA public key */
struct cx_rsa_2048_public_key_s {
  /** @copydoc cx_rsa_public_key_s::size */
  unsigned int size;
  /** @copydoc cx_rsa_public_key_s::e */
  unsigned char e[4];
  /** @copydoc cx_rsa_public_key_s::n */
  unsigned char n[256];
};
/** 2048 bits RSA private key */
struct cx_rsa_2048_private_key_s {
  /** @copydoc cx_rsa_private_key_s::size */
  unsigned int size;
  /** @copydoc cx_rsa_private_key_s::e */
  unsigned char d[256];
  /** @copydoc cx_rsa_private_key_s::n */
  unsigned char n[256];
};
/** Convenience type. See #cx_rsa_2048_public_key_s. */
typedef struct cx_rsa_2048_public_key_s cx_rsa_2048_public_key_t;
/** Convenience type. See #cx_rsa_2048_private_key_s. */
typedef struct cx_rsa_2048_private_key_s cx_rsa_2048_private_key_t;

/** 3072 bits RSA public key */
struct cx_rsa_3072_public_key_s {
  /** @copydoc cx_rsa_public_key_s::size */
  unsigned int size;
  /** @copydoc cx_rsa_public_key_s::e */
  unsigned char e[4];
  /** @copydoc cx_rsa_public_key_s::n */
  unsigned char n[384];
};
/** 3072 bits RSA private key */
struct cx_rsa_3072_private_key_s {
  /** @copydoc cx_rsa_private_key_s::size */
  unsigned int size;
  /** @copydoc cx_rsa_private_key_s::e */
  unsigned char d[384];
  /** @copydoc cx_rsa_private_key_s::n */
  unsigned char n[384];
};
/** Convenience type. See #cx_rsa_3072_public_key_s. */
typedef struct cx_rsa_3072_public_key_s cx_rsa_3072_public_key_t;
/** Convenience type. See #cx_rsa_3072_private_key_s. */
typedef struct cx_rsa_3072_private_key_s cx_rsa_3072_private_key_t;

/** 4096 bits RSA public key */
struct cx_rsa_4096_public_key_s {
  /** @copydoc cx_rsa_public_key_s::size */
  unsigned int size;
  /** @copydoc cx_rsa_public_key_s::e */
  unsigned char e[4];
  /** @copydoc cx_rsa_public_key_s::n */
  unsigned char n[512];
};
/** 4096 bits RSA private key */
struct cx_rsa_4096_private_key_s {
  /** @copydoc cx_rsa_private_key_s::size */
  unsigned int size;
  /** @copydoc cx_rsa_private_key_s::e */
  unsigned char d[512];
  /** @copydoc cx_rsa_private_key_s::n */
  unsigned char n[512];
};
/** Convenience type. See #cx_rsa_4096_public_key_s. */
typedef struct cx_rsa_4096_public_key_s cx_rsa_4096_public_key_t;
/** Convenience type. See #cx_rsa_4096_private_key_s. */
typedef struct cx_rsa_4096_private_key_s cx_rsa_4096_private_key_t;

/**
 * Initialize a public RSA Key.
 *
 * Once initialized, the key may be stored in non-volatile memory
 * an reused 'as-is' for any RSA processing
 *
 * Passing NULL as raw key initializes the key without value. The key can not be
 * used
 *
 * @param [in] 4 bytes public exponent
 *   Raw key value or NULL.
 *
 * @param [in] private modulus
 *   Raw key as big endian  value or NULL.
 *
 * @param [in] modulus_len
 *   Key bytes lenght
 *
 * @param [out] key
 *   Public RSA key to init.
 *
 * @return something
 *
 * @throws INVALID_PARAMETER
 */
CXCALL int cx_rsa_init_public_key(
    const unsigned char WIDE *exponent PLENGTH(exponent_len),
    unsigned int exponent_len,
    const unsigned char WIDE *modulus PLENGTH(modulus_len),
    unsigned int modulus_len,
    cx_rsa_public_key_t *key PLENGTH(sizeof(cx_rsa_public_key_t) +
                                     modulus_len));

/**
 * Initialize a private RSA Key.
 * Once initialized, the key may be stored in non-volatile memory
 * an reused 'as-is' for any RSA processing
 * Passing NULL as raw key initializes the key without value. The key can not be
 * used
 *
 * @param [in] private exponent
 *   Raw key value or NULL.
 *
 * @param [in] private modulus
 *   Raw key value or NULL.
 *
 * @param [in] modulus_len
 *   Key bytes lenght
 *
 * @param [out] key
 *   Public RSA key to init.
 *
 * @return something
 *
 * @throws INVALID_PARAMETER
 */
CXCALL int cx_rsa_init_private_key(
    const unsigned char WIDE *exponent PLENGTH(exponent_len),
    unsigned int exponent_len,
    const unsigned char WIDE *modulus PLENGTH(modulus_len),
    unsigned int modulus_len,
    cx_rsa_private_key_t *key PLENGTH(sizeof(cx_rsa_private_key_t) +
                                      2 * modulus_len));

/**
 * Generate a rsa key pair
 *
 * @param [in] modulus_len
 *   Moduluse size in bytes. SHALL be one of 256,384 or 512
 *
 * @param [out] public_key
 *   A rsa public key to generate. The real struct SHALL match the modulus_len
 *
 * @param [out] private_key
 *   A rsa privat key to generate. The real struct SHALL match the modulus_len
 *
 * @param [in] pub_exponent
 *   Public exponent. ZERO means default value: 0x010001 (65337). The public
 * exponent shall be lesser than 0x0FFFFFFF. No verification is done on the
 * public exponent value except its range. An invalid value may throw an error
 * or provide unuseable key pair.
 *
 * @param [in] externalPQ
 *   If set to non NULL, it is assumed it contains primes  P and Q. They shall
 * be modulus_len/2 bytes length and store in big endian order. P =
 * externalPQ[0:modulus_len/2-1], Q = externalPQ[modulus_len/2 : modulus_len-1]
 *   There is no verification on provided P and Q,  Invalid values may throw an
 * error or provide unuseable key pair.
 *
 * @return zero
 *
 * @throws INVALID_PARAMETER
 */
CXCALL int cx_rsa_generate_pair(
    unsigned int modulus_len,
    cx_rsa_public_key_t *public_key PLENGTH(sizeof(cx_rsa_public_key_t) +
                                            modulus_len),
    cx_rsa_private_key_t *private_key PLENGTH(sizeof(cx_rsa_private_key_t) +
                                              2 * modulus_len),
    const unsigned char *pub_exponent, unsigned int exponent_len,
    const unsigned char *externalPQ PLENGTH(modulus_len));

/**
 * Sign a hash message signature according to RSA specification.
 *
 * When using PSS padding, the salt len is fixed to to hash output.
 * The MGF1 function is the one descrided in PKCS1 v2.0 specifiction, using the
 * the same hash algorithm as specified by hashID.
 *
 * @param [in] key
 *   A private RSA key fully inited with 'cx_rsa_init_private_key'
 *
 * @param [in] mode
 *   Crypto mode flags. See above.
 *   Supported flags:
 *     - CX_PAD_PKCS1_1o5
 *     - CX_PAD_PKCS1_PSS
 *
 * @param [in] hashID
 *  Hash identifier used to compute the input hash. It shall be one of:
 *    - CX_SHA224
 *    - CX_SHA256
 *    - CX_SHA384
 *    - CX_SHA512
 *
 * @param [in] hash
 *   Input hash data to sign
 *   The data should be the hash of the original message.
 *
 * @param [in] hash_len
 *   Length of input to data.
 *
 * @param [in] sig
 *   Where to set the signature
 *
 * @return
 *   1 if signature is verified
 *   0 is signarure is not verified
 *
 * @throws INVALID_PARAMETER
 */
CXCALL int cx_rsa_sign(const cx_rsa_private_key_t WIDE *key
                           PLENGTH(scc__cx_scc_struct_size_rsa_privkey__key),
                       int mode, cx_md_t hashID,
                       const unsigned char WIDE *hash PLENGTH(hash_len),
                       unsigned int hash_len,
                       unsigned char WIDE *sig PLENGTH(sig_len),
                       unsigned int sig_len);

/**
 * Verify a hash message signature according to RSA specification.
 *
 * @param [in] key
 *   A public RSA key fully inited with 'cx_rsa_init_public_key'
 *
 * @param [in] mode
 *   Crypto mode flags. See above.
 *   Supported flags:
 *     - CX_PAD_PKCS1_1o5
 *     - CX_PAD_PKCS1_PSS
 *
 * @param [in] hashID
 *  Hash identifier used to compute the input data.It shall be one of:
 *    - CX_SHA224
 *    - CX_SHA256
 *    - CX_SHA384
 *    - CX_SHA512
 *
 * @param [in] hash
 *   Input hash data to verify
 *   The data should be the hash of the original message.
 *
 * @param [in] hash_len
 *   Length of input to data.
 *
 * @param [in] sig
 *   RSA signature to verify encoded as raw bytes
 *
 * @return
 *   1 if signature is verified
 *   0 is signarure is not verified
 *
 * @throws INVALID_PARAMETER
 */
CXCALL int cx_rsa_verify(const cx_rsa_public_key_t WIDE *key
                             PLENGTH(scc__cx_scc_struct_size_rsa_pubkey__key),
                         int mode, cx_md_t hashID,
                         const unsigned char WIDE *hash PLENGTH(hash_len),
                         unsigned int hash_len,
                         const unsigned char WIDE *sig PLENGTH(sig_len),
                         unsigned int sig_len);

/**
 * Encrypt a message according to RSA specification.
 *
 * @param [in] key
 *   A public RSA key fully inited with 'cx_rsa_init_public_key'
 *
 * @param [in] mode
 *   Crypto mode flags. See above.
 *   Supported flags:
 *     - CX_PAD_PKCS1_1o5
 *     - CX_PAD_PKCS1_OAEP
 *
 * @param [in] hashID
 *  Hash identifier to use in OEAP padding. It shall be one of:
 *    - CX_SHA224
 *    - CX_SHA256
 *    - CX_SHA384
 *    - CX_SHA512
 *
 * @param [in] mesg
 *   Input message data to encrypt
 *
 * @param [in] mesg_len
 *   Length of input message
 *
 * @param [in] enc
 *   Where to set the encrypted message
 *
 * @return
 *   1 if encryption succeed
 *   0 else
 *
 * @throws INVALID_PARAMETER
 */
CXCALL CXPORT(CXPORT_ED_RSA) int cx_rsa_encrypt(
    const cx_rsa_public_key_t WIDE *key
        PLENGTH(scc__cx_scc_struct_size_rsa_pubkey__key),
    int mode, cx_md_t hashID, const unsigned char WIDE *mesg PLENGTH(mesg_len),
    unsigned int mesg_len, unsigned char WIDE *enc PLENGTH(enc_len),
    unsigned int enc_len);

/**
 * Decrypt a mesg message signature according to RSA specification.
 *
 * @param [in] key
 *   A private RSA key fully inited with 'cx_rsa_init_private_key'
 *
 * @param [in] mode
 *   Crypto mode flags. See above.
 *   Supported flags:
 *     - CX_PAD_PKCS1_1o5
 *     - CX_PAD_PKCS1_PSS
 *
 * @param [in] hashID
 *  Hash identifier to use in OEAP padding. It shall be one of:
 *    - CX_SHA224
 *    - CX_SHA256
 *    - CX_SHA384
 *    - CX_SHA512
 *
 * @param [in] mesg
 *   Input message to decrypt.
 *
 * @param [in] mesg_len
 *   Length of input  to decrypt.
 *
 * @param [out] sig
 *   Where tp put decrypted message
 *
 * @return
 *   >=0 length of decrypted message
 *   -1  if decryption fail
 *
 * @throws INVALID_PARAMETER
 */
CXCALL CXPORT(CXPORT_ED_RSA) int cx_rsa_decrypt(
    const cx_rsa_private_key_t WIDE *key
        PLENGTH(scc__cx_scc_struct_size_rsa_privkey__key),
    int mode, cx_md_t hashID, const unsigned char WIDE *mesg PLENGTH(mesg_len),
    unsigned int mesg_len, unsigned char WIDE *dec PLENGTH(dec_len),
    unsigned int dec_len);

#endif
