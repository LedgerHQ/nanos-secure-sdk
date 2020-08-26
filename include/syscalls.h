
/* MACHINE GENERATED: DO NOT MODIFY */
#ifndef SYSCALL_DEFS_H
#define SYSCALL_DEFS_H

#define SYSCALL_check_api_level_ID_IN 0x60000137UL
#define SYSCALL_check_api_level_ID_OUT 0x900001c6UL
__attribute__((always_inline)) inline void
check_api_level_inline(unsigned int apiLevel) {
  volatile unsigned int parameters[2 + 1];
  parameters[0] = (unsigned int)apiLevel;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_check_api_level_ID_IN)
                 : "r0", "r1");
}

#define SYSCALL_halt_ID_IN 0x6000023cUL
#define SYSCALL_halt_ID_OUT 0x9000020dUL
__attribute__((always_inline)) inline void halt_inline(void) {
  volatile unsigned int parameters[2];
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_halt_ID_IN)
                 : "r0", "r1");
}

#define SYSCALL_nvm_write_ID_IN 0x6000037fUL
#define SYSCALL_nvm_write_ID_OUT 0x900003bcUL
__attribute__((always_inline)) inline void
nvm_write_inline(void *dst_adr, void *src_adr, unsigned int src_len) {
  volatile unsigned int parameters[2 + 3];
  parameters[0] = (unsigned int)dst_adr;
  parameters[1] = (unsigned int)src_adr;
  parameters[2] = (unsigned int)src_len;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_nvm_write_ID_IN)
                 : "r0", "r1");
}

#define SYSCALL_cx_rng_u8_ID_IN 0x600004c0UL
#define SYSCALL_cx_rng_u8_ID_OUT 0x90000425UL
__attribute__((always_inline)) inline unsigned char cx_rng_u8_inline(void) {
  volatile unsigned int parameters[2];
#ifdef __clang_analyzer__
  parameters[1] = 0;
#endif
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_rng_u8_ID_IN)
                 : "r0", "r1");
  return (unsigned char)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_rng_ID_IN 0x6000052cUL
#define SYSCALL_cx_rng_ID_OUT 0x90000567UL
__attribute__((always_inline)) inline unsigned char *
cx_rng_inline(unsigned char *buffer, unsigned int len) {
  volatile unsigned int parameters[2 + 2];
  parameters[0] = (unsigned int)buffer;
  parameters[1] = (unsigned int)len;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_rng_ID_IN)
                 : "r0", "r1");
  return (unsigned char *)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_hash_ID_IN 0x6000073bUL
#define SYSCALL_cx_hash_ID_OUT 0x900007adUL
__attribute__((always_inline)) inline int
cx_hash_inline(cx_hash_t *hash, int mode, const unsigned char *in,
               unsigned int len, unsigned char *out, unsigned int out_len) {
  volatile unsigned int parameters[2 + 6];
  parameters[0] = (unsigned int)hash;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)in;
  parameters[3] = (unsigned int)len;
  parameters[4] = (unsigned int)out;
  parameters[5] = (unsigned int)out_len;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_hash_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_ripemd160_init_ID_IN 0x6000087fUL
