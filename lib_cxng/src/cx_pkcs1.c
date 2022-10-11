
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

#include "cx_rsa.h"
#include "cx_ram.h"
#include "cx_utils.h"
#include "errors.h"
#include "exceptions.h"
#include "lcx_rng.h"

#include <string.h>

#ifdef HAVE_RSA

/* ----------------------------------------------------------------------- */
/*                                                                         */
/* ----------------------------------------------------------------------- */

#if defined(HAVE_SHA224)
/* sha224 OID */
const uint8_t C_cx_oid_sha224[CX_OID_SHA224_LENGTH] = {0x30, 0x2d, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01,
                                                       0x65, 0x03, 0x04, 0x02, 0x04, 0x05, 0x00, 0x04, 0x1c};
#endif // HAVE_SHA224

#if defined(HAVE_SHA256)
/* sha256 OID */
const uint8_t C_cx_oid_sha256[CX_OID_SHA256_LENGTH] = {0x30, 0x31, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01,
                                                       0x65, 0x03, 0x04, 0x02, 0x01, 0x05, 0x00, 0x04, 0x20};
#endif // HAVE_SHA256

#if defined(HAVE_SHA384)
/* sha384 OID */
const uint8_t C_cx_oid_sha384[CX_OID_SHA384_LENGTH] = {0x30, 0x41, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01,
                                                       0x65, 0x03, 0x04, 0x02, 0x02, 0x05, 0x00, 0x04, 0x30};
#endif // HAVE_SHA384

#if defined(HAVE_SHA512)
/* sha512 OID */
const uint8_t C_cx_oid_sha512[CX_OID_SHA512_LENGTH] = {0x30, 0x51, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01,
                                                       0x65, 0x03, 0x04, 0x02, 0x03, 0x05, 0x00, 0x04, 0x40};
#endif // HAVE_SHA512

#if defined(HAVE_SHA3)
/* sha3-256 OID */
const uint8_t C_cx_oid_sha3_256[CX_OID_SHA3_256_LENGTH]
                                                    = {0x30, 0x31, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01,
                                                       0x65, 0x03, 0x04, 0x02, 0x08, 0x05, 0x00, 0x04, 0x20};

/* sha3-512 OID */
const uint8_t C_cx_oid_sha3_512[CX_OID_SHA3_512_LENGTH]
                                                    = {0x30, 0x51, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01,
                                                       0x65, 0x03, 0x04, 0x02, 0x0a, 0x05, 0x00, 0x04, 0x40};
#endif // HAVE_SHA3

size_t cx_pkcs1_get_hash_len(cx_md_t hID) {
  switch (hID) {
#if defined(HAVE_SHA224)
  case CX_SHA224:
    return 224/8;
#endif // HAVE_SHA224

#if defined(HAVE_SHA256)
  case CX_SHA256:
    return 256/8;
#endif // HAVE_SHA256

#if defined(HAVE_SHA384)
  case CX_SHA384:
    return 384/8;
#endif // HAVE_SHA384

#if defined(HAVE_SHA512)
  case CX_SHA512:
    return 512/8;
#endif // HAVE_SHA512

#if defined(HAVE_SHA3)
  case CX_SHA3_256:
    return 256/8;

  case CX_SHA3_512:
    return 512/8;
#endif // HAVE_SHA3

  default:
    break;
  }
  return 0;
}

static const uint8_t *cx_pkcs1_get_hash_oid(cx_md_t hID, size_t *len) {
  switch (hID) {
#if defined(HAVE_SHA224)
  case CX_SHA224:
    *len = sizeof(C_cx_oid_sha224);
    return C_cx_oid_sha224;
#endif // HAVE_SHA224

#if defined(HAVE_SHA256)
  case CX_SHA256:
    *len = sizeof(C_cx_oid_sha256);
    return C_cx_oid_sha256;
#endif // HAVE_SHA256

#if defined(HAVE_SHA384)
  case CX_SHA384:
    *len = sizeof(C_cx_oid_sha384);
    return C_cx_oid_sha384;
#endif // HAVE_SHA384

#if defined(HAVE_SHA512)
  case CX_SHA512:
    *len = sizeof(C_cx_oid_sha512);
    return C_cx_oid_sha512;
#endif // HAVE_SHA512

#if defined(HAVE_SHA3)
  case CX_SHA3_256:
      *len = sizeof(C_cx_oid_sha3_256);
      return C_cx_oid_sha3_256;

  case CX_SHA3_512:
    *len = sizeof(C_cx_oid_sha3_512);
    return C_cx_oid_sha3_512;
#endif // HAVE_SHA3

  default:
    break;
  }
  *len = 0;
  return NULL;
}

