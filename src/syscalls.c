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
#include "os.h"
#include "syscalls.h"

void check_api_level ( unsigned int apiLevel ) 
{
  unsigned int ret;
  unsigned int parameters [2+1];
  parameters[0] = (unsigned int)SYSCALL_check_api_level_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)apiLevel;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_check_api_level_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
}

void reset ( void ) 
{
  unsigned int ret;
  unsigned int parameters [2];
  parameters[0] = (unsigned int)SYSCALL_reset_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_reset_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
}

void nvm_write ( void * dst_adr, void * src_adr, unsigned int src_len ) 
{
  unsigned int ret;
  unsigned int parameters [2+3];
  parameters[0] = (unsigned int)SYSCALL_nvm_write_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)dst_adr;
  parameters[3] = (unsigned int)src_adr;
  parameters[4] = (unsigned int)src_len;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_nvm_write_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
}

unsigned char cx_rng_u8 ( void ) 
{
  unsigned int ret;
  unsigned int parameters [2];
  parameters[0] = (unsigned int)SYSCALL_cx_rng_u8_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_rng_u8_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned char)ret;
}

unsigned char * cx_rng ( unsigned char * buffer, unsigned int len ) 
{
  unsigned int ret;
  unsigned int parameters [2+2];
  parameters[0] = (unsigned int)SYSCALL_cx_rng_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)buffer;
  parameters[3] = (unsigned int)len;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_rng_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned char *)ret;
}

int cx_ripemd160_init ( cx_ripemd160_t * hash ) 
{
  unsigned int ret;
  unsigned int parameters [2+1];
  parameters[0] = (unsigned int)SYSCALL_cx_ripemd160_init_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)hash;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_ripemd160_init_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_sha256_init ( cx_sha256_t * hash ) 
{
  unsigned int ret;
  unsigned int parameters [2+1];
  parameters[0] = (unsigned int)SYSCALL_cx_sha256_init_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)hash;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_sha256_init_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_sha512_init ( cx_sha512_t * hash ) 
{
  unsigned int ret;
  unsigned int parameters [2+1];
  parameters[0] = (unsigned int)SYSCALL_cx_sha512_init_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)hash;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_sha512_init_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_sha3_init ( cx_sha3_t * hash, int size ) 
{
  unsigned int ret;
  unsigned int parameters [2+2];
  parameters[0] = (unsigned int)SYSCALL_cx_sha3_init_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)hash;
  parameters[3] = (unsigned int)size;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_sha3_init_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_keccak_init ( cx_sha3_t * hash, int size ) 
{
  unsigned int ret;
  unsigned int parameters [2+2];
  parameters[0] = (unsigned int)SYSCALL_cx_keccak_init_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)hash;
  parameters[3] = (unsigned int)size;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_keccak_init_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_sha3_xof_init ( cx_sha3_t * hash, unsigned int size, unsigned int out_length ) 
{
  unsigned int ret;
  unsigned int parameters [2+3];
  parameters[0] = (unsigned int)SYSCALL_cx_sha3_xof_init_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)hash;
  parameters[3] = (unsigned int)size;
  parameters[4] = (unsigned int)out_length;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_sha3_xof_init_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_hash ( cx_hash_t * hash, int mode, unsigned char * in, unsigned int len, unsigned char * out ) 
{
  unsigned int ret;
  unsigned int parameters [2+5];
  parameters[0] = (unsigned int)SYSCALL_cx_hash_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)hash;
  parameters[3] = (unsigned int)mode;
  parameters[4] = (unsigned int)in;
  parameters[5] = (unsigned int)len;
  parameters[6] = (unsigned int)out;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_hash_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_hash_sha256 ( unsigned char * in, unsigned int len, unsigned char * out ) 
{
  unsigned int ret;
  unsigned int parameters [2+3];
  parameters[0] = (unsigned int)SYSCALL_cx_hash_sha256_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)in;
  parameters[3] = (unsigned int)len;
  parameters[4] = (unsigned int)out;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_hash_sha256_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_hash_sha512 ( unsigned char * in, unsigned int len, unsigned char * out ) 
{
  unsigned int ret;
  unsigned int parameters [2+3];
  parameters[0] = (unsigned int)SYSCALL_cx_hash_sha512_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)in;
  parameters[3] = (unsigned int)len;
  parameters[4] = (unsigned int)out;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_hash_sha512_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_hmac_ripemd160_init ( cx_hmac_ripemd160_t * hmac, unsigned char * key, unsigned int key_len ) 
{
  unsigned int ret;
  unsigned int parameters [2+3];
  parameters[0] = (unsigned int)SYSCALL_cx_hmac_ripemd160_init_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)hmac;
  parameters[3] = (unsigned int)key;
  parameters[4] = (unsigned int)key_len;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_hmac_ripemd160_init_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_hmac_sha256_init ( cx_hmac_sha256_t * hmac, unsigned char * key, unsigned int key_len ) 
{
  unsigned int ret;
  unsigned int parameters [2+3];
  parameters[0] = (unsigned int)SYSCALL_cx_hmac_sha256_init_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)hmac;
  parameters[3] = (unsigned int)key;
  parameters[4] = (unsigned int)key_len;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_hmac_sha256_init_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_hmac_sha512_init ( cx_hmac_sha512_t * hmac, unsigned char * key, unsigned int key_len ) 
{
  unsigned int ret;
  unsigned int parameters [2+3];
  parameters[0] = (unsigned int)SYSCALL_cx_hmac_sha512_init_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)hmac;
  parameters[3] = (unsigned int)key;
  parameters[4] = (unsigned int)key_len;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_hmac_sha512_init_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_hmac ( cx_hmac_t * hmac, int mode, unsigned char * in, unsigned int len, unsigned char * mac ) 
{
  unsigned int ret;
  unsigned int parameters [2+5];
  parameters[0] = (unsigned int)SYSCALL_cx_hmac_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)hmac;
  parameters[3] = (unsigned int)mode;
  parameters[4] = (unsigned int)in;
  parameters[5] = (unsigned int)len;
  parameters[6] = (unsigned int)mac;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_hmac_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_hmac_sha512 ( unsigned char * key, unsigned int key_len, unsigned char * in, unsigned int len, unsigned char * out ) 
{
  unsigned int ret;
  unsigned int parameters [2+5];
  parameters[0] = (unsigned int)SYSCALL_cx_hmac_sha512_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)key;
  parameters[3] = (unsigned int)key_len;
  parameters[4] = (unsigned int)in;
  parameters[5] = (unsigned int)len;
  parameters[6] = (unsigned int)out;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_hmac_sha512_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_hmac_sha256 ( unsigned char * key, unsigned int key_len, unsigned char * in, unsigned int len, unsigned char * out ) 
{
  unsigned int ret;
  unsigned int parameters [2+5];
  parameters[0] = (unsigned int)SYSCALL_cx_hmac_sha256_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)key;
  parameters[3] = (unsigned int)key_len;
  parameters[4] = (unsigned int)in;
  parameters[5] = (unsigned int)len;
  parameters[6] = (unsigned int)out;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_hmac_sha256_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

