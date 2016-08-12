/*******************************************************************************
*   Ledger Nano S - Secure firmware
*   (c) 2016 Ledger
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

#ifndef CX_H
#define CX_H

/* ####################################################################### */
/*                                    OPTIONS                              */
/* ####################################################################### */

/* ####################################################################### */
/*                                  CHIP/LIB3rd                            */
/* ####################################################################### */

/* ####################################################################### */
/*                                  COMMON                                 */
/* ####################################################################### */

/* 64bits types, native or by-hands, depending on target and/or compiler
 * support.
 * This type is defined here only because sha-3 struct used it INTENALLY.
 * It should never be directly used by other modules.
 */

#ifndef NATIVE_64BITS // NO 64BITS
struct uint64_s {
#ifdef OS_LITTLE_ENDIAN
    unsigned long int l;
    unsigned long int h;
#else
    unsigned long int h;
    unsigned long int l;
#endif
};
typedef struct uint64_s uint64bits_t;
#else
typedef unsigned long long uint64bits_t;
#endif

/*
 * Crypto mode encoding:
 * =====================
 *
 * size:
 * -----
 *  int, a least 16 bits
 *
 * encoding:
 * ---------
 *  | bit pos   |  H constant        |   meanings
 *  ---------------------------------------------------
 *  |  0        |  CX_LAST           | last block
 *  |           |                    |
 *
 *  |  2:1      |  CX_ENCRYPT        |
 *  |           |  CX_DECRYPT        |
 *  |           |  CX_SIGN           |
 *  |           |  CX_VERIFY         |
 *
 *  |  5:3      |  CX_PAD_NONE       |
 *  |           |  CX_PAD_ISO9797M1  |
 *  |           |  CX_PAD_ISO9797M2  |
 *  |           |  CX_PAD_PKCS1_1o5  |
 *  |           |  CX_PAD_PKCS1_PSS  |
 *  |           |  CX_PAD_PKCS1_OAEP |
 *
 *  |  8:6      |  CX_CHAIN_ECB      |
 *  |           |  CX_CHAIN_CBC      |
 *  |           |  CX_CHAIN_CTR      |
 *  |           |  CX_CHAIN_CFB      |
 *  |           |  CX_CHAIN_OFB      |
 *
 *  |  11:9     |  CX_RND_TRNG       |
 *              |  CX_RND_RFC6979    |
 *
 *  |  14:12    |  CX_ECDH_POINT     | share full point
 *  |           |  CX_ECDH_X         | share only x coordinate
 *  |           |  CX_ECDH_HASHED    | return a sha256 of the x coordinate
 *  |           |  CX_ECSCHNORR_XY   | Use X,Y coordinate when computing
 signature (ISO 14888)
 *  |           |                    | Use X only if not set (BSI TR 03111)
 *
 *  |  15        | CX_NO_REINIT      | do not reinitialize context on CX_LAST
 when supported

 */

/**
 * Bit 0
 */
#define CX_LAST (1 << 0)

/**
 * Bit 1
 */
#define CX_SIG_MODE (1 << 1)

/**
 * Bit 2:1
 */
#define CX_MASK_SIGCRYPT (3 << 1)
#define CX_ENCRYPT (2 << 1)
#define CX_DECRYPT (0 << 1)
#define CX_SIGN (CX_SIG_MODE | CX_ENCRYPT)
#define CX_VERIFY (CX_SIG_MODE | CX_DECRYPT)

/**
 * Bit 5:3
 */
#define CX_MASK_PAD (7 << 3)
#define CX_PAD_NONE (0 << 3)
#define CX_PAD_ISO9797M1 (1 << 3)
#define CX_PAD_ISO9797M2 (2 << 3)
#define CX_PAD_PKCS1_1o5 (3 << 3)
#define CX_PAD_PKCS1_PSS (4 << 3)
#define CX_PAD_PKCS1_OAEP (5 << 3)
/**
 * Bit 7:6
 */
#define CX_MASK_CHAIN (7 << 6)
#define CX_CHAIN_ECB (0 << 6)
#define CX_CHAIN_CBC (1 << 6)
#define CX_CHAIN_CTR (2 << 6)
#define CX_CHAIN_CFB (3 << 6)
#define CX_CHAIN_OFB (4 << 6)

/**
 * Bit 11:9
 */
#define CX_MASK_RND (7 << 9)
#define CX_RND_PRNG (1 << 9)
#define CX_RND_TRNG (2 << 9)
#define CX_RND_RFC6979 (3 << 9)

/**
 * Bit 14:12
 */
#define CX_MASK_EC (7 << 12)
#define CX_ECDH_POINT (1 << 12)
#define CX_ECDH_X (2 << 12)
#define CX_ECSCHNORR_ISO14888_XY (1 << 12)
#define CX_ECSCHNORR_ISO14888_X (2 << 12)
#define CX_ECSCHNORR_BSI03111 (3 << 12)
#define CX_ECSCHNORR_LIBSECP (4 << 12)

/**
 * Bit 15
 */
#define CX_NO_REINIT (1 << 15)

/* ####################################################################### */
/*                                   RAND                                  */
/* ####################################################################### */

/**
 * generate a random char
 */
SYSCALL unsigned char cx_rng_u8(void);

/**
 * generate a random buffer
 */
SYSCALL unsigned char *cx_rng(unsigned char *buffer PLENGTH(len),
                              unsigned int len);

/* ####################################################################### */
/*                                 HASH/HMAC                               */
/* ####################################################################### */

/* ======================================================================= */
/*                                   HASH                                 */
/* ======================================================================= */
/*
 * ripemd160 :protocole standard
 * sha256    :protocole standard
 * sha512    :bip32
 */

enum cx_md_e {
    CX_NONE,
    CX_RIPEMD160, // 20 bytes
    CX_SHA224, // 28 bytes
    CX_SHA256, // 32 bytes
    CX_SHA384, // 48 bytes
    CX_SHA512, // 64 bytes
    CX_KECCAK, // 28,32,48,64 bytes
    CX_SHA3, // 28,32,48,64 bytes
    CX_SHA3_XOF, // any bytes
};
typedef enum cx_md_e cx_md_t;

#define CX_RIPEMD160_SIZE 20
#define CX_SHA256_SIZE 32
#define CX_SHA512_SIZE 64

#define CX_HASH_MAX_BLOCK_COUNT 65535

struct cx_hash_header_s {
    cx_md_t algo;
    unsigned int counter;
};

struct cx_ripemd160_s {
    struct cx_hash_header_s header;
    // 64 bytes per block
    int blen;
    unsigned char block[64];
    // five 32bits words
    unsigned char acc[5 * 4];
};
typedef struct cx_ripemd160_s cx_ripemd160_t;

struct cx_sha256_s {
    struct cx_hash_header_s header;
#if defined(CX_SHA256_NES_LIB)
    tNesLibSHA256State state;
#else
    // 64 bytes per block
    int blen;
    unsigned char block[64];
    // eight 32bits words
    unsigned char acc[8 * 4];
#endif
};
typedef struct cx_sha256_s cx_sha256_t;

