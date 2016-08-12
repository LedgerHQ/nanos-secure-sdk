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

/* MACHINE GENERATED: DO NOT MODIFY */
#ifndef SYSCALL_DEFS_H
#define SYSCALL_DEFS_H

#define EXC_RETURN_THREAD_PSP 0xFFFFFFFD
#define EXC_RETURN_THREAD_MSP 0xFFFFFFF9
#define EXC_RETURN_HANDLER_MSP 0xFFFFFFF1

// called when entering the SVC Handler C
void syscall_enter(unsigned int syscall_id, unsigned int entry_lr);
void syscall_exit(void);

unsigned int syscall_check_app_address(unsigned int *parameters,
                                       unsigned int checked_idx,
                                       unsigned int checked_length);
unsigned int syscall_check_app_flags(unsigned int flags);

void os_sched_hold_r4_r11(void);
void os_sched_restore_r4_r11(void);
void os_sched_task_save_current(void);

#define SYSCALL_reset_ID_IN 0x60000100UL
#define SYSCALL_reset_ID_OUT 0x900001f1UL
void reset(void);

#define SYSCALL_nvm_write_ID_IN 0x6000027fUL
#define SYSCALL_nvm_write_ID_OUT 0x900002bcUL
void nvm_write(void *dst_adr, void *src_adr, unsigned int src_len);

#define SYSCALL_cx_rng_u8_ID_IN 0x600003c0UL
#define SYSCALL_cx_rng_u8_ID_OUT 0x90000325UL
unsigned char cx_rng_u8(void);

#define SYSCALL_cx_rng_ID_IN 0x6000042cUL
#define SYSCALL_cx_rng_ID_OUT 0x90000467UL
unsigned char *cx_rng(unsigned char *buffer, unsigned int len);

#define SYSCALL_cx_ripemd160_init_ID_IN 0x6000057fUL
#define SYSCALL_cx_ripemd160_init_ID_OUT 0x900005f8UL
int cx_ripemd160_init(cx_ripemd160_t *hash);

#define SYSCALL_cx_sha256_init_ID_IN 0x600006dbUL
#define SYSCALL_cx_sha256_init_ID_OUT 0x90000664UL
int cx_sha256_init(cx_sha256_t *hash);

#define SYSCALL_cx_sha512_init_ID_IN 0x600007c1UL
#define SYSCALL_cx_sha512_init_ID_OUT 0x900007eeUL
int cx_sha512_init(cx_sha512_t *hash);

#define SYSCALL_cx_sha3_init_ID_IN 0x6000080eUL
#define SYSCALL_cx_sha3_init_ID_OUT 0x90000859UL
int cx_sha3_init(cx_sha3_t *hash, int size);

#define SYSCALL_cx_keccak_init_ID_IN 0x6000093cUL
#define SYSCALL_cx_keccak_init_ID_OUT 0x90000939UL
int cx_keccak_init(cx_sha3_t *hash, int size);

#define SYSCALL_cx_sha3_xof_init_ID_IN 0x60000a40UL
#define SYSCALL_cx_sha3_xof_init_ID_OUT 0x90000ae0UL
int cx_sha3_xof_init(cx_sha3_t *hash, unsigned int size,
                     unsigned int out_length);

#define SYSCALL_cx_hash_ID_IN 0x60000ba6UL
#define SYSCALL_cx_hash_ID_OUT 0x90000b46UL
int cx_hash(cx_hash_t *hash, int mode, unsigned char *in, unsigned int len,
            unsigned char *out);

#define SYSCALL_cx_hash_sha256_ID_IN 0x60000c27UL
#define SYSCALL_cx_hash_sha256_ID_OUT 0x90000c1eUL
int cx_hash_sha256(unsigned char *in, unsigned int len, unsigned char *out);

#define SYSCALL_cx_hash_sha512_ID_IN 0x60000d8bUL
#define SYSCALL_cx_hash_sha512_ID_OUT 0x90000ddfUL
int cx_hash_sha512(unsigned char *in, unsigned int len, unsigned char *out);

#define SYSCALL_cx_hmac_ripemd160_init_ID_IN 0x60000ec2UL
#define SYSCALL_cx_hmac_ripemd160_init_ID_OUT 0x90000e58UL
int cx_hmac_ripemd160_init(cx_hmac_ripemd160_t *hmac, unsigned char *key,
                           unsigned int key_len);

