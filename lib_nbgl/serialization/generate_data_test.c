#ifdef NBGL_GENERATE_DATA_TEST

#include <stdio.h>
#include "nbgl_types.h"
#include "nbgl_serialize.h"
#include "nbgl_obj.h"

void print_hex(const char *name, uint8_t *buffer, size_t len) {
    printf("%s,", name);
    for(size_t i = 0; i < len; i++) {
        printf("%02x", buffer[i]);
    }
    printf("\n");
}

void run_serialize_and_print(const char *name, nbgl_serialized_event_type_e event, nbgl_obj_t *obj) {
    uint8_t buf[192];
    size_t len = 0;
    nbgl_serializeNbglEvent(event, obj, buf, &len, sizeof(buf));
    print_hex(name, buf, len);
}

#define SERIALIZE_AND_PRINT(obj, event) run_serialize_and_print(__FUNCTION__, event, (nbgl_obj_t *) obj)

void test_draw_nbgl_container()
{
    nbgl_container_t container = {
        .type = CONTAINER,

        .backgroundColor = DARK_GRAY,
        .bpp = NBGL_BPP_4,
        .height = 450,
        .width = 460,
        .x0 = 56,
        .y0 = 12,
        
        .layout = VERTICAL,
        .nbChildren = 4,
        .forceClean = true
    };

    SERIALIZE_AND_PRINT(&container, NBGL_DRAW_OBJ);
}

void test_draw_nbgl_text_area()
{
    nbgl_text_area_t text = {
        .type = TEXT_AREA,

        .backgroundColor = DARK_GRAY,
        .bpp = NBGL_BPP_4,
        .height = 400,
        .width = 360,
        .x0 = 12,
        .y0 = 256,

        .textColor = BLACK,
        .textAlignment = BOTTOM_RIGHT,
        .style = LEDGER_BORDER,
        .fontId = BAGL_FONT_INTER_REGULAR_32px,
        .localized = false,
        .autoHideLongLine = true,
        .text = "arthur"
    };

    SERIALIZE_AND_PRINT(&text, NBGL_DRAW_OBJ);
}

void test_draw_nbgl_line()
{
    nbgl_line_t line = {
        .type = LINE,
        .backgroundColor = WHITE,
        .bpp = NBGL_BPP_1,
        .height = 267,
        .width = 36,
        .x0 = 0,
        .y0 = 42,
        .direction = HORIZONTAL,
        .lineColor = DARK_GRAY,
        .thickness = 4,
        .offset = 2
    };

    SERIALIZE_AND_PRINT(&line, NBGL_DRAW_OBJ);
}

void test_draw_nbgl_qr_code()
{
    nbgl_qrcode_t qr_code = {
        .type = QR_CODE,

        .backgroundColor = DARK_GRAY,
        .bpp = NBGL_BPP_2,
        .height = 55,
        .width = 66,
        .x0 = 400,
        .y0 = 300,

        .foregroundColor = DARK_GRAY,
        .text = "fatstacks",
        .version = QRCODE_V10
    };

    SERIALIZE_AND_PRINT(&qr_code, NBGL_DRAW_OBJ);
}

void test_draw_nbgl_radio()
{
    nbgl_radio_t radio = {
        .type = RADIO_BUTTON,

        .backgroundColor = BLACK,
        .bpp = NBGL_BPP_4,
        .height = 100,
        .width = 200,
        .x0 = 123,
        .y0 = 234,

        .activeColor = BLACK,
        .borderColor = DARK_GRAY,
        .state = ON_STATE
    };

    SERIALIZE_AND_PRINT(&radio, NBGL_DRAW_OBJ);
}

void test_draw_nbgl_switch()
{
    nbgl_switch_t switch_obj = {
        .type = SWITCH,

        .backgroundColor = LIGHT_GRAY,
        .bpp = NBGL_BPP_1,
        .height = 333,
        .width = 89,
        .x0 = 1,
        .y0 = 10000,

        .offColor = WHITE,
        .onColor = BLACK,
        .state = OFF_STATE
    };

    SERIALIZE_AND_PRINT(&switch_obj, NBGL_DRAW_OBJ);
}



void test_draw_nbgl_progress_bar()
{
    nbgl_progress_bar_t progress_bar = {
        .type = PROGRESS_BAR,

        .backgroundColor = BLACK,
        .bpp = NBGL_BPP_1,
        .height = 10000,
        .width = 11000,
        .x0 = 12000,
        .y0 = 13000,

        .withBorder = true,
        .state = 91
    };

    SERIALIZE_AND_PRINT(&progress_bar, NBGL_DRAW_OBJ);
}

