
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

#ifdef HAVE_SELFTESTS

#include "os.h"
#include "cx_selftests.h"
#include "cx_ecfp.h"
#include "cx_eddsa.h"
#include "cx_ram.h"

enum test_id {
  CX_TEST_MATHM_ID,

  CX_TEST_DES_ID,
  CX_TEST_AES_ID,

  CX_TEST_RIPEMD160_ID,
  CX_TEST_SHA224_ID,
  CX_TEST_SHA256_ID,
  CX_TEST_SHA384_ID,
  CX_TEST_SHA512_ID,

  CX_TEST_SHA3_ID,
  CX_TEST_SHA3_XOF_ID,

  CX_TEST_BLAKE2B_ID,
  CX_TEST_GROESTL_ID,

  CX_TEST_HMAC_RIPEMD160_ID,
  CX_TEST_HMAC_SHA256_ID,
  CX_TEST_HMAC_SHA512_ID,
  CX_TEST_PBKDF2_ID,

  CX_TEST_ECDSA_ID,
  CX_TEST_ECDSA_RFC6979_ID,

  CX_TEST_EDDSA_ID,

  CX_TEST_ECSCHNORR_ID,

  CX_TEST_BORROMEAN_ID,

  CX_TEST_ECFP_ADD_ID,

  CX_TEST_EC_KEYGEN_ID,

  CX_TEST_ECDH_ID,

  CX_TEST_RSA_ID,

  // how many
  CX_TEST_COUNT,
};

typedef struct cx_selftest_s {
  uint8_t _in[512];
  uint8_t _out[512];
  int  _cx_failure_counter;
  int  _cx_results[CX_TEST_COUNT];
} cx_selftest_t;

#define G_HAVE_SELFTESTS ((cx_selftest_t*)0x0020000800)
#define IN                 G_HAVE_SELFTESTS->_in
#define OUT                G_HAVE_SELFTESTS->_out
#define cx_failure_counter G_HAVE_SELFTESTS->_cx_failure_counter
#define cx_results         G_HAVE_SELFTESTS->_cx_results

void cx_bm_setup();
void cx_bm_start();
void cx_bm_stop();

void cx_printa(char *prefix, const unsigned char *r, unsigned short len) {
  UNUSED(prefix);
  cx_printf(("  %s  %.4d  ", prefix, len));
  while (len--) {
    cx_printf(("%.2x", *r));
    r++;
  }
  cx_printf(("\n"));
}


#ifdef ST31
#include "product.h"
#endif

int cx_st_onefail(void) {
  volatile int i = 1; // avoid inline
  // breaker
  return i;
}

int cx_break(void) {
  volatile int i = 2; // avoid inline
  // breaker
  return i;
}

void cx_st_check_result(int r, int ID, int c) {
  if (r) {
    cx_results[ID] |= 1 << c;
    cx_failure_counter++;
    cx_st_onefail();
  }
}
/* ======================================================================= */
/*                                 RAM BUFF                                */
/* ======================================================================= */

static void cx_clr_inout(void) {
  memset(IN, 0, sizeof(IN));
  memset(OUT, 0, sizeof(OUT));
}

/* ======================================================================= */
/*                                   RAND                                  */
/* ======================================================================= */






