#define SYSCALL_cx_hmac_sha256_init_ID_IN 0x60000f8bUL
#define SYSCALL_cx_hmac_sha256_init_ID_OUT 0x90000fceUL
int cx_hmac_sha256_init(cx_hmac_sha256_t *hmac, unsigned char *key,
                        unsigned int key_len);

#define SYSCALL_cx_hmac_sha512_init_ID_IN 0x600010fcUL
#define SYSCALL_cx_hmac_sha512_init_ID_OUT 0x9000100eUL
int cx_hmac_sha512_init(cx_hmac_sha512_t *hmac, unsigned char *key,
                        unsigned int key_len);

#define SYSCALL_cx_hmac_ID_IN 0x60001177UL
#define SYSCALL_cx_hmac_ID_OUT 0x9000116cUL
int cx_hmac(cx_hmac_t *hmac, int mode, unsigned char *in, unsigned int len,
            unsigned char *mac);

#define SYSCALL_cx_hmac_sha512_ID_IN 0x6000121bUL
#define SYSCALL_cx_hmac_sha512_ID_OUT 0x900012ecUL
int cx_hmac_sha512(unsigned char *key, unsigned int key_len, unsigned char *in,
                   unsigned int len, unsigned char *out);

#define SYSCALL_cx_hmac_sha256_ID_IN 0x60001368UL
#define SYSCALL_cx_hmac_sha256_ID_OUT 0x90001321UL
int cx_hmac_sha256(unsigned char *key, unsigned int key_len, unsigned char *in,
                   unsigned int len, unsigned char *out);

#define SYSCALL_cx_pbkdf2_sha512_ID_IN 0x60001490UL
#define SYSCALL_cx_pbkdf2_sha512_ID_OUT 0x9000149dUL
void cx_pbkdf2_sha512(unsigned char *password, unsigned short passwordlen,
                      unsigned char *salt, unsigned short saltlen,
                      unsigned int iterations, unsigned char *out,
                      unsigned int outLength);

#define SYSCALL_cx_des_init_key_ID_IN 0x600015c3UL
#define SYSCALL_cx_des_init_key_ID_OUT 0x900015efUL
int cx_des_init_key(unsigned char *rawkey, unsigned int key_len,
                    cx_des_key_t *key);

#define SYSCALL_cx_des_iv_ID_IN 0x60001601UL
#define SYSCALL_cx_des_iv_ID_OUT 0x900016b7UL
int cx_des_iv(cx_des_key_t *key, int mode, unsigned char *iv, unsigned char *in,
              unsigned int len, unsigned char *out);

#define SYSCALL_cx_des_ID_IN 0x60001734UL
#define SYSCALL_cx_des_ID_OUT 0x900017cdUL
int cx_des(cx_des_key_t *key, int mode, unsigned char *in, unsigned int len,
           unsigned char *out);

#define SYSCALL_cx_aes_init_key_ID_IN 0x60001866UL
#define SYSCALL_cx_aes_init_key_ID_OUT 0x90001826UL
int cx_aes_init_key(unsigned char *rawkey, unsigned int key_len,
                    cx_aes_key_t *key);

#define SYSCALL_cx_aes_iv_ID_IN 0x600019abUL
#define SYSCALL_cx_aes_iv_ID_OUT 0x9000190bUL
int cx_aes_iv(cx_aes_key_t *key, int mode, unsigned char *iv, unsigned char *in,
              unsigned int len, unsigned char *out);

#define SYSCALL_cx_aes_ID_IN 0x60001a21UL
#define SYSCALL_cx_aes_ID_OUT 0x90001a6aUL
int cx_aes(cx_aes_key_t *key, int mode, unsigned char *in, unsigned int len,
           unsigned char *out);

#define SYSCALL_cx_rsa_init_public_key_ID_IN 0x60001b0fUL
#define SYSCALL_cx_rsa_init_public_key_ID_OUT 0x90001b7aUL
int cx_rsa_init_public_key(unsigned char *exponent, unsigned char *modulus,
                           unsigned int modulus_len, cx_rsa_public_key_t *key);