struct cx_sha512_s {
    struct cx_hash_header_s header;
#if defined(CX_SHA512_NES_LIB)
    tNesLibSHA512State state;
#else
    // 128 bytes per block
    unsigned int blen;
    unsigned char block[128];
    // eight 64bits words
    unsigned char acc[8 * 8];
#endif
};
typedef struct cx_sha512_s cx_sha512_t;

struct cx_sha3_s {
    struct cx_hash_header_s header;

    // x bytes per input block, depends on sha3-xxx output size
    unsigned int output_size;
    unsigned char block_size;
    //
    unsigned int blen;
    unsigned char block[200];
    // use 64bits type to ensure alignment
    uint64bits_t acc[25];
    // unsigned char    acc[200];
};
typedef struct cx_sha3_s cx_sha3_t;

typedef struct cx_hash_header_s cx_hash_t;

/**
 * Init a ripmd160 context.
 *
 * @param [out] hash the context to init.
 *    The context shall be in RAM
 *
 * @return algorithm identifier
 */
SYSCALL int
cx_ripemd160_init(cx_ripemd160_t *hash PLENGTH(sizeof(cx_ripemd160_t)));

/**
 * Init a sha256 context.
 *
 * @param [out] hash the context to init.
 *    The context shall be in RAM
 *
 * @return algorithm identifier
 */
SYSCALL int cx_sha256_init(cx_sha256_t *hash PLENGTH(sizeof(cx_sha256_t)));

/**
 * Init a sha512 context.
 *
 * @param [out] hash the context to init.
 *    The context shall be in RAM
 *
 * @return algorithm identifier
 */
SYSCALL int cx_sha512_init(cx_sha512_t *hash PLENGTH(sizeof(cx_sha512_t)));

/**
 * Init a sha3 context.
 *
 * SHA3 family as specified in FIPS 202.
 * Supported output sizes are: 224,256,384,512
 *
 * @param [out] hash  the context to init.
 *    The context shall be in RAM
 *
 * @param [in] size   output sha3 size, in BITS.

 *
 * @return algorithm identifier
 */
SYSCALL int cx_sha3_init(cx_sha3_t *hash PLENGTH(sizeof(cx_sha3_t)), int size);

/**
 * Init a sha3 context.
 *
 * SHA3 family as specified in KECCAK submission.
 * Supported output sizes are: 224,256,384,512
 *
 * @param [out] hash  the context to init.
 *    The context shall be in RAM
 *
 * @param [in] size   output sha3 size, in BITS.

 *
 * @return algorithm identifier
 */
SYSCALL int cx_keccak_init(cx_sha3_t *hash PLENGTH(sizeof(cx_sha3_t)),
                           int size);

/**
 * Init a sha3-XOF context.
 *
 * SHA3-XOF family as specified in FIPS 202.
 * Supported output sha3 sizes are: 256,512
 *
 * @param [out] hash        the context to init.
 *    The context shall be in RAM
 *
 * @param [in] size         output sha3 size, in BITS.
 * @param [in] out_length   desired output size, in BYTES.
 *
 * @return algorithm identifier
 */
SYSCALL int cx_sha3_xof_init(cx_sha3_t *hash PLENGTH(sizeof(cx_sha3_t)),
                             unsigned int size, unsigned int out_length);

/**
 * Add more data to hash.
 *
 * @param  [in/out] hash
 *   Univers Continuation Blob.
 *   The hash context pointer shall point to  either a cx_ripemd160_t, either a
 * cx_sha256_t  or cx_sha512_t .
 *   The hash context shall be inited with 'cx_xxx_init'
 *   The hash context shall be in RAM
 *   The function should be called with a nice cast.
 *
 * @param  [in] mode
 *   16bits flags. See Above
 *   If CX_LAST is set,
 *     - hash can be found in cx_ripemd160|sha1|sha256|sha512_t.acc field
 *     - if  out is provided, hash is copied in out
 *     - context is NOT automatically re-inited.
 *   Supported flags:
 *     - CX_LAST
 *
 * @param  [in] in
 *   Input data to add to current hash
 *
 * @param  [in] len
 *   Length of input to data.
 *
 * @param [out] out
 *   Either:
 *     - NULL (ignored) if CX_LAST is NOT set
 *     - produced hash  if CX_LAST is set
 *   'out' length is implicit, no check is done
 *
 */
SYSCALL int cx_hash(cx_hash_t *hash PLENGTH(scc__cx_hash_ctx_size__hash),
                    int mode, unsigned char WIDE *in PLENGTH(len),
                    unsigned int len,
                    unsigned char *out PLENGTH(scc__cx_hash_size__hash));

/**
 * One shot sha256 digest
 *
 * @param  [in] in
 *   Input data to compute the hash
 *
 * @param  [in] len
 *   Length of input to data.
 *
 * @param [out] out
 *   'out' length is implicit
 *
 */
SYSCALL int cx_hash_sha256(unsigned char WIDE *in PLENGTH(len),
                           unsigned int len, unsigned char *out PLENGTH(32));

/**
 * One shot sha512 digest
 *
 * @param  [in] in
 *   Input data to compute the hash
 *
 * @param  [in] len
 *   Length of input to data.
 *
 * @param [out] out
 *   'out' length is implicit
 *
 */
SYSCALL int cx_hash_sha512(unsigned char WIDE *in PLENGTH(len),
                           unsigned int len, unsigned char *out PLENGTH(64));

/* ======================================================================= */
/*                                 HASH MAC                                */
/* ======================================================================= */

/*
 * hmac : bip32 seed extension
 */

/*Note: DO NOT reorder the following structures, it will break magic casts */

struct cx_hmac_ripemd160_s {
    struct cx_ripemd160_s hash;
    // 64 bytes key
    unsigned char key_len;
    unsigned char key[64];
};
typedef struct cx_hmac_ripemd160_s cx_hmac_ripemd160_t;

struct cx_hmac_sha256_s {
    struct cx_sha256_s hash;
    // 64 bytes key
    unsigned char key_len;
    unsigned char key[64];
};
typedef struct cx_hmac_sha256_s cx_hmac_sha256_t;

struct cx_hmac_sha512_s {
    struct cx_sha512_s hash;
    // 128 bytes key
    unsigned char key_len;
    unsigned char key[128];
};
typedef struct cx_hmac_sha512_s cx_hmac_sha512_t;

typedef struct cx_hash_header_s cx_hmac_t;

/**
 * Init a hmac sha512 context.
 *
 * @param  [out] hash        the context to init.
 *    The context shall be in RAM
 *
 * @param  [in] key         hmac key value
 *    Passing a NULL pointeur, will reinit the context with the previously set
 * key.
 *    If no key has already been set, passing NULL will lead into an undefined
 * behavior.
 *
 * @param  [in] key_len     hmac key length
 *    The key length shall be less than 64 bytes
 *
 * @return algorithm  identifier
 */
SYSCALL int cx_hmac_ripemd160_init(
    cx_hmac_ripemd160_t *hmac PLENGTH(sizeof(cx_hmac_ripemd160_t)),
    unsigned char WIDE *key PLENGTH(key_len), unsigned int key_len);