void cx_pbkdf2_sha512 ( unsigned char * password, unsigned short passwordlen, unsigned char * salt, unsigned short saltlen, unsigned int iterations, unsigned char * out, unsigned int outLength ) 
{
  unsigned int ret;
  unsigned int parameters [2+7];
  parameters[0] = (unsigned int)SYSCALL_cx_pbkdf2_sha512_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)password;
  parameters[3] = (unsigned int)passwordlen;
  parameters[4] = (unsigned int)salt;
  parameters[5] = (unsigned int)saltlen;
  parameters[6] = (unsigned int)iterations;
  parameters[7] = (unsigned int)out;
  parameters[8] = (unsigned int)outLength;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_pbkdf2_sha512_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
}

int cx_des_init_key ( unsigned char * rawkey, unsigned int key_len, cx_des_key_t * key ) 
{
  unsigned int ret;
  unsigned int parameters [2+3];
  parameters[0] = (unsigned int)SYSCALL_cx_des_init_key_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)rawkey;
  parameters[3] = (unsigned int)key_len;
  parameters[4] = (unsigned int)key;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_des_init_key_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_des_iv ( cx_des_key_t * key, int mode, unsigned char * iv, unsigned char * in, unsigned int len, unsigned char * out ) 
{
  unsigned int ret;
  unsigned int parameters [2+6];
  parameters[0] = (unsigned int)SYSCALL_cx_des_iv_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)key;
  parameters[3] = (unsigned int)mode;
  parameters[4] = (unsigned int)iv;
  parameters[5] = (unsigned int)in;
  parameters[6] = (unsigned int)len;
  parameters[7] = (unsigned int)out;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_des_iv_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_des ( cx_des_key_t * key, int mode, unsigned char * in, unsigned int len, unsigned char * out ) 
{
  unsigned int ret;
  unsigned int parameters [2+5];
  parameters[0] = (unsigned int)SYSCALL_cx_des_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)key;
  parameters[3] = (unsigned int)mode;
  parameters[4] = (unsigned int)in;
  parameters[5] = (unsigned int)len;
  parameters[6] = (unsigned int)out;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_des_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_aes_init_key ( unsigned char * rawkey, unsigned int key_len, cx_aes_key_t * key ) 
{
  unsigned int ret;
  unsigned int parameters [2+3];
  parameters[0] = (unsigned int)SYSCALL_cx_aes_init_key_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)rawkey;
  parameters[3] = (unsigned int)key_len;
  parameters[4] = (unsigned int)key;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_aes_init_key_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_aes_iv ( cx_aes_key_t * key, int mode, unsigned char * iv, unsigned char * in, unsigned int len, unsigned char * out ) 
{
  unsigned int ret;
  unsigned int parameters [2+6];
  parameters[0] = (unsigned int)SYSCALL_cx_aes_iv_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)key;
  parameters[3] = (unsigned int)mode;
  parameters[4] = (unsigned int)iv;
  parameters[5] = (unsigned int)in;
  parameters[6] = (unsigned int)len;
  parameters[7] = (unsigned int)out;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_aes_iv_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_aes ( cx_aes_key_t * key, int mode, unsigned char * in, unsigned int len, unsigned char * out ) 
{
  unsigned int ret;
  unsigned int parameters [2+5];
  parameters[0] = (unsigned int)SYSCALL_cx_aes_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)key;
  parameters[3] = (unsigned int)mode;
  parameters[4] = (unsigned int)in;
  parameters[5] = (unsigned int)len;
  parameters[6] = (unsigned int)out;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_aes_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_rsa_init_public_key ( unsigned char * exponent, unsigned char * modulus, unsigned int modulus_len, cx_rsa_public_key_t * key ) 
{
  unsigned int ret;
  unsigned int parameters [2+4];
  parameters[0] = (unsigned int)SYSCALL_cx_rsa_init_public_key_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)exponent;
  parameters[3] = (unsigned int)modulus;
  parameters[4] = (unsigned int)modulus_len;
  parameters[5] = (unsigned int)key;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_rsa_init_public_key_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_rsa_init_private_key ( unsigned char * exponent, unsigned char * modulus, unsigned int modulus_len, cx_rsa_private_key_t * key ) 
{
  unsigned int ret;
  unsigned int parameters [2+4];
  parameters[0] = (unsigned int)SYSCALL_cx_rsa_init_private_key_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)exponent;
  parameters[3] = (unsigned int)modulus;
  parameters[4] = (unsigned int)modulus_len;
  parameters[5] = (unsigned int)key;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_rsa_init_private_key_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_rsa_generate_pair ( unsigned int modulus_len, cx_rsa_public_key_t * public_key, cx_rsa_private_key_t * private_key, unsigned long int pub_exponent ) 
{
  unsigned int ret;
  unsigned int parameters [2+4];
  parameters[0] = (unsigned int)SYSCALL_cx_rsa_generate_pair_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)modulus_len;
  parameters[3] = (unsigned int)public_key;
  parameters[4] = (unsigned int)private_key;
  parameters[5] = (unsigned int)pub_exponent;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_rsa_generate_pair_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_rsa_sign ( cx_rsa_private_key_t * key, int mode, cx_md_t hashID, unsigned char * hash, unsigned int hash_len, unsigned char * sig, unsigned int sig_len ) 
{
  unsigned int ret;
  unsigned int parameters [2+7];
  parameters[0] = (unsigned int)SYSCALL_cx_rsa_sign_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)key;
  parameters[3] = (unsigned int)mode;
  parameters[4] = (unsigned int)hashID;
  parameters[5] = (unsigned int)hash;
  parameters[6] = (unsigned int)hash_len;
  parameters[7] = (unsigned int)sig;
  parameters[8] = (unsigned int)sig_len;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_rsa_sign_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_rsa_verify ( cx_rsa_public_key_t * key, int mode, cx_md_t hashID, unsigned char * hash, unsigned int hash_len, unsigned char * sig, unsigned int sig_len ) 
{
  unsigned int ret;
  unsigned int parameters [2+7];
  parameters[0] = (unsigned int)SYSCALL_cx_rsa_verify_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)key;
  parameters[3] = (unsigned int)mode;
  parameters[4] = (unsigned int)hashID;
  parameters[5] = (unsigned int)hash;
  parameters[6] = (unsigned int)hash_len;
  parameters[7] = (unsigned int)sig;
  parameters[8] = (unsigned int)sig_len;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_rsa_verify_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_rsa_encrypt ( cx_rsa_public_key_t * key, int mode, cx_md_t hashID, unsigned char * mesg, unsigned int mesg_len, unsigned char * enc, unsigned int enc_len ) 
{
  unsigned int ret;
  unsigned int parameters [2+7];
  parameters[0] = (unsigned int)SYSCALL_cx_rsa_encrypt_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)key;
  parameters[3] = (unsigned int)mode;
  parameters[4] = (unsigned int)hashID;
  parameters[5] = (unsigned int)mesg;
  parameters[6] = (unsigned int)mesg_len;
  parameters[7] = (unsigned int)enc;
  parameters[8] = (unsigned int)enc_len;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_rsa_encrypt_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_rsa_decrypt ( cx_rsa_private_key_t * key, int mode, cx_md_t hashID, unsigned char * mesg, unsigned int mesg_len, unsigned char * dec, unsigned int dec_len ) 
{
  unsigned int ret;
  unsigned int parameters [2+7];
  parameters[0] = (unsigned int)SYSCALL_cx_rsa_decrypt_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)key;
  parameters[3] = (unsigned int)mode;
  parameters[4] = (unsigned int)hashID;
  parameters[5] = (unsigned int)mesg;
  parameters[6] = (unsigned int)mesg_len;
  parameters[7] = (unsigned int)dec;
  parameters[8] = (unsigned int)dec_len;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_rsa_decrypt_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_ecfp_is_valid_point ( cx_curve_domain_t * domain, unsigned char * point ) 
{
  unsigned int ret;
  unsigned int parameters [2+2];
  parameters[0] = (unsigned int)SYSCALL_cx_ecfp_is_valid_point_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)domain;
  parameters[3] = (unsigned int)point;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_ecfp_is_valid_point_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_ecfp_add_point ( cx_curve_domain_t * domain, unsigned char * R, unsigned char * P, unsigned char * Q ) 
{
  unsigned int ret;
  unsigned int parameters [2+4];
  parameters[0] = (unsigned int)SYSCALL_cx_ecfp_add_point_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)domain;
  parameters[3] = (unsigned int)R;
  parameters[4] = (unsigned int)P;
  parameters[5] = (unsigned int)Q;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_ecfp_add_point_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_ecfp_scalar_mult ( cx_curve_domain_t * domain, unsigned char * P, unsigned char * k, unsigned int k_len ) 
{
  unsigned int ret;
  unsigned int parameters [2+4];
  parameters[0] = (unsigned int)SYSCALL_cx_ecfp_scalar_mult_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)domain;
  parameters[3] = (unsigned int)P;
  parameters[4] = (unsigned int)k;
  parameters[5] = (unsigned int)k_len;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_ecfp_scalar_mult_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_ecfp_init_public_key ( cx_curve_t curve, unsigned char * rawkey, unsigned int key_len, cx_ecfp_public_key_t * key ) 
{
  unsigned int ret;
  unsigned int parameters [2+4];
  parameters[0] = (unsigned int)SYSCALL_cx_ecfp_init_public_key_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)curve;
  parameters[3] = (unsigned int)rawkey;
  parameters[4] = (unsigned int)key_len;
  parameters[5] = (unsigned int)key;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_ecfp_init_public_key_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_ecfp_init_private_key ( cx_curve_t curve, unsigned char * rawkey, unsigned int key_len, cx_ecfp_private_key_t * key ) 
{
  unsigned int ret;
  unsigned int parameters [2+4];
  parameters[0] = (unsigned int)SYSCALL_cx_ecfp_init_private_key_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)curve;
  parameters[3] = (unsigned int)rawkey;
  parameters[4] = (unsigned int)key_len;
  parameters[5] = (unsigned int)key;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_ecfp_init_private_key_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_ecfp_generate_pair ( cx_curve_t curve, cx_ecfp_public_key_t * pubkey, cx_ecfp_private_key_t * privkey, int keepprivate ) 
{
  unsigned int ret;
  unsigned int parameters [2+4];
  parameters[0] = (unsigned int)SYSCALL_cx_ecfp_generate_pair_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)curve;
  parameters[3] = (unsigned int)pubkey;
  parameters[4] = (unsigned int)privkey;
  parameters[5] = (unsigned int)keepprivate;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_ecfp_generate_pair_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_borromean_sign ( cx_ecfp_private_key_t * * privkeys, cx_ecfp_public_key_t * * pubkeys, unsigned int * rsizes, unsigned int * pv_keys_index, unsigned int rcount, unsigned int mode, cx_md_t hashID, unsigned char * msg, unsigned int msg_len, unsigned char * sig ) 
{
  unsigned int ret;
  unsigned int parameters [2+10];
  parameters[0] = (unsigned int)SYSCALL_cx_borromean_sign_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)privkeys;
  parameters[3] = (unsigned int)pubkeys;
  parameters[4] = (unsigned int)rsizes;
  parameters[5] = (unsigned int)pv_keys_index;
  parameters[6] = (unsigned int)rcount;
  parameters[7] = (unsigned int)mode;
  parameters[8] = (unsigned int)hashID;
  parameters[9] = (unsigned int)msg;
  parameters[10] = (unsigned int)msg_len;
  parameters[11] = (unsigned int)sig;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_borromean_sign_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_borromean_verify ( cx_ecfp_public_key_t * * pubkeys, unsigned int * rsizes, unsigned int rcount, int mode, cx_md_t hashID, unsigned char * msg, unsigned int msg_len, unsigned char * sig, unsigned int sig_len ) 
{
  unsigned int ret;
  unsigned int parameters [2+9];
  parameters[0] = (unsigned int)SYSCALL_cx_borromean_verify_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)pubkeys;
  parameters[3] = (unsigned int)rsizes;
  parameters[4] = (unsigned int)rcount;
  parameters[5] = (unsigned int)mode;
  parameters[6] = (unsigned int)hashID;
  parameters[7] = (unsigned int)msg;
  parameters[8] = (unsigned int)msg_len;
  parameters[9] = (unsigned int)sig;
  parameters[10] = (unsigned int)sig_len;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_borromean_verify_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_ecschnorr_sign ( cx_ecfp_private_key_t * pvkey, int mode, cx_md_t hashID, unsigned char * msg, unsigned int msg_len, unsigned char * sig ) 
{
  unsigned int ret;
  unsigned int parameters [2+6];
  parameters[0] = (unsigned int)SYSCALL_cx_ecschnorr_sign_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)pvkey;
  parameters[3] = (unsigned int)mode;
  parameters[4] = (unsigned int)hashID;
  parameters[5] = (unsigned int)msg;
  parameters[6] = (unsigned int)msg_len;
  parameters[7] = (unsigned int)sig;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_ecschnorr_sign_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_ecschnorr_verify ( cx_ecfp_public_key_t * pukey, int mode, cx_md_t hashID, unsigned char * msg, unsigned int msg_len, unsigned char * sig, unsigned int sig_len ) 
{
  unsigned int ret;
  unsigned int parameters [2+7];
  parameters[0] = (unsigned int)SYSCALL_cx_ecschnorr_verify_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)pukey;
  parameters[3] = (unsigned int)mode;
  parameters[4] = (unsigned int)hashID;
  parameters[5] = (unsigned int)msg;
  parameters[6] = (unsigned int)msg_len;
  parameters[7] = (unsigned int)sig;
  parameters[8] = (unsigned int)sig_len;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_ecschnorr_verify_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

