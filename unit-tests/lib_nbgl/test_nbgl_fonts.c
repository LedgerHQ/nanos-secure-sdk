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

const LANGUAGE_PACK *language_pack = NULL;

static void fetch_language_packs(void) {
  // If we are looking for a language pack:
  // - if the expected language is found then we'll use its begin/length range.
  // - else we'll use the built-in package and need to reset allowed MMU range.

  FILE *fptr = NULL;

  fptr = fopen("../bolos_pack_fr.bin","rb");

  assert_non_null(fptr);
  if (fptr != NULL) {
    fseek(fptr, 0, SEEK_END);

    uint32_t len = ftell(fptr);

    fseek(fptr, 0, SEEK_SET);

    uint8_t *source = (uint8_t*)malloc(len);

    assert_non_null(source);

    assert_int_equal(fread((unsigned char*)source, 1, len, fptr),len);

    fclose(fptr);

    language_pack = (LANGUAGE_PACK *)source;

  }
}

#include "nbgl_fonts.h"
#include "nbgl_font_hmalpha_mono_medium_32.inc"
#include "nbgl_font_inter_regular_24.inc"
#include "nbgl_font_inter_semibold_24.inc"
#include "nbgl_font_inter_medium_32.inc"
#include "nbgl_font_inter_regular_24_1bpp.inc"
#include "nbgl_font_inter_semibold_24_1bpp.inc"
#include "nbgl_font_inter_medium_32_1bpp.inc"

static const nbgl_font_t* const C_nbgl_fonts[] = {
#include "nbgl_font_rom_struct.inc"
};
static const unsigned int C_nbgl_fonts_count = sizeof(C_nbgl_fonts)/sizeof(C_nbgl_fonts[0]);

const nbgl_font_t* nbgl_font_getFont(unsigned int fontId) {
  unsigned int i = C_nbgl_fonts_count;
  while(i--) {
    // font id match this entry (non indexed array)
    if (C_nbgl_fonts[i]->font_id == fontId) {
      return C_nbgl_fonts[i];
    }
  }

  // id not found
  return NULL;
}

void *pic(void *addr) {
  return addr;
}