#define SYSCALL_cx_rsa_init_private_key_ID_IN 0x60001c12UL
#define SYSCALL_cx_rsa_init_private_key_ID_OUT 0x90001cb0UL
int cx_rsa_init_private_key(unsigned char *exponent, unsigned char *modulus,
                            unsigned int modulus_len,
                            cx_rsa_private_key_t *key);

#define SYSCALL_cx_rsa_generate_pair_ID_IN 0x60001d2eUL
#define SYSCALL_cx_rsa_generate_pair_ID_OUT 0x90001d42UL
int cx_rsa_generate_pair(unsigned int modulus_len,
                         cx_rsa_public_key_t *public_key,
                         cx_rsa_private_key_t *private_key,
                         unsigned long int pub_exponent);

#define SYSCALL_cx_rsa_sign_ID_IN 0x60001e2dUL
#define SYSCALL_cx_rsa_sign_ID_OUT 0x90001ec8UL
int cx_rsa_sign(cx_rsa_private_key_t *key, int mode, cx_md_t hashID,
                unsigned char *hash, unsigned int hash_len, unsigned char *sig,
                unsigned int sig_len);

#define SYSCALL_cx_rsa_verify_ID_IN 0x60001f10UL
#define SYSCALL_cx_rsa_verify_ID_OUT 0x90001f8cUL
int cx_rsa_verify(cx_rsa_public_key_t *key, int mode, cx_md_t hashID,
                  unsigned char *hash, unsigned int hash_len,
                  unsigned char *sig, unsigned int sig_len);

#define SYSCALL_cx_rsa_encrypt_ID_IN 0x60002028UL
#define SYSCALL_cx_rsa_encrypt_ID_OUT 0x90002092UL
int cx_rsa_encrypt(cx_rsa_public_key_t *key, int mode, cx_md_t hashID,
                   unsigned char *mesg, unsigned int mesg_len,
                   unsigned char *enc, unsigned int enc_len);

#define SYSCALL_cx_rsa_decrypt_ID_IN 0x60002126UL
#define SYSCALL_cx_rsa_decrypt_ID_OUT 0x900021a1UL
int cx_rsa_decrypt(cx_rsa_private_key_t *key, int mode, cx_md_t hashID,
                   unsigned char *mesg, unsigned int mesg_len,
                   unsigned char *dec, unsigned int dec_len);

#define SYSCALL_cx_ecfp_is_valid_point_ID_IN 0x600022a2UL
#define SYSCALL_cx_ecfp_is_valid_point_ID_OUT 0x900022d8UL
int cx_ecfp_is_valid_point(cx_curve_domain_t *domain, unsigned char *point);

#define SYSCALL_cx_ecfp_add_point_ID_IN 0x60002399UL
#define SYSCALL_cx_ecfp_add_point_ID_OUT 0x900023edUL
int cx_ecfp_add_point(cx_curve_domain_t *domain, unsigned char *R,
                      unsigned char *P, unsigned char *Q);

#define SYSCALL_cx_ecfp_scalar_mult_ID_IN 0x600024e5UL
#define SYSCALL_cx_ecfp_scalar_mult_ID_OUT 0x900024d6UL
int cx_ecfp_scalar_mult(cx_curve_domain_t *domain, unsigned char *P,
                        unsigned char *k, unsigned int k_len);

#define SYSCALL_cx_ecfp_init_public_key_ID_IN 0x60002535UL
#define SYSCALL_cx_ecfp_init_public_key_ID_OUT 0x900025f0UL
int cx_ecfp_init_public_key(cx_curve_t curve, unsigned char *rawkey,
                            unsigned int key_len, cx_ecfp_public_key_t *key);

#define SYSCALL_cx_ecfp_init_private_key_ID_IN 0x600026edUL
#define SYSCALL_cx_ecfp_init_private_key_ID_OUT 0x900026aeUL
int cx_ecfp_init_private_key(cx_curve_t curve, unsigned char *rawkey,
                             unsigned int key_len, cx_ecfp_private_key_t *key);

#define SYSCALL_cx_ecfp_generate_pair_ID_IN 0x6000272eUL
#define SYSCALL_cx_ecfp_generate_pair_ID_OUT 0x90002774UL
int cx_ecfp_generate_pair(cx_curve_t curve, cx_ecfp_public_key_t *pubkey,
                          cx_ecfp_private_key_t *privkey, int keepprivate);