#define SYSCALL_cx_ripemd160_init_ID_OUT 0x900008f8UL
__attribute__((always_inline)) inline int
cx_ripemd160_init_inline(cx_ripemd160_t *hash) {
  volatile unsigned int parameters[2 + 1];
#ifdef __clang_analyzer__
  parameters[1] = 0;
#endif
  parameters[0] = (unsigned int)hash;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_ripemd160_init_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_sha224_init_ID_IN 0x6000095bUL
#define SYSCALL_cx_sha224_init_ID_OUT 0x9000091dUL
__attribute__((always_inline)) inline int
cx_sha224_init_inline(cx_sha256_t *hash) {
  volatile unsigned int parameters[2 + 1];
#ifdef __clang_analyzer__
  parameters[1] = 0;
#endif
  parameters[0] = (unsigned int)hash;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_sha224_init_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_sha256_init_ID_IN 0x60000adbUL
#define SYSCALL_cx_sha256_init_ID_OUT 0x90000a64UL
__attribute__((always_inline)) inline int
cx_sha256_init_inline(cx_sha256_t *hash) {
  volatile unsigned int parameters[2 + 1];
#ifdef __clang_analyzer__
  parameters[1] = 0;
#endif
  parameters[0] = (unsigned int)hash;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_sha256_init_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_hash_sha256_ID_IN 0x60000b2cUL
#define SYSCALL_cx_hash_sha256_ID_OUT 0x90000ba0UL
__attribute__((always_inline)) inline int
cx_hash_sha256_inline(const unsigned char *in, unsigned int len,
                      unsigned char *out, unsigned int out_len) {
  volatile unsigned int parameters[2 + 4];
  parameters[0] = (unsigned int)in;
  parameters[1] = (unsigned int)len;
  parameters[2] = (unsigned int)out;
  parameters[3] = (unsigned int)out_len;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_hash_sha256_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_sha384_init_ID_IN 0x60000c2bUL
#define SYSCALL_cx_sha384_init_ID_OUT 0x90000cafUL
__attribute__((always_inline)) inline int
cx_sha384_init_inline(cx_sha512_t *hash) {
  volatile unsigned int parameters[2 + 1];
#ifdef __clang_analyzer__
  parameters[1] = 0;
#endif
  parameters[0] = (unsigned int)hash;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_sha384_init_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_sha512_init_ID_IN 0x60000dc1UL
#define SYSCALL_cx_sha512_init_ID_OUT 0x90000deeUL
__attribute__((always_inline)) inline int
cx_sha512_init_inline(cx_sha512_t *hash) {
  volatile unsigned int parameters[2 + 1];
#ifdef __clang_analyzer__
  parameters[1] = 0;
#endif
  parameters[0] = (unsigned int)hash;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_sha512_init_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_hash_sha512_ID_IN 0x60000e48UL
#define SYSCALL_cx_hash_sha512_ID_OUT 0x90000e55UL
__attribute__((always_inline)) inline int
cx_hash_sha512_inline(const unsigned char *in, unsigned int len,
                      unsigned char *out, unsigned int out_len) {
  volatile unsigned int parameters[2 + 4];
  parameters[0] = (unsigned int)in;
  parameters[1] = (unsigned int)len;
  parameters[2] = (unsigned int)out;
  parameters[3] = (unsigned int)out_len;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_hash_sha512_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_sha3_init_ID_IN 0x60000fd1UL
#define SYSCALL_cx_sha3_init_ID_OUT 0x90000f76UL
__attribute__((always_inline)) inline int
cx_sha3_init_inline(cx_sha3_t *hash, unsigned int size) {
  volatile unsigned int parameters[2 + 2];
  parameters[0] = (unsigned int)hash;
  parameters[1] = (unsigned int)size;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_sha3_init_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_keccak_init_ID_IN 0x600010cfUL
#define SYSCALL_cx_keccak_init_ID_OUT 0x900010d8UL
__attribute__((always_inline)) inline int
cx_keccak_init_inline(cx_sha3_t *hash, unsigned int size) {
  volatile unsigned int parameters[2 + 2];
  parameters[0] = (unsigned int)hash;
  parameters[1] = (unsigned int)size;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_keccak_init_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_shake128_init_ID_IN 0x60009309UL
#define SYSCALL_cx_shake128_init_ID_OUT 0x90009391UL
__attribute__((always_inline)) inline int
cx_shake128_init_inline(cx_sha3_t *hash, unsigned int out_size) {
  volatile unsigned int parameters[2 + 2];
  parameters[0] = (unsigned int)hash;
  parameters[1] = (unsigned int)out_size;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_shake128_init_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_shake256_init_ID_IN 0x600094e6UL
#define SYSCALL_cx_shake256_init_ID_OUT 0x900094a7UL
__attribute__((always_inline)) inline int
cx_shake256_init_inline(cx_sha3_t *hash, unsigned int out_size) {
  volatile unsigned int parameters[2 + 2];
  parameters[0] = (unsigned int)hash;
  parameters[1] = (unsigned int)out_size;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_shake256_init_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_sha3_xof_init_ID_IN 0x60001140UL
#define SYSCALL_cx_sha3_xof_init_ID_OUT 0x900011e0UL
__attribute__((always_inline)) inline int
cx_sha3_xof_init_inline(cx_sha3_t *hash, unsigned int size,
                        unsigned int out_length) {
  volatile unsigned int parameters[2 + 3];
  parameters[0] = (unsigned int)hash;
  parameters[1] = (unsigned int)size;
  parameters[2] = (unsigned int)out_length;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_sha3_xof_init_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_blake2b_init_ID_IN 0x60001308UL
#define SYSCALL_cx_blake2b_init_ID_OUT 0x90001399UL
__attribute__((always_inline)) inline int
cx_blake2b_init_inline(cx_blake2b_t *hash, unsigned int out_len) {
  volatile unsigned int parameters[2 + 2];
  parameters[0] = (unsigned int)hash;
  parameters[1] = (unsigned int)out_len;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_blake2b_init_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_blake2b_init2_ID_IN 0x60001466UL
#define SYSCALL_cx_blake2b_init2_ID_OUT 0x90001491UL
__attribute__((always_inline)) inline int
cx_blake2b_init2_inline(cx_blake2b_t *hash, unsigned int out_len,
                        unsigned char *salt, unsigned int salt_len,
                        unsigned char *perso, unsigned int perso_len) {
  volatile unsigned int parameters[2 + 6];
  parameters[0] = (unsigned int)hash;
  parameters[1] = (unsigned int)out_len;
  parameters[2] = (unsigned int)salt;
  parameters[3] = (unsigned int)salt_len;
  parameters[4] = (unsigned int)perso;
  parameters[5] = (unsigned int)perso_len;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_blake2b_init2_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_groestl_init_ID_IN 0x60001220UL
#define SYSCALL_cx_groestl_init_ID_OUT 0x90001243UL
__attribute__((always_inline)) inline int
cx_groestl_init_inline(cx_groestl_t *hash, unsigned int size) {
  volatile unsigned int parameters[2 + 2];
  parameters[0] = (unsigned int)hash;
  parameters[1] = (unsigned int)size;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_groestl_init_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_hmac_ripemd160_init_ID_IN 0x600015cfUL
#define SYSCALL_cx_hmac_ripemd160_init_ID_OUT 0x90001576UL
__attribute__((always_inline)) inline int
cx_hmac_ripemd160_init_inline(cx_hmac_ripemd160_t *hmac,
                              const unsigned char *key, unsigned int key_len) {
  volatile unsigned int parameters[2 + 3];
  parameters[0] = (unsigned int)hmac;
  parameters[1] = (unsigned int)key;
  parameters[2] = (unsigned int)key_len;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_hmac_ripemd160_init_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_hmac_sha256_init_ID_IN 0x600016d1UL
#define SYSCALL_cx_hmac_sha256_init_ID_OUT 0x90001607UL
__attribute__((always_inline)) inline int
cx_hmac_sha256_init_inline(cx_hmac_sha256_t *hmac, const unsigned char *key,
                           unsigned int key_len) {
  volatile unsigned int parameters[2 + 3];
  parameters[0] = (unsigned int)hmac;
  parameters[1] = (unsigned int)key;
  parameters[2] = (unsigned int)key_len;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_hmac_sha256_init_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_hmac_sha512_init_ID_IN 0x600017b3UL
#define SYSCALL_cx_hmac_sha512_init_ID_OUT 0x90001719UL
__attribute__((always_inline)) inline int
cx_hmac_sha512_init_inline(cx_hmac_sha512_t *hmac, const unsigned char *key,
                           unsigned int key_len) {
  volatile unsigned int parameters[2 + 3];
  parameters[0] = (unsigned int)hmac;
  parameters[1] = (unsigned int)key;
  parameters[2] = (unsigned int)key_len;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_hmac_sha512_init_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_hmac_ID_IN 0x600018d1UL
#define SYSCALL_cx_hmac_ID_OUT 0x900018d6UL
__attribute__((always_inline)) inline int
cx_hmac_inline(cx_hmac_t *hmac, int mode, const unsigned char *in,
               unsigned int len, unsigned char *mac, unsigned int mac_len) {
  volatile unsigned int parameters[2 + 6];
  parameters[0] = (unsigned int)hmac;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)in;
  parameters[3] = (unsigned int)len;
  parameters[4] = (unsigned int)mac;
  parameters[5] = (unsigned int)mac_len;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_hmac_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_hmac_sha512_ID_IN 0x600019cfUL
#define SYSCALL_cx_hmac_sha512_ID_OUT 0x9000197eUL
__attribute__((always_inline)) inline int
cx_hmac_sha512_inline(const unsigned char *key, unsigned int key_len,
                      const unsigned char *in, unsigned int len,
                      unsigned char *mac, unsigned int mac_len) {
  volatile unsigned int parameters[2 + 6];
  parameters[0] = (unsigned int)key;
  parameters[1] = (unsigned int)key_len;
  parameters[2] = (unsigned int)in;
  parameters[3] = (unsigned int)len;
  parameters[4] = (unsigned int)mac;
  parameters[5] = (unsigned int)mac_len;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_hmac_sha512_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_hmac_sha256_ID_IN 0x60001a2bUL
#define SYSCALL_cx_hmac_sha256_ID_OUT 0x90001ab4UL
__attribute__((always_inline)) inline int
cx_hmac_sha256_inline(const unsigned char *key, unsigned int key_len,
                      const unsigned char *in, unsigned int len,
                      unsigned char *mac, unsigned int mac_len) {
  volatile unsigned int parameters[2 + 6];
  parameters[0] = (unsigned int)key;
  parameters[1] = (unsigned int)key_len;
  parameters[2] = (unsigned int)in;
  parameters[3] = (unsigned int)len;
  parameters[4] = (unsigned int)mac;
  parameters[5] = (unsigned int)mac_len;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_hmac_sha256_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_pbkdf2_ID_IN 0x6000a22cUL
#define SYSCALL_cx_pbkdf2_ID_OUT 0x9000a255UL
__attribute__((always_inline)) inline void
cx_pbkdf2_inline(cx_md_t md_type, const unsigned char *password,
                 unsigned short passwordlen, unsigned char *salt,
                 unsigned short saltlen, unsigned int iterations,
                 unsigned char *out, unsigned int outLength) {
  volatile unsigned int parameters[2 + 8];
  parameters[0] = (unsigned int)md_type;
  parameters[1] = (unsigned int)password;
  parameters[2] = (unsigned int)passwordlen;
  parameters[3] = (unsigned int)salt;
  parameters[4] = (unsigned int)saltlen;
  parameters[5] = (unsigned int)iterations;
  parameters[6] = (unsigned int)out;
  parameters[7] = (unsigned int)outLength;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_pbkdf2_ID_IN)
                 : "r0", "r1");
}

#define SYSCALL_cx_des_init_key_ID_IN 0x60001c83UL
#define SYSCALL_cx_des_init_key_ID_OUT 0x90001c4dUL
__attribute__((always_inline)) inline int
cx_des_init_key_inline(const unsigned char *rawkey, unsigned int key_len,
                       cx_des_key_t *key) {
  volatile unsigned int parameters[2 + 3];
  parameters[0] = (unsigned int)rawkey;
  parameters[1] = (unsigned int)key_len;
  parameters[2] = (unsigned int)key;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_des_init_key_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_des_iv_ID_IN 0x6000a322UL
#define SYSCALL_cx_des_iv_ID_OUT 0x9000a347UL
__attribute__((always_inline)) inline int
cx_des_iv_inline(const cx_des_key_t *key, int mode, unsigned char *iv,
                 unsigned int iv_len, const unsigned char *in,
                 unsigned int in_len, unsigned char *out,
                 unsigned int out_len) {
  volatile unsigned int parameters[2 + 8];
  parameters[0] = (unsigned int)key;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)iv;
  parameters[3] = (unsigned int)iv_len;
  parameters[4] = (unsigned int)in;
  parameters[5] = (unsigned int)in_len;
  parameters[6] = (unsigned int)out;
  parameters[7] = (unsigned int)out_len;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_des_iv_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_des_ID_IN 0x60001e4eUL
#define SYSCALL_cx_des_ID_OUT 0x90001efeUL
__attribute__((always_inline)) inline int
cx_des_inline(const cx_des_key_t *key, int mode, const unsigned char *in,
              unsigned int in_len, unsigned char *out, unsigned int out_len) {
  volatile unsigned int parameters[2 + 6];
  parameters[0] = (unsigned int)key;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)in;
  parameters[3] = (unsigned int)in_len;
  parameters[4] = (unsigned int)out;
  parameters[5] = (unsigned int)out_len;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_des_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_aes_init_key_ID_IN 0x60001f2bUL
#define SYSCALL_cx_aes_init_key_ID_OUT 0x90001f31UL
__attribute__((always_inline)) inline int
cx_aes_init_key_inline(const unsigned char *rawkey, unsigned int key_len,
                       cx_aes_key_t *key) {
  volatile unsigned int parameters[2 + 3];
  parameters[0] = (unsigned int)rawkey;
  parameters[1] = (unsigned int)key_len;
  parameters[2] = (unsigned int)key;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_aes_init_key_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_aes_iv_ID_IN 0x6000a4a5UL
#define SYSCALL_cx_aes_iv_ID_OUT 0x9000a4e2UL
__attribute__((always_inline)) inline int
cx_aes_iv_inline(const cx_aes_key_t *key, int mode, unsigned char *iv,
                 unsigned int iv_len, const unsigned char *in,
                 unsigned int in_len, unsigned char *out,
                 unsigned int out_len) {
  volatile unsigned int parameters[2 + 8];
  parameters[0] = (unsigned int)key;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)iv;
  parameters[3] = (unsigned int)iv_len;
  parameters[4] = (unsigned int)in;
  parameters[5] = (unsigned int)in_len;
  parameters[6] = (unsigned int)out;
  parameters[7] = (unsigned int)out_len;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_aes_iv_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_aes_ID_IN 0x600021e2UL
#define SYSCALL_cx_aes_ID_OUT 0x9000213cUL
__attribute__((always_inline)) inline int
cx_aes_inline(const cx_aes_key_t *key, int mode, const unsigned char *in,
              unsigned int in_len, unsigned char *out, unsigned int out_len) {
  volatile unsigned int parameters[2 + 6];
  parameters[0] = (unsigned int)key;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)in;
  parameters[3] = (unsigned int)in_len;
  parameters[4] = (unsigned int)out;
  parameters[5] = (unsigned int)out_len;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_aes_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_rsa_init_public_key_ID_IN 0x600022e5UL
#define SYSCALL_cx_rsa_init_public_key_ID_OUT 0x9000228bUL
__attribute__((always_inline)) inline int cx_rsa_init_public_key_inline(
    const unsigned char *exponent, unsigned int exponent_len,
    const unsigned char *modulus, unsigned int modulus_len,
    cx_rsa_public_key_t *key) {
  volatile unsigned int parameters[2 + 5];
  parameters[0] = (unsigned int)exponent;
  parameters[1] = (unsigned int)exponent_len;
  parameters[2] = (unsigned int)modulus;
  parameters[3] = (unsigned int)modulus_len;
  parameters[4] = (unsigned int)key;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_rsa_init_public_key_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_rsa_init_private_key_ID_IN 0x60002337UL
#define SYSCALL_cx_rsa_init_private_key_ID_OUT 0x900023c3UL
__attribute__((always_inline)) inline int cx_rsa_init_private_key_inline(
    const unsigned char *exponent, unsigned int exponent_len,
    const unsigned char *modulus, unsigned int modulus_len,
    cx_rsa_private_key_t *key) {
  volatile unsigned int parameters[2 + 5];
  parameters[0] = (unsigned int)exponent;
  parameters[1] = (unsigned int)exponent_len;
  parameters[2] = (unsigned int)modulus;
  parameters[3] = (unsigned int)modulus_len;
  parameters[4] = (unsigned int)key;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_rsa_init_private_key_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_rsa_generate_pair_ID_IN 0x600024a1UL
#define SYSCALL_cx_rsa_generate_pair_ID_OUT 0x900024fdUL
__attribute__((always_inline)) inline int cx_rsa_generate_pair_inline(
    unsigned int modulus_len, cx_rsa_public_key_t *public_key,
    cx_rsa_private_key_t *private_key, const unsigned char *pub_exponent,
    unsigned int exponent_len, const unsigned char *externalPQ) {
  volatile unsigned int parameters[2 + 6];
  parameters[0] = (unsigned int)modulus_len;
  parameters[1] = (unsigned int)public_key;
  parameters[2] = (unsigned int)private_key;
  parameters[3] = (unsigned int)pub_exponent;
  parameters[4] = (unsigned int)exponent_len;
  parameters[5] = (unsigned int)externalPQ;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_rsa_generate_pair_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_rsa_sign_ID_IN 0x600025c6UL
#define SYSCALL_cx_rsa_sign_ID_OUT 0x900025f3UL
__attribute__((always_inline)) inline int
cx_rsa_sign_inline(const cx_rsa_private_key_t *key, int mode, cx_md_t hashID,
                   const unsigned char *hash, unsigned int hash_len,
                   unsigned char *sig, unsigned int sig_len) {
  volatile unsigned int parameters[2 + 7];
  parameters[0] = (unsigned int)key;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)hashID;
  parameters[3] = (unsigned int)hash;
  parameters[4] = (unsigned int)hash_len;
  parameters[5] = (unsigned int)sig;
  parameters[6] = (unsigned int)sig_len;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_rsa_sign_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_rsa_verify_ID_IN 0x600026abUL
#define SYSCALL_cx_rsa_verify_ID_OUT 0x900026b3UL
__attribute__((always_inline)) inline int
cx_rsa_verify_inline(const cx_rsa_public_key_t *key, int mode, cx_md_t hashID,
                     const unsigned char *hash, unsigned int hash_len,
                     const unsigned char *sig, unsigned int sig_len) {
  volatile unsigned int parameters[2 + 7];
  parameters[0] = (unsigned int)key;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)hashID;
  parameters[3] = (unsigned int)hash;
  parameters[4] = (unsigned int)hash_len;
  parameters[5] = (unsigned int)sig;
  parameters[6] = (unsigned int)sig_len;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_rsa_verify_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_rsa_encrypt_ID_IN 0x600027daUL
#define SYSCALL_cx_rsa_encrypt_ID_OUT 0x900027c9UL
__attribute__((always_inline)) inline int
cx_rsa_encrypt_inline(const cx_rsa_public_key_t *key, int mode, cx_md_t hashID,
                      const unsigned char *mesg, unsigned int mesg_len,
                      unsigned char *enc, unsigned int enc_len) {
  volatile unsigned int parameters[2 + 7];
  parameters[0] = (unsigned int)key;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)hashID;
  parameters[3] = (unsigned int)mesg;
  parameters[4] = (unsigned int)mesg_len;
  parameters[5] = (unsigned int)enc;
  parameters[6] = (unsigned int)enc_len;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_rsa_encrypt_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_rsa_decrypt_ID_IN 0x60002884UL
#define SYSCALL_cx_rsa_decrypt_ID_OUT 0x900028a7UL
__attribute__((always_inline)) inline int
cx_rsa_decrypt_inline(const cx_rsa_private_key_t *key, int mode, cx_md_t hashID,
                      const unsigned char *mesg, unsigned int mesg_len,
                      unsigned char *dec, unsigned int dec_len) {
  volatile unsigned int parameters[2 + 7];
  parameters[0] = (unsigned int)key;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)hashID;
  parameters[3] = (unsigned int)mesg;
  parameters[4] = (unsigned int)mesg_len;
  parameters[5] = (unsigned int)dec;
  parameters[6] = (unsigned int)dec_len;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_rsa_decrypt_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_ecfp_is_valid_point_ID_IN 0x6000296bUL
#define SYSCALL_cx_ecfp_is_valid_point_ID_OUT 0x90002901UL
__attribute__((always_inline)) inline int
cx_ecfp_is_valid_point_inline(cx_curve_t curve, const unsigned char *P,
                              unsigned int P_len) {
  volatile unsigned int parameters[2 + 3];
  parameters[0] = (unsigned int)curve;
  parameters[1] = (unsigned int)P;
  parameters[2] = (unsigned int)P_len;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_ecfp_is_valid_point_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_ecfp_is_cryptographic_point_ID_IN 0x60002ae3UL
#define SYSCALL_cx_ecfp_is_cryptographic_point_ID_OUT 0x90002af0UL
__attribute__((always_inline)) inline int
cx_ecfp_is_cryptographic_point_inline(cx_curve_t curve, const unsigned char *P,
                                      unsigned int P_len) {
  volatile unsigned int parameters[2 + 3];
  parameters[0] = (unsigned int)curve;
  parameters[1] = (unsigned int)P;
  parameters[2] = (unsigned int)P_len;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_ecfp_is_cryptographic_point_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_ecfp_add_point_ID_IN 0x60002b17UL
#define SYSCALL_cx_ecfp_add_point_ID_OUT 0x90002bc7UL
__attribute__((always_inline)) inline int
cx_ecfp_add_point_inline(cx_curve_t curve, unsigned char *R,
                         const unsigned char *P, const unsigned char *Q,
                         unsigned int X_len) {
  volatile unsigned int parameters[2 + 5];
  parameters[0] = (unsigned int)curve;
  parameters[1] = (unsigned int)R;
  parameters[2] = (unsigned int)P;
  parameters[3] = (unsigned int)Q;
  parameters[4] = (unsigned int)X_len;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_ecfp_add_point_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_ecfp_scalar_mult_ID_IN 0x60002cf3UL
#define SYSCALL_cx_ecfp_scalar_mult_ID_OUT 0x90002ce3UL
__attribute__((always_inline)) inline int
cx_ecfp_scalar_mult_inline(cx_curve_t curve, unsigned char *P,
                           unsigned int P_len, const unsigned char *k,
                           unsigned int k_len) {
  volatile unsigned int parameters[2 + 5];
  parameters[0] = (unsigned int)curve;
  parameters[1] = (unsigned int)P;
  parameters[2] = (unsigned int)P_len;
  parameters[3] = (unsigned int)k;
  parameters[4] = (unsigned int)k_len;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_ecfp_scalar_mult_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_ecfp_init_public_key_ID_IN 0x60002dedUL
#define SYSCALL_cx_ecfp_init_public_key_ID_OUT 0x90002d49UL
__attribute__((always_inline)) inline int
cx_ecfp_init_public_key_inline(cx_curve_t curve, const unsigned char *rawkey,
                               unsigned int key_len,
                               cx_ecfp_public_key_t *key) {
  volatile unsigned int parameters[2 + 4];
  parameters[0] = (unsigned int)curve;
  parameters[1] = (unsigned int)rawkey;
  parameters[2] = (unsigned int)key_len;
  parameters[3] = (unsigned int)key;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_ecfp_init_public_key_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_ecfp_init_private_key_ID_IN 0x60002eeaUL
#define SYSCALL_cx_ecfp_init_private_key_ID_OUT 0x90002e63UL
__attribute__((always_inline)) inline int
cx_ecfp_init_private_key_inline(cx_curve_t curve, const unsigned char *rawkey,
                                unsigned int key_len,
                                cx_ecfp_private_key_t *pvkey) {
  volatile unsigned int parameters[2 + 4];
  parameters[0] = (unsigned int)curve;
  parameters[1] = (unsigned int)rawkey;
  parameters[2] = (unsigned int)key_len;
  parameters[3] = (unsigned int)pvkey;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_ecfp_init_private_key_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_ecfp_generate_pair_ID_IN 0x60002f2eUL
#define SYSCALL_cx_ecfp_generate_pair_ID_OUT 0x90002f74UL
__attribute__((always_inline)) inline int
cx_ecfp_generate_pair_inline(cx_curve_t curve, cx_ecfp_public_key_t *pubkey,
                             cx_ecfp_private_key_t *privkey, int keepprivate) {
  volatile unsigned int parameters[2 + 4];
  parameters[0] = (unsigned int)curve;
  parameters[1] = (unsigned int)pubkey;
  parameters[2] = (unsigned int)privkey;
  parameters[3] = (unsigned int)keepprivate;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_ecfp_generate_pair_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_ecfp_generate_pair2_ID_IN 0x6000301fUL
#define SYSCALL_cx_ecfp_generate_pair2_ID_OUT 0x900030e6UL
__attribute__((always_inline)) inline int
cx_ecfp_generate_pair2_inline(cx_curve_t curve, cx_ecfp_public_key_t *pubkey,
                              cx_ecfp_private_key_t *privkey, int keepprivate,
                              cx_md_t hashID) {
  volatile unsigned int parameters[2 + 5];
  parameters[0] = (unsigned int)curve;
  parameters[1] = (unsigned int)pubkey;
  parameters[2] = (unsigned int)privkey;
  parameters[3] = (unsigned int)keepprivate;
  parameters[4] = (unsigned int)hashID;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_ecfp_generate_pair2_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_ecdsa_sign_ID_IN 0x600038f3UL
#define SYSCALL_cx_ecdsa_sign_ID_OUT 0x90003876UL
__attribute__((always_inline)) inline int
cx_ecdsa_sign_inline(const cx_ecfp_private_key_t *pvkey, int mode,
                     cx_md_t hashID, const unsigned char *hash,
                     unsigned int hash_len, unsigned char *sig,
                     unsigned int sig_len, unsigned int *info) {
  volatile unsigned int parameters[2 + 8];
  parameters[0] = (unsigned int)pvkey;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)hashID;
  parameters[3] = (unsigned int)hash;
  parameters[4] = (unsigned int)hash_len;
  parameters[5] = (unsigned int)sig;
  parameters[6] = (unsigned int)sig_len;
  parameters[7] = (unsigned int)info;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_ecdsa_sign_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_ecdsa_verify_ID_IN 0x600039f1UL
#define SYSCALL_cx_ecdsa_verify_ID_OUT 0x900039e7UL
__attribute__((always_inline)) inline int
cx_ecdsa_verify_inline(const cx_ecfp_public_key_t *pukey, int mode,
                       cx_md_t hashID, const unsigned char *hash,
                       unsigned int hash_len, const unsigned char *sig,
                       unsigned int sig_len) {
  volatile unsigned int parameters[2 + 7];
  parameters[0] = (unsigned int)pukey;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)hashID;
  parameters[3] = (unsigned int)hash;
  parameters[4] = (unsigned int)hash_len;
  parameters[5] = (unsigned int)sig;
  parameters[6] = (unsigned int)sig_len;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_ecdsa_verify_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_edward_compress_point_ID_IN 0x60003359UL
#define SYSCALL_cx_edward_compress_point_ID_OUT 0x9000332bUL
__attribute__((always_inline)) inline void
cx_edward_compress_point_inline(cx_curve_t curve, unsigned char *P,
                                unsigned int P_len) {
  volatile unsigned int parameters[2 + 3];
  parameters[0] = (unsigned int)curve;
  parameters[1] = (unsigned int)P;
  parameters[2] = (unsigned int)P_len;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_edward_compress_point_ID_IN)
                 : "r0", "r1");
}

#define SYSCALL_cx_edward_decompress_point_ID_IN 0x60003431UL
#define SYSCALL_cx_edward_decompress_point_ID_OUT 0x900034caUL
__attribute__((always_inline)) inline void
cx_edward_decompress_point_inline(cx_curve_t curve, unsigned char *P,
                                  unsigned int P_len) {
  volatile unsigned int parameters[2 + 3];
  parameters[0] = (unsigned int)curve;
  parameters[1] = (unsigned int)P;
  parameters[2] = (unsigned int)P_len;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_edward_decompress_point_ID_IN)
                 : "r0", "r1");
}

#define SYSCALL_cx_eddsa_get_public_key_ID_IN 0x6000351cUL
#define SYSCALL_cx_eddsa_get_public_key_ID_OUT 0x900035bfUL
__attribute__((always_inline)) inline void
cx_eddsa_get_public_key_inline(const cx_ecfp_private_key_t *pvkey,
                               cx_md_t hashID, cx_ecfp_public_key_t *pukey,
                               unsigned char *a, unsigned int a_len,
                               unsigned char *h, unsigned int h_len) {
  volatile unsigned int parameters[2 + 7];
  parameters[0] = (unsigned int)pvkey;
  parameters[1] = (unsigned int)hashID;
  parameters[2] = (unsigned int)pukey;
  parameters[3] = (unsigned int)a;
  parameters[4] = (unsigned int)a_len;
  parameters[5] = (unsigned int)h;
  parameters[6] = (unsigned int)h_len;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_eddsa_get_public_key_ID_IN)
                 : "r0", "r1");
}

#define SYSCALL_cx_eddsa_sign_ID_IN 0x6000363bUL
#define SYSCALL_cx_eddsa_sign_ID_OUT 0x900036f6UL
__attribute__((always_inline)) inline int
cx_eddsa_sign_inline(const cx_ecfp_private_key_t *pvkey, int mode,
                     cx_md_t hashID, const unsigned char *hash,
                     unsigned int hash_len, const unsigned char *ctx,
                     unsigned int ctx_len, unsigned char *sig,
                     unsigned int sig_len, unsigned int *info) {
  volatile unsigned int parameters[2 + 10];
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
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_eddsa_sign_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_eddsa_verify_ID_IN 0x600037caUL
#define SYSCALL_cx_eddsa_verify_ID_OUT 0x90003721UL
__attribute__((always_inline)) inline int cx_eddsa_verify_inline(
    const cx_ecfp_public_key_t *pukey, int mode, cx_md_t hashID,
    const unsigned char *hash, unsigned int hash_len, const unsigned char *ctx,
    unsigned int ctx_len, const unsigned char *sig, unsigned int sig_len) {
  volatile unsigned int parameters[2 + 9];
  parameters[0] = (unsigned int)pukey;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)hashID;
  parameters[3] = (unsigned int)hash;
  parameters[4] = (unsigned int)hash_len;
  parameters[5] = (unsigned int)ctx;
  parameters[6] = (unsigned int)ctx_len;
  parameters[7] = (unsigned int)sig;
  parameters[8] = (unsigned int)sig_len;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_eddsa_verify_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_ecschnorr_sign_ID_IN 0x60003141UL
#define SYSCALL_cx_ecschnorr_sign_ID_OUT 0x9000314aUL
__attribute__((always_inline)) inline int
cx_ecschnorr_sign_inline(const cx_ecfp_private_key_t *pvkey, int mode,
                         cx_md_t hashID, const unsigned char *msg,
                         unsigned int msg_len, unsigned char *sig,
                         unsigned int sig_len, unsigned int *info) {
  volatile unsigned int parameters[2 + 8];
  parameters[0] = (unsigned int)pvkey;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)hashID;
  parameters[3] = (unsigned int)msg;
  parameters[4] = (unsigned int)msg_len;
  parameters[5] = (unsigned int)sig;
  parameters[6] = (unsigned int)sig_len;
  parameters[7] = (unsigned int)info;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_ecschnorr_sign_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_ecschnorr_verify_ID_IN 0x60003205UL
#define SYSCALL_cx_ecschnorr_verify_ID_OUT 0x900032f7UL
__attribute__((always_inline)) inline int
cx_ecschnorr_verify_inline(const cx_ecfp_public_key_t *pukey, int mode,
                           cx_md_t hashID, const unsigned char *msg,
                           unsigned int msg_len, const unsigned char *sig,
                           unsigned int sig_len) {
  volatile unsigned int parameters[2 + 7];
  parameters[0] = (unsigned int)pukey;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)hashID;
  parameters[3] = (unsigned int)msg;
  parameters[4] = (unsigned int)msg_len;
  parameters[5] = (unsigned int)sig;
  parameters[6] = (unsigned int)sig_len;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_ecschnorr_verify_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_ecdh_ID_IN 0x60003a9dUL
#define SYSCALL_cx_ecdh_ID_OUT 0x90003a94UL
__attribute__((always_inline)) inline int
cx_ecdh_inline(const cx_ecfp_private_key_t *pvkey, int mode,
               const unsigned char *P, unsigned int P_len,
               unsigned char *secret, unsigned int secret_len) {
  volatile unsigned int parameters[2 + 6];
  parameters[0] = (unsigned int)pvkey;
  parameters[1] = (unsigned int)mode;
  parameters[2] = (unsigned int)P;
  parameters[3] = (unsigned int)P_len;
  parameters[4] = (unsigned int)secret;
  parameters[5] = (unsigned int)secret_len;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_ecdh_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_crc16_ID_IN 0x600091f8UL
#define SYSCALL_cx_crc16_ID_OUT 0x9000912cUL
__attribute__((always_inline)) inline unsigned short
cx_crc16_inline(const void *buffer, size_t len) {
  volatile unsigned int parameters[2 + 2];
  parameters[0] = (unsigned int)buffer;
  parameters[1] = (unsigned int)len;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_crc16_ID_IN)
                 : "r0", "r1");
  return (unsigned short)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_crc16_update_ID_IN 0x6000926eUL
#define SYSCALL_cx_crc16_update_ID_OUT 0x900092ebUL
__attribute__((always_inline)) inline unsigned short
cx_crc16_update_inline(unsigned short crc, const void *buffer, size_t len) {
  volatile unsigned int parameters[2 + 3];
  parameters[0] = (unsigned int)crc;
  parameters[1] = (unsigned int)buffer;
  parameters[2] = (unsigned int)len;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_crc16_update_ID_IN)
                 : "r0", "r1");
  return (unsigned short)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_math_cmp_ID_IN 0x60003d5bUL
#define SYSCALL_cx_math_cmp_ID_OUT 0x90003dbcUL
__attribute__((always_inline)) inline int
cx_math_cmp_inline(const unsigned char *a, const unsigned char *b,
                   unsigned int len) {
  volatile unsigned int parameters[2 + 3];
  parameters[0] = (unsigned int)a;
  parameters[1] = (unsigned int)b;
  parameters[2] = (unsigned int)len;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_math_cmp_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_math_is_zero_ID_IN 0x60003e37UL
#define SYSCALL_cx_math_is_zero_ID_OUT 0x90003e50UL
__attribute__((always_inline)) inline int
cx_math_is_zero_inline(const unsigned char *a, unsigned int len) {
  volatile unsigned int parameters[2 + 2];
  parameters[0] = (unsigned int)a;
  parameters[1] = (unsigned int)len;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_math_is_zero_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_math_add_ID_IN 0x60003ffbUL
#define SYSCALL_cx_math_add_ID_OUT 0x90003fd8UL
__attribute__((always_inline)) inline int
cx_math_add_inline(unsigned char *r, const unsigned char *a,
                   const unsigned char *b, unsigned int len) {
  volatile unsigned int parameters[2 + 4];
  parameters[0] = (unsigned int)r;
  parameters[1] = (unsigned int)a;
  parameters[2] = (unsigned int)b;
  parameters[3] = (unsigned int)len;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_math_add_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_math_sub_ID_IN 0x6000409fUL
#define SYSCALL_cx_math_sub_ID_OUT 0x9000401dUL
__attribute__((always_inline)) inline int
cx_math_sub_inline(unsigned char *r, const unsigned char *a,
                   const unsigned char *b, unsigned int len) {
  volatile unsigned int parameters[2 + 4];
  parameters[0] = (unsigned int)r;
  parameters[1] = (unsigned int)a;
  parameters[2] = (unsigned int)b;
  parameters[3] = (unsigned int)len;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_math_sub_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_math_mult_ID_IN 0x6000413eUL
#define SYSCALL_cx_math_mult_ID_OUT 0x900041c2UL
__attribute__((always_inline)) inline void
cx_math_mult_inline(unsigned char *r, const unsigned char *a,
                    const unsigned char *b, unsigned int len) {
  volatile unsigned int parameters[2 + 4];
  parameters[0] = (unsigned int)r;
  parameters[1] = (unsigned int)a;
  parameters[2] = (unsigned int)b;
  parameters[3] = (unsigned int)len;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_math_mult_ID_IN)
                 : "r0", "r1");
}

#define SYSCALL_cx_math_addm_ID_IN 0x600042a6UL
#define SYSCALL_cx_math_addm_ID_OUT 0x90004248UL
__attribute__((always_inline)) inline void
cx_math_addm_inline(unsigned char *r, const unsigned char *a,
                    const unsigned char *b, const unsigned char *m,
                    unsigned int len) {
  volatile unsigned int parameters[2 + 5];
  parameters[0] = (unsigned int)r;
  parameters[1] = (unsigned int)a;
  parameters[2] = (unsigned int)b;
  parameters[3] = (unsigned int)m;
  parameters[4] = (unsigned int)len;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_math_addm_ID_IN)
                 : "r0", "r1");
}

#define SYSCALL_cx_math_subm_ID_IN 0x6000437dUL
#define SYSCALL_cx_math_subm_ID_OUT 0x900043e0UL
__attribute__((always_inline)) inline void
cx_math_subm_inline(unsigned char *r, const unsigned char *a,
                    const unsigned char *b, const unsigned char *m,
                    unsigned int len) {
  volatile unsigned int parameters[2 + 5];
  parameters[0] = (unsigned int)r;
  parameters[1] = (unsigned int)a;
  parameters[2] = (unsigned int)b;
  parameters[3] = (unsigned int)m;
  parameters[4] = (unsigned int)len;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_math_subm_ID_IN)
                 : "r0", "r1");
}

#define SYSCALL_cx_math_multm_ID_IN 0x60004445UL
#define SYSCALL_cx_math_multm_ID_OUT 0x900044f3UL
__attribute__((always_inline)) inline void
cx_math_multm_inline(unsigned char *r, const unsigned char *a,
                     const unsigned char *b, const unsigned char *m,
                     unsigned int len) {
  volatile unsigned int parameters[2 + 5];
  parameters[0] = (unsigned int)r;
  parameters[1] = (unsigned int)a;
  parameters[2] = (unsigned int)b;
  parameters[3] = (unsigned int)m;
  parameters[4] = (unsigned int)len;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_math_multm_ID_IN)
                 : "r0", "r1");
}

#define SYSCALL_cx_math_powm_ID_IN 0x6000454dUL
#define SYSCALL_cx_math_powm_ID_OUT 0x9000453eUL
__attribute__((always_inline)) inline void
cx_math_powm_inline(unsigned char *r, const unsigned char *a,
                    const unsigned char *e, unsigned int len_e,
                    const unsigned char *m, unsigned int len) {
  volatile unsigned int parameters[2 + 6];
  parameters[0] = (unsigned int)r;
  parameters[1] = (unsigned int)a;
  parameters[2] = (unsigned int)e;
  parameters[3] = (unsigned int)len_e;
  parameters[4] = (unsigned int)m;
  parameters[5] = (unsigned int)len;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_math_powm_ID_IN)
                 : "r0", "r1");
}

#define SYSCALL_cx_math_modm_ID_IN 0x60004645UL
#define SYSCALL_cx_math_modm_ID_OUT 0x9000468cUL
__attribute__((always_inline)) inline void
cx_math_modm_inline(unsigned char *v, unsigned int len_v,
                    const unsigned char *m, unsigned int len_m) {
  volatile unsigned int parameters[2 + 4];
  parameters[0] = (unsigned int)v;
  parameters[1] = (unsigned int)len_v;
  parameters[2] = (unsigned int)m;
  parameters[3] = (unsigned int)len_m;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_math_modm_ID_IN)
                 : "r0", "r1");
}

#define SYSCALL_cx_math_invprimem_ID_IN 0x600047e9UL
#define SYSCALL_cx_math_invprimem_ID_OUT 0x90004719UL
__attribute__((always_inline)) inline void
cx_math_invprimem_inline(unsigned char *r, const unsigned char *a,
                         const unsigned char *m, unsigned int len) {
  volatile unsigned int parameters[2 + 4];
  parameters[0] = (unsigned int)r;
  parameters[1] = (unsigned int)a;
  parameters[2] = (unsigned int)m;
  parameters[3] = (unsigned int)len;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_math_invprimem_ID_IN)
                 : "r0", "r1");
}

#define SYSCALL_cx_math_invintm_ID_IN 0x6000482cUL
#define SYSCALL_cx_math_invintm_ID_OUT 0x90004891UL
__attribute__((always_inline)) inline void
cx_math_invintm_inline(unsigned char *r, unsigned long int a,
                       const unsigned char *m, unsigned int len) {
  volatile unsigned int parameters[2 + 4];
  parameters[0] = (unsigned int)r;
  parameters[1] = (unsigned int)a;
  parameters[2] = (unsigned int)m;
  parameters[3] = (unsigned int)len;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_math_invintm_ID_IN)
                 : "r0", "r1");
}

#define SYSCALL_cx_math_is_prime_ID_IN 0x60004948UL
#define SYSCALL_cx_math_is_prime_ID_OUT 0x900049faUL
__attribute__((always_inline)) inline int
cx_math_is_prime_inline(const unsigned char *p, unsigned int len) {
  volatile unsigned int parameters[2 + 2];
  parameters[0] = (unsigned int)p;
  parameters[1] = (unsigned int)len;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_math_is_prime_ID_IN)
                 : "r0", "r1");
  return (int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_math_next_prime_ID_IN 0x60004aa4UL
#define SYSCALL_cx_math_next_prime_ID_OUT 0x90004acbUL
__attribute__((always_inline)) inline void
cx_math_next_prime_inline(unsigned char *n, unsigned int len) {
  volatile unsigned int parameters[2 + 2];
  parameters[0] = (unsigned int)n;
  parameters[1] = (unsigned int)len;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_math_next_prime_ID_IN)
                 : "r0", "r1");
}

#define SYSCALL_os_perso_erase_all_ID_IN 0x60004bf5UL
#define SYSCALL_os_perso_erase_all_ID_OUT 0x90004bf4UL
__attribute__((always_inline)) inline void os_perso_erase_all_inline(void) {
  volatile unsigned int parameters[2];
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_perso_erase_all_ID_IN)
                 : "r0", "r1");
}

#define SYSCALL_os_perso_set_pin_ID_IN 0x60004cdfUL
#define SYSCALL_os_perso_set_pin_ID_OUT 0x90004c95UL
__attribute__((always_inline)) inline void
os_perso_set_pin_inline(unsigned int identity, unsigned char *pin,
                        unsigned int length) {
  volatile unsigned int parameters[2 + 3];
  parameters[0] = (unsigned int)identity;
  parameters[1] = (unsigned int)pin;
  parameters[2] = (unsigned int)length;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_perso_set_pin_ID_IN)
                 : "r0", "r1");
}

#define SYSCALL_os_perso_set_current_identity_pin_ID_IN 0x60004dfeUL
#define SYSCALL_os_perso_set_current_identity_pin_ID_OUT 0x90004d0aUL
__attribute__((always_inline)) inline void
os_perso_set_current_identity_pin_inline(unsigned char *pin,
                                         unsigned int length) {
  volatile unsigned int parameters[2 + 2];
  parameters[0] = (unsigned int)pin;
  parameters[1] = (unsigned int)length;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_perso_set_current_identity_pin_ID_IN)
                 : "r0", "r1");
}

#define SYSCALL_os_perso_set_seed_ID_IN 0x60004ebcUL
#define SYSCALL_os_perso_set_seed_ID_OUT 0x90004eeaUL
__attribute__((always_inline)) inline void
os_perso_set_seed_inline(unsigned int identity, unsigned int algorithm,
                         unsigned char *seed, unsigned int length) {
  volatile unsigned int parameters[2 + 4];
  parameters[0] = (unsigned int)identity;
  parameters[1] = (unsigned int)algorithm;
  parameters[2] = (unsigned int)seed;
  parameters[3] = (unsigned int)length;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_perso_set_seed_ID_IN)
                 : "r0", "r1");
}

#define SYSCALL_os_perso_derive_and_set_seed_ID_IN 0x60004fbdUL
#define SYSCALL_os_perso_derive_and_set_seed_ID_OUT 0x90004fcfUL
__attribute__((always_inline)) inline void os_perso_derive_and_set_seed_inline(
    unsigned char identity, const char *prefix, unsigned int prefix_length,
    const char *passphrase, unsigned int passphrase_length, const char *words,
    unsigned int words_length) {
  volatile unsigned int parameters[2 + 7];
  parameters[0] = (unsigned int)identity;
  parameters[1] = (unsigned int)prefix;
  parameters[2] = (unsigned int)prefix_length;
  parameters[3] = (unsigned int)passphrase;
  parameters[4] = (unsigned int)passphrase_length;
  parameters[5] = (unsigned int)words;
  parameters[6] = (unsigned int)words_length;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_perso_derive_and_set_seed_ID_IN)
                 : "r0", "r1");
}

#define SYSCALL_os_perso_set_words_ID_IN 0x60005018UL
#define SYSCALL_os_perso_set_words_ID_OUT 0x900050eaUL
__attribute__((always_inline)) inline void
os_perso_set_words_inline(const unsigned char *words, unsigned int length) {
  volatile unsigned int parameters[2 + 2];
  parameters[0] = (unsigned int)words;
  parameters[1] = (unsigned int)length;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_perso_set_words_ID_IN)
                 : "r0", "r1");
}

