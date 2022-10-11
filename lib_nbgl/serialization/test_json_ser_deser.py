from threading import local
from nbgl_lib import *
import pytest


def test_json_deserialize_container():
    serialized = {
        'event': 'NBGL_DRAW_OBJ',
        'obj': {
            'type': 'CONTAINER',
            'content': {
                'area': {
                    'width': 1,
                    'height': 2,
                    'x0': 3,
                    'y0': 4,
                    'background_color': 'DARK_GRAY',
                    'bpp': 'BPP_4'
                },
                'layout': 'VERTICAL',
                'nb_children': 4,
                'force_clean': True
            }
        }
    }
    deserialized = \
        NbglDrawObjectEvent(
            obj=NbglContainer(
                area=NbglArea(
                    width=1,
                    height=2,
                    x0=3,
                    y0=4,
                    background_color=NbglColor.DARK_GRAY,
                    bpp=NbglBpp.BPP_4
                ),
                layout=NbglDirection.VERTICAL,
                nb_children=4,
                force_clean=True
            )

        )

    assert serialize_nbgl_json(deserialized) == serialized
    assert deserialized == deserialize_nbgl_json(serialized)


def test_json_deserialize_line():
    serialized = {
        'event': 'NBGL_DRAW_OBJ',
        'obj': {
            'type': 'LINE',
            'content': {
                'area': {
                    'width': 10,
                    'height': 20,
                    'x0': 30,
                    'y0': 40,
                    'background_color': 'BLACK',
                    'bpp': 'BPP_2'
                },
                'direction': 'VERTICAL',
                'line_color': 'WHITE',
                'thickness': 4,
                'offset': 1
            }
        }
    }
    deserialized = \
        NbglDrawObjectEvent(
            obj=NbglLine(
                area=NbglArea(
                    width=10,
                    height=20,
                    x0=30,
                    y0=40,
                    background_color=NbglColor.BLACK,
                    bpp=NbglBpp.BPP_2
                ),
                direction=NbglDirection.VERTICAL,
                line_color=NbglColor.WHITE,
                thickness=4,
                offset=1
            )
        )

    assert serialize_nbgl_json(deserialized) == serialized
    assert deserialized == deserialize_nbgl_json(serialized)


def test_json_deserialize_radio_button():
    serialized = {
        'event': 'NBGL_DRAW_OBJ',
        'obj': {
            'type': 'RADIO_BUTTON',
            'content': {
                'area': {
                    'width': 100,
                    'height': 200,
                    'x0': 300,
                    'y0': 400,
                    'background_color': 'LIGHT_GRAY',
                    'bpp': 'BPP_1'
                },
                'active_color': 'WHITE',
                'border_color': 'BLACK',
                'state': 'OFF_STATE'
            }
        }
    }
    deserialized = \
        NbglDrawObjectEvent(
            obj=NbglRadioButton(
                area=NbglArea(
                    width=100,
                    height=200,
                    x0=300,
                    y0=400,
                    background_color=NbglColor.LIGHT_GRAY,
                    bpp=NbglBpp.BPP_1
                ),
                active_color=NbglColor.WHITE,
                border_color=NbglColor.BLACK,
                state=NbglState.OFF_STATE
            )
        )

    assert serialize_nbgl_json(deserialized) == serialized
    assert deserialized == deserialize_nbgl_json(serialized)


def test_json_deserialize_switch():
    serialized = {
        'event': 'NBGL_DRAW_OBJ',
        'obj': {
            'type': 'SWITCH',
            'content': {
                'area': {
                    'width': 50,
                    'height': 60,
                    'x0': 150,
                    'y0': 300,
                    'background_color': 'DARK_GRAY',
                    'bpp': 'BPP_2'
                },
                'on_color': 'BLACK',
                'off_color': 'DARK_GRAY',
                'state': 'ON_STATE'
            }
        }
    }
    deserialized = \
        NbglDrawObjectEvent(
            obj=NbglSwitch(
                area=NbglArea(
                    width=50,
                    height=60,
                    x0=150,
                    y0=300,
                    background_color=NbglColor.DARK_GRAY,
                    bpp=NbglBpp.BPP_2
                ),
                on_color=NbglColor.BLACK,
                off_color=NbglColor.DARK_GRAY,
                state=NbglState.ON_STATE
            )
        )

    assert serialize_nbgl_json(deserialized) == serialized
    assert deserialized == deserialize_nbgl_json(serialized)


