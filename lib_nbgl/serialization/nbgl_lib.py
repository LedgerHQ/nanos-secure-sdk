from dataclasses import dataclass
from enum import IntEnum, Enum
from typing import Tuple, Dict, Union
from abc import ABC, abstractclassmethod
import struct

# Common types


class NbglColor(IntEnum):
    BLACK = 0,
    DARK_GRAY = 1,
    LIGHT_GRAY = 2,
    WHITE = 3


class NbglBpp(IntEnum):
    BPP_1 = 0,
    BPP_2 = 1
    BPP_4 = 2


class NbglDirection(IntEnum):
    VERTICAL = 0,
    HORIZONTAL = 1


class NbglState(IntEnum):
    OFF_STATE = 0,
    ON_STATE = 1


class NbglQrCodeVersion(IntEnum):
    QRCODE_V4 = 0,
    QRCODE_V10 = 1


class NbglRadius(IntEnum):
    RADIUS_4_PIXELS = 0,
    RADIUS_8_PIXELS = 1,
    RADIUS_16_PIXELS = 2,
    RADIUS_20_PIXELS = 3,
    RADIUS_24_PIXELS = 4,
    RADIUS_32_PIXELS = 5,
    RADIUS_40_PIXELS = 6,
    RADIUS_48_PIXELS = 7,
    RADIUS_0_PIXELS = 0xFF,


class NbglKeyboardMode(IntEnum):
    MODE_LETTERS = 0
    MODE_DIGITS = 1,
    MODE_SPECIAL = 2


class NbglObjType(IntEnum):
    SCREEN,         = 0,  # Main screen
    CONTAINER,      = 1,  # Empty container
    PANEL,          = 2,  # Container with special border
    IMAGE,          = 3,  # Bitmap (x and width must be multiple of 4)
    LINE,           = 4,  # Vertical or Horizontal line
    TEXT_AREA,      = 5,  # Area to contain text line(s)
    BUTTON,         = 6,  # Rounded rectangle button with icon and/or text
    SWITCH,         = 7,  # Switch to turn on/off something
    PAGE_INDICATOR, = 8,  # horizontal bar to indicate navigation across pages
    # horizontal bar to indicate progression of something (between 0% and 100%)
    PROGRESS_BAR,   = 9,
    RADIO_BUTTON,   = 10,  # Indicator to inform whether something is on or off
    # Notification bar, to inform user about alarm... only for internal usage
    QR_CODE,        = 11,  # QR Code
    KEYBOARD,       = 12,  # Keyboard
    KEYPAD,         = 13,  # Keypad
    SPINNER,        = 14,  # Spinner
    IMAGE_FILE = 15,  # Image file (with Ledger compression)


class NbglAlignment(IntEnum):
    NO_ALIGNMENT = 0,
    TOP_LEFT = 1,
    TOP_MIDDLE = 2,
    TOP_RIGHT = 3,
    MID_LEFT = 4,
    CENTER = 5,
    MID_RIGHT = 6,
    BOTTOM_LEFT = 7,
    BOTTOM_MIDDLE = 8,
    BOTTOM_RIGHT = 9,
    LEFT_TOP = 10,
    LEFT_BOTTOM = 11,
    RIGHT_TOP = 12,
    RIGHT_BOTTOM = 13,


class NbglFontId(IntEnum):
    BAGL_FONT_INTER_REGULAR_24px = 0,
    BAGL_FONT_INTER_SEMIBOLD_24px = 1,
    BAGL_FONT_INTER_REGULAR_32px = 2,
    BAGL_FONT_HM_ALPHA_MONO_MEDIUM_32px = 3


class NbglStyle(IntEnum):
    NO_STYLE = 0,
    LEDGER_BORDER = 1


def parse_str(data: bytes) -> Tuple[str, int]:
    """
    Utility function to parse a NULL terminated string
    from input bytes. If the string is not terminated by NULL,
    take the truncated string instead.

    Returns the string and its size.
    """
    result = ""
    size = 0
    for b in data:
        size += 1
        if b == 0:
            break
        else:
            result += chr(b)
    return result, size


