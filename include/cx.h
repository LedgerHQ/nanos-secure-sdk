/*******************************************************************************
 *   Ledger Nano S - Secure firmware
 *   (c) 2016, 2017, 2018, 2019 Ledger
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
/** @defgroup CX Crypto
 *   @{
 */

/* ####################################################################### */
/*                                    OPTIONS                              */
/* ####################################################################### */

/* ####################################################################### */
/*                                  CHIP/LIB3rd                            */
/* ####################################################################### */

/* ####################################################################### */
/*                                  COMMON                                 */
/* ####################################################################### */
/** @defgroup CX_Common Crypto Commons
 *   @{
 */

#ifndef NATIVE_64BITS // NO 64BITS
/** 64bits types, native or by-hands, depending on target and/or compiler
 * support.
 * This type is defined here only because sha-3 struct used it INTENALLY.
 * It should never be directly used by other modules.
 */
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


/**
 * Some function take logical or of various flags. The follwing flags are globally defined:
 * @rststar
 *
 *  +------------+----------------------------+------------------------------------------------------------------+
 *  | bit pos    |  H constant                |   meanings                                                       |
 *  +============+============================+==================================================================+
 *  |  0         | - CX_LAST                  | last block                                                       |
 *  +------------+----------------------------+------------------------------------------------------------------+
 *  |  2:1       | - CX_ENCRYPT               |                                                                  |
 *  |            | - CX_DECRYPT               |                                                                  |
 *  |            | - CX_SIGN                  |                                                                  |
 *  |            | - CX_VERIFY                |                                                                  |
 *  +------------+----------------------------+------------------------------------------------------------------+
 *  |  5:3       | - CX_PAD_NONE              |                                                                  |
 *  |            | - CX_PAD_ISO9797M1         |                                                                  |
 *  |            | - CX_PAD_ISO9797M2         |                                                                  |
 *  |            | - CX_PAD_PKCS1_1o5         |                                                                  |
 *  |            | - CX_PAD_PKCS1_PSS         |                                                                  |
 *  |            | - CX_PAD_PKCS1_OAEP        |                                                                  |
 *  +------------+----------------------------+------------------------------------------------------------------+
 *  |  8:6       | - CX_CHAIN_ECB             |                                                                  |
 *  |            | - CX_CHAIN_CBC             |                                                                  |
 *  |  -DES      | - CX_CHAIN_CTR             |                                                                  |
 *  |  -AES      | - CX_CHAIN_CFB             |                                                                  |
 *  |            | - CX_CHAIN_OFB             |                                                                  |
 *  +------------+----------------------------+------------------------------------------------------------------+
 *  |  8:6       | - CX_NO_CANONICAL          | do not perform canonical sig                                                                 |
 *  |            |                            |                                                                  |
 *  | -ECDSA     |                            |                                                                  |
 *  | -EDDSA     |                            |                                                                  |
 *  | -ECSCHNORR |                            |                                                                  |
 *  +------------+----------------------------+------------------------------------------------------------------+
 *  |  11:9      | - CX_RND_TRNG              |                                                                  |
 *  |            | - CX_RND_PRNG              |                                                                  |
 *  |            | - CX_RND_RFC6979           |                                                                  |
 *  |            | - CX_RND_PROVIDED          |                                                                  |
 *  +------------+----------------------------+------------------------------------------------------------------+
 *  |  14:12     | - CX_ECDH_POINT            | share full point                                                 |
 *  |            | - CX_ECDH_X                | share only x coordinate                                          |
 *  |            | - CX_ECSCHNORR_BSI03111    |                                                                  |
 *  |            | - CX_ECSCHNORR_ISO14888_XY |                                                                  |
 *  |            | - CX_ECSCHNORR_ISO14888_X  |                                                                  |
 *  |            |  -CX_ECSCHNORR_LIBSECP     |                                                                  |
 *  |            |  -CX_ECSCHNORR_Z           | Zilliqa scheme                                                   |
 *  +------------+----------------------------+------------------------------------------------------------------+
 *  |  15        | CX_NO_REINIT               | do not reinitialize context on CX_LAST when supported            |
 *  +------------+----------------------------+------------------------------------------------------------------+
 *
 * @endrststar
 */
#define CX_FLAG

/*
 * Bit 0
 */
#define CX_LAST (1 << 0)

/*
 * Bit 1
 */
#define CX_SIG_MODE (1 << 1)

/*
 * Bit 2:1
 */
#define CX_MASK_SIGCRYPT (3 << 1)
#define CX_ENCRYPT (2 << 1)
#define CX_DECRYPT (0 << 1)
#define CX_SIGN (CX_SIG_MODE | CX_ENCRYPT)
#define CX_VERIFY (CX_SIG_MODE | CX_DECRYPT)

/*
 * Bit 5:3: padding
 */
#define CX_MASK_PAD (7 << 3)
#define CX_PAD_NONE (0 << 3)
#define CX_PAD_ISO9797M1 (1 << 3)
#define CX_PAD_ISO9797M2 (2 << 3)
#define CX_PAD_PKCS1_1o5 (3 << 3)
#define CX_PAD_PKCS1_PSS (4 << 3)
#define CX_PAD_PKCS1_OAEP (5 << 3)

/*
 * Bit 8:6 DES/AES chaining
 */
#define CX_MASK_CHAIN (7 << 6)
#define CX_CHAIN_ECB (0 << 6)
#define CX_CHAIN_CBC (1 << 6)
#define CX_CHAIN_CTR (2 << 6)
#define CX_CHAIN_CFB (3 << 6)
#define CX_CHAIN_OFB (4 << 6)

/*
 * Bit 8:6 ECC variant
 */
#define CX_MASK_ECC_VARIANT (7 << 6)
#define CX_NO_CANONICAL (1 << 6)

/*
 * Bit 11:9
 */
#define CX_MASK_RND (7 << 9)
#define CX_RND_PRNG (1 << 9)
#define CX_RND_TRNG (2 << 9)
#define CX_RND_RFC6979 (3 << 9)
#define CX_RND_PROVIDED (4 << 9)

/*
 * Bit 14:12
 */
#define CX_MASK_EC (7 << 12)
#define CX_ECDH_POINT (1 << 12)
#define CX_ECDH_X (2 << 12)
#define CX_ECSCHNORR_ISO14888_XY (3 << 12)
#define CX_ECSCHNORR_ISO14888_X (4 << 12)
#define CX_ECSCHNORR_BSI03111 (5 << 12)
#define CX_ECSCHNORR_LIBSECP (6 << 12)
#define CX_ECSCHNORR_Z (7 << 12)
/*
 * Bit 15
 */
#define CX_NO_REINIT (1 << 15)

/** @} */ //@defgroup CX_Common

/* ####################################################################### */
/*                                   RAND                                  */
/* ####################################################################### */
/** @defgroup CX_rand Random
 * Random number generation APIs
 * @{
 */

/**
 * Generate a random unsigned byte between 0 and 255
 *
 * @return a random 8bits
 */
SYSCALL unsigned char cx_rng_u8(void);

/**
 * Generate a random buffer, each bytes between 0 and 255*
 *
 * @param [out] buffer to randomize
 * @param [in]  buffer length
 */
SYSCALL unsigned char *cx_rng(unsigned char *buffer PLENGTH(len),
                              unsigned int len);

