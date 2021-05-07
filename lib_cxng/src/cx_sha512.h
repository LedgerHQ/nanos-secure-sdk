#if defined(HAVE_SHA512) || defined(HAVE_SHA384)

#ifndef CX_SHA512_H
#define CX_SHA512_H

#include <stdbool.h>
#include <stddef.h>
#include "lcx_sha512.h"

#ifdef HAVE_SHA384
extern const cx_hash_info_t cx_sha384_info;
#endif // HAVE_SHA384

#ifdef HAVE_SHA512
extern const cx_hash_info_t cx_sha512_info;
#endif // HAVE_SHA512

cx_err_t cx_sha512_update(cx_sha512_t *ctx, const uint8_t *data, size_t len);
cx_err_t cx_sha512_final(cx_sha512_t *ctx, uint8_t *digest);

#endif // CX_SHA512_H

#endif // defined(HAVE_SHA512) || defined(HAVE_SHA384)