void cx_edward_compress_point ( cx_curve_twisted_edward_t * domain, unsigned char * P ) 
{
  unsigned int ret;
  unsigned int parameters [2+2];
  parameters[0] = (unsigned int)SYSCALL_cx_edward_compress_point_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)domain;
  parameters[3] = (unsigned int)P;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_edward_compress_point_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
}

void cx_edward_decompress_point ( cx_curve_twisted_edward_t * domain, unsigned char * P ) 
{
  unsigned int ret;
  unsigned int parameters [2+2];
  parameters[0] = (unsigned int)SYSCALL_cx_edward_decompress_point_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)domain;
  parameters[3] = (unsigned int)P;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_edward_decompress_point_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
}

void cx_eddsa_get_public_key ( cx_ecfp_private_key_t * pvkey, cx_ecfp_public_key_t * pukey ) 
{
  unsigned int ret;
  unsigned int parameters [2+2];
  parameters[0] = (unsigned int)SYSCALL_cx_eddsa_get_public_key_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)pvkey;
  parameters[3] = (unsigned int)pukey;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_eddsa_get_public_key_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
}

int cx_eddsa_sign ( cx_ecfp_private_key_t * pvkey, cx_ecfp_public_key_t * pukey, int mode, cx_md_t hashID, unsigned char * hash, unsigned int hash_len, unsigned char * sig ) 
{
  unsigned int ret;
  unsigned int parameters [2+7];
  parameters[0] = (unsigned int)SYSCALL_cx_eddsa_sign_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)pvkey;
  parameters[3] = (unsigned int)pukey;
  parameters[4] = (unsigned int)mode;
  parameters[5] = (unsigned int)hashID;
  parameters[6] = (unsigned int)hash;
  parameters[7] = (unsigned int)hash_len;
  parameters[8] = (unsigned int)sig;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_eddsa_sign_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_eddsa_verify ( cx_ecfp_public_key_t * key, int mode, cx_md_t hashID, unsigned char * hash, unsigned int hash_len, unsigned char * sig, unsigned int sig_len ) 
{
  unsigned int ret;
  unsigned int parameters [2+7];
  parameters[0] = (unsigned int)SYSCALL_cx_eddsa_verify_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)key;
  parameters[3] = (unsigned int)mode;
  parameters[4] = (unsigned int)hashID;
  parameters[5] = (unsigned int)hash;
  parameters[6] = (unsigned int)hash_len;
  parameters[7] = (unsigned int)sig;
  parameters[8] = (unsigned int)sig_len;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_eddsa_verify_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_ecdsa_sign ( cx_ecfp_private_key_t * key, int mode, cx_md_t hashID, unsigned char * hash, unsigned int hash_len, unsigned char * sig ) 
{
  unsigned int ret;
  unsigned int parameters [2+6];
  parameters[0] = (unsigned int)SYSCALL_cx_ecdsa_sign_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)key;
  parameters[3] = (unsigned int)mode;
  parameters[4] = (unsigned int)hashID;
  parameters[5] = (unsigned int)hash;
  parameters[6] = (unsigned int)hash_len;
  parameters[7] = (unsigned int)sig;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_ecdsa_sign_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_ecdsa_verify ( cx_ecfp_public_key_t * key, int mode, cx_md_t hashID, unsigned char * hash, unsigned int hash_len, unsigned char * sig, unsigned int sig_len ) 
{
  unsigned int ret;
  unsigned int parameters [2+7];
  parameters[0] = (unsigned int)SYSCALL_cx_ecdsa_verify_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)key;
  parameters[3] = (unsigned int)mode;
  parameters[4] = (unsigned int)hashID;
  parameters[5] = (unsigned int)hash;
  parameters[6] = (unsigned int)hash_len;
  parameters[7] = (unsigned int)sig;
  parameters[8] = (unsigned int)sig_len;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_ecdsa_verify_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_ecdh ( cx_ecfp_private_key_t * key, int mode, unsigned char * public_point, unsigned char * secret ) 
{
  unsigned int ret;
  unsigned int parameters [2+4];
  parameters[0] = (unsigned int)SYSCALL_cx_ecdh_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)key;
  parameters[3] = (unsigned int)mode;
  parameters[4] = (unsigned int)public_point;
  parameters[5] = (unsigned int)secret;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_ecdh_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