#define SYSCALL_cx_borromean_sign_ID_IN 0x60002817UL
#define SYSCALL_cx_borromean_sign_ID_OUT 0x900028ffUL
int cx_borromean_sign(cx_ecfp_private_key_t **privkeys,
                      cx_ecfp_public_key_t **pubkeys, unsigned int *rsizes,
                      unsigned int *pv_keys_index, unsigned int rcount,
                      unsigned int mode, cx_md_t hashID, unsigned char *msg,
                      unsigned int msg_len, unsigned char *sig);

#define SYSCALL_cx_borromean_verify_ID_IN 0x60002908UL
#define SYSCALL_cx_borromean_verify_ID_OUT 0x90002944UL
int cx_borromean_verify(cx_ecfp_public_key_t **pubkeys, unsigned int *rsizes,
                        unsigned int rcount, int mode, cx_md_t hashID,
                        unsigned char *msg, unsigned int msg_len,
                        unsigned char *sig, unsigned int sig_len);

#define SYSCALL_cx_ecschnorr_sign_ID_IN 0x60002a5eUL
#define SYSCALL_cx_ecschnorr_sign_ID_OUT 0x90002acbUL
int cx_ecschnorr_sign(cx_ecfp_private_key_t *pvkey, int mode, cx_md_t hashID,
                      unsigned char *msg, unsigned int msg_len,
                      unsigned char *sig);

#define SYSCALL_cx_ecschnorr_verify_ID_IN 0x60002bf2UL
#define SYSCALL_cx_ecschnorr_verify_ID_OUT 0x90002b73UL
int cx_ecschnorr_verify(cx_ecfp_public_key_t *pukey, int mode, cx_md_t hashID,
                        unsigned char *msg, unsigned int msg_len,
                        unsigned char *sig, unsigned int sig_len);

#define SYSCALL_cx_edward_compress_point_ID_IN 0x60002c0bUL
#define SYSCALL_cx_edward_compress_point_ID_OUT 0x90002cb5UL
void cx_edward_compress_point(cx_curve_twisted_edward_t *domain,
                              unsigned char *P);

#define SYSCALL_cx_edward_decompress_point_ID_IN 0x60002daaUL
#define SYSCALL_cx_edward_decompress_point_ID_OUT 0x90002dd2UL
void cx_edward_decompress_point(cx_curve_twisted_edward_t *domain,
                                unsigned char *P);

#define SYSCALL_cx_eddsa_get_public_key_ID_IN 0x60002e6bUL
#define SYSCALL_cx_eddsa_get_public_key_ID_OUT 0x90002ef0UL
void cx_eddsa_get_public_key(cx_ecfp_private_key_t *pvkey,
                             cx_ecfp_public_key_t *pukey);

#define SYSCALL_cx_eddsa_sign_ID_IN 0x60002f5dUL
#define SYSCALL_cx_eddsa_sign_ID_OUT 0x90002f5eUL
int cx_eddsa_sign(cx_ecfp_private_key_t *pvkey, cx_ecfp_public_key_t *pukey,
                  int mode, cx_md_t hashID, unsigned char *hash,
                  unsigned int hash_len, unsigned char *sig);

#define SYSCALL_cx_eddsa_verify_ID_IN 0x6000309dUL
#define SYSCALL_cx_eddsa_verify_ID_OUT 0x900030bbUL
int cx_eddsa_verify(cx_ecfp_public_key_t *key, int mode, cx_md_t hashID,
                    unsigned char *hash, unsigned int hash_len,
                    unsigned char *sig, unsigned int sig_len);

#define SYSCALL_cx_ecdsa_sign_ID_IN 0x60003101UL
#define SYSCALL_cx_ecdsa_sign_ID_OUT 0x900031f3UL
int cx_ecdsa_sign(cx_ecfp_private_key_t *key, int mode, cx_md_t hashID,
                  unsigned char *hash, unsigned int hash_len,
                  unsigned char *sig);

#define SYSCALL_cx_ecdsa_verify_ID_IN 0x6000323eUL
#define SYSCALL_cx_ecdsa_verify_ID_OUT 0x9000328fUL
int cx_ecdsa_verify(cx_ecfp_public_key_t *key, int mode, cx_md_t hashID,
                    unsigned char *hash, unsigned int hash_len,
                    unsigned char *sig, unsigned int sig_len);

