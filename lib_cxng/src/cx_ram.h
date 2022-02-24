
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

#ifndef CX_RAM_H
#define CX_RAM_H

#include "cx_hash.h"
#include "cx_pbkdf2.h"
#include "cx_ecfp.h"
#include "cx_rng.h"
#include "cx_rng_rfc6979.h"
#include "cx_hmac.h"
#include "cx_blake2b.h"
#include "cx_groestl.h"
#include "cx_rsa.h"
#include "cx_sha3.h"
#include "cx_sha256.h"
#include "cx_sha512.h"

/** 1K RAM lib */
union cx_u {
/* PBKDF internal hash */
#ifdef HAVE_PBKDF2
  cx_pbkdf2_t pbkdf2;
#endif

  /* RSA internal PKCS1 */
#ifdef HAVE_RSA
  cx_pkcs1_t pkcs1;
#endif

/* hash & hmac */
#ifdef HAVE_HASH
  cx_hash_t hash_ctx;

#if defined(HAVE_SHA256) || defined(HAVE_SHA224)
  cx_sha256_t sha256;
#endif

#if defined(HAVE_SHA512) || defined(HAVE_SHA384)
  cx_sha512_t sha512;
#endif

#if defined(HAVE_SHA3)
  cx_sha3_t sha3;
#endif

#if defined(HAVE_BLAKE2)
  cx_xblake_t blake;
#endif

#if defined(HAVE_GROESTL)
  cx_xgroestl_t groestl;
#endif
#endif

#ifdef HAVE_HMAC
  cx_hmac_t hmac;

#if defined(HAVE_HMAC) && (defined(HAVE_SHA512) || defined(HAVE_SHA384))
  cx_hmac_sha512_t hmac_sha512;
#endif

#if defined(HAVE_HMAC) && (defined(HAVE_SHA256) || defined(HAVE_SHA224))
  cx_hmac_sha256_t hmac_sha256;
#endif
#endif


#ifdef HAVE_RNG_RFC6979
  cx_rnd_rfc6979_ctx_t rfc6979;
#endif

};
extern union cx_u G_cx;

#endif