/**
 * Generate determnist random number according to RFC6979.
 *
 * If x is not NULL, this is the first call. The function fill rnd with
 * canditate and V with initial chaining value.
 *
 * If x is NULL, this is subsequent call. rnd and V shall contain previous
 * returned values.
 *
 * @param [inout] rnd       generated random value,
 * @param [in]    hashID    hash to used. Only SHA256 and SH512 are supported
 * @param [in]    h1        initial hash (first call) or NULL
 * @param [in]    x         secret or NULL
 * @param [in]    x_len     secret length
 * @param [in]    q         maximum random value, i.e rnd < q
 * @param [in]    q_len     q length
 * @param [inout] V         Chaining vector
 * @param [in]    V_len     V length, WARNING: SHALL be hash length plus one
 * byte
 *
 * @return length of returned V
 */
SYSCALL int
cx_rng_rfc6979(unsigned char *rnd PLENGTH(rnd_len), unsigned int rnd_len,
               unsigned int hashID, const unsigned char *h1 PLENGTH(h1_len),
               unsigned int h1_len, const unsigned char *x PLENGTH(x_len),
               unsigned int x_len, const unsigned char *q PLENGTH(q_len),
               unsigned int q_len, unsigned char *V PLENGTH(V_len),
               unsigned int V_len);

/** @} */

/* ####################################################################### */
/*                                 HASH/HMAC                               */
/* ####################################################################### */

/* ======================================================================= */
/*                                   HASH                                 */
/* ======================================================================= */
/** @defgroup CX_hash Message Disgest
 * @{
 */

/** Message Disgest algorithm identifiers. */
enum cx_md_e {
    /** NONE Digest */
    CX_NONE,
    /** RIPEMD160 Digest */
    CX_RIPEMD160, // 20 bytes
    /** SHA224 Digest */
    CX_SHA224, // 28 bytes
    /** SHA256 Digest */
    CX_SHA256, // 32 bytes
    /** SHA384 Digest */
    CX_SHA384, // 48 bytes
    /** SHA512 Digest */
    CX_SHA512, // 64 bytes
    /** KECCAK (pre-SHA3) Digest */
    CX_KECCAK, // 28,32,48,64 bytes
    /** SHA3 Digest */
    CX_SHA3, // 28,32,48,64 bytes
    /** SHA3-XOF  Digest */
    CX_SHA3_XOF, // any bytes
    /** */
    CX_GROESTL,
    /** */
    CX_BLAKE2B,
};
/** Convenience type. See #cx_md_e. */
typedef enum cx_md_e cx_md_t;

/**
 * @internal
 * Maximum size of message for any digest. The size is given in block,
 */
#define CX_HASH_MAX_BLOCK_COUNT 65535

/**
 * Common Message Digest context, used as abstract type.
 */
struct cx_hash_header_s {
    /** Message digest identifier, See cx_md_e. */
    cx_md_t algo;
    /** Number of block already processed */
    unsigned int counter;
};
/** Convenience type. See #cx_hash_header_s. */
typedef struct cx_hash_header_s cx_hash_t;

/**
 * Add more data to hash.
 *
 * @param  [in/out] hash
 *   Univers Continuation Blob.
 *   The hash context pointer shall point to  either a cx_ripemd160_t, either a
 * cx_sha256_t  or cx_sha512_t . The hash context shall be inited with
 * 'cx_xxx_init' The hash context shall be in RAM The function should be called
 * with a nice cast.
 *
 * @note: 'out' length is implicit, no check is done
 *
 * @param  [in] mode
 *   Supported flags: CX_LAST
 *   If CX_LAST is set,
 *     - hash can be found in cx_ripemd160|sha1|sha256|sha512_t.acc field
 *     - if  out is provided, hash is copied in out
 *     - context is NOT automatically re-inited.
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
 *
 */
SYSCALL int cx_hash(cx_hash_t *hash PLENGTH(scc__cx_scc_struct_size_hash__hash),
                    int mode, const unsigned char WIDE *in PLENGTH(len),
                    unsigned int len, unsigned char *out PLENGTH(out_len),
                    unsigned int out_len);

/* ------------------------------ RIPEMD160 ------------------------------ */

/** RIPEMD160 message digest size */
#define CX_RIPEMD160_SIZE 20

/**
 * RIPEMD160 context
 */
struct cx_ripemd160_s {
    /** See #cx_hash_header_s */
    struct cx_hash_header_s header;
    /** @internal
     * pending partial block length
     */
    unsigned int blen;
    /** @internal
     * pending partial block
     */
    unsigned char block[64];
    /** Current digest state.
     * After finishing the digest, contains the digest if correct parameters are
     * passed.
     */
    unsigned char acc[5 * 4];
};
/** Convenience type. See #cx_ripemd160_s. */
typedef struct cx_ripemd160_s cx_ripemd160_t;

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

/* --------------------------------- SHA2 -------------------------------- */

/** SHA224 message digest size */
#define CX_SHA224_SIZE 28
/** SHA256 message digest size */
#define CX_SHA256_SIZE 32
/** SHA384 message digest size */
#define CX_SHA384_SIZE 48
/** SHA512 message digest size */
#define CX_SHA512_SIZE 64

/**
 * SHA224 and SHA256 context
 */
struct cx_sha256_s {
    /** @copydoc cx_ripemd160_s::header */
    struct cx_hash_header_s header;
    /** @internal @copydoc cx_ripemd160_s::blen */
    int blen;
    /** @internal @copydoc cx_ripemd160_s::block */
    unsigned char block[64];
    /** @copydoc cx_ripemd160_s::acc */
    unsigned char acc[8 * 4];
};
/** Convenience type. See #cx_sha256_s. */
typedef struct cx_sha256_s cx_sha256_t;

/**
 * SHA384 and SHA512 context
 */
struct cx_sha512_s {
    /** @copydoc cx_ripemd160_s::header */
    struct cx_hash_header_s header;
    /** @internal @copydoc cx_ripemd160_s::blen */
    unsigned int blen;
    /** @internal @copydoc cx_ripemd160_s::block */
    unsigned char block[128];
    /** @copydoc cx_ripemd160_s::acc */
    unsigned char acc[8 * 8];
};
/** Convenience type. See #cx_sha512_s. */
typedef struct cx_sha512_s cx_sha512_t;

/**
 * Init a sha256 context.
 *
 * @param [out] hash the context to init.
 *    The context shall be in RAM
 *
 * @return algorithm identifier
 */
SYSCALL int cx_sha224_init(cx_sha256_t *hash PLENGTH(sizeof(cx_sha256_t)));

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
SYSCALL int cx_hash_sha256(const unsigned char WIDE *in PLENGTH(len),
                           unsigned int len,
                           unsigned char *out PLENGTH(out_len),
                           unsigned int out_len);

/**
 * Init a sha512 context.
 *
 * @param [out] hash the context to init.
 *    The context shall be in RAM
 *
 * @return algorithm identifier
 */
SYSCALL int cx_sha384_init(cx_sha512_t *hash PLENGTH(sizeof(cx_sha512_t)));

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
SYSCALL int cx_hash_sha512(const unsigned char WIDE *in PLENGTH(len),
                           unsigned int len,
                           unsigned char *out PLENGTH(out_len),
                           unsigned int out_len);

/* ----------------------------- SHA3/KECCAK ----------------------------- */

/**
 * KECCAK, SHA3 and SHA3-XOF context
 */
struct cx_sha3_s {
    /** @copydoc cx_ripemd160_s::header */
    struct cx_hash_header_s header;

    /** @internal output digest size*/
    unsigned int output_size;
    /** @internal input block size*/
    unsigned int block_size;
    /** @internal @copydoc cx_ripemd160_s::blen */
    unsigned int blen;
    /** @internal @copydoc cx_ripemd160_s::block */
    unsigned char block[200];
    /** @copydoc cx_ripemd160_s::acc */
    uint64bits_t acc[25];
};
/** Convenience type. See #cx_sha3_s. */
typedef struct cx_sha3_s cx_sha3_t;

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
SYSCALL int cx_sha3_init(cx_sha3_t *hash PLENGTH(sizeof(cx_sha3_t)),
                         unsigned int size);

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
                           unsigned int size);

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

