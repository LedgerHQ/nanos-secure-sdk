
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
 * @file    lcx_common.h
 * @brief   Cryptography flags.
 *
 * Flags required by some functions.
 */

#ifndef LCX_COMMON_H
#define LCX_COMMON_H

#include <stdint.h>

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define ARCH_LITTLE_ENDIAN
#else
#define ARCH_BIG_ENDIAN
#endif

#ifdef __UINT64_TYPE__
#define NATIVE_64BITS
#endif

#ifndef NATIVE_64BITS // NO 64BITS
/** 
 * @brief   64-bit types, native or by-hands, depending on target and/or compiler
 *          support.
 * @details This type is defined here only because SHA-3 structure uses it INTERNALLY.
 *          It should never be directly used by other modules.
 */
struct uint64_s {
#ifdef ARCH_LITTLE_ENDIAN
  uint32_t l; ///< 32 least significant bits
  uint32_t h; ///< 32 most significant bits
#else
  uint32_t h;
  uint32_t l;
#endif
};
typedef struct uint64_s uint64bits_t;
#else
typedef uint64_t uint64bits_t;
#endif

/**
 * @brief   Cryptography flags
 * @details Some functions take **logical or** of various flags.
 *          The following flags are globally defined:
 * 
 * | Bits position  | Values            | Flags                         | Meaning                                    | Algorithms            |
 * |----------------|-------------------|-------------------------------|--------------------------------------------|-----------------------|
 * | 15             | 1000000000000000  | CX_NO_REINIT                  | Do not reinitialize the context on CX_LAST |                       |
 * | 14:12          | 0111000000000000  | CX_ECSCHNORR_Z                | Zilliqa scheme                             | ECSCHNORR             |
 * | 14:12          | 0110000000000000  | CX_ECSCHNORR_LIBSECP          | ECSCHNORR according to libsecp256k1        | ECSCHNORR             |
 * | 14:12          | 0101000000000000  | CX_ECSCHNORR_BSI03111         | ECSCHNORR according to BSI TR-03111        | ECSCHNORR             |
 * | 14:12          | 0100000000000000  | CX_ECSCHNORR_ISO14888_X       | ECSCHNORR according to ISO/IEC 14888-3     | ECSCHNORR             |
 * | 14:12          | 0011000000000000  | CX_ECSCHNORR_ISO14888_XY      | ECSCHNORR according to ISO/IEC 14888-3     | ECSCHNORR             |
 * | 14:12          | 0010000000000000  | CX_ECDH_X                     | ECDH with the x-coordinate of the point    | ECDH                  |
 * | 14:12          | 0001000000000000  | CX_ECDH_POINT                 | ECDH with a point                          | ECDH                  |
 * | 11:9           | 0000100000000000  | CX_RND_PROVIDED               | Provided random                            |                       |
 * | 11:9           | 0000011000000000  | CX_RND_RFC6979                | Random from RFC6979                        |                       |
 * | 11:9           | 0000010000000000  | CX_RND_TRNG                   | Random from a PRNG                         |                       |
 * | 11:9           | 0000001000000000  | CX_RND_PRNG                   | Random from a TRNG                         |                       |
 * | 8:6            | 0000000100000000  | CX_CHAIN_OFB                  | Output feedback mode                       | AES/DES               |
 * | 8:6            | 0000000011000000  | CX_CHAIN_CFB                  | Cipher feedback mode                       | AES/DES               |
 * | 8:6            | 0000000010000000  | CX_CHAIN_CTR                  | Counter mode                               | AES/DES               |
 * | 8:6            | 0000000001000000  | CX_CHAIN_CBC                  | Cipher block chaining mode                 | AES/DES               |
 * | 8:6            | 0000000001000000  | CX_NO_CANONICAL               | Do not compute a canonical signature       | ECDSA/EDDSA/ECSCHNORR |
 * | 8:6            | 0000000000000000  | CX_CHAIN_ECB                  | Electronic codebook mode                   | AES/DES               |
 * | 5:3            | 0000000010100000  | CX_PAD_PKCS1_OAEP             | PKCS1_OAEP padding                         |                       |
 * | 5:3            | 0000000010000000  | CX_PAD_PKCS1_PSS              | PKCS1_PSS padding                          |                       |
 * | 5:3            | 0000000001100000  | CX_PAD_PKCS1_1o5              | PKCS1-v1_5 padding                         |                       |
 * | 5:3            | 0000000001000000  | CX_PAD_ISO9797M2              | ISO9797 padding, method 2                  |                       |
 * | 5:3            | 0000000000100000  | CX_PAD_ISO9797M1              | ISO9797 padding, method 1                  |                       |
 * | 5:3            | 0000000000000000  | CX_PAD_NONE                   | No padding                                 |                       |
 * | 2:1            | 0000000000000110  | CX_SIGN                       | Signature                                  | AES/DES               |
 * | 2:1            | 0000000000000100  | CX_ENCRYPT                    | Encryption                                 | AES/DES               |
 * | 2:1            | 0000000000000010  | CX_VERIFY                     | Signature verification                     | AES/DES               |
 * | 2:1            | 0000000000000000  | CX_DECRYPT                    | Decryption                                 | AES/DES               |
 * | 0              | 0000000000000000  | CX_LAST                       | Last block                                 |                       |
 */