#define SYSCALL_os_perso_finalize_ID_IN 0x60005180UL
#define SYSCALL_os_perso_finalize_ID_OUT 0x90005154UL
__attribute__((always_inline)) inline void os_perso_finalize_inline(void) {
  volatile unsigned int parameters[2];
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_perso_finalize_ID_IN)
                 : "r0", "r1");
}

#define SYSCALL_os_perso_isonboarded_ID_IN 0x60009f4fUL
#define SYSCALL_os_perso_isonboarded_ID_OUT 0x90009f12UL
__attribute__((always_inline)) inline bolos_bool_t
os_perso_isonboarded_inline(void) {
  volatile unsigned int parameters[2];
#ifdef __clang_analyzer__
  parameters[1] = 0;
#endif
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_perso_isonboarded_ID_IN)
                 : "r0", "r1");
  return (bolos_bool_t)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_os_perso_derive_node_bip32_ID_IN 0x600053baUL
#define SYSCALL_os_perso_derive_node_bip32_ID_OUT 0x9000531eUL
__attribute__((always_inline)) inline void os_perso_derive_node_bip32_inline(
    cx_curve_t curve, const unsigned int *path, unsigned int pathLength,
    unsigned char *privateKey, unsigned char *chain) {
  volatile unsigned int parameters[2 + 5];
  parameters[0] = (unsigned int)curve;
  parameters[1] = (unsigned int)path;
  parameters[2] = (unsigned int)pathLength;
  parameters[3] = (unsigned int)privateKey;
  parameters[4] = (unsigned int)chain;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_perso_derive_node_bip32_ID_IN)
                 : "r0", "r1");
}