/* ------------------------------- GROESTL ------------------------------- */

#define ROWS 8
#define COLS1024 16
#define SIZE1024 (ROWS * COLS1024)
typedef unsigned char BitSequence;
/**  @Private */
struct hashState_s {
    uint8_t chaining[ROWS][COLS1024]; /* the actual state */
    uint64_t block_counter;           /* block counter */
    unsigned int hashbitlen;          /* output length */
    BitSequence buffer[SIZE1024];     /* block buffer */
    unsigned int buf_ptr;             /* buffer pointer */
    unsigned int bits_in_last_byte;   /* number of bits in incomplete byte */
    unsigned int columns;             /* number of columns in state */
    unsigned int rounds;              /* number of rounds in P and Q */
    unsigned int statesize;           /* size of state (ROWS*columns) */
};
typedef struct hashState_s hashState;

struct cx_groestl_s {
    /** @copydoc cx_ripemd160_s::header */
    struct cx_hash_header_s header;
    /** @internal output digest size*/
    unsigned int output_size;

    struct hashState_s ctx;
};
/** Convenience type. See #cx_groestl512_s. */
typedef struct cx_groestl_s cx_groestl_t;

/**
 * Init a groestl224 context.
 *
 * @param [out] hash the context to init.
 *    The context shall be in RAM
 *
 * @return algorithm identifier
 */
SYSCALL int cx_groestl_init(cx_groestl_t *hash PLENGTH(sizeof(cx_groestl_t)),
                            unsigned int size);

/* ------------------------------- BLAKE2B ------------------------------- */

enum blake2b_constant {
    BLAKE2B_BLOCKBYTES = 128,
    BLAKE2B_OUTBYTES = 64,
    BLAKE2B_KEYBYTES = 64,
    BLAKE2B_SALTBYTES = 16,
    BLAKE2B_PERSONALBYTES = 16
};

/**  @private */
struct blake2b_state__ {
    uint64_t h[8];
    uint64_t t[2];
    uint64_t f[2];
    uint8_t buf[BLAKE2B_BLOCKBYTES];
    size_t buflen;
    size_t outlen;
    uint8_t last_node;
};
typedef struct blake2b_state__ blake2b_state;

struct cx_blake2b_s {
    /** @copydoc cx_ripemd160_s::header */
    struct cx_hash_header_s header;
    /** @internal output digest size*/
    unsigned int output_size;
    struct blake2b_state__ ctx;
};
/** Convenience type. See #cx_blake2b_s. */
typedef struct cx_blake2b_s cx_blake2b_t;

/**
 * Init a blake2b context.
 *
 * Blake2b as specified at https://blake2.net.
 *
 * @param [out] hash  the context to init.
 *    The context shall be in RAM
 *
 * @param [in] size   output blake2b size, in BITS.
 *
 *
 * @return algorithm identifier
 */
SYSCALL int cx_blake2b_init(cx_blake2b_t *hash PLENGTH(sizeof(cx_blake2b_t)),
                            unsigned int out_len);

SYSCALL int cx_blake2b_init2(cx_blake2b_t *hash PLENGTH(sizeof(cx_blake2b_t)),
                             unsigned int out_len,
                             unsigned char *salt PLENGTH(salt_len),
                             unsigned int salt_len,
                             unsigned char *perso PLENGTH(perso_len),
                             unsigned int perso_len);

/** @} */

/* ======================================================================= */
/*                                 HASH MAC                                */
/* ======================================================================= */
/** @defgroup CX_hmac Message Hash Mac
 * @{
 */

/*Note: DO NOT reorder the following structures, it will break magic casts */

/**
 * RIPEMD160 hmac context
 */
struct cx_hmac_ripemd160_s {
    /** @internal */
    struct cx_ripemd160_s hash;
    /** hmac key len, 64 bytes max. */
    unsigned char key_len;
    /** hmac key */
    unsigned char key[64];
};
/** Convenience type. See #cx_hmac_ripemd160_s. */
typedef struct cx_hmac_ripemd160_s cx_hmac_ripemd160_t;

/**
 * SHA256 hmac context
 */
struct cx_hmac_sha256_s {
    /** @internal */
    struct cx_sha256_s hash;
    /** hmac key len, 64 bytes max. */
    unsigned char key_len;
    /** hmac key */
    unsigned char key[64];
};
/** Convenience type. See #cx_hmac_sha256_s. */
typedef struct cx_hmac_sha256_s cx_hmac_sha256_t;

/**
 * SHA512 hmac context
 */
struct cx_hmac_sha512_s {
    /** @internal */
    struct cx_sha512_s hash;
    /** hmac key len, 128 bytes max. */
    unsigned char key_len;
    /** hmac key */
    unsigned char key[128];
};
/** Convenience type. See #cx_hmac_sha512_s. */
typedef struct cx_hmac_sha512_s cx_hmac_sha512_t;

/**
 * Common hmac context, used as abstract type.
 */
typedef struct cx_hash_header_s cx_hmac_t;

/**
 * Init a hmac sha512 context.
 *
 * @param  [out] hash        the context to init.
 *    The context shall be in RAM
 *
 * @param  [in] key         hmac key value
 *    Passing a NULL pointeur, will reinit the context with the previously set
 * key. If no key has already been set, passing NULL will lead into an undefined
 * behavior.
 *
 * @param  [in] key_len     hmac key length
 *    The key length shall be less than 64 bytes
 *
 * @return algorithm  identifier
 */
SYSCALL int cx_hmac_ripemd160_init(
    cx_hmac_ripemd160_t *hmac PLENGTH(sizeof(cx_hmac_ripemd160_t)),
    const unsigned char WIDE *key PLENGTH(key_len), unsigned int key_len);

/**
 * Init a hmac sha256 context.
 *
 * @param [out] hash        the context to init.
 *    The context shall be in RAM
 *
 * @param [in] key         hmac key value
 *    Passing a NULL pointeur, will reinit the context with the previously set
 * key. If no key has already been set, passing NULL will lead into an undefined
 * behavior.
 *
 * @param [in] key_len     hmac key length
 *    The key length shall be less than 64 bytes
 *
 * @return algorithm  identifier
 */
SYSCALL int
cx_hmac_sha256_init(cx_hmac_sha256_t *hmac PLENGTH(sizeof(cx_hmac_sha256_t)),
                    const unsigned char WIDE *key PLENGTH(key_len),
                    unsigned int key_len);

/**
 * Init a hmac sha512 context.
 *
 * @param [out] hash        the context to init.
 *    The context shall be in RAM
 *
 * @param [in] key         hmac key value
 *    Passing a NULL pointeur, will reinit the context with the previously set
 * key. If no key has already been set, passing NULL will lead into an undefined
 * behavior.
 *
 * @param [in] key_len     hmac key length
 *    The key length shall be less than 128 bytes
 *
 * @return algorithm  identifier
 */
SYSCALL int
cx_hmac_sha512_init(cx_hmac_sha512_t *hmac PLENGTH(sizeof(cx_hmac_sha512_t)),
                    const unsigned char WIDE *key PLENGTH(key_len),
                    unsigned int key_len);

