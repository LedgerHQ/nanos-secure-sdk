
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
#include "cx_utils.h"
#include "cx_ram.h"

#include <string.h>

static cx_err_t modulus_valid(size_t modulus_len)
{
  switch (modulus_len) {
  case 128:
  case 256:
  case 384:
  case 512:
    return CX_OK;
  default:
    return CX_INVALID_PARAMETER;
  }
}

cx_err_t cx_rsa_private_key_ctx_size(const cx_rsa_private_key_t *key, size_t *size) {
  switch (key->size) {
  case 128:
    *size = sizeof(cx_rsa_1024_private_key_t);
    break;
  case 256:
    *size = sizeof(cx_rsa_2048_private_key_t);
    break;
  case 384:
    *size = sizeof(cx_rsa_3072_private_key_t);
    break;
  case 512:
    *size = sizeof(cx_rsa_4096_private_key_t);
    break;
  default:
    return CX_INVALID_PARAMETER;
  }

  return CX_OK;
}

cx_err_t cx_rsa_get_public_components(const cx_rsa_public_key_t *key, uint8_t **e, uint8_t **n) {
  switch (key->size) {
  case 128:
    *e = ((cx_rsa_1024_public_key_t *)key)->e;
    *n = ((cx_rsa_1024_public_key_t *)key)->n;
    break;
  case 256:
    *e = ((cx_rsa_2048_public_key_t *)key)->e;
    *n = ((cx_rsa_2048_public_key_t *)key)->n;
    break;
  case 384:
    *e = ((cx_rsa_3072_public_key_t *)key)->e;
    *n = ((cx_rsa_3072_public_key_t *)key)->n;
    break;
  case 512:
    *e = ((cx_rsa_4096_public_key_t *)key)->e;
    *n = ((cx_rsa_4096_public_key_t *)key)->n;
    break;
  default:
    return CX_INVALID_PARAMETER;
  }
  return CX_OK;
}

cx_err_t cx_rsa_get_private_components(const cx_rsa_private_key_t *key, uint8_t **d, uint8_t **n) {
  switch (key->size) {
  case 128:
    *d = ((cx_rsa_1024_private_key_t *)key)->d;
    *n = ((cx_rsa_1024_private_key_t *)key)->n;
    break;
  case 256:
    *d = ((cx_rsa_2048_private_key_t *)key)->d;
    *n = ((cx_rsa_2048_private_key_t *)key)->n;
    break;
  case 384:
    *d = ((cx_rsa_3072_private_key_t *)key)->d;
    *n = ((cx_rsa_3072_private_key_t *)key)->n;
    break;
  case 512:
    *d = ((cx_rsa_4096_private_key_t *)key)->d;
    *n = ((cx_rsa_4096_private_key_t *)key)->n;
    break;
  default:
    return CX_INVALID_PARAMETER;
  }
  return CX_OK;
}

cx_err_t cx_rsa_init_public_key_no_throw(const uint8_t *      exponent,
                                size_t               exponent_len,
                                const uint8_t *      modulus,
                                size_t               modulus_len,
                                cx_rsa_public_key_t *key) {
  cx_err_t error;
  uint8_t *e;
  uint8_t *n;

  if (!(((exponent == NULL) && (exponent_len == 0)) || ((exponent) && (exponent_len <= 4)))) {
    return CX_INVALID_PARAMETER;
  }
  if (!(((modulus == NULL) && (modulus_len == 0)) || (modulus))) {
    return CX_INVALID_PARAMETER;
  }
  if (key == NULL) {
    return CX_INVALID_PARAMETER;
  }

  CX_CHECK(modulus_valid(modulus_len));
  key->size = modulus_len;

  CX_CHECK(cx_rsa_get_public_components(key, &e, &n));

  if (modulus && exponent) {
    memset(e, 0, 4);
    memmove(e + (4 - exponent_len), exponent, exponent_len);
    memmove(n, modulus, modulus_len);
  } else {
    memset(n, 0, modulus_len);
    memset(e, 0, 4);
  }

 end:
  return error;
}

cx_err_t cx_rsa_init_private_key_no_throw(const uint8_t *       exponent,
                                 size_t                exponent_len,
                                 const uint8_t *       modulus,
                                 size_t                modulus_len,
                                 cx_rsa_private_key_t *key) {
  cx_err_t error;
  uint8_t *d;
  uint8_t *n;

  if (!(((exponent == NULL) && (exponent_len == 0)) || (exponent))) {
    return CX_INVALID_PARAMETER;
  }
  if (!(((modulus == NULL) && (modulus_len == 0)) || (modulus))) {
    return CX_INVALID_PARAMETER;
  }
  if (exponent_len != modulus_len) {
    return CX_INVALID_PARAMETER;
  }
  if (key == NULL) {
    return CX_INVALID_PARAMETER;
  }

  CX_CHECK(modulus_valid(modulus_len));
  key->size = modulus_len;

  CX_CHECK(cx_rsa_get_private_components(key, &d, &n));

  if (modulus && exponent) {
    memmove(d, exponent, modulus_len);
    memmove(n, modulus, modulus_len);
  } else {
    memset(d, 0, modulus_len);
    memset(n, 0, modulus_len);
  }

 end:
  return error;
}

