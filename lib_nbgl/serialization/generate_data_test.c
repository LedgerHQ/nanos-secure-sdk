#ifdef NBGL_GENERATE_DATA_TEST

#include <stdio.h>
#include "nbgl_types.h"
#include "nbgl_serialize.h"
#include "nbgl_obj.h"
#include "nbgl_screen.h"

static uint8_t const C_leftArrow32px_bitmap[] = {
    0x20, 0x00, 0x20, 0x00, 0x02, 0x4d, 0x00, 0x00, 0xe3, 0xf0, 0xe3, 0xf0, 0xe3, 0xf0, 0xe3,
    0xf0, 0xe3, 0xf0, 0xe3, 0xf0, 0xe3, 0xf0, 0xe3, 0xf0, 0xe3, 0xf0, 0xe3, 0xf0, 0xe3, 0xf0,
    0xe3, 0xf0, 0xe3, 0xf0, 0xe3, 0xf0, 0xe3, 0xf0, 0xe3, 0xf0, 0xe3, 0xf0, 0x32, 0x93, 0x92,
    0x64, 0x83, 0x84, 0x64, 0x73, 0x74, 0x84, 0x63, 0x64, 0xa4, 0x53, 0x54, 0xc4, 0x43, 0x44,
    0xe4, 0x33, 0x34, 0xf0, 0x14, 0x23, 0x24, 0xf0, 0x34, 0x13, 0x14, 0xf0, 0x5b, 0xf0, 0x79,
    0xf0, 0x97, 0xf0, 0xb5, 0xf0, 0xd3, 0xf0, 0xf1, 0xf0, 0x10,
};
const nbgl_icon_details_t C_leftArrow32px = {32, 32, NBGL_BPP_1, true, C_leftArrow32px_bitmap};

uint8_t nbgl_objPoolGetId(nbgl_obj_t *obj)
{
    return 0;
}

void print_hex(const char *name, uint8_t *buffer, size_t len)
{
    printf("%s,", name);
    for (size_t i = 0; i < len; i++) {
        printf("%02x", buffer[i]);
    }
    printf("\n");
}

void run_serialize_and_print(const char *name, nbgl_serialized_event_type_e event, nbgl_obj_t *obj)
{
    uint8_t buf[192];
    size_t  len = 0;
    nbgl_serializeNbglEvent(event, obj, buf, &len, sizeof(buf));
    print_hex(name, buf, len);
}

#define SERIALIZE_AND_PRINT(obj, event) \
    run_serialize_and_print(__FUNCTION__, event, (nbgl_obj_t *) obj)

void test_draw_nbgl_screen()
{
    nbgl_screen_t screen = {
        .container.obj.type = SCREEN,

        .container.obj.area.backgroundColor = WHITE,
        .container.obj.area.bpp             = NBGL_BPP_4,
        .container.obj.area.height          = 670,
        .container.obj.area.width           = 400,
        .container.obj.area.x0              = 0,
        .container.obj.area.y0              = 0,
    };

    SERIALIZE_AND_PRINT(&screen, NBGL_DRAW_OBJ);
}

void test_draw_nbgl_container()
{
    nbgl_container_t container = {.obj.type = CONTAINER,

                                  .obj.area.backgroundColor = DARK_GRAY,
                                  .obj.area.bpp             = NBGL_BPP_4,
                                  .obj.area.height          = 450,
                                  .obj.area.width           = 460,
                                  .obj.area.x0              = 56,
                                  .obj.area.y0              = 12,

                                  .layout     = VERTICAL,
                                  .nbChildren = 4,
                                  .forceClean = true};

    SERIALIZE_AND_PRINT(&container, NBGL_DRAW_OBJ);
}

void test_draw_nbgl_text_area()
{
    nbgl_text_area_t text = {.obj.type = TEXT_AREA,

                             .obj.area.backgroundColor = DARK_GRAY,
                             .obj.area.bpp             = NBGL_BPP_4,
                             .obj.area.height          = 400,
                             .obj.area.width           = 360,
                             .obj.area.x0              = 12,
                             .obj.area.y0              = 256,

                             .textColor        = BLACK,
                             .textAlignment    = BOTTOM_RIGHT,
                             .style            = NO_STYLE,
                             .fontId           = BAGL_FONT_INTER_MEDIUM_32px,
                             .localized        = false,
                             .autoHideLongLine = true,
                             .text             = "arthur"};

    SERIALIZE_AND_PRINT(&text, NBGL_DRAW_OBJ);
}