#define SYSCALL_cx_ecdh_ID_IN 0x60003316UL
#define SYSCALL_cx_ecdh_ID_OUT 0x90003361UL
int cx_ecdh(cx_ecfp_private_key_t *key, int mode, unsigned char *public_point,
            unsigned char *secret);

#define SYSCALL_cx_crc16_ID_IN 0x60003437UL
#define SYSCALL_cx_crc16_ID_OUT 0x90003495UL
unsigned short cx_crc16(void *buffer, unsigned int len);

#define SYSCALL_cx_crc16_update_ID_IN 0x600035f4UL
#define SYSCALL_cx_crc16_update_ID_OUT 0x9000357fUL
unsigned short cx_crc16_update(unsigned short crc, void *buffer,
                               unsigned int len);

#define SYSCALL_cx_math_cmp_ID_IN 0x60003673UL
#define SYSCALL_cx_math_cmp_ID_OUT 0x9000368aUL
int cx_math_cmp(unsigned char *a, unsigned char *b, unsigned int len);

#define SYSCALL_cx_math_is_zero_ID_IN 0x60003791UL
#define SYSCALL_cx_math_is_zero_ID_OUT 0x90003723UL
int cx_math_is_zero(unsigned char *a, unsigned int len);

#define SYSCALL_cx_math_add_ID_IN 0x60003850UL
#define SYSCALL_cx_math_add_ID_OUT 0x90003868UL
int cx_math_add(unsigned char *r, unsigned char *a, unsigned char *b,
                unsigned int len);

#define SYSCALL_cx_math_sub_ID_IN 0x60003937UL
#define SYSCALL_cx_math_sub_ID_OUT 0x900039f2UL
int cx_math_sub(unsigned char *r, unsigned char *a, unsigned char *b,
                unsigned int len);

#define SYSCALL_cx_math_mult_ID_IN 0x60003ae1UL
#define SYSCALL_cx_math_mult_ID_OUT 0x90003a67UL
void cx_math_mult(unsigned char *r, unsigned char *a, unsigned char *b,
                  unsigned int len);

#define SYSCALL_cx_math_addm_ID_IN 0x60003bddUL
#define SYSCALL_cx_math_addm_ID_OUT 0x90003b05UL
void cx_math_addm(unsigned char *r, unsigned char *a, unsigned char *b,
                  unsigned char *m, unsigned int len);

#define SYSCALL_cx_math_subm_ID_IN 0x60003c32UL
#define SYSCALL_cx_math_subm_ID_OUT 0x90003c69UL
void cx_math_subm(unsigned char *r, unsigned char *a, unsigned char *b,
                  unsigned char *m, unsigned int len);

#define SYSCALL_cx_math_multm_ID_IN 0x60003d24UL
#define SYSCALL_cx_math_multm_ID_OUT 0x90003dc2UL
void cx_math_multm(unsigned char *r, unsigned char *a, unsigned char *b,
                   unsigned char *m, unsigned int len);

#define SYSCALL_cx_math_modm_ID_IN 0x60003eb0UL
#define SYSCALL_cx_math_modm_ID_OUT 0x90003e9cUL
void cx_math_modm(unsigned char *v, unsigned int len_v, unsigned char *m,
                  unsigned int len_m);

#define SYSCALL_cx_math_invprimem_ID_IN 0x60003f5dUL
#define SYSCALL_cx_math_invprimem_ID_OUT 0x90003f78UL
void cx_math_invprimem(unsigned char *r, unsigned char *a, unsigned char *m,
                       unsigned int len);

#define SYSCALL_cx_math_invintm_ID_IN 0x600040ebUL
#define SYSCALL_cx_math_invintm_ID_OUT 0x900040c1UL
void cx_math_invintm(unsigned char *r, unsigned long int a, unsigned char *m,
                     unsigned int len);

#define SYSCALL_cx_math_is_prime_ID_IN 0x60004195UL
#define SYSCALL_cx_math_is_prime_ID_OUT 0x9000415eUL
int cx_math_is_prime(unsigned char *p, unsigned int len);