#define SYSCALL_os_perso_derive_node_with_seed_key_ID_IN 0x6000a6d8UL
#define SYSCALL_os_perso_derive_node_with_seed_key_ID_OUT 0x9000a604UL
__attribute__((always_inline)) inline void
os_perso_derive_node_with_seed_key_inline(
    unsigned int mode, cx_curve_t curve, const unsigned int *path,
    unsigned int pathLength, unsigned char *privateKey, unsigned char *chain,
    unsigned char *seed_key, unsigned int seed_key_length) {
  volatile unsigned int parameters[2 + 8];
  parameters[0] = (unsigned int)mode;
  parameters[1] = (unsigned int)curve;
  parameters[2] = (unsigned int)path;
  parameters[3] = (unsigned int)pathLength;
  parameters[4] = (unsigned int)privateKey;
  parameters[5] = (unsigned int)chain;
  parameters[6] = (unsigned int)seed_key;
  parameters[7] = (unsigned int)seed_key_length;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_perso_derive_node_with_seed_key_ID_IN)
                 : "r0", "r1");
}

#define SYSCALL_os_perso_seed_cookie_ID_IN 0x6000a8fcUL
#define SYSCALL_os_perso_seed_cookie_ID_OUT 0x9000a877UL
__attribute__((always_inline)) inline unsigned int
os_perso_seed_cookie_inline(unsigned char *seed_cookie,
                            unsigned int seed_cookie_length) {
  volatile unsigned int parameters[2 + 2];
  parameters[0] = (unsigned int)seed_cookie;
  parameters[1] = (unsigned int)seed_cookie_length;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_perso_seed_cookie_ID_IN)
                 : "r0", "r1");
  return (unsigned int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_os_endorsement_get_code_hash_ID_IN 0x6000550fUL
#define SYSCALL_os_endorsement_get_code_hash_ID_OUT 0x900055a1UL
__attribute__((always_inline)) inline unsigned int
os_endorsement_get_code_hash_inline(unsigned char *buffer) {
  volatile unsigned int parameters[2 + 1];
#ifdef __clang_analyzer__
  parameters[1] = 0;
#endif
  parameters[0] = (unsigned int)buffer;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_endorsement_get_code_hash_ID_IN)
                 : "r0", "r1");
  return (unsigned int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_os_endorsement_get_public_key_ID_IN 0x600056f3UL
#define SYSCALL_os_endorsement_get_public_key_ID_OUT 0x90005699UL
__attribute__((always_inline)) inline unsigned int
os_endorsement_get_public_key_inline(unsigned char index,
                                     unsigned char *buffer) {
  volatile unsigned int parameters[2 + 2];
  parameters[0] = (unsigned int)index;
  parameters[1] = (unsigned int)buffer;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_endorsement_get_public_key_ID_IN)
                 : "r0", "r1");
  return (unsigned int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_os_endorsement_get_public_key_certificate_ID_IN 0x6000574cUL
#define SYSCALL_os_endorsement_get_public_key_certificate_ID_OUT 0x9000577fUL
__attribute__((always_inline)) inline unsigned int
os_endorsement_get_public_key_certificate_inline(unsigned char index,
                                                 unsigned char *buffer) {
  volatile unsigned int parameters[2 + 2];
  parameters[0] = (unsigned int)index;
  parameters[1] = (unsigned int)buffer;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_endorsement_get_public_key_certificate_ID_IN)
                 : "r0", "r1");
  return (unsigned int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_os_endorsement_key1_get_app_secret_ID_IN 0x6000585cUL
#define SYSCALL_os_endorsement_key1_get_app_secret_ID_OUT 0x90005860UL
__attribute__((always_inline)) inline unsigned int
os_endorsement_key1_get_app_secret_inline(unsigned char *buffer) {
  volatile unsigned int parameters[2 + 1];
#ifdef __clang_analyzer__
  parameters[1] = 0;
#endif
  parameters[0] = (unsigned int)buffer;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_endorsement_key1_get_app_secret_ID_IN)
                 : "r0", "r1");
  return (unsigned int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_os_endorsement_key1_sign_data_ID_IN 0x600059d8UL
#define SYSCALL_os_endorsement_key1_sign_data_ID_OUT 0x9000592bUL
__attribute__((always_inline)) inline unsigned int
os_endorsement_key1_sign_data_inline(unsigned char *src, unsigned int srcLength,
                                     unsigned char *signature) {
  volatile unsigned int parameters[2 + 3];
  parameters[0] = (unsigned int)src;
  parameters[1] = (unsigned int)srcLength;
  parameters[2] = (unsigned int)signature;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_endorsement_key1_sign_data_ID_IN)
                 : "r0", "r1");
  return (unsigned int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_os_endorsement_key2_derive_sign_data_ID_IN 0x60005a4aUL
#define SYSCALL_os_endorsement_key2_derive_sign_data_ID_OUT 0x90005a3eUL
__attribute__((always_inline)) inline unsigned int
os_endorsement_key2_derive_sign_data_inline(unsigned char *src,
                                            unsigned int srcLength,
                                            unsigned char *signature) {
  volatile unsigned int parameters[2 + 3];
  parameters[0] = (unsigned int)src;
  parameters[1] = (unsigned int)srcLength;
  parameters[2] = (unsigned int)signature;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_endorsement_key2_derive_sign_data_ID_IN)
                 : "r0", "r1");
  return (unsigned int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_os_global_pin_is_validated_ID_IN 0x6000a03cUL
#define SYSCALL_os_global_pin_is_validated_ID_OUT 0x9000a078UL
__attribute__((always_inline)) inline bolos_bool_t
os_global_pin_is_validated_inline(void) {
  volatile unsigned int parameters[2];
#ifdef __clang_analyzer__
  parameters[1] = 0;
#endif
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_global_pin_is_validated_ID_IN)
                 : "r0", "r1");
  return (bolos_bool_t)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_os_global_pin_check_ID_IN 0x6000a102UL