#ifdef _CX_PKCS1_CONST_SEED_SALT
static const uint8_t C_cx_pkcs_salt[] = {'T', 'h', 'e', 'S', 'a', 'l', 't', ' ', 'T', 'h', 'e',
                                         'S', 'a', 'l', 't', ' ', 'T', 'h', 'e', 'S', 'a', 'l',
                                         't', ' ', 'T', 'h', 'e', 'S', 'a', 'l', 't', ' '};
#endif

static const uint8_t C_cx_pss_zeros[] = {0, 0, 0, 0, 0, 0, 0, 0};

// We only support the empty string label, defined as default.
// The following arrays are the corresponding hash applied to this empty string label.
#if defined(HAVE_SHA224)
static const uint8_t C_cx_oeap_sha224_[] = {
  0xd1, 0x4a, 0x02, 0x8c, 0x2a, 0x3a, 0x2b, 0xc9,
  0x47, 0x61, 0x02, 0xbb, 0x28, 0x82, 0x34, 0xc4,
  0x15, 0xa2, 0xb0, 0x1f, 0x82, 0x8e, 0xa6, 0x2a,
  0xc5, 0xb3, 0xe4, 0x2f};
#endif // HAVE_SHA224

#if defined(HAVE_SHA256)
static const uint8_t C_cx_oeap_sha256_[] = {
  0xe3, 0xb0, 0xc4, 0x42, 0x98, 0xfc, 0x1c, 0x14,
  0x9a, 0xfb, 0xf4, 0xc8, 0x99, 0x6f, 0xb9, 0x24,
  0x27, 0xae, 0x41, 0xe4, 0x64, 0x9b, 0x93, 0x4c,
  0xa4, 0x95, 0x99, 0x1b, 0x78, 0x52, 0xb8, 0x55,
};
#endif // HAVE_SHA256

#if defined(HAVE_SHA384)
static const uint8_t C_cx_oeap_sha384_[] = {
  0x38, 0xb0, 0x60, 0xa7, 0x51, 0xac, 0x96, 0x38,
  0x4c, 0xd9, 0x32, 0x7e, 0xb1, 0xb1, 0xe3, 0x6a,
  0x21, 0xfd, 0xb7, 0x11, 0x14, 0xbe, 0x07, 0x43,
  0x4c, 0x0c, 0xc7, 0xbf, 0x63, 0xf6, 0xe1, 0xda,
  0x27, 0x4e, 0xde, 0xbf, 0xe7, 0x6f, 0x65, 0xfb,
  0xd5, 0x1a, 0xd2, 0xf1, 0x48, 0x98, 0xb9, 0x5b};
#endif // HAVE_SHA384

#if defined(HAVE_SHA512)
static const uint8_t C_cx_oeap_sha512_[] = {
  0xcf, 0x83, 0xe1, 0x35, 0x7e, 0xef, 0xb8, 0xbd,
  0xf1, 0x54, 0x28, 0x50, 0xd6, 0x6d, 0x80, 0x07,
  0xd6, 0x20, 0xe4, 0x05, 0x0b, 0x57, 0x15, 0xdc,
  0x83, 0xf4, 0xa9, 0x21, 0xd3, 0x6c, 0xe9, 0xce,
  0x47, 0xd0, 0xd1, 0x3c, 0x5d, 0x85, 0xf2, 0xb0,
  0xff, 0x83, 0x18, 0xd2, 0x87, 0x7e, 0xec, 0x2f,
  0x63, 0xb9, 0x31, 0xbd, 0x47, 0x41, 0x7a, 0x81,
  0xa5, 0x38, 0x32, 0x7a, 0xf9, 0x27, 0xda, 0x3e};
