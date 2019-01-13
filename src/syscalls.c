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

  /* MACHINE GENERATED: DO NOT MODIFY */
  #define SYSCALL_STUB
  #include "os.h"
  #include "syscalls.h"

  // avoid a separate asm file, but avoid any intrusion from the compiler
  unsigned int SVC_Call(unsigned int syscall_id, unsigned int * parameters) __attribute__ ((naked));
  //                    r0                       r1
  unsigned int SVC_Call(unsigned int syscall_id, unsigned int * parameters) {
    // delegate svc
    asm volatile("svc #1":::"r0","r1");
    // directly return R0 value
    asm volatile("bx  lr");
  }
  void check_api_level ( unsigned int apiLevel ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+1];
  parameters[0] = (unsigned int)apiLevel;
  retid = SVC_Call(SYSCALL_check_api_level_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_check_api_level_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
}

void reset ( void ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0];
  retid = SVC_Call(SYSCALL_reset_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_reset_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
}

void nvm_write ( void * dst_adr, void * src_adr, unsigned int src_len ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+3];
  parameters[0] = (unsigned int)dst_adr;
  parameters[1] = (unsigned int)src_adr;
  parameters[2] = (unsigned int)src_len;
  retid = SVC_Call(SYSCALL_nvm_write_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_nvm_write_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
}

unsigned char cx_rng_u8 ( void ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0];
  retid = SVC_Call(SYSCALL_cx_rng_u8_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_rng_u8_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned char)ret;
}

unsigned char * cx_rng ( unsigned char * buffer, unsigned int len ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+2];
  parameters[0] = (unsigned int)buffer;
  parameters[1] = (unsigned int)len;
  retid = SVC_Call(SYSCALL_cx_rng_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_rng_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned char *)ret;
}