#define SYSCALL_os_global_pin_check_ID_OUT 0x9000a1d3UL
__attribute__((always_inline)) inline bolos_bool_t
os_global_pin_check_inline(unsigned char *pin_buffer,
                           unsigned char pin_length) {
  volatile unsigned int parameters[2 + 2];
  parameters[0] = (unsigned int)pin_buffer;
  parameters[1] = (unsigned int)pin_length;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_global_pin_check_ID_IN)
                 : "r0", "r1");
  return (bolos_bool_t)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_os_global_pin_invalidate_ID_IN 0x60005dd0UL
#define SYSCALL_os_global_pin_invalidate_ID_OUT 0x90005dfbUL
__attribute__((always_inline)) inline void
os_global_pin_invalidate_inline(void) {
  volatile unsigned int parameters[2];
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_global_pin_invalidate_ID_IN)
                 : "r0", "r1");
}

#define SYSCALL_os_global_pin_retries_ID_IN 0x60005e59UL
#define SYSCALL_os_global_pin_retries_ID_OUT 0x90005e18UL
__attribute__((always_inline)) inline unsigned int
os_global_pin_retries_inline(void) {
  volatile unsigned int parameters[2];
#ifdef __clang_analyzer__
  parameters[1] = 0;
#endif
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_global_pin_retries_ID_IN)
                 : "r0", "r1");
  return (unsigned int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_os_registry_count_ID_IN 0x60005f40UL
#define SYSCALL_os_registry_count_ID_OUT 0x90005f06UL
__attribute__((always_inline)) inline unsigned int
os_registry_count_inline(void) {
  volatile unsigned int parameters[2];
#ifdef __clang_analyzer__
  parameters[1] = 0;
#endif
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_registry_count_ID_IN)
                 : "r0", "r1");
  return (unsigned int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_os_registry_get_ID_IN 0x60006065UL
#define SYSCALL_os_registry_get_ID_OUT 0x900060b2UL
__attribute__((always_inline)) inline void
os_registry_get_inline(unsigned int index,
                       application_t *out_application_entry) {
  volatile unsigned int parameters[2 + 2];
  parameters[0] = (unsigned int)index;
  parameters[1] = (unsigned int)out_application_entry;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_registry_get_ID_IN)
                 : "r0", "r1");
}