unsigned short cx_crc16 ( void * buffer, unsigned int len ) 
{
  unsigned int ret;
  unsigned int parameters [2+2];
  parameters[0] = (unsigned int)SYSCALL_cx_crc16_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)buffer;
  parameters[3] = (unsigned int)len;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_crc16_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned short)ret;
}

unsigned short cx_crc16_update ( unsigned short crc, void * buffer, unsigned int len ) 
{
  unsigned int ret;
  unsigned int parameters [2+3];
  parameters[0] = (unsigned int)SYSCALL_cx_crc16_update_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)crc;
  parameters[3] = (unsigned int)buffer;
  parameters[4] = (unsigned int)len;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_crc16_update_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned short)ret;
}

int cx_math_cmp ( unsigned char * a, unsigned char * b, unsigned int len ) 
{
  unsigned int ret;
  unsigned int parameters [2+3];
  parameters[0] = (unsigned int)SYSCALL_cx_math_cmp_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)a;
  parameters[3] = (unsigned int)b;
  parameters[4] = (unsigned int)len;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_math_cmp_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_math_is_zero ( unsigned char * a, unsigned int len ) 
{
  unsigned int ret;
  unsigned int parameters [2+2];
  parameters[0] = (unsigned int)SYSCALL_cx_math_is_zero_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)a;
  parameters[3] = (unsigned int)len;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_math_is_zero_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_math_add ( unsigned char * r, unsigned char * a, unsigned char * b, unsigned int len ) 
{
  unsigned int ret;
  unsigned int parameters [2+4];
  parameters[0] = (unsigned int)SYSCALL_cx_math_add_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)r;
  parameters[3] = (unsigned int)a;
  parameters[4] = (unsigned int)b;
  parameters[5] = (unsigned int)len;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_math_add_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

