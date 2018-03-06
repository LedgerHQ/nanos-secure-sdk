/*******************************************************************************
*   Ledger Nano S - Secure firmware
*   (c) 2016, 2017, 2018 Ledger
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
void syscall_enter(unsigned int syscall_id, unsigned int psp);
void syscall_exit(void);
unsigned int syscall_get_caller_try_ctx(unsigned int *stack_ptr);

unsigned int syscall_check_app_address(const unsigned int *const parameters,
                                       unsigned int checked_idx,
                                       unsigned int checked_length);
unsigned int syscall_check_app_flags(unsigned int flags);
unsigned int syscall_check_app_cxport(unsigned int cxport);

void os_sched_hold_r4_r11(void);
void os_sched_restore_r4_r11(unsigned int sp) __attribute((naked));
#define SYSCALL_check_api_level_ID_IN 0x60000137UL
#define SYSCALL_check_api_level_ID_OUT 0x900001c6UL
#define SYSCALL_reset_ID_IN 0x60000200UL
#define SYSCALL_reset_ID_OUT 0x900002f1UL
#define SYSCALL_nvm_write_ID_IN 0x6000037fUL
#define SYSCALL_nvm_write_ID_OUT 0x900003bcUL
#define SYSCALL_cx_rng_u8_ID_IN 0x600004c0UL
#define SYSCALL_cx_rng_u8_ID_OUT 0x90000425UL
#define SYSCALL_cx_rng_ID_IN 0x6000052cUL
#define SYSCALL_cx_rng_ID_OUT 0x90000567UL
#define SYSCALL_cx_rng_rfc6979_ID_IN 0x60000617UL
#define SYSCALL_cx_rng_rfc6979_ID_OUT 0x9000060eUL
#define SYSCALL_cx_ripemd160_init_ID_IN 0x6000077fUL
#define SYSCALL_cx_ripemd160_init_ID_OUT 0x900007f8UL
#define SYSCALL_cx_sha224_init_ID_IN 0x6000085bUL
#define SYSCALL_cx_sha224_init_ID_OUT 0x9000081dUL
#define SYSCALL_cx_sha256_init_ID_IN 0x600009dbUL
#define SYSCALL_cx_sha256_init_ID_OUT 0x90000964UL
#define SYSCALL_cx_sha384_init_ID_IN 0x60000a2bUL
#define SYSCALL_cx_sha384_init_ID_OUT 0x90000aafUL
#define SYSCALL_cx_sha512_init_ID_IN 0x60000bc1UL
#define SYSCALL_cx_sha512_init_ID_OUT 0x90000beeUL
#define SYSCALL_cx_sha3_init_ID_IN 0x60000c0eUL
#define SYSCALL_cx_sha3_init_ID_OUT 0x90000c59UL
#define SYSCALL_cx_keccak_init_ID_IN 0x60000d3cUL
#define SYSCALL_cx_keccak_init_ID_OUT 0x90000d39UL
#define SYSCALL_cx_sha3_xof_init_ID_IN 0x60000e40UL
#define SYSCALL_cx_sha3_xof_init_ID_OUT 0x90000ee0UL
#define SYSCALL_cx_hash_ID_IN 0x60000fa6UL
#define SYSCALL_cx_hash_ID_OUT 0x90000f46UL
#define SYSCALL_cx_hash_sha256_ID_IN 0x60001027UL
#define SYSCALL_cx_hash_sha256_ID_OUT 0x9000101eUL
#define SYSCALL_cx_hash_sha512_ID_IN 0x6000118bUL
#define SYSCALL_cx_hash_sha512_ID_OUT 0x900011dfUL
#define SYSCALL_cx_hmac_ripemd160_init_ID_IN 0x600012c2UL
#define SYSCALL_cx_hmac_ripemd160_init_ID_OUT 0x90001258UL
#define SYSCALL_cx_hmac_sha256_init_ID_IN 0x6000138bUL
#define SYSCALL_cx_hmac_sha256_init_ID_OUT 0x900013ceUL
#define SYSCALL_cx_hmac_sha512_init_ID_IN 0x600014fcUL
#define SYSCALL_cx_hmac_sha512_init_ID_OUT 0x9000140eUL
#define SYSCALL_cx_hmac_ID_IN 0x60001577UL
#define SYSCALL_cx_hmac_ID_OUT 0x9000156cUL
#define SYSCALL_cx_hmac_sha512_ID_IN 0x6000161bUL
#define SYSCALL_cx_hmac_sha512_ID_OUT 0x900016ecUL
#define SYSCALL_cx_hmac_sha256_ID_IN 0x60001768UL
#define SYSCALL_cx_hmac_sha256_ID_OUT 0x90001721UL
#define SYSCALL_cx_pbkdf2_sha512_ID_IN 0x60001890UL
#define SYSCALL_cx_pbkdf2_sha512_ID_OUT 0x9000189dUL
#define SYSCALL_cx_des_init_key_ID_IN 0x600019c3UL
#define SYSCALL_cx_des_init_key_ID_OUT 0x900019efUL
#define SYSCALL_cx_des_iv_ID_IN 0x60001a01UL
#define SYSCALL_cx_des_iv_ID_OUT 0x90001ab7UL
#define SYSCALL_cx_des_ID_IN 0x60001b34UL
#define SYSCALL_cx_des_ID_OUT 0x90001bcdUL
#define SYSCALL_cx_aes_init_key_ID_IN 0x60001c66UL
#define SYSCALL_cx_aes_init_key_ID_OUT 0x90001c26UL
#define SYSCALL_cx_aes_iv_ID_IN 0x60001dabUL
#define SYSCALL_cx_aes_iv_ID_OUT 0x90001d0bUL
#define SYSCALL_cx_aes_ID_IN 0x60001e21UL
#define SYSCALL_cx_aes_ID_OUT 0x90001e6aUL
#define SYSCALL_cx_rsa_init_public_key_ID_IN 0x60001f0fUL
#define SYSCALL_cx_rsa_init_public_key_ID_OUT 0x90001f7aUL
#define SYSCALL_cx_rsa_init_private_key_ID_IN 0x60002012UL
#define SYSCALL_cx_rsa_init_private_key_ID_OUT 0x900020b0UL
#define SYSCALL_cx_rsa_generate_pair_ID_IN 0x60002162UL
#define SYSCALL_cx_rsa_generate_pair_ID_OUT 0x90002176UL
#define SYSCALL_cx_rsa_sign_ID_IN 0x6000222dUL
#define SYSCALL_cx_rsa_sign_ID_OUT 0x900022c8UL
#define SYSCALL_cx_rsa_verify_ID_IN 0x60002310UL
#define SYSCALL_cx_rsa_verify_ID_OUT 0x9000238cUL
#define SYSCALL_cx_rsa_encrypt_ID_IN 0x60002428UL
#define SYSCALL_cx_rsa_encrypt_ID_OUT 0x90002492UL
#define SYSCALL_cx_rsa_decrypt_ID_IN 0x60002526UL
#define SYSCALL_cx_rsa_decrypt_ID_OUT 0x900025a1UL
#define SYSCALL_cx_ecfp_is_valid_point_ID_IN 0x60002610UL
#define SYSCALL_cx_ecfp_is_valid_point_ID_OUT 0x90002655UL
#define SYSCALL_cx_ecfp_is_cryptographic_point_ID_IN 0x60002748UL
#define SYSCALL_cx_ecfp_is_cryptographic_point_ID_OUT 0x9000279eUL
#define SYSCALL_cx_ecfp_add_point_ID_IN 0x6000282aUL
#define SYSCALL_cx_ecfp_add_point_ID_OUT 0x9000281bUL
#define SYSCALL_cx_ecfp_scalar_mult_ID_IN 0x6000297bUL
#define SYSCALL_cx_ecfp_scalar_mult_ID_OUT 0x9000293fUL
#define SYSCALL_cx_ecfp_init_public_key_ID_IN 0x60002a35UL
#define SYSCALL_cx_ecfp_init_public_key_ID_OUT 0x90002af0UL
#define SYSCALL_cx_ecfp_init_private_key_ID_IN 0x60002b1cUL
#define SYSCALL_cx_ecfp_init_private_key_ID_OUT 0x90002be6UL
#define SYSCALL_cx_ecfp_generate_pair_ID_IN 0x60002c2eUL
#define SYSCALL_cx_ecfp_generate_pair_ID_OUT 0x90002c74UL
#define SYSCALL_cx_ecfp_generate_pair2_ID_IN 0x60002d1fUL
#define SYSCALL_cx_ecfp_generate_pair2_ID_OUT 0x90002de6UL
#define SYSCALL_cx_ecschnorr_sign_ID_IN 0x60002e94UL
#define SYSCALL_cx_ecschnorr_sign_ID_OUT 0x90002eaaUL
#define SYSCALL_cx_ecschnorr_verify_ID_IN 0x60002ff2UL
#define SYSCALL_cx_ecschnorr_verify_ID_OUT 0x90002f73UL
#define SYSCALL_cx_edward_compress_point_ID_IN 0x600030ecUL
#define SYSCALL_cx_edward_compress_point_ID_OUT 0x90003026UL
#define SYSCALL_cx_edward_decompress_point_ID_IN 0x6000318eUL
#define SYSCALL_cx_edward_decompress_point_ID_OUT 0x9000317dUL
#define SYSCALL_cx_eddsa_get_public_key_ID_IN 0x60003218UL
#define SYSCALL_cx_eddsa_get_public_key_ID_OUT 0x900032c0UL
#define SYSCALL_cx_eddsa_sign_ID_IN 0x6000330bUL
#define SYSCALL_cx_eddsa_sign_ID_OUT 0x900033abUL
#define SYSCALL_cx_eddsa_verify_ID_IN 0x600034d3UL
#define SYSCALL_cx_eddsa_verify_ID_OUT 0x900034dfUL
#define SYSCALL_cx_ecdsa_sign_ID_IN 0x60003568UL
#define SYSCALL_cx_ecdsa_sign_ID_OUT 0x900035b1UL
#define SYSCALL_cx_ecdsa_verify_ID_IN 0x6000363eUL
#define SYSCALL_cx_ecdsa_verify_ID_OUT 0x9000368fUL
#define SYSCALL_cx_ecdh_ID_IN 0x6000373eUL
#define SYSCALL_cx_ecdh_ID_OUT 0x900037b1UL
#define SYSCALL_cx_crc16_ID_IN 0x60003837UL
#define SYSCALL_cx_crc16_ID_OUT 0x90003895UL
#define SYSCALL_cx_crc16_update_ID_IN 0x600039f4UL
#define SYSCALL_cx_crc16_update_ID_OUT 0x9000397fUL
#define SYSCALL_cx_math_cmp_ID_IN 0x60003a73UL
#define SYSCALL_cx_math_cmp_ID_OUT 0x90003a8aUL
#define SYSCALL_cx_math_is_zero_ID_IN 0x60003b91UL
#define SYSCALL_cx_math_is_zero_ID_OUT 0x90003b23UL
#define SYSCALL_cx_math_add_ID_IN 0x60003c50UL
#define SYSCALL_cx_math_add_ID_OUT 0x90003c68UL
#define SYSCALL_cx_math_sub_ID_IN 0x60003d37UL
#define SYSCALL_cx_math_sub_ID_OUT 0x90003df2UL
#define SYSCALL_cx_math_mult_ID_IN 0x60003ee1UL
#define SYSCALL_cx_math_mult_ID_OUT 0x90003e67UL
#define SYSCALL_cx_math_addm_ID_IN 0x60003fddUL
#define SYSCALL_cx_math_addm_ID_OUT 0x90003f05UL
#define SYSCALL_cx_math_subm_ID_IN 0x60004032UL
#define SYSCALL_cx_math_subm_ID_OUT 0x90004069UL
#define SYSCALL_cx_math_multm_ID_IN 0x60004124UL
#define SYSCALL_cx_math_multm_ID_OUT 0x900041c2UL
#define SYSCALL_cx_math_powm_ID_IN 0x600042adUL
#define SYSCALL_cx_math_powm_ID_OUT 0x90004264UL
#define SYSCALL_cx_math_modm_ID_IN 0x600043b0UL
#define SYSCALL_cx_math_modm_ID_OUT 0x9000439cUL
#define SYSCALL_cx_math_invprimem_ID_IN 0x6000445dUL
#define SYSCALL_cx_math_invprimem_ID_OUT 0x90004478UL
#define SYSCALL_cx_math_invintm_ID_IN 0x600045ebUL
#define SYSCALL_cx_math_invintm_ID_OUT 0x900045c1UL
#define SYSCALL_cx_math_is_prime_ID_IN 0x60004695UL
#define SYSCALL_cx_math_is_prime_ID_OUT 0x9000465eUL
#define SYSCALL_cx_math_next_prime_ID_IN 0x600047a4UL
#define SYSCALL_cx_math_next_prime_ID_OUT 0x900047cbUL
#define SYSCALL_os_perso_erase_all_ID_IN 0x600048f5UL
#define SYSCALL_os_perso_erase_all_ID_OUT 0x900048f4UL
#define SYSCALL_os_perso_set_pin_ID_IN 0x600049dfUL
#define SYSCALL_os_perso_set_pin_ID_OUT 0x90004995UL
#define SYSCALL_os_perso_set_current_identity_pin_ID_IN 0x60004afeUL
#define SYSCALL_os_perso_set_current_identity_pin_ID_OUT 0x90004a0aUL
#define SYSCALL_os_perso_set_seed_ID_IN 0x60004bbcUL
#define SYSCALL_os_perso_set_seed_ID_OUT 0x90004beaUL
#define SYSCALL_os_perso_derive_and_set_seed_ID_IN 0x60004cbdUL
#define SYSCALL_os_perso_derive_and_set_seed_ID_OUT 0x90004ccfUL
#define SYSCALL_os_perso_set_words_ID_IN 0x60004d18UL
#define SYSCALL_os_perso_set_words_ID_OUT 0x90004deaUL
#define SYSCALL_os_perso_finalize_ID_IN 0x60004e80UL
#define SYSCALL_os_perso_finalize_ID_OUT 0x90004e54UL
#define SYSCALL_os_perso_isonboarded_ID_IN 0x60004f9aUL
#define SYSCALL_os_perso_isonboarded_ID_OUT 0x90004fd5UL
#define SYSCALL_os_perso_derive_node_bip32_ID_IN 0x600050baUL
#define SYSCALL_os_perso_derive_node_bip32_ID_OUT 0x9000501eUL
#define SYSCALL_os_endorsement_get_code_hash_ID_IN 0x6000510fUL
#define SYSCALL_os_endorsement_get_code_hash_ID_OUT 0x900051a1UL
#define SYSCALL_os_endorsement_get_public_key_ID_IN 0x600052f3UL
#define SYSCALL_os_endorsement_get_public_key_ID_OUT 0x90005299UL
#define SYSCALL_os_endorsement_get_public_key_certificate_ID_IN 0x6000534cUL
#define SYSCALL_os_endorsement_get_public_key_certificate_ID_OUT 0x9000537fUL
#define SYSCALL_os_endorsement_key1_get_app_secret_ID_IN 0x6000545cUL
#define SYSCALL_os_endorsement_key1_get_app_secret_ID_OUT 0x90005460UL
#define SYSCALL_os_endorsement_key1_sign_data_ID_IN 0x600055d8UL
#define SYSCALL_os_endorsement_key1_sign_data_ID_OUT 0x9000552bUL
#define SYSCALL_os_endorsement_key2_derive_sign_data_ID_IN 0x6000564aUL
#define SYSCALL_os_endorsement_key2_derive_sign_data_ID_OUT 0x9000563eUL
#define SYSCALL_os_global_pin_is_validated_ID_IN 0x60005789UL
#define SYSCALL_os_global_pin_is_validated_ID_OUT 0x90005745UL
#define SYSCALL_os_global_pin_check_ID_IN 0x6000586fUL
#define SYSCALL_os_global_pin_check_ID_OUT 0x9000581eUL
#define SYSCALL_os_global_pin_invalidate_ID_IN 0x600059d0UL
#define SYSCALL_os_global_pin_invalidate_ID_OUT 0x900059fbUL
#define SYSCALL_os_global_pin_retries_ID_IN 0x60005a59UL
#define SYSCALL_os_global_pin_retries_ID_OUT 0x90005a18UL
#define SYSCALL_os_registry_count_ID_IN 0x60005b40UL
#define SYSCALL_os_registry_count_ID_OUT 0x90005b06UL
#define SYSCALL_os_registry_get_ID_IN 0x60005c65UL
#define SYSCALL_os_registry_get_ID_OUT 0x90005cb2UL
#define SYSCALL_os_sched_exec_ID_IN 0x60005d14UL
#define SYSCALL_os_sched_exec_ID_OUT 0x90005d9fUL
#define SYSCALL_os_sched_exit_ID_IN 0x60005ee1UL
#define SYSCALL_os_sched_exit_ID_OUT 0x90005e6fUL
#define SYSCALL_os_ux_register_ID_IN 0x60005f15UL
#define SYSCALL_os_ux_register_ID_OUT 0x90005fb9UL
#define SYSCALL_os_ux_ID_IN 0x60006058UL
#define SYSCALL_os_ux_ID_OUT 0x9000601fUL
#define SYSCALL_os_lib_call_ID_IN 0x6000610bUL
#define SYSCALL_os_lib_call_ID_OUT 0x90006115UL
#define SYSCALL_os_lib_end_ID_IN 0x600062c8UL
#define SYSCALL_os_lib_end_ID_OUT 0x900062e3UL
#define SYSCALL_os_lib_throw_ID_IN 0x60006345UL
#define SYSCALL_os_lib_throw_ID_OUT 0x90006387UL
#define SYSCALL_os_flags_ID_IN 0x6000646eUL
#define SYSCALL_os_flags_ID_OUT 0x9000647fUL
#define SYSCALL_os_version_ID_IN 0x600065b8UL
#define SYSCALL_os_version_ID_OUT 0x900065c4UL
#define SYSCALL_os_seph_features_ID_IN 0x600066d6UL
#define SYSCALL_os_seph_features_ID_OUT 0x90006644UL
#define SYSCALL_os_seph_version_ID_IN 0x600067acUL
#define SYSCALL_os_seph_version_ID_OUT 0x9000675dUL
#define SYSCALL_os_setting_get_ID_IN 0x600068c5UL
#define SYSCALL_os_setting_get_ID_OUT 0x900068afUL
#define SYSCALL_os_setting_set_ID_IN 0x60006996UL
#define SYSCALL_os_setting_set_ID_OUT 0x900069a5UL
#define SYSCALL_os_get_memory_info_ID_IN 0x60006a63UL
#define SYSCALL_os_get_memory_info_ID_OUT 0x90006acbUL
#define SYSCALL_os_registry_get_tag_ID_IN 0x60006b51UL
#define SYSCALL_os_registry_get_tag_ID_OUT 0x90006b89UL
#define SYSCALL_os_customca_verify_ID_IN 0x60006c61UL
#define SYSCALL_os_customca_verify_ID_OUT 0x90006c82UL
#define SYSCALL_io_seproxyhal_spi_send_ID_IN 0x60006d1cUL
#define SYSCALL_io_seproxyhal_spi_send_ID_OUT 0x90006df3UL
#define SYSCALL_io_seproxyhal_spi_is_status_sent_ID_IN 0x60006ecfUL
#define SYSCALL_io_seproxyhal_spi_is_status_sent_ID_OUT 0x90006e7fUL
#define SYSCALL_io_seproxyhal_spi_recv_ID_IN 0x60006fd1UL
#define SYSCALL_io_seproxyhal_spi_recv_ID_OUT 0x90006f2bUL
#endif // SYSCALL_DEFS_H