static const uint8_t C_default_e[] = {0x00, 0x01, 0x00, 0x01};

cx_err_t cx_rsa_generate_pair_no_throw(size_t                modulus_len,
                              cx_rsa_public_key_t * public_key,
                              cx_rsa_private_key_t *private_key,
                              const uint8_t *       exponent,
                              size_t                exponent_len,
                              const uint8_t *       externalPQ) {
  cx_bn_t bn_p, bn_q, bn_n;
  cx_err_t error;
  uint8_t *pv_d;
  uint8_t *pv_n;
  uint8_t *pu_e;
  uint8_t *pu_n;
  size_t   size;

  if (!(((exponent == NULL) && (exponent_len == 0)) || ((exponent) && (exponent_len <= 4)))) {
    return CX_INVALID_PARAMETER;
  }
  if (!((public_key != NULL) && (private_key != NULL))) {
    return CX_INVALID_PARAMETER;
  }

  CX_CHECK(modulus_valid(modulus_len));

  private_key->size = modulus_len;
  public_key->size  = modulus_len;
  CX_CHECK(cx_rsa_get_private_components(private_key, &pv_d, &pv_n));
  CX_CHECK(cx_rsa_get_public_components(public_key, &pu_e, &pu_n));

  size = modulus_len / 2;

  CX_CHECK(cx_bn_lock(size, 0));
  CX_CHECK(cx_bn_alloc(&bn_p, size));
  CX_CHECK(cx_bn_alloc(&bn_q, size));
  // gen prime
  if (externalPQ) {
    CX_CHECK(cx_bn_init(bn_p, externalPQ, size));
    CX_CHECK(cx_bn_init(bn_q, externalPQ + size, size));
  } else {
    CX_CHECK(cx_bn_rand(bn_p));
    CX_CHECK(cx_bn_set_bit(bn_p, size * 8 - 1));
    CX_CHECK(cx_bn_set_bit(bn_p, size * 8 - 2));
    CX_CHECK(cx_bn_rand(bn_q));
    CX_CHECK(cx_bn_set_bit(bn_q, size * 8 - 1));
    CX_CHECK(cx_bn_set_bit(bn_q, size * 8 - 2));
    CX_CHECK(cx_bn_next_prime(bn_p));
    CX_CHECK(cx_bn_next_prime(bn_q));
  }

  // public key:
  CX_CHECK(cx_bn_alloc(&bn_n, modulus_len));
  CX_CHECK(cx_bn_mul(bn_n, bn_p, bn_q));
  CX_CHECK(cx_bn_export(bn_n, pu_n, modulus_len));
  if (exponent == NULL) {
    exponent     = C_default_e;
    exponent_len = sizeof(C_default_e);
  }
  memmove(pu_e + (4 - exponent_len), exponent, exponent_len);
  CX_CHECK(cx_bn_destroy(&bn_n));

  // private key:
  // - n=(p-1)(q-1)
  CX_CHECK(cx_bn_alloc(&bn_n, size));
  CX_CHECK(cx_bn_set_u32(bn_n, 1));
  CX_CHECK_IGNORE_CARRY(cx_bn_sub(bn_p, bn_p, bn_n));
  CX_CHECK_IGNORE_CARRY(cx_bn_sub(bn_q, bn_q, bn_n));
  CX_CHECK(cx_bn_destroy(&bn_n));
  CX_CHECK(cx_bn_alloc(&bn_n, modulus_len));
  CX_CHECK(cx_bn_mul(bn_n, bn_p, bn_q));
  CX_CHECK(cx_bn_destroy(&bn_p));
  CX_CHECK(cx_bn_destroy(&bn_q));
  // - d = inv(e) mod n
  CX_CHECK(cx_bn_alloc(&bn_p, modulus_len));
  cx_bn_mod_u32_invert(bn_p, (pu_e[0] << 24) | (pu_e[1] << 16) | (pu_e[2] << 8) | (pu_e[3] << 0), bn_n);
  CX_CHECK(cx_bn_export(bn_p, pv_d, modulus_len));
  memmove(pv_n, pu_n, modulus_len);

 end:
  cx_bn_unlock();
  return error;
}