def test_json_deserialize_progress_bar():
    serialized = {
        'event': 'NBGL_DRAW_OBJ',
        'obj': {
            'type': 'PROGRESS_BAR',
            'content': {
                'area': {
                    'width': 20,
                    'height': 30,
                    'x0': 70,
                    'y0': 80,
                    'background_color': 'WHITE',
                    'bpp': 'BPP_1'
                },
                'with_border': True,
                'state': 42
            }
        }
    }
    deserialized = \
        NbglDrawObjectEvent(
            obj=NbglProgressBar(
                area=NbglArea(
                    width=20,
                    height=30,
                    x0=70,
                    y0=80,
                    background_color=NbglColor.WHITE,
                    bpp=NbglBpp.BPP_1
                ),
                with_border=True,
                state=42
            )
        )

    assert serialize_nbgl_json(deserialized) == serialized
    assert deserialized == deserialize_nbgl_json(serialized)


def test_json_deserialize_page_indicator():
    serialized = {
        'event': 'NBGL_DRAW_OBJ',
        'obj': {
            'type': 'PAGE_INDICATOR',
            'content': {
                'area': {
                    'width': 35,
                    'height': 36,
                    'x0': 37,
                    'y0': 38,
                    'background_color': 'BLACK',
                    'bpp': 'BPP_2'
                },
                'active_page': 2,
                'nb_pages': 10
            }
        }
    }
    deserialized = \
        NbglDrawObjectEvent(
            obj=NbglPageIndicator(
                area=NbglArea(
                    width=35,
                    height=36,
                    x0=37,
                    y0=38,
                    background_color=NbglColor.BLACK,
                    bpp=NbglBpp.BPP_2
                ),
                active_page=2,
                nb_pages=10
            )
        )

    assert serialize_nbgl_json(deserialized) == serialized
    assert deserialized == deserialize_nbgl_json(serialized)


def test_json_deserialize_button():
    serialized = {
        'event': 'NBGL_DRAW_OBJ',
        'obj': {
            'type': 'BUTTON',
            'content': {
                'area': {
                    'width': 35,
                    'height': 36,
                    'x0': 37,
                    'y0': 38,
                    'background_color': 'BLACK',
                    'bpp': 'BPP_2'
                },
                'inner_color': 'BLACK',
                'border_color': 'WHITE',
                'foreground_color': 'DARK_GRAY',
                'radius': 'RADIUS_40_PIXELS',
                'font_id': 'BAGL_FONT_INTER_REGULAR_24px',
                'localized': True,
                'text': 'Hello world'
            }
        }
    }
    deserialized = \
        NbglDrawObjectEvent(
            obj=NbglButton(
                area=NbglArea(
                    width=35,
                    height=36,
                    x0=37,
                    y0=38,
                    background_color=NbglColor.BLACK,
                    bpp=NbglBpp.BPP_2
                ),
                inner_color=NbglColor.BLACK,
                border_color=NbglColor.WHITE,
                foreground_color=NbglColor.DARK_GRAY,
                radius=NbglRadius.RADIUS_40_PIXELS,
                font_id=NbglFontId.BAGL_FONT_INTER_REGULAR_24px,
                localized=True,
                text="Hello world"
            )
        )

    assert serialize_nbgl_json(deserialized) == serialized
    assert deserialized == deserialize_nbgl_json(serialized)