int cx_math_sub ( unsigned char * r, unsigned char * a, unsigned char * b, unsigned int len ) 
{
  unsigned int ret;
  unsigned int parameters [2+4];
  parameters[0] = (unsigned int)SYSCALL_cx_math_sub_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)r;
  parameters[3] = (unsigned int)a;
  parameters[4] = (unsigned int)b;
  parameters[5] = (unsigned int)len;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_math_sub_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

void cx_math_mult ( unsigned char * r, unsigned char * a, unsigned char * b, unsigned int len ) 
{
  unsigned int ret;
  unsigned int parameters [2+4];
  parameters[0] = (unsigned int)SYSCALL_cx_math_mult_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)r;
  parameters[3] = (unsigned int)a;
  parameters[4] = (unsigned int)b;
  parameters[5] = (unsigned int)len;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_math_mult_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
}

void cx_math_addm ( unsigned char * r, unsigned char * a, unsigned char * b, unsigned char * m, unsigned int len ) 
{
  unsigned int ret;
  unsigned int parameters [2+5];
  parameters[0] = (unsigned int)SYSCALL_cx_math_addm_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)r;
  parameters[3] = (unsigned int)a;
  parameters[4] = (unsigned int)b;
  parameters[5] = (unsigned int)m;
  parameters[6] = (unsigned int)len;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_math_addm_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
}