/**
 * Init a hmac sha256 context.
 *
 * @param [out] hash        the context to init.
 *    The context shall be in RAM
 *
 * @param [in] key         hmac key value
 *    Passing a NULL pointeur, will reinit the context with the previously set
 * key.
 *    If no key has already been set, passing NULL will lead into an undefined
 * behavior.
 *
 * @param [in] key_len     hmac key length
 *    The key length shall be less than 64 bytes
 *
 * @return algorithm  identifier
 */
SYSCALL int
cx_hmac_sha256_init(cx_hmac_sha256_t *hmac PLENGTH(sizeof(cx_hmac_sha256_t)),
                    unsigned char WIDE *key PLENGTH(key_len),
                    unsigned int key_len);

/**
 * Init a hmac sha512 context.
 *
 * @param [out] hash        the context to init.
 *    The context shall be in RAM
 *
 * @param [in] key         hmac key value
 *    Passing a NULL pointeur, will reinit the context with the previously set
 * key.
 *    If no key has already been set, passing NULL will lead into an undefined
 * behavior.
 *
 * @param [in] key_len     hmac key length
 *    The key length shall be less than 128 bytes
 *
 * @return algorithm  identifier
 */
SYSCALL int
cx_hmac_sha512_init(cx_hmac_sha512_t *hmac PLENGTH(sizeof(cx_hmac_sha512_t)),
                    unsigned char WIDE *key PLENGTH(key_len),
                    unsigned int key_len);

/**
 * @param [in/out] hmac
 *   Univers Continuation Blob.
 *   The hmac context pointer shall point to  either a cx_ripemd160_t, either a
 * cx_sha256_t  or cx_sha512_t .
 *   The hmac context shall be inited with 'cx_xxx_init'
 *   The hmac context shall be in RAM
 *   The function should be called with a nice cast.
 *
 * @param [in] mode
 *   16bits flags. See Above
 *   If CX_LAST is set and CX_NO_REINIT is not set, context is automatically
 * re-inited.
 *   Supported flags:
 *     - CX_LAST
 *     - CX_NO_REINIT
 *
 * @param [in] in
 *   Input data to add to current hmac
 *
 * @param [in] len
 *   Length of input to data.
 *
 * @param [out] mac
 *   Either:
 *     - NULL (ignored) if CX_LAST is NOT set
 *     - produced hmac  if CX_LAST is set
 *   'out' length is implicit, no check is done
 *
 */
SYSCALL int cx_hmac(cx_hmac_t *hmac PLENGTH(scc__cx_hmac_ctx_size__hmac),
                    int mode, unsigned char WIDE *in PLENGTH(len),
                    unsigned int len,
                    unsigned char *mac PLENGTH(scc__cx_hmac_size__hmac));
/**
 * One shot hmac sha512 digest
 *
 * @param  [in] key_in
 *   hmac key value
 *
 * @param  [in] key_len
 *   Length of the hmac key
 *
 * @param  [in] in
 *   Input data to compute the hash
 *
 * @param  [in] len
 *   Length of input to data.
 *
 * @param [out] out
 *   'out' length is implicit
 *
 */
SYSCALL int cx_hmac_sha512(unsigned char WIDE *key PLENGTH(key_len),
                           unsigned int key_len,
                           unsigned char WIDE *in PLENGTH(len),
                           unsigned int len, unsigned char *out PLENGTH(64));

/**
 * One shot hmac sha256 digest
 *
 * @param  [in] key_in
 *   hmac key value
 *
 * @param  [in] key_len
 *   Length of the hmac key
 *
 * @param  [in] in
 *   Input data to compute the hash
 *
 * @param  [in] len
 *   Length of input to data.
 *
 * @param [out] out
 *   'out' length is implicit
 *
 */
SYSCALL int cx_hmac_sha256(unsigned char WIDE *key PLENGTH(key_len),
                           unsigned int key_len,
                           unsigned char WIDE *in PLENGTH(len),
                           unsigned int len, unsigned char *out PLENGTH(32));

//#ifdef CX_PBKDF2
/* ======================================================================= */
/*                                  PKDF2                                  */
/* ======================================================================= */

/**
 * @param [in]  password
 *    password
 * @param [in]  passwordlen
 *    password len
 * @param [in]  salt
 *    salt
 * @param [in]  saltlen
 *    salt value
 * @param [in]  iterations
 *    iteration
 * @param [in]  out
 *    where to put result
 * @param [in] outLength
 *    how many...
 *
 */
SYSCALL void cx_pbkdf2_sha512(unsigned char *password PLENGTH(passwordlen),
                              unsigned short passwordlen,
                              unsigned char *salt PLENGTH(saltlen),
                              unsigned short saltlen, unsigned int iterations,
                              unsigned char *out PLENGTH(outLength),
                              unsigned int outLength);
//#endif // CX_PBKDF2

/* ####################################################################### */
/*                               CIPHER/SIGNATURE                          */
/* ####################################################################### */
/* - DES
 * - ECDSA
 * - ECDH
 */

/* ======================================================================= */
/*                                   DES                                   */
/* ======================================================================= */

#define CX_DES_BLOCK_SIZE 8

struct cx_des_key_s {
    unsigned char size;
    unsigned char keys[16];
};

typedef struct cx_des_key_s cx_des_key_t;

/**
 * Initialize a DES Key.
 * Once initialized, the key may be stored in non-volatile memory
 * an reused 'as-is' for any DES processing
 *
 * @param [in] rawkey
 *   raw key value
 *
 * @param [in] key_len
 *   key bytes lenght: 8,16 or 24
 *
 * @param [out] key
 *   DES key to init
 *
 * @param key
 *   ready to use key to init
 */
SYSCALL int cx_des_init_key(unsigned char WIDE *rawkey PLENGTH(key_len),
                            unsigned int key_len,
                            cx_des_key_t *key PLENGTH(sizeof(cx_des_key_t)));

/**
 * Encrypt, Decrypt, Sign or Verify data with DES algorithm.
 *
 * @param [in] key
 *   A des key fully inited with 'cx_des_init_key'
 *
 * @param [in] mode
 *   16bits crypto mode flags. See above.
 *   Supported flags:
 *     - CX_LAST
 *     - CX_ENCRYPT
 *     - CX_DECRYPT
 *     - CX_SIGN
 *     - CX_VERIFY
 *     - CX_PAD_NONE
 *     - CX_PAD_ISO9797M1
 *     - CX_PAD_ISO9797M2
 *     - CX_CHAIN_ECB
 *     - CX_CHAIN_CBC
 *
 * @param [in] in
 *   Input data to encrypt/decrypt
 *
 * @param [in] len
 *   Length of input to data.
 *   If CX_LAST is set, padding is automatically done according to  'mode'.
 *   Else  'len' shall be a multiple of DES_BLOCK_SIZE.
 *
 * @param [in] iv
 *   Initial IV for chaining mode
 *
 * @param [out] out
 *   Either:
 *     - encrypted/decrypted ouput data
 *     - produced signature
 *     - signature to check
 *   'out' buffer length is implicit, no check is done
 *
 * @return
 *   - In case of ENCRYPT, DECRYPT or SIGN mode: output lenght data
 *   - In case of VERIFY mode: 0 if signature is false, DES_BLOCK_SIZE if
 * signature is correct
 *
 * @throws INVALID_PARAMETER
 */

