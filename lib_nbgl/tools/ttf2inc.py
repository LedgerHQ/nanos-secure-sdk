#!/usr/bin/env python3
# coding: utf-8
# -----------------------------------------------------------------------------
import argparse
import sys
import os
import json
import struct
from PIL import Image, ImageFont, ImageDraw
import configparser


# -----------------------------------------------------------------------------
class TTF2INC (object):
    """
    This class will contain methods to handle TTF fonts & generate .inc files
    """

# -------------------------------------------------------------------------
    def __init__(self, args):
        super().__init__()
        # Default value:
        self.unicode_needed = False
        self.basename = None
        self.directory = None
        # Be sure there are at least some mandatory characters
        self.unicode_chars = "�"
        self.ids = []
        # Store parameters:
        self.args = args
        #read config
        config = configparser.ConfigParser()
        config.read(args.init_file)

        print(args.init_file)
        self.configMain = config['main']
        self.first_character = int(self.configMain.get('firstChar','0x20'),16)
        self.last_character = int(self.configMain.get('lastChar','0x7A'),16)
        # first Line to be taken into glyphs
        self.firstLine = self.configMain.getint('firstLine',0)

        self.font_name = self.configMain.get('font')
        self.font_size = self.configMain.getint('fontSize')
        self.line_size = self.configMain.getint('lineSize')
        self.align_width = self.configMain.getboolean('align',False)
        try:
            self.font = ImageFont.truetype(os.path.join(os.path.dirname(args.init_file),self.font_name)+'.otf', self.font_size-1)
        except (BaseException, Exception) as error:
            sys.stderr.write(f"Error with font {self.font_name}: {error}\n")
            sys.exit(-1)

        # Get font metrics:
        # - ascent: distance from the baseline to the highest outline point.
        # - descent: distance from the baseline to the lowest outline point.
        self.ascent, self.descent = self.font.getmetrics()
        if self.args.verbose:
            sys.stdout.write(f"ascent={self.ascent}, descent={self.descent}\n")

        self.font_height = self.font.font.height

        if self.args.verbose:
            sys.stdout.write(f"height={self.font_height}\n")

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
    def check_unicode(self, char):
        """
        Check if unicode is needed to encode provided character.
        """
        if ord(char) > 0x7F:
            self.unicode_needed = True

    # -------------------------------------------------------------------------
    def build_names(self):
        """
        Update the unicode_needed flag and Build filenames depending on font characteristics.
        """
        # Get the font basename, without extension:
        self.basename = os.path.splitext(
            os.path.basename(self.font_name))[0]
        self.basename = self.basename.replace("-","_")
        self.basename += f"_{self.font_size}px"
        # If characters are using Unicode, update basename:
        if self.unicode_needed:
            self.basename += "_unicode"
        # Build the destination directory name, based on font+size[+unicode]:
        self.directory = f"nbgl_font_{self.basename}"
        # Create that directory if it doesn't exist:
        if not self.args.no_write and not os.path.exists(self.directory):
            if self.args.verbose:
                sys.stdout.write(f"Creating directory {self.directory}\n")
            os.mkdir(self.directory)

    # -------------------------------------------------------------------------
    def get_char_picture(self, char):
        """
        Return the bitmap corresponding to the provided character(s).
        """
        # Get the unicode value of that char:
        # (in fact there is a plane value from 0 to 16 then a 16-bit code)
        unicode_value = f"0x{ord(char):06X}"
        # Get text size of the character:
        width, height = self.font.getsize(char)

        # Build full path of the destination file:
        filename = f"nbgl_font_{self.basename}_{unicode_value}"
        fullpath = os.path.join(self.directory, filename)

        # STEP 1: Generate the .gif file if it doesn't exist:
        if not os.path.exists(fullpath + ".gif") or self.args.overwrite:
            # Generate and save the .GIF picture
            # Create a B&W Image with that size
            # get potential offsets to apply for this char
            offsetY = self.configMain.getint('%s_offsetY'%unicode_value,0)
            charY = self.firstLine+offsetY

            if self.align_width:
                # align on closest multiple of 4
                i = 6
                while i < 50:
                    if width <= i:
                        width = i-2
                        break
                    i+=4

            img = Image.new('1', (width, self.font.font.height), color='black')
            draw = ImageDraw.Draw(img)
            draw.text((0, 0),
                    char, font=self.font, fill='white')
            img = img.crop((0,charY,width,charY+self.font_size))

            if not self.args.no_write:
                # Save the .GIF file:
                if self.args.verbose:
                    sys.stdout.write(f"Saving {fullpath}.gif ({width}x{height})\n")
                img.save(fullpath + ".gif")
        else:
            # We will load the .GIF picture:
            if self.args.verbose:
                sys.stdout.write(f"Loading {fullpath}.gif\n")
            img = Image.open(fullpath + ".gif")
            img = img.convert('L')

        return img

    # -------------------------------------------------------------------------
    # (based on icon3.py source code)
    def image_to_packed_buffer(self, img):
        """
        Rotate and pack bitmap data of the character.
        """
        width, height = img.size

        current_byte = 0
        current_bit = 0
        image_data = []

        # col first
        for col in reversed(range(width)):
            for row in range(height):
                # Return an index in the indexed colors list
                # top to bottom
                # Perform implicit rotation here (0,0) is left top in NBGL,
                # and generally left bottom for various canvas
                color_index = img.getpixel((col, row))
                if color_index >= 128:
                    color_index = 1
                else:
                    color_index = 0

                # Big Endian encoded
                current_byte += color_index << (7-current_bit)
                current_bit += 1

                if current_bit >= 8:
                    image_data.append(current_byte & 0xFF)
                    current_bit = 0
                    current_byte = 0

        # Handle last byte if any
        if current_bit > 0:
            image_data.append(current_byte & 0xFF)

        return bytes(image_data)

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
    def get_font_id_name(self):
        """
        Return the name of the font id.
        """
        # Those ids are defined in nbgl_font.h file, in nbgl_font_id_e enums.
        font_id = "BAGL_FONT"
        font_name = self.font_name.lower()
        if "open" in font_name and "sans" in font_name:
            font_id += "_OPEN_SANS"
            if "regular" in font_name:
                font_id += "_REGULAR"
            elif "extrabold" in font_name:
                font_id += "_EXTRABOLD"
            elif "semibold" in font_name:
                font_id += "_SEMIBOLD"
            elif "light" in font_name:
                font_id += "_LIGHT"
        elif "inter" in font_name:
            font_id += "_INTER"
            if "regular" in font_name:
                font_id += "_REGULAR"
            elif "extrabold" in font_name:
                font_id += "_EXTRABOLD"
            elif "semibold" in font_name:
                font_id += "_SEMIBOLD"
            elif "light" in font_name:
                font_id += "_LIGHT"
        elif "hma" in font_name:
            font_id += "_HM_ALPHA_MONO_MEDIUM"
        else:
            font_id += "_UNKNOWN"

        font_id += f"_{self.font_size}px"

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
# Program entry point:
# -----------------------------------------------------------------------------
if __name__ == "__main__":

    # -------------------------------------------------------------------------
    def main(args):

        with TTF2INC(args) as ttf:

            # If we provided a list of IDs to use, read the file!
            if args.id:
                ttf.read_ids(args.id)

            # If JSON filename(s) was provided, parse them to get the strings
            if args.json_filenames:
                string = ttf.parse_json_strings(args.json_filenames)
            # Use the provided string or generate all wanted ASCII characters:
            else:
                string = args.string
            if len(string) == 0:
                for value in range(ttf.first_character, ttf.last_character+1):
                    string += chr(value)

            for char in string:
                ttf.check_unicode(char)
            ttf.build_names()

            # create a mask with the string containing all chars
            mask = ttf.font.getmask(string, mode='1')
            mask_w, mask_h = mask.size

            # create an image (B&W) with the string containing all chars
            string_img = Image.new('1', (mask_w, ttf.font_height), color='black')
            draw = ImageDraw.Draw(string_img)

            # Compute 'real' font height & baseline from top:
            baseline = ttf.ascent - (ttf.font_height - ttf.font_size)
            if args.verbose:
                sys.stdout.write(f"Font height={ttf.font_size}, baseline={baseline}\n")

            # Now, handle all characters from the string:
            char_info = {}
            for char in string:
                try:
                    # We could have done everything in one step, from .ttf to
                    # .inc, but to allow manual editing of generated .gif
                    # files, we will process chars in two steps:
                    # - from .ttf to .gif
                    # - from .gif to .inc

                    # STEP 1: Generate the .gif file if it doesn't exist:
                    # (or load it otherwise)
                    img = ttf.get_char_picture(char)

                    # STEP 2: Get bitmap data based on .gif content:
                    image_data = ttf.image_to_packed_buffer(img)

                    # Store the information to process it later:
                    width, _ = img.size
                    char_info[char] = {"bitmap": image_data,
                                       "width": width,
                                       "size": len(image_data),
                                       "offset": 0,
                                       "img":img}

                except (BaseException, Exception) as error:
                    sys.stderr.write(f"An error occurred while processing char"
                                     f" '{char}' with font {ttf.font_name}"
                                     f" x {ttf.font_size}: {error}\n")
                    return -2

            # Last step: generate .inc file with bitmap data for all chars:
            # (the .inc file will be stored in src directory)
            if args.append:
                mode = "a"
            else:
                mode = "w"
            if args.output_name:
                inc_filename = args.output_name
            else:
                filename = f"nbgl_font_{ttf.basename}.inc"
                inc_filename = os.path.join("../../../public_sdk/lib_bagl/src/", filename)
            if args.suffix:
                suffix = args.suffix
            else:
                suffix = ""
            if args.verbose:
                sys.stdout.write(f"Generating file {inc_filename}\n")
            with open(inc_filename, mode) as inc:
                if not args.output_name:
                    inc.write("/* @BANNER@ */\n\n")
                # Write the array containing all bitmaps:
                inc.write('#include "nbgl_fonts.h"\n\n')
                inc.write(f"__attribute__ ((section(\"._nbgl_fonts_\"))) const unsigned char bitmap{ttf.basename.upper()}{suffix}[]"
                          f" = {{\n")
                offset = 0
                first_char = None
                for char, info in sorted(char_info.items()):
                    image_data = info["bitmap"]
                    info["offset"] = offset
                    offset += info["size"]

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

                # Write the array containing information about characters:
                if ttf.unicode_needed:
                    typedef = "nbgl_font_unicode_character_t"
                else:
                    typedef = "nbgl_font_character_t"

                inc.write(
                    f"\n __attribute__ ((section(\"._nbgl_fonts_\"))) const {typedef} characters"
                    f"{ttf.basename.upper()}{suffix}[{len(char_info)}] = {{\n")
                for char, info in sorted(char_info.items()):
                    width = info["width"]
                    size = info["size"]
                    offset = info["offset"]
                    if ttf.unicode_needed:
                        unicode = f"0x{ord(char):06X}"
                        inc.write(f"  {{ 0x{ord(char):06X}, {width:3}, {size:3}"
                                  f", {offset:4} }}, //unicode {unicode}\n")
                    else:
                        inc.write(f"  {{ {width:3}, {size:3}, {offset:4} }},"
                                  f" //ascii 0x{ord(char):04X}\n")
                inc.write("};\n")

                # Write the struct containing information about the font:
                if ttf.unicode_needed:
                    typedef = "nbgl_font_unicode_t"
                else:
                    typedef = "nbgl_font_t"
                inc.write(
                    f"\n __attribute__ ((section(\"._nbgl_fonts_\"))) const {typedef} font{ttf.basename.upper()}{suffix} = {{\n")
                inc.write(f"  {ttf.get_font_id_name()}, // font id\n")
                inc.write( "  1, // bpp => 1 for B&W\n")
                inc.write(f"  {ttf.font_size}, // font height in pixels\n")
                inc.write(f"  {baseline}, // baseline height in pixels\n")
                inc.write(f"  {ttf.line_size}, // line height in pixels\n")
                inc.write( "  0, // kerning - specific to the font\n")
                inc.write(f"  0x{first_char:X}, // first character\n")
                inc.write(f"  0x{last_char:X}, // last character\n")
                if not suffix:
                    inc.write(f"  characters{ttf.basename.upper()},\n")
                    inc.write(f"  bitmap{ttf.basename.upper()}\n")
                inc.write("};\n")

                if args.test_align != None:
                    string_width = 0
                    for char, info in sorted(char_info.items()):
                        string_width+=info['width']
                    img = Image.new('1', (string_width, ttf.font_size), color='black')
                    current_width = 0
                    for char, info in sorted(char_info.items()):
                        img.paste(info['img'], (current_width, 0))
                        current_width += info['width']
                    draw = ImageDraw.Draw(img)
                    shape = [(0,args.test_align), (string_width, args.test_align)]
                    draw.line(shape, fill='white',width=0)

                    img.show()



        return 0

    # -------------------------------------------------------------------------
    # Parse arguments:
    parser = argparse.ArgumentParser(
        description="Convert a .ttf file into a .inc file (Build #221118.1041)")

    parser.add_argument(
        "--output",
        dest="output_name", type=str, required = True,
        help="C File Output name")

    parser.add_argument(
        "--suffix",
        dest="suffix", type=str,
        default=None,
        help="Suffix added to variable names ('%(default)s' by default)")

    parser.add_argument(
        "-c", "--chars", "--string",
        dest="string", type=str,
        default='',
        help="String with characters to convert ('' by default)")

    parser.add_argument(
        "-i", "--init",
        dest="init_file", type=str, required = True,
        help="init file for this font/size")

    parser.add_argument(
        "--test_align",
        dest="test_align", type=int,
        help="If set, creates a line at the given offset and save the whole string as a picture")

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
        "--no_write",
        dest="no_write", action='store_true',
        default=False,
        help="Don't create GIF file(s) ('%(default)s' by default)")

    parser.add_argument(
        "-o", "--overwrite",
        dest="overwrite", action='store_true',
        default=False,
        help="Overwrite existing GIF file(s) ('%(default)s' by default)")

    parser.add_argument(
        "-v", "--verbose",
        dest="verbose", action='store_true',
        default=False,
        help="Add verbosity to output ('%(default)s' by default)")

    # Call main function:
    EXIT_CODE = main(parser.parse_args())

    sys.exit(EXIT_CODE)