void cx_math_subm ( unsigned char * r, unsigned char * a, unsigned char * b, unsigned char * m, unsigned int len ) 
{
  unsigned int ret;
  unsigned int parameters [2+5];
  parameters[0] = (unsigned int)SYSCALL_cx_math_subm_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)r;
  parameters[3] = (unsigned int)a;
  parameters[4] = (unsigned int)b;
  parameters[5] = (unsigned int)m;
  parameters[6] = (unsigned int)len;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_math_subm_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
}

void cx_math_multm ( unsigned char * r, unsigned char * a, unsigned char * b, unsigned char * m, unsigned int len ) 
{
  unsigned int ret;
  unsigned int parameters [2+5];
  parameters[0] = (unsigned int)SYSCALL_cx_math_multm_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)r;
  parameters[3] = (unsigned int)a;
  parameters[4] = (unsigned int)b;
  parameters[5] = (unsigned int)m;
  parameters[6] = (unsigned int)len;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_math_multm_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
}

void cx_math_powm ( unsigned char * r, unsigned char * a, unsigned char * e, unsigned int len_e, unsigned char * m, unsigned int len ) 
{
  unsigned int ret;
  unsigned int parameters [2+6];
  parameters[0] = (unsigned int)SYSCALL_cx_math_powm_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)r;
  parameters[3] = (unsigned int)a;
  parameters[4] = (unsigned int)e;
  parameters[5] = (unsigned int)len_e;
  parameters[6] = (unsigned int)m;
  parameters[7] = (unsigned int)len;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_math_powm_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
}

void cx_math_modm ( unsigned char * v, unsigned int len_v, unsigned char * m, unsigned int len_m ) 
{
  unsigned int ret;
  unsigned int parameters [2+4];
  parameters[0] = (unsigned int)SYSCALL_cx_math_modm_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)v;
  parameters[3] = (unsigned int)len_v;
  parameters[4] = (unsigned int)m;
  parameters[5] = (unsigned int)len_m;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_math_modm_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
}

void cx_math_invprimem ( unsigned char * r, unsigned char * a, unsigned char * m, unsigned int len ) 
{
  unsigned int ret;
  unsigned int parameters [2+4];
  parameters[0] = (unsigned int)SYSCALL_cx_math_invprimem_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)r;
  parameters[3] = (unsigned int)a;
  parameters[4] = (unsigned int)m;
  parameters[5] = (unsigned int)len;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_math_invprimem_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
}

void cx_math_invintm ( unsigned char * r, unsigned long int a, unsigned char * m, unsigned int len ) 
{
  unsigned int ret;
  unsigned int parameters [2+4];
  parameters[0] = (unsigned int)SYSCALL_cx_math_invintm_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)r;
  parameters[3] = (unsigned int)a;
  parameters[4] = (unsigned int)m;
  parameters[5] = (unsigned int)len;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_math_invintm_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
}

int cx_math_is_prime ( unsigned char * p, unsigned int len ) 
{
  unsigned int ret;
  unsigned int parameters [2+2];
  parameters[0] = (unsigned int)SYSCALL_cx_math_is_prime_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)p;
  parameters[3] = (unsigned int)len;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_math_is_prime_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (int)ret;
}

void cx_math_next_prime ( unsigned char * n, unsigned int len ) 
{
  unsigned int ret;
  unsigned int parameters [2+2];
  parameters[0] = (unsigned int)SYSCALL_cx_math_next_prime_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)n;
  parameters[3] = (unsigned int)len;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_cx_math_next_prime_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
}

void os_perso_wipe ( void ) 
{
  unsigned int ret;
  unsigned int parameters [2];
  parameters[0] = (unsigned int)SYSCALL_os_perso_wipe_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_os_perso_wipe_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
}

void os_perso_set_pin ( unsigned char * pin, unsigned int length ) 
{
  unsigned int ret;
  unsigned int parameters [2+2];
  parameters[0] = (unsigned int)SYSCALL_os_perso_set_pin_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)pin;
  parameters[3] = (unsigned int)length;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_os_perso_set_pin_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
}

void os_perso_set_seed ( unsigned char * seed, unsigned int length ) 
{
  unsigned int ret;
  unsigned int parameters [2+2];
  parameters[0] = (unsigned int)SYSCALL_os_perso_set_seed_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)seed;
  parameters[3] = (unsigned int)length;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_os_perso_set_seed_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
}

void os_perso_set_alternate_pin ( unsigned char * pin, unsigned int pinLength ) 
{
  unsigned int ret;
  unsigned int parameters [2+2];
  parameters[0] = (unsigned int)SYSCALL_os_perso_set_alternate_pin_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)pin;
  parameters[3] = (unsigned int)pinLength;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_os_perso_set_alternate_pin_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
}

