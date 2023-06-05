#!/usr/bin/env python3
# coding: utf-8
"""
@BANNER@
"""
# -----------------------------------------------------------------------------
import argparse
import sys
import os
import json
import base64
import struct
import configparser
from PIL import Image, ImageFont, ImageDraw
from rle_custom import RLECustom

# -----------------------------------------------------------------------------
class TTF2INC:
    """
    This class will contain methods to handle TTF fonts & generate .inc files
    """
    # Default values:
    FONT_NAME = "open_sans_regular.ttf"
    FONT_SIZE = 11
    FIRST_CHARACTER = 0x20
    LAST_CHARACTER = 0x7E
    # Maximum possible bytes to skip when crop is False
    # (x_min_offset & y_min_offset contain the nb of skipped bytes on 6 bits)
    MAX_SKIPPED_BYTES = 63
    # Default string containing all supported unicode characters
    # Mandatory character
    STRING = "�"
    # Add French specific characters
    STRING += "àâäæçèéêëîïôœùûüÀÂÄÆÇÈÉÊËÎÏÔŒÙÛÜ"
    # Add Spanish specific characters (not already included)
    STRING += "¡¿ÁÍÑÓÚáíñóú"
    # Add German specific characters (not already included)
    STRING += "Ößö"
    # Add Portuguese specific characters (not already included)
    STRING += "ÃÕãõ"
    # Add Turkish specific characters (not already included)
    STRING += "İıĞğŞş₤"
    # Add Russian specific characters (not already included)
    STRING += "АаБбВвГгДдЕеЁёЖжЗзИиЙйКкЛлМмНнОоПпРрСсТтУуФфХхЦцЧчШшЩщЪъЫыЬьЭэЮюЯя"

    # -------------------------------------------------------------------------
    def __init__(self, args):
        super().__init__()
        # Default values:
        self.nbgl = False
        self.rle = False
        self.crop = True
        self.unicode_needed = False
        self.font_name = None
        self.font_id_name = None
        self.basename = None
        self.directory = None
        self.left = None
        self.right = None
        self.top = None
        self.bottom = None
        self.font_prefix = None
        self.height = 0
        self.line_size = 0
        self.char_leftmost_x = 0
        self.max_y_min_offset = 0
        self.max_x_max_offset = 0
        self.char_topmost_y = 0
        self.bitmap_len = 0
        self.loaded_baseline = 0
        self.baseline_offset = 0
        self.bpp = 1
        self.char_info = {}
        self.ttf_info_dictionary = {}
        # Be sure there are at least some mandatory characters
        self.unicode_chars = "�"
        self.ids = []
        # Store parameters:
        self.args = args
        # Initialise config with provided values or default ones
        self.init_config()
        # Get font infos
        try:
            self.font = ImageFont.truetype(self.font_name, self.font_size)
        except (BaseException, Exception) as error:
            sys.stderr.write(f"Error with font {self.font_name}: {error}\n")
            sys.exit(-1)

        # Get font metrics:
        # - ascent: distance from the baseline to the highest outline point.
        # - descent: distance from the baseline to the lowest outline point.
        # (descent is a negative value)
        self.ascent, self.descent = self.font.getmetrics()
        self.font_width = self.args.font_size
        self.font_height = self.font.font.height
        self.baseline = self.ascent
        self.char_leftmost_x = self.font_width
        self.char_topmost_y = self.font_height

    # -------------------------------------------------------------------------
    def __enter__(self):
        """
        Return an instance to this object.
        """
        return self

    # -------------------------------------------------------------------------
    def __exit__(self, exec_type, exec_value, traceback):
        """
        Do all the necessary cleanup.
        """

    # -------------------------------------------------------------------------
    def get_font_id_name(self):
        """
        Return the name of the font id.
        """
        # If we provided one, just return it
        if self.args.font_id_name:
            return self.args.font_id_name

        # Those ids are defined in bagl.h file, in bagl_font_id_e enums.
        font_id_name = "BAGL_FONT"
        font_name = self.font_name.lower()
        if "open" in font_name and "sans" in font_name:
            font_id_name += "_OPEN_SANS"
            if "regular" in font_name:
                font_id_name += "_REGULAR"
            elif "extrabold" in font_name:
                font_id_name += "_EXTRABOLD"
            elif "semibold" in font_name:
                font_id_name += "_SEMIBOLD"
            elif "light" in font_name:
                font_id_name += "_LIGHT"
        else:
            font_id_name += "_UNKNOWN"

        font_id_name += f"_{self.args.font_size}px"

        return font_id_name

    # -------------------------------------------------------------------------
    def init_config(self):
        """
        Initialise all important values using:
        - .ini file if we provided one
        - command line parameters
        - default values otherwise
        """
        # Read config from the .ini file if we provided one
        if self.args.init_file:
            config_parser = configparser.ConfigParser()
            config_parser.read(self.args.init_file)
            print(self.args.init_file)
            config = config_parser['main']
            self.font_name = config.get('font')
            self.font_id_name = config.get('font_id_name')
            self.loaded_baseline = int(config.get('loaded_baseline', 0))
            self.baseline_offset = int(config.get('baseline_offset', 0))
            self.first_character = int(config.get('firstChar', '0x20'), 16)
            self.last_character = int(config.get('lastChar', '0x7E'), 16)
            self.font_size = config.getint('fontSize')
            self.line_size = config.getint('lineSize')
            self.crop = config.getboolean('crop', False)
            self.rle = config.getboolean('rle', True)
            self.bpp = config.getint('bpp', 1)
            self.nbgl = config.getboolean('nbgl', False)
        # Otherwise, use command line parameters (or default values)
        else:
            self.font_name = self.args.font_name
            self.font_id_name = self.get_font_id_name()
            self.first_character = self.args.first_character
            self.last_character = self.args.last_character
            self.font_size = self.args.font_size

        if self.nbgl:
            self.font_prefix = "nbgl_font_"
        else:
            self.font_prefix = "bagl_font_"

        # Be sure there is a file at self.font_name
        self.find_font()

    # -------------------------------------------------------------------------
    def find_font(self):
        """
        Try to find where is located the font specified in self.font_name
        (in current directory, in the .ini directory or in the .py directory)
        """
        # Check first in current directory
        if os.path.exists(self.font_name):
            # We found it!!
            return

        # Check at .ini location
        if self.args.init_file:
            filename = os.path.dirname(self.args.init_file)
            filename = os.path.join(filename, self.font_name)
            if os.path.exists(filename):
                # We found it!!
                self.font_name = filename
                return

        # Check at the location of the script
        filename = os.path.dirname(__file__)
        filename = os.path.join(filename, self.font_name)
        if os.path.exists(filename):
            # We found it!!
            self.font_name = filename
            return

        # We didn't found the specified font... :(
        # => let pillow search by itself (it looks in system directories too)

    # -------------------------------------------------------------------------
    def build_names(self):
        """
        Build filenames depending on font characteristics.
        """
        # Get the font basename, without extension
        self.basename = os.path.splitext(
            os.path.basename(self.font_name))[0]
        self.basename = self.basename.replace("-", "_")
        self.basename += f"_{self.font_size}px"
        if self.nbgl and self.bpp != 4:
            self.basename += f"_{self.bpp}bpp"
        if self.unicode_needed:
            self.basename += "_unicode"
        # Build destination directory name, based on font+size[+unicode]+bpp
        self.directory = os.path.dirname(self.font_name)
        name = self.font_prefix + self.basename
        self.directory = os.path.join(self.directory, name)
        # Create that directory if it doesn't exist
        if not os.path.exists(self.directory) and self.args.save:
            if self.args.verbose:
                sys.stdout.write(f"Creating directory {self.directory}\n")
            os.mkdir(self.directory)

    # -------------------------------------------------------------------------
    def align_top(self, top):
        """
        Handle Stax constraint on Y (should be modulo 4), top side.
        (decrease Y until it is modulo 4)
        """
        if self.nbgl:
            remaining = top % 4
            if remaining:
                top -= remaining

        return top

    # -------------------------------------------------------------------------
    def align_bottom(self, bottom):
        """
        Handle Stax constraint on Y (should be modulo 4), bottom side.
        (increase Y until it is modulo 4)
        """
        if self.nbgl:
            remaining = bottom % 4
            if remaining:
                bottom += 4 - remaining

        return bottom

    # -------------------------------------------------------------------------
    def check_unicode(self, char):
        """
        Check if unicode is needed to encode provided character.
        """
        if ord(char) > 0x7F:
            self.unicode_needed = True

    # -------------------------------------------------------------------------
    def find_string_dimension(self, string):
        """
        Find the real dimension needed by all the characters in string.
        Update the unicode_needed flag and build filenames.
        """
        for char in string:

            # Update Unicode flag
            self.check_unicode(char)

            # Get dimension using font.getbbox
            left, top, right, bottom = self.font.getbbox(char)

            # Some characters (ie 'j' & '_') may need to be displayed at x < 0
            # As we display char by char, we can't modify previous one...
            # (there is no real transparency support on current devices)
            if left < 0:
                left_offset = left
                left = 0
            else:
                left_offset = 0

            # To be able to check if some y are < 0
            if top < self.char_topmost_y:
                self.char_topmost_y = top

            # Store relevant information
            self.char_info[char] = {
                "left_offset": left_offset,
                "left": left,
                "top": top,
                "right": right,
                "bottom": bottom
            }
            if self.args.verbose:
                sys.stderr.write(f"char (font): {char} ({ord(char):06X}) "
                                 f"left_offset={left_offset:2}, left={left:2}"
                                 f",right={right:2} "
                                 f"top= {top:2}, bottom={bottom}\n")

        # Compute value of max_x_max_offset, depending of allowed bits
        if self.nbgl:
            if self.unicode_needed:
                max_bits = 5
            else:
                max_bits = 2
        else:
            if self.unicode_needed:
                max_bits = 5
            else:
                max_bits = 4
        self.max_x_max_offset = pow(2, max_bits) - 1

        # Compute value of max_y_min_offset, depending of allowed bits
        if self.nbgl:
            if self.unicode_needed:
                max_bits = 4
            else:
                max_bits = 3
        else:
            if self.unicode_needed:
                max_bits = 6
            else:
                max_bits = 5
        self.max_y_min_offset = 4 * (pow(2, max_bits) - 1)

        # Some fonts display some characters at y < 0
        # => Be sure we are aware of that and already compensated it
        if (self.baseline_offset + self.char_topmost_y) < 0:
            sys.stderr.write(f"WARNING: Font {self.font_name} ({self.bpp} BPP)"
                             f" display characters at y={self.char_topmost_y} "
                             f"=> 'baseline_offset' value should be set "
                             f"at least to {-(self.char_topmost_y)} !\n")
            # Increase font height accordingly
            self.font_height -= self.baseline_offset + self.char_topmost_y
        else:
            self.font_height += self.baseline_offset

        # char_topmost_y will use the real information (from bitmap data)
        self.char_topmost_y = self.font_height

        # We now know everything on this font & used chars => build filenames
        self.build_names()

    # -------------------------------------------------------------------------
    def create_empty_image(self, width, height):
        """
        Create an image using font characteristics and provided dimensions.
        """
        background_color = 'black'
        text_color = 'white'
        mode = "1"
        if self.bpp != 1:
            mode = "L"
            if self.nbgl:
                background_color = 'white'
                text_color = 'black'
        img = Image.new(mode, (width, height), color=background_color)

        return img, text_color

    # -------------------------------------------------------------------------
    def get_char_image(self, char):
        """
        Load or generate the image corresponding to the provided character(s).
        """
        # Get the unicode value of that char:
        # (in fact there is a plane value from 0 to 16 then a 16-bit code)
        unicode_value = f"0x{ord(char):06X}"

        # Build full path of the destination file:
        filename = f"{self.font_prefix}{self.basename}_{unicode_value}"
        fullpath = os.path.join(self.directory, filename)

        # We'll look for .bmp, .png and .gif extensions to check if file exists
        for ext in [".bmp", ".png", ".gif"]:
            image_name = fullpath + ext
            if os.path.exists(image_name):
                break

        info = self.char_info[char]
        # STEP 1: Generate the image if it doesn't exist
        if not os.path.exists(image_name) or self.args.overwrite:
            # To keep compatibility with 'genuine' generated images, picture
            # will have the height of the font and use font.getbbox sizes
            # Build an image with font height & taking in account left_offset
            width = info['right']
            if info['left_offset'] != 0:
                # Increase width by the number of negative pixels
                width -= info['left_offset']
                # Update right coordinate
                info['right'] = width

            height = self.align_bottom(self.font_height)

            # Generate and save the picture
            # Create a B&W (or grey levels) image with that size
            img, text_color = self.create_empty_image(width, height)
            draw = ImageDraw.Draw(img)
            # Compute display coordinates, using real size of the character
            # (no pixel should be displayed outside of [width, height])
            offset_x = 0 - info['left_offset']
            offset_y = self.baseline_offset
            offset_y += self.font_height - self.font.font.height

            # Draw the character
            draw.text((offset_x, offset_y), char, font=self.font,
                      fill=text_color)
            # Save the picture if we asked to
            if self.args.save:
                image_name = change_ext(image_name, ".png")
                #sys.stdout.write(f"Saving {image_name} ({width}x{height})\n")
                img.save(image_name)
        else:
            # We will load the existing picture
            #if self.args.verbose:
            #    sys.stdout.write(f"Loading {image_name}\n")
            img = Image.open(image_name)
            # Be sure it is a B&W or grey levels picture:
            mode = "1"
            if self.bpp != 1:
                mode = "L"
            if img.mode != mode:
                # Convert the picture (ImageMagick may obtain a better quality)
                img = img.convert(mode)

            # Make sure the loaded picture have same characteristics than
            # the ones already used for the font (height, baseline etc)
            # => char bitmap should be displayed at the correct coordinates.
            width, height = img.size
            if height < self.align_bottom(self.font_height) or \
               self.loaded_baseline != 0 or self.baseline_offset != 0:
                height = self.align_bottom(self.font_height)
                # Create a new picture with expanded height
                new_img, _ = self.create_empty_image(width, height)
                # Integrate baseline (take in account baseline differences)
                if self.loaded_baseline != 0:
                    offset_y = self.baseline - self.loaded_baseline
                else:
                    offset_y = 0
                offset_y += self.baseline_offset
                new_img.paste(img, (0, offset_y))
                img = new_img

            # Save the picture if we asked to
            if self.args.save:
                image_name = change_ext(image_name, ".png")
                #sys.stdout.write(f"Saving {image_name} ({width}x{height})\n")
                img.save(image_name)

            # Let suppose picture correctly takes in account left_offset
            # (if it was generated using checkpics.py script, it is the case)
            # Keep width but check other real coordinates from the bounding box
            box = self.getbbox(img)
            if box is None:
                left = 0
                top = 0
                bottom = height
            else:
                left, top, _, bottom = box

            info['left_offset'] = 0
            info['left'] = left
            info['top'] = top
            info['right'] = width       # right is equal to width for the moment
            info['bottom'] = bottom

        # Update the dictionary containing character information
        info['img'] = img
        info['width'] = width
        info['height'] = height
        self.char_info[char] = info

    # -------------------------------------------------------------------------
    def box_to_packed_bitmap(self, img, left, top, right, bottom):
        """
        Store the pixel values of the specified box, in little endian encoding.
        (left pixels are stored starting at bit 0, grouped by bytes)
        """
        current_byte = 0
        current_bit = 0
        image_data = []

        if self.nbgl:
            nb_colors = pow(2, self.bpp)
            base_threshold = int(256 / nb_colors)
            half_threshold = int(base_threshold / 2)
            # Rotate & revert X axes on Stax
            for coord_x in reversed(range(left, right)):
                for coord_y in range(top, bottom):
                    color_index = img.getpixel((coord_x, coord_y))
                    color_index = \
                        int((color_index + half_threshold) / base_threshold)

                    if color_index >= nb_colors:
                        color_index = nb_colors - 1

                    # le encoded
                    current_byte += color_index << ((8-self.bpp)-current_bit)
                    current_bit += self.bpp

                    if current_bit >= 8:
                        image_data.append(current_byte & 0xFF)
                        current_bit = 0
                        current_byte = 0
        else:
            # bottom is actually Y2+1 => perfect for range scan
            for coord_y in range(top, bottom):
                # right is actually X2+1 => perfect for range scan
                for coord_x in range(left, right):
                    color_index = img.getpixel((coord_x, coord_y))
                    if color_index >= 128:
                        color_index = 1
                    else:
                        color_index = 0

                    # le encoded
                    current_byte += color_index << current_bit
                    current_bit += 1

                    if current_bit >= 8:
                        image_data.append(current_byte & 0xFF)
                        current_bit = 0
                        current_byte = 0

        # Handle last byte if any
        if current_bit > 0:
            image_data.append(current_byte & 0xFF)

        # Remove final transparent pixels, if any
        background_color = 0
        if self.nbgl and self.bpp != 1:
            background_color = 0xFF

        while len(image_data) != 0 and image_data[-1] == background_color:
            image_data = image_data[:-1]

        # When crop is False, we can crop at bytes level
        skipped_bytes = 0
        if not self.crop:
            while len(image_data) != 0 and \
                  image_data[0] == background_color and \
                  skipped_bytes < self.MAX_SKIPPED_BYTES:
                image_data = image_data[1:]
                skipped_bytes += 1

        return skipped_bytes, bytes(image_data)

    # -------------------------------------------------------------------------
    def getbbox(self, img):
        """
        Return a 4-tuple defining the left, upper, right, and lower pixel
        coordinate, by taking in account the transparent color.
        (unlike what does Image.getbbox() method from Pillow)
        """
        width, height = img.size
        left = width
        top = height
        right = 0
        bottom = 0
        # Find transparent color (not 0 on Stax with 4BPP)
        background_color = 0
        if self.nbgl and self.bpp != 1:
            background_color = 0xFF

        # Find left, top, right and bottom values for existing pixels
        for y_coord in range(height):
            for x_coord in range(width):
                if img.getpixel((x_coord, y_coord)) == background_color:
                    # This pixel is transparent => ignore it
                    continue

                if x_coord < left:
                    left = x_coord
                if x_coord >= right:
                    right = x_coord+1 # Right is X2+1 (right - left = width)

                if y_coord < top:
                    top = y_coord
                if y_coord >= bottom:
                    bottom = y_coord+1 # Bottom is X2+1 (bottom - top = height)

        # Is it an empty box?
        if right <= left or bottom <= top:
            return None

        # Handle Stax constraint on Y (should be modulo 4)
        top = self.align_top(top)
        bottom = self.align_bottom(bottom)

        # Return the box coordinates in a 4-tuple
        return (left, top, right, bottom)

    # -------------------------------------------------------------------------
    def get_char_bitmap(self, char):
        """
        Return the packed bitmap corresponding to the provided character(s).
        """
        # Get the image that was generated or loaded from an existing picture
        info = self.char_info[char]
        img = info['img']

        # Get the bounding box (non-transparent region) of img
        box = self.getbbox(img)

        # Is it an empty box?
        if box is None:
            right = 0
            left = 0
            top = 0
            bottom = 0
            height = 0
        else:
            if self.crop:
                left, top, right, bottom = box
                if False and not self.nbgl:
                    # Don't modify right on BAGL (char spacing)
                    right = info['right']
            else:
                left = 0
                top = 0
                right = info['right']
                bottom = self.align_bottom(self.font_height)

            height = bottom - top

            # Are some values too big to fit in a small bitfield?
            # If yes, add transparent pixels to make the value fit
            if self.crop:
                # Be sure y_min_offset value is not too big
                if top > self.max_y_min_offset:
                    #sys.stderr.write(f"Adjusting top from {top} to "
                    #                 f"{self.max_y_min_offset}"
                    #                 f" for char {char} ({ord(char):06X})\n")
                    # Compression will compensate those added transparent lines
                    top = self.max_y_min_offset

                # Be sure difference between right & width is not too big
                if (info['width'] - right) > self.max_x_max_offset:
                    #sys.stderr.write(f"Adjusting right from {right} to "
                    #                 f"{info['width'] - self.max_x_max_offset}"
                    #                 f" for char {char} ({ord(char):06X})\n")
                    # Compression will compensate those added 'empty' pixels
                    right = info['width'] - self.max_x_max_offset

            # Update offsets used by all chars in this font
            if left < self.char_leftmost_x:
                self.char_leftmost_x = left

            if bottom > self.height:
                self.height = bottom

            if top < self.char_topmost_y:
                self.char_topmost_y = top

        # Store the information to process it later
        info['left'] = left
        info['top'] = top
        info['right'] = right
        info['bottom'] = bottom
        info['height'] = height

        # Get the packed bitmap corresponding to this box
        if height:
            skipped_bytes, img_data = self.box_to_packed_bitmap(
                img, left, top, right, bottom)
            size = len(img_data)
        else:
            img_data = None
            skipped_bytes = 0
            size = 0

        # Number of bytes that was skipped at the beginning of data
        info['skipped'] = skipped_bytes

        encoding = 0
        # Do we want to compress this font?
        if self.rle and img_data is not None:
            method, compressed_data = RLECustom.encode(img_data, self.bpp)
            # Is compressed size really better?
            # (for the moment, we enforce RLE even if it is a little bigger)
            if len(compressed_data) < len(img_data) or True:
                img_data = compressed_data
                size = len(img_data)
                encoding = method

        info['encoding'] = encoding
        info['bitmap'] = img_data
        info['size'] = size
        info['offset'] = 0

        self.char_info[char] = info

        if self.args.verbose:
            sys.stderr.write(f"char (img): {char} ({ord(char):06X}) "\
                             f"width:{info['width']:2}, height:{height:2} "\
                             f"left={left:2}, right={right:2} "\
                             f"top={top:2}, bottom={bottom:2} "\
                             f"datasize={size}\n")

    # -------------------------------------------------------------------------
    def check_max_bits(self, value, max_bits, char, name):
        """
        Check if a provided value exceeds the number of bits allowed.
        """
        if value >= pow(2, max_bits):
            sys.stderr.write(f"Field '{name}' for char 0x{ord(char):X}({char})"
                             f" needs too much bits: value is 0x{value:X} and"
                             f" {max_bits} bits are allowed!\n")
            sys.stderr.write(f"Font ID: {self.font_id_name}")
            sys.exit(3)


    # -------------------------------------------------------------------------
    def save_nbgl_font(self, inc, crop, suffix, first_char, last_char):
        """
        Save the nbgl_font_t info into the .inc file.
        Structure defined in public_sdk/lib_nbgl/include/nbgl_fonts.h
        typedef struct {
          uint32_t bitmap_len;      ///< Size in bytes of the associated bitmap
          uint8_t font_id;          ///< ID of the font, from @ref nbgl_font_id_e
          uint8_t bpp;              ///< number of bits per pixels
          uint8_t height;           ///< height of all characters in pixels
          uint8_t line_height;      ///< height of a line for all characters in pixels
          uint8_t crop;             ///< If false, x_min_offset+y_min_offset=bytes to skip
          uint8_t y_min;            ///< Most top Y coordinate of any char in the font
          uint8_t first_char;       ///< ASCII code of the first character
          uint8_t last_char;        ///< ASCII code of the last character
          const nbgl_font_character_t *const characters; ///< array containing definitions of all characters
          uint8_t const *bitmap;    ///< array containing bitmaps of all characters
        } nbgl_font_t;
        """
        inc.write(
            "\n__attribute__ ((section(\"._nbgl_fonts_\"))) const "
            f"nbgl_font_t font{self.basename.upper()}{suffix}"
            f"= {{\n"
            f"  {self.bitmap_len}, // bitmap len\n"
            f"  {self.font_id_name}, // font id\n"
            f"  (uint8_t) NBGL_BPP_{self.bpp}, // bpp\n"
            f"  {self.height}, // height of all characters in pixels\n"
            f"  {self.line_size}, // line height in pixels\n"
            f"  {crop}, // crop enabled (1) or not (0)\n"
            f"  {self.char_topmost_y}, // Most top Y coordinate of any char\n"
            f"  0x{first_char:X}, // first character\n"
            f"  0x{last_char:X}, // last character\n"
            f"  characters{self.basename.upper()},\n"
            f"  bitmap{self.basename.upper()}\n")

    # -------------------------------------------------------------------------
    def save_nbgl_font_character(self, inc, char, info):
        """
        Save the nbgl_font_character_t info into the .inc file, but first
        check that the values do not exceed the boundaries of each fieds.
        Structure defined in public_sdk/lib_nbgl/include/nbgl_fonts.h
        typedef struct {
          uint32_t encoding:1;        ///< method used to encode bitmap data
          uint32_t bitmap_offset:14;  ///< offset of this character in chars buffer
          uint32_t width:6;           ///< width of character in pixels
          uint32_t x_min_offset:3;    ///< x_min = x_min_offset
          uint32_t y_min_offset:3;    ///< y_min = (y_min + y_min_offset) * 4
          uint32_t x_max_offset:2;    ///< x_max = width - x_max_offset
          uint32_t y_max_offset:3;    ///< y_max = (height - y_max_offset) * 4
        } nbgl_font_character_t;
        """
        encoding = info["encoding"]
        bitmap_offset = info["offset"]
        width = info["width"]

        # If it's an empty box, just put everything at 0
        if info["bitmap"] is None or len(info["bitmap"]) == 0:
            x_min_offset = 0
            y_min_offset = 0

            x_max_offset = 0
            y_max_offset = 0
        else:
            x_min_offset = info["left"]
            y_min_offset = info["top"] - self.char_topmost_y
            y_min_offset = y_min_offset // 4

            x_max_offset = width - info["right"]
            y_max_offset = self.height - info["bottom"]
            y_max_offset = y_max_offset // 4

        # When crop is False, we may have some bytes to skip at beginning
        if not self.crop:
            skipped_bytes = info["skipped"]
            self.check_max_bits(skipped_bytes, 6, char, "skipped bytes")
            # We'll store 6 bits of skipped bytes in x_min_offset & y_min_offset
            x_min_offset = (skipped_bytes >> 3) & 7
            y_min_offset = skipped_bytes & 7

        # Check values does not exceed bitfield capabilities
        self.check_max_bits(encoding, 1, char, "encoding")
        self.check_max_bits(bitmap_offset, 14, char, "bitmap_offset")
        self.check_max_bits(width, 6, char, "width")
        self.check_max_bits(x_min_offset, 3, char, "x_min_offset")
        self.check_max_bits(y_min_offset, 3, char, "y_min_offset")
        # Next one should never occur, thanks to max_x_max_offset check
        self.check_max_bits(x_max_offset, 2, char, "x_max_offset")
        self.check_max_bits(y_max_offset, 3, char, "y_max_offset")

        inc.write(f"  {{ {encoding:1}, {bitmap_offset:5}, {width:2},"
                  f"{x_min_offset}, {y_min_offset}, "
                  f"{x_max_offset}, {y_max_offset} }},"
                  f" //asciii 0x{ord(char):04X}\n")

    # -------------------------------------------------------------------------
    def save_nbgl_font_unicode(self, inc, crop, suffix):
        """
        Save the nbgl_unicode_font_t info into the .inc file.
        Structure defined in public_sdk/lib_nbgl/include/nbgl_fonts.h
        typedef struct {
          uint8_t   font_id;            ///< ID of the font, from @ref nbgl_font_id_e
          uint8_t   bpp;                ///< Number of bits/pixels, (nbgl_bpp_t)
          uint8_t   height;             ///< height of all characters in pixels
          uint8_t   line_height;        ///< height of a line for all characters in pixels
          uint8_t   crop;               ///< If false, x_min_offset+y_min_offset=bytes to skip
          uint8_t   y_min;              ///< Most top Y coordinate of any char in the font
          uint16_t  nb_characters;      ///< Number of characters in this font
        } nbgl_font_unicode_t;
        """
        inc.write(
            "\n__attribute__ ((section(\"._nbgl_fonts_\"))) const "
            f"nbgl_font_unicode_t font{self.basename.upper()}{suffix}"
            f"= {{\n"
            f"  {self.font_id_name}, // font id\n"
            f"  (uint8_t) NBGL_BPP_{self.bpp}, // bpp\n"
            f"  {self.height}, // height of all characters in pixels\n"
            f"  {self.line_size}, // line height in pixels\n"
            f"  {crop}, // crop enabled (1) or not (0)\n"
            f"  {self.char_topmost_y}, // Most top Y coordinate of any char\n"
            f"  {len(self.char_info)}, // Nb of characters\n")
        if not suffix:
            inc.write(f"  characters{self.basename.upper()},\n")
            inc.write(f"  bitmap{self.basename.upper()}\n")

    # -------------------------------------------------------------------------
    def save_nbgl_font_unicode_character(self, inc, char, info):
        """
        Save the nbgl_unicode_font_character_t info into the .inc file, but first
        check that the values do not exceed the boundaries of each fieds.
        Structure defined in public_sdk/lib_nbgl/include/nbgl_fonts.h
        typedef struct {
          uint32_t  char_unicode;     ///< unicode = plane value from 0 to 16 then 16-bit code.
          uint16_t  bitmap_byte_count;///< number of bytes used in chars buffer for this character
          uint16_t  bitmap_offset;    ///< offset of this character in chars buffer
          uint8_t   width;            ///< width of character in pixels
          uint8_t   x_min_offset;     ///< x_min = x_min_offset
          uint8_t   y_min_offset;     ///< y_min = (y_min + y_min_offset) * 4
          uint8_t   x_max_offset;     ///< x_max = width - x_max_offset
          uint8_t   y_max_offset;     ///< y_max = (height - y_max_offset) * 4
          uint8_t   encoding;         ///< method used to encode bitmap data
        } nbgl_font_unicode_character_t;
        """
        char_unicode = ord(char)
        bitmap_byte_count = info["size"]
        encoding = info["encoding"]
        bitmap_offset = info["offset"]
        width = info["width"]

        # If it's an empty box, just put everything at 0
        if info["bitmap"] is None or len(info["bitmap"]) == 0:
            x_min_offset = 0
            y_min_offset = 0

            x_max_offset = 0
            y_max_offset = 0
        else:
            x_min_offset = info["left"]
            y_min_offset = info["top"] - self.char_topmost_y
            y_min_offset = y_min_offset // 4

            x_max_offset = width - info["right"]
            y_max_offset = self.height - info["bottom"]
            y_max_offset = y_max_offset // 4

        # When crop is False, we may have some bytes to skip at beginning
        if not self.crop:
            skipped_bytes = info["skipped"]
            self.check_max_bits(skipped_bytes, 6, char, "skipped bytes")
            # We'll store 6 bits of skipped bytes in x_min_offset & y_min_offset
            x_min_offset = (skipped_bytes >> 3) & 7
            y_min_offset = skipped_bytes & 7

        # Check maximum values
        self.check_max_bits(char_unicode, 21, char, "char_unicode")
        self.check_max_bits(bitmap_byte_count, 16, char, "bitmap_byte_count")
        self.check_max_bits(bitmap_offset, 16, char, "bitmap_offset")
        self.check_max_bits(width, 8, char, "width")
        self.check_max_bits(x_min_offset, 8, char, "x_min_offset")
        self.check_max_bits(y_min_offset, 8, char, "y_min_offset")
        # Next one should never occur, thanks to max_x_max_offset check
        self.check_max_bits(x_max_offset, 8, char, "x_max_offset")
        self.check_max_bits(y_max_offset, 8, char, "y_max_offset")
        self.check_max_bits(encoding, 1, char, "encoding")

        unicode = f"0x{ord(char):06X}"
        self.ttf_info_dictionary["nbgl_font_unicode_character"].append({
            "char_unicode": ord(char),
            "bitmap_byte_count": bitmap_byte_count,
            "bitmap_offset": bitmap_offset,
            "width": width,
            "x_min_offset": x_min_offset,
            "y_min_offset": y_min_offset,
            "x_max_offset": x_max_offset,
            "y_max_offset": y_max_offset,
            "encoding": encoding
        })
        inc.write(f"  {{ 0x{ord(char):06X}, {bitmap_byte_count:3},"
                  f" {bitmap_offset:5}, {width:2},"
                  f" {x_min_offset:2}, {y_min_offset:2},"
                  f" {x_max_offset:2}, {y_max_offset:2}, {encoding} }}, "
                  f"//unicode {unicode}\n")

    # -------------------------------------------------------------------------
    def save_bagl_font(self, inc, suffix, first_char, last_char):
        """
        Save the bagl_font_t info into the .inc file.
        Structure defined in public_sdk/lib_bagl/include/bagl.h
        typedef struct {
          uint16_t bitmap_len;      // Size in bytes of all characters bitmaps
          uint8_t font_id;          // to allow for sparse font embedding with a linear enum
          uint8_t bpp;              // for antialiased fonts (blue?)
          uint8_t height;           // Does already contain the nb of skipped lines
          uint8_t baseline;         // Does already contain the nb of skipped lines
          uint8_t first_char;
          uint8_t last_char;
          const bagl_font_character_t * const characters;
          unsigned char const * bitmap; // single bitmap for all chars of a font
        } bagl_font_t;
        """
        height = self.height - self.char_topmost_y
        baseline = self.baseline - self.char_topmost_y
        inc.write(
            f"\nconst bagl_font_t font{self.basename.upper()}{suffix}"
            f" = {{\n"
            f"  {self.bitmap_len}, // bitmap len\n"
            f"  {self.font_id_name}, // font id\n"
            f"  {self.bpp}, // bpp => 1 for B&W\n"
            f"  {height}, // height (does already contain the nb of skipped "
            "lines)\n"
            f"  {baseline}, // baseline (does already contain the nb of "
            "skipped lines)\n"
            f"  0x{first_char:X}, // first character\n"
            f"  0x{last_char:X}, // last character\n")
        if not suffix:
            inc.write(f"  characters{self.basename.upper()},\n")
            inc.write(f"  bitmap{self.basename.upper()}\n")

    # -------------------------------------------------------------------------
    def save_bagl_font_character(self, inc, char, info):
        """
        Save the bagl_font_character_t info into the .inc file, but first
        check that the values do not exceed the boundaries of each fieds.
        Structure defined in public_sdk/lib_bagl/include/bagl.h
        typedef struct {
          uint32_t encoding:2;
          uint32_t bitmap_offset:12;
          uint32_t width:5;
          uint32_t x_min_offset:4;
          uint32_t y_min_offset:5;
          uint32_t x_max_offset:4;
        } bagl_font_character_t;
        """
        width = info["width"]

        # If it's an empty box, just put everything at 0
        if info["bitmap"] is None or len(info["bitmap"]) == 0:
            x_min_offset = 0
            y_min_offset = 0
            x_max_offset = 0
        else:
            x_min_offset = info["left"]
            x_max_offset = width - info["right"]
            y_min_offset = info["top"] - self.char_topmost_y

        offset = info["offset"]
        encoding = info["encoding"]

        # Check maximum values
        self.check_max_bits(encoding, 2, char, "encoding")
        self.check_max_bits(offset, 12, char, "bitmap_offset")
        self.check_max_bits(width, 5, char, "width")
        self.check_max_bits(x_min_offset, 4, char, "x_min_offset")
        self.check_max_bits(y_min_offset, 5, char, "y_min_offset")
        self.check_max_bits(x_max_offset, 4, char, "x_max_offset")

        inc.write(f"  {{ {encoding:1}, {offset:5}, {width:2},"
                  f"{x_min_offset}, {y_min_offset}, {x_max_offset} }},"
                  f" //asciii 0x{ord(char):04X}\n")

    # -------------------------------------------------------------------------
    def save_bagl_font_unicode(self, inc, suffix):
        """
        Save the bagl_font_unicode_t info into the .inc file.
        Structure defined in public_sdk/lib_bagl/include/bagl.h
        typedef struct {
          uint16_t  bitmap_len;       // Size in bytes of all characters bitmaps
          uint8_t   font_id;
          uint8_t   baseline;         // Does already contain the nb of skipped lines
        #if !defined(HAVE_LANGUAGE_PACK)
          // When using language packs, those 2 pointers does not exists
          const bagl_font_unicode_character_t * const characters;
          unsigned char const * bitmap; // single bitmap for all chars of a font
        #endif //!defined(HAVE_LANGUAGE_PACK)
        } bagl_font_unicode_t;
        """
        baseline = self.baseline - self.char_topmost_y
        inc.write(
            f"\nconst bagl_font_unicode_t font{self.basename.upper()}{suffix}"
            f" = {{\n"
            f"  {self.bitmap_len}, // bitmap len\n"
            f"  {self.font_id_name}, // font id\n"
            f"  {baseline}, // baseline (does already contain the nb of "
            "skipped lines)\n")
        if not suffix:
            inc.write(f"  characters{self.basename.upper()},\n")
            inc.write(f"  bitmap{self.basename.upper()}\n")

    # -------------------------------------------------------------------------
    def save_bagl_font_unicode_character(self, inc, char, info):
        """
        Save the bagl_font_character_t info into the .inc file, but first
        check that the values do not exceed the boundaries of each fieds.
        Structure defined in public_sdk/lib_bagl/include/bagl.h
        typedef struct {
          uint32_t  char_unicode:21;  // plane value from 0 to 16 then 16-bit code.
          uint32_t  width:6;
          uint32_t  x_min_offset:5;   //  x_min = x_min_offset
          uint32_t  y_min_offset:6;   //  Does already contain the nb of skipped lines
          uint32_t  x_max_offset:5;   //  x_max = width - x_max_offset
          uint32_t  encoding:2;       //  method used to encode bitmap data
          uint32_t  bitmap_offset:19;
        } bagl_font_unicode_character_t;
        """
        width = info["width"]

        # If it's an empty box, just put everything at 0
        if info["bitmap"] is None or len(info["bitmap"]) == 0:
            x_min_offset = 0
            y_min_offset = 0
            x_max_offset = 0
        else:
            x_min_offset = info["left"]
            x_max_offset = width - info["right"]
            y_min_offset = info["top"] - self.char_topmost_y

        offset = info["offset"]
        encoding = info["encoding"]

        # Check maximum values
        val = ord(char)
        self.check_max_bits(val, 21, char, "char_unicode")
        self.check_max_bits(width, 6, char, "width")
        self.check_max_bits(x_min_offset, 5, char, "x_min_offset")
        self.check_max_bits(y_min_offset, 6, char, "y_min_offset")
        self.check_max_bits(x_max_offset, 5, char, "x_max_offset")
        self.check_max_bits(encoding, 2, char, "encoding")
        self.check_max_bits(offset, 19, char, "bitmap_offset")

        unicode = f"0x{ord(char):06X}"
        self.ttf_info_dictionary["bagl_font_unicode_character"].append({
            "char_unicode": ord(char),
            "width": width,
            "x_min_offset": x_min_offset,
            "x_max_offset": x_max_offset,
            "y_min_offset": y_min_offset,
            "bitmap_offset": offset,
            "encoding": encoding
        })

        inc.write(f"  {{ 0x{ord(char):06X}, {width:3}, "\
                  f"{x_min_offset:2}, {y_min_offset:2},"\
                  f"{x_max_offset:2}, {encoding:1}, "\
                  f"{offset:5} }}, //unicode {unicode}\n")

    # -------------------------------------------------------------------------
    @staticmethod
    def swap32(data):
        """
        Transform a Big Endian value into Little Endian one, and vice versa.
        """
        if isinstance(data, bytes):
            return [struct.unpack("<I", struct.pack(">I", i))[0] for i in data]

        result = (data & 0x000000FF) << 24
        result += (data & 0x0000FF00) << 8
        result += (data & 0x00FF0000) >> 8
        result += (data >> 24) & 0xFF
        return result

    # -------------------------------------------------------------------------
    def get_font_id(self):
        """
        Return the font id.
        """
        # Those ids are defined in bagl.h file, in bagl_font_id_e enums.
        bagl_font_ids={
            "BAGL_FONT_LUCIDA_CONSOLE_8PX": 0,
            "BAGL_FONT_OPEN_SANS_LIGHT_16_22PX": 1,
            "BAGL_FONT_OPEN_SANS_REGULAR_8_11PX": 2,
            "BAGL_FONT_OPEN_SANS_REGULAR_10_13PX": 3,
            "BAGL_FONT_OPEN_SANS_REGULAR_11_14PX": 4,
            "BAGL_FONT_OPEN_SANS_REGULAR_13_18PX": 5,
            "BAGL_FONT_OPEN_SANS_REGULAR_22_30PX": 6,
            "BAGL_FONT_OPEN_SANS_SEMIBOLD_8_11PX": 7,
            "BAGL_FONT_OPEN_SANS_EXTRABOLD_11px": 8,
            "BAGL_FONT_OPEN_SANS_LIGHT_16px": 9,
            "BAGL_FONT_OPEN_SANS_REGULAR_11px": 10,
            "BAGL_FONT_OPEN_SANS_SEMIBOLD_10_13PX": 11,
            "BAGL_FONT_OPEN_SANS_SEMIBOLD_11_16PX": 12,
            "BAGL_FONT_OPEN_SANS_SEMIBOLD_13_18PX": 13,
            "BAGL_FONT_SYMBOLS_0": 14,
            "BAGL_FONT_SYMBOLS_1": 15
        }
        # Those ids are defined in nbgl_fonts.h file, in nbgl_font_id_e enums.
        nbgl_font_ids={
            "BAGL_FONT_INTER_REGULAR_24px": 0,
            "BAGL_FONT_INTER_SEMIBOLD_24px": 1,
            "BAGL_FONT_INTER_MEDIUM_32px": 2,
            "BAGL_FONT_HM_ALPHA_MONO_MEDIUM_32px": 3,
            "BAGL_FONT_INTER_REGULAR_24px_1bpp": 4,
            "BAGL_FONT_INTER_SEMIBOLD_24px_1bpp": 5,
            "BAGL_FONT_INTER_MEDIUM_32px_1bpp": 6
        }
        if self.nbgl:
            font_ids = nbgl_font_ids
            font_id = 0        # BAGL_FONT_INTER_REGULAR_24px by default
        else:
            font_ids = bagl_font_ids
            font_id = 10        # BAGL_FONT_OPEN_SANS_REGULAR_11px by default

        if self.font_id_name in font_ids.keys():
            font_id = font_ids[self.font_id_name]

        return font_id

    # -------------------------------------------------------------------------
    def read_ids(self, id_filename):
        """
        Read a text file (a .h actually) containing all IDs to use.
        """
        if not os.path.exists(id_filename):
            sys.stderr.write(f"Can't open file {id_filename}\n")
            sys.exit(-5)

        # Read the .h file and store each ID found
        with open(id_filename, "r") as id_file:
            for line in id_file:
                # Ignore lines that don't start with "
                if line[0] != '"':
                    continue
                # Remove leading "
                line = line.lstrip('"')
                # Remove trailing ",
                line = line.rstrip('",\n')
                self.ids.append(line)

    # -------------------------------------------------------------------------
    def add_unicode_chars(self, string, string_id):
        """
        Parse string and add unicode characters not already stored
        """
        # If we have a list of IDs, be sure that one is included in it
        if len(self.ids) != 0:
            if string_id not in self.ids:
                return

        for char in string:
            # Just check unicode characters
            # \b, \n, \f and \e\xAB will appear individually, as ASCII chars!
            if ord(char) > 0x7F and char not in self.unicode_chars:
                self.unicode_chars += char

    # -------------------------------------------------------------------------
    def parse_json_strings(self, filenames):
        """
        Parse the provided JSON file(s) and scan all unicode chars found in strings
        """
        for file in filenames.split():
            if not os.path.exists(file):
                sys.stderr.write(f"Can't open file {file}\n")
                sys.exit(-4)

            # Read the JSON file into json_data
            with open(file, "r") as json_file:
                json_data = json.load(json_file, strict=False)

            # Parse all strings contained in txt fields of json_data
            for category in json_data:
                # Skip Smartling related part of the JSON file
                if category.lower() == "smartling":
                    continue

                # Parse all "text" strings and add the unicode characters
                for string in json_data[category]:
                    if not "text" in string.keys() or not "id" in string.keys():
                        sys.stdout.write(f"Skipping {string} because it does "\
                                         "not contain \"text\" or \"id\"!\n")
                        continue
                    self.add_unicode_chars(string['text'], string['id'])

        return self.unicode_chars