#define SYSCALL_os_ux_ID_IN 0x60006458UL
#define SYSCALL_os_ux_ID_OUT 0x9000641fUL
__attribute__((always_inline)) inline unsigned int
os_ux_inline(bolos_ux_params_t *params) {
  volatile unsigned int parameters[2 + 1];
#ifdef __clang_analyzer__
  parameters[1] = 0;
#endif
  parameters[0] = (unsigned int)params;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_ux_ID_IN)
                 : "r0", "r1");
  return (unsigned int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_os_ux_result_ID_IN 0x60006500UL
#define SYSCALL_os_ux_result_ID_OUT 0x900065b0UL
__attribute__((always_inline)) inline void
os_ux_result_inline(bolos_ux_params_t *params) {
  volatile unsigned int parameters[2 + 1];
  parameters[0] = (unsigned int)params;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_ux_result_ID_IN)
                 : "r0", "r1");
}

#define SYSCALL_os_ux_read_parameters_ID_IN 0x600066a7UL
#define SYSCALL_os_ux_read_parameters_ID_OUT 0x900066abUL
__attribute__((always_inline)) inline void
os_ux_read_parameters_inline(bolos_ux_params_t *params) {
  volatile unsigned int parameters[2 + 1];
  parameters[0] = (unsigned int)params;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_ux_read_parameters_ID_IN)
                 : "r0", "r1");
}