#endif // HAVE_SHA512

static const uint8_t *cx_pkcs1_get_hash_oeap(cx_md_t hID, size_t *len) {
  switch (hID) {
#if defined(HAVE_SHA224)
  case CX_SHA224:
    *len = sizeof(C_cx_oeap_sha224_);
    return C_cx_oeap_sha224_;
#endif // HAVE_SHA224

#if defined(HAVE_SHA256)
  case CX_SHA256:
    *len = sizeof(C_cx_oeap_sha256_);
    return C_cx_oeap_sha256_;
#endif // HAVE_SHA256

#if defined(HAVE_SHA384)
  case CX_SHA384:
    *len = sizeof(C_cx_oeap_sha384_);
    return C_cx_oeap_sha384_;
#endif // HAVE_SHA384

#if defined(HAVE_SHA512)
  case CX_SHA512:
    *len = sizeof(C_cx_oeap_sha512_);
    return C_cx_oeap_sha512_;
#endif // HAVE_SHA512

  default:
    break;
  }
  *len = 0;
  return NULL;
}

static cx_err_t cx_pkcs1_MGF1(cx_md_t hID, uint8_t *seed, size_t seed_len, uint8_t *out, size_t out_len) {
  size_t     hLen;
  size_t     round_len;
  uint8_t    counter[4] = {0};
  cx_hash_t *hash_ctx   = &G_cx.pkcs1.hash_ctx.hash;

  if (hID != CX_SHA224 && hID != CX_SHA256 && hID != CX_SHA384 && hID != CX_SHA512
   && hID != CX_SHA3_256 && hID != CX_SHA3_512) {
    return CX_INVALID_PARAMETER;
  }
  hLen = cx_pkcs1_get_hash_len(hID);

  while (out_len) {
    round_len = (out_len < hLen) ? out_len : hLen;

    cx_hash_init_ex(hash_ctx, hID, hLen);
    cx_hash_update(hash_ctx, seed, seed_len);
    cx_hash_update(hash_ctx, counter, 4);
    cx_hash_final(hash_ctx, G_cx.pkcs1.digest);

    memcpy(out, G_cx.pkcs1.digest, round_len);
    out_len -= round_len;
    out += round_len;
    counter[3]++;
    if (counter[3] == 0) {
      counter[2]++;
    }
  }

  return CX_OK;
}

/* ----------------------------------------------------------------------- */
/* EMSA: pkcs1 V1.5                                                        */
/* ----------------------------------------------------------------------- */

/*
 *  message: at ptr+size-msg_size
 */
cx_err_t cx_pkcs1_emsa_v1o5_encode(cx_md_t hID, uint8_t *em, size_t emLen, const uint8_t *mHash, size_t mHashLen) {
  size_t         PSLen;
  size_t         oid_len;
  const uint8_t *oid;

  oid = cx_pkcs1_get_hash_oid(hID, &oid_len);

  if ((3 + oid_len + mHashLen) >= emLen) {
    return CX_INVALID_PARAMETER;
  }
  PSLen = emLen - (3 + oid_len + mHashLen);

  memcpy(em + 2 + PSLen + 1 + oid_len, mHash, mHashLen);
  memcpy(em + 2 + PSLen + 1, oid, oid_len);
  em[0] = 0;
  em[1] = 1;
  memset(em + 2, 0xFF, PSLen);
  em[2 + PSLen] = 0;

  return CX_OK;
}

/** out
 * ptr :    right aligned unpadded message:
 * return : size unpadded message, -1 else
 *
 */