SYSCALL int cx_des_iv(cx_des_key_t WIDE *key PLENGTH(sizeof(cx_des_key_t)),
                      int mode, unsigned char WIDE *iv PLENGTH(8),
                      unsigned char WIDE *in PLENGTH(len), unsigned int len,
                      unsigned char *out PLENGTH(len + 8));

/**
 *  Same as cx_des_iv with initial IV assumed to be heigt zeros.
 */
SYSCALL int cx_des(cx_des_key_t WIDE *key PLENGTH(sizeof(cx_des_key_t)),
                   int mode, unsigned char WIDE *in PLENGTH(len),
                   unsigned int len, unsigned char *out PLENGTH(len + 8));

/* ======================================================================= */
/*                                   AES                                   */
/* ======================================================================= */

#define CX_AES_BLOCK_SIZE 16

struct cx_aes_key_s {
    unsigned int size;
    unsigned char keys[32];
};

typedef struct cx_aes_key_s cx_aes_key_t;

/**
 * Initialize a AES Key.
 * Once initialized, the key may be stored in non-volatile memory
 * an reused 'as-is' for any AES processing
 *
 * @param [in] rawkey
 *   raw key value
 *
 * @param [in] key_len
 *   key bytes lenght: 8,16 or 24
 *
 * @param [out] key
 *   AES key to init
 *
 * @param key
 *   ready to use key to init
 */
SYSCALL int cx_aes_init_key(unsigned char WIDE *rawkey PLENGTH(key_len),
                            unsigned int key_len,
                            cx_aes_key_t *key PLENGTH(sizeof(cx_aes_key_t)));

/**
 * Encrypt, Decrypt, Sign or Verify data with AES algorithm.
 *
 * @param [in] key
 *   A aes key fully inited with 'cx_aes_init_key'
 *
 * @param [in] mode
 *   16bits crypto mode flags. See above.
 *   Supported flags:
 *     - CX_LAST
 *     - CX_ENCRYPT
 *     - CX_DECRYPT
 *     - CX_SIGN
 *     - CX_VERIFY
 *     - CX_PAD_NONE
 *     - CX_PAD_ISO9797M1
 *     - CX_PAD_ISO9797M2
 *     - CX_CHAIN_ECB
 *     - CX_CHAIN_CBC
 *
 * @param [in] in
 *   Input data to encrypt/decrypt
 *
 * @param [in] len
 *   Length of input to data.
 *   If CX_LAST is set, padding is automtically done according to  'mode'.
 *   Else  'len' shall be a multiple of AES_BLOCK_SIZE.
 *
 * @param [out] out
 *   Either:
 *     - encrypted/decrypted output data
 *     - produced signature
 *     - signature to check
 *   'out' buffer length is implicit, no check is done
 *
 * @return
 *   - In case of ENCRYPT, DECRYPT or SIGN mode: output length data
 *   - In case of VERIFY mode: 0 if signature is false, AES_BLOCK_SIZE if
 * signature is correct
 *
 * @throws INVALID_PARAMETER
 */
SYSCALL int cx_aes_iv(cx_aes_key_t WIDE *key PLENGTH(sizeof(cx_aes_key_t)),
                      int mode, unsigned char WIDE *iv PLENGTH(16),
                      unsigned char WIDE *in PLENGTH(len), unsigned int len,
                      unsigned char *out PLENGTH(len + 15));

/**
 *  Same as cx_aes_iv with initial IV assumed to be sixteen zeros.
 */
SYSCALL int cx_aes(cx_aes_key_t WIDE *key PLENGTH(sizeof(cx_aes_key_t)),
                   int mode, unsigned char WIDE *in PLENGTH(len),
                   unsigned int len, unsigned char *out PLENGTH(len + 16));

//#ifdef CX_RSA
/* ======================================================================= */
/*                                     RSA                                 */
/* ======================================================================= */
struct cx_rsa_public_key_s {
    unsigned int size;
    unsigned char e[4];
    unsigned char n[1];
};
struct cx_rsa_private_key_s {
    unsigned int size;
    unsigned char d[1];
    unsigned char n[1];
};
typedef struct cx_rsa_public_key_s cx_rsa_public_key_t;
typedef struct cx_rsa_private_key_s cx_rsa_private_key_t;

/* 1024 bits */
struct cx_rsa_1024_public_key_s {
    unsigned int size;
    unsigned char e[4];
    unsigned char n[128];
};
struct cx_rsa_1024_private_key_s {
    unsigned int size;
    unsigned char d[128];
    unsigned char n[128];
};
typedef struct cx_rsa_1024_public_key_s cx_rsa_1024_public_key_t;
typedef struct cx_rsa_1024_private_key_s cx_rsa_1024_private_key_t;

/* 2048 bits */
struct cx_rsa_2048_public_key_s {
    unsigned int size;
    unsigned char e[4];
    unsigned char n[256];
};
struct cx_rsa_2048_private_key_s {
    unsigned int size;
    unsigned char d[256];
    unsigned char n[256];
};
typedef struct cx_rsa_2048_public_key_s cx_rsa_2048_public_key_t;
typedef struct cx_rsa_2048_private_key_s cx_rsa_2048_private_key_t;

/* 3072 bits */
struct cx_rsa_3072_public_key_s {
    unsigned int size;
    unsigned char e[4];
    unsigned char n[384];
};
struct cx_rsa_3072_private_key_s {
    unsigned int size;
    unsigned char d[384];
    unsigned char n[384];
};
typedef struct cx_rsa_3072_public_key_s cx_rsa_3072_public_key_t;
typedef struct cx_rsa_3072_private_key_s cx_rsa_3072_private_key_t;

/* 4096 bits */
struct cx_rsa_4096_public_key_s {
    unsigned int size;
    unsigned char e[512];
    unsigned char n[512];
};
struct cx_rsa_4096_private_key_s {
    unsigned int size;
    unsigned char d[512];
    unsigned char n[512];
};
typedef struct cx_rsa_4096_public_key_s cx_rsa_4096_public_key_t;
typedef struct cx_rsa_4096_private_key_s cx_rsa_4096_private_key_t;

/**
 * Initialize a public RSA Key.
 * Once initialized, the key may be stored in non-volatile memory
 * an reused 'as-is' for any RSA processing
 * Passing NULL as raw key initializes the key without value. The key can not be
 * used
 *
 * @param [in] 4 bytes public exponent
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
SYSCALL int cx_rsa_init_public_key(
    unsigned char WIDE *exponent PLENGTH(4),
    unsigned char WIDE *modulus PLENGTH(modulus_len), unsigned int modulus_len,
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
SYSCALL int cx_rsa_init_private_key(
    unsigned char WIDE *exponent PLENGTH(modulus_len),
    unsigned char WIDE *modulus PLENGTH(modulus_len), unsigned int modulus_len,
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
 *    A rsa privat key to generate. The real struct SHALL match the modulus_len
 *
 * @param [in] pub_exponent
 *    public exponent. ZERO means default value: 0x010001 (65337)
 *
 * @return zero
 *
 * @throws INVALID_PARAMETER
 */