/*
*************************************** 1 skip 0 *********************************************************
--------REF--------
order fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f
secexp c48b5c4c5b2ae33df94192f6269d55921fb0f095be4d843b507a2a716954eea7
hash b'ef84dae8cb46951314920500ec18a38670cda437b45c7aeaa73662166638f74d'
ref   b2i/ x= ef84dae8cb46951314920500ec18a38670cda437b45c7aeaa73662166638f74d (256)
ref   b2i/ ret ef84dae8cb46951314920500ec18a38670cda437b45c7aeaa73662166638f74d
qlen 256
holen 32
rolen 32
ref  B/ V= b'0101010101010101010101010101010101010101010101010101010101010101'
ref  C/ V= b'0000000000000000000000000000000000000000000000000000000000000000'
ref  D/ i= b'010101010101010101010101010101010101010101010101010101010101010100'
ref  D/ i= b'c48b5c4c5b2ae33df94192f6269d55921fb0f095be4d843b507a2a716954eea7'
ref  D/ i= b'ef84dae8cb46951314920500ec18a38670cda437b45c7aeaa73662166638f74d'
ref  D/ i= b''
ref  D/ K= b'c446324ebec5416899f23fdd3d89432575ec0e16f901b1b0e074c15dffcb3676'
ref  E/ V= b'122feea4ac17a41e01b31608cf2d8899f1518e7601b375eeaf7fddeb2525b7ec'
ref  F/ V= b'7bf9d593c9514dbf18a86cd26cfcc39cf87dafd993dfb00b129c723eb3cba4a1'
ref  G/ V= b'2fb05f6bf880fb602918990e503a75367e5078fd928b5a608ebf77fc0f5d6a9e'
ref  H3/ loop 0: 0 < 32 :: True
ref  H3/ T= 32 ?< 32 b'b22ac3d26ff3734315f18c9662e7c7d8e84a1e83a6d9c77766e6e149a53c584f'
ref   b2i/ x= b22ac3d26ff3734315f18c9662e7c7d8e84a1e83a6d9c77766e6e149a53c584f (256)
ref   b2i/ ret b22ac3d26ff3734315f18c9662e7c7d8e84a1e83a6d9c77766e6e149a53c584f

=> b22ac3d26ff3734315f18c9662e7c7d8e84a1e83a6d9c77766e6e149a53c584f
*/
static const unsigned char rfc6979_sha256_order1[] = {
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xfc, 0x2f
};
static const unsigned char rfc6979_sha256_key1[] = {
  0xc4, 0x8b, 0x5c, 0x4c, 0x5b, 0x2a, 0xe3, 0x3d, 0xf9, 0x41, 0x92, 0xf6, 0x26, 0x9d, 0x55, 0x92, 0x1f, 0xb0, 0xf0, 0x95, 0xbe, 0x4d, 0x84, 0x3b, 0x50, 0x7a, 0x2a, 0x71, 0x69, 0x54, 0xee, 0xa7
};
static const unsigned char rfc6979_sha256_hash1[] = {
  0xef, 0x84, 0xda, 0xe8, 0xcb, 0x46, 0x95, 0x13, 0x14, 0x92, 0x05, 0x00, 0xec, 0x18, 0xa3, 0x86, 0x70, 0xcd, 0xa4, 0x37, 0xb4, 0x5c, 0x7a, 0xea, 0xa7, 0x36, 0x62, 0x16, 0x66, 0x38, 0xf7, 0x4d
};
/*

*************************************** 2 skip 0 *********************************************************
--------REF--------
order  0800000000000011000000000000000000000000000000000000000000000001
secexp    3f8311eece8cafea3bf568aa90457ad6bbc47deed0e3c44e0aa94b98ac41f
hash b'e511c5432ea4f80aa434b842fe5398ec23d62f3e0da06c2400005ef9c7ac1b4b'
ref   b2i/ x= e511c5432ea4f80aa434b842fe5398ec23d62f3e0da06c2400005ef9c7ac1b4b (256)
ref   b2i/ shift by 4
qlen 252
holen 32
rolen 32
ref  B/ V= b'0101010101010101010101010101010101010101010101010101010101010101'
ref  C/ V= b'0000000000000000000000000000000000000000000000000000000000000000'
ref  D/ i= b'010101010101010101010101010101010101010101010101010101010101010100'
ref  D/ i= b'0003f8311eece8cafea3bf568aa90457ad6bbc47deed0e3c44e0aa94b98ac41f'
ref  D/ i= b'06511c5432ea4f6faa434b842fe5398ec23d62f3e0da06c2400005ef9c7ac1b3'
ref  D/ i= b''
ref  D/ K= b'387e7380b6499950fef0661cee7e11346f3ed21ff34331847832d5669e227fa0'
ref  E/ V= b'ee6cadf713e3211a719f64180851b873773075e7259839a110ce527871f4b801'
ref  F/ V= b'c735259ef043020c7f9c1633d10643444979ee5ee67ff836593f9a99c87006a9'
ref  G/ V= b'f4c60173490d35c3341954bd9b07d430da2fb0c70cb578d0b98059d11fa2fa37'
ref  H3/ loop 0: 0 < 32 :: True
ref  H3/ T= 32 ?< 32 b'de22c0b3708147a2301738a26af35e08ba4512e49abda4b88c2e4aa889226200'
ref   b2i/ x= de22c0b3708147a2301738a26af35e08ba4512e49abda4b88c2e4aa889226200 (256)
ref   b2i/ shift by 4
ref  H3/ skip bad de22c0b3708147a2301738a26af35e08ba4512e49abda4b88c2e4aa88922620
ref  H3/ loop 0: 0 < 32 :: True
ref  H3/ T= 32 ?< 32 b'0b8c9f65f6a2a026f75b621380bb23089362c71c6212b3cceaa6c99d2e414d8c'
ref   b2i/ x= b8c9f65f6a2a026f75b621380bb23089362c71c6212b3cceaa6c99d2e414d8c (256)
ref   b2i/ shift by 4

=> b8c9f65f6a2a026f75b621380bb23089362c71c6212b3cceaa6c99d2e414d8
*/