#define SYSCALL_os_lib_call_ID_IN 0x6000670dUL
#define SYSCALL_os_lib_call_ID_OUT 0x9000675cUL
__attribute__((always_inline)) inline void
os_lib_call_inline(unsigned int *call_parameters) {
  volatile unsigned int parameters[2 + 1];
  parameters[0] = (unsigned int)call_parameters;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_lib_call_ID_IN)
                 : "r0", "r1");
}

#define SYSCALL_os_lib_end_ID_IN 0x6000688dUL
#define SYSCALL_os_lib_end_ID_OUT 0x90006808UL
__attribute__((always_inline)) inline void os_lib_end_inline(void) {
  volatile unsigned int parameters[2];
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_lib_end_ID_IN)
                 : "r0", "r1");
}

#define SYSCALL_os_lib_throw_ID_IN 0x60006945UL
#define SYSCALL_os_lib_throw_ID_OUT 0x90006987UL
__attribute__((always_inline)) inline void
os_lib_throw_inline(unsigned int exception) {
  volatile unsigned int parameters[2 + 1];
  parameters[0] = (unsigned int)exception;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_lib_throw_ID_IN)
                 : "r0", "r1");
}

#define SYSCALL_os_flags_ID_IN 0x60006a6eUL
#define SYSCALL_os_flags_ID_OUT 0x90006a7fUL
__attribute__((always_inline)) inline unsigned int os_flags_inline(void) {
  volatile unsigned int parameters[2];
#ifdef __clang_analyzer__
  parameters[1] = 0;
#endif
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_flags_ID_IN)
                 : "r0", "r1");
  return (unsigned int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_os_version_ID_IN 0x60006bb8UL
#define SYSCALL_os_version_ID_OUT 0x90006bc4UL
__attribute__((always_inline)) inline unsigned int
os_version_inline(unsigned char *version, unsigned int maxlength) {
  volatile unsigned int parameters[2 + 2];
  parameters[0] = (unsigned int)version;
  parameters[1] = (unsigned int)maxlength;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_version_ID_IN)
                 : "r0", "r1");
  return (unsigned int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_os_serial_ID_IN 0x60006cb3UL
#define SYSCALL_os_serial_ID_OUT 0x90006cd6UL
__attribute__((always_inline)) inline unsigned int
os_serial_inline(unsigned char *serial, unsigned int maxlength) {
  volatile unsigned int parameters[2 + 2];
  parameters[0] = (unsigned int)serial;
  parameters[1] = (unsigned int)maxlength;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_serial_ID_IN)
                 : "r0", "r1");
  return (unsigned int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_os_seph_features_ID_IN 0x60006ed6UL
#define SYSCALL_os_seph_features_ID_OUT 0x90006e44UL
__attribute__((always_inline)) inline unsigned int
os_seph_features_inline(void) {
  volatile unsigned int parameters[2];
#ifdef __clang_analyzer__
  parameters[1] = 0;
#endif
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_seph_features_ID_IN)
                 : "r0", "r1");
  return (unsigned int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_os_seph_version_ID_IN 0x60006facUL
#define SYSCALL_os_seph_version_ID_OUT 0x90006f5dUL
__attribute__((always_inline)) inline unsigned int
os_seph_version_inline(unsigned char *version, unsigned int maxlength) {
  volatile unsigned int parameters[2 + 2];
  parameters[0] = (unsigned int)version;
  parameters[1] = (unsigned int)maxlength;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_seph_version_ID_IN)
                 : "r0", "r1");
  return (unsigned int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_os_bootloader_version_ID_IN 0x6000a917UL
#define SYSCALL_os_bootloader_version_ID_OUT 0x9000a97aUL
__attribute__((always_inline)) inline unsigned int
os_bootloader_version_inline(unsigned char *version, unsigned int maxlength) {
  volatile unsigned int parameters[2 + 2];
  parameters[0] = (unsigned int)version;
  parameters[1] = (unsigned int)maxlength;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_bootloader_version_ID_IN)
                 : "r0", "r1");
  return (unsigned int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_os_setting_get_ID_IN 0x600070c5UL
#define SYSCALL_os_setting_get_ID_OUT 0x900070afUL
__attribute__((always_inline)) inline unsigned int
os_setting_get_inline(unsigned int setting_id, unsigned char *value,
                      unsigned int maxlen) {
  volatile unsigned int parameters[2 + 3];
  parameters[0] = (unsigned int)setting_id;
  parameters[1] = (unsigned int)value;
  parameters[2] = (unsigned int)maxlen;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_setting_get_ID_IN)
                 : "r0", "r1");
  return (unsigned int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_os_setting_set_ID_IN 0x60007196UL
#define SYSCALL_os_setting_set_ID_OUT 0x900071a5UL
__attribute__((always_inline)) inline void
os_setting_set_inline(unsigned int setting_id, unsigned char *value,
                      unsigned int length) {
  volatile unsigned int parameters[2 + 3];
  parameters[0] = (unsigned int)setting_id;
  parameters[1] = (unsigned int)value;
  parameters[2] = (unsigned int)length;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_setting_set_ID_IN)
                 : "r0", "r1");
}

#define SYSCALL_os_get_memory_info_ID_IN 0x60007263UL
#define SYSCALL_os_get_memory_info_ID_OUT 0x900072cbUL
__attribute__((always_inline)) inline void
os_get_memory_info_inline(meminfo_t *meminfo) {
  volatile unsigned int parameters[2 + 1];
  parameters[0] = (unsigned int)meminfo;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_get_memory_info_ID_IN)
                 : "r0", "r1");
}

#define SYSCALL_os_registry_get_tag_ID_IN 0x60007351UL
#define SYSCALL_os_registry_get_tag_ID_OUT 0x90007389UL
__attribute__((always_inline)) inline unsigned int
os_registry_get_tag_inline(unsigned int appidx, unsigned int *tlvoffset,
                           unsigned int tag, unsigned int value_offset,
                           void *buffer, unsigned int maxlength) {
  volatile unsigned int parameters[2 + 6];
  parameters[0] = (unsigned int)appidx;
  parameters[1] = (unsigned int)tlvoffset;
  parameters[2] = (unsigned int)tag;
  parameters[3] = (unsigned int)value_offset;
  parameters[4] = (unsigned int)buffer;
  parameters[5] = (unsigned int)maxlength;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_registry_get_tag_ID_IN)
                 : "r0", "r1");
  return (unsigned int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_os_registry_get_current_app_tag_ID_IN 0x600074d4UL
#define SYSCALL_os_registry_get_current_app_tag_ID_OUT 0x90007487UL
__attribute__((always_inline)) inline unsigned int
os_registry_get_current_app_tag_inline(unsigned int tag, unsigned char *buffer,
                                       unsigned int maxlen) {
  volatile unsigned int parameters[2 + 3];
  parameters[0] = (unsigned int)tag;
  parameters[1] = (unsigned int)buffer;
  parameters[2] = (unsigned int)maxlen;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_registry_get_current_app_tag_ID_IN)
                 : "r0", "r1");
  return (unsigned int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_os_customca_verify_ID_IN 0x60009061UL
#define SYSCALL_os_customca_verify_ID_OUT 0x90009082UL
__attribute__((always_inline)) inline unsigned int
os_customca_verify_inline(unsigned char *hash, unsigned char *sign,
                          unsigned int sign_length) {
  volatile unsigned int parameters[2 + 3];
  parameters[0] = (unsigned int)hash;
  parameters[1] = (unsigned int)sign;
  parameters[2] = (unsigned int)sign_length;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_customca_verify_ID_IN)
                 : "r0", "r1");
  return (unsigned int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_os_sched_exec_ID_IN 0x60009979UL
#define SYSCALL_os_sched_exec_ID_OUT 0x90009948UL
__attribute__((always_inline)) inline void
os_sched_exec_inline(unsigned int application_index) {
  volatile unsigned int parameters[2 + 1];
  parameters[0] = (unsigned int)application_index;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_sched_exec_ID_IN)
                 : "r0", "r1");
}