bool cx_pkcs1_emsa_v1o5_verify(cx_md_t hID, uint8_t *em, size_t emLen, const uint8_t *mHash, size_t mHashLen) {
  size_t         PSLen;
  size_t         i;
  size_t         oid_len;
  const uint8_t *oid;

  oid = cx_pkcs1_get_hash_oid(hID, &oid_len);

  if ((3 + oid_len + mHashLen) >= emLen) {
    return false;
  }
  PSLen = emLen - (3 + oid_len + mHashLen);

  // -> check 00 01 ... 00
  if ((em[0]) || (em[1] != 1) || (em[2 + PSLen] != 0)) {
    return 0;
  }
  // -> check .. .. FFFFFF ..
  for (i = 2 + PSLen - 1; i >= 2; i--) {
    if (em[i] != 0xFF) {
      return false;
    }
  }
  // check OID / hash
  if (memcmp(em + 2 + PSLen + 1, oid, oid_len) != 0 || memcmp(em + 2 + PSLen + 1 + oid_len, mHash, mHashLen) != 0) {
    return false;
  }

  // OKAY
  return true;
}

/* ----------------------------------------------------------------------- */
/* EMSA: pkcs1 PSS                                                         */
/* ----------------------------------------------------------------------- */
static uint8_t const CX_PSS_MASK[] = {0, 0x7F, 0x3F, 0x1F, 0xF, 0x7, 0x3, 0x1};

cx_err_t cx_pkcs1_emsa_pss_encode_with_salt_len(cx_md_t hID, uint8_t *em, size_t emBits, const uint8_t *mHash, size_t mHashLen, size_t mSaltLen, size_t *size) {
  uint8_t    salt[64];
  size_t     hLen;
  size_t     mDBlen;
  size_t     PSLen;
  size_t     emLen;
  cx_hash_t *hash_ctx = &G_cx.pkcs1.hash_ctx.hash;
  cx_err_t error;

  if (hID != CX_SHA224 && hID != CX_SHA256 && hID != CX_SHA384 && hID != CX_SHA512) {
    return CX_INVALID_PARAMETER;
  }

  hLen  = cx_pkcs1_get_hash_len(hID);
  emLen = (emBits + 7) / 8;

  if ((1 + hLen) >= emLen) {
    return CX_INVALID_PARAMETER;
  }
  mDBlen = emLen - (1 + hLen);

  if ((hLen + mSaltLen + 2) >= emLen) {
    return CX_INVALID_PARAMETER;
  }
  PSLen = emLen - (mSaltLen + hLen + 2);

#ifndef _CX_PKCS1_CONST_SEED_SALT
  cx_rng_no_throw(salt, mSaltLen);
#endif

  cx_hash_init_ex(hash_ctx, hID, hLen);
  cx_hash_update(hash_ctx, C_cx_pss_zeros, 8);
  cx_hash_update(hash_ctx, mHash, mHashLen);
  cx_hash_update(hash_ctx, salt, mSaltLen);
  cx_hash_final(hash_ctx, em + mDBlen);

  em[emLen - 1] = 0xbc;
  CX_CHECK(cx_pkcs1_MGF1(hID, em + mDBlen, hLen, em, mDBlen));
  em[PSLen] ^= 1;
  cx_memxor(em + PSLen + 1, salt, mSaltLen);
  em[0] &= CX_PSS_MASK[8 * emLen - emBits];
  *size = emLen;

 end:
  return error;
}

cx_err_t cx_pkcs1_emsa_pss_encode(cx_md_t hID, uint8_t *em, size_t emBits, const uint8_t *mHash, size_t mHashLen, size_t *size) {
  size_t hLen = cx_pkcs1_get_hash_len(hID);
  return cx_pkcs1_emsa_pss_encode_with_salt_len(hID, em, emBits, mHash, mHashLen, hLen, size);
}