int cx_rng_rfc6979 ( unsigned char * rnd, unsigned int rnd_len, unsigned int hashID, const unsigned char * h1, unsigned int h1_len, const unsigned char * x, unsigned int x_len, const unsigned char * q, unsigned int q_len, unsigned char * V, unsigned int V_len ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+11];
  parameters[0] = (unsigned int)rnd;
  parameters[1] = (unsigned int)rnd_len;
  parameters[2] = (unsigned int)hashID;
  parameters[3] = (unsigned int)h1;
  parameters[4] = (unsigned int)h1_len;
  parameters[5] = (unsigned int)x;
  parameters[6] = (unsigned int)x_len;
  parameters[7] = (unsigned int)q;
  parameters[8] = (unsigned int)q_len;
  parameters[9] = (unsigned int)V;
  parameters[10] = (unsigned int)V_len;
  retid = SVC_Call(SYSCALL_cx_rng_rfc6979_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_rng_rfc6979_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_hash ( cx_hash_t * hash, int mode, const unsigned char * in, unsigned int len, unsigned char * out, unsigned int out_len ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+6];
  parameters[0] = (unsigned int)hash;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)in;
  parameters[3] = (unsigned int)len;
  parameters[4] = (unsigned int)out;
  parameters[5] = (unsigned int)out_len;
  retid = SVC_Call(SYSCALL_cx_hash_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_hash_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_ripemd160_init ( cx_ripemd160_t * hash ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+1];
  parameters[0] = (unsigned int)hash;
  retid = SVC_Call(SYSCALL_cx_ripemd160_init_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_ripemd160_init_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_sha224_init ( cx_sha256_t * hash ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+1];
  parameters[0] = (unsigned int)hash;
  retid = SVC_Call(SYSCALL_cx_sha224_init_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_sha224_init_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_sha256_init ( cx_sha256_t * hash ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+1];
  parameters[0] = (unsigned int)hash;
  retid = SVC_Call(SYSCALL_cx_sha256_init_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_sha256_init_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_hash_sha256 ( const unsigned char * in, unsigned int len, unsigned char * out, unsigned int out_len ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+4];
  parameters[0] = (unsigned int)in;
  parameters[1] = (unsigned int)len;
  parameters[2] = (unsigned int)out;
  parameters[3] = (unsigned int)out_len;
  retid = SVC_Call(SYSCALL_cx_hash_sha256_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_hash_sha256_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_sha384_init ( cx_sha512_t * hash ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+1];
  parameters[0] = (unsigned int)hash;
  retid = SVC_Call(SYSCALL_cx_sha384_init_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_sha384_init_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_sha512_init ( cx_sha512_t * hash ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+1];
  parameters[0] = (unsigned int)hash;
  retid = SVC_Call(SYSCALL_cx_sha512_init_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_sha512_init_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_hash_sha512 ( const unsigned char * in, unsigned int len, unsigned char * out, unsigned int out_len ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+4];
  parameters[0] = (unsigned int)in;
  parameters[1] = (unsigned int)len;
  parameters[2] = (unsigned int)out;
  parameters[3] = (unsigned int)out_len;
  retid = SVC_Call(SYSCALL_cx_hash_sha512_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_hash_sha512_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_sha3_init ( cx_sha3_t * hash, unsigned int size ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+2];
  parameters[0] = (unsigned int)hash;
  parameters[1] = (unsigned int)size;
  retid = SVC_Call(SYSCALL_cx_sha3_init_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_sha3_init_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_keccak_init ( cx_sha3_t * hash, unsigned int size ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+2];
  parameters[0] = (unsigned int)hash;
  parameters[1] = (unsigned int)size;
  retid = SVC_Call(SYSCALL_cx_keccak_init_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_keccak_init_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_sha3_xof_init ( cx_sha3_t * hash, unsigned int size, unsigned int out_length ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+3];
  parameters[0] = (unsigned int)hash;
  parameters[1] = (unsigned int)size;
  parameters[2] = (unsigned int)out_length;
  retid = SVC_Call(SYSCALL_cx_sha3_xof_init_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_sha3_xof_init_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_groestl_init ( cx_groestl_t * hash, unsigned int size ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+2];
  parameters[0] = (unsigned int)hash;
  parameters[1] = (unsigned int)size;
  retid = SVC_Call(SYSCALL_cx_groestl_init_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_groestl_init_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_blake2b_init ( cx_blake2b_t * hash, unsigned int out_len ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+2];
  parameters[0] = (unsigned int)hash;
  parameters[1] = (unsigned int)out_len;
  retid = SVC_Call(SYSCALL_cx_blake2b_init_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_blake2b_init_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_blake2b_init2 ( cx_blake2b_t * hash, unsigned int out_len, unsigned char * salt, unsigned int salt_len, unsigned char * perso, unsigned int perso_len ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+6];
  parameters[0] = (unsigned int)hash;
  parameters[1] = (unsigned int)out_len;
  parameters[2] = (unsigned int)salt;
  parameters[3] = (unsigned int)salt_len;
  parameters[4] = (unsigned int)perso;
  parameters[5] = (unsigned int)perso_len;
  retid = SVC_Call(SYSCALL_cx_blake2b_init2_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_blake2b_init2_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_hmac_ripemd160_init ( cx_hmac_ripemd160_t * hmac, const unsigned char * key, unsigned int key_len ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+3];
  parameters[0] = (unsigned int)hmac;
  parameters[1] = (unsigned int)key;
  parameters[2] = (unsigned int)key_len;
  retid = SVC_Call(SYSCALL_cx_hmac_ripemd160_init_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_hmac_ripemd160_init_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_hmac_sha256_init ( cx_hmac_sha256_t * hmac, const unsigned char * key, unsigned int key_len ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+3];
  parameters[0] = (unsigned int)hmac;
  parameters[1] = (unsigned int)key;
  parameters[2] = (unsigned int)key_len;
  retid = SVC_Call(SYSCALL_cx_hmac_sha256_init_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_hmac_sha256_init_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_hmac_sha512_init ( cx_hmac_sha512_t * hmac, const unsigned char * key, unsigned int key_len ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+3];
  parameters[0] = (unsigned int)hmac;
  parameters[1] = (unsigned int)key;
  parameters[2] = (unsigned int)key_len;
  retid = SVC_Call(SYSCALL_cx_hmac_sha512_init_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_hmac_sha512_init_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_hmac ( cx_hmac_t * hmac, int mode, const unsigned char * in, unsigned int len, unsigned char * mac, unsigned int mac_len ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+6];
  parameters[0] = (unsigned int)hmac;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)in;
  parameters[3] = (unsigned int)len;
  parameters[4] = (unsigned int)mac;
  parameters[5] = (unsigned int)mac_len;
  retid = SVC_Call(SYSCALL_cx_hmac_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_hmac_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_hmac_sha512 ( const unsigned char * key, unsigned int key_len, const unsigned char * in, unsigned int len, unsigned char * mac, unsigned int mac_len ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+6];
  parameters[0] = (unsigned int)key;
  parameters[1] = (unsigned int)key_len;
  parameters[2] = (unsigned int)in;
  parameters[3] = (unsigned int)len;
  parameters[4] = (unsigned int)mac;
  parameters[5] = (unsigned int)mac_len;
  retid = SVC_Call(SYSCALL_cx_hmac_sha512_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_hmac_sha512_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_hmac_sha256 ( const unsigned char * key, unsigned int key_len, const unsigned char * in, unsigned int len, unsigned char * mac, unsigned int mac_len ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+6];
  parameters[0] = (unsigned int)key;
  parameters[1] = (unsigned int)key_len;
  parameters[2] = (unsigned int)in;
  parameters[3] = (unsigned int)len;
  parameters[4] = (unsigned int)mac;
  parameters[5] = (unsigned int)mac_len;
  retid = SVC_Call(SYSCALL_cx_hmac_sha256_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_hmac_sha256_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

void cx_pbkdf2_sha512 ( const unsigned char * password, unsigned short passwordlen, unsigned char * salt, unsigned short saltlen, unsigned int iterations, unsigned char * out, unsigned int outLength ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+7];
  parameters[0] = (unsigned int)password;
  parameters[1] = (unsigned int)passwordlen;
  parameters[2] = (unsigned int)salt;
  parameters[3] = (unsigned int)saltlen;
  parameters[4] = (unsigned int)iterations;
  parameters[5] = (unsigned int)out;
  parameters[6] = (unsigned int)outLength;
  retid = SVC_Call(SYSCALL_cx_pbkdf2_sha512_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_pbkdf2_sha512_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
}

int cx_des_init_key ( const unsigned char * rawkey, unsigned int key_len, cx_des_key_t * key ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+3];
  parameters[0] = (unsigned int)rawkey;
  parameters[1] = (unsigned int)key_len;
  parameters[2] = (unsigned int)key;
  retid = SVC_Call(SYSCALL_cx_des_init_key_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_des_init_key_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_des_iv ( const cx_des_key_t * key, int mode, const unsigned char * iv, unsigned int iv_len, const unsigned char * in, unsigned int in_len, unsigned char * out, unsigned int out_len ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+8];
  parameters[0] = (unsigned int)key;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)iv;
  parameters[3] = (unsigned int)iv_len;
  parameters[4] = (unsigned int)in;
  parameters[5] = (unsigned int)in_len;
  parameters[6] = (unsigned int)out;
  parameters[7] = (unsigned int)out_len;
  retid = SVC_Call(SYSCALL_cx_des_iv_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_des_iv_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_des ( const cx_des_key_t * key, int mode, const unsigned char * in, unsigned int in_len, unsigned char * out, unsigned int out_len ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+6];
  parameters[0] = (unsigned int)key;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)in;
  parameters[3] = (unsigned int)in_len;
  parameters[4] = (unsigned int)out;
  parameters[5] = (unsigned int)out_len;
  retid = SVC_Call(SYSCALL_cx_des_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_des_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_aes_init_key ( const unsigned char * rawkey, unsigned int key_len, cx_aes_key_t * key ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+3];
  parameters[0] = (unsigned int)rawkey;
  parameters[1] = (unsigned int)key_len;
  parameters[2] = (unsigned int)key;
  retid = SVC_Call(SYSCALL_cx_aes_init_key_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_aes_init_key_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_aes_iv ( const cx_aes_key_t * key, int mode, const unsigned char * iv, unsigned int iv_len, const unsigned char * in, unsigned int in_len, unsigned char * out, unsigned int out_len ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+8];
  parameters[0] = (unsigned int)key;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)iv;
  parameters[3] = (unsigned int)iv_len;
  parameters[4] = (unsigned int)in;
  parameters[5] = (unsigned int)in_len;
  parameters[6] = (unsigned int)out;
  parameters[7] = (unsigned int)out_len;
  retid = SVC_Call(SYSCALL_cx_aes_iv_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_aes_iv_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_aes ( const cx_aes_key_t * key, int mode, const unsigned char * in, unsigned int in_len, unsigned char * out, unsigned int out_len ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+6];
  parameters[0] = (unsigned int)key;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)in;
  parameters[3] = (unsigned int)in_len;
  parameters[4] = (unsigned int)out;
  parameters[5] = (unsigned int)out_len;
  retid = SVC_Call(SYSCALL_cx_aes_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_aes_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_rsa_init_public_key ( const unsigned char * exponent, unsigned int exponent_len, const unsigned char * modulus, unsigned int modulus_len, cx_rsa_public_key_t * key ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+5];
  parameters[0] = (unsigned int)exponent;
  parameters[1] = (unsigned int)exponent_len;
  parameters[2] = (unsigned int)modulus;
  parameters[3] = (unsigned int)modulus_len;
  parameters[4] = (unsigned int)key;
  retid = SVC_Call(SYSCALL_cx_rsa_init_public_key_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_rsa_init_public_key_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_rsa_init_private_key ( const unsigned char * exponent, unsigned int exponent_len, const unsigned char * modulus, unsigned int modulus_len, cx_rsa_private_key_t * key ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+5];
  parameters[0] = (unsigned int)exponent;
  parameters[1] = (unsigned int)exponent_len;
  parameters[2] = (unsigned int)modulus;
  parameters[3] = (unsigned int)modulus_len;
  parameters[4] = (unsigned int)key;
  retid = SVC_Call(SYSCALL_cx_rsa_init_private_key_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_rsa_init_private_key_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_rsa_generate_pair ( unsigned int modulus_len, cx_rsa_public_key_t * public_key, cx_rsa_private_key_t * private_key, const unsigned char * pub_exponent, unsigned int exponent_len, const unsigned char * externalPQ ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+6];
  parameters[0] = (unsigned int)modulus_len;
  parameters[1] = (unsigned int)public_key;
  parameters[2] = (unsigned int)private_key;
  parameters[3] = (unsigned int)pub_exponent;
  parameters[4] = (unsigned int)exponent_len;
  parameters[5] = (unsigned int)externalPQ;
  retid = SVC_Call(SYSCALL_cx_rsa_generate_pair_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_rsa_generate_pair_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_rsa_sign ( const cx_rsa_private_key_t * key, int mode, cx_md_t hashID, const unsigned char * hash, unsigned int hash_len, unsigned char * sig, unsigned int sig_len ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+7];
  parameters[0] = (unsigned int)key;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)hashID;
  parameters[3] = (unsigned int)hash;
  parameters[4] = (unsigned int)hash_len;
  parameters[5] = (unsigned int)sig;
  parameters[6] = (unsigned int)sig_len;
  retid = SVC_Call(SYSCALL_cx_rsa_sign_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_rsa_sign_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_rsa_verify ( const cx_rsa_public_key_t * key, int mode, cx_md_t hashID, const unsigned char * hash, unsigned int hash_len, const unsigned char * sig, unsigned int sig_len ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+7];
  parameters[0] = (unsigned int)key;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)hashID;
  parameters[3] = (unsigned int)hash;
  parameters[4] = (unsigned int)hash_len;
  parameters[5] = (unsigned int)sig;
  parameters[6] = (unsigned int)sig_len;
  retid = SVC_Call(SYSCALL_cx_rsa_verify_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_rsa_verify_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_rsa_encrypt ( const cx_rsa_public_key_t * key, int mode, cx_md_t hashID, const unsigned char * mesg, unsigned int mesg_len, unsigned char * enc, unsigned int enc_len ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+7];
  parameters[0] = (unsigned int)key;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)hashID;
  parameters[3] = (unsigned int)mesg;
  parameters[4] = (unsigned int)mesg_len;
  parameters[5] = (unsigned int)enc;
  parameters[6] = (unsigned int)enc_len;
  retid = SVC_Call(SYSCALL_cx_rsa_encrypt_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_rsa_encrypt_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_rsa_decrypt ( const cx_rsa_private_key_t * key, int mode, cx_md_t hashID, const unsigned char * mesg, unsigned int mesg_len, unsigned char * dec, unsigned int dec_len ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+7];
  parameters[0] = (unsigned int)key;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)hashID;
  parameters[3] = (unsigned int)mesg;
  parameters[4] = (unsigned int)mesg_len;
  parameters[5] = (unsigned int)dec;
  parameters[6] = (unsigned int)dec_len;
  retid = SVC_Call(SYSCALL_cx_rsa_decrypt_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_rsa_decrypt_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_ecfp_is_valid_point ( cx_curve_t curve, const unsigned char * P, unsigned int P_len ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+3];
  parameters[0] = (unsigned int)curve;
  parameters[1] = (unsigned int)P;
  parameters[2] = (unsigned int)P_len;
  retid = SVC_Call(SYSCALL_cx_ecfp_is_valid_point_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_ecfp_is_valid_point_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_ecfp_is_cryptographic_point ( cx_curve_t curve, const unsigned char * P, unsigned int P_len ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+3];
  parameters[0] = (unsigned int)curve;
  parameters[1] = (unsigned int)P;
  parameters[2] = (unsigned int)P_len;
  retid = SVC_Call(SYSCALL_cx_ecfp_is_cryptographic_point_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_ecfp_is_cryptographic_point_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_ecfp_add_point ( cx_curve_t curve, unsigned char * R, const unsigned char * P, const unsigned char * Q, unsigned int X_len ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+5];
  parameters[0] = (unsigned int)curve;
  parameters[1] = (unsigned int)R;
  parameters[2] = (unsigned int)P;
  parameters[3] = (unsigned int)Q;
  parameters[4] = (unsigned int)X_len;
  retid = SVC_Call(SYSCALL_cx_ecfp_add_point_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_ecfp_add_point_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_ecfp_scalar_mult ( cx_curve_t curve, unsigned char * P, unsigned int P_len, const unsigned char * k, unsigned int k_len ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+5];
  parameters[0] = (unsigned int)curve;
  parameters[1] = (unsigned int)P;
  parameters[2] = (unsigned int)P_len;
  parameters[3] = (unsigned int)k;
  parameters[4] = (unsigned int)k_len;
  retid = SVC_Call(SYSCALL_cx_ecfp_scalar_mult_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_ecfp_scalar_mult_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_ecfp_init_public_key ( cx_curve_t curve, const unsigned char * rawkey, unsigned int key_len, cx_ecfp_public_key_t * key ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+4];
  parameters[0] = (unsigned int)curve;
  parameters[1] = (unsigned int)rawkey;
  parameters[2] = (unsigned int)key_len;
  parameters[3] = (unsigned int)key;
  retid = SVC_Call(SYSCALL_cx_ecfp_init_public_key_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_ecfp_init_public_key_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_ecfp_init_private_key ( cx_curve_t curve, const unsigned char * rawkey, unsigned int key_len, cx_ecfp_private_key_t * pvkey ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+4];
  parameters[0] = (unsigned int)curve;
  parameters[1] = (unsigned int)rawkey;
  parameters[2] = (unsigned int)key_len;
  parameters[3] = (unsigned int)pvkey;
  retid = SVC_Call(SYSCALL_cx_ecfp_init_private_key_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_ecfp_init_private_key_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_ecfp_generate_pair ( cx_curve_t curve, cx_ecfp_public_key_t * pubkey, cx_ecfp_private_key_t * privkey, int keepprivate ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+4];
  parameters[0] = (unsigned int)curve;
  parameters[1] = (unsigned int)pubkey;
  parameters[2] = (unsigned int)privkey;
  parameters[3] = (unsigned int)keepprivate;
  retid = SVC_Call(SYSCALL_cx_ecfp_generate_pair_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_ecfp_generate_pair_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_ecfp_generate_pair2 ( cx_curve_t curve, cx_ecfp_public_key_t * pubkey, cx_ecfp_private_key_t * privkey, int keepprivate, cx_md_t hashID ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+5];
  parameters[0] = (unsigned int)curve;
  parameters[1] = (unsigned int)pubkey;
  parameters[2] = (unsigned int)privkey;
  parameters[3] = (unsigned int)keepprivate;
  parameters[4] = (unsigned int)hashID;
  retid = SVC_Call(SYSCALL_cx_ecfp_generate_pair2_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_ecfp_generate_pair2_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_ecschnorr_sign ( const cx_ecfp_private_key_t * pvkey, int mode, cx_md_t hashID, const unsigned char * msg, unsigned int msg_len, unsigned char * sig, unsigned int sig_len, unsigned int * info ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+8];
  parameters[0] = (unsigned int)pvkey;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)hashID;
  parameters[3] = (unsigned int)msg;
  parameters[4] = (unsigned int)msg_len;
  parameters[5] = (unsigned int)sig;
  parameters[6] = (unsigned int)sig_len;
  parameters[7] = (unsigned int)info;
  retid = SVC_Call(SYSCALL_cx_ecschnorr_sign_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_ecschnorr_sign_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_ecschnorr_verify ( const cx_ecfp_public_key_t * pukey, int mode, cx_md_t hashID, const unsigned char * msg, unsigned int msg_len, const unsigned char * sig, unsigned int sig_len ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+7];
  parameters[0] = (unsigned int)pukey;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)hashID;
  parameters[3] = (unsigned int)msg;
  parameters[4] = (unsigned int)msg_len;
  parameters[5] = (unsigned int)sig;
  parameters[6] = (unsigned int)sig_len;
  retid = SVC_Call(SYSCALL_cx_ecschnorr_verify_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_ecschnorr_verify_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

void cx_edward_compress_point ( cx_curve_t curve, unsigned char * P, unsigned int P_len ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+3];
  parameters[0] = (unsigned int)curve;
  parameters[1] = (unsigned int)P;
  parameters[2] = (unsigned int)P_len;
  retid = SVC_Call(SYSCALL_cx_edward_compress_point_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_edward_compress_point_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
}

void cx_edward_decompress_point ( cx_curve_t curve, unsigned char * P, unsigned int P_len ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+3];
  parameters[0] = (unsigned int)curve;
  parameters[1] = (unsigned int)P;
  parameters[2] = (unsigned int)P_len;
  retid = SVC_Call(SYSCALL_cx_edward_decompress_point_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_edward_decompress_point_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
}

void cx_eddsa_get_public_key ( const cx_ecfp_private_key_t * pvkey, cx_md_t hashID, cx_ecfp_public_key_t * pukey, unsigned char * a, unsigned int a_len, unsigned char * h, unsigned int h_len ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+7];
  parameters[0] = (unsigned int)pvkey;
  parameters[1] = (unsigned int)hashID;
  parameters[2] = (unsigned int)pukey;
  parameters[3] = (unsigned int)a;
  parameters[4] = (unsigned int)a_len;
  parameters[5] = (unsigned int)h;
  parameters[6] = (unsigned int)h_len;
  retid = SVC_Call(SYSCALL_cx_eddsa_get_public_key_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_eddsa_get_public_key_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
}

int cx_eddsa_sign ( const cx_ecfp_private_key_t * pvkey, int mode, cx_md_t hashID, const unsigned char * hash, unsigned int hash_len, const unsigned char * ctx, unsigned int ctx_len, unsigned char * sig, unsigned int sig_len, unsigned int * info ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+10];
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
  retid = SVC_Call(SYSCALL_cx_eddsa_sign_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_eddsa_sign_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_eddsa_verify ( const cx_ecfp_public_key_t * pukey, int mode, cx_md_t hashID, const unsigned char * hash, unsigned int hash_len, const unsigned char * ctx, unsigned int ctx_len, const unsigned char * sig, unsigned int sig_len ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+9];
  parameters[0] = (unsigned int)pukey;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)hashID;
  parameters[3] = (unsigned int)hash;
  parameters[4] = (unsigned int)hash_len;
  parameters[5] = (unsigned int)ctx;
  parameters[6] = (unsigned int)ctx_len;
  parameters[7] = (unsigned int)sig;
  parameters[8] = (unsigned int)sig_len;
  retid = SVC_Call(SYSCALL_cx_eddsa_verify_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_eddsa_verify_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_ecdsa_sign ( const cx_ecfp_private_key_t * pvkey, int mode, cx_md_t hashID, const unsigned char * hash, unsigned int hash_len, unsigned char * sig, unsigned int sig_len, unsigned int * info ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+8];
  parameters[0] = (unsigned int)pvkey;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)hashID;
  parameters[3] = (unsigned int)hash;
  parameters[4] = (unsigned int)hash_len;
  parameters[5] = (unsigned int)sig;
  parameters[6] = (unsigned int)sig_len;
  parameters[7] = (unsigned int)info;
  retid = SVC_Call(SYSCALL_cx_ecdsa_sign_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_ecdsa_sign_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_ecdsa_verify ( const cx_ecfp_public_key_t * pukey, int mode, cx_md_t hashID, const unsigned char * hash, unsigned int hash_len, const unsigned char * sig, unsigned int sig_len ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+7];
  parameters[0] = (unsigned int)pukey;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)hashID;
  parameters[3] = (unsigned int)hash;
  parameters[4] = (unsigned int)hash_len;
  parameters[5] = (unsigned int)sig;
  parameters[6] = (unsigned int)sig_len;
  retid = SVC_Call(SYSCALL_cx_ecdsa_verify_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_ecdsa_verify_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_ecdh ( const cx_ecfp_private_key_t * pvkey, int mode, const unsigned char * P, unsigned int P_len, unsigned char * secret, unsigned int secret_len ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+6];
  parameters[0] = (unsigned int)pvkey;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)P;
  parameters[3] = (unsigned int)P_len;
  parameters[4] = (unsigned int)secret;
  parameters[5] = (unsigned int)secret_len;
  retid = SVC_Call(SYSCALL_cx_ecdh_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_ecdh_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

unsigned short cx_crc16 ( const void * buffer, unsigned int len ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+2];
  parameters[0] = (unsigned int)buffer;
  parameters[1] = (unsigned int)len;
  retid = SVC_Call(SYSCALL_cx_crc16_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_crc16_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned short)ret;
}

unsigned short cx_crc16_update ( unsigned short crc, const void * buffer, unsigned int len ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+3];
  parameters[0] = (unsigned int)crc;
  parameters[1] = (unsigned int)buffer;
  parameters[2] = (unsigned int)len;
  retid = SVC_Call(SYSCALL_cx_crc16_update_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_crc16_update_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned short)ret;
}

int cx_math_cmp ( const unsigned char * a, const unsigned char * b, unsigned int len ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+3];
  parameters[0] = (unsigned int)a;
  parameters[1] = (unsigned int)b;
  parameters[2] = (unsigned int)len;
  retid = SVC_Call(SYSCALL_cx_math_cmp_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_math_cmp_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_math_is_zero ( const unsigned char * a, unsigned int len ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+2];
  parameters[0] = (unsigned int)a;
  parameters[1] = (unsigned int)len;
  retid = SVC_Call(SYSCALL_cx_math_is_zero_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_math_is_zero_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_math_add ( unsigned char * r, const unsigned char * a, const unsigned char * b, unsigned int len ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+4];
  parameters[0] = (unsigned int)r;
  parameters[1] = (unsigned int)a;
  parameters[2] = (unsigned int)b;
  parameters[3] = (unsigned int)len;
  retid = SVC_Call(SYSCALL_cx_math_add_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_math_add_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_math_sub ( unsigned char * r, const unsigned char * a, const unsigned char * b, unsigned int len ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+4];
  parameters[0] = (unsigned int)r;
  parameters[1] = (unsigned int)a;
  parameters[2] = (unsigned int)b;
  parameters[3] = (unsigned int)len;
  retid = SVC_Call(SYSCALL_cx_math_sub_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_math_sub_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

void cx_math_mult ( unsigned char * r, const unsigned char * a, const unsigned char * b, unsigned int len ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+4];
  parameters[0] = (unsigned int)r;
  parameters[1] = (unsigned int)a;
  parameters[2] = (unsigned int)b;
  parameters[3] = (unsigned int)len;
  retid = SVC_Call(SYSCALL_cx_math_mult_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_math_mult_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
}

void cx_math_addm ( unsigned char * r, const unsigned char * a, const unsigned char * b, const unsigned char * m, unsigned int len ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+5];
  parameters[0] = (unsigned int)r;
  parameters[1] = (unsigned int)a;
  parameters[2] = (unsigned int)b;
  parameters[3] = (unsigned int)m;
  parameters[4] = (unsigned int)len;
  retid = SVC_Call(SYSCALL_cx_math_addm_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_math_addm_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
}

void cx_math_subm ( unsigned char * r, const unsigned char * a, const unsigned char * b, const unsigned char * m, unsigned int len ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+5];
  parameters[0] = (unsigned int)r;
  parameters[1] = (unsigned int)a;
  parameters[2] = (unsigned int)b;
  parameters[3] = (unsigned int)m;
  parameters[4] = (unsigned int)len;
  retid = SVC_Call(SYSCALL_cx_math_subm_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_math_subm_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
}

void cx_math_multm ( unsigned char * r, const unsigned char * a, const unsigned char * b, const unsigned char * m, unsigned int len ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+5];
  parameters[0] = (unsigned int)r;
  parameters[1] = (unsigned int)a;
  parameters[2] = (unsigned int)b;
  parameters[3] = (unsigned int)m;
  parameters[4] = (unsigned int)len;
  retid = SVC_Call(SYSCALL_cx_math_multm_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_math_multm_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
}

void cx_math_powm ( unsigned char * r, const unsigned char * a, const unsigned char * e, unsigned int len_e, const unsigned char * m, unsigned int len ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+6];
  parameters[0] = (unsigned int)r;
  parameters[1] = (unsigned int)a;
  parameters[2] = (unsigned int)e;
  parameters[3] = (unsigned int)len_e;
  parameters[4] = (unsigned int)m;
  parameters[5] = (unsigned int)len;
  retid = SVC_Call(SYSCALL_cx_math_powm_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_math_powm_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
}

void cx_math_modm ( unsigned char * v, unsigned int len_v, const unsigned char * m, unsigned int len_m ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+4];
  parameters[0] = (unsigned int)v;
  parameters[1] = (unsigned int)len_v;
  parameters[2] = (unsigned int)m;
  parameters[3] = (unsigned int)len_m;
  retid = SVC_Call(SYSCALL_cx_math_modm_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_math_modm_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
}

void cx_math_invprimem ( unsigned char * r, const unsigned char * a, const unsigned char * m, unsigned int len ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+4];
  parameters[0] = (unsigned int)r;
  parameters[1] = (unsigned int)a;
  parameters[2] = (unsigned int)m;
  parameters[3] = (unsigned int)len;
  retid = SVC_Call(SYSCALL_cx_math_invprimem_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_math_invprimem_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
}

void cx_math_invintm ( unsigned char * r, unsigned long int a, const unsigned char * m, unsigned int len ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+4];
  parameters[0] = (unsigned int)r;
  parameters[1] = (unsigned int)a;
  parameters[2] = (unsigned int)m;
  parameters[3] = (unsigned int)len;
  retid = SVC_Call(SYSCALL_cx_math_invintm_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_math_invintm_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
}

int cx_math_is_prime ( const unsigned char * p, unsigned int len ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+2];
  parameters[0] = (unsigned int)p;
  parameters[1] = (unsigned int)len;
  retid = SVC_Call(SYSCALL_cx_math_is_prime_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_math_is_prime_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

void cx_math_next_prime ( unsigned char * n, unsigned int len ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+2];
  parameters[0] = (unsigned int)n;
  parameters[1] = (unsigned int)len;
  retid = SVC_Call(SYSCALL_cx_math_next_prime_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_cx_math_next_prime_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
}

void os_perso_erase_all ( void ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0];
  retid = SVC_Call(SYSCALL_os_perso_erase_all_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_os_perso_erase_all_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
}

void os_perso_set_pin ( unsigned int identity, unsigned char * pin, unsigned int length ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+3];
  parameters[0] = (unsigned int)identity;
  parameters[1] = (unsigned int)pin;
  parameters[2] = (unsigned int)length;
  retid = SVC_Call(SYSCALL_os_perso_set_pin_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_os_perso_set_pin_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
}

void os_perso_set_current_identity_pin ( unsigned char * pin, unsigned int length ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+2];
  parameters[0] = (unsigned int)pin;
  parameters[1] = (unsigned int)length;
  retid = SVC_Call(SYSCALL_os_perso_set_current_identity_pin_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_os_perso_set_current_identity_pin_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
}

void os_perso_set_seed ( unsigned int identity, unsigned int algorithm, unsigned char * seed, unsigned int length ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+4];
  parameters[0] = (unsigned int)identity;
  parameters[1] = (unsigned int)algorithm;
  parameters[2] = (unsigned int)seed;
  parameters[3] = (unsigned int)length;
  retid = SVC_Call(SYSCALL_os_perso_set_seed_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_os_perso_set_seed_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
}

void os_perso_derive_and_set_seed ( unsigned char identity, const char * prefix, unsigned int prefix_length, const char * passphrase, unsigned int passphrase_length, const char * words, unsigned int words_length ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+7];
  parameters[0] = (unsigned int)identity;
  parameters[1] = (unsigned int)prefix;
  parameters[2] = (unsigned int)prefix_length;
  parameters[3] = (unsigned int)passphrase;
  parameters[4] = (unsigned int)passphrase_length;
  parameters[5] = (unsigned int)words;
  parameters[6] = (unsigned int)words_length;
  retid = SVC_Call(SYSCALL_os_perso_derive_and_set_seed_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_os_perso_derive_and_set_seed_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
}

void os_perso_set_words ( const unsigned char * words, unsigned int length ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+2];
  parameters[0] = (unsigned int)words;
  parameters[1] = (unsigned int)length;
  retid = SVC_Call(SYSCALL_os_perso_set_words_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_os_perso_set_words_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
}

void os_perso_finalize ( void ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0];
  retid = SVC_Call(SYSCALL_os_perso_finalize_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_os_perso_finalize_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
}

unsigned int os_perso_isonboarded ( void ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0];
  retid = SVC_Call(SYSCALL_os_perso_isonboarded_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_os_perso_isonboarded_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned int)ret;
}

void os_perso_derive_node_bip32 ( cx_curve_t curve, const unsigned int * path, unsigned int pathLength, unsigned char * privateKey, unsigned char * chain ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+5];
  parameters[0] = (unsigned int)curve;
  parameters[1] = (unsigned int)path;
  parameters[2] = (unsigned int)pathLength;
  parameters[3] = (unsigned int)privateKey;
  parameters[4] = (unsigned int)chain;
  retid = SVC_Call(SYSCALL_os_perso_derive_node_bip32_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_os_perso_derive_node_bip32_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
}

void os_perso_derive_node_bip32_seed_key ( unsigned int mode, cx_curve_t curve, const unsigned int * path, unsigned int pathLength, unsigned char * privateKey, unsigned char * chain, unsigned char * seed_key, unsigned int seed_key_length ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+8];
  parameters[0] = (unsigned int)mode;
  parameters[1] = (unsigned int)curve;
  parameters[2] = (unsigned int)path;
  parameters[3] = (unsigned int)pathLength;
  parameters[4] = (unsigned int)privateKey;
  parameters[5] = (unsigned int)chain;
  parameters[6] = (unsigned int)seed_key;
  parameters[7] = (unsigned int)seed_key_length;
  retid = SVC_Call(SYSCALL_os_perso_derive_node_bip32_seed_key_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_os_perso_derive_node_bip32_seed_key_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
}

unsigned int os_endorsement_get_code_hash ( unsigned char * buffer ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+1];
  parameters[0] = (unsigned int)buffer;
  retid = SVC_Call(SYSCALL_os_endorsement_get_code_hash_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_os_endorsement_get_code_hash_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned int)ret;
}

unsigned int os_endorsement_get_public_key ( unsigned char index, unsigned char * buffer ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+2];
  parameters[0] = (unsigned int)index;
  parameters[1] = (unsigned int)buffer;
  retid = SVC_Call(SYSCALL_os_endorsement_get_public_key_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_os_endorsement_get_public_key_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned int)ret;
}

unsigned int os_endorsement_get_public_key_certificate ( unsigned char index, unsigned char * buffer ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+2];
  parameters[0] = (unsigned int)index;
  parameters[1] = (unsigned int)buffer;
  retid = SVC_Call(SYSCALL_os_endorsement_get_public_key_certificate_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_os_endorsement_get_public_key_certificate_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned int)ret;
}

unsigned int os_endorsement_key1_get_app_secret ( unsigned char * buffer ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+1];
  parameters[0] = (unsigned int)buffer;
  retid = SVC_Call(SYSCALL_os_endorsement_key1_get_app_secret_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_os_endorsement_key1_get_app_secret_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned int)ret;
}

unsigned int os_endorsement_key1_sign_data ( unsigned char * src, unsigned int srcLength, unsigned char * signature ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+3];
  parameters[0] = (unsigned int)src;
  parameters[1] = (unsigned int)srcLength;
  parameters[2] = (unsigned int)signature;
  retid = SVC_Call(SYSCALL_os_endorsement_key1_sign_data_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_os_endorsement_key1_sign_data_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned int)ret;
}

unsigned int os_endorsement_key2_derive_sign_data ( unsigned char * src, unsigned int srcLength, unsigned char * signature ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+3];
  parameters[0] = (unsigned int)src;
  parameters[1] = (unsigned int)srcLength;
  parameters[2] = (unsigned int)signature;
  retid = SVC_Call(SYSCALL_os_endorsement_key2_derive_sign_data_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_os_endorsement_key2_derive_sign_data_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned int)ret;
}

unsigned int os_global_pin_is_validated ( void ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0];
  retid = SVC_Call(SYSCALL_os_global_pin_is_validated_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_os_global_pin_is_validated_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned int)ret;
}

unsigned int os_global_pin_check ( unsigned char * pin_buffer, unsigned char pin_length ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+2];
  parameters[0] = (unsigned int)pin_buffer;
  parameters[1] = (unsigned int)pin_length;
  retid = SVC_Call(SYSCALL_os_global_pin_check_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_os_global_pin_check_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned int)ret;
}

void os_global_pin_invalidate ( void ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0];
  retid = SVC_Call(SYSCALL_os_global_pin_invalidate_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_os_global_pin_invalidate_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
}

unsigned int os_global_pin_retries ( void ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0];
  retid = SVC_Call(SYSCALL_os_global_pin_retries_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_os_global_pin_retries_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned int)ret;
}

unsigned int os_registry_count ( void ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0];
  retid = SVC_Call(SYSCALL_os_registry_count_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_os_registry_count_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned int)ret;
}

void os_registry_get ( unsigned int index, application_t * out_application_entry ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+2];
  parameters[0] = (unsigned int)index;
  parameters[1] = (unsigned int)out_application_entry;
  retid = SVC_Call(SYSCALL_os_registry_get_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_os_registry_get_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
}

unsigned int os_sched_exec ( unsigned int application_index ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+1];
  parameters[0] = (unsigned int)application_index;
  retid = SVC_Call(SYSCALL_os_sched_exec_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_os_sched_exec_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned int)ret;
}

void os_sched_exit ( unsigned int exit_code ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+1];
  parameters[0] = (unsigned int)exit_code;
  retid = SVC_Call(SYSCALL_os_sched_exit_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_os_sched_exit_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
}

void os_ux_register ( bolos_ux_params_t * parameter_ram_pointer ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+1];
  parameters[0] = (unsigned int)parameter_ram_pointer;
  retid = SVC_Call(SYSCALL_os_ux_register_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_os_ux_register_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
}

unsigned int os_ux ( bolos_ux_params_t * params ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+1];
  parameters[0] = (unsigned int)params;
  retid = SVC_Call(SYSCALL_os_ux_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_os_ux_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned int)ret;
}

unsigned int os_lib_call ( unsigned int * call_parameters ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+1];
  parameters[0] = (unsigned int)call_parameters;
  retid = SVC_Call(SYSCALL_os_lib_call_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_os_lib_call_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned int)ret;
}

void os_lib_end ( unsigned int returnvalue ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+1];
  parameters[0] = (unsigned int)returnvalue;
  retid = SVC_Call(SYSCALL_os_lib_end_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_os_lib_end_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
}

void os_lib_throw ( unsigned int exception ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+1];
  parameters[0] = (unsigned int)exception;
  retid = SVC_Call(SYSCALL_os_lib_throw_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_os_lib_throw_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
}

unsigned int os_flags ( void ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0];
  retid = SVC_Call(SYSCALL_os_flags_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_os_flags_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned int)ret;
}

unsigned int os_version ( unsigned char * version, unsigned int maxlength ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+2];
  parameters[0] = (unsigned int)version;
  parameters[1] = (unsigned int)maxlength;
  retid = SVC_Call(SYSCALL_os_version_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_os_version_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned int)ret;
}

unsigned int os_seph_features ( void ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0];
  retid = SVC_Call(SYSCALL_os_seph_features_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_os_seph_features_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned int)ret;
}

unsigned int os_seph_version ( unsigned char * version, unsigned int maxlength ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+2];
  parameters[0] = (unsigned int)version;
  parameters[1] = (unsigned int)maxlength;
  retid = SVC_Call(SYSCALL_os_seph_version_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_os_seph_version_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned int)ret;
}

unsigned int os_setting_get ( unsigned int setting_id, unsigned char * value, unsigned int maxlen ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+3];
  parameters[0] = (unsigned int)setting_id;
  parameters[1] = (unsigned int)value;
  parameters[2] = (unsigned int)maxlen;
  retid = SVC_Call(SYSCALL_os_setting_get_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_os_setting_get_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned int)ret;
}

void os_setting_set ( unsigned int setting_id, unsigned char * value, unsigned int length ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+3];
  parameters[0] = (unsigned int)setting_id;
  parameters[1] = (unsigned int)value;
  parameters[2] = (unsigned int)length;
  retid = SVC_Call(SYSCALL_os_setting_set_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_os_setting_set_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
}

void os_get_memory_info ( meminfo_t * meminfo ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+1];
  parameters[0] = (unsigned int)meminfo;
  retid = SVC_Call(SYSCALL_os_get_memory_info_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_os_get_memory_info_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
}

unsigned int os_registry_get_tag ( unsigned int appidx, unsigned int * tlvoffset, unsigned int tag, unsigned int value_offset, void * buffer, unsigned int maxlength ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+6];
  parameters[0] = (unsigned int)appidx;
  parameters[1] = (unsigned int)tlvoffset;
  parameters[2] = (unsigned int)tag;
  parameters[3] = (unsigned int)value_offset;
  parameters[4] = (unsigned int)buffer;
  parameters[5] = (unsigned int)maxlength;
  retid = SVC_Call(SYSCALL_os_registry_get_tag_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_os_registry_get_tag_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned int)ret;
}

unsigned int os_registry_get_current_app_tag ( unsigned int tag, unsigned char * buffer, unsigned int maxlen ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+3];
  parameters[0] = (unsigned int)tag;
  parameters[1] = (unsigned int)buffer;
  parameters[2] = (unsigned int)maxlen;
  retid = SVC_Call(SYSCALL_os_registry_get_current_app_tag_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_os_registry_get_current_app_tag_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned int)ret;
}

unsigned int os_customca_verify ( unsigned char * hash, unsigned char * sign, unsigned int sign_length ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+3];
  parameters[0] = (unsigned int)hash;
  parameters[1] = (unsigned int)sign;
  parameters[2] = (unsigned int)sign_length;
  retid = SVC_Call(SYSCALL_os_customca_verify_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_os_customca_verify_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned int)ret;
}

void io_seproxyhal_spi_send ( const unsigned char * buffer, unsigned short length ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+2];
  parameters[0] = (unsigned int)buffer;
  parameters[1] = (unsigned int)length;
  retid = SVC_Call(SYSCALL_io_seproxyhal_spi_send_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_io_seproxyhal_spi_send_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
}

unsigned int io_seproxyhal_spi_is_status_sent ( void ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0];
  retid = SVC_Call(SYSCALL_io_seproxyhal_spi_is_status_sent_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_io_seproxyhal_spi_is_status_sent_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned int)ret;
}

unsigned short io_seproxyhal_spi_recv ( unsigned char * buffer, unsigned short maxlength, unsigned int flags ) 
{
  unsigned int ret;
  unsigned int retid;
  unsigned int parameters [0+3];
  parameters[0] = (unsigned int)buffer;
  parameters[1] = (unsigned int)maxlength;
  parameters[2] = (unsigned int)flags;
  retid = SVC_Call(SYSCALL_io_seproxyhal_spi_recv_ID_IN, parameters);
  asm volatile("str r1, %0":"=m"(ret)::"r1");
  if (retid != SYSCALL_io_seproxyhal_spi_recv_ID_OUT) {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned short)ret;
}