cx_err_t cx_rsa_sign_with_salt_len(const cx_rsa_private_key_t *key,
                                   uint32_t                    mode,
                                   cx_md_t                     hashID,
                                   const uint8_t *             hash,
                                   size_t                      hash_len,
                                   uint8_t *                   sig,
                                   size_t                      sig_len,
                                   size_t                      salt_len) {
  uint8_t *key_d;
  uint8_t *key_n;
  cx_bn_t  bn_n, bn_msg, bn_r;
  uint32_t nbits;
  cx_err_t error;

  // cx_scc_struct_check_rsa_privkey(key);
  if (!(hash && (hash_len <= key->size))) {
    return CX_INVALID_PARAMETER;
  }
  if (!(sig && (sig_len >= key->size))) {
    return CX_INVALID_PARAMETER;
  }

  CX_CHECK(cx_rsa_get_private_components(key, &key_d, &key_n));
  CX_CHECK(cx_bn_lock(key->size, 0));
  CX_CHECK(cx_bn_alloc(&bn_r, key->size));
  CX_CHECK(cx_bn_alloc_init(&bn_n, key->size, key_n, key->size));

  // encode
  switch (mode & CX_MASK_PAD) {
  case CX_PAD_PKCS1_1o5:
    sig_len = key->size;
    CX_CHECK(cx_pkcs1_emsa_v1o5_encode(hashID, sig, sig_len, hash, hash_len));
    break;
  case CX_PAD_PKCS1_PSS:
    CX_CHECK(cx_bn_cnt_bits(bn_n, &nbits));
    CX_CHECK(cx_pkcs1_emsa_pss_encode_with_salt_len(hashID, sig, nbits - 1, hash, hash_len, salt_len, &sig_len));
    break;
  default:
    error = CX_INVALID_PARAMETER;
    goto end;
  }

  // encrypt
  CX_CHECK(cx_bn_alloc_init(&bn_msg, key->size, sig, sig_len));
  CX_CHECK(cx_bn_mod_pow2(bn_r, bn_msg, key_d, key->size, bn_n));
  CX_CHECK(cx_bn_export(bn_r, sig, key->size));

 end:
  cx_bn_unlock();
  return error;
}

cx_err_t cx_rsa_sign_no_throw(const cx_rsa_private_key_t *key,
                     uint32_t                    mode,
                     cx_md_t                     hashID,
                     const uint8_t *             hash,
                     size_t                      hash_len,
                     uint8_t *                   sig,
                     size_t                      sig_len) {
  size_t output_hash_len = cx_pkcs1_get_hash_len(hashID);
  return cx_rsa_sign_with_salt_len(key, mode, hashID, hash, hash_len, sig, sig_len, output_hash_len);
}

bool cx_rsa_verify_with_salt_len(const cx_rsa_public_key_t *key,
                                 uint32_t                   mode,
                                 cx_md_t                    hashID,
                                 const uint8_t *            hash,
                                 size_t                     hash_len,
                                 uint8_t *                  sig,
                                 size_t                     sig_len,
                                 size_t                     salt_len) {
  size_t   ok;
  uint8_t *key_n;
  uint8_t *key_e;
  cx_bn_t  bn_n, bn_msg, bn_r;
  uint32_t nbits;
  cx_err_t error;

  if (hash == NULL || hash_len > key->size) {
    return false;
  }
  if (sig == NULL || sig_len != key->size) {
    return false;
  }

  ok = false;

  // decrypt sig
  CX_CHECK(cx_rsa_get_public_components(key, &key_e, &key_n));
  CX_CHECK(cx_bn_lock(key->size, 0));
  CX_CHECK(cx_bn_alloc(&bn_r, key->size));
  CX_CHECK(cx_bn_alloc_init(&bn_n, key->size, key_n, key->size));
  CX_CHECK(cx_bn_alloc_init(&bn_msg, key->size, sig, sig_len));
  CX_CHECK(cx_bn_mod_pow2(bn_r, bn_msg, key_e, 4, bn_n));
  CX_CHECK(cx_bn_export(bn_r, sig, sig_len));

  // verify sig
  switch (mode & CX_MASK_PAD) {
  case CX_PAD_PKCS1_1o5:
    ok = cx_pkcs1_emsa_v1o5_verify(hashID, sig, sig_len, hash, hash_len);
    break;
  case CX_PAD_PKCS1_PSS:
    CX_CHECK(cx_bn_cnt_bits(bn_n, &nbits));
    ok = cx_pkcs1_emsa_pss_verify_with_salt_len(hashID, sig, nbits - 1, hash, hash_len, salt_len);
    break;
  default:
    error = CX_INVALID_PARAMETER;
    goto end;
  }

 end:
  cx_bn_unlock();
  return error == CX_OK && ok;
}