#define SYSCALL_os_sched_exit_ID_IN 0x60009abeUL
#define SYSCALL_os_sched_exit_ID_OUT 0x90009adeUL
__attribute__((always_inline)) inline void
os_sched_exit_inline(bolos_task_status_t exit_code) {
  volatile unsigned int parameters[2 + 1];
  parameters[0] = (unsigned int)exit_code;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_sched_exit_ID_IN)
                 : "r0", "r1");
}

#define SYSCALL_os_sched_is_running_ID_IN 0x60009bdaUL
#define SYSCALL_os_sched_is_running_ID_OUT 0x90009b7dUL
__attribute__((always_inline)) inline bolos_bool_t
os_sched_is_running_inline(unsigned int task_idx) {
  volatile unsigned int parameters[2 + 1];
#ifdef __clang_analyzer__
  parameters[1] = 0;
#endif
  parameters[0] = (unsigned int)task_idx;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_sched_is_running_ID_IN)
                 : "r0", "r1");
  return (bolos_bool_t)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_os_sched_create_ID_IN 0x600077c7UL
#define SYSCALL_os_sched_create_ID_OUT 0x900077a6UL
__attribute__((always_inline)) inline unsigned int os_sched_create_inline(
    unsigned int permissions, void *main, void *nvram,
    unsigned int nvram_length, void *ram0, unsigned int ram0_length, void *ram1,
    unsigned int ram1_length, void *stack, unsigned int stack_length) {
  volatile unsigned int parameters[2 + 10];
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
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_sched_create_ID_IN)
                 : "r0", "r1");
  return (unsigned int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_os_sched_kill_ID_IN 0x600078d8UL
#define SYSCALL_os_sched_kill_ID_OUT 0x9000783dUL
__attribute__((always_inline)) inline void
os_sched_kill_inline(unsigned int taskidx) {
  volatile unsigned int parameters[2 + 1];
  parameters[0] = (unsigned int)taskidx;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_sched_kill_ID_IN)
                 : "r0", "r1");
}

#define SYSCALL_io_seph_send_ID_IN 0x60008381UL
#define SYSCALL_io_seph_send_ID_OUT 0x900083edUL
__attribute__((always_inline)) inline void
io_seph_send_inline(const unsigned char *buffer, unsigned short length) {
  volatile unsigned int parameters[2 + 2];
  parameters[0] = (unsigned int)buffer;
  parameters[1] = (unsigned int)length;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_io_seph_send_ID_IN)
                 : "r0", "r1");
}

#define SYSCALL_io_seph_is_status_sent_ID_IN 0x600084bbUL
#define SYSCALL_io_seph_is_status_sent_ID_OUT 0x90008405UL
__attribute__((always_inline)) inline unsigned int
io_seph_is_status_sent_inline(void) {
  volatile unsigned int parameters[2];
#ifdef __clang_analyzer__
  parameters[1] = 0;
#endif
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_io_seph_is_status_sent_ID_IN)
                 : "r0", "r1");
  return (unsigned int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_io_seph_recv_ID_IN 0x600085e4UL
#define SYSCALL_io_seph_recv_ID_OUT 0x900085f7UL
__attribute__((always_inline)) inline unsigned short
io_seph_recv_inline(unsigned char *buffer, unsigned short maxlength,
                    unsigned int flags) {
  volatile unsigned int parameters[2 + 3];
  parameters[0] = (unsigned int)buffer;
  parameters[1] = (unsigned int)maxlength;
  parameters[2] = (unsigned int)flags;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_io_seph_recv_ID_IN)
                 : "r0", "r1");
  return (unsigned short)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_nvm_write_page_ID_IN 0x600086feUL
#define SYSCALL_nvm_write_page_ID_OUT 0x900086fcUL
__attribute__((always_inline)) inline void
nvm_write_page_inline(unsigned char *page_adr) {
  volatile unsigned int parameters[2 + 1];
  parameters[0] = (unsigned int)page_adr;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_nvm_write_page_ID_IN)
                 : "r0", "r1");
}

#define SYSCALL_try_context_get_ID_IN 0x600087b1UL
#define SYSCALL_try_context_get_ID_OUT 0x90008722UL
__attribute__((always_inline)) inline try_context_t *
try_context_get_inline(void) {
  volatile unsigned int parameters[2];
#ifdef __clang_analyzer__
  parameters[1] = 0;
#endif
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_try_context_get_ID_IN)
                 : "r0", "r1");
  return (try_context_t *)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_try_context_set_ID_IN 0x60008875UL
#define SYSCALL_try_context_set_ID_OUT 0x9000883cUL
__attribute__((always_inline)) inline try_context_t *
try_context_set_inline(try_context_t *context) {
  volatile unsigned int parameters[2 + 1];
#ifdef __clang_analyzer__
  parameters[1] = 0;
#endif
  parameters[0] = (unsigned int)context;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_try_context_set_ID_IN)
                 : "r0", "r1");
  return (try_context_t *)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_cx_rng_u32_ID_IN 0x600089ecUL
#define SYSCALL_cx_rng_u32_ID_OUT 0x900089d4UL
__attribute__((always_inline)) inline unsigned int cx_rng_u32_inline(void) {
  volatile unsigned int parameters[2];
#ifdef __clang_analyzer__
  parameters[1] = 0;
#endif
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_cx_rng_u32_ID_IN)
                 : "r0", "r1");
  return (unsigned int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_os_sched_last_status_ID_IN 0x60009c8bUL
#define SYSCALL_os_sched_last_status_ID_OUT 0x90009c6dUL
__attribute__((always_inline)) inline bolos_task_status_t
os_sched_last_status_inline(unsigned int task_idx) {
  volatile unsigned int parameters[2 + 1];
#ifdef __clang_analyzer__
  parameters[1] = 0;
#endif
  parameters[0] = (unsigned int)task_idx;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_sched_last_status_ID_IN)
                 : "r0", "r1");
  return (bolos_task_status_t)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_os_sched_yield_ID_IN 0x60009dbeUL
#define SYSCALL_os_sched_yield_ID_OUT 0x90009d2dUL
__attribute__((always_inline)) inline void
os_sched_yield_inline(bolos_task_status_t status) {
  volatile unsigned int parameters[2 + 1];
  parameters[0] = (unsigned int)status;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_sched_yield_ID_IN)
                 : "r0", "r1");
}

#define SYSCALL_os_sched_switch_ID_IN 0x60009e1dUL
#define SYSCALL_os_sched_switch_ID_OUT 0x90009e30UL
__attribute__((always_inline)) inline void
os_sched_switch_inline(unsigned int task_idx, bolos_task_status_t status) {
  volatile unsigned int parameters[2 + 2];
  parameters[0] = (unsigned int)task_idx;
  parameters[1] = (unsigned int)status;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_sched_switch_ID_IN)
                 : "r0", "r1");
}

#define SYSCALL_os_sched_current_task_ID_IN 0x60008b51UL
#define SYSCALL_os_sched_current_task_ID_OUT 0x90008bf0UL
__attribute__((always_inline)) inline unsigned int
os_sched_current_task_inline(void) {
  volatile unsigned int parameters[2];
#ifdef __clang_analyzer__
  parameters[1] = 0;
#endif
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_sched_current_task_ID_IN)
                 : "r0", "r1");
  return (unsigned int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_os_mpu_protect_ram_ID_IN 0x60008d42UL
#define SYSCALL_os_mpu_protect_ram_ID_OUT 0x90008d80UL
__attribute__((always_inline)) inline unsigned int
os_mpu_protect_ram_inline(unsigned int state) {
  volatile unsigned int parameters[2 + 1];
#ifdef __clang_analyzer__
  parameters[1] = 0;
#endif
  parameters[0] = (unsigned int)state;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_mpu_protect_ram_ID_IN)
                 : "r0", "r1");
  return (unsigned int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_os_mpu_protect_flash_ID_IN 0x60008ef7UL
#define SYSCALL_os_mpu_protect_flash_ID_OUT 0x90008ed2UL
__attribute__((always_inline)) inline unsigned int
os_mpu_protect_flash_inline(unsigned int state) {
  volatile unsigned int parameters[2 + 1];
#ifdef __clang_analyzer__
  parameters[1] = 0;
#endif
  parameters[0] = (unsigned int)state;
  __asm volatile("mov r0, %1\n"
                 "mov r1, %0\n"
                 "svc #1" ::"r"(parameters),
                 "r"(SYSCALL_os_mpu_protect_flash_ID_IN)
                 : "r0", "r1");
  return (unsigned int)(((volatile unsigned int *)parameters)[1]);
}

#define SYSCALL_PARAMETER_ARRAY_MAX_LEN 12

#endif // SYSCALL_DEFS_H