SYSCALL int cx_rsa_generate_pair(
    unsigned int modulus_len,
    cx_rsa_public_key_t *public_key PLENGTH(sizeof(cx_rsa_public_key_t) +
                                            modulus_len),
    cx_rsa_private_key_t *private_key PLENGTH(sizeof(cx_rsa_private_key_t) +
                                              2 * modulus_len),
    unsigned long int pub_exponent);

/**
 * Sign a hash message signature according to RSA specification.
 *
 * @param [in] key
 *   A private RSA key fully inited with 'cx_rsa_init_private_key'
 *
 * @param [in] mode
 *   16bits crypto mode flags. See above.
 *   Supported flags:
 *     - CX_PAD_PKCS1_1o5
 *     - CX_PAD_PKCS1_PSS
 *
 * @param [in] hashID
 *  Hash identifier used to compute the input data. Only sha256 is supported.
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
SYSCALL int cx_rsa_sign(cx_rsa_private_key_t WIDE *key
                            PLENGTH(scc__cx_rsa_private_key_ctx_size__key),
                        int mode, cx_md_t hashID,
                        unsigned char WIDE *hash PLENGTH(hash_len),
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
 *   16bits crypto mode flags. See above.
 *   Supported flags:
 *     - CX_PAD_PKCS1_1o5
 *     - CX_PAD_PKCS1_PSS
 *
 * @param [in] hashID
 *  Hash identifier used to compute the input data.
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
SYSCALL int cx_rsa_verify(
    cx_rsa_public_key_t WIDE *key PLENGTH(scc__cx_rsa_public_key_ctx_size__key),
    int mode, cx_md_t hashID, unsigned char WIDE *hash PLENGTH(hash_len),
    unsigned int hash_len, unsigned char WIDE *sig PLENGTH(sig_len),
    unsigned int sig_len);

/**
 * Encrypt a message according to RSA specification.
 *
 * @param [in] key
 *   A public RSA key fully inited with 'cx_rsa_init_public_key'
 *
 * @param [in] mode
 *   16bits crypto mode flags. See above.
 *   Supported flags:
 *     - CX_PAD_PKCS1_1o5
 *     - CX_PAD_PKCS1_OAEP
 *
 * @param [in] hashID
 *  Hash identifier used to compute the in internal data. Only sha256 is
 * supported.
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
SYSCALL int cx_rsa_encrypt(
    cx_rsa_public_key_t WIDE *key PLENGTH(scc__cx_rsa_public_key_ctx_size__key),
    int mode, cx_md_t hashID, unsigned char WIDE *mesg PLENGTH(mesg_len),
    unsigned int mesg_len, unsigned char WIDE *enc PLENGTH(enc_len),
    unsigned int enc_len);

/**
 * Decrypt a mesg message signature according to RSA specification.
 *
 * @param [in] key
 *   A private RSA key fully inited with 'cx_rsa_init_private_key'
 *
 * @param [in] mode
 *   16bits crypto mode flags. See above.
 *   Supported flags:
 *     - CX_PAD_PKCS1_1o5
 *     - CX_PAD_PKCS1_PSS
 *
 * @param [in] hashID
 *  Hash identifier used to compute the input data.
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
SYSCALL int cx_rsa_decrypt(cx_rsa_private_key_t WIDE *key
                               PLENGTH(scc__cx_rsa_private_key_ctx_size__key),
                           int mode, cx_md_t hashID,
                           unsigned char WIDE *mesg PLENGTH(mesg_len),
                           unsigned int mesg_len,
                           unsigned char WIDE *dec PLENGTH(dec_len),
                           unsigned int dec_len);

//#endif //CX_RSA

/* ======================================================================= */
/*                                     ECC                                 */
/* ======================================================================= */
/** Only curved defined at compiled time will be supported */

enum cx_curve_e {
    CX_CURVE_NONE,
    CX_CURVE_256K1,
    CX_CURVE_256R1,
    CX_CURVE_192K1,
    CX_CURVE_192R1,
    CX_CURVE_Ed25519,
};
typedef enum cx_curve_e cx_curve_t;

#define CX_CURVE_HEADER                                                        \
    cx_curve_t id;                                                             \
    unsigned int size;                                                         \
    unsigned char WIDE *p;  /*Field*/                                          \
    unsigned char WIDE *Hp; /*2nd Mongtomery constant for Field*/              \
    unsigned char WIDE *Gx; /*Point Generator x coordinate*/                   \
    unsigned char WIDE *Gy; /*Point Generator y coordinate*/                   \
    unsigned char WIDE *n;  /*Curve order*/                                    \
    unsigned char WIDE *Hn  /*2nd Mongtomery constant for Curve order*/

/**
 * Weirstrass curve :     y??=x??+a*x+b        over F(p)
 *
 */
typedef struct cx_curve_weierstrass_s {
    CX_CURVE_HEADER;
    unsigned char WIDE *a; // Weierstrass a coef
    unsigned char WIDE *b; // Weierstrass b coef
    int h; // Weierstrass cofactor
} cx_curve_weierstrass_t;

/*
 * Twisted Edward curve : a*x??+y??=1+d*x??*y??  over F(q)
 */
typedef struct cx_curve_twisted_edward_t {
    CX_CURVE_HEADER;
    unsigned char WIDE *a; // T Edward a coef
    unsigned char WIDE *d; // T Edward d coef
    unsigned char WIDE *I; // Square root of -1
    unsigned char WIDE *Q3; //(q+3)/8
} cx_curve_twisted_edward_t;

typedef struct cx_curve_domain_s { CX_CURVE_HEADER; } cx_curve_domain_t;

cx_curve_domain_t WIDE *cx_ecfp_get_domain(cx_curve_t curve);

extern cx_curve_weierstrass_t const WIDE C_cx_secp256k1;

struct cx_ecfp_public_key_s {
    cx_curve_t curve;
    unsigned int W_len;
    unsigned char W[65];
};

struct cx_ecfp_private_key_s {
    cx_curve_t curve;
    unsigned int d_len;
    unsigned char d[32];
};

typedef struct cx_ecfp_public_key_s cx_ecfp_public_key_t;
typedef struct cx_ecfp_private_key_s cx_ecfp_private_key_t;

/**
 * Verify that a given point is really on the specified curve.
 *
 * @param [in] domain
 *   The curve domain parameters to work with.
 *
 * @param [in]  public_point
 *   The point to test  encoded as: 04 x y
 *
 * @return
 *    1 if point is on the curve
 *    0 if point is not on the curve
 *   -1 if undefined (function not impl)
 *
 * @throws INVALID_PARAMETER
 */
SYSCALL int cx_ecfp_is_valid_point(
    cx_curve_domain_t WIDE *domain PLENGTH(scc__cx_ecfp_domain_scc__domain),
    unsigned char WIDE *point PLENGTH(1 + 32 + 32));

/**
 * Add two affine point
 *
 * @param [in] domain
 *   The curve domain parameters to work with.
 *
 * @param [out] R
 *   P+Q encoded as: 04 x y, where x and y are
 *   encoded as  big endian raw value and have bits length equals to
 *   the curve size.
 *
 * @param [in] P
 *   First point to add *
 *   The value shall be a point encoded as: 04 x y, where x and y are
 *   encoded as  big endian raw value and have bits length equals to
 *   the curve size.
 *
 * @param [in] Q
 *   Second point to add
 *
 * @param [in]  public_point
 *   The point to test  encoded as: 04 x y
 *
 * @return
 *   R encoding length, if add success
 *   -1 if failed
 *
 * @throws INVALID_PARAMETER
 */