static void test_get_length(void **state __attribute__((unused))) {
  char *str_with_unicode= "çoto";
  char *str_without_unicode= "toto";
  fetch_language_packs();

  uint16_t width = nbgl_getTextWidth(BAGL_FONT_INTER_REGULAR_24px, str_without_unicode);
  assert_int_equal(width,46);
  uint16_t len = nbgl_getTextLength(str_without_unicode);

  assert_int_equal(len,4);
  len = nbgl_getTextLength(str_with_unicode);
  assert_int_equal(len,5);
  assert_int_equal(strlen(str_with_unicode),5);

  width = nbgl_getTextWidth(BAGL_FONT_INTER_REGULAR_24px, str_with_unicode);
  assert_int_equal(width,45);

  char myChar = 0x30;
  width = nbgl_getCharWidth(BAGL_FONT_INTER_REGULAR_24px, &myChar);
  assert_int_equal(width,15);
  width = nbgl_getCharWidth(BAGL_FONT_INTER_REGULAR_24px, "ç");
  assert_int_equal(width,8);

  assert_int_equal(nbgl_getTextNbLines(str_without_unicode),1);
  assert_int_equal(nbgl_getTextNbLines(str_with_unicode),1);
  assert_int_equal(nbgl_getTextNbLines("bonjour\nau revoir"),2);
  assert_int_equal(nbgl_getTextNbLines("bonjour\nau çevoir"),2);

  // '\n' is considered as end of string for nbgl_getTextLength
  assert_int_equal(nbgl_getTextLength("bonjour\nau revoir"),7);
  // 'ç' counts for 2 bytes
  assert_int_equal(nbgl_getTextLength("bonçour\nau revoir"),8);

  nbgl_getTextMaxLenAndWidth(BAGL_FONT_INTER_REGULAR_24px,"totoour\nau revoir", 50, &len, &width, false);
  assert_int_equal(len,4);
  assert_int_equal(width,46);

  assert_int_equal(nbgl_getTextWidth(BAGL_FONT_INTER_REGULAR_24px,"au revoir"),100);
  assert_int_equal(nbgl_getTextWidth(BAGL_FONT_INTER_REGULAR_24px,"totoour"),83);
  assert_int_equal(strlen("totoour\nau revoir"),17);
  nbgl_getTextMaxLenAndWidth(BAGL_FONT_INTER_REGULAR_24px,"totoour\nau revoir", 100, &len, &width, false);
  assert_int_equal(len,7);
  assert_int_equal(width,83); // width of latest line, doesn't mean anything

  uint8_t nbLines = nbgl_getTextNbLinesInWidth(BAGL_FONT_INTER_REGULAR_24px,"totoour\nau revoir", 100, false);
  assert_int_equal(nbLines,2);
  nbLines = nbgl_getTextNbLinesInWidth(BAGL_FONT_INTER_REGULAR_24px,"totoour\nau revoir", 60, false);
  assert_int_equal(nbLines,4);
  nbLines = nbgl_getTextNbLinesInWidth(BAGL_FONT_INTER_REGULAR_24px,"totoour\na", 50, false);
  assert_int_equal(nbLines,3);
  nbLines = nbgl_getTextNbLinesInWidth(BAGL_FONT_INTER_REGULAR_24px,"totoour\nau revoi", 50, false);
  assert_int_equal(nbLines,4);

  nbLines = nbgl_getTextNbLinesInWidth(BAGL_FONT_INTER_REGULAR_24px,"totçour\nau revoir", 100, false);
  assert_int_equal(nbLines,2);
  nbLines = nbgl_getTextNbLinesInWidth(BAGL_FONT_INTER_REGULAR_24px,"totçour\nau revoir", 60, false);
  assert_int_equal(nbLines,4);
  nbLines = nbgl_getTextNbLinesInWidth(BAGL_FONT_INTER_REGULAR_24px,"totçour\na", 50, false);
  assert_int_equal(nbLines,3);
  nbLines = nbgl_getTextNbLinesInWidth(BAGL_FONT_INTER_REGULAR_24px,"totçour\nau revoi", 50, false);
  assert_int_equal(nbLines,4);

  nbLines = nbgl_getTextNbLinesInWidth(BAGL_FONT_INTER_REGULAR_24px,"totçour au revoi", 100, true);
  assert_int_equal(nbLines,2);

  nbgl_getTextMaxLenAndWidth(BAGL_FONT_INTER_REGULAR_24px,"totçour\nau revoir", 50, &len, &width, false);
  assert_int_equal(len,5);
  assert_int_equal(width,40);

  char textToWrap[32] = "toto";
  nbgl_textWrapOnNbLines(BAGL_FONT_INTER_SEMIBOLD_24px, textToWrap, 156, 2);
  assert_string_equal(textToWrap,"toto");

  strcpy(textToWrap,"bonjour tu aimes les mois");
  nbgl_textWrapOnNbLines(BAGL_FONT_INTER_SEMIBOLD_24px, textToWrap, 156, 2);
  assert_string_equal(textToWrap,"bonjour tu\naimes les...");

  strcpy(textToWrap,"bonjourtuaimestr les mois");
  nbgl_textWrapOnNbLines(BAGL_FONT_INTER_SEMIBOLD_24px, textToWrap, 156, 2);
  assert_string_equal(textToWrap,"bonjourtuaimestr les...");

  nbLines = nbgl_getTextNbLinesInWidth(BAGL_FONT_INTER_MEDIUM_32px, "AB WWWWWWWW WWW W", 200, true);
  assert_int_equal(nbLines,4);

  int height = nbgl_getTextHeightInWidth(BAGL_FONT_INTER_MEDIUM_32px, "AB WWWWWWWW WWW W", 200, true);
  assert_int_equal(height,160);
}

int main(int argc, char **argv) {
  const struct CMUnitTest tests[] = {cmocka_unit_test(test_get_length)};
  return cmocka_run_group_tests(tests, NULL, NULL);
}