# -----------------------------------------------------------------------------
def change_ext(filename, extension):
    """
    Change a filename extension
    """
    # Get the filename without current extension
    new_filename = os.path.splitext(filename)[0]
    # Add the new extension
    new_filename += extension

    return new_filename

# -----------------------------------------------------------------------------
# Program entry point:
# -----------------------------------------------------------------------------
if __name__ == "__main__":

    # -------------------------------------------------------------------------
    def main(args):

        with TTF2INC(args) as ttf:

            # If we provided a list of IDs to use, read the file!
            if args.id:
                ttf.read_ids(args.id)

            # if JSON filename(s) was provided, parse them to get the strings
            if args.json_filenames:
                string = ttf.parse_json_strings(args.json_filenames)
            # else use the provided string or generate all wanted ASCII characters:
            else:
                string = args.string
            if len(string) == 0:
                for value in range(ttf.first_character, ttf.last_character+1):
                    string += chr(value)

            # Find dimension used by specified characters with that font
            ttf.find_string_dimension(string)

            # Now, handle all characters from the string:
            for char in string:
                try:
                    # We could have done everything in one step, from .ttf to
                    # .inc, but to allow manual editing of generated images
                    # files, we will process chars in two steps:
                    # - Step 1: from .ttf (or .otf) to .png (or .bmp)
                    # - Step 2: from .png (or .bmp) to .inc

                    # STEP 1: Generate the img if it doesn't exist
                    # (or load it otherwise)
                    ttf.get_char_image(char)

                    # STEP 2: Get bitmap data based on img content
                    ttf.get_char_bitmap(char)

                except (BaseException, Exception) as error:
                    sys.stderr.write(f"An error occurred while processing char"
                                     f" '{char}' with font {ttf.font_name}"
                                     f" size {args.font_size}: {error}\n")
                    return 1

            if args.verbose:
                sys.stdout.write(f"Font {ttf.basename.upper()}: " \
                                 f"ascent={ttf.ascent}, descent={ttf.descent}"\
                                 f", font size={ttf.font.size}, "\
                                 f"font height={ttf.font_height}\n"\
                                 f"crop={ttf.crop}, "\
                                 f"char_leftmost_x={ttf.char_leftmost_x}, "\
                                 f"char_topmost_y={ttf.char_topmost_y}, "\
                                 f"real font height={ttf.height}, "\
                                 f"baseline={ttf.baseline}\n")

            # Last step: generate .inc & json files with data for all chars
            # (the .inc file will be stored in src directory)
            if args.output_name:
                inc_filename = args.output_name
            else:
                filename = f"{ttf.font_prefix}{ttf.basename}.inc"
                inc_filename = os.path.join("../../../public_sdk/lib_bagl/src/", filename)

            # Force .inc extension for inc_filename
            inc_filename = change_ext(inc_filename, ".inc")

            # Build the corresponding .json file, if we need to
            if ttf.unicode_needed:
                inc_json = change_ext(inc_filename, ".json")
            else:
                inc_json = None

            if args.suffix:
                suffix = args.suffix
            else:
                suffix = ""
            if args.verbose:
                sys.stdout.write(f"Generating file {inc_filename}\n")

            ttf_info_list = []
            if args.append:
                mode = "a"
                # Read previous entries, if such file exists
                if inc_json and os.path.exists(inc_json):
                    with open(inc_json, "r") as json_file:
                        ttf_info_list = json.load(json_file, strict=False)
            else:
                mode = "w"

            ttf.ttf_info_dictionary = {}
            crop = 0
            if ttf.crop:
                crop = 1
            with open(inc_filename, mode) as inc:
                if not args.output_name:
                    inc.write("/* @BANNER@ */\n\n")
                # Write the array containing all bitmaps:
                if ttf.nbgl:
                    inc.write('#include "nbgl_fonts.h"\n\n')
                    inc.write(
                        f"__attribute__ ((section(\"._nbgl_fonts_\"))) "
                        f"const unsigned char bitmap{ttf.basename.upper()}"
                        f"{suffix}[] = {{\n")
                else:
                    inc.write(
                        f"const unsigned char bitmap{ttf.basename.upper()}"
                        f"{suffix}[] = {{\n")
                offset = 0
                first_char = None
                ttf.ttf_info_dictionary["bitmap"] = bytes()
                # Write the bitmap part of each character
                for char, info in sorted(ttf.char_info.items()):
                    image_data = info["bitmap"]
                    if image_data is not None:
                        ttf.ttf_info_dictionary["bitmap"] += image_data
                    info["offset"] = offset
                    offset += info["size"]
                    ttf.bitmap_len = offset

                    # Keep track of first and last chars:
                    if not first_char:
                        first_char = ord(char)
                    last_char = ord(char)

                    if ttf.unicode_needed:
                        # Get the unicode value of that char:
                        # (plane value from 0 to 16 then a 16-bit code)
                        inc.write(f"//unicode 0x{ord(char):06X}\n")
                    else:
                        inc.write(f"//ascii 0x{ord(char):04X}\n")
                    for i in range(0, info["size"], 8):
                        inc.write("  " + ", ".join("0x{0:02X}".format(c) for c
                                                   in image_data[i:i+8]) + ",")
                        inc.write("\n")

                inc.write("};\n")

                # Serialize bitmap
                ttf.ttf_info_dictionary["bitmap"] = base64.b64encode(
                    ttf.ttf_info_dictionary["bitmap"]).decode('utf-8')

                # Write the array containing information about characters:
                if ttf.unicode_needed:
                    typedef = f"{ttf.font_prefix}unicode_character_t"
                else:
                    typedef = f"{ttf.font_prefix}character_t"

                inc.write("\n")
                if ttf.nbgl:
                    inc.write(" __attribute__ ((section(\"._nbgl_fonts_\"))) ")
                inc.write(
                    f"const {typedef} characters{ttf.basename.upper()}"
                    f"{suffix}[{len(ttf.char_info)}] = {{\n")

                ttf.ttf_info_dictionary[f"{ttf.font_prefix}unicode_character"]= []

                # Save each character info into the .inc file
                for char, info in sorted(ttf.char_info.items()):

                    if ttf.unicode_needed:
                        if ttf.nbgl:
                            ttf.save_nbgl_font_unicode_character(
                                inc, char, info)
                        else:
                            ttf.save_bagl_font_unicode_character(
                                inc, char, info)
                    else:
                        if ttf.nbgl:
                            ttf.save_nbgl_font_character(
                                inc, char, info)
                        else:
                            ttf.save_bagl_font_character(
                                inc, char, info)

                inc.write("};\n")

                # Write the struct containing information about the font:
                if ttf.unicode_needed:
                    typedef = f"{ttf.font_prefix}unicode_t"
                    ttf.ttf_info_dictionary[f"{ttf.font_prefix}unicode"] = {
                        "bitmap_len": ttf.bitmap_len,
                        "font_id": ttf.get_font_id(),
                        "bpp": ttf.bpp,
                        "height": ttf.height,
                        "baseline": ttf.baseline,
                        "line_height": ttf.line_size,
                        "crop": crop,
                        "nb_characters": len(ttf.char_info),
                        "char_leftmost_x": ttf.char_leftmost_x,
                        "char_topmost_y": ttf.char_topmost_y
                    }
                else:
                    typedef = f"{ttf.font_prefix}t"

                if ttf.unicode_needed:
                    if ttf.nbgl:
                        ttf.save_nbgl_font_unicode(
                            inc, crop, suffix)
                    else:
                        ttf.save_bagl_font_unicode(
                            inc, suffix)
                else:
                    if ttf.nbgl:
                        ttf.save_nbgl_font(
                            inc, crop, suffix, first_char, last_char)
                    else:
                        ttf.save_bagl_font(
                            inc, suffix, first_char, last_char)

                inc.write("};\n")

            # Do we need to generate a JSON file with unicode related info?
            if ttf.unicode_needed:
                ttf_info_list.append(ttf.ttf_info_dictionary)
                with open(inc_json, "w") as json_file:
                    json.dump(ttf_info_list, json_file)
                    # Be sure there is a newline at the end of the file
                    json_file.write("\n")

        return 0

    # -------------------------------------------------------------------------
    # Parse arguments:
    parser = argparse.ArgumentParser(
        description="Convert a .ttf file into a .inc file (Build #230531.1811)")

    parser.add_argument(
        "-n", "--name", "--font_name",
        dest="font_name", type=str,
        default=TTF2INC.FONT_NAME,
        help="Font name ('%(default)s' by default)")

    parser.add_argument(
        "--font_id_name",
        dest="font_id_name", type=str,
        default=None,
        help="Font ID name ('%(default)s' by default)")

    parser.add_argument(
        "--output",
        dest="output_name", type=str,
        default=None,
        help="Output name ('%(default)s' by default)")

    parser.add_argument(
        "--suffix",
        dest="suffix", type=str,
        default=None,
        help="Suffix added to variable names ('%(default)s' by default)")

    parser.add_argument(
        "-s", "--size", "--font_size",
        dest="font_size", type=int,
        default=TTF2INC.FONT_SIZE,
        help="Font size in pixels ('%(default)s' by default)")

    parser.add_argument(
        "-f", "--first", "--first_character",
        dest="first_character", type=int,
        default=TTF2INC.FIRST_CHARACTER,
        help="First character ('%(default)s' by default)")

    parser.add_argument(
        "-l", "--last", "--last_character",
        dest="last_character", type=int,
        default=TTF2INC.LAST_CHARACTER,
        help="Last character ('%(default)s' by default)")

    parser.add_argument(
        "-c", "--chars", "--string",
        dest="string", type=str,
        default=TTF2INC.STRING,
        help="String with characters to convert ('%(default)s' by default)")

    parser.add_argument(
        "-i", "--init",
        dest="init_file", type=str,
        help="init file for this font/size")

    parser.add_argument(
        "-j", "--json",
        dest="json_filenames", type=str,
        default=None,
        help="Full path of JSON filenames containing all strings ('%(default)s' by default)")

    parser.add_argument(
        "--id",
        dest="id", type=str,
        default=None,
        help="Filename containing all IDs to use ('%(default)s' by default)")

    parser.add_argument(
        "-a", "--append",
        dest="append", action='store_true',
        default=False,
        help="Append to existing INC file(s) ('%(default)s' by default)")

    parser.add_argument(
        "-o", "--overwrite",
        dest="overwrite", action='store_true',
        default=False,
        help="Overwrite & ignore existing picture(s) ('%(default)s' by default)")

    parser.add_argument(
        "--save",
        dest="save", action='store_true',
        default=False,
        help="Save PNG file(s) ('%(default)s' by default)")

    parser.add_argument(
        "-v", "--verbose",
        dest="verbose", action='store_true',
        default=False,
        help="Add verbosity to output ('%(default)s' by default)")

    # Call main function:
    EXIT_CODE = main(parser.parse_args())

    sys.exit(EXIT_CODE)