static const unsigned char rfc6979_sha256_order2[] = {
  0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01
};
static const unsigned char rfc6979_sha256_key2[] = {
  0x00, 0x03, 0xf8, 0x31, 0x1e, 0xec, 0xe8, 0xca, 0xfe, 0xa3, 0xbf, 0x56, 0x8a, 0xa9, 0x04, 0x57, 0xad, 0x6b, 0xbc, 0x47, 0xde, 0xed, 0x0e, 0x3c, 0x44, 0xe0, 0xaa, 0x94, 0xb9, 0x8a, 0xc4, 0x1f
};
static const unsigned char rfc6979_sha256_hash2[] = {
  0xe5, 0x11, 0xc5, 0x43, 0x2e, 0xa4, 0xf8, 0x0a, 0xa4, 0x34, 0xb8, 0x42, 0xfe, 0x53, 0x98, 0xec, 0x23, 0xd6, 0x2f, 0x3e, 0x0d, 0xa0, 0x6c, 0x24, 0x00, 0x00, 0x5e, 0xf9, 0xc7, 0xac, 0x1b, 0x4b
};


void cx_st_rfc6979() {

  cx_rng_rfc6979(CX_SHA256,
                 rfc6979_sha256_key1, sizeof(rfc6979_sha256_key1),
                 rfc6979_sha256_hash1, sizeof(rfc6979_sha256_hash1),
                 rfc6979_sha256_order1, sizeof(rfc6979_sha256_order1),
                 OUT, sizeof(rfc6979_sha256_order1));

  cx_rng_rfc6979(CX_SHA256,
                 rfc6979_sha256_key2, sizeof(rfc6979_sha256_key2),
                 rfc6979_sha256_hash2, sizeof(rfc6979_sha256_hash2),
                 rfc6979_sha256_order2, sizeof(rfc6979_sha256_order2),
                 OUT, sizeof(rfc6979_sha256_order1));

}



// =====================================================================================


int cx_selftest() {
  int volatile looper = HAVE_SELFTESTS_START_LOOP;
  while (looper);
  memset(G_HAVE_SELFTESTS, 0, sizeof(cx_selftest_t));
  cx_st_rfc6979();

  cx_break();
  return cx_failure_counter;
  }

#endif // SELFTEST