class NbglGenericJsonSerializable(ABC):
    """
    Base type for all NBGL objects.
    Implements generic json dict serialization / deserialization functions.
    """

    def to_json_dict(self) -> Dict:
        """
        Returns the json dict version of the object.
        """
        result = {}
        for name, val in self.__dict__.items():
            if issubclass(type(val), NbglObj):
                result[name] = val.to_json_dict()
            elif issubclass(type(val), Enum):
                result[name] = val.name
            else:
                result[name] = val
        return result

    @classmethod
    def from_json_dict(cls, data: Dict):
        """
        Get an instance of the class, from its json dict version.
        """
        fields = {}
        for field_name, field_obj in cls.__dataclass_fields__.items():
            field_type = field_obj.type
            if issubclass(field_type, Enum):
                fields[field_name] = field_type[data[field_name]]
            elif issubclass(field_type, NbglObj):
                fields[field_name] = NbglArea.from_json_dict(data[field_name])
            else:
                fields[field_name] = data[field_name]
        return cls(**fields)

    @abstractclassmethod
    def from_bytes(cls, data: bytes):
        """
        Get an instance of the class, from raw bytes.
        """
        pass


class NbglObj(NbglGenericJsonSerializable):
    pass

# Nbgl objects


@dataclass
class NbglArea(NbglObj):
    width: int
    height: int
    x0: int
    y0: int
    background_color: NbglColor
    bpp: NbglBpp

    @classmethod
    def from_bytes(cls, data: bytes):
        x0, y0, width, height, color_n, bpp_n = struct.unpack('>HHHHBB', data)
        color = NbglColor(color_n)
        bpp = NbglBpp(bpp_n)
        return cls(width, height, x0, y0, color, bpp)

    @staticmethod
    def size():
        return struct.calcsize('>HHHHBB')


@dataclass
class NbglContainer(NbglObj):
    area: NbglArea
    layout: NbglDirection
    nb_children: int
    force_clean: bool

    @classmethod
    def from_bytes(cls, data: bytes):
        area = NbglArea.from_bytes(data[0:NbglArea.size()])
        layout, nb_children, force_clean = struct.unpack(
            '>BB?', data[NbglArea.size():])
        return cls(
            area=area,
            layout=NbglDirection(layout),
            nb_children=nb_children,
            force_clean=force_clean
        )


@dataclass
class NbglLine(NbglObj):
    area: NbglArea
    direction: NbglDirection
    line_color: NbglColor
    thickness: int
    offset: int

    @classmethod
    def from_bytes(cls, data: bytes):
        area = NbglArea.from_bytes(data[0:NbglArea.size()])
        direction, line_color, thickness, offset = struct.unpack(
            '>BBBB', data[NbglArea.size():])
        return cls(
            area=area,
            direction=NbglDirection(direction),
            line_color=NbglColor(line_color),
            thickness=thickness,
            offset=offset)


@dataclass
class NbglRadioButton(NbglObj):
    area: NbglArea
    active_color: NbglColor
    border_color: NbglColor
    state: NbglState

    @classmethod
    def from_bytes(cls, data):
        area = NbglArea.from_bytes(data[0:NbglArea.size()])
        active_color, border_color, state = struct.unpack(
            '>BBB', data[NbglArea.size():])
        return cls(
            area=area,
            active_color=NbglColor(active_color),
            border_color=NbglColor(border_color),
            state=NbglState(state)
        )


@dataclass
class NbglSwitch(NbglObj):
    area: NbglArea
    on_color: NbglColor
    off_color: NbglColor
    state: NbglState

    @classmethod
    def from_bytes(cls, data):
        area = NbglArea.from_bytes(data[0:NbglArea.size()])
        on_color, off_color, state = struct.unpack(
            '>BBB', data[NbglArea.size():])
        return cls(
            area=area,
            on_color=NbglColor(on_color),
            off_color=NbglColor(off_color),
            state=NbglState(state)
        )


@dataclass
class NbglProgressBar(NbglObj):
    area: NbglArea
    with_border: bool
    state: int

    @classmethod
    def from_bytes(cls, data):
        area = NbglArea.from_bytes(data[0:NbglArea.size()])
        with_border, state = struct.unpack(
            '>?B', data[NbglArea.size():])
        return cls(
            area=area,
            with_border=with_border,
            state=state
        )


@dataclass
class NbglPageIndicator(NbglObj):
    area: NbglArea
    active_page: int
    nb_pages: int

    @classmethod
    def from_bytes(cls, data):
        area = NbglArea.from_bytes(data[0:NbglArea.size()])
        active_page, nb_pages = \
            struct.unpack('>BB', data[NbglArea.size():])
        return cls(
            area=area,
            active_page=active_page,
            nb_pages=nb_pages
        )