void test_draw_nbgl_page_indicator()
{
    nbgl_page_indicator_t page_indicator = {
        .type = PAGE_INDICATOR,

        .backgroundColor = BLACK,
        .bpp = NBGL_BPP_2,
        .height = 11,
        .width = 22,
        .x0 = 33,
        .y0 = 44,

        .activePage = 2,
        .nbPages = 10
    };

    SERIALIZE_AND_PRINT(&page_indicator, NBGL_DRAW_OBJ);
}

void test_draw_nbgl_button()
{
    nbgl_button_t button = {
        .type = BUTTON,

        .backgroundColor = DARK_GRAY,
        .bpp = NBGL_BPP_1,
        .height = 50,
        .width = 255,
        .x0 = 500,
        .y0 = 1000,
        
        .innerColor = WHITE,
        .borderColor = DARK_GRAY,
        .foregroundColor = LIGHT_GRAY,
        .radius = RADIUS_24_PIXELS,
        .fontId = BAGL_FONT_HM_ALPHA_MONO_MEDIUM_32px,
        .localized = true,
        .text = "Test button",
    };

    SERIALIZE_AND_PRINT(&button, NBGL_DRAW_OBJ);
}

void test_draw_nbgl_image()
{
    nbgl_image_t image = {
        .type = IMAGE,

        .backgroundColor = WHITE,
        .bpp = NBGL_BPP_2,
        .height = 101,
        .width = 201,
        .x0 = 124,
        .y0 = 235,

        .foregroundColor = DARK_GRAY
    };

    SERIALIZE_AND_PRINT(&image, NBGL_DRAW_OBJ);
}

void test_draw_nbgl_keyboard()
{
    nbgl_keyboard_t keyboard = {
        .type = KEYBOARD,

        .backgroundColor = LIGHT_GRAY,
        .bpp = NBGL_BPP_2,
        .height = 210,
        .width = 225,
        .x0 = 332,
        .y0 = 431,

        .textColor = WHITE,
        .borderColor = BLACK,
        .lettersOnly = true,
        .upperCase = false,
        .mode = MODE_DIGITS,
        .keyMask = 0x12345678,
    };

    SERIALIZE_AND_PRINT(&keyboard, NBGL_DRAW_OBJ);
}

void test_draw_nbgl_keypad()
{
    nbgl_keypad_t keypad = {
        .type = KEYPAD,

        .backgroundColor = WHITE,
        .bpp = NBGL_BPP_4,
        .height = 4,
        .width = 4,
        .x0 = 3,
        .y0 = 4,

        .textColor = WHITE,
        .borderColor = BLACK,
        .enableBackspace = true,
        .enableValidate = false
    };

    SERIALIZE_AND_PRINT(&keypad, NBGL_DRAW_OBJ);
}

void test_draw_nbgl_spinner()
{
    nbgl_spinner_t spinner = {
        .type = SPINNER,

        .backgroundColor = LIGHT_GRAY,
        .bpp = NBGL_BPP_1,
        .height = 14,
        .width = 25,
        .x0 = 12,
        .y0 = 10,

        .position = 2
    };

    SERIALIZE_AND_PRINT(&spinner, NBGL_DRAW_OBJ);
}

void test_draw_nbgl_image_file()
{
    nbgl_image_file_t image_file = {
        .type = IMAGE_FILE,

        .backgroundColor = DARK_GRAY,
        .bpp = NBGL_BPP_4,
        .height = 24,
        .width = 35,
        .x0 = 22,
        .y0 = 20,
    };

    SERIALIZE_AND_PRINT(&image_file, NBGL_DRAW_OBJ);
}

void test_refresh_area()
{
    nbgl_area_t area = {
        .backgroundColor = WHITE,
        .bpp = NBGL_BPP_4,
        .height = 4,
        .width = 4,
        .x0 = 3,
        .y0 = 4,
    };

    SERIALIZE_AND_PRINT(&area, NBGL_REFRESH_AREA);
}

int main()
{
    test_draw_nbgl_container();
    test_draw_nbgl_line();
    test_draw_nbgl_text_area();
    test_draw_nbgl_qr_code();
    test_draw_nbgl_radio();
    test_draw_nbgl_switch();
    test_draw_nbgl_progress_bar();
    test_draw_nbgl_page_indicator();
    test_draw_nbgl_button();
    test_draw_nbgl_image();
    test_draw_nbgl_keyboard();
    test_draw_nbgl_keypad();
    test_draw_nbgl_spinner();
    test_draw_nbgl_image_file();
    test_refresh_area();
}

#endif