def test_json_deserialize_text_area():
    serialized = {
        'event': 'NBGL_DRAW_OBJ',
        'obj': {
            'type': 'TEXT_AREA',
            'content': {
                'area': {
                    'width': 35,
                    'height': 36,
                    'x0': 37,
                    'y0': 38,
                    'background_color': 'BLACK',
                    'bpp': 'BPP_2'
                },
                'text_color': 'BLACK',
                'text_alignment': 'RIGHT_BOTTOM',
                'style': 'HYPERTEXT',
                'font_id': 'BAGL_FONT_INTER_SEMIBOLD_24px',
                'localized': True,
                'auto_hide_long_line': True,
                'text': 'Hello fatstacks'
            }
        }
    }
    deserialized = \
        NbglDrawObjectEvent(
            obj=NbglTextArea(
                area=NbglArea(
                    width=35,
                    height=36,
                    x0=37,
                    y0=38,
                    background_color=NbglColor.BLACK,
                    bpp=NbglBpp.BPP_2
                ),
                text_color=NbglColor.BLACK,
                text_alignment=NbglAlignment.RIGHT_BOTTOM,
                style=NbglStyle.HYPERTEXT,
                font_id=NbglFontId.BAGL_FONT_INTER_SEMIBOLD_24px,
                localized=True,
                auto_hide_long_line=True,
                text='Hello fatstacks'
            )
        )
    assert serialize_nbgl_json(deserialized) == serialized
    assert deserialized == deserialize_nbgl_json(serialized)


def test_json_deserialize_spinner():
    serialized = {
        'event': 'NBGL_DRAW_OBJ',
        'obj': {
            'type': 'SPINNER',
            'content': {
                'area': {
                    'width': 35,
                    'height': 36,
                    'x0': 37,
                    'y0': 38,
                    'background_color': 'BLACK',
                    'bpp': 'BPP_2'
                },
                'position': 3
            }
        }
    }
    deserialized = \
        NbglDrawObjectEvent(
            obj=NbglSpinner(
                area=NbglArea(
                    width=35,
                    height=36,
                    x0=37,
                    y0=38,
                    background_color=NbglColor.BLACK,
                    bpp=NbglBpp.BPP_2
                ),
                position=3
            )
        )

    assert serialize_nbgl_json(deserialized) == serialized
    assert deserialized == deserialize_nbgl_json(serialized)


def test_json_deserialize_image():
    serialized = {
        'event': 'NBGL_DRAW_OBJ',
        'obj': {
            'type': 'IMAGE',
            'content': {
                'area': {
                    'width': 35,
                    'height': 36,
                    'x0': 37,
                    'y0': 38,
                    'background_color': 'BLACK',
                    'bpp': 'BPP_2'
                },
                'foreground_color': 'WHITE'
            }
        }
    }
    deserialized = \
        NbglDrawObjectEvent(
            NbglImage(
                area=NbglArea(
                    width=35,
                    height=36,
                    x0=37,
                    y0=38,
                    background_color=NbglColor.BLACK,
                    bpp=NbglBpp.BPP_2
                ),
                foreground_color=NbglColor.WHITE
            )
        )
    assert serialize_nbgl_json(deserialized) == serialized
    assert deserialized == deserialize_nbgl_json(serialized)


def test_json_deserialize_image_file():
    serialized = {
        'event': 'NBGL_DRAW_OBJ',
        'obj': {
            'type': 'IMAGE_FILE',
            'content': {
                'area': {
                    'width': 35,
                    'height': 36,
                    'x0': 37,
                    'y0': 38,
                    'background_color': 'BLACK',
                    'bpp': 'BPP_2'
                },
            }
        }
    }
    deserialized = \
        NbglDrawObjectEvent(
            obj=NbglImageFile(
                area=NbglArea(
                    width=35,
                    height=36,
                    x0=37,
                    y0=38,
                    background_color=NbglColor.BLACK,
                    bpp=NbglBpp.BPP_2
                ),
            )
        )

    assert serialize_nbgl_json(deserialized) == serialized
    assert deserialized == deserialize_nbgl_json(serialized)


