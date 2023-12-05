#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <string.h>

#include <cmocka.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include "nbgl_fonts.h"
#include "ux_loc.h"

void fetch_language_packs(void);

const LANGUAGE_PACK *language_pack = NULL;

void *pic(void *addr)
{
    return addr;
}

void fetch_language_packs(void)
{
    // If we are looking for a language pack:
    // - if the expected language is found then we'll use its begin/length range.
    // - else we'll use the built-in package and need to reset allowed MMU range.

    FILE *fptr = NULL;

#ifdef HAVE_SE_TOUCH
    fptr = fopen("../bolos_pack_fr_stax.bin", "rb");
#else   // HAVE_SE_TOUCH
    fptr = fopen("../bolos_pack_fr_nanos.bin", "rb");
#endif  // HAVE_SE_TOUCH

    assert_non_null(fptr);
    if (fptr != NULL) {
        fseek(fptr, 0, SEEK_END);

        uint32_t len = ftell(fptr);

        fseek(fptr, 0, SEEK_SET);

        uint8_t *source = (uint8_t *) malloc(len);

        assert_non_null(source);

        assert_int_equal(fread((unsigned char *) source, 1, len, fptr), len);

        fclose(fptr);

        language_pack = (LANGUAGE_PACK *) source;
    }
}

#include "nbgl_fonts.h"
#include "nbgl_font_inter_regular_24.inc"
#include "nbgl_font_inter_semibold_24.inc"
#include "nbgl_font_inter_medium_32.inc"
#include "nbgl_font_inter_regular_24_1bpp.inc"
#include "nbgl_font_inter_semibold_24_1bpp.inc"
#include "nbgl_font_inter_medium_32_1bpp.inc"
#include "nbgl_font_open_sans_extrabold_11.inc"
#include "nbgl_font_open_sans_regular_11.inc"
#include "nbgl_font_open_sans_light_16.inc"

static const nbgl_font_t *const C_nbgl_fonts[] = {
#include "nbgl_font_rom_struct.inc"
};
static const unsigned int C_nbgl_fonts_count = sizeof(C_nbgl_fonts) / sizeof(C_nbgl_fonts[0]);

const nbgl_font_t *nbgl_font_getFont(unsigned int fontId)
{
    unsigned int i = C_nbgl_fonts_count;
    while (i--) {
        // font id match this entry (non indexed array)
        if (C_nbgl_fonts[i]->font_id == fontId) {
            return C_nbgl_fonts[i];
        }
    }

    // id not found
    return NULL;
}