void test_draw_nbgl_line()
{
    nbgl_line_t line = {.obj.type                 = LINE,
                        .obj.area.backgroundColor = WHITE,
                        .obj.area.bpp             = NBGL_BPP_1,
                        .obj.area.height          = 267,
                        .obj.area.width           = 36,
                        .obj.area.x0              = 0,
                        .obj.area.y0              = 42,
                        .direction                = HORIZONTAL,
                        .lineColor                = DARK_GRAY,
                        .thickness                = 4,
                        .offset                   = 2};

    SERIALIZE_AND_PRINT(&line, NBGL_DRAW_OBJ);
}

void test_draw_nbgl_qr_code()
{
    nbgl_qrcode_t qr_code = {.obj.type = QR_CODE,

                             .obj.area.backgroundColor = DARK_GRAY,
                             .obj.area.bpp             = NBGL_BPP_2,
                             .obj.area.height          = 55,
                             .obj.area.width           = 66,
                             .obj.area.x0              = 400,
                             .obj.area.y0              = 300,

                             .foregroundColor = DARK_GRAY,
                             .text            = "fatstacks",
                             .version         = QRCODE_V10};

    SERIALIZE_AND_PRINT(&qr_code, NBGL_DRAW_OBJ);
}

void test_draw_nbgl_radio()
{
    nbgl_radio_t radio = {.obj.type = RADIO_BUTTON,

                          .obj.area.backgroundColor = BLACK,
                          .obj.area.bpp             = NBGL_BPP_4,
                          .obj.area.height          = 100,
                          .obj.area.width           = 200,
                          .obj.area.x0              = 123,
                          .obj.area.y0              = 234,

                          .activeColor = BLACK,
                          .borderColor = DARK_GRAY,
                          .state       = ON_STATE};

    SERIALIZE_AND_PRINT(&radio, NBGL_DRAW_OBJ);
}

void test_draw_nbgl_switch()
{
    nbgl_switch_t switch_obj = {.obj.type = SWITCH,

                                .obj.area.backgroundColor = LIGHT_GRAY,
                                .obj.area.bpp             = NBGL_BPP_1,
                                .obj.area.height          = 333,
                                .obj.area.width           = 89,
                                .obj.area.x0              = 1,
                                .obj.area.y0              = 10000,

                                .offColor = WHITE,
                                .onColor  = BLACK,
                                .state    = OFF_STATE};

    SERIALIZE_AND_PRINT(&switch_obj, NBGL_DRAW_OBJ);
}

void test_draw_nbgl_progress_bar()
{
    nbgl_progress_bar_t progress_bar = {.obj.type = PROGRESS_BAR,

                                        .obj.area.backgroundColor = BLACK,
                                        .obj.area.bpp             = NBGL_BPP_1,
                                        .obj.area.height          = 10000,
                                        .obj.area.width           = 11000,
                                        .obj.area.x0              = 12000,
                                        .obj.area.y0              = 13000,

                                        .withBorder = true,
                                        .state      = 91};

    SERIALIZE_AND_PRINT(&progress_bar, NBGL_DRAW_OBJ);
}

void test_draw_nbgl_page_indicator()
{
    nbgl_page_indicator_t page_indicator = {.obj.type = PAGE_INDICATOR,

                                            .obj.area.backgroundColor = BLACK,
                                            .obj.area.bpp             = NBGL_BPP_2,
                                            .obj.area.height          = 11,
                                            .obj.area.width           = 22,
                                            .obj.area.x0              = 33,
                                            .obj.area.y0              = 44,

                                            .activePage = 2,
                                            .nbPages    = 10};

    SERIALIZE_AND_PRINT(&page_indicator, NBGL_DRAW_OBJ);
}

void test_draw_nbgl_button()
{
    nbgl_button_t button = {
        .obj.type = BUTTON,

        .obj.area.backgroundColor = DARK_GRAY,
        .obj.area.bpp             = NBGL_BPP_1,
        .obj.area.height          = 50,
        .obj.area.width           = 255,
        .obj.area.x0              = 500,
        .obj.area.y0              = 1000,

        .innerColor      = WHITE,
        .borderColor     = DARK_GRAY,
        .foregroundColor = LIGHT_GRAY,
        .radius          = RADIUS_40_PIXELS,
        .fontId          = BAGL_FONT_INTER_MEDIUM_32px,
        .localized       = true,
        .text            = "Test button",
    };

    SERIALIZE_AND_PRINT(&button, NBGL_DRAW_OBJ);
}

void test_draw_nbgl_image()
{
    nbgl_image_t image = {.obj.type = IMAGE,

                          .obj.area.backgroundColor = WHITE,
                          .obj.area.bpp             = NBGL_BPP_1,
                          .obj.area.height          = 32,
                          .obj.area.width           = 32,
                          .obj.area.x0              = 124,
                          .obj.area.y0              = 235,

                          .foregroundColor = DARK_GRAY,
                          .buffer          = &C_leftArrow32px};

    SERIALIZE_AND_PRINT(&image, NBGL_DRAW_OBJ);
}