SYSCALL int cx_ecfp_add_point(cx_curve_domain_t WIDE *domain
                                  PLENGTH(scc__cx_ecfp_domain_scc__domain),
                              unsigned char *R PLENGTH(1 + 32 + 32),
                              unsigned char WIDE *P PLENGTH(1 + 32 + 32),
                              unsigned char WIDE *Q PLENGTH(1 + 32 + 32));

/**
 * Multiply an  affine point
 *
 * @param [in] domain
 *   The curve domain parameters to work with.
 *
 * @param [out] R
 *   R = k.P encoded as: 04 x y, where x and y are
 *   encoded as  big endian raw value and have bits length equals to
 *   the curve size.
 *
 * @param [in] P
 *   Point to multiply *
 *   The value shall be a point encoded as: 04 x y, where x and y are
 *   encoded as  big endian raw value and have bits length equals to
 *   the curve size.
 *
 * @param [in] k
 *   scalar to multiply
 *
 * @param [in] k_len
 *   byte length of scalar to multiply
 *
 * @return
 *   R encoding length, if mult success
 *   -1 if failed
 *
 * @throws INVALID_PARAMETER
 */
SYSCALL int cx_ecfp_scalar_mult(
    cx_curve_domain_t WIDE *domain PLENGTH(scc__cx_ecfp_domain_scc__domain),
    unsigned char *P, unsigned char WIDE *k PLENGTH(k_len), unsigned int k_len);

/**
 * Initialize a public ECFP Key.
 * Once initialized, the key may be stored in non-volatile memory
 * an reused 'as-is' for any ECDSA/25519 processing
 * Passing NULL as raw key initializes the key without value. The key may be
 used
 * as parameter for cx_ecfp_generate_pair.

 * @param [in] curve
 *   The curve domain parameters to work with.
 *
 * @param [in] rawkey
 *   Raw key value or NULL.
 *   The value shall be the public point encoded as: 04 x y, where x and y are
 *   encoded as  big endian raw value and have bits length equals to
 *   the curve size.
 *
 * @param [in] key_len
 *   Key bytes lenght
 *
 * @param [out] key
 *   Public ecfp key to init.
 *
 * @param key
 *   Ready to use key to init
 *
 * @return something
 *
 * @throws INVALID_PARAMETER
 */
SYSCALL int cx_ecfp_init_public_key(
    cx_curve_t curve, unsigned char WIDE *rawkey PLENGTH(key_len),
    unsigned int key_len,
    cx_ecfp_public_key_t *key PLENGTH(sizeof(cx_ecfp_public_key_t)));

/**
 * Initialize a private ECFP Key.
 * Once initialized, the key may be  stored in non-volatile memory
 * and reused 'as-is' for any ECDSA/EC25519 processing
 * Passing NULL as raw key initializes the key without value. The key may be
 * used
 * as parameter for cx_ecfp_generate_pair.
 *
 * @param [in] curve
 *   The curve domain parameters to work with.
 *
 * @param [in] rawkey
 *   Raw key value or NULL.
 *   The value shall be the private key big endian raw value.
 *
 * @param [in] key_len
 *   Key bytes lenght
 *
 * @param [out] key
 *   Private ecfp key to init.
 *
 * @param key
 *   Ready to use key to init
 *
 * @return something
 *
 * @throws INVALID_PARAMETER
 */
SYSCALL int cx_ecfp_init_private_key(
    cx_curve_t curve, unsigned char WIDE *rawkey PLENGTH(key_len),
    unsigned int key_len,
    cx_ecfp_private_key_t *key PLENGTH(sizeof(cx_ecfp_private_key_t)));

/**
 * Generate a ecfp key pair
 *
 * @param [in] curve
 *   The curve domain parameters to work with.
 *
 * @param [out] pubkey
 *   A public ecfp public key to generate.
 *
 * @param [in/out] privkey
 *   A private ecfp private key to generate.
 *   Either:
 *     - if the private ecfp key is fully inited, i.e  parameter 'rawkey' of
 *       'cx_ecfp_init_private_key' is NOT null, the private key value is kept
 *       if the 'keep_private' parameter is non zero
 *     - else a new private key is generated.
 *
 * @param [in] keep_private if set to non zero, keep the private key value if
 * set.
 *             Else generate a new random one
 *
 * @return zero
 *
 * @throws INVALID_PARAMETER
 */
SYSCALL int cx_ecfp_generate_pair(
    cx_curve_t curve,
    cx_ecfp_public_key_t *pubkey PLENGTH(sizeof(cx_ecfp_public_key_t)),
    cx_ecfp_private_key_t *privkey
        PLENGTH(scc__cx_ecfp_private_key2_scc__privkey_keepprivate),
    int keepprivate);

/* =========================== Borromean ================================== */
SYSCALL int cx_borromean_sign(
    cx_ecfp_private_key_t **privkeys, cx_ecfp_public_key_t **pubkeys,
    unsigned int *rsizes
        PLENGTH(scc__cx_borromean_scc__privkeys_pubkeys_rsizes_rcount_sig),
    unsigned int *pv_keys_index PLENGTH(rcount * sizeof(unsigned int)),
    unsigned int rcount, unsigned int mode, cx_md_t hashID,
    unsigned char WIDE *msg PLENGTH(msg_len), unsigned int msg_len,
    unsigned char *sig);

SYSCALL int cx_borromean_verify(
    cx_ecfp_public_key_t **pubkeys,
    unsigned int *rsizes
        PLENGTH(scc__cx_borromean_scc__NULL_pubkeys_rsizes_rcount_NULL),
    unsigned int rcount, int mode, cx_md_t hashID,
    unsigned char WIDE *msg PLENGTH(msg_len), unsigned int msg_len,
    unsigned char *sig PLENGTH(sig_len), unsigned int sig_len);

/* ============================ ECSchnorr ================================== */
/**
 * Sign a hash message according to ECSchnorr specification (BSI TR 03111).
 *
 * @param [in] key
 *   A private ecfp key fully inited with 'cx_ecfp_init_private_key'
 *
 * @param [in] mode
 *   16bits crypto mode flags. See above.
 *   Supported flags:
 *     - CX_ECSCHNORR_XY
 *
 * @param [in] hashID
 *  Hash identifier used to compute the input data.
 *  This parameter is mandatory for rng of type CX_RND_RFC6979.
 *
 * @param [in] msg
 *   Input data to sign.
 *
 * @param [in] msg_len
 *   Length of input to data.
 *
 * @param [out] sig
 *   ECSchnorr signature encoded as TLV:  30 L 02 Lr r 02 Ls s
 *
 * @return
 *   Full length of signature
 *
 * @throws INVALID_PARAMETER
 */
SYSCALL int cx_ecschnorr_sign(
    cx_ecfp_private_key_t WIDE *pvkey
        PLENGTH(scc__cx_ecfp_private_key_scc__pvkey),
    int mode, cx_md_t hashID, unsigned char WIDE *msg PLENGTH(msg_len),
    unsigned int msg_len, unsigned char *sig PLENGTH(1 + 1 + 2 * (1 + 1 + 33)));