@dataclass
class NbglButton(NbglObj):
    area: NbglArea
    inner_color: NbglColor
    border_color: NbglColor
    foreground_color: NbglColor
    radius: NbglRadius
    font_id: NbglFontId
    localized: bool
    text: str

    @classmethod
    def from_bytes(cls, data):
        cnt = NbglArea.size()
        area = NbglArea.from_bytes(data[0:cnt])
        params_template = '>BBBBB?'
        params_size = struct.calcsize(params_template)
        inner_color, border_color, \
            foreground_color, \
            radius, font_id, localized = struct.unpack(
                params_template, data[cnt:cnt+params_size])

        cnt += params_size
        text, _ = parse_str(data[cnt:])

        return cls(
            area=area,
            inner_color=NbglColor(inner_color),
            border_color=NbglColor(border_color),
            foreground_color=NbglColor(foreground_color),
            radius=NbglRadius(radius),
            font_id=NbglFontId(font_id),
            localized=localized,
            text=text)


@dataclass
class NbglTextArea(NbglObj):
    area: NbglArea
    text_color: NbglColor
    text_alignment: NbglAlignment
    style: NbglStyle
    font_id: NbglFontId
    localized: bool
    auto_hide_long_line: bool
    text: str

    @classmethod
    def from_bytes(cls, data: bytes):
        # Parse area
        area = NbglArea.from_bytes(data[0:NbglArea.size()])
        cnt = NbglArea.size()

        # Parse pattern
        params_pattern = '>BBBB??'
        params_size = struct.calcsize(params_pattern)
        text_color, alignment, style, font_id, localized, auto_hide_long_line = struct.unpack(
            params_pattern, data[cnt:cnt+params_size]
        )
        cnt += params_size

        # Parse text
        text, _ = parse_str(data[cnt:])
        return cls(
            area=area,
            text_color=NbglColor(text_color),
            text_alignment=NbglAlignment(alignment),
            style=NbglStyle(style),
            font_id=NbglFontId(font_id),
            localized=localized,
            auto_hide_long_line=auto_hide_long_line,
            text=text
        )


@dataclass
class NbglSpinner(NbglObj):
    area: NbglArea
    position: int

    @classmethod
    def from_bytes(cls, data: bytes):
        area = NbglArea.from_bytes(data[0:NbglArea.size()])
        position, = struct.unpack(
            '>B', data[NbglArea.size():]
        )
        return cls(
            area=area,
            position=position
        )


@dataclass
class NbglImage(NbglObj):
    area: NbglArea
    foreground_color: NbglColor

    @classmethod
    def from_bytes(cls, data):
        area = NbglArea.from_bytes(data[0:NbglArea.size()])
        foreground_color, = struct.unpack('>B', data[NbglArea.size():])
        return cls(
            area=area,
            foreground_color=NbglColor(foreground_color)
        )


@dataclass
class NbglImageFile(NbglObj):
    area: NbglArea

    @classmethod
    def from_bytes(cls, data: bytes):
        area = NbglArea.from_bytes(data[0:])
        return cls(area)


@dataclass
class NbglQrCode(NbglObj):
    area: NbglArea
    foreground_color: NbglColor
    version: NbglQrCodeVersion
    text: str

    @classmethod
    def from_bytes(cls, data: bytes):
        # Parse area
        area = NbglArea.from_bytes(data[0:NbglArea.size()])
        cnt = NbglArea.size()

        # Parse QR code color and version
        foreground_color, version = struct.unpack('>BB', data[cnt: cnt + 2])
        cnt += 2

        # Parse text
        text, _ = parse_str(data[cnt:])

        # Return
        return cls(
            area=area,
            foreground_color=NbglColor(foreground_color),
            version=NbglQrCodeVersion(version),
            text=text)


@dataclass
class NbglKeyboard(NbglObj):
    area: NbglArea
    text_color: NbglColor
    border_color: NbglColor
    letters_only: bool
    upper_case: bool
    mode: NbglKeyboardMode
    key_mask: int

    @classmethod
    def from_bytes(cls, data: bytes):
        area = NbglArea.from_bytes(data[0:NbglArea.size()])
        text_color, border_color, letters_only, upper_case, mode, key_mask = struct.unpack(
            '>BBBBBI', data[NbglArea.size():])
        return cls(
            area=area,
            text_color=NbglColor(text_color),
            border_color=NbglColor(border_color),
            letters_only=letters_only,
            upper_case=upper_case,
            mode=NbglKeyboardMode(mode),
            key_mask=key_mask
        )


