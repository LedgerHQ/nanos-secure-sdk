#ifdef HAVE_SHA3

#ifndef CX_SHA3_H
#define CX_SHA3_H

#include "lcx_sha3.h"

extern const cx_hash_info_t cx_sha3_info;
extern const cx_hash_info_t cx_keccak_info;
extern const cx_hash_info_t cx_shake128_info;
extern const cx_hash_info_t cx_shake256_info;

cx_err_t cx_sha3_update(cx_sha3_t *ctx, const uint8_t *data, size_t len);
cx_err_t cx_sha3_final(cx_sha3_t *ctx, uint8_t *digest);
size_t cx_sha3_get_output_size(const cx_sha3_t *ctx);

#endif // CX_SHA3_H

#endif // HAVE_SHA3