bool cx_rsa_verify(const cx_rsa_public_key_t *key,
                   uint32_t                   mode,
                   cx_md_t                    hashID,
                   const uint8_t *            hash,
                   size_t                     hash_len,
                   uint8_t *                  sig,
                   size_t                     sig_len) {
  size_t output_hash_len = cx_pkcs1_get_hash_len(hashID);
  return cx_rsa_verify_with_salt_len(key, mode, hashID, hash, hash_len, sig, sig_len, output_hash_len);
}

cx_err_t cx_rsa_encrypt_no_throw(const cx_rsa_public_key_t *key,
                        uint32_t                   mode,
                        cx_md_t                    hashID,
                        const uint8_t *            mesg,
                        size_t                     mesg_len,
                        uint8_t *                  enc,
                        size_t                     enc_len) {
  uint8_t *key_n;
  uint8_t *key_e;
  cx_bn_t  bn_n, bn_msg, bn_r;
  cx_err_t error;

  // cx_scc_struct_check_rsa_pubkey(key);
  if (!(mesg && (mesg_len <= key->size))) {
    return CX_INVALID_PARAMETER;
  }
  if (!(enc && (enc_len >= key->size))) {
    return CX_INVALID_PARAMETER;
  }

  // encode
  switch (mode & CX_MASK_PAD) {
  case CX_PAD_PKCS1_1o5:
    enc_len = key->size;
    CX_CHECK(cx_pkcs1_eme_v1o5_encode(hashID, enc, enc_len, mesg, mesg_len));
    break;
  case CX_PAD_PKCS1_OAEP:
    enc_len = key->size;
    CX_CHECK(cx_pkcs1_eme_oaep_encode(hashID, enc, enc_len, mesg, mesg_len));
    break;
  case CX_PAD_NONE:
    memmove(enc, mesg, mesg_len);
    enc_len = mesg_len;
    break;
  default:
    error = CX_INVALID_PARAMETER;
    goto end;
  }

  // encrypt
  CX_CHECK(cx_rsa_get_public_components(key, &key_e, &key_n));
  CX_CHECK(cx_bn_lock(key->size, 0));
  CX_CHECK(cx_bn_alloc(&bn_r, key->size));
  CX_CHECK(cx_bn_alloc_init(&bn_n, key->size, key_n, key->size));
  CX_CHECK(cx_bn_alloc_init(&bn_msg, key->size, enc, enc_len));
  CX_CHECK(cx_bn_mod_pow2(bn_r, bn_msg, key_e, 4, bn_n));
  CX_CHECK(cx_bn_export(bn_r, enc, key->size));

 end:
  cx_bn_unlock();
  return error;
}
cx_err_t cx_rsa_decrypt_no_throw(const cx_rsa_private_key_t *key,
                        uint32_t                    mode,
                        cx_md_t                     hashID,
                        const uint8_t *             mesg,
                        size_t                      mesg_len,
                        uint8_t *                   dec,
                        size_t *                    dec_len) {
  uint8_t *key_n;
  uint8_t *key_d;
  cx_bn_t  bn_n, bn_msg, bn_r;
  cx_err_t error;
  int diff;

  // cx_scc_struct_check_rsa_privkey(key);
  if (!(mesg && (mesg_len == key->size))) {
    return CX_INVALID_PARAMETER;
  }
  if (!(dec && (*dec_len >= key->size))) {
    return CX_INVALID_PARAMETER;
  }

  // decrypt
  CX_CHECK(cx_rsa_get_private_components(key, &key_d, &key_n));
  CX_CHECK(cx_bn_lock(key->size, 0));
  CX_CHECK(cx_bn_alloc(&bn_r, key->size));
  CX_CHECK(cx_bn_alloc_init(&bn_n, key->size, key_n, key->size));
  CX_CHECK(cx_bn_alloc_init(&bn_msg, key->size, mesg, mesg_len));

  // If the encrypted message is greater than the modulus,
  // we consider the encrypted message as incorrect.
  CX_CHECK(cx_bn_cmp(bn_msg, bn_n, &diff));
  if (diff > 0) {
    error = CX_INVALID_PARAMETER;
    goto end;
  }
  else {
    CX_CHECK(cx_bn_mod_pow2(bn_r, bn_msg, key_d, key->size, bn_n));
    CX_CHECK(cx_bn_export(bn_r, dec, key->size));
  }

  switch (mode & CX_MASK_PAD) {
  case CX_PAD_PKCS1_1o5:
    *dec_len = cx_pkcs1_eme_v1o5_decode(hashID, dec, key->size, dec, *dec_len);
    break;
  case CX_PAD_PKCS1_OAEP:
    CX_CHECK(cx_pkcs1_eme_oaep_decode(hashID, dec, key->size, dec, dec_len));
    break;
  case CX_PAD_NONE:
    *dec_len = key->size;
    break;
  default:
    error = CX_INVALID_PARAMETER;
    goto end;
  }

 end:
  cx_bn_unlock();
  return error;
}

#endif // HAVE_RSA