/**
 * Verify a hash message signature according to ECSchnorr specification (BSI TR
 * 03111).
 *
 * @param [in] key
 *   A public ecfp key fully inited with 'cx_ecfp_init_public_key'
 *
 * @param [in] mode
 *   16bits crypto mode flags. See above.
 *   Supported flags:
 *     - CX_ECSCHNORR_XY
 *
 * @param [in] hashID
 *  Hash identifier used to compute the input data.
 *
 * @param [in] msg
 *   Signed input data to verify the signature.
 *
 * @param [in] msg_len
 *   Length of input to data.
 *
 * @param [in] sig
 *   ECDSA signature to verify encoded as TLV:  30 L 02 Lr r 02 Ls s
 *
 * @return
 *   1 if signature is verified
 *   0 is signature is not verified
 *
 * @throws INVALID_PARAMETER
 */
SYSCALL int cx_ecschnorr_verify(cx_ecfp_public_key_t WIDE *pukey
                                    PLENGTH(scc__cx_ecfp_public_key_scc__pukey),
                                int mode, cx_md_t hashID,
                                unsigned char WIDE *msg PLENGTH(msg_len),
                                unsigned int msg_len,
                                unsigned char WIDE *sig PLENGTH(sig_len),
                                unsigned int sig_len);

/* ============================= EdDSA =================================== */
//#ifdef CX_EDDSA

/**
 *  Compress point according to draft-irtf-cfrg-eddsa-05.
 *
 * @param [in]     domain
 * @param [in/out] P
 */
SYSCALL void
cx_edward_compress_point(cx_curve_twisted_edward_t WIDE *domain
                             PLENGTH(scc__cx_ecfp_domain_scc__domain),
                         unsigned char *P PLENGTH(65));

/**
 *  Decompress point according to draft-irtf-cfrg-eddsa-05.
 *
 * @param [in]     domain
 * @param [in/out] P
 */
SYSCALL void
cx_edward_decompress_point(cx_curve_twisted_edward_t WIDE *domain
                               PLENGTH(scc__cx_ecfp_domain_scc__domain),
                           unsigned char *P PLENGTH(65));

/**
 * Retrieve the public key from the private one according to
 * draft-irtf-cfrg-eddsa-05.
 *
 * @param [in]  pvkey
 * @param [out] pukey
 */
SYSCALL void cx_eddsa_get_public_key(
    cx_ecfp_private_key_t WIDE *pvkey
        PLENGTH(scc__cx_ecfp_private_key_scc__pvkey),
    cx_ecfp_public_key_t *pukey PLENGTH(scc__cx_ecfp_public_key_scc__pukey));

/**
 * Sign a hash message according to EdDSA specification.
 *
 * @param [in] pv_key
 *   A private ecfp key fully inited with 'cx_ecfp_init_private_key'.
 *
 * @param [in] pu_key
 *   A public ecfp key fully inited with 'cx_ecfp_init_private_key' or NULL
 *   If NULL is specified, the public key value is internally retrieved.
 *
 * @param [in] mode
 *   16bits crypto mode flags. See above.
 *   Supported flags:
 *      <none>
 *
 * @param [in] hashID
 *  Hash identifier used to compute the input data.
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
 *   EdDSA signature encoded as
 *
 * @return
 *   Full length of signature
 *
 * @throws INVALID_PARAMETER
 */
SYSCALL int cx_eddsa_sign(cx_ecfp_private_key_t WIDE *pvkey
                              PLENGTH(scc__cx_ecfp_private_key_scc__pvkey),
                          cx_ecfp_public_key_t WIDE *pukey
                              PLENGTH(scc__cx_ecfp_public_key_scc__pukey),
                          int mode, cx_md_t hashID,
                          unsigned char WIDE *hash PLENGTH(hash_len),
                          unsigned int hash_len,
                          unsigned char *sig PLENGTH(32 * 2));

/**
 * Verify a hash message signature according to EDDSA specification.
 *
 * @param [in] key
 *   A public ecfp key fully inited with 'cx_ecfp_init_public_key'
 *
 * @param [in] mode
 *   16bits crypto mode flags. See above.
 *   Supported flags:
 *     - <none>
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
 *   EDDSA signature to verify encoded as :
 *
 * @return
 *   1 if signature is verified
 *   0 is signarure is not verified
 *
 * @throws INVALID_PARAMETER
 */
SYSCALL int cx_eddsa_verify(
    cx_ecfp_public_key_t WIDE *key PLENGTH(scc__cx_ecfp_public_key_scc__key),
    int mode, cx_md_t hashID, unsigned char WIDE *hash PLENGTH(hash_len),
    unsigned int hash_len, unsigned char WIDE *sig PLENGTH(sig_len),
    unsigned int sig_len);
// #endif // CX_EDDSA

/* ============================= ECDSA =================================== */

#define cx_ecdsa_init_public_key cx_ecfp_init_public_key
#define cx_ecdsa_init_private_key cx_ecfp_init_private_key

/**
 * Sign a hash message according to ECDSA specification.
 *
 * @param [in] key
 *   A private ecfp key fully inited with 'cx_ecfp_init_private_key'
 *
 * @param [in] mode
 *   16bits crypto mode flags. See above.
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
 * @return
 *   Full length of signature
 *
 * @throws INVALID_PARAMETER
 */
SYSCALL int cx_ecdsa_sign(
    cx_ecfp_private_key_t WIDE *key PLENGTH(scc__cx_ecfp_private_key_scc__key),
    int mode, cx_md_t hashID, unsigned char WIDE *hash PLENGTH(hash_len),
    unsigned int hash_len,
    unsigned char *sig PLENGTH(1 + 1 + 2 * (1 + 1 + 33)));

/**
 * Verify a hash message signature according to ECDSA specification.
 *
 * @param [in] key
 *   A public ecfp key fully inited with 'cx_ecfp_init_public_key'
 *
 * @param [in] mode
 *   16bits crypto mode flags. See above.
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
SYSCALL int cx_ecdsa_verify(
    cx_ecfp_public_key_t WIDE *key PLENGTH(scc__cx_ecfp_public_key_scc__key),
    int mode, cx_md_t hashID, unsigned char WIDE *hash PLENGTH(hash_len),
    unsigned int hash_len, unsigned char WIDE *sig PLENGTH(sig_len),
    unsigned int sig_len);

/* ======================================================================= */
/*                                    ECC-KA                               */
/* ======================================================================= */

/**
 * Compute a shared secret according to ECDH specifiaction
 * Depending on the mode, the shared secret is either the full point or
 * only the x coordinate
 *
 * @param [in] key
 *   A private ecfp key fully inited with 'cx_ecfp_init_private_key'
 *
 * @param [in] mode
 *   16bits crypto mode flags. See above.
 *   Supported flags:
 *     - CX_ECDH_POINT
 *     - CX_ECDH_X
 *
 * @param [in] public_point
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
SYSCALL int cx_ecdh(
    cx_ecfp_private_key_t WIDE *key PLENGTH(scc__cx_ecfp_private_key_scc__key),
    int mode, unsigned char WIDE *public_point PLENGTH(1 + 32 + 32),
    unsigned char *secret PLENGTH(1 + 32 + 32));

/* ======================================================================= */
/*                                    CRC                                */
/* ======================================================================= */