/**
 * @param [in,out] hmac
 *   Univers Continuation Blob.
 *   The hmac context pointer shall point to  either a cx_ripemd160_t, either a
 * cx_sha256_t  or cx_sha512_t . The hmac context shall be inited with
 * 'cx_xxx_init' The hmac context shall be in RAM The function should be called
 * with a nice cast.
 *
 * @param [in] mode
 *   Crypto mode flags. See Above.
 *   If CX_LAST is set and CX_NO_REINIT is not set, context is automatically
 * re-inited. Supported flags:
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
SYSCALL int cx_hmac(cx_hmac_t *hmac PLENGTH(scc__cx_scc_struct_size_hmac__hmac),
                    int mode, const unsigned char WIDE *in PLENGTH(len),
                    unsigned int len, unsigned char *mac PLENGTH(mac_len),
                    unsigned int mac_len);
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
SYSCALL int cx_hmac_sha512(const unsigned char WIDE *key PLENGTH(key_len),
                           unsigned int key_len,
                           const unsigned char WIDE *in PLENGTH(len),
                           unsigned int len,
                           unsigned char *mac PLENGTH(mac_len),
                           unsigned int mac_len);

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
SYSCALL int cx_hmac_sha256(const unsigned char WIDE *key PLENGTH(key_len),
                           unsigned int key_len,
                           const unsigned char WIDE *in PLENGTH(len),
                           unsigned int len,
                           unsigned char *mac PLENGTH(mac_len),
                           unsigned int mac_len);

/** @} */

/* ======================================================================= */
/*                                  PKDF2                                  */
/* ======================================================================= */
/** @defgroup CX_pbkdf2 PBKDF2
 * @{
 */

/**
 * Compute pbkdf2 bytes sequence as specified by RFC 2898.
 * The undelying hash function is SHA512
 *
 * @param [in]  password
 *    The hmac key
 * @param [in]  passwordlen
 *    The hmac key bytes length
 * @param [in]  salt
 *    The initial salt.
 *    The last four bytes muste be zero and are modified and internally used
 *    by the function.
 * @param [in]  saltlen
 *    The salt key bytes length, last four zero bytes included.
 * @param [in]  iterations
 *    Per block iteration.
 * @param [in]  out
 *    Where to put result.
 * @param [in] outLength
 *    How many bytes to generate.
 *
 */
SYSCALL void
cx_pbkdf2_sha512(const unsigned char WIDE *password PLENGTH(passwordlen),
                 unsigned short passwordlen,
                 unsigned char *salt PLENGTH(saltlen), unsigned short saltlen,
                 unsigned int iterations, unsigned char *out PLENGTH(outLength),
                 unsigned int outLength);

/** @} */

/* ####################################################################### */
/*                               CIPHER/SIGNATURE                          */
/* ####################################################################### */

/** @defgroup CX_sym Symetric Cryptography
 *  @{
 */

/* ======================================================================= */
/*                                   DES                                   */
/* ======================================================================= */
/** @defgroup CX_des DES
 *  @{
 */

/** @internal */
#define CX_DES_BLOCK_SIZE 8

/** DES key container.
 *  Such container should be initialize with cx_des_init_key to ensure future
 * API compatibility. Indeed, in next API level, the key store format may
 * changed at all. Only 8 bytes (simple DES) and 16 bytes (triple DES with 2
 * keys) are supported.
 */
struct cx_des_key_s {
    /** key size */
    unsigned char size;
    /** key value */
    unsigned char keys[16];
};
/** Convenience type. See #cx_des_key_s. */
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
SYSCALL int cx_des_init_key(const unsigned char WIDE *rawkey PLENGTH(key_len),
                            unsigned int key_len,
                            cx_des_key_t *key PLENGTH(sizeof(cx_des_key_t)));

/**
 * Encrypt, Decrypt, Sign or Verify data with DES algorithm.
 *
 * @param [in] key
 *   A des key fully inited with 'cx_des_init_key'
 *
 * @param [in] mode
 *   Crypto mode flags. See above.
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

SYSCALL CXPORT(CXPORT_ED_DES) int cx_des_iv(
    const cx_des_key_t WIDE *key PLENGTH(sizeof(cx_des_key_t)), int mode,
    const unsigned char WIDE *iv PLENGTH(iv_len), unsigned int iv_len,
    const unsigned char WIDE *in PLENGTH(in_len), unsigned int in_len,
    unsigned char *out PLENGTH(out_len), unsigned int out_len);

/**
 *  Same as cx_des_iv with initial IV assumed to be heigt zeros.
 */
SYSCALL CXPORT(CXPORT_ED_DES) int cx_des(
    const cx_des_key_t WIDE *key PLENGTH(sizeof(cx_des_key_t)), int mode,
    const unsigned char WIDE *in PLENGTH(in_len), unsigned int in_len,
    unsigned char *out PLENGTH(out_len), unsigned int out_len);

/** @} */ // CX_des

/* ======================================================================= */
/*                                   AES                                   */
/* ======================================================================= */
/** @defgroup CX_aes AES
 *  @{
 */

/** @internal */
#define CX_AES_BLOCK_SIZE 16

/** DES key container.
 *  Such container should be initialize with cx_des_init_key to ensure future
 * API compatibility. Indeed, in next API level, the key store format may
 * changed at all. Only 16 bytes key (AES128) are supported .
 */
struct cx_aes_key_s {
    /** key size */
    unsigned int size;
    /** key value */
    unsigned char keys[32];
};
/** Convenience type. See #cx_aes_key_s. */
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
SYSCALL int cx_aes_init_key(const unsigned char WIDE *rawkey PLENGTH(key_len),
                            unsigned int key_len,
                            cx_aes_key_t *key PLENGTH(sizeof(cx_aes_key_t)));

/**
 * Encrypt, Decrypt, Sign or Verify data with AES algorithm.
 *
 * @param [in] key
 *   A aes key fully inited with 'cx_aes_init_key'
 *
 * @param [in] mode
 *   Crypto mode flags. See above.
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
SYSCALL CXPORT(CXPORT_ED_AES) int cx_aes_iv(
    const cx_aes_key_t WIDE *key PLENGTH(sizeof(cx_aes_key_t)), int mode,
    const unsigned char WIDE *iv PLENGTH(iv_len), unsigned int iv_len,
    const unsigned char WIDE *in PLENGTH(in_len), unsigned int in_len,
    unsigned char *out PLENGTH(out_len), unsigned int out_len);

/**
 *  Same as cx_aes_iv with initial IV assumed to be sixteen zeros.
 */
SYSCALL CXPORT(CXPORT_ED_AES) int cx_aes(
    const cx_aes_key_t WIDE *key PLENGTH(sizeof(cx_aes_key_t)), int mode,
    const unsigned char WIDE *in PLENGTH(in_len), unsigned int in_len,
    unsigned char *out PLENGTH(out_len), unsigned int out_len);
/** @} */ // cx_aes

/** @} */ // cx_sym

/** @defgroup CX_asym Asymetric Cryptography
 * @{
 */

