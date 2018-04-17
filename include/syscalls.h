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
#define SYSCALL_cx_rng_rfc6979_ID_IN 0x60000671UL
#define SYSCALL_cx_rng_rfc6979_ID_OUT 0x90000669UL
#define SYSCALL_cx_hash_ID_IN 0x6000073bUL
#define SYSCALL_cx_hash_ID_OUT 0x900007adUL
#define SYSCALL_cx_ripemd160_init_ID_IN 0x6000087fUL
#define SYSCALL_cx_ripemd160_init_ID_OUT 0x900008f8UL
#define SYSCALL_cx_sha224_init_ID_IN 0x6000095bUL
#define SYSCALL_cx_sha224_init_ID_OUT 0x9000091dUL
#define SYSCALL_cx_sha256_init_ID_IN 0x60000adbUL
#define SYSCALL_cx_sha256_init_ID_OUT 0x90000a64UL
#define SYSCALL_cx_hash_sha256_ID_IN 0x60000b2cUL
#define SYSCALL_cx_hash_sha256_ID_OUT 0x90000ba0UL
#define SYSCALL_cx_sha384_init_ID_IN 0x60000c2bUL
#define SYSCALL_cx_sha384_init_ID_OUT 0x90000cafUL
#define SYSCALL_cx_sha512_init_ID_IN 0x60000dc1UL
#define SYSCALL_cx_sha512_init_ID_OUT 0x90000deeUL
#define SYSCALL_cx_hash_sha512_ID_IN 0x60000e48UL
#define SYSCALL_cx_hash_sha512_ID_OUT 0x90000e55UL
#define SYSCALL_cx_sha3_init_ID_IN 0x60000fd1UL
#define SYSCALL_cx_sha3_init_ID_OUT 0x90000f76UL
#define SYSCALL_cx_keccak_init_ID_IN 0x600010cfUL
#define SYSCALL_cx_keccak_init_ID_OUT 0x900010d8UL
#define SYSCALL_cx_sha3_xof_init_ID_IN 0x60001140UL
#define SYSCALL_cx_sha3_xof_init_ID_OUT 0x900011e0UL
#define SYSCALL_cx_hmac_ripemd160_init_ID_IN 0x600012cfUL
#define SYSCALL_cx_hmac_ripemd160_init_ID_OUT 0x90001276UL
#define SYSCALL_cx_hmac_sha256_init_ID_IN 0x600013d1UL
#define SYSCALL_cx_hmac_sha256_init_ID_OUT 0x90001307UL
#define SYSCALL_cx_hmac_sha512_init_ID_IN 0x600014b3UL
#define SYSCALL_cx_hmac_sha512_init_ID_OUT 0x90001419UL
#define SYSCALL_cx_hmac_ID_IN 0x600015d1UL
#define SYSCALL_cx_hmac_ID_OUT 0x900015d6UL
#define SYSCALL_cx_hmac_sha512_ID_IN 0x600016cfUL
#define SYSCALL_cx_hmac_sha512_ID_OUT 0x9000167eUL
#define SYSCALL_cx_hmac_sha256_ID_IN 0x6000172bUL
#define SYSCALL_cx_hmac_sha256_ID_OUT 0x900017b4UL
#define SYSCALL_cx_pbkdf2_sha512_ID_IN 0x600018e9UL
#define SYSCALL_cx_pbkdf2_sha512_ID_OUT 0x90001883UL
#define SYSCALL_cx_des_init_key_ID_IN 0x60001983UL
#define SYSCALL_cx_des_init_key_ID_OUT 0x9000194dUL
#define SYSCALL_cx_des_iv_ID_IN 0x60001a27UL
#define SYSCALL_cx_des_iv_ID_OUT 0x90001aabUL
#define SYSCALL_cx_des_ID_IN 0x60001b4eUL
#define SYSCALL_cx_des_ID_OUT 0x90001bfeUL
#define SYSCALL_cx_aes_init_key_ID_IN 0x60001c2bUL
#define SYSCALL_cx_aes_init_key_ID_OUT 0x90001c31UL
#define SYSCALL_cx_aes_iv_ID_IN 0x60001d2cUL
#define SYSCALL_cx_aes_iv_ID_OUT 0x90001d1bUL
#define SYSCALL_cx_aes_ID_IN 0x60001ee2UL
#define SYSCALL_cx_aes_ID_OUT 0x90001e3cUL
#define SYSCALL_cx_rsa_init_public_key_ID_IN 0x60001fe5UL
#define SYSCALL_cx_rsa_init_public_key_ID_OUT 0x90001f8bUL
#define SYSCALL_cx_rsa_init_private_key_ID_IN 0x60002037UL
#define SYSCALL_cx_rsa_init_private_key_ID_OUT 0x900020c3UL
#define SYSCALL_cx_rsa_generate_pair_ID_IN 0x600021a1UL
#define SYSCALL_cx_rsa_generate_pair_ID_OUT 0x900021fdUL
#define SYSCALL_cx_rsa_sign_ID_IN 0x600022c6UL
#define SYSCALL_cx_rsa_sign_ID_OUT 0x900022f3UL
#define SYSCALL_cx_rsa_verify_ID_IN 0x600023abUL
#define SYSCALL_cx_rsa_verify_ID_OUT 0x900023b3UL
#define SYSCALL_cx_rsa_encrypt_ID_IN 0x600024daUL
#define SYSCALL_cx_rsa_encrypt_ID_OUT 0x900024c9UL
#define SYSCALL_cx_rsa_decrypt_ID_IN 0x60002584UL
#define SYSCALL_cx_rsa_decrypt_ID_OUT 0x900025a7UL
#define SYSCALL_cx_ecfp_is_valid_point_ID_IN 0x6000266bUL
#define SYSCALL_cx_ecfp_is_valid_point_ID_OUT 0x90002601UL
#define SYSCALL_cx_ecfp_is_cryptographic_point_ID_IN 0x600027e3UL
#define SYSCALL_cx_ecfp_is_cryptographic_point_ID_OUT 0x900027f0UL
#define SYSCALL_cx_ecfp_add_point_ID_IN 0x60002817UL
#define SYSCALL_cx_ecfp_add_point_ID_OUT 0x900028c7UL
#define SYSCALL_cx_ecfp_scalar_mult_ID_IN 0x600029f3UL
#define SYSCALL_cx_ecfp_scalar_mult_ID_OUT 0x900029e3UL
#define SYSCALL_cx_ecfp_init_public_key_ID_IN 0x60002aedUL
#define SYSCALL_cx_ecfp_init_public_key_ID_OUT 0x90002a49UL
#define SYSCALL_cx_ecfp_init_private_key_ID_IN 0x60002beaUL
#define SYSCALL_cx_ecfp_init_private_key_ID_OUT 0x90002b63UL
#define SYSCALL_cx_ecfp_generate_pair_ID_IN 0x60002c2eUL
#define SYSCALL_cx_ecfp_generate_pair_ID_OUT 0x90002c74UL
#define SYSCALL_cx_ecfp_generate_pair2_ID_IN 0x60002d1fUL
#define SYSCALL_cx_ecfp_generate_pair2_ID_OUT 0x90002de6UL
#define SYSCALL_cx_ecschnorr_sign_ID_IN 0x60002e41UL
#define SYSCALL_cx_ecschnorr_sign_ID_OUT 0x90002e4aUL
#define SYSCALL_cx_ecschnorr_verify_ID_IN 0x60002f05UL
#define SYSCALL_cx_ecschnorr_verify_ID_OUT 0x90002ff7UL
#define SYSCALL_cx_edward_compress_point_ID_IN 0x60003059UL
#define SYSCALL_cx_edward_compress_point_ID_OUT 0x9000302bUL
#define SYSCALL_cx_edward_decompress_point_ID_IN 0x60003131UL
#define SYSCALL_cx_edward_decompress_point_ID_OUT 0x900031caUL
#define SYSCALL_cx_eddsa_get_public_key_ID_IN 0x6000321cUL
#define SYSCALL_cx_eddsa_get_public_key_ID_OUT 0x900032bfUL
#define SYSCALL_cx_eddsa_sign_ID_IN 0x6000333bUL
#define SYSCALL_cx_eddsa_sign_ID_OUT 0x900033f6UL
#define SYSCALL_cx_eddsa_verify_ID_IN 0x600034caUL
#define SYSCALL_cx_eddsa_verify_ID_OUT 0x90003421UL
#define SYSCALL_cx_ecdsa_sign_ID_IN 0x600035f3UL
#define SYSCALL_cx_ecdsa_sign_ID_OUT 0x90003576UL
#define SYSCALL_cx_ecdsa_verify_ID_IN 0x600036f1UL
#define SYSCALL_cx_ecdsa_verify_ID_OUT 0x900036e7UL
#define SYSCALL_cx_ecdh_ID_IN 0x6000379dUL
#define SYSCALL_cx_ecdh_ID_OUT 0x90003794UL
#define SYSCALL_cx_crc16_ID_IN 0x6000380eUL
#define SYSCALL_cx_crc16_ID_OUT 0x9000383cUL
#define SYSCALL_cx_crc16_update_ID_IN 0x6000399eUL
#define SYSCALL_cx_crc16_update_ID_OUT 0x900039b9UL
#define SYSCALL_cx_math_cmp_ID_IN 0x60003a5bUL
#define SYSCALL_cx_math_cmp_ID_OUT 0x90003abcUL
#define SYSCALL_cx_math_is_zero_ID_IN 0x60003b37UL
#define SYSCALL_cx_math_is_zero_ID_OUT 0x90003b50UL
#define SYSCALL_cx_math_add_ID_IN 0x60003cfbUL
#define SYSCALL_cx_math_add_ID_OUT 0x90003cd8UL
#define SYSCALL_cx_math_sub_ID_IN 0x60003d9fUL
#define SYSCALL_cx_math_sub_ID_OUT 0x90003d1dUL
#define SYSCALL_cx_math_mult_ID_IN 0x60003e3eUL
#define SYSCALL_cx_math_mult_ID_OUT 0x90003ec2UL
#define SYSCALL_cx_math_addm_ID_IN 0x60003fa6UL
#define SYSCALL_cx_math_addm_ID_OUT 0x90003f48UL
#define SYSCALL_cx_math_subm_ID_IN 0x6000407dUL
#define SYSCALL_cx_math_subm_ID_OUT 0x900040e0UL
#define SYSCALL_cx_math_multm_ID_IN 0x60004145UL
#define SYSCALL_cx_math_multm_ID_OUT 0x900041f3UL
#define SYSCALL_cx_math_powm_ID_IN 0x6000424dUL
#define SYSCALL_cx_math_powm_ID_OUT 0x9000423eUL
#define SYSCALL_cx_math_modm_ID_IN 0x60004345UL
#define SYSCALL_cx_math_modm_ID_OUT 0x9000438cUL
#define SYSCALL_cx_math_invprimem_ID_IN 0x600044e9UL
#define SYSCALL_cx_math_invprimem_ID_OUT 0x90004419UL
#define SYSCALL_cx_math_invintm_ID_IN 0x6000452cUL
#define SYSCALL_cx_math_invintm_ID_OUT 0x90004591UL
#define SYSCALL_cx_math_is_prime_ID_IN 0x60004648UL
#define SYSCALL_cx_math_is_prime_ID_OUT 0x900046faUL
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
#define SYSCALL_os_perso_derive_node_bip32_seed_key_ID_IN 0x600051fbUL
#define SYSCALL_os_perso_derive_node_bip32_seed_key_ID_OUT 0x900051e7UL
#define SYSCALL_os_endorsement_get_code_hash_ID_IN 0x6000520fUL
#define SYSCALL_os_endorsement_get_code_hash_ID_OUT 0x900052a1UL
#define SYSCALL_os_endorsement_get_public_key_ID_IN 0x600053f3UL
#define SYSCALL_os_endorsement_get_public_key_ID_OUT 0x90005399UL
#define SYSCALL_os_endorsement_get_public_key_certificate_ID_IN 0x6000544cUL
#define SYSCALL_os_endorsement_get_public_key_certificate_ID_OUT 0x9000547fUL
#define SYSCALL_os_endorsement_key1_get_app_secret_ID_IN 0x6000555cUL
#define SYSCALL_os_endorsement_key1_get_app_secret_ID_OUT 0x90005560UL
#define SYSCALL_os_endorsement_key1_sign_data_ID_IN 0x600056d8UL
#define SYSCALL_os_endorsement_key1_sign_data_ID_OUT 0x9000562bUL
#define SYSCALL_os_endorsement_key2_derive_sign_data_ID_IN 0x6000574aUL
#define SYSCALL_os_endorsement_key2_derive_sign_data_ID_OUT 0x9000573eUL
#define SYSCALL_os_global_pin_is_validated_ID_IN 0x60005889UL
#define SYSCALL_os_global_pin_is_validated_ID_OUT 0x90005845UL
#define SYSCALL_os_global_pin_check_ID_IN 0x6000596fUL
#define SYSCALL_os_global_pin_check_ID_OUT 0x9000591eUL
#define SYSCALL_os_global_pin_invalidate_ID_IN 0x60005ad0UL
#define SYSCALL_os_global_pin_invalidate_ID_OUT 0x90005afbUL
#define SYSCALL_os_global_pin_retries_ID_IN 0x60005b59UL
#define SYSCALL_os_global_pin_retries_ID_OUT 0x90005b18UL
#define SYSCALL_os_registry_count_ID_IN 0x60005c40UL
#define SYSCALL_os_registry_count_ID_OUT 0x90005c06UL
#define SYSCALL_os_registry_get_ID_IN 0x60005d65UL
#define SYSCALL_os_registry_get_ID_OUT 0x90005db2UL
#define SYSCALL_os_sched_exec_ID_IN 0x60005e14UL
#define SYSCALL_os_sched_exec_ID_OUT 0x90005e9fUL
#define SYSCALL_os_sched_exit_ID_IN 0x60005fe1UL
#define SYSCALL_os_sched_exit_ID_OUT 0x90005f6fUL
#define SYSCALL_os_ux_register_ID_IN 0x60006015UL
#define SYSCALL_os_ux_register_ID_OUT 0x900060b9UL
#define SYSCALL_os_ux_ID_IN 0x60006158UL
#define SYSCALL_os_ux_ID_OUT 0x9000611fUL
#define SYSCALL_os_lib_call_ID_IN 0x6000620bUL
#define SYSCALL_os_lib_call_ID_OUT 0x90006215UL
#define SYSCALL_os_lib_end_ID_IN 0x600063c8UL
#define SYSCALL_os_lib_end_ID_OUT 0x900063e3UL
#define SYSCALL_os_lib_throw_ID_IN 0x60006445UL
#define SYSCALL_os_lib_throw_ID_OUT 0x90006487UL
#define SYSCALL_os_flags_ID_IN 0x6000656eUL
#define SYSCALL_os_flags_ID_OUT 0x9000657fUL
#define SYSCALL_os_version_ID_IN 0x600066b8UL
#define SYSCALL_os_version_ID_OUT 0x900066c4UL
#define SYSCALL_os_seph_features_ID_IN 0x600067d6UL
#define SYSCALL_os_seph_features_ID_OUT 0x90006744UL
#define SYSCALL_os_seph_version_ID_IN 0x600068acUL
#define SYSCALL_os_seph_version_ID_OUT 0x9000685dUL
#define SYSCALL_os_setting_get_ID_IN 0x600069c5UL
#define SYSCALL_os_setting_get_ID_OUT 0x900069afUL
#define SYSCALL_os_setting_set_ID_IN 0x60006a96UL
#define SYSCALL_os_setting_set_ID_OUT 0x90006aa5UL
#define SYSCALL_os_get_memory_info_ID_IN 0x60006b63UL
#define SYSCALL_os_get_memory_info_ID_OUT 0x90006bcbUL
#define SYSCALL_os_registry_get_tag_ID_IN 0x60006c51UL
#define SYSCALL_os_registry_get_tag_ID_OUT 0x90006c89UL
#define SYSCALL_os_customca_verify_ID_IN 0x60006d61UL
#define SYSCALL_os_customca_verify_ID_OUT 0x90006d82UL
#define SYSCALL_io_seproxyhal_spi_send_ID_IN 0x60006e1cUL
#define SYSCALL_io_seproxyhal_spi_send_ID_OUT 0x90006ef3UL
#define SYSCALL_io_seproxyhal_spi_is_status_sent_ID_IN 0x60006fcfUL
#define SYSCALL_io_seproxyhal_spi_is_status_sent_ID_OUT 0x90006f7fUL
#define SYSCALL_io_seproxyhal_spi_recv_ID_IN 0x600070d1UL
#define SYSCALL_io_seproxyhal_spi_recv_ID_OUT 0x9000702bUL
#endif // SYSCALL_DEFS_H
