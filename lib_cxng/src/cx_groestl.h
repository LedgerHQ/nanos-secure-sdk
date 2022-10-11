#ifdef HAVE_GROESTL

#ifndef CX_GROESTL_H
#define CX_GROESTL_H

#include "lcx_groestl.h"
#include <stdbool.h>
#include <stddef.h>

struct cx_xgroestl_s {
    cx_groestl_t  groestl;
    unsigned char temp1[ROWS][COLS1024];
    unsigned char temp2[ROWS][COLS1024];
};
typedef struct cx_xgroestl_s cx_xgroestl_t;

#endif // CX_GROESTL_H

#endif // HAVE_GROESTL