def test_json_deserialize_qr_code():
    serialized = {
        'event': 'NBGL_DRAW_OBJ',
        'obj': {
            'type': 'QR_CODE',
            'content': {
                'area': {
                    'width': 35,
                    'height': 36,
                    'x0': 37,
                    'y0': 38,
                    'background_color': 'BLACK',
                    'bpp': 'BPP_2'
                },
                'foreground_color': 'LIGHT_GRAY',
                'version': 'QRCODE_V10',
                'text': 'Qr code text qr code'
            }
        }
    }
    deserialized = \
        NbglDrawObjectEvent(
            NbglQrCode(
                area=NbglArea(
                    width=35,
                    height=36,
                    x0=37,
                    y0=38,
                    background_color=NbglColor.BLACK,
                    bpp=NbglBpp.BPP_2
                ),
                foreground_color=NbglColor.LIGHT_GRAY,
                version=NbglQrCodeVersion.QRCODE_V10,
                text='Qr code text qr code'
            )
        )

    assert serialize_nbgl_json(deserialized) == serialized
    assert deserialized == deserialize_nbgl_json(serialized)


def test_json_deserialize_keyboard():
    serialized = {
        'event': 'NBGL_DRAW_OBJ',
        'obj': {
            'type': 'KEYBOARD',
            'content': {
                'area': {
                    'width': 35,
                    'height': 36,
                    'x0': 37,
                    'y0': 38,
                    'background_color': 'BLACK',
                    'bpp': 'BPP_2'
                },
                'text_color': 'WHITE',
                'border_color': 'LIGHT_GRAY',
                'letters_only': False,
                'upper_case': True,
                'mode': 'MODE_SPECIAL',
                'key_mask': 255
            }
        }
    }
    deserialized = \
        NbglDrawObjectEvent(
            obj=NbglKeyboard(
                area=NbglArea(
                    width=35,
                    height=36,
                    x0=37,
                    y0=38,
                    background_color=NbglColor.BLACK,
                    bpp=NbglBpp.BPP_2
                ),
                text_color=NbglColor.WHITE,
                border_color=NbglColor.LIGHT_GRAY,
                letters_only=False,
                upper_case=True,
                mode=NbglKeyboardMode.MODE_SPECIAL,
                key_mask=255
            )
        )

    assert serialize_nbgl_json(deserialized) == serialized
    assert deserialized == deserialize_nbgl_json(serialized)


def test_json_deserialize_keypad():
    serialized = {
        'event': 'NBGL_DRAW_OBJ',
        'obj': {
            'type': 'KEYPAD',
            'content': {
                'area': {
                    'width': 35,
                    'height': 36,
                    'x0': 37,
                    'y0': 38,
                    'background_color': 'BLACK',
                    'bpp': 'BPP_2'
                },
                'text_color': 'WHITE',
                'border_color': 'LIGHT_GRAY',
                'enable_backspace': True,
                'enable_validate': False
            }
        }
    }
    deserialized = \
        NbglDrawObjectEvent(
            obj=NbglKeypad(
                area=NbglArea(
                    width=35,
                    height=36,
                    x0=37,
                    y0=38,
                    background_color=NbglColor.BLACK,
                    bpp=NbglBpp.BPP_2
                ),
                text_color=NbglColor.WHITE,
                border_color=NbglColor.LIGHT_GRAY,
                enable_backspace=True,
                enable_validate=False
            )
        )

    assert serialize_nbgl_json(deserialized) == serialized
    assert deserialized == deserialize_nbgl_json(serialized)


def test_json_deserialize_refresh():
    serialized = {
        'event': 'NBGL_REFRESH_AREA',
        'area': {
            'width': 35,
            'height': 36,
            'x0': 37,
            'y0': 38,
            'background_color': 'BLACK',
            'bpp': 'BPP_2'
        },
    }

    deserialized = \
        NbglRefreshAreaEvent(
            area=NbglArea(
                width=35,
                height=36,
                x0=37,
                y0=38,
                background_color=NbglColor.BLACK,
                bpp=NbglBpp.BPP_2
            ),
        )

    assert serialize_nbgl_json(deserialized) == serialized
    assert deserialized == deserialize_nbgl_json(serialized)
