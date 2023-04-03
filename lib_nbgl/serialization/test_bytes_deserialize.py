from turtle import back
from nbgl_lib import *
import pytest


@pytest.fixture
def data_test():
    with open("data_test.txt", "r") as data_test_file:
        data_test = data_test_file.readlines()
        data_test = list(map(lambda s: s.rstrip().split(','), data_test))
        data_test = {el[0]: el[1] for el in data_test}
    return data_test


def run_deserialize_nbgl(hex_str: str):
    bytes_in = bytes.fromhex(hex_str)
    return deserialize_nbgl_bytes(bytes_in)


def test_draw_nbgl_container(data_test):
    serialized = data_test["test_draw_nbgl_container"]
    deserialized = run_deserialize_nbgl(serialized)
    expected = \
        NbglDrawObjectEvent(
            obj=NbglContainer(
                area=NbglArea(
                    background_color=NbglColor.DARK_GRAY,
                    bpp=NbglBpp.BPP_4,
                    height=450,
                    width=460,
                    x0=56,
                    y0=12
                ),
                layout=NbglDirection.VERTICAL,
                nb_children=4,
                force_clean=True
            )
        )

    assert deserialized == expected


def test_draw_nbgl_text_area(data_test):
    serialized = data_test["test_draw_nbgl_text_area"]
    deserialized = run_deserialize_nbgl(serialized)
    excepted = \
        NbglDrawObjectEvent(
            obj=NbglTextArea(
                area=NbglArea(
                    bpp=NbglBpp.BPP_4,
                    width=360,
                    height=400,
                    x0=12,
                    y0=256,
                    background_color=NbglColor.DARK_GRAY,
                ),

                text_color=NbglColor.BLACK,
                text_alignment=NbglAlignment.BOTTOM_RIGHT,
                style=NbglStyle.LEDGER_BORDER,
                font_id=NbglFontId.BAGL_FONT_INTER_MEDIUM_32px,
                localized=False,
                auto_hide_long_line=True,
                text="arthur"
            )
        )
    assert deserialized == excepted


def test_draw_nbgl_line(data_test):
    serialized = data_test["test_draw_nbgl_line"]
    deserialized = run_deserialize_nbgl(serialized)
    expected = \
        NbglDrawObjectEvent(
            obj=NbglLine(
                area=NbglArea(
                    bpp=NbglBpp.BPP_1,
                    width=36,
                    height=267,
                    x0=0,
                    y0=42,
                    background_color=NbglColor.WHITE,
                ),
                direction=NbglDirection.HORIZONTAL,
                line_color=NbglColor.DARK_GRAY,
                thickness=4,
                offset=2
            )
        )
    assert deserialized == expected


def test_draw_nbgl_qr_code(data_test):
    serialized = data_test["test_draw_nbgl_qr_code"]
    deserialized = run_deserialize_nbgl(serialized)
    expected = \
        NbglDrawObjectEvent(
            obj=NbglQrCode(
                area=NbglArea(
                    background_color=NbglColor.DARK_GRAY,
                    bpp=NbglBpp.BPP_2,
                    height=55,
                    width=66,
                    x0=400,
                    y0=300
                ),
                foreground_color=NbglColor.DARK_GRAY,
                text="fatstacks",
                version=NbglQrCodeVersion.QRCODE_V10
            ))
    assert deserialized == expected


def test_draw_nbgl_radio(data_test):
    serialized = data_test["test_draw_nbgl_radio"]
    deserialized = run_deserialize_nbgl(serialized)
    expected = \
        NbglDrawObjectEvent(
            obj=NbglRadioButton(
                area=NbglArea(
                    background_color=NbglColor.BLACK,
                    bpp=NbglBpp.BPP_4,
                    height=100,
                    width=200,
                    x0=123,
                    y0=234
                ),
                active_color=NbglColor.BLACK,
                border_color=NbglColor.DARK_GRAY,
                state=NbglState.ON_STATE
            )
        )
    assert deserialized == expected


def test_draw_nbgl_switch(data_test):
    serialized = data_test["test_draw_nbgl_switch"]
    deserialized = run_deserialize_nbgl(serialized)
    expected = \
        NbglDrawObjectEvent(
            obj=NbglSwitch(
                area=NbglArea(
                    background_color=NbglColor.LIGHT_GRAY,
                    bpp=NbglBpp.BPP_1,
                    height=333,
                    width=89,
                    x0=1,
                    y0=10000
                ),
                off_color=NbglColor.WHITE,
                on_color=NbglColor.BLACK,
                state=NbglState.OFF_STATE
            )
        )
    assert deserialized == expected


def test_draw_nbgl_progress_bar(data_test):
    serialized = data_test["test_draw_nbgl_progress_bar"]
    deserialized = run_deserialize_nbgl(serialized)
    expected = \
        NbglDrawObjectEvent(
            obj=NbglProgressBar(
                area=NbglArea(
                    background_color=NbglColor.BLACK,
                    bpp=NbglBpp.BPP_1,
                    height=10000,
                    width=11000,
                    x0=12000,
                    y0=13000,
                ),
                with_border=True,
                state=91
            )
        )
    assert deserialized == expected