bool cx_pkcs1_emsa_pss_verify_with_salt_len(cx_md_t hID, uint8_t *em, size_t emBits, const uint8_t *mHash, size_t mHashLen, size_t mSaltLen) {
  size_t     mDBlen;
  size_t     PSLen;
  size_t     i;
  size_t     emLen;
  size_t     hLen;
  cx_hash_t *hash_ctx = &G_cx.pkcs1.hash_ctx.hash;

  if (hID != CX_SHA224 && hID != CX_SHA256 && hID != CX_SHA384 && hID != CX_SHA512) {
    return false;
  }

  hLen  = cx_pkcs1_get_hash_len(hID);
  emLen = (emBits + 7) / 8;

  if ((1 + hLen) >= emLen) {
    return false;
  }
  mDBlen = emLen - (1 + hLen);

  if ((mSaltLen + hLen + 2) >= emLen) {
    return false;
  }
  PSLen = emLen - (mSaltLen + hLen + 2);

  if (em[emLen - 1] != 0xbc) {
    return false;
  }
  if (em[0] & ~CX_PSS_MASK[8 * emLen - emBits]) {
    return false;
  }
  if (cx_pkcs1_MGF1(hID, em + mDBlen, hLen, G_cx.pkcs1.MGF1, mDBlen) != CX_OK) {
    return false;
  }
  cx_memxor(em, G_cx.pkcs1.MGF1, mDBlen);
  em[0] &= CX_PSS_MASK[8 * emLen - emBits];
  for (i = 0; i < PSLen; i++) {
    if (em[i] != 0) {
      return false;
    }
  }
  if (em[PSLen] != 0x01) {
    return false;
  }

  cx_hash_init_ex(hash_ctx, hID, hLen);
  cx_hash_update(hash_ctx, C_cx_pss_zeros, 8);
  cx_hash_update(hash_ctx, mHash, mHashLen);
  cx_hash_update(hash_ctx, em + PSLen + 1, mSaltLen);
  cx_hash_final(hash_ctx, G_cx.pkcs1.digest);

  return memcmp(G_cx.pkcs1.digest, em + mDBlen, hLen) == 0;
}

bool cx_pkcs1_emsa_pss_verify(cx_md_t hID, uint8_t *em, size_t emBits, const uint8_t *mHash, size_t mHashLen) {
  // By default and by convention, the salt length is the hash length.
  // In order to verify a RSA PSS signature with a variable salt length,
  // the 'cx_pkcs1_emsa_pss_verify_with_salt_len' function must be
  // used directly.
  size_t hLen = cx_pkcs1_get_hash_len(hID);
  return cx_pkcs1_emsa_pss_verify_with_salt_len(hID, em, emBits, mHash, mHashLen, hLen);
}

/* ----------------------------------------------------------------------- */
/* EME: pkcs1 V1.5                                                           */
/* ----------------------------------------------------------------------- */
cx_err_t cx_pkcs1_eme_v1o5_encode(cx_md_t hID, uint8_t *em, size_t emLen, const uint8_t *m, size_t mLen) {
  size_t PSLen;
  size_t offset;
  (void)hID;

  if ((3 + mLen) >= emLen) {
    return CX_INVALID_PARAMETER;
  }
  PSLen = emLen - (3 + mLen);

  memcpy(em + 2 + PSLen + 1, m, mLen);
  em[0] = 0;
  em[1] = 2;
  cx_rng_no_throw(em + 2, PSLen);
  for (offset = 2 + PSLen - 1; offset >= 2; offset--) {
    if (em[offset] == 0) {
      em[offset] = 0x24;
    }
  }
  em[2 + PSLen] = 0;
  return CX_OK;
}

size_t cx_pkcs1_eme_v1o5_decode(cx_md_t hID, uint8_t *em, size_t emLen, uint8_t *m, size_t mLen) {
  size_t offset;
  (void)hID;
  // -> check 00 02 ... 00
  if ((em[0]) || (em[1] != 2)) {
    return -1;
  }
  // -> check .. .. FFFFFF ..
  offset = 2;
  while (offset < emLen) {
    if (em[offset] == 0) {
      break;
    }
    offset++;
  }
  if (offset == emLen) {
    return -1;
  }
  offset++;
  // copy M
  if (mLen < emLen - offset) {
    return -1;
  }
  mLen = emLen - offset;
  memmove(m, em + offset, mLen);
  return mLen;
}