#define CX_FLAG

/**
 * Bit 0: Last block
 */
#define CX_LAST (1 << 0)

/**
 * Bit 1: Signature or verification
 */
#define CX_SIG_MODE (1 << 1)

/**
 * Bit 2:1: Signature and/or encryption
 */
#define CX_MASK_SIGCRYPT (3 << 1)
#define CX_ENCRYPT (2 << 1)
#define CX_DECRYPT (0 << 1)
#define CX_SIGN (CX_SIG_MODE | CX_ENCRYPT)
#define CX_VERIFY (CX_SIG_MODE | CX_DECRYPT)

/**
 * Bit 5:3: Padding
 */
#define CX_MASK_PAD (7 << 3)
#define CX_PAD_NONE (0 << 3)
#define CX_PAD_ISO9797M1 (1 << 3)
#define CX_PAD_ISO9797M2 (2 << 3)
#define CX_PAD_PKCS1_1o5 (3 << 3)
#define CX_PAD_PKCS1_PSS (4 << 3)
#define CX_PAD_PKCS1_OAEP (5 << 3)

/**
 * Bit 8:6 DES/AES chaining
 */
#define CX_MASK_CHAIN (7 << 6)
#define CX_CHAIN_ECB (0 << 6)
#define CX_CHAIN_CBC (1 << 6)
#define CX_CHAIN_CTR (2 << 6)
#define CX_CHAIN_CFB (3 << 6)
#define CX_CHAIN_OFB (4 << 6)

/**
 * Bit 8:6 ECC variant
 */
#define CX_MASK_ECC_VARIANT (7 << 6)
#define CX_NO_CANONICAL (1 << 6)

/**
 * Bit 11:9 Random number generation
 */
#define CX_MASK_RND (7 << 9)
#define CX_RND_PRNG (1 << 9)
#define CX_RND_TRNG (2 << 9)
#define CX_RND_RFC6979 (3 << 9)
#define CX_RND_PROVIDED (4 << 9)

/**
 * Bit 14:12: ECDH and ECSCHNORR specificities
 */
#define CX_MASK_EC (7 << 12)
#define CX_ECSCHNORR_BIP0340 (0 << 12)
#define CX_ECDH_POINT (1 << 12)
#define CX_ECDH_X (2 << 12)
#define CX_ECSCHNORR_ISO14888_XY (3 << 12)
#define CX_ECSCHNORR_ISO14888_X (4 << 12)
#define CX_ECSCHNORR_BSI03111 (5 << 12)
#define CX_ECSCHNORR_LIBSECP (6 << 12)
#define CX_ECSCHNORR_Z (7 << 12)

/**
 * Bit 15: No reinitialization
 */
#define CX_NO_REINIT (1 << 15)

#endif