def test_draw_nbgl_page_indicator(data_test):
    serialized = data_test["test_draw_nbgl_page_indicator"]
    deserialized = run_deserialize_nbgl(serialized)
    expected = \
        NbglDrawObjectEvent(
            obj=NbglPageIndicator(
                area=NbglArea(
                    background_color=NbglColor.BLACK,
                    bpp=NbglBpp.BPP_2,
                    height=11,
                    width=22,
                    x0=33,
                    y0=44
                ),
                active_page=2,
                nb_pages=10
            )
        )
    assert deserialized == expected


def test_draw_nbgl_button(data_test):
    serialized = data_test["test_draw_nbgl_button"]
    deserialized = run_deserialize_nbgl(serialized)
    expected = \
        NbglDrawObjectEvent(
            obj=NbglButton(
                area=NbglArea(
                    background_color=NbglColor.DARK_GRAY,
                    bpp=NbglBpp.BPP_1,
                    height=50,
                    width=255,
                    x0=500,
                    y0=1000
                ),
                inner_color=NbglColor.WHITE,
                border_color=NbglColor.DARK_GRAY,
                foreground_color=NbglColor.LIGHT_GRAY,
                radius=NbglRadius.RADIUS_24_PIXELS,
                font_id=NbglFontId.BAGL_FONT_HM_ALPHA_MONO_MEDIUM_32px,
                text="Test button",
                localized=True
            )
        )
    assert deserialized == expected


def test_draw_nbgl_image(data_test):
    serialized = data_test["test_draw_nbgl_image"]
    deserialized = run_deserialize_nbgl(serialized)
    expected = \
        NbglDrawObjectEvent(
            obj=NbglImage(
                area=NbglArea(
                    background_color=NbglColor.WHITE,
                    bpp=NbglBpp.BPP_2,
                    height=101,
                    width=201,
                    x0=124,
                    y0=235
                ),
                foreground_color=NbglColor.DARK_GRAY
            )
        )
    assert deserialized == expected


def test_draw_nbgl_keyboard(data_test):
    serialized = data_test["test_draw_nbgl_keyboard"]
    deserialized = run_deserialize_nbgl(serialized)
    expected = \
        NbglDrawObjectEvent(
            obj=NbglKeyboard(
                area=NbglArea(
                    width=225,
                    height=210,
                    x0=332,
                    y0=431,
                    background_color=NbglColor.LIGHT_GRAY,
                    bpp=NbglBpp.BPP_2
                ),
                text_color=NbglColor.WHITE,
                border_color=NbglColor.BLACK,
                letters_only=True,
                upper_case=False,
                mode=NbglKeyboardMode.MODE_DIGITS,
                key_mask=0x12345678
            )
        )
    assert deserialized == expected


def test_draw_nbgl_keypad(data_test):
    serialized = data_test["test_draw_nbgl_keypad"]
    deserialized = run_deserialize_nbgl(serialized)
    expected = \
        NbglDrawObjectEvent(
            obj=NbglKeypad(
                area=NbglArea(
                    width=4,
                    height=4,
                    x0=3,
                    y0=4,
                    background_color=NbglColor.WHITE,
                    bpp=NbglBpp.BPP_4
                ),
                text_color=NbglColor.WHITE,
                border_color=NbglColor.BLACK,
                enable_backspace=True,
                enable_validate=False
            )
        )
    assert deserialized == expected


def test_draw_nbgl_spinner(data_test):
    serialized = data_test["test_draw_nbgl_spinner"]
    deserialized = run_deserialize_nbgl(serialized)
    expected = \
        NbglDrawObjectEvent(
            obj=NbglSpinner(
                area=NbglArea(
                    background_color=NbglColor.LIGHT_GRAY,
                    bpp=NbglBpp.BPP_1,
                    height=14,
                    width=25,
                    x0=12,
                    y0=10,
                ),
                position=2
            )
        )
    assert deserialized == expected


def test_draw_nbgl_image_file(data_test):
    serialized = data_test["test_draw_nbgl_image_file"]
    deserialized = run_deserialize_nbgl(serialized)
    expected = \
        NbglDrawObjectEvent(
            obj=NbglImageFile(
                area=NbglArea(
                    background_color=NbglColor.DARK_GRAY,
                    bpp=NbglBpp.BPP_4,
                    height=24,
                    width=35,
                    x0=22,
                    y0=20,
                ),
            )
        )
    assert deserialized == expected


def test_refresh_area(data_test):
    serialized = data_test["test_refresh_area"]
    deserialized = run_deserialize_nbgl(serialized)
    expected = \
        NbglRefreshAreaEvent(
            area=NbglArea(
                background_color=NbglColor.WHITE,
                bpp=NbglBpp.BPP_4,
                height=4,
                width=4,
                x0=3,
                y0=4,
            ),
        )
    assert deserialized == expected