#define SYSCALL_cx_math_next_prime_ID_IN 0x600042a4UL
#define SYSCALL_cx_math_next_prime_ID_OUT 0x900042cbUL
void cx_math_next_prime(unsigned char *n, unsigned int len);

#define SYSCALL_os_perso_wipe_ID_IN 0x6000432aUL
#define SYSCALL_os_perso_wipe_ID_OUT 0x90004345UL
void os_perso_wipe(void);

#define SYSCALL_os_perso_set_pin_ID_IN 0x600044ccUL
#define SYSCALL_os_perso_set_pin_ID_OUT 0x900044dfUL
void os_perso_set_pin(unsigned char *pin, unsigned int length);

#define SYSCALL_os_perso_set_seed_ID_IN 0x60004565UL
#define SYSCALL_os_perso_set_seed_ID_OUT 0x900045b1UL
void os_perso_set_seed(unsigned char *seed, unsigned int length);

#define SYSCALL_os_perso_set_words_ID_IN 0x6000463dUL
#define SYSCALL_os_perso_set_words_ID_OUT 0x9000468cUL
void os_perso_set_words(unsigned char *words, unsigned int length);

#define SYSCALL_os_perso_set_devname_ID_IN 0x6000477bUL
#define SYSCALL_os_perso_set_devname_ID_OUT 0x900047beUL
void os_perso_set_devname(unsigned char *devname, unsigned int length);

#define SYSCALL_os_perso_finalize_ID_IN 0x60004880UL
#define SYSCALL_os_perso_finalize_ID_OUT 0x90004854UL
void os_perso_finalize(void);

#define SYSCALL_os_perso_isonboarded_ID_IN 0x6000499aUL
#define SYSCALL_os_perso_isonboarded_ID_OUT 0x900049d5UL
unsigned int os_perso_isonboarded(void);

#define SYSCALL_os_perso_derive_node_bip32_ID_IN 0x60004a2bUL
#define SYSCALL_os_perso_derive_node_bip32_ID_OUT 0x90004a7fUL
void os_perso_derive_node_bip32(cx_curve_t curve, unsigned int *path,
                                unsigned int pathLength,
                                unsigned char *privateKey,
                                unsigned char *chain);

#define SYSCALL_os_endorsement_get_code_hash_ID_IN 0x60004b0fUL
#define SYSCALL_os_endorsement_get_code_hash_ID_OUT 0x90004ba1UL
unsigned int os_endorsement_get_code_hash(unsigned char *buffer);

#define SYSCALL_os_endorsement_get_public_key_ID_IN 0x60004cf3UL
#define SYSCALL_os_endorsement_get_public_key_ID_OUT 0x90004c99UL
unsigned int os_endorsement_get_public_key(unsigned char index,
                                           unsigned char *buffer);

#define SYSCALL_os_endorsement_get_public_key_certificate_ID_IN 0x60004d4cUL
#define SYSCALL_os_endorsement_get_public_key_certificate_ID_OUT 0x90004d7fUL
unsigned int os_endorsement_get_public_key_certificate(unsigned char index,
                                                       unsigned char *buffer);

#define SYSCALL_os_endorsement_key1_get_app_secret_ID_IN 0x60004e5cUL
#define SYSCALL_os_endorsement_key1_get_app_secret_ID_OUT 0x90004e60UL
unsigned int os_endorsement_key1_get_app_secret(unsigned char *buffer);

#define SYSCALL_os_endorsement_key1_sign_data_ID_IN 0x60004fd8UL
#define SYSCALL_os_endorsement_key1_sign_data_ID_OUT 0x90004f2bUL
unsigned int os_endorsement_key1_sign_data(unsigned char *src,
                                           unsigned int srcLength,
                                           unsigned char *signature);

#define SYSCALL_os_endorsement_key2_derive_sign_data_ID_IN 0x6000504aUL
#define SYSCALL_os_endorsement_key2_derive_sign_data_ID_OUT 0x9000503eUL
unsigned int os_endorsement_key2_derive_sign_data(unsigned char *src,
                                                  unsigned int srcLength,
                                                  unsigned char *signature);

#define SYSCALL_os_global_pin_is_validated_ID_IN 0x60005189UL
#define SYSCALL_os_global_pin_is_validated_ID_OUT 0x90005145UL
unsigned int os_global_pin_is_validated(void);