/* ----------------------------------------------------------------------- */
/* EME: pkcs1 OAEP                                                         */
/* ----------------------------------------------------------------------- */
cx_err_t cx_pkcs1_eme_oaep_encode(cx_md_t hID, uint8_t *em, size_t emLen, const uint8_t *m, size_t mLen) {
  size_t         mDBlen;
  size_t         psLen;
  size_t         offset;
  uint8_t        seed[64];
  size_t         hLen;
  const uint8_t *lHash;
  size_t         lHashLen;
  cx_err_t       error;

  hLen  = cx_pkcs1_get_hash_len(hID);
  lHash = cx_pkcs1_get_hash_oeap(hID, &lHashLen);

  if ((hLen + 1) >= emLen) {
    return CX_INVALID_PARAMETER;
  }
  mDBlen = emLen - (hLen + 1);
  if ((mLen + 2 * hLen + 2) >= emLen) {
    return CX_INVALID_PARAMETER;
  }
  psLen = emLen - (mLen + 2 * hLen + 2);

  // random seed
  cx_rng_no_throw(seed, hLen);

  // DB = lHash || PS || 01 || M
  offset = 1 + hLen;
  memcpy(em + offset, lHash, lHashLen);
  offset += lHashLen;
  memset(em + offset, 0, psLen);
  offset += psLen;
  em[offset] = 0x01;
  offset++;
  memcpy(em + offset, m, mLen);

  // 00 | masked seed || maskedDB
  em[0] = 0x00;

  CX_CHECK(cx_pkcs1_MGF1(hID, seed, hLen, G_cx.pkcs1.MGF1, mDBlen));
  cx_memxor(em + 1 + hLen, G_cx.pkcs1.MGF1, mDBlen);

  CX_CHECK(cx_pkcs1_MGF1(hID, em + 1 + hLen, mDBlen, G_cx.pkcs1.MGF1, hLen));
  memcpy(em + 1, seed, hLen);
  cx_memxor(em + 1, G_cx.pkcs1.MGF1, hLen);

 end:
  return error;
}

cx_err_t cx_pkcs1_eme_oaep_decode(cx_md_t hID, uint8_t *em, size_t emLen, uint8_t *m, size_t *mLen) {
  size_t         mDBlen;
  size_t         offset;
  size_t         hLen;
  const uint8_t *lHash;
  size_t         lHashLen;
  cx_err_t       error;

  hLen  = cx_pkcs1_get_hash_len(hID);
  lHash = cx_pkcs1_get_hash_oeap(hID, &lHashLen);
  if ((hLen + 1) >= emLen) {
    return CX_INVALID_PARAMETER;
  }
  mDBlen = emLen - (hLen + 1);

  // unmask all
  CX_CHECK(cx_pkcs1_MGF1(hID, em + 1 + hLen, mDBlen, G_cx.pkcs1.MGF1, hLen));
  cx_memxor(em + 1, G_cx.pkcs1.MGF1, hLen);
  CX_CHECK(cx_pkcs1_MGF1(hID, em + 1, hLen, G_cx.pkcs1.MGF1, mDBlen));
  cx_memxor(em + 1 + hLen, G_cx.pkcs1.MGF1, mDBlen);

  // Y||seed||lHash||PS||01||M
  if (em[0] != 0x00) {
    return CX_INVALID_PARAMETER;
  }
  if (memcmp(em + 1 + hLen, lHash, hLen) != 0) {
    return CX_INVALID_PARAMETER;
  }

  for (offset = 1 + hLen + lHashLen; offset < emLen; offset++) {
    if (em[offset] != 0) {
      break;
    }
  }
  if ((offset == emLen) || (em[offset] != 0x01)) {
    return CX_INVALID_PARAMETER;
  }
  offset++;

  // copy M
  if (*mLen < emLen - offset) {
    return CX_INVALID_PARAMETER;
  }

  *mLen = emLen - offset;
  memmove(m, em + offset, *mLen);

 end:
  return error;
}

#endif

#endif // HAVE_RSA