void os_perso_set_alternate_seed ( unsigned char * seed, unsigned int seedLength ) 
{
  unsigned int ret;
  unsigned int parameters [2+2];
  parameters[0] = (unsigned int)SYSCALL_os_perso_set_alternate_seed_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)seed;
  parameters[3] = (unsigned int)seedLength;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_os_perso_set_alternate_seed_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
}

void os_perso_set_words ( unsigned char * words, unsigned int length ) 
{
  unsigned int ret;
  unsigned int parameters [2+2];
  parameters[0] = (unsigned int)SYSCALL_os_perso_set_words_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)words;
  parameters[3] = (unsigned int)length;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_os_perso_set_words_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
}

void os_perso_set_devname ( unsigned char * devname, unsigned int length ) 
{
  unsigned int ret;
  unsigned int parameters [2+2];
  parameters[0] = (unsigned int)SYSCALL_os_perso_set_devname_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)devname;
  parameters[3] = (unsigned int)length;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_os_perso_set_devname_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
}

void os_perso_finalize ( void ) 
{
  unsigned int ret;
  unsigned int parameters [2];
  parameters[0] = (unsigned int)SYSCALL_os_perso_finalize_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_os_perso_finalize_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
}

unsigned int os_perso_isonboarded ( void ) 
{
  unsigned int ret;
  unsigned int parameters [2];
  parameters[0] = (unsigned int)SYSCALL_os_perso_isonboarded_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_os_perso_isonboarded_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned int)ret;
}

unsigned int os_perso_get_devname ( unsigned char * devname, unsigned int length ) 
{
  unsigned int ret;
  unsigned int parameters [2+2];
  parameters[0] = (unsigned int)SYSCALL_os_perso_get_devname_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)devname;
  parameters[3] = (unsigned int)length;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_os_perso_get_devname_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned int)ret;
}

void os_perso_derive_node_bip32 ( cx_curve_t curve, unsigned int * path, unsigned int pathLength, unsigned char * privateKey, unsigned char * chain ) 
{
  unsigned int ret;
  unsigned int parameters [2+5];
  parameters[0] = (unsigned int)SYSCALL_os_perso_derive_node_bip32_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)curve;
  parameters[3] = (unsigned int)path;
  parameters[4] = (unsigned int)pathLength;
  parameters[5] = (unsigned int)privateKey;
  parameters[6] = (unsigned int)chain;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_os_perso_derive_node_bip32_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
}

unsigned int os_endorsement_get_code_hash ( unsigned char * buffer ) 
{
  unsigned int ret;
  unsigned int parameters [2+1];
  parameters[0] = (unsigned int)SYSCALL_os_endorsement_get_code_hash_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)buffer;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_os_endorsement_get_code_hash_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned int)ret;
}

unsigned int os_endorsement_get_public_key ( unsigned char index, unsigned char * buffer ) 
{
  unsigned int ret;
  unsigned int parameters [2+2];
  parameters[0] = (unsigned int)SYSCALL_os_endorsement_get_public_key_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)index;
  parameters[3] = (unsigned int)buffer;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_os_endorsement_get_public_key_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned int)ret;
}

unsigned int os_endorsement_get_public_key_certificate ( unsigned char index, unsigned char * buffer ) 
{
  unsigned int ret;
  unsigned int parameters [2+2];
  parameters[0] = (unsigned int)SYSCALL_os_endorsement_get_public_key_certificate_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)index;
  parameters[3] = (unsigned int)buffer;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_os_endorsement_get_public_key_certificate_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned int)ret;
}

unsigned int os_endorsement_key1_get_app_secret ( unsigned char * buffer ) 
{
  unsigned int ret;
  unsigned int parameters [2+1];
  parameters[0] = (unsigned int)SYSCALL_os_endorsement_key1_get_app_secret_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)buffer;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_os_endorsement_key1_get_app_secret_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned int)ret;
}

unsigned int os_endorsement_key1_sign_data ( unsigned char * src, unsigned int srcLength, unsigned char * signature ) 
{
  unsigned int ret;
  unsigned int parameters [2+3];
  parameters[0] = (unsigned int)SYSCALL_os_endorsement_key1_sign_data_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)src;
  parameters[3] = (unsigned int)srcLength;
  parameters[4] = (unsigned int)signature;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_os_endorsement_key1_sign_data_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned int)ret;
}

unsigned int os_endorsement_key2_derive_sign_data ( unsigned char * src, unsigned int srcLength, unsigned char * signature ) 
{
  unsigned int ret;
  unsigned int parameters [2+3];
  parameters[0] = (unsigned int)SYSCALL_os_endorsement_key2_derive_sign_data_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)src;
  parameters[3] = (unsigned int)srcLength;
  parameters[4] = (unsigned int)signature;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_os_endorsement_key2_derive_sign_data_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned int)ret;
}

unsigned int os_global_pin_is_validated ( void ) 
{
  unsigned int ret;
  unsigned int parameters [2];
  parameters[0] = (unsigned int)SYSCALL_os_global_pin_is_validated_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_os_global_pin_is_validated_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned int)ret;
}

unsigned int os_global_pin_check ( unsigned char * pin_buffer, unsigned char pin_length ) 
{
  unsigned int ret;
  unsigned int parameters [2+2];
  parameters[0] = (unsigned int)SYSCALL_os_global_pin_check_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)pin_buffer;
  parameters[3] = (unsigned int)pin_length;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_os_global_pin_check_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned int)ret;
}

