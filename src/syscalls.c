
/* MACHINE GENERATED: DO NOT MODIFY */
#define SYSCALL_STUB
#include "os.h"
#include "syscalls.h"

// avoid a separate asm file, but avoid any intrusion from the compiler
__attribute__((naked)) void SVC_Call(unsigned int syscall_id, volatile unsigned int * parameters);
__attribute__((naked)) void SVC_Call(__attribute__((unused)) unsigned int syscall_id, __attribute__((unused)) volatile unsigned int * parameters) {
  // delegate svc, ensure no optimization by gcc with naked and r0, r1 marked as clobbered
  asm volatile("svc #1":::"r0","r1");
  asm volatile("bx  lr");
}
void check_api_level ( unsigned int apiLevel ) 
{
  volatile unsigned int parameters [2+1];
  parameters[0] = (unsigned int)apiLevel;
  SVC_Call(SYSCALL_check_api_level_ID_IN, parameters);
}

void halt ( void ) 
{
  volatile unsigned int parameters [2];
  SVC_Call(SYSCALL_halt_ID_IN, parameters);
}

void nvm_write ( void * dst_adr, void * src_adr, unsigned int src_len ) 
{
  volatile unsigned int parameters [2+3];
  parameters[0] = (unsigned int)dst_adr;
  parameters[1] = (unsigned int)src_adr;
  parameters[2] = (unsigned int)src_len;
  SVC_Call(SYSCALL_nvm_write_ID_IN, parameters);
}

unsigned char cx_rng_u8 ( void ) 
{
  volatile unsigned int parameters [2];
#ifdef __clang_analyzer__
  parameters[1] = 0;
#endif
  SVC_Call(SYSCALL_cx_rng_u8_ID_IN, parameters);
  return (unsigned char)(((volatile unsigned int*)parameters)[1]);
}

unsigned char * cx_rng ( unsigned char * buffer, unsigned int len ) 
{
  volatile unsigned int parameters [2+2];
  parameters[0] = (unsigned int)buffer;
  parameters[1] = (unsigned int)len;
  SVC_Call(SYSCALL_cx_rng_ID_IN, parameters);
  return (unsigned char *)(((volatile unsigned int*)parameters)[1]);
}