#define SYSCALL_os_global_pin_check_ID_IN 0x6000526fUL
#define SYSCALL_os_global_pin_check_ID_OUT 0x9000521eUL
unsigned int os_global_pin_check(unsigned char *pin_buffer,
                                 unsigned char pin_length);

#define SYSCALL_os_global_pin_invalidate_ID_IN 0x600053d0UL
#define SYSCALL_os_global_pin_invalidate_ID_OUT 0x900053fbUL
void os_global_pin_invalidate(void);

#define SYSCALL_os_global_pin_retries_ID_IN 0x60005459UL
#define SYSCALL_os_global_pin_retries_ID_OUT 0x90005418UL
unsigned int os_global_pin_retries(void);

#define SYSCALL_os_registry_count_ID_IN 0x60005540UL
#define SYSCALL_os_registry_count_ID_OUT 0x90005506UL
unsigned int os_registry_count(void);

#define SYSCALL_os_registry_get_ID_IN 0x60005665UL
#define SYSCALL_os_registry_get_ID_OUT 0x900056b2UL
void os_registry_get(unsigned int index, application_t *out_application_entry);

#define SYSCALL_os_sched_exec_ID_IN 0x60005714UL
#define SYSCALL_os_sched_exec_ID_OUT 0x9000579fUL
unsigned int os_sched_exec(unsigned int application_index);

#define SYSCALL_os_sched_exit_ID_IN 0x600058e1UL
#define SYSCALL_os_sched_exit_ID_OUT 0x9000586fUL
void os_sched_exit(unsigned int exit_code);

#define SYSCALL_os_ux_register_ID_IN 0x60005915UL
#define SYSCALL_os_ux_register_ID_OUT 0x900059b9UL
void os_ux_register(bolos_ux_params_t *parameter_ram_pointer);

#define SYSCALL_os_ux_ID_IN 0x60005a58UL
#define SYSCALL_os_ux_ID_OUT 0x90005a1fUL
unsigned int os_ux(bolos_ux_params_t *params);

#define SYSCALL_os_flags_ID_IN 0x60005b6eUL
#define SYSCALL_os_flags_ID_OUT 0x90005b7fUL
unsigned int os_flags(void);

#define SYSCALL_os_version_ID_IN 0x60005cb8UL
#define SYSCALL_os_version_ID_OUT 0x90005cc4UL
unsigned int os_version(unsigned char *version, unsigned int maxlength);

#define SYSCALL_os_seph_features_ID_IN 0x60005dd6UL
#define SYSCALL_os_seph_features_ID_OUT 0x90005d44UL
unsigned int os_seph_features(void);

#define SYSCALL_os_seph_version_ID_IN 0x60005eacUL
#define SYSCALL_os_seph_version_ID_OUT 0x90005e5dUL
unsigned int os_seph_version(unsigned char *version, unsigned int maxlength);

#define SYSCALL_os_setting_get_ID_IN 0x60005f97UL
#define SYSCALL_os_setting_get_ID_OUT 0x90005f03UL
unsigned int os_setting_get(unsigned int setting_id);

#define SYSCALL_os_setting_set_ID_IN 0x60006067UL
#define SYSCALL_os_setting_set_ID_OUT 0x9000608dUL
void os_setting_set(unsigned int setting_id, unsigned int value);

#define SYSCALL_io_seproxyhal_spi_send_ID_IN 0x6000611cUL
#define SYSCALL_io_seproxyhal_spi_send_ID_OUT 0x900061f3UL
void io_seproxyhal_spi_send(const unsigned char *buffer, unsigned short length);

#define SYSCALL_io_seproxyhal_spi_is_status_sent_ID_IN 0x600062cfUL
#define SYSCALL_io_seproxyhal_spi_is_status_sent_ID_OUT 0x9000627fUL
unsigned int io_seproxyhal_spi_is_status_sent(void);

#define SYSCALL_io_seproxyhal_spi_recv_ID_IN 0x600063d1UL
#define SYSCALL_io_seproxyhal_spi_recv_ID_OUT 0x9000632bUL
unsigned short io_seproxyhal_spi_recv(unsigned char *buffer,
                                      unsigned short maxlength,
                                      unsigned int flags);

#endif // SYSCALL_DEFS_H