void os_global_pin_invalidate ( void ) 
{
  unsigned int ret;
  unsigned int parameters [2];
  parameters[0] = (unsigned int)SYSCALL_os_global_pin_invalidate_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_os_global_pin_invalidate_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
}

unsigned int os_global_pin_retries ( void ) 
{
  unsigned int ret;
  unsigned int parameters [2];
  parameters[0] = (unsigned int)SYSCALL_os_global_pin_retries_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_os_global_pin_retries_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned int)ret;
}

unsigned int os_registry_count ( void ) 
{
  unsigned int ret;
  unsigned int parameters [2];
  parameters[0] = (unsigned int)SYSCALL_os_registry_count_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_os_registry_count_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned int)ret;
}

void os_registry_get ( unsigned int index, application_t * out_application_entry ) 
{
  unsigned int ret;
  unsigned int parameters [2+2];
  parameters[0] = (unsigned int)SYSCALL_os_registry_get_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)index;
  parameters[3] = (unsigned int)out_application_entry;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_os_registry_get_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
}

unsigned int os_sched_exec ( unsigned int application_index ) 
{
  unsigned int ret;
  unsigned int parameters [2+1];
  parameters[0] = (unsigned int)SYSCALL_os_sched_exec_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)application_index;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_os_sched_exec_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned int)ret;
}

void os_sched_exit ( unsigned int exit_code ) 
{
  unsigned int ret;
  unsigned int parameters [2+1];
  parameters[0] = (unsigned int)SYSCALL_os_sched_exit_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)exit_code;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_os_sched_exit_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
}

void os_ux_register ( bolos_ux_params_t * parameter_ram_pointer ) 
{
  unsigned int ret;
  unsigned int parameters [2+1];
  parameters[0] = (unsigned int)SYSCALL_os_ux_register_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)parameter_ram_pointer;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_os_ux_register_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
}

unsigned int os_ux ( bolos_ux_params_t * params ) 
{
  unsigned int ret;
  unsigned int parameters [2+1];
  parameters[0] = (unsigned int)SYSCALL_os_ux_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)params;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_os_ux_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned int)ret;
}

unsigned int os_flags ( void ) 
{
  unsigned int ret;
  unsigned int parameters [2];
  parameters[0] = (unsigned int)SYSCALL_os_flags_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_os_flags_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned int)ret;
}

unsigned int os_version ( unsigned char * version, unsigned int maxlength ) 
{
  unsigned int ret;
  unsigned int parameters [2+2];
  parameters[0] = (unsigned int)SYSCALL_os_version_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)version;
  parameters[3] = (unsigned int)maxlength;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_os_version_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned int)ret;
}

unsigned int os_seph_features ( void ) 
{
  unsigned int ret;
  unsigned int parameters [2];
  parameters[0] = (unsigned int)SYSCALL_os_seph_features_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_os_seph_features_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned int)ret;
}

unsigned int os_seph_version ( unsigned char * version, unsigned int maxlength ) 
{
  unsigned int ret;
  unsigned int parameters [2+2];
  parameters[0] = (unsigned int)SYSCALL_os_seph_version_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)version;
  parameters[3] = (unsigned int)maxlength;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_os_seph_version_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned int)ret;
}

unsigned int os_setting_get ( unsigned int setting_id ) 
{
  unsigned int ret;
  unsigned int parameters [2+1];
  parameters[0] = (unsigned int)SYSCALL_os_setting_get_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)setting_id;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_os_setting_get_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned int)ret;
}

void os_setting_set ( unsigned int setting_id, unsigned int value ) 
{
  unsigned int ret;
  unsigned int parameters [2+2];
  parameters[0] = (unsigned int)SYSCALL_os_setting_set_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)setting_id;
  parameters[3] = (unsigned int)value;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_os_setting_set_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
}

void io_seproxyhal_spi_send ( const unsigned char * buffer, unsigned short length ) 
{
  unsigned int ret;
  unsigned int parameters [2+2];
  parameters[0] = (unsigned int)SYSCALL_io_seproxyhal_spi_send_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)buffer;
  parameters[3] = (unsigned int)length;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_io_seproxyhal_spi_send_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
}

unsigned int io_seproxyhal_spi_is_status_sent ( void ) 
{
  unsigned int ret;
  unsigned int parameters [2];
  parameters[0] = (unsigned int)SYSCALL_io_seproxyhal_spi_is_status_sent_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_io_seproxyhal_spi_is_status_sent_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned int)ret;
}

unsigned short io_seproxyhal_spi_recv ( unsigned char * buffer, unsigned short maxlength, unsigned int flags ) 
{
  unsigned int ret;
  unsigned int parameters [2+3];
  parameters[0] = (unsigned int)SYSCALL_io_seproxyhal_spi_recv_ID_IN;
  parameters[1] = (unsigned int)G_try_last_open_context->jmp_buf;
  parameters[2] = (unsigned int)buffer;
  parameters[3] = (unsigned int)maxlength;
  parameters[4] = (unsigned int)flags;

                              asm volatile("mov r0, %0"::"r"(parameters));
                              asm volatile("svc #1");
                              asm volatile("mov %0, r0":"=r"(ret));
                                if (parameters[0] != SYSCALL_io_seproxyhal_spi_recv_ID_OUT)
  {
    THROW(EXCEPTION_SECURITY);
  }
  return (unsigned short)ret;
}

