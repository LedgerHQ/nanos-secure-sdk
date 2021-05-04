#ifdef HAVE_GROESTL

#ifndef CX_GROESTL_H
#define CX_GROESTL_H

#include "lcx_groestl.h"
#include <stdbool.h>
#include <stddef.h>

extern const cx_hash_info_t cx_groestl_info;

cx_err_t cx_groestl_update(cx_groestl_t *ctx, const uint8_t *data, size_t len);
cx_err_t cx_groestl_final(cx_groestl_t *ctx, uint8_t *digest);
size_t cx_groestl_get_output_size(const cx_groestl_t *ctx);

struct cx_xgroestl_s {
    cx_groestl_t  groestl;
    unsigned char temp1[ROWS][COLS1024];
    unsigned char temp2[ROWS][COLS1024];
};
typedef struct cx_xgroestl_s cx_xgroestl_t;

#endif // CX_GROESTL_H

#endif // HAVE_GROESTL