/* ======================================================================= */
/*                                     RSA                                 */
/* ======================================================================= */
/** @defgroup CX_rsa RSA Asymetric Cryptography
 * @{
 */

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
SYSCALL int cx_rsa_init_private_key(
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
SYSCALL int cx_rsa_generate_pair(
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
SYSCALL int cx_rsa_sign(const cx_rsa_private_key_t WIDE *key
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
SYSCALL int cx_rsa_verify(const cx_rsa_public_key_t WIDE *key
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
SYSCALL CXPORT(CXPORT_ED_RSA) int cx_rsa_encrypt(
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
SYSCALL CXPORT(CXPORT_ED_RSA) int cx_rsa_decrypt(
    const cx_rsa_private_key_t WIDE *key
        PLENGTH(scc__cx_scc_struct_size_rsa_privkey__key),
    int mode, cx_md_t hashID, const unsigned char WIDE *mesg PLENGTH(mesg_len),
    unsigned int mesg_len, unsigned char WIDE *dec PLENGTH(dec_len),
    unsigned int dec_len);

/** @} */ // cx_rsa
/* ======================================================================= */
/*                                     ECC                                 */
/* ======================================================================= */
/** @defgroup CX_ecc Elliptic curve Cryptography
 * Only curved defined at compiled time will be fully supported
 * @{
 */

/**
 *
 */
#define CX_ECCINFO_PARITY_ODD 1
#define CX_ECCINFO_xGTn 2

/** List of supported elliptic curves */
enum cx_curve_e {
    CX_CURVE_NONE,
    /* ------------------------ */
    /* --- Type Weierstrass --- */
    /* ------------------------ */
    /** Low limit (not included) of Weierstrass curve ID */
    CX_CURVE_WEIERSTRASS_START = 0x20,

    /** Secp.org */
    CX_CURVE_SECP256K1,
    CX_CURVE_SECP256R1,
#define CX_CURVE_256K1 CX_CURVE_SECP256K1
#define CX_CURVE_256R1 CX_CURVE_SECP256R1
    CX_CURVE_SECP384R1,
    CX_CURVE_SECP521R1,

    /** BrainPool */
    CX_CURVE_BrainPoolP256T1,
    CX_CURVE_BrainPoolP256R1,
    CX_CURVE_BrainPoolP320T1,
    CX_CURVE_BrainPoolP320R1,
    CX_CURVE_BrainPoolP384T1,
    CX_CURVE_BrainPoolP384R1,
    CX_CURVE_BrainPoolP512T1,
    CX_CURVE_BrainPoolP512R1,

/* NIST P256 curve*/
#define CX_CURVE_NISTP256 CX_CURVE_SECP256R1
#define CX_CURVE_NISTP384 CX_CURVE_SECP384R1
#define CX_CURVE_NISTP521 CX_CURVE_SECP521R1

    /* ANSSI P256 */
    CX_CURVE_FRP256V1,

    /** High limit (not included) of Weierstrass curve ID */
    CX_CURVE_WEIERSTRASS_END,

    /* --------------------------- */
    /* --- Type Twister Edward --- */
    /* --------------------------- */
    /** Low limit (not included) of  Twister Edward curve ID */
    CX_CURVE_TWISTED_EDWARD_START = 0x40,

    /** Ed25519 curve */
    CX_CURVE_Ed25519,
    CX_CURVE_Ed448,

    CX_CURVE_TWISTED_EDWARD_END,
    /** High limit (not included) of Twister Edward  curve ID */

    /* ----------------------- */
    /* --- Type Montgomery --- */
    /* ----------------------- */
    /** Low limit (not included) of Montgomery curve ID */
    CX_CURVE_MONTGOMERY_START = 0x60,

    /** Curve25519 curve */
    CX_CURVE_Curve25519,
    CX_CURVE_Curve448,

    CX_CURVE_MONTGOMERY_END
    /** High limit (not included) of Montgomery curve ID */
};
/** Convenience type. See #cx_curve_e. */
typedef enum cx_curve_e cx_curve_t;

/** Return true if curve type is short weierstrass curve */
#define CX_CURVE_IS_WEIRSTRASS(c)                                              \
    (((c) > CX_CURVE_WEIERSTRASS_START) && ((c) < CX_CURVE_WEIERSTRASS_END))

/** Return true if curve type is short weierstrass curve */
#define CX_CURVE_IS_TWISTED_EDWARD(c)                                          \
    (((c) > CX_CURVE_TWISTED_EDWARD_START) &&                                  \
     ((c) < CX_CURVE_TWISTED_EDWARD_END))

/** Return true if curve type is short weierstrass curve */
#define CX_CURVE_IS_MONTGOMERY(c)                                              \
    (((c) > CX_CURVE_MONTGOMERY_START) && ((c) < CX_CURVE_MONTGOMERY_END))

#define CX_CURVE_HEADER                                                        \
    /** Curve Identifier. See #cx_curve_e */                                   \
    cx_curve_t curve;                                                          \
    /** Curve size in bits */                                                  \
    unsigned int bit_size;                                                     \
    /** component lenth in bytes */                                            \
    unsigned int length;                                                       \
    /** Curve field */                                                         \
    unsigned char WIDE *p;                                                     \
    /** @internal 2nd Mongtomery constant for Field */                         \
    unsigned char WIDE *Hp;                                                    \
    /** Point Generator x coordinate*/                                         \
    unsigned char WIDE *Gx;                                                    \
    /** Point Generator y coordinate*/                                         \
    unsigned char WIDE *Gy;                                                    \
    /** Curve order*/                                                          \
    unsigned char WIDE *n;                                                     \
    /** @internal 2nd Mongtomery constant for Curve order*/                    \
    unsigned char WIDE *Hn;                                                    \
    /**  cofactor */                                                           \
    int h

/**
 * Weirstrass curve :     y^3=x^2+a*x+b        over F(p)
 *
 */
struct cx_curve_weierstrass_s {
    CX_CURVE_HEADER;
    /**  a coef */
    unsigned char WIDE *a;
    /**  b coef */
    unsigned char WIDE *b;
};
/** Convenience type. See #cx_curve_weierstrass_s. */
typedef struct cx_curve_weierstrass_s cx_curve_weierstrass_t;

/*
 * Twisted Edward curve : a*x^2+y^2=1+d*x2*y2  over F(q)
 */
struct cx_curve_twisted_edward_s {
    CX_CURVE_HEADER;
    /**  a coef */
    unsigned char WIDE *a;
    /**  d coef */
    unsigned char WIDE *d;
    /** @internal Square root of -1 or zero */
    unsigned char WIDE *I;
    /** @internal  (q+3)/8 or (q+1)/4*/
    unsigned char WIDE *Qq;
};
/** Convenience type. See #cx_curve_twisted_edward_s. */
typedef struct cx_curve_twisted_edward_s cx_curve_twisted_edward_t;

/*
 * Twisted Edward curve : a*x??+y??=1+d*x??*y??  over F(q)
 */
struct cx_curve_montgomery_s {
    CX_CURVE_HEADER;
    /**  a coef */
    unsigned char WIDE *a;
    /**  b coef */
    unsigned char WIDE *b;
    /** @internal (a + 2) / 4*/
    unsigned char WIDE *A24;
    /** @internal  (p-1)/2 */
    unsigned char WIDE *P1;
};
/** Convenience type. See #cx_curve_montgomery_s. */
typedef struct cx_curve_montgomery_s cx_curve_montgomery_t;

/** Abstract type for elliptic curve domain */
struct cx_curve_domain_s {
    CX_CURVE_HEADER;
};
/** Convenience type. See #cx_curve_domain_s. */
typedef struct cx_curve_domain_s cx_curve_domain_t;

/** Retrieve domain parameters
 *
 * @param curve curve ID #cx_curve_e
 *
 * @return curve parameters
 */
const cx_curve_domain_t WIDE *cx_ecfp_get_domain(cx_curve_t curve);

/** Public Elliptic Curve key */
struct cx_ecfp_public_key_s {
    /** curve ID #cx_curve_e */
    cx_curve_t curve;
    /** Public key length in bytes */
    unsigned int W_len;
    /** Public key value starting at offset 0 */
    unsigned char W[1];
};
/** Private Elliptic Curve key */
struct cx_ecfp_private_key_s {
    /** curve ID #cx_curve_e */
    cx_curve_t curve;
    /** Public key length in bytes */
    unsigned int d_len;
    /** Public key value starting at offset 0 */
    unsigned char d[1];
};
// temporary typedef for scc check
typedef struct cx_ecfp_private_key_s __cx_ecfp_private_key_t;
typedef struct cx_ecfp_public_key_s __cx_ecfp_public_key_t;

/** Up to 256 bits Public Elliptic Curve key */
struct cx_ecfp_256_public_key_s {
    /** curve ID #cx_curve_e */
    cx_curve_t curve;
    /** Public key length in bytes */
    unsigned int W_len;
    /** Public key value starting at offset 0 */
    unsigned char W[65];
};
/** Up to 256 bits Private Elliptic Curve key */
struct cx_ecfp_256_private_key_s {
    /** curve ID #cx_curve_e */
    cx_curve_t curve;
    /** Public key length in bytes */
    unsigned int d_len;
    /** Public key value starting at offset 0 */
    unsigned char d[32];
};
/** Up to 256 bits Extended Private Elliptic Curve key */
struct cx_ecfp_256_extended_private_key_s {
    /** curve ID #cx_curve_e */
    cx_curve_t curve;
    /** Public key length in bytes */
    unsigned int d_len;
    /** Public key value starting at offset 0 */
    unsigned char d[64];
};
/** Convenience type. See #cx_ecfp_256_public_key_s. */
typedef struct cx_ecfp_256_public_key_s cx_ecfp_256_public_key_t;
/** temporary def type. See #cx_ecfp_256_private_key_s. */
typedef struct cx_ecfp_256_private_key_s cx_ecfp_256_private_key_t;
/** Convenience type. See #cx_ecfp_256_extended_private_key_s. */
typedef struct cx_ecfp_256_extended_private_key_s
    cx_ecfp_256_extended_private_key_t;

/* Do not use those types anymore for declaration, they will become abstract */
typedef struct cx_ecfp_256_public_key_s cx_ecfp_public_key_t;
typedef struct cx_ecfp_256_private_key_s cx_ecfp_private_key_t;

/** Up to 384 bits Public Elliptic Curve key */
struct cx_ecfp_384_public_key_s {
    /** curve ID #cx_curve_e */
    cx_curve_t curve;
    /** Public key length in bytes */
    unsigned int W_len;
    /** Public key value starting at offset 0 */
    unsigned char W[97];
};
/** Up to 384 bits Private Elliptic Curve key */
struct cx_ecfp_384_private_key_s {
    /** curve ID #cx_curve_e */
    cx_curve_t curve;
    /** Public key length in bytes */
    unsigned int d_len;
    /** Public key value starting at offset 0 */
    unsigned char d[48];
};
/** Convenience type. See #cx_ecfp_384_public_key_s. */
typedef struct cx_ecfp_384_private_key_s cx_ecfp_384_private_key_t;
/** Convenience type. See #cx_ecfp_384_private_key_s. */
typedef struct cx_ecfp_384_public_key_s cx_ecfp_384_public_key_t;

/** Up to 512 bits Public Elliptic Curve key */
struct cx_ecfp_512_public_key_s {
    /** curve ID #cx_curve_e */
    cx_curve_t curve;
    /** Public key length in bytes */
    unsigned int W_len;
    /** Public key value starting at offset 0 */
    unsigned char W[129];
};
/** Up to 512 bits Private Elliptic Curve key */
struct cx_ecfp_512_private_key_s {
    /** curve ID #cx_curve_e */
    cx_curve_t curve;
    /** Public key length in bytes */
    unsigned int d_len;
    /** Public key value starting at offset 0 */
    unsigned char d[64];
};
/** Up to 512 bits Extended Private Elliptic Curve key */
struct cx_ecfp_512_extented_private_key_s {
    /** curve ID #cx_curve_e */
    cx_curve_t curve;
    /** Public key length in bytes */
    unsigned int d_len;
    /** Public key value starting at offset 0 */
    unsigned char d[128];
};
/** Convenience type. See #cx_ecfp_512_public_key_s. */
typedef struct cx_ecfp_512_public_key_s cx_ecfp_512_public_key_t;
/** Convenience type. See #cx_ecfp_512_private_key_s. */
typedef struct cx_ecfp_512_private_key_s cx_ecfp_512_private_key_t;
/** Convenience type. See #cx_ecfp_512_extented_private_key_s. */
typedef struct cx_ecfp_512_extented_private_key_s
    cx_ecfp_512_extented_private_key_t;

/** Up to 640 bits Public Elliptic Curve key */
struct cx_ecfp_640_public_key_s {
    /** curve ID #cx_curve_e */
    cx_curve_t curve;
    /** Public key length in bytes */
    unsigned int W_len;
    /** Public key value starting at offset 0 */
    unsigned char W[161];
};
/** Up to 640 bits Private Elliptic Curve key */
struct cx_ecfp_640_private_key_s {
    /** curve ID #cx_curve_e */
    cx_curve_t curve;
    /** Public key length in bytes */
    unsigned int d_len;
    /** Public key value starting at offset 0 */
    unsigned char d[80];
};
/** Convenience type. See #cx_ecfp_640_public_key_s. */
typedef struct cx_ecfp_640_public_key_s cx_ecfp_640_public_key_t;
/** Convenience type. See #cx_ecfp_640_private_key_s. */
typedef struct cx_ecfp_640_private_key_s cx_ecfp_640_private_key_t;

/**
 * Verify that a given point is really on the specified curve.
 *
 * @param [in] domain
 *   The curve domain parameters to work with.
 *
 * @param [in]  P
 *   The point to test  encoded as: 04 x y
 *
 * @return
 *    1 if point is on the curve
 *    0 if point is not on the curve
 *   -1 if undefined (function not impl)
 *
 * @throws INVALID_PARAMETER
 */
SYSCALL int cx_ecfp_is_valid_point(cx_curve_t curve,
                                   const unsigned char WIDE *P PLENGTH(P_len),
                                   unsigned int P_len);

/**
 * Verify that a given point is really on the specified curve and its order
 * is the curve order
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
SYSCALL int
cx_ecfp_is_cryptographic_point(cx_curve_t curve,
                               const unsigned char WIDE *P PLENGTH(P_len),
                               unsigned int P_len);

/**
 * Add two affine point
 *
 * This routine only support Weierstrass and Twisted edward curve.
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
 *   0 if result is infinity
 *
 * @throws INVALID_PARAMETER
 */
SYSCALL int cx_ecfp_add_point(cx_curve_t curve, unsigned char *R PLENGTH(X_len),
                              const unsigned char WIDE *P PLENGTH(X_len),
                              const unsigned char WIDE *Q PLENGTH(X_len),
                              unsigned int X_len);

/**
 * Multiply an affine point
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
 *   scalar to multiply, encoded as big endian integer
 *
 * @param [in] k_len
 *   byte length of scalar to multiply
 *
 * @return
 *   R encoding length, if mult success
 *   0 if result is infinity
 *
 * @throws INVALID_PARAMETER
 */
SYSCALL int cx_ecfp_scalar_mult(cx_curve_t curve,
                                unsigned char *P PLENGTH(P_len),
                                unsigned int P_len,
                                const unsigned char WIDE *k PLENGTH(k_len),
                                unsigned int k_len);

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
 *   The value shall be the public point encoded as:
 *     - '04 x y' for Weiertrass curve
 *     - '04 x y'  or '02 y' (plus sign) for twisted Edward curves
 *     - '04 x y'  or '02 x' for Montgomery curves
 *    where x and y are encoded as big endian raw value and have bits length
 *    equals to the curve size. Any specific integer decoding from binary,
 *    such as specified in RFC7748 and RFC eddsa-draft, is up to caller.
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
    cx_curve_t curve, const unsigned char WIDE *rawkey PLENGTH(key_len),
    unsigned int key_len,
    cx_ecfp_public_key_t *key
        PLENGTH(scc__cx_scc_struct_size_ecfp_pubkey_from_curve__curve));

/**
 * Initialize a private ECFP Key.
 * Once initialized, the key may be  stored in non-volatile memory
 * and reused 'as-is' for any ECDSA/EC25519 processing
 * Passing NULL as raw key initializes the key without value. The key may be
 * used as parameter for cx_ecfp_generate_pair.
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
 * @param [out] pvkey
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
    cx_curve_t curve, const unsigned char WIDE *rawkey PLENGTH(key_len),
    unsigned int key_len,
    cx_ecfp_private_key_t *pvkey
        PLENGTH(scc__cx_scc_struct_size_ecfp_privkey_from_curve__curve));

/**
 * Generate a ecfp key pair.
 * This function call cx_ecfp_generate_pair2 with hashID equals to CX_SHA512.
 *
 * @param [in] curve
 *   The curve domain parameters to work with.
 *
 * @param [out] pubkey
 *   A public ecfp public key to generate.
 *
 * @param [in,out] privkey
 *   A private ecfp private key to generate.
 *   Either:
 *     - if the private ecfp key is fully inited, i.e  parameter 'rawkey' of
 *       'cx_ecfp_init_private_key' is NOT null, the private key value is kept
 *       if the 'keep_private' parameter is non zero
 *     - else a new private key is generated.
 *
 * @param [in] keepprivate if set to non zero, keep the private key value if
 * set. Else generate a new random one
 *
 * @return zero
 *
 * @throws INVALID_PARAMETER
 */
SYSCALL int cx_ecfp_generate_pair(
    cx_curve_t curve,
    cx_ecfp_public_key_t *pubkey
        PLENGTH(scc__cx_scc_struct_size_ecfp_pubkey_from_curve__curve),
    cx_ecfp_private_key_t *privkey
        PLENGTH(scc__cx_scc_struct_size_ecfp_privkey_from_curve__curve),
    int keepprivate);

/**
 * Generate a ecfp key pair
 *
 * @param [in] curve
 *   The curve domain parameters to work with.
 *
 * @param [out] pubkey
 *   A public ecfp public key to generate.
 *
 * @param [in,out] privkey
 *   A private ecfp private key to generate.
 *   Either:
 *     - if the private ecfp key is fully inited, i.e  parameter 'rawkey' of
 *       'cx_ecfp_init_private_key' is NOT null, the private key value is kept
 *       if the 'keep_private' parameter is non zero
 *     - else a new private key is generated.
 *
 * @param [in] keepprivate if set to non zero, keep the private key value if
 * set. Else generate a new random one
 *
 *  @param [in] hashID Hash to use for eddsa (SHA512, SHA3 and KECCAK are
 * supported)
 *
 * @return zero
 *
 * @throws INVALID_PARAMETER
 */
SYSCALL int cx_ecfp_generate_pair2(
    cx_curve_t curve,
    cx_ecfp_public_key_t *pubkey
        PLENGTH(scc__cx_scc_struct_size_ecfp_pubkey_from_curve__curve),
    cx_ecfp_private_key_t *privkey
        PLENGTH(scc__cx_scc_struct_size_ecfp_privkey_from_curve__curve),
    int keepprivate, cx_md_t hashID);

/* ============================ ECSchnorr ================================== */
/**
 * Sign a hash message according to ECSchnorr specification (BSI TR 03111).
 *
 * @param [in] key
 *   A private ecfp key fully inited with 'cx_ecfp_init_private_key'
 *
 * @param [in] mode
 *   Crypto mode flags. See above.
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
 * @param [out] info
 *   Set to zero
 *
 * @return
 *   Full length of signature
 *
 * @throws INVALID_PARAMETER
 */
SYSCALL int cx_ecschnorr_sign(const cx_ecfp_private_key_t WIDE *pvkey PLENGTH(
                                  scc__cx_scc_struct_size_ecfp_privkey__pvkey),
                              int mode, cx_md_t hashID,
                              const unsigned char WIDE *msg PLENGTH(msg_len),
                              unsigned int msg_len,
                              unsigned char *sig PLENGTH(sig_len),
                              unsigned int sig_len,
                              unsigned int *info PLENGTH(sizeof(unsigned int)));

/**
 * Verify a hash message signature according to ECSchnorr specification (BSI TR
 * 03111).
 *
 * @param [in] key
 *   A public ecfp key fully inited with 'cx_ecfp_init_public_key'
 *
 * @param [in] mode
 *   Crypto mode flags. See above.
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
SYSCALL int cx_ecschnorr_verify(const cx_ecfp_public_key_t WIDE *pukey PLENGTH(
                                    scc__cx_scc_struct_size_ecfp_pubkey__pukey),
                                int mode, cx_md_t hashID,
                                const unsigned char WIDE *msg PLENGTH(msg_len),
                                unsigned int msg_len,
                                const unsigned char WIDE *sig PLENGTH(sig_len),
                                unsigned int sig_len);

/* ============================= EdDSA =================================== */

/**
 *  Compress point according to RFC8032.
 *
 * @param [in]     domain
 * @param [in,out] P
 */
SYSCALL void cx_edward_compress_point(cx_curve_t curve,
                                      unsigned char *P PLENGTH(P_len),
                                      unsigned int P_len);

/**
 *  Decompress point according to draft-irtf-cfrg-eddsa-05.
 *
 * @param [in]     domain
 * @param [in,out] P
 */
SYSCALL void cx_edward_decompress_point(cx_curve_t curve,
                                        unsigned char *P PLENGTH(P_len),
                                        unsigned int P_len);

/**
 *  Retrieve (a,h) = (Kr, Kl), such (Kr, Kl) = Hash(pv_key) as specified in
 * RFC8032
 *
 * @param [in] pv_key
 *   A private ecfp key fully inited with 'cx_ecfp_init_private_key'.
 *
 * @param [in] hashID
 *  Hash identifier used to compute the input data. SHA512, SHA3 and Keccak are
 * supported.
 *
 * @param [out] pu_key
 *   A public null-inited ecfp key container for retrieving public key A.
 *
 * @param [out] a
 *   private scalar such A = a.B
 *
 * @param [out] h
 *   prefix signature
 *
 * @param [in] hashID
 *  Hash identifier used to compute the input data. SHA512, SHA3 and Keccak are
 * supported.
 *
 */
SYSCALL void cx_eddsa_get_public_key(
    const cx_ecfp_private_key_t WIDE *pvkey
        PLENGTH(scc__cx_scc_struct_size_ecfp_privkey__pvkey),
    cx_md_t hashID,
    cx_ecfp_public_key_t *pukey
        PLENGTH(scc__cx_scc_struct_size_ecfp_pubkey_from_pvkey__pvkey),
    unsigned char *a PLENGTH(a_len), unsigned int a_len,
    unsigned char *h PLENGTH(h_len), unsigned int h_len);

/**
 * Sign a hash message according to EdDSA specification RFC8032.
 *
 * @param [in] pv_key
 *   A private ecfp key fully inited with 'cx_ecfp_init_private_key'.
 *
 *
 * @param [in] mode
 *   Crypto mode flags. See above.
 *   Supported flags:
 *      <none>
 *
 * @param [in] hashID
 *  Hash identifier used to compute the input data. SHA512, SHA3 and Keccak are
 * supported.
 *
 * @param [in] hash
 *   Input data to sign.
 *   The data should be the hash of the original message.
 *   The data length must be lesser than the curve size.
 *
 * @param [in] hash_len
 *   Length of input to data.
 *
 * @param [in] ctx
 *   UNUSED, SHALL BE NULL
 *
 * @param [in] ctx_len
 *   UNUSED, SHALL BE ZERO
 *
 * @param [out] sig
 *   EdDSA signature encoded as : R|S
 *
 * @param [out] info
 *   Set to zero
 *
 * @return
 *   Full length of signature
 *
 * @throws INVALID_PARAMETER
 */
SYSCALL int cx_eddsa_sign(const cx_ecfp_private_key_t WIDE *pvkey PLENGTH(
                              scc__cx_scc_struct_size_ecfp_privkey__pvkey),
                          int mode, cx_md_t hashID,
                          const unsigned char WIDE *hash PLENGTH(hash_len),
                          unsigned int hash_len,
                          const unsigned char WIDE *ctx PLENGTH(ctx_len),
                          unsigned int ctx_len,
                          unsigned char *sig PLENGTH(sig_len),
                          unsigned int sig_len, unsigned int *info);

/**
 * Verify a hash message signature according to EDDSA specification RFC8032.
 *
 * @param [in] key
 *   A public ecfp key fully inited with 'cx_ecfp_init_public_key'
 *
 * @param [in] mode
 *   Crypto mode flags. See above.
 *   Supported flags:
 *     - <none>
 *
 * @param [in] hashID
 *  Hash identifier used to compute the input data.  SHA512, SHA3 and Keccak are
 * supported.
 *
 * @param [in] hash
 *   Signed input data to verify the signature.
 *   The data should be the hash of the original message.
 *   The data length must be lesser than the curve size.
 *
 * @param [in] hash_len
 *   Length of input to data.
 *
 * @param [in] ctx
 *   UNUSED, SHALL BE NULL
 *
 * @param [in] ctx_len
 *   UNUSED, SHALL BE ZERO
 *
 * @param [in] sig
 *   EDDSA signature to verify encoded as : R|S
 *
 * @param [in] sig_len
 *   sig length in bytes
 *
 * @return
 *   1 if signature is verified
 *   0 is signarure is not verified
 *
 * @throws INVALID_PARAMETER
 */
SYSCALL int cx_eddsa_verify(const cx_ecfp_public_key_t WIDE *pukey PLENGTH(
                                scc__cx_scc_struct_size_ecfp_pubkey__pukey),
                            int mode, cx_md_t hashID,
                            const unsigned char WIDE *hash PLENGTH(hash_len),
                            unsigned int hash_len,
                            const unsigned char WIDE *ctx PLENGTH(ctx_len),
                            unsigned int ctx_len,
                            const unsigned char WIDE *sig PLENGTH(sig_len),
                            unsigned int sig_len);

/* ============================= ECDSA =================================== */

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
SYSCALL int cx_ecdsa_sign(const cx_ecfp_private_key_t WIDE *pvkey PLENGTH(
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
SYSCALL int cx_ecdsa_verify(const cx_ecfp_public_key_t WIDE *pukey PLENGTH(
                                scc__cx_scc_struct_size_ecfp_pubkey__pukey),
                            int mode, cx_md_t hashID,
                            const unsigned char WIDE *hash PLENGTH(hash_len),
                            unsigned int hash_len,
                            const unsigned char WIDE *sig PLENGTH(sig_len),
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
SYSCALL int cx_ecdh(const cx_ecfp_private_key_t WIDE *pvkey
                        PLENGTH(scc__cx_scc_struct_size_ecfp_privkey__pvkey),
                    int mode, const unsigned char WIDE *P PLENGTH(P_len),
                    unsigned int P_len,
                    unsigned char *secret PLENGTH(secret_len),
                    unsigned int secret_len);

/** @} */ //*cx_ecc */

/** @} */ // cx_asym

/* ======================================================================= */
/*                                    CRC                                */
/* ======================================================================= */
/** @defgroup CX_crc Message Checksum
 * @{
 */

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
 * @return current crc value
 *
 */
SYSCALL unsigned short cx_crc16(const void WIDE *buffer PLENGTH(len),
                                unsigned int len);

#define CX_CRC16_INIT 0xFFFF

/** Accumulate more data to crc */
SYSCALL unsigned short cx_crc16_update(unsigned short crc,
                                       const void WIDE *buffer PLENGTH(len),
                                       unsigned int len);

/** @} */

/* ======================================================================= */
/*                                    MATH                                 */
/* ======================================================================= */
/** @defgroup CX_math Mathematiczal operation
 * @{
 */

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
SYSCALL int cx_math_cmp(const unsigned char WIDE *a PLENGTH(len),
                        const unsigned char WIDE *b PLENGTH(len),
                        unsigned int len);

/**
 * Compare to unsigned long big-endian integer to zero
 *
 * @param a    value to compare to zero
 * @param len  byte length of a
 *
 * @return 1 if a==0,  0 else
 */
SYSCALL int cx_math_is_zero(const unsigned char WIDE *a PLENGTH(len),
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
                        const unsigned char WIDE *a PLENGTH(len),
                        const unsigned char WIDE *b PLENGTH(len),
                        unsigned int len);

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
                        const unsigned char WIDE *a PLENGTH(len),
                        const unsigned char WIDE *b PLENGTH(len),
                        unsigned int len);

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
                          const unsigned char WIDE *a PLENGTH(len),
                          const unsigned char WIDE *b PLENGTH(len),
                          unsigned int len);

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
                          const unsigned char WIDE *a PLENGTH(len),
                          const unsigned char WIDE *b PLENGTH(len),
                          const unsigned char WIDE *m PLENGTH(len),
                          unsigned int len);
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
                          const unsigned char WIDE *a PLENGTH(len),
                          const unsigned char WIDE *b PLENGTH(len),
                          const unsigned char WIDE *m PLENGTH(len),
                          unsigned int len);
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
                           const unsigned char WIDE *a PLENGTH(len),
                           const unsigned char WIDE *b PLENGTH(len),
                           const unsigned char WIDE *m PLENGTH(len),
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
SYSCALL void
cx_math_powm(unsigned char *r PLENGTH(len), const unsigned char *a PLENGTH(len),
             const unsigned char WIDE *e PLENGTH(len_e), unsigned int len_e,
             const unsigned char WIDE *m PLENGTH(len), unsigned int len);

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
                          const unsigned char WIDE *m PLENGTH(len_m),
                          unsigned int len_m);

/**
 * Modular prime inversion: r = (a^-1) mod m, with m prime
 *
 * @param r     where to put result
 * @param a        value to invert
 * @param m        modulus
 * @param len   length of r,a,m
 *
 */
SYSCALL void cx_math_invprimem(unsigned char *r PLENGTH(len),
                               const unsigned char *a PLENGTH(len),
                               const unsigned char WIDE *m PLENGTH(len),
                               unsigned int len);

/**
 * Modular integer inversion: r = (a^-1) mod m, with a 32 bits
 *
 * @param r     where to put result
 * @param a     value to invert
 * @param m     modulus
 * @param len   length of r,m
 *
 */
SYSCALL void cx_math_invintm(unsigned char *r PLENGTH(len), unsigned long int a,
                             const unsigned char WIDE *m PLENGTH(len),
                             unsigned int len);

/**
 * Test if p is prime
 *
 * @param p     value to test
 * @param len   length p
 */
SYSCALL int cx_math_is_prime(const unsigned char *p PLENGTH(len),
                             unsigned int len);

/**
 * Find in place the next prime number follwing n
 *
 * @param n     seed value for next prime
 * @param len   length n
 */
SYSCALL void cx_math_next_prime(unsigned char *n PLENGTH(len),
                                unsigned int len);

/** @} */

/* ======================================================================= */
/*                                    DEBUG                                */
/* ======================================================================= */

/** @internal
 *  @private
 */
int cx_selftest(void);

#endif // CX_H
#include "cx_compliance_141.h"
