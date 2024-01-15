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

#ifdef HAVE_SE_TOUCH
static void test_get_length(void **state __attribute__((unused)))
{
    char *str_with_unicode    = "çoto";
    char *str_without_unicode = "toto";
    fetch_language_packs();

    uint16_t width = nbgl_getTextWidth(BAGL_FONT_INTER_REGULAR_24px, str_without_unicode);
    assert_int_equal(width, 46);
    uint16_t len = nbgl_getTextLength(str_without_unicode);

    assert_int_equal(len, 4);
    len = nbgl_getTextLength(str_with_unicode);
    assert_int_equal(len, 5);
    assert_int_equal(strlen(str_with_unicode), 5);

    width = nbgl_getTextWidth(BAGL_FONT_INTER_REGULAR_24px, str_with_unicode);
    assert_int_equal(width, 50);

    char myChar = 0x30;
    width       = nbgl_getCharWidth(BAGL_FONT_INTER_REGULAR_24px, &myChar);
    assert_int_equal(width, 15);
    width = nbgl_getCharWidth(BAGL_FONT_INTER_REGULAR_24px, "ç");
    assert_int_equal(width, 13);

    assert_int_equal(nbgl_getTextNbLines(str_without_unicode), 1);
    assert_int_equal(nbgl_getTextNbLines(str_with_unicode), 1);
    assert_int_equal(nbgl_getTextNbLines("bonjour\nau revoir"), 2);
    assert_int_equal(nbgl_getTextNbLines("bonjour\nau çevoir"), 2);

    // '\n' is considered as end of string for nbgl_getTextLength
    assert_int_equal(nbgl_getTextLength("bonjour\nau revoir"), 7);
    // 'ç' counts for 2 bytes
    assert_int_equal(nbgl_getTextLength("bonçour\nau revoir"), 8);

    nbgl_getTextMaxLenAndWidth(
        BAGL_FONT_INTER_REGULAR_24px, "totoour\nau revoir", 50, &len, &width, false);
    assert_int_equal(len, 4);
    assert_int_equal(width, 46);

    assert_int_equal(nbgl_getTextWidth(BAGL_FONT_INTER_REGULAR_24px, "au revoir"), 100);
    assert_int_equal(nbgl_getTextWidth(BAGL_FONT_INTER_REGULAR_24px, "totoour"), 83);
    assert_int_equal(strlen("totoour\nau revoir"), 17);
    nbgl_getTextMaxLenAndWidth(
        BAGL_FONT_INTER_REGULAR_24px, "totoour\nau revoir", 100, &len, &width, false);
    assert_int_equal(len, 7);
    assert_int_equal(width, 83);  // width of latest line, doesn't mean anything

    uint8_t nbLines = nbgl_getTextNbLinesInWidth(
        BAGL_FONT_INTER_REGULAR_24px, "totoour\nau revoir", 100, false);
    assert_int_equal(nbLines, 2);
    nbLines
        = nbgl_getTextNbLinesInWidth(BAGL_FONT_INTER_REGULAR_24px, "totoour\nau revoir", 60, false);
    assert_int_equal(nbLines, 4);
    nbLines = nbgl_getTextNbLinesInWidth(BAGL_FONT_INTER_REGULAR_24px, "totoour\na", 50, false);
    assert_int_equal(nbLines, 3);
    nbLines
        = nbgl_getTextNbLinesInWidth(BAGL_FONT_INTER_REGULAR_24px, "totoour\nau revoi", 50, false);
    assert_int_equal(nbLines, 4);

    nbLines = nbgl_getTextNbLinesInWidth(
        BAGL_FONT_INTER_REGULAR_24px, "totçour\nau revoir", 100, false);
    assert_int_equal(nbLines, 2);
    nbLines
        = nbgl_getTextNbLinesInWidth(BAGL_FONT_INTER_REGULAR_24px, "totçour\nau revoir", 60, false);
    assert_int_equal(nbLines, 4);
    nbLines = nbgl_getTextNbLinesInWidth(BAGL_FONT_INTER_REGULAR_24px, "totçour\na", 50, false);
    assert_int_equal(nbLines, 3);
    nbLines
        = nbgl_getTextNbLinesInWidth(BAGL_FONT_INTER_REGULAR_24px, "totçour\nau revoi", 50, false);
    assert_int_equal(nbLines, 4);

    nbLines
        = nbgl_getTextNbLinesInWidth(BAGL_FONT_INTER_REGULAR_24px, "totçour au revoi", 100, true);
    assert_int_equal(nbLines, 2);

    nbgl_getTextMaxLenAndWidth(
        BAGL_FONT_INTER_REGULAR_24px, "totçour\nau revoir", 50, &len, &width, false);
    assert_int_equal(len, 5);
    assert_int_equal(width, 45);

    char textToWrap[32] = "toto";
    nbgl_textWrapOnNbLines(BAGL_FONT_INTER_SEMIBOLD_24px, textToWrap, 156, 2);
    assert_string_equal(textToWrap, "toto");

    strcpy(textToWrap, "bonjour tu aimes les mois");
    nbgl_textWrapOnNbLines(BAGL_FONT_INTER_SEMIBOLD_24px, textToWrap, 156, 2);
    assert_string_equal(textToWrap, "bonjour tu\naimes les...");

    strcpy(textToWrap, "bonjourtuaimestr les mois");
    nbgl_textWrapOnNbLines(BAGL_FONT_INTER_SEMIBOLD_24px, textToWrap, 156, 2);
    assert_string_equal(textToWrap, "bonjourtuaimestr les...");

    nbLines
        = nbgl_getTextNbLinesInWidth(BAGL_FONT_INTER_MEDIUM_32px, "AB WWWWWWWW WWW W", 200, true);
    assert_int_equal(nbLines, 4);

    int height
        = nbgl_getTextHeightInWidth(BAGL_FONT_INTER_MEDIUM_32px, "AB WWWWWWWW WWW W", 200, true);
    assert_int_equal(height, 160);
}
#else   // HAVE_SE_TOUCH
static void test_get_length(void **state __attribute__((unused)))
{
    uint16_t len;
    uint16_t width;
    fetch_language_packs();

    width = nbgl_getTextWidth(BAGL_FONT_OPEN_SANS_REGULAR_11px_1bpp,
                              "justement ca tombe bienheureux");
    assert_int_equal(width, 0xAC);
    width = nbgl_getTextWidth(BAGL_FONT_OPEN_SANS_REGULAR_11px_1bpp, "Setup as a");
    assert_int_equal(width, 0x35);
    width = nbgl_getTextWidth(BAGL_FONT_OPEN_SANS_REGULAR_11px_1bpp, "new device");
    assert_int_equal(width, 0x3A);

    nbgl_getTextMaxLenAndWidth(
        BAGL_FONT_OPEN_SANS_REGULAR_11px_1bpp, "new device", 114, &len, &width, false);
    assert_int_equal(width, 0x3A);

    nbgl_getTextMaxLenAndWidth(
        BAGL_FONT_OPEN_SANS_EXTRABOLD_11px_1bpp, "la mise à jour", 114, &len, &width, false);
    assert_int_equal(width, 77);

    nbgl_getTextMaxLenAndWidth(
        BAGL_FONT_OPEN_SANS_EXTRABOLD_11px_1bpp, "Confirmer", 114, &len, &width, false);
    assert_int_equal(width, 58);

    nbgl_getTextMaxLenInNbLines(BAGL_FONT_OPEN_SANS_REGULAR_11px_1bpp, "toto", 114, 4, &len, true);
    assert_int_equal(len, 4);
    nbgl_getTextMaxLenInNbLines(BAGL_FONT_OPEN_SANS_REGULAR_11px_1bpp,
                                "toto\nbonjour au revoir a demain",
                                114,
                                4,
                                &len,
                                true);
    assert_int_equal(len, strlen("toto\nbonjour au revoir a demain"));
    nbgl_getTextMaxLenInNbLines(BAGL_FONT_OPEN_SANS_REGULAR_11px_1bpp,
                                "toto\nbonjour au revoir a demain\njustement ca tombe bienheureux",
                                114,
                                4,
                                &len,
                                true);
    assert_int_equal(len, strlen("toto\nbonjour au revoir a demain\njustement ca tombe "));

    width = nbgl_getTextWidth(BAGL_FONT_OPEN_SANS_REGULAR_11px_1bpp,
                              "Pressing both buttons allows you");
    assert_int_equal(width, 0xAE);

    assert_int_equal(
        3,
        nbgl_getTextNbPagesInWidth(
            BAGL_FONT_OPEN_SANS_REGULAR_11px_1bpp,
            "Pressing both buttons allows you\nto select or confirm\fNow, download Ledger Live "
            "at\n\bledger.com/start\b\fFollow the instructions in Live to set up your Nano",
            4,
            114));
    nbgl_getTextMaxLenInNbLines(
        BAGL_FONT_OPEN_SANS_REGULAR_11px_1bpp,
        "Pressing both buttons allows you\nto select or confirm\fNow, download Ledger Live "
        "at\n\bledger.com/start\b\fFollow the instructions in Live to set up your Nano",
        114,
        4,
        &len,
        true);
    assert_int_equal(len, strlen("Pressing both buttons allows you\nto select or confirm\f"));
    nbgl_getTextMaxLenInNbLines(BAGL_FONT_OPEN_SANS_REGULAR_11px_1bpp,
                                "Now, download Ledger Live at\n\bledger.com/start\b\fFollow the "
                                "instructions in Live to set up your Nano",
                                114,
                                4,
                                &len,
                                true);
    assert_int_equal(len, strlen("Now, download Ledger Live at\n\bledger.com/start\b\f"));
    nbgl_getTextMaxLenInNbLines(BAGL_FONT_OPEN_SANS_REGULAR_11px_1bpp,
                                "Follow the instructions in Live to set up your Nano",
                                114,
                                4,
                                &len,
                                true);
    assert_int_equal(len, strlen("Follow the instructions in Live to set up your Nano"));

    width = nbgl_getTextWidth(BAGL_FONT_OPEN_SANS_REGULAR_11px_1bpp, "FCC Rules. Operation");
    assert_int_equal(width, 0x6E);
    nbgl_getTextMaxLenInNbLines(BAGL_FONT_OPEN_SANS_REGULAR_11px_1bpp,
                                "\bFCC Notes\b\nThis device complies with Part 15 of the FCC "
                                "Rules. Operation is subject to the following",
                                114,
                                4,
                                &len,
                                true);
    assert_int_equal(
        len,
        strlen("\bFCC Notes\b\nThis device complies with Part 15 of the FCC Rules. Operation "));

    uint8_t nbPages = nbgl_getTextNbPagesInWidth(
        BAGL_FONT_OPEN_SANS_REGULAR_11px_1bpp,
        "Pour sélectionner ou confirmer, appuyez sur les deux boutons\fTéléchargez\nLedger "
        "Live\nmaintenant sur\n\bledger.com/start\b\fSuivez les instructions dans Ledger Live pour "
        "configurer\nvotre Nano",
        4,
        114);
    assert_int_equal(nbPages, 3);
}
#endif  // HAVE_SE_TOUCH

int main(int argc, char **argv)
{
    const struct CMUnitTest tests[] = {cmocka_unit_test(test_get_length)};
    return cmocka_run_group_tests(tests, NULL, NULL);
}