/**
 * Compute a 16 bits checksum value.
 * The 16 bits value is computed according to the CRC16 CCITT definition.
 *
 * @param [in] buffer
 *   The buffer to compute the crc over.
 *
 * @param [in]
 *   Bytes Length of the 'buffer'
 *
 * @return crc des_
 *
 */
SYSCALL unsigned short cx_crc16(void WIDE *buffer PLENGTH(len),
                                unsigned int len);

#define CX_CRC16_INIT 0xFFFF

SYSCALL unsigned short cx_crc16_update(unsigned short crc,
                                       void WIDE *buffer PLENGTH(len),
                                       unsigned int len);

/* ======================================================================= */
/*                                    MATH                                 */
/* ======================================================================= */

/**
 * Compare to unsigned long big-endian integer
 * The maximum length supported is 64.
 *
 * @param a    first operand
 * @param b    second operand
 * @param len  byte length of a, b
 *
 * @return 0 if a==b,  negative value if a<b, positive value if a>b
 */
SYSCALL int cx_math_cmp(unsigned char WIDE *a PLENGTH(len),
                        unsigned char WIDE *b PLENGTH(len), unsigned int len);

/**
 * Compare to unsigned long big-endian integer to zero
 *
 * @param a    value to compare to zero
 * @param len  byte length of a
 *
 * @return 1 if a==0,  0 else
 */
SYSCALL int cx_math_is_zero(unsigned char WIDE *a PLENGTH(len),
                            unsigned int len);

/**
 * Addition of two big integer: r = a+b
 *
 * @param r    where to put result
 * @param a    first operand
 * @param b    second operand
 * @param m    modulo
 * @param len  byte length of r, a, b, m
 *
 * @return carry
 */
SYSCALL int cx_math_add(unsigned char *r PLENGTH(len),
                        unsigned char WIDE *a PLENGTH(len),
                        unsigned char WIDE *b PLENGTH(len), unsigned int len);

/**
 * Subtraction of two big integer: r = a-b
 *
 * @param r    where to put result
 * @param a    first operand
 * @param b    second operand
 * @param m    modulo
 * @param len  byte length of r, a, b, m
 *
 * @return borrow
 */
SYSCALL int cx_math_sub(unsigned char *r PLENGTH(len),
                        unsigned char WIDE *a PLENGTH(len),
                        unsigned char WIDE *b PLENGTH(len), unsigned int len);

/**
 * Subtraction of two big integer: r = a-b
 *
 * @param r    where to put result, len*2 bytes
 * @param a    first operand, len bytes
 * @param b    second operand, len bytes
 * @param len  byte length base of a, b, r
 *
 */
SYSCALL void cx_math_mult(unsigned char *r PLENGTH(2 * len),
                          unsigned char WIDE *a PLENGTH(len),
                          unsigned char WIDE *b PLENGTH(len), unsigned int len);

/**
 * Modular addition of two big integer: r = a+b mod m
 *
 * @param r    where to put result
 * @param a    first operand
 * @param b    second operand
 * @param m    modulo
 * @param len  byte length of r, a, b, m
 *
 */
SYSCALL void cx_math_addm(unsigned char *r PLENGTH(len),
                          unsigned char WIDE *a PLENGTH(len),
                          unsigned char WIDE *b PLENGTH(len),
                          unsigned char WIDE *m PLENGTH(len), unsigned int len);
/**
 * Modular subtraction of tow big integer: r = a-b mod m
 *
 * @param r    where to put result
 * @param a    first operand
 * @param b    second operand
 * @param m    modulo
 * @param len  byte length of r, a, b, m
 *
 */
SYSCALL void cx_math_subm(unsigned char *r PLENGTH(len),
                          unsigned char WIDE *a PLENGTH(len),
                          unsigned char WIDE *b PLENGTH(len),
                          unsigned char WIDE *m PLENGTH(len), unsigned int len);
/**
 * Modular multiplication of tow big integer: r = a*b mod m
 *
 * @param r    where to put result
 * @param a    first operand
 * @param b    second operand
 * @param m    modulo
 * @param len  byte length of r, a, b, m
 *
 */
SYSCALL void cx_math_multm(unsigned char *r PLENGTH(len),
                           unsigned char WIDE *a PLENGTH(len),
                           unsigned char WIDE *b PLENGTH(len),
                           unsigned char WIDE *m PLENGTH(len),
                           unsigned int len);

/**
 * Modular exponentiation of tow big integer: r = a^^e mod m
 *
 * @param r     where to put result
 * @param a     first operand
 * @param e     second operand
 * @param len_e byte length of e
 * @param m     modulo
 * @param len   byte length of r, a, b, m
 *
 */
void cx_math_powm(unsigned char *r PLENGTH(len), unsigned char *a PLENGTH(len),
                  unsigned char WIDE *e PLENGTH(len_e), unsigned int len_e,
                  unsigned char WIDE *m PLENGTH(len), unsigned int len);

/**
 * Reduce in place (left zero padded) the given value: v = v mod m
 *
 * @param v        value to reduce
 * @param len_v    shall be >= len_m
 * @param m        modulus
 * @param len_m    length of modulus
 *
 */
SYSCALL void cx_math_modm(unsigned char *v PLENGTH(len_v), unsigned int len_v,
                          unsigned char WIDE *m PLENGTH(len_m),
                          unsigned int len_m);

/**
 * Modular prime inversion: r = a????? mod m, with m prime
 *
 * @param r     where to put result
 * @param a        value to invert
 * @param m        modulus
 * @param len   length of r,a,m
 *
 */
SYSCALL void cx_math_invprimem(unsigned char *r PLENGTH(len),
                               unsigned char *a PLENGTH(len),
                               unsigned char WIDE *m PLENGTH(len),
                               unsigned int len);

/**
 * Modular integer inversion: r = a????? mod m, with a 32 bits
 *
 * @param r     where to put result
 * @param a     value to invert
 * @param m     modulus
 * @param len   length of r,m
 *
 */
SYSCALL void cx_math_invintm(unsigned char *r PLENGTH(len), unsigned long int a,
                             unsigned char WIDE *m PLENGTH(len),
                             unsigned int len);

/**
 * Test if p is prime
 *
 * @param p     value to test
 * @param len   length p
 */
SYSCALL int cx_math_is_prime(unsigned char *p PLENGTH(len), unsigned int len);

/**
 * Find in place the next prime number follwing n
 *
 * @param n     seed value for next prime
 * @param len   length n
 */
SYSCALL void cx_math_next_prime(unsigned char *n PLENGTH(len),
                                unsigned int len);

/* ======================================================================= */
/*                                    DEBUG                                */
/* ======================================================================= */
int cx_selftest(void);

/* ======================================================================= */
/*                            WORK PRIVATE BUFFER                          */
/* ======================================================================= */
/*
 * This functions is meant for applications to work in the crypto ram without
 * being able to use its content
 * This is to be removed when pbkdf2 and various hash would be able to work with
 * a NULL context.
 */
void *cx_shared_buffer(void);

#endif // CX_H