int cx_hash ( cx_hash_t * hash, int mode, const unsigned char * in, unsigned int len, unsigned char * out, unsigned int out_len ) 
{
  volatile unsigned int parameters [2+6];
  parameters[0] = (unsigned int)hash;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)in;
  parameters[3] = (unsigned int)len;
  parameters[4] = (unsigned int)out;
  parameters[5] = (unsigned int)out_len;
  SVC_Call(SYSCALL_cx_hash_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_ripemd160_init ( cx_ripemd160_t * hash ) 
{
  volatile unsigned int parameters [2+1];
#ifdef __clang_analyzer__
  parameters[1] = 0;
#endif
  parameters[0] = (unsigned int)hash;
  SVC_Call(SYSCALL_cx_ripemd160_init_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_sha224_init ( cx_sha256_t * hash ) 
{
  volatile unsigned int parameters [2+1];
#ifdef __clang_analyzer__
  parameters[1] = 0;
#endif
  parameters[0] = (unsigned int)hash;
  SVC_Call(SYSCALL_cx_sha224_init_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_sha256_init ( cx_sha256_t * hash ) 
{
  volatile unsigned int parameters [2+1];
#ifdef __clang_analyzer__
  parameters[1] = 0;
#endif
  parameters[0] = (unsigned int)hash;
  SVC_Call(SYSCALL_cx_sha256_init_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_hash_sha256 ( const unsigned char * in, unsigned int len, unsigned char * out, unsigned int out_len ) 
{
  volatile unsigned int parameters [2+4];
  parameters[0] = (unsigned int)in;
  parameters[1] = (unsigned int)len;
  parameters[2] = (unsigned int)out;
  parameters[3] = (unsigned int)out_len;
  SVC_Call(SYSCALL_cx_hash_sha256_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_sha384_init ( cx_sha512_t * hash ) 
{
  volatile unsigned int parameters [2+1];
#ifdef __clang_analyzer__
  parameters[1] = 0;
#endif
  parameters[0] = (unsigned int)hash;
  SVC_Call(SYSCALL_cx_sha384_init_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_sha512_init ( cx_sha512_t * hash ) 
{
  volatile unsigned int parameters [2+1];
#ifdef __clang_analyzer__
  parameters[1] = 0;
#endif
  parameters[0] = (unsigned int)hash;
  SVC_Call(SYSCALL_cx_sha512_init_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_hash_sha512 ( const unsigned char * in, unsigned int len, unsigned char * out, unsigned int out_len ) 
{
  volatile unsigned int parameters [2+4];
  parameters[0] = (unsigned int)in;
  parameters[1] = (unsigned int)len;
  parameters[2] = (unsigned int)out;
  parameters[3] = (unsigned int)out_len;
  SVC_Call(SYSCALL_cx_hash_sha512_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_sha3_init ( cx_sha3_t * hash, unsigned int size ) 
{
  volatile unsigned int parameters [2+2];
  parameters[0] = (unsigned int)hash;
  parameters[1] = (unsigned int)size;
  SVC_Call(SYSCALL_cx_sha3_init_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_keccak_init ( cx_sha3_t * hash, unsigned int size ) 
{
  volatile unsigned int parameters [2+2];
  parameters[0] = (unsigned int)hash;
  parameters[1] = (unsigned int)size;
  SVC_Call(SYSCALL_cx_keccak_init_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_shake128_init ( cx_sha3_t * hash, unsigned int out_size ) 
{
  volatile unsigned int parameters [2+2];
  parameters[0] = (unsigned int)hash;
  parameters[1] = (unsigned int)out_size;
  SVC_Call(SYSCALL_cx_shake128_init_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_shake256_init ( cx_sha3_t * hash, unsigned int out_size ) 
{
  volatile unsigned int parameters [2+2];
  parameters[0] = (unsigned int)hash;
  parameters[1] = (unsigned int)out_size;
  SVC_Call(SYSCALL_cx_shake256_init_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_sha3_xof_init ( cx_sha3_t * hash, unsigned int size, unsigned int out_length ) 
{
  volatile unsigned int parameters [2+3];
  parameters[0] = (unsigned int)hash;
  parameters[1] = (unsigned int)size;
  parameters[2] = (unsigned int)out_length;
  SVC_Call(SYSCALL_cx_sha3_xof_init_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_blake2b_init ( cx_blake2b_t * hash, unsigned int out_len ) 
{
  volatile unsigned int parameters [2+2];
  parameters[0] = (unsigned int)hash;
  parameters[1] = (unsigned int)out_len;
  SVC_Call(SYSCALL_cx_blake2b_init_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_blake2b_init2 ( cx_blake2b_t * hash, unsigned int out_len, unsigned char * salt, unsigned int salt_len, unsigned char * perso, unsigned int perso_len ) 
{
  volatile unsigned int parameters [2+6];
  parameters[0] = (unsigned int)hash;
  parameters[1] = (unsigned int)out_len;
  parameters[2] = (unsigned int)salt;
  parameters[3] = (unsigned int)salt_len;
  parameters[4] = (unsigned int)perso;
  parameters[5] = (unsigned int)perso_len;
  SVC_Call(SYSCALL_cx_blake2b_init2_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_groestl_init ( cx_groestl_t * hash, unsigned int size ) 
{
  volatile unsigned int parameters [2+2];
  parameters[0] = (unsigned int)hash;
  parameters[1] = (unsigned int)size;
  SVC_Call(SYSCALL_cx_groestl_init_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_hmac_ripemd160_init ( cx_hmac_ripemd160_t * hmac, const unsigned char * key, unsigned int key_len ) 
{
  volatile unsigned int parameters [2+3];
  parameters[0] = (unsigned int)hmac;
  parameters[1] = (unsigned int)key;
  parameters[2] = (unsigned int)key_len;
  SVC_Call(SYSCALL_cx_hmac_ripemd160_init_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_hmac_sha256_init ( cx_hmac_sha256_t * hmac, const unsigned char * key, unsigned int key_len ) 
{
  volatile unsigned int parameters [2+3];
  parameters[0] = (unsigned int)hmac;
  parameters[1] = (unsigned int)key;
  parameters[2] = (unsigned int)key_len;
  SVC_Call(SYSCALL_cx_hmac_sha256_init_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_hmac_sha512_init ( cx_hmac_sha512_t * hmac, const unsigned char * key, unsigned int key_len ) 
{
  volatile unsigned int parameters [2+3];
  parameters[0] = (unsigned int)hmac;
  parameters[1] = (unsigned int)key;
  parameters[2] = (unsigned int)key_len;
  SVC_Call(SYSCALL_cx_hmac_sha512_init_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_hmac ( cx_hmac_t * hmac, int mode, const unsigned char * in, unsigned int len, unsigned char * mac, unsigned int mac_len ) 
{
  volatile unsigned int parameters [2+6];
  parameters[0] = (unsigned int)hmac;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)in;
  parameters[3] = (unsigned int)len;
  parameters[4] = (unsigned int)mac;
  parameters[5] = (unsigned int)mac_len;
  SVC_Call(SYSCALL_cx_hmac_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_hmac_sha512 ( const unsigned char * key, unsigned int key_len, const unsigned char * in, unsigned int len, unsigned char * mac, unsigned int mac_len ) 
{
  volatile unsigned int parameters [2+6];
  parameters[0] = (unsigned int)key;
  parameters[1] = (unsigned int)key_len;
  parameters[2] = (unsigned int)in;
  parameters[3] = (unsigned int)len;
  parameters[4] = (unsigned int)mac;
  parameters[5] = (unsigned int)mac_len;
  SVC_Call(SYSCALL_cx_hmac_sha512_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_hmac_sha256 ( const unsigned char * key, unsigned int key_len, const unsigned char * in, unsigned int len, unsigned char * mac, unsigned int mac_len ) 
{
  volatile unsigned int parameters [2+6];
  parameters[0] = (unsigned int)key;
  parameters[1] = (unsigned int)key_len;
  parameters[2] = (unsigned int)in;
  parameters[3] = (unsigned int)len;
  parameters[4] = (unsigned int)mac;
  parameters[5] = (unsigned int)mac_len;
  SVC_Call(SYSCALL_cx_hmac_sha256_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

void cx_pbkdf2 ( cx_md_t md_type, const unsigned char * password, unsigned short passwordlen, unsigned char * salt, unsigned short saltlen, unsigned int iterations, unsigned char * out, unsigned int outLength ) 
{
  volatile unsigned int parameters [2+8];
  parameters[0] = (unsigned int)md_type;
  parameters[1] = (unsigned int)password;
  parameters[2] = (unsigned int)passwordlen;
  parameters[3] = (unsigned int)salt;
  parameters[4] = (unsigned int)saltlen;
  parameters[5] = (unsigned int)iterations;
  parameters[6] = (unsigned int)out;
  parameters[7] = (unsigned int)outLength;
  SVC_Call(SYSCALL_cx_pbkdf2_ID_IN, parameters);
}

int cx_des_init_key ( const unsigned char * rawkey, unsigned int key_len, cx_des_key_t * key ) 
{
  volatile unsigned int parameters [2+3];
  parameters[0] = (unsigned int)rawkey;
  parameters[1] = (unsigned int)key_len;
  parameters[2] = (unsigned int)key;
  SVC_Call(SYSCALL_cx_des_init_key_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_des_iv ( const cx_des_key_t * key, int mode, unsigned char * iv, unsigned int iv_len, const unsigned char * in, unsigned int in_len, unsigned char * out, unsigned int out_len ) 
{
  volatile unsigned int parameters [2+8];
  parameters[0] = (unsigned int)key;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)iv;
  parameters[3] = (unsigned int)iv_len;
  parameters[4] = (unsigned int)in;
  parameters[5] = (unsigned int)in_len;
  parameters[6] = (unsigned int)out;
  parameters[7] = (unsigned int)out_len;
  SVC_Call(SYSCALL_cx_des_iv_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_des ( const cx_des_key_t * key, int mode, const unsigned char * in, unsigned int in_len, unsigned char * out, unsigned int out_len ) 
{
  volatile unsigned int parameters [2+6];
  parameters[0] = (unsigned int)key;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)in;
  parameters[3] = (unsigned int)in_len;
  parameters[4] = (unsigned int)out;
  parameters[5] = (unsigned int)out_len;
  SVC_Call(SYSCALL_cx_des_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_aes_init_key ( const unsigned char * rawkey, unsigned int key_len, cx_aes_key_t * key ) 
{
  volatile unsigned int parameters [2+3];
  parameters[0] = (unsigned int)rawkey;
  parameters[1] = (unsigned int)key_len;
  parameters[2] = (unsigned int)key;
  SVC_Call(SYSCALL_cx_aes_init_key_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_aes_iv ( const cx_aes_key_t * key, int mode, unsigned char * iv, unsigned int iv_len, const unsigned char * in, unsigned int in_len, unsigned char * out, unsigned int out_len ) 
{
  volatile unsigned int parameters [2+8];
  parameters[0] = (unsigned int)key;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)iv;
  parameters[3] = (unsigned int)iv_len;
  parameters[4] = (unsigned int)in;
  parameters[5] = (unsigned int)in_len;
  parameters[6] = (unsigned int)out;
  parameters[7] = (unsigned int)out_len;
  SVC_Call(SYSCALL_cx_aes_iv_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_aes ( const cx_aes_key_t * key, int mode, const unsigned char * in, unsigned int in_len, unsigned char * out, unsigned int out_len ) 
{
  volatile unsigned int parameters [2+6];
  parameters[0] = (unsigned int)key;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)in;
  parameters[3] = (unsigned int)in_len;
  parameters[4] = (unsigned int)out;
  parameters[5] = (unsigned int)out_len;
  SVC_Call(SYSCALL_cx_aes_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_rsa_init_public_key ( const unsigned char * exponent, unsigned int exponent_len, const unsigned char * modulus, unsigned int modulus_len, cx_rsa_public_key_t * key ) 
{
  volatile unsigned int parameters [2+5];
  parameters[0] = (unsigned int)exponent;
  parameters[1] = (unsigned int)exponent_len;
  parameters[2] = (unsigned int)modulus;
  parameters[3] = (unsigned int)modulus_len;
  parameters[4] = (unsigned int)key;
  SVC_Call(SYSCALL_cx_rsa_init_public_key_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_rsa_init_private_key ( const unsigned char * exponent, unsigned int exponent_len, const unsigned char * modulus, unsigned int modulus_len, cx_rsa_private_key_t * key ) 
{
  volatile unsigned int parameters [2+5];
  parameters[0] = (unsigned int)exponent;
  parameters[1] = (unsigned int)exponent_len;
  parameters[2] = (unsigned int)modulus;
  parameters[3] = (unsigned int)modulus_len;
  parameters[4] = (unsigned int)key;
  SVC_Call(SYSCALL_cx_rsa_init_private_key_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_rsa_generate_pair ( unsigned int modulus_len, cx_rsa_public_key_t * public_key, cx_rsa_private_key_t * private_key, const unsigned char * pub_exponent, unsigned int exponent_len, const unsigned char * externalPQ ) 
{
  volatile unsigned int parameters [2+6];
  parameters[0] = (unsigned int)modulus_len;
  parameters[1] = (unsigned int)public_key;
  parameters[2] = (unsigned int)private_key;
  parameters[3] = (unsigned int)pub_exponent;
  parameters[4] = (unsigned int)exponent_len;
  parameters[5] = (unsigned int)externalPQ;
  SVC_Call(SYSCALL_cx_rsa_generate_pair_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_rsa_sign ( const cx_rsa_private_key_t * key, int mode, cx_md_t hashID, const unsigned char * hash, unsigned int hash_len, unsigned char * sig, unsigned int sig_len ) 
{
  volatile unsigned int parameters [2+7];
  parameters[0] = (unsigned int)key;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)hashID;
  parameters[3] = (unsigned int)hash;
  parameters[4] = (unsigned int)hash_len;
  parameters[5] = (unsigned int)sig;
  parameters[6] = (unsigned int)sig_len;
  SVC_Call(SYSCALL_cx_rsa_sign_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_rsa_verify ( const cx_rsa_public_key_t * key, int mode, cx_md_t hashID, const unsigned char * hash, unsigned int hash_len, const unsigned char * sig, unsigned int sig_len ) 
{
  volatile unsigned int parameters [2+7];
  parameters[0] = (unsigned int)key;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)hashID;
  parameters[3] = (unsigned int)hash;
  parameters[4] = (unsigned int)hash_len;
  parameters[5] = (unsigned int)sig;
  parameters[6] = (unsigned int)sig_len;
  SVC_Call(SYSCALL_cx_rsa_verify_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_rsa_encrypt ( const cx_rsa_public_key_t * key, int mode, cx_md_t hashID, const unsigned char * mesg, unsigned int mesg_len, unsigned char * enc, unsigned int enc_len ) 
{
  volatile unsigned int parameters [2+7];
  parameters[0] = (unsigned int)key;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)hashID;
  parameters[3] = (unsigned int)mesg;
  parameters[4] = (unsigned int)mesg_len;
  parameters[5] = (unsigned int)enc;
  parameters[6] = (unsigned int)enc_len;
  SVC_Call(SYSCALL_cx_rsa_encrypt_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_rsa_decrypt ( const cx_rsa_private_key_t * key, int mode, cx_md_t hashID, const unsigned char * mesg, unsigned int mesg_len, unsigned char * dec, unsigned int dec_len ) 
{
  volatile unsigned int parameters [2+7];
  parameters[0] = (unsigned int)key;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)hashID;
  parameters[3] = (unsigned int)mesg;
  parameters[4] = (unsigned int)mesg_len;
  parameters[5] = (unsigned int)dec;
  parameters[6] = (unsigned int)dec_len;
  SVC_Call(SYSCALL_cx_rsa_decrypt_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_ecfp_is_valid_point ( cx_curve_t curve, const unsigned char * P, unsigned int P_len ) 
{
  volatile unsigned int parameters [2+3];
  parameters[0] = (unsigned int)curve;
  parameters[1] = (unsigned int)P;
  parameters[2] = (unsigned int)P_len;
  SVC_Call(SYSCALL_cx_ecfp_is_valid_point_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_ecfp_is_cryptographic_point ( cx_curve_t curve, const unsigned char * P, unsigned int P_len ) 
{
  volatile unsigned int parameters [2+3];
  parameters[0] = (unsigned int)curve;
  parameters[1] = (unsigned int)P;
  parameters[2] = (unsigned int)P_len;
  SVC_Call(SYSCALL_cx_ecfp_is_cryptographic_point_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_ecfp_add_point ( cx_curve_t curve, unsigned char * R, const unsigned char * P, const unsigned char * Q, unsigned int X_len ) 
{
  volatile unsigned int parameters [2+5];
  parameters[0] = (unsigned int)curve;
  parameters[1] = (unsigned int)R;
  parameters[2] = (unsigned int)P;
  parameters[3] = (unsigned int)Q;
  parameters[4] = (unsigned int)X_len;
  SVC_Call(SYSCALL_cx_ecfp_add_point_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_ecfp_scalar_mult ( cx_curve_t curve, unsigned char * P, unsigned int P_len, const unsigned char * k, unsigned int k_len ) 
{
  volatile unsigned int parameters [2+5];
  parameters[0] = (unsigned int)curve;
  parameters[1] = (unsigned int)P;
  parameters[2] = (unsigned int)P_len;
  parameters[3] = (unsigned int)k;
  parameters[4] = (unsigned int)k_len;
  SVC_Call(SYSCALL_cx_ecfp_scalar_mult_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_ecfp_init_public_key ( cx_curve_t curve, const unsigned char * rawkey, unsigned int key_len, cx_ecfp_public_key_t * key ) 
{
  volatile unsigned int parameters [2+4];
  parameters[0] = (unsigned int)curve;
  parameters[1] = (unsigned int)rawkey;
  parameters[2] = (unsigned int)key_len;
  parameters[3] = (unsigned int)key;
  SVC_Call(SYSCALL_cx_ecfp_init_public_key_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_ecfp_init_private_key ( cx_curve_t curve, const unsigned char * rawkey, unsigned int key_len, cx_ecfp_private_key_t * pvkey ) 
{
  volatile unsigned int parameters [2+4];
  parameters[0] = (unsigned int)curve;
  parameters[1] = (unsigned int)rawkey;
  parameters[2] = (unsigned int)key_len;
  parameters[3] = (unsigned int)pvkey;
  SVC_Call(SYSCALL_cx_ecfp_init_private_key_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_ecfp_generate_pair ( cx_curve_t curve, cx_ecfp_public_key_t * pubkey, cx_ecfp_private_key_t * privkey, int keepprivate ) 
{
  volatile unsigned int parameters [2+4];
  parameters[0] = (unsigned int)curve;
  parameters[1] = (unsigned int)pubkey;
  parameters[2] = (unsigned int)privkey;
  parameters[3] = (unsigned int)keepprivate;
  SVC_Call(SYSCALL_cx_ecfp_generate_pair_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_ecfp_generate_pair2 ( cx_curve_t curve, cx_ecfp_public_key_t * pubkey, cx_ecfp_private_key_t * privkey, int keepprivate, cx_md_t hashID ) 
{
  volatile unsigned int parameters [2+5];
  parameters[0] = (unsigned int)curve;
  parameters[1] = (unsigned int)pubkey;
  parameters[2] = (unsigned int)privkey;
  parameters[3] = (unsigned int)keepprivate;
  parameters[4] = (unsigned int)hashID;
  SVC_Call(SYSCALL_cx_ecfp_generate_pair2_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_ecdsa_sign ( const cx_ecfp_private_key_t * pvkey, int mode, cx_md_t hashID, const unsigned char * hash, unsigned int hash_len, unsigned char * sig, unsigned int sig_len, unsigned int * info ) 
{
  volatile unsigned int parameters [2+8];
  parameters[0] = (unsigned int)pvkey;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)hashID;
  parameters[3] = (unsigned int)hash;
  parameters[4] = (unsigned int)hash_len;
  parameters[5] = (unsigned int)sig;
  parameters[6] = (unsigned int)sig_len;
  parameters[7] = (unsigned int)info;
  SVC_Call(SYSCALL_cx_ecdsa_sign_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_ecdsa_verify ( const cx_ecfp_public_key_t * pukey, int mode, cx_md_t hashID, const unsigned char * hash, unsigned int hash_len, const unsigned char * sig, unsigned int sig_len ) 
{
  volatile unsigned int parameters [2+7];
  parameters[0] = (unsigned int)pukey;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)hashID;
  parameters[3] = (unsigned int)hash;
  parameters[4] = (unsigned int)hash_len;
  parameters[5] = (unsigned int)sig;
  parameters[6] = (unsigned int)sig_len;
  SVC_Call(SYSCALL_cx_ecdsa_verify_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

void cx_edward_compress_point ( cx_curve_t curve, unsigned char * P, unsigned int P_len ) 
{
  volatile unsigned int parameters [2+3];
  parameters[0] = (unsigned int)curve;
  parameters[1] = (unsigned int)P;
  parameters[2] = (unsigned int)P_len;
  SVC_Call(SYSCALL_cx_edward_compress_point_ID_IN, parameters);
}

void cx_edward_decompress_point ( cx_curve_t curve, unsigned char * P, unsigned int P_len ) 
{
  volatile unsigned int parameters [2+3];
  parameters[0] = (unsigned int)curve;
  parameters[1] = (unsigned int)P;
  parameters[2] = (unsigned int)P_len;
  SVC_Call(SYSCALL_cx_edward_decompress_point_ID_IN, parameters);
}

void cx_eddsa_get_public_key ( const cx_ecfp_private_key_t * pvkey, cx_md_t hashID, cx_ecfp_public_key_t * pukey, unsigned char * a, unsigned int a_len, unsigned char * h, unsigned int h_len ) 
{
  volatile unsigned int parameters [2+7];
  parameters[0] = (unsigned int)pvkey;
  parameters[1] = (unsigned int)hashID;
  parameters[2] = (unsigned int)pukey;
  parameters[3] = (unsigned int)a;
  parameters[4] = (unsigned int)a_len;
  parameters[5] = (unsigned int)h;
  parameters[6] = (unsigned int)h_len;
  SVC_Call(SYSCALL_cx_eddsa_get_public_key_ID_IN, parameters);
}

int cx_eddsa_sign ( const cx_ecfp_private_key_t * pvkey, int mode, cx_md_t hashID, const unsigned char * hash, unsigned int hash_len, const unsigned char * ctx, unsigned int ctx_len, unsigned char * sig, unsigned int sig_len, unsigned int * info ) 
{
  volatile unsigned int parameters [2+10];
  parameters[0] = (unsigned int)pvkey;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)hashID;
  parameters[3] = (unsigned int)hash;
  parameters[4] = (unsigned int)hash_len;
  parameters[5] = (unsigned int)ctx;
  parameters[6] = (unsigned int)ctx_len;
  parameters[7] = (unsigned int)sig;
  parameters[8] = (unsigned int)sig_len;
  parameters[9] = (unsigned int)info;
  SVC_Call(SYSCALL_cx_eddsa_sign_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_eddsa_verify ( const cx_ecfp_public_key_t * pukey, int mode, cx_md_t hashID, const unsigned char * hash, unsigned int hash_len, const unsigned char * ctx, unsigned int ctx_len, const unsigned char * sig, unsigned int sig_len ) 
{
  volatile unsigned int parameters [2+9];
  parameters[0] = (unsigned int)pukey;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)hashID;
  parameters[3] = (unsigned int)hash;
  parameters[4] = (unsigned int)hash_len;
  parameters[5] = (unsigned int)ctx;
  parameters[6] = (unsigned int)ctx_len;
  parameters[7] = (unsigned int)sig;
  parameters[8] = (unsigned int)sig_len;
  SVC_Call(SYSCALL_cx_eddsa_verify_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_ecschnorr_sign ( const cx_ecfp_private_key_t * pvkey, int mode, cx_md_t hashID, const unsigned char * msg, unsigned int msg_len, unsigned char * sig, unsigned int sig_len, unsigned int * info ) 
{
  volatile unsigned int parameters [2+8];
  parameters[0] = (unsigned int)pvkey;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)hashID;
  parameters[3] = (unsigned int)msg;
  parameters[4] = (unsigned int)msg_len;
  parameters[5] = (unsigned int)sig;
  parameters[6] = (unsigned int)sig_len;
  parameters[7] = (unsigned int)info;
  SVC_Call(SYSCALL_cx_ecschnorr_sign_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_ecschnorr_verify ( const cx_ecfp_public_key_t * pukey, int mode, cx_md_t hashID, const unsigned char * msg, unsigned int msg_len, const unsigned char * sig, unsigned int sig_len ) 
{
  volatile unsigned int parameters [2+7];
  parameters[0] = (unsigned int)pukey;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)hashID;
  parameters[3] = (unsigned int)msg;
  parameters[4] = (unsigned int)msg_len;
  parameters[5] = (unsigned int)sig;
  parameters[6] = (unsigned int)sig_len;
  SVC_Call(SYSCALL_cx_ecschnorr_verify_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_ecdh ( const cx_ecfp_private_key_t * pvkey, int mode, const unsigned char * P, unsigned int P_len, unsigned char * secret, unsigned int secret_len ) 
{
  volatile unsigned int parameters [2+6];
  parameters[0] = (unsigned int)pvkey;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)P;
  parameters[3] = (unsigned int)P_len;
  parameters[4] = (unsigned int)secret;
  parameters[5] = (unsigned int)secret_len;
  SVC_Call(SYSCALL_cx_ecdh_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

unsigned short cx_crc16 ( const void * buffer, size_t len ) 
{
  volatile unsigned int parameters [2+2];
  parameters[0] = (unsigned int)buffer;
  parameters[1] = (unsigned int)len;
  SVC_Call(SYSCALL_cx_crc16_ID_IN, parameters);
  return (unsigned short)(((volatile unsigned int*)parameters)[1]);
}

unsigned short cx_crc16_update ( unsigned short crc, const void * buffer, size_t len ) 
{
  volatile unsigned int parameters [2+3];
  parameters[0] = (unsigned int)crc;
  parameters[1] = (unsigned int)buffer;
  parameters[2] = (unsigned int)len;
  SVC_Call(SYSCALL_cx_crc16_update_ID_IN, parameters);
  return (unsigned short)(((volatile unsigned int*)parameters)[1]);
}

int cx_math_cmp ( const unsigned char * a, const unsigned char * b, unsigned int len ) 
{
  volatile unsigned int parameters [2+3];
  parameters[0] = (unsigned int)a;
  parameters[1] = (unsigned int)b;
  parameters[2] = (unsigned int)len;
  SVC_Call(SYSCALL_cx_math_cmp_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_math_is_zero ( const unsigned char * a, unsigned int len ) 
{
  volatile unsigned int parameters [2+2];
  parameters[0] = (unsigned int)a;
  parameters[1] = (unsigned int)len;
  SVC_Call(SYSCALL_cx_math_is_zero_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_math_add ( unsigned char * r, const unsigned char * a, const unsigned char * b, unsigned int len ) 
{
  volatile unsigned int parameters [2+4];
  parameters[0] = (unsigned int)r;
  parameters[1] = (unsigned int)a;
  parameters[2] = (unsigned int)b;
  parameters[3] = (unsigned int)len;
  SVC_Call(SYSCALL_cx_math_add_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

int cx_math_sub ( unsigned char * r, const unsigned char * a, const unsigned char * b, unsigned int len ) 
{
  volatile unsigned int parameters [2+4];
  parameters[0] = (unsigned int)r;
  parameters[1] = (unsigned int)a;
  parameters[2] = (unsigned int)b;
  parameters[3] = (unsigned int)len;
  SVC_Call(SYSCALL_cx_math_sub_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

void cx_math_mult ( unsigned char * r, const unsigned char * a, const unsigned char * b, unsigned int len ) 
{
  volatile unsigned int parameters [2+4];
  parameters[0] = (unsigned int)r;
  parameters[1] = (unsigned int)a;
  parameters[2] = (unsigned int)b;
  parameters[3] = (unsigned int)len;
  SVC_Call(SYSCALL_cx_math_mult_ID_IN, parameters);
}

void cx_math_addm ( unsigned char * r, const unsigned char * a, const unsigned char * b, const unsigned char * m, unsigned int len ) 
{
  volatile unsigned int parameters [2+5];
  parameters[0] = (unsigned int)r;
  parameters[1] = (unsigned int)a;
  parameters[2] = (unsigned int)b;
  parameters[3] = (unsigned int)m;
  parameters[4] = (unsigned int)len;
  SVC_Call(SYSCALL_cx_math_addm_ID_IN, parameters);
}

void cx_math_subm ( unsigned char * r, const unsigned char * a, const unsigned char * b, const unsigned char * m, unsigned int len ) 
{
  volatile unsigned int parameters [2+5];
  parameters[0] = (unsigned int)r;
  parameters[1] = (unsigned int)a;
  parameters[2] = (unsigned int)b;
  parameters[3] = (unsigned int)m;
  parameters[4] = (unsigned int)len;
  SVC_Call(SYSCALL_cx_math_subm_ID_IN, parameters);
}

void cx_math_multm ( unsigned char * r, const unsigned char * a, const unsigned char * b, const unsigned char * m, unsigned int len ) 
{
  volatile unsigned int parameters [2+5];
  parameters[0] = (unsigned int)r;
  parameters[1] = (unsigned int)a;
  parameters[2] = (unsigned int)b;
  parameters[3] = (unsigned int)m;
  parameters[4] = (unsigned int)len;
  SVC_Call(SYSCALL_cx_math_multm_ID_IN, parameters);
}

void cx_math_powm ( unsigned char * r, const unsigned char * a, const unsigned char * e, unsigned int len_e, const unsigned char * m, unsigned int len ) 
{
  volatile unsigned int parameters [2+6];
  parameters[0] = (unsigned int)r;
  parameters[1] = (unsigned int)a;
  parameters[2] = (unsigned int)e;
  parameters[3] = (unsigned int)len_e;
  parameters[4] = (unsigned int)m;
  parameters[5] = (unsigned int)len;
  SVC_Call(SYSCALL_cx_math_powm_ID_IN, parameters);
}

void cx_math_modm ( unsigned char * v, unsigned int len_v, const unsigned char * m, unsigned int len_m ) 
{
  volatile unsigned int parameters [2+4];
  parameters[0] = (unsigned int)v;
  parameters[1] = (unsigned int)len_v;
  parameters[2] = (unsigned int)m;
  parameters[3] = (unsigned int)len_m;
  SVC_Call(SYSCALL_cx_math_modm_ID_IN, parameters);
}

void cx_math_invprimem ( unsigned char * r, const unsigned char * a, const unsigned char * m, unsigned int len ) 
{
  volatile unsigned int parameters [2+4];
  parameters[0] = (unsigned int)r;
  parameters[1] = (unsigned int)a;
  parameters[2] = (unsigned int)m;
  parameters[3] = (unsigned int)len;
  SVC_Call(SYSCALL_cx_math_invprimem_ID_IN, parameters);
}

void cx_math_invintm ( unsigned char * r, unsigned long int a, const unsigned char * m, unsigned int len ) 
{
  volatile unsigned int parameters [2+4];
  parameters[0] = (unsigned int)r;
  parameters[1] = (unsigned int)a;
  parameters[2] = (unsigned int)m;
  parameters[3] = (unsigned int)len;
  SVC_Call(SYSCALL_cx_math_invintm_ID_IN, parameters);
}

int cx_math_is_prime ( const unsigned char * p, unsigned int len ) 
{
  volatile unsigned int parameters [2+2];
  parameters[0] = (unsigned int)p;
  parameters[1] = (unsigned int)len;
  SVC_Call(SYSCALL_cx_math_is_prime_ID_IN, parameters);
  return (int)(((volatile unsigned int*)parameters)[1]);
}

void cx_math_next_prime ( unsigned char * n, unsigned int len ) 
{
  volatile unsigned int parameters [2+2];
  parameters[0] = (unsigned int)n;
  parameters[1] = (unsigned int)len;
  SVC_Call(SYSCALL_cx_math_next_prime_ID_IN, parameters);
}

void os_perso_erase_all ( void ) 
{
  volatile unsigned int parameters [2];
  SVC_Call(SYSCALL_os_perso_erase_all_ID_IN, parameters);
}

void os_perso_set_pin ( unsigned int identity, unsigned char * pin, unsigned int length ) 
{
  volatile unsigned int parameters [2+3];
  parameters[0] = (unsigned int)identity;
  parameters[1] = (unsigned int)pin;
  parameters[2] = (unsigned int)length;
  SVC_Call(SYSCALL_os_perso_set_pin_ID_IN, parameters);
}

void os_perso_set_current_identity_pin ( unsigned char * pin, unsigned int length ) 
{
  volatile unsigned int parameters [2+2];
  parameters[0] = (unsigned int)pin;
  parameters[1] = (unsigned int)length;
  SVC_Call(SYSCALL_os_perso_set_current_identity_pin_ID_IN, parameters);
}

void os_perso_set_seed ( unsigned int identity, unsigned int algorithm, unsigned char * seed, unsigned int length ) 
{
  volatile unsigned int parameters [2+4];
  parameters[0] = (unsigned int)identity;
  parameters[1] = (unsigned int)algorithm;
  parameters[2] = (unsigned int)seed;
  parameters[3] = (unsigned int)length;
  SVC_Call(SYSCALL_os_perso_set_seed_ID_IN, parameters);
}

void os_perso_derive_and_set_seed ( unsigned char identity, const char * prefix, unsigned int prefix_length, const char * passphrase, unsigned int passphrase_length, const char * words, unsigned int words_length ) 
{
  volatile unsigned int parameters [2+7];
  parameters[0] = (unsigned int)identity;
  parameters[1] = (unsigned int)prefix;
  parameters[2] = (unsigned int)prefix_length;
  parameters[3] = (unsigned int)passphrase;
  parameters[4] = (unsigned int)passphrase_length;
  parameters[5] = (unsigned int)words;
  parameters[6] = (unsigned int)words_length;
  SVC_Call(SYSCALL_os_perso_derive_and_set_seed_ID_IN, parameters);
}

void os_perso_set_words ( const unsigned char * words, unsigned int length ) 
{
  volatile unsigned int parameters [2+2];
  parameters[0] = (unsigned int)words;
  parameters[1] = (unsigned int)length;
  SVC_Call(SYSCALL_os_perso_set_words_ID_IN, parameters);
}

void os_perso_finalize ( void ) 
{
  volatile unsigned int parameters [2];
  SVC_Call(SYSCALL_os_perso_finalize_ID_IN, parameters);
}

bolos_bool_t os_perso_isonboarded ( void ) 
{
  volatile unsigned int parameters [2];
#ifdef __clang_analyzer__
  parameters[1] = 0;
#endif
  SVC_Call(SYSCALL_os_perso_isonboarded_ID_IN, parameters);
  return (bolos_bool_t)(((volatile unsigned int*)parameters)[1]);
}

void os_perso_derive_node_bip32 ( cx_curve_t curve, const unsigned int * path, unsigned int pathLength, unsigned char * privateKey, unsigned char * chain ) 
{
  volatile unsigned int parameters [2+5];
  parameters[0] = (unsigned int)curve;
  parameters[1] = (unsigned int)path;
  parameters[2] = (unsigned int)pathLength;
  parameters[3] = (unsigned int)privateKey;
  parameters[4] = (unsigned int)chain;
  SVC_Call(SYSCALL_os_perso_derive_node_bip32_ID_IN, parameters);
}

void os_perso_derive_node_with_seed_key ( unsigned int mode, cx_curve_t curve, const unsigned int * path, unsigned int pathLength, unsigned char * privateKey, unsigned char * chain, unsigned char * seed_key, unsigned int seed_key_length ) 
{
  volatile unsigned int parameters [2+8];
  parameters[0] = (unsigned int)mode;
  parameters[1] = (unsigned int)curve;
  parameters[2] = (unsigned int)path;
  parameters[3] = (unsigned int)pathLength;
  parameters[4] = (unsigned int)privateKey;
  parameters[5] = (unsigned int)chain;
  parameters[6] = (unsigned int)seed_key;
  parameters[7] = (unsigned int)seed_key_length;
  SVC_Call(SYSCALL_os_perso_derive_node_with_seed_key_ID_IN, parameters);
}

unsigned int os_perso_seed_cookie ( unsigned char * seed_cookie, unsigned int seed_cookie_length ) 
{
  volatile unsigned int parameters [2+2];
  parameters[0] = (unsigned int)seed_cookie;
  parameters[1] = (unsigned int)seed_cookie_length;
  SVC_Call(SYSCALL_os_perso_seed_cookie_ID_IN, parameters);
  return (unsigned int)(((volatile unsigned int*)parameters)[1]);
}

unsigned int os_endorsement_get_code_hash ( unsigned char * buffer ) 
{
  volatile unsigned int parameters [2+1];
#ifdef __clang_analyzer__
  parameters[1] = 0;
#endif
  parameters[0] = (unsigned int)buffer;
  SVC_Call(SYSCALL_os_endorsement_get_code_hash_ID_IN, parameters);
  return (unsigned int)(((volatile unsigned int*)parameters)[1]);
}

unsigned int os_endorsement_get_public_key ( unsigned char index, unsigned char * buffer ) 
{
  volatile unsigned int parameters [2+2];
  parameters[0] = (unsigned int)index;
  parameters[1] = (unsigned int)buffer;
  SVC_Call(SYSCALL_os_endorsement_get_public_key_ID_IN, parameters);
  return (unsigned int)(((volatile unsigned int*)parameters)[1]);
}

unsigned int os_endorsement_get_public_key_certificate ( unsigned char index, unsigned char * buffer ) 
{
  volatile unsigned int parameters [2+2];
  parameters[0] = (unsigned int)index;
  parameters[1] = (unsigned int)buffer;
  SVC_Call(SYSCALL_os_endorsement_get_public_key_certificate_ID_IN, parameters);
  return (unsigned int)(((volatile unsigned int*)parameters)[1]);
}

unsigned int os_endorsement_key1_get_app_secret ( unsigned char * buffer ) 
{
  volatile unsigned int parameters [2+1];
#ifdef __clang_analyzer__
  parameters[1] = 0;
#endif
  parameters[0] = (unsigned int)buffer;
  SVC_Call(SYSCALL_os_endorsement_key1_get_app_secret_ID_IN, parameters);
  return (unsigned int)(((volatile unsigned int*)parameters)[1]);
}

unsigned int os_endorsement_key1_sign_data ( unsigned char * src, unsigned int srcLength, unsigned char * signature ) 
{
  volatile unsigned int parameters [2+3];
  parameters[0] = (unsigned int)src;
  parameters[1] = (unsigned int)srcLength;
  parameters[2] = (unsigned int)signature;
  SVC_Call(SYSCALL_os_endorsement_key1_sign_data_ID_IN, parameters);
  return (unsigned int)(((volatile unsigned int*)parameters)[1]);
}

unsigned int os_endorsement_key2_derive_sign_data ( unsigned char * src, unsigned int srcLength, unsigned char * signature ) 
{
  volatile unsigned int parameters [2+3];
  parameters[0] = (unsigned int)src;
  parameters[1] = (unsigned int)srcLength;
  parameters[2] = (unsigned int)signature;
  SVC_Call(SYSCALL_os_endorsement_key2_derive_sign_data_ID_IN, parameters);
  return (unsigned int)(((volatile unsigned int*)parameters)[1]);
}

bolos_bool_t os_global_pin_is_validated ( void ) 
{
  volatile unsigned int parameters [2];
#ifdef __clang_analyzer__
  parameters[1] = 0;
#endif
  SVC_Call(SYSCALL_os_global_pin_is_validated_ID_IN, parameters);
  return (bolos_bool_t)(((volatile unsigned int*)parameters)[1]);
}

bolos_bool_t os_global_pin_check ( unsigned char * pin_buffer, unsigned char pin_length ) 
{
  volatile unsigned int parameters [2+2];
  parameters[0] = (unsigned int)pin_buffer;
  parameters[1] = (unsigned int)pin_length;
  SVC_Call(SYSCALL_os_global_pin_check_ID_IN, parameters);
  return (bolos_bool_t)(((volatile unsigned int*)parameters)[1]);
}

void os_global_pin_invalidate ( void ) 
{
  volatile unsigned int parameters [2];
  SVC_Call(SYSCALL_os_global_pin_invalidate_ID_IN, parameters);
}

unsigned int os_global_pin_retries ( void ) 
{
  volatile unsigned int parameters [2];
#ifdef __clang_analyzer__
  parameters[1] = 0;
#endif
  SVC_Call(SYSCALL_os_global_pin_retries_ID_IN, parameters);
  return (unsigned int)(((volatile unsigned int*)parameters)[1]);
}

unsigned int os_registry_count ( void ) 
{
  volatile unsigned int parameters [2];
#ifdef __clang_analyzer__
  parameters[1] = 0;
#endif
  SVC_Call(SYSCALL_os_registry_count_ID_IN, parameters);
  return (unsigned int)(((volatile unsigned int*)parameters)[1]);
}

void os_registry_get ( unsigned int index, application_t * out_application_entry ) 
{
  volatile unsigned int parameters [2+2];
  parameters[0] = (unsigned int)index;
  parameters[1] = (unsigned int)out_application_entry;
  SVC_Call(SYSCALL_os_registry_get_ID_IN, parameters);
}

unsigned int os_ux ( bolos_ux_params_t * params ) 
{
  volatile unsigned int parameters [2+1];
#ifdef __clang_analyzer__
  parameters[1] = 0;
#endif
  parameters[0] = (unsigned int)params;
  SVC_Call(SYSCALL_os_ux_ID_IN, parameters);
  return (unsigned int)(((volatile unsigned int*)parameters)[1]);
}

void os_ux_result ( bolos_ux_params_t * params ) 
{
  volatile unsigned int parameters [2+1];
  parameters[0] = (unsigned int)params;
  SVC_Call(SYSCALL_os_ux_result_ID_IN, parameters);
}

void os_ux_read_parameters ( bolos_ux_params_t * params ) 
{
  volatile unsigned int parameters [2+1];
  parameters[0] = (unsigned int)params;
  SVC_Call(SYSCALL_os_ux_read_parameters_ID_IN, parameters);
}

void os_lib_call ( unsigned int * call_parameters ) 
{
  volatile unsigned int parameters [2+1];
  parameters[0] = (unsigned int)call_parameters;
  SVC_Call(SYSCALL_os_lib_call_ID_IN, parameters);
}

void os_lib_end ( void ) 
{
  volatile unsigned int parameters [2];
  SVC_Call(SYSCALL_os_lib_end_ID_IN, parameters);
}

void os_lib_throw ( unsigned int exception ) 
{
  volatile unsigned int parameters [2+1];
  parameters[0] = (unsigned int)exception;
  SVC_Call(SYSCALL_os_lib_throw_ID_IN, parameters);
}

unsigned int os_flags ( void ) 
{
  volatile unsigned int parameters [2];
#ifdef __clang_analyzer__
  parameters[1] = 0;
#endif
  SVC_Call(SYSCALL_os_flags_ID_IN, parameters);
  return (unsigned int)(((volatile unsigned int*)parameters)[1]);
}

unsigned int os_version ( unsigned char * version, unsigned int maxlength ) 
{
  volatile unsigned int parameters [2+2];
  parameters[0] = (unsigned int)version;
  parameters[1] = (unsigned int)maxlength;
  SVC_Call(SYSCALL_os_version_ID_IN, parameters);
  return (unsigned int)(((volatile unsigned int*)parameters)[1]);
}

unsigned int os_serial ( unsigned char * serial, unsigned int maxlength ) 
{
  volatile unsigned int parameters [2+2];
  parameters[0] = (unsigned int)serial;
  parameters[1] = (unsigned int)maxlength;
  SVC_Call(SYSCALL_os_serial_ID_IN, parameters);
  return (unsigned int)(((volatile unsigned int*)parameters)[1]);
}

unsigned int os_seph_features ( void ) 
{
  volatile unsigned int parameters [2];
#ifdef __clang_analyzer__
  parameters[1] = 0;
#endif
  SVC_Call(SYSCALL_os_seph_features_ID_IN, parameters);
  return (unsigned int)(((volatile unsigned int*)parameters)[1]);
}

unsigned int os_seph_version ( unsigned char * version, unsigned int maxlength ) 
{
  volatile unsigned int parameters [2+2];
  parameters[0] = (unsigned int)version;
  parameters[1] = (unsigned int)maxlength;
  SVC_Call(SYSCALL_os_seph_version_ID_IN, parameters);
  return (unsigned int)(((volatile unsigned int*)parameters)[1]);
}

unsigned int os_bootloader_version ( unsigned char * version, unsigned int maxlength ) 
{
  volatile unsigned int parameters [2+2];
  parameters[0] = (unsigned int)version;
  parameters[1] = (unsigned int)maxlength;
  SVC_Call(SYSCALL_os_bootloader_version_ID_IN, parameters);
  return (unsigned int)(((volatile unsigned int*)parameters)[1]);
}

unsigned int os_setting_get ( unsigned int setting_id, unsigned char * value, unsigned int maxlen ) 
{
  volatile unsigned int parameters [2+3];
  parameters[0] = (unsigned int)setting_id;
  parameters[1] = (unsigned int)value;
  parameters[2] = (unsigned int)maxlen;
  SVC_Call(SYSCALL_os_setting_get_ID_IN, parameters);
  return (unsigned int)(((volatile unsigned int*)parameters)[1]);
}

void os_setting_set ( unsigned int setting_id, unsigned char * value, unsigned int length ) 
{
  volatile unsigned int parameters [2+3];
  parameters[0] = (unsigned int)setting_id;
  parameters[1] = (unsigned int)value;
  parameters[2] = (unsigned int)length;
  SVC_Call(SYSCALL_os_setting_set_ID_IN, parameters);
}

void os_get_memory_info ( meminfo_t * meminfo ) 
{
  volatile unsigned int parameters [2+1];
  parameters[0] = (unsigned int)meminfo;
  SVC_Call(SYSCALL_os_get_memory_info_ID_IN, parameters);
}

unsigned int os_registry_get_tag ( unsigned int appidx, unsigned int * tlvoffset, unsigned int tag, unsigned int value_offset, void * buffer, unsigned int maxlength ) 
{
  volatile unsigned int parameters [2+6];
  parameters[0] = (unsigned int)appidx;
  parameters[1] = (unsigned int)tlvoffset;
  parameters[2] = (unsigned int)tag;
  parameters[3] = (unsigned int)value_offset;
  parameters[4] = (unsigned int)buffer;
  parameters[5] = (unsigned int)maxlength;
  SVC_Call(SYSCALL_os_registry_get_tag_ID_IN, parameters);
  return (unsigned int)(((volatile unsigned int*)parameters)[1]);
}

unsigned int os_registry_get_current_app_tag ( unsigned int tag, unsigned char * buffer, unsigned int maxlen ) 
{
  volatile unsigned int parameters [2+3];
  parameters[0] = (unsigned int)tag;
  parameters[1] = (unsigned int)buffer;
  parameters[2] = (unsigned int)maxlen;
  SVC_Call(SYSCALL_os_registry_get_current_app_tag_ID_IN, parameters);
  return (unsigned int)(((volatile unsigned int*)parameters)[1]);
}

unsigned int os_customca_verify ( unsigned char * hash, unsigned char * sign, unsigned int sign_length ) 
{
  volatile unsigned int parameters [2+3];
  parameters[0] = (unsigned int)hash;
  parameters[1] = (unsigned int)sign;
  parameters[2] = (unsigned int)sign_length;
  SVC_Call(SYSCALL_os_customca_verify_ID_IN, parameters);
  return (unsigned int)(((volatile unsigned int*)parameters)[1]);
}

void os_sched_exec ( unsigned int application_index ) 
{
  volatile unsigned int parameters [2+1];
  parameters[0] = (unsigned int)application_index;
  SVC_Call(SYSCALL_os_sched_exec_ID_IN, parameters);
}

void os_sched_exit ( bolos_task_status_t exit_code ) 
{
  volatile unsigned int parameters [2+1];
  parameters[0] = (unsigned int)exit_code;
  SVC_Call(SYSCALL_os_sched_exit_ID_IN, parameters);
}

bolos_bool_t os_sched_is_running ( unsigned int task_idx ) 
{
  volatile unsigned int parameters [2+1];
#ifdef __clang_analyzer__
  parameters[1] = 0;
#endif
  parameters[0] = (unsigned int)task_idx;
  SVC_Call(SYSCALL_os_sched_is_running_ID_IN, parameters);
  return (bolos_bool_t)(((volatile unsigned int*)parameters)[1]);
}

unsigned int os_sched_create ( unsigned int permissions, void * main, void * nvram, unsigned int nvram_length, void * ram0, unsigned int ram0_length, void * ram1, unsigned int ram1_length, void * stack, unsigned int stack_length ) 
{
  volatile unsigned int parameters [2+10];
  parameters[0] = (unsigned int)permissions;
  parameters[1] = (unsigned int)main;
  parameters[2] = (unsigned int)nvram;
  parameters[3] = (unsigned int)nvram_length;
  parameters[4] = (unsigned int)ram0;
  parameters[5] = (unsigned int)ram0_length;
  parameters[6] = (unsigned int)ram1;
  parameters[7] = (unsigned int)ram1_length;
  parameters[8] = (unsigned int)stack;
  parameters[9] = (unsigned int)stack_length;
  SVC_Call(SYSCALL_os_sched_create_ID_IN, parameters);
  return (unsigned int)(((volatile unsigned int*)parameters)[1]);
}

void os_sched_kill ( unsigned int taskidx ) 
{
  volatile unsigned int parameters [2+1];
  parameters[0] = (unsigned int)taskidx;
  SVC_Call(SYSCALL_os_sched_kill_ID_IN, parameters);
}

void io_seph_send ( const unsigned char * buffer, unsigned short length ) 
{
  volatile unsigned int parameters [2+2];
  parameters[0] = (unsigned int)buffer;
  parameters[1] = (unsigned int)length;
  SVC_Call(SYSCALL_io_seph_send_ID_IN, parameters);
}

unsigned int io_seph_is_status_sent ( void ) 
{
  volatile unsigned int parameters [2];
#ifdef __clang_analyzer__
  parameters[1] = 0;
#endif
  SVC_Call(SYSCALL_io_seph_is_status_sent_ID_IN, parameters);
  return (unsigned int)(((volatile unsigned int*)parameters)[1]);
}

unsigned short io_seph_recv ( unsigned char * buffer, unsigned short maxlength, unsigned int flags ) 
{
  volatile unsigned int parameters [2+3];
  parameters[0] = (unsigned int)buffer;
  parameters[1] = (unsigned int)maxlength;
  parameters[2] = (unsigned int)flags;
  SVC_Call(SYSCALL_io_seph_recv_ID_IN, parameters);
  return (unsigned short)(((volatile unsigned int*)parameters)[1]);
}

void nvm_write_page ( unsigned char * page_adr ) 
{
  volatile unsigned int parameters [2+1];
  parameters[0] = (unsigned int)page_adr;
  SVC_Call(SYSCALL_nvm_write_page_ID_IN, parameters);
}

try_context_t * try_context_get ( void ) 
{
  volatile unsigned int parameters [2];
#ifdef __clang_analyzer__
  parameters[1] = 0;
#endif
  SVC_Call(SYSCALL_try_context_get_ID_IN, parameters);
  return (try_context_t *)(((volatile unsigned int*)parameters)[1]);
}

try_context_t * try_context_set ( try_context_t * context ) 
{
  volatile unsigned int parameters [2+1];
#ifdef __clang_analyzer__
  parameters[1] = 0;
#endif
  parameters[0] = (unsigned int)context;
  SVC_Call(SYSCALL_try_context_set_ID_IN, parameters);
  return (try_context_t *)(((volatile unsigned int*)parameters)[1]);
}

unsigned int cx_rng_u32 ( void ) 
{
  volatile unsigned int parameters [2];
#ifdef __clang_analyzer__
  parameters[1] = 0;
#endif
  SVC_Call(SYSCALL_cx_rng_u32_ID_IN, parameters);
  return (unsigned int)(((volatile unsigned int*)parameters)[1]);
}

bolos_task_status_t os_sched_last_status ( unsigned int task_idx ) 
{
  volatile unsigned int parameters [2+1];
#ifdef __clang_analyzer__
  parameters[1] = 0;
#endif
  parameters[0] = (unsigned int)task_idx;
  SVC_Call(SYSCALL_os_sched_last_status_ID_IN, parameters);
  return (bolos_task_status_t)(((volatile unsigned int*)parameters)[1]);
}

void os_sched_yield ( bolos_task_status_t status ) 
{
  volatile unsigned int parameters [2+1];
  parameters[0] = (unsigned int)status;
  SVC_Call(SYSCALL_os_sched_yield_ID_IN, parameters);
}

void os_sched_switch ( unsigned int task_idx, bolos_task_status_t status ) 
{
  volatile unsigned int parameters [2+2];
  parameters[0] = (unsigned int)task_idx;
  parameters[1] = (unsigned int)status;
  SVC_Call(SYSCALL_os_sched_switch_ID_IN, parameters);
}

unsigned int os_sched_current_task ( void ) 
{
  volatile unsigned int parameters [2];
#ifdef __clang_analyzer__
  parameters[1] = 0;
#endif
  SVC_Call(SYSCALL_os_sched_current_task_ID_IN, parameters);
  return (unsigned int)(((volatile unsigned int*)parameters)[1]);
}

unsigned int os_mpu_protect_ram ( unsigned int state ) 
{
  volatile unsigned int parameters [2+1];
#ifdef __clang_analyzer__
  parameters[1] = 0;
#endif
  parameters[0] = (unsigned int)state;
  SVC_Call(SYSCALL_os_mpu_protect_ram_ID_IN, parameters);
  return (unsigned int)(((volatile unsigned int*)parameters)[1]);
}

unsigned int os_mpu_protect_flash ( unsigned int state ) 
{
  volatile unsigned int parameters [2+1];
#ifdef __clang_analyzer__
  parameters[1] = 0;
#endif
  parameters[0] = (unsigned int)state;
  SVC_Call(SYSCALL_os_mpu_protect_flash_ID_IN, parameters);
  return (unsigned int)(((volatile unsigned int*)parameters)[1]);
}