void test_draw_nbgl_keyboard()
{
    nbgl_keyboard_t keyboard = {
        .obj.type = KEYBOARD,

        .obj.area.backgroundColor = LIGHT_GRAY,
        .obj.area.bpp             = NBGL_BPP_2,
        .obj.area.height          = 210,
        .obj.area.width           = 225,
        .obj.area.x0              = 332,
        .obj.area.y0              = 431,

        .textColor   = WHITE,
        .borderColor = BLACK,
        .lettersOnly = true,
#ifdef HAVE_SE_TOUCH
        .casing = 0,
        .mode   = MODE_DIGITS,
#else   // HAVE_SE_TOUCH
        .mode = MODE_UPPER_LETTERS,
#endif  // HAVE_SE_TOUCH
        .keyMask = 0x12345678,
    };

    SERIALIZE_AND_PRINT(&keyboard, NBGL_DRAW_OBJ);
}

void test_draw_nbgl_keypad()
{
    nbgl_keypad_t keypad = {.obj.type = KEYPAD,

                            .obj.area.backgroundColor = WHITE,
                            .obj.area.bpp             = NBGL_BPP_4,
                            .obj.area.height          = 4,
                            .obj.area.width           = 4,
                            .obj.area.x0              = 3,
                            .obj.area.y0              = 4,

#ifdef HAVE_SE_TOUCH
                            .textColor   = WHITE,
                            .borderColor = BLACK,
#endif  // HAVE_SE_TOUCH
                            .enableBackspace = true,
                            .enableValidate  = false,
#ifdef HAVE_SE_TOUCH
                            .enableDigits = true,
#endif  // HAVE_SE_TOUCH
                            .shuffled = false};

    SERIALIZE_AND_PRINT(&keypad, NBGL_DRAW_OBJ);
}

void test_draw_nbgl_spinner()
{
    nbgl_spinner_t spinner = {.obj.type = SPINNER,

                              .obj.area.backgroundColor = LIGHT_GRAY,
                              .obj.area.bpp             = NBGL_BPP_1,
                              .obj.area.height          = 14,
                              .obj.area.width           = 25,
                              .obj.area.x0              = 12,
                              .obj.area.y0              = 10,

                              .position = 2};

    SERIALIZE_AND_PRINT(&spinner, NBGL_DRAW_OBJ);
}

void test_draw_nbgl_image_file()
{
    nbgl_image_file_t image_file = {
        .obj.type = IMAGE_FILE,

        .obj.area.backgroundColor = DARK_GRAY,
        .obj.area.bpp             = NBGL_BPP_4,
        .obj.area.height          = 24,
        .obj.area.width           = 35,
        .obj.area.x0              = 22,
        .obj.area.y0              = 20,
    };

    SERIALIZE_AND_PRINT(&image_file, NBGL_DRAW_OBJ);
}

void test_refresh_area()
{
    nbgl_area_t area = {
        .backgroundColor = WHITE,
        .bpp             = NBGL_BPP_4,
        .height          = 4,
        .width           = 4,
        .x0              = 3,
        .y0              = 4,
    };

    SERIALIZE_AND_PRINT(&area, NBGL_REFRESH_AREA);
}

int main()
{
#ifdef HAVE_SE_TOUCH
    printf("stax\n");
#else   // HAVE_SE_TOUCH
    printf("nano\n");
#endif  // HAVE_SE_TOUCH
    test_draw_nbgl_screen();
    test_draw_nbgl_container();
#ifdef HAVE_SE_TOUCH
    test_draw_nbgl_line();
#endif  // HAVE_SE_TOUCH
    test_draw_nbgl_text_area();
#ifdef HAVE_SE_TOUCH
    test_draw_nbgl_qr_code();
    test_draw_nbgl_radio();
    test_draw_nbgl_switch();
#endif  // HAVE_SE_TOUCH
    test_draw_nbgl_progress_bar();
#ifdef HAVE_SE_TOUCH
    test_draw_nbgl_page_indicator();
    test_draw_nbgl_button();
#endif  // HAVE_SE_TOUCH
    test_draw_nbgl_image();
    test_draw_nbgl_keyboard();
    test_draw_nbgl_keypad();
#ifdef HAVE_SE_TOUCH
    test_draw_nbgl_spinner();
#endif  // HAVE_SE_TOUCH
    test_draw_nbgl_image_file();
    test_refresh_area();
}

#endif