@dataclass
class NbglKeypad(NbglObj):
    area: NbglArea
    text_color: NbglColor
    border_color: NbglColor
    enable_backspace: bool
    enable_validate: bool

    @classmethod
    def from_bytes(cls, data: bytes):
        area = NbglArea.from_bytes(data[0:NbglArea.size()])
        text_color, border_color, enable_backspace, enable_validate = struct.unpack(
            '>BBBB', data[NbglArea.size():])
        return cls(
            area=area,
            text_color=NbglColor(text_color),
            border_color=NbglColor(border_color),
            enable_backspace=enable_backspace,
            enable_validate=enable_validate
        )


# Mapping of NbglObjType and their associated class.
NBGL_OBJ_TYPES = {
    NbglObjType.CONTAINER: NbglContainer,
    NbglObjType.LINE: NbglLine,
    NbglObjType.IMAGE: NbglImage,
    NbglObjType.IMAGE_FILE: NbglImageFile,
    NbglObjType.QR_CODE: NbglQrCode,
    NbglObjType.RADIO_BUTTON: NbglRadioButton,
    NbglObjType.TEXT_AREA: NbglTextArea,
    NbglObjType.BUTTON: NbglButton,
    NbglObjType.SWITCH: NbglSwitch,
    NbglObjType.PAGE_INDICATOR: NbglPageIndicator,
    NbglObjType.PROGRESS_BAR: NbglProgressBar,
    NbglObjType.KEYBOARD: NbglKeyboard,
    NbglObjType.KEYPAD: NbglKeypad,
    NbglObjType.SPINNER: NbglSpinner,
}

# Nbgl events


class NbglEventType(IntEnum):
    """
    Available serialized Nbgl events
    """
    NBGL_DRAW_OBJ = 0,
    NBGL_REFRESH_AREA = 1


@dataclass
class NbglRefreshAreaEvent(NbglGenericJsonSerializable):
    area: NbglArea

    @classmethod
    def from_bytes(cls, data: bytes):
        return cls(
            area=NbglArea.from_bytes(data)
        )


@dataclass
class NbglDrawObjectEvent(NbglGenericJsonSerializable):
    obj: NbglObj

    @classmethod
    def from_bytes(cls, data: bytes):
        obj_type = NbglObjType(data[0])
        class_type = NBGL_OBJ_TYPES[obj_type]

        return cls(
            obj=class_type.from_bytes(data[1:])
        )

    def to_json_dict(self) -> Dict:
        for obj_type, obj_class in NBGL_OBJ_TYPES.items():
            if obj_class == type(self.obj):
                return {
                    'obj': {
                        'type': obj_type.name,
                        'content': self.obj.to_json_dict()
                    }
                }
        # Object not serializable
        return None

    @classmethod
    def from_json_dict(cls, data: Dict):
        obj_data = data['obj']
        obj_type = NBGL_OBJ_TYPES[NbglObjType[obj_data['type']]]

        return cls(
            obj=obj_type.from_json_dict(obj_data['content'])
        )

# Public functions


NbglEvent = Union[
    NbglRefreshAreaEvent,
    NbglDrawObjectEvent
]


def deserialize_nbgl_bytes(data: bytes) -> NbglEvent:
    """
    Return a NbglRefreshAreaEvent or a NbglDrawObjectEvent,
    from input bytes.
    """
    event_type = NbglEventType(int(data[0]))

    if event_type == NbglEventType.NBGL_DRAW_OBJ:
        return NbglDrawObjectEvent.from_bytes(data[1:])
    elif event_type == NbglEventType.NBGL_REFRESH_AREA:
        return NbglRefreshAreaEvent.from_bytes(data[1:])


def deserialize_nbgl_json(data: Dict) -> NbglEvent:
    """
    Return a NbglRefreshAreaEvent or a NbglDrawObjectEvent,
    from input json-like dictionary.
    """
    event_type = NbglEventType[data['event']]

    if event_type == NbglEventType.NBGL_DRAW_OBJ:
        return NbglDrawObjectEvent.from_json_dict(data)
    elif event_type == NbglEventType.NBGL_REFRESH_AREA:
        return NbglRefreshAreaEvent.from_json_dict(data)


def serialize_nbgl_json(data: NbglEvent) -> Dict:
    """
    Return a json-like dictionary from
    input NbglRefreshAreaEvent / NbglDrawObjectEvent
    """
    EVENT_TYPES = {
        NbglRefreshAreaEvent: NbglEventType.NBGL_REFRESH_AREA,
        NbglDrawObjectEvent: NbglEventType.NBGL_DRAW_OBJ
    }

    result = {'event': EVENT_TYPES[type(data)].name}
    result.update(data.to_json_dict())
    return result
