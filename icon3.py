#!/usr/bin/env python3

"""

/*******************************************************************************
*   Ledger Nano S - Secure firmware
*   (c) 2019 Ledger
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
********************************************************************************/
"""

import argparse
import collections
import colorsys
import math
import os
import struct
import binascii
import sys
import traceback

from PIL import Image


MAX_COLORS = 16


def is_power2(n):
    return n != 0 and ((n & (n - 1)) == 0)


def image_to_packed_buffer(im, palette, bits_per_pixel):
    width, height = im.size

    current_byte = 0
    current_bit = 0
    image_data = []

    # Row first
    for row in range(height):
        for col in range(width):
            # Return an index in the indexed colors list for indexed address spaces
            # left to right
            # Perform implicit rotation here (0,0) is left top in BAGL, and generally left bottom for various canvas
            color_index = im.getpixel((col, row))

            # Remap index by luminance
            color_index = palette[color_index]

            # le encoded
            current_byte += color_index << current_bit
            current_bit += bits_per_pixel

            if current_bit >= 8:
                image_data.append(current_byte&0xFF)
                current_bit = 0
                current_byte = 0

        # Handle last byte if any
    if current_bit > 0:
        image_data.append(current_byte&0xFF)
    return bytes(image_data)


def main():
    parser = argparse.ArgumentParser(description='Generate source code for BAGL icons.')
    parser.add_argument('image_file', help="Icons to process", nargs='+')
    parser.add_argument('--max_width', type=int, default=4096, help="Max width")
    parser.add_argument('--max_height', type=int, default=4096, help="Max height")
    parser.add_argument('--hexbitmaponly', action='store_true')
    parser.add_argument('--glyphcheader', action='store_true')
    parser.add_argument('--glyphcfile', action='store_true')
    parser.add_argument('--errors', action='store_true')
    parser.add_argument('--factorize', action='store_true')
    args = parser.parse_args()

    exitcode = 0
    for file in args.image_file:
        if not os.path.exists(file):
            sys.stderr.write("Error: {} does not exist!".format(file) + "\n")
            if args.errors:
                exitcode=-1
            continue

    colors_array = {}
    processed_image_names = []
    for file in args.image_file:
        try:
            im = Image.open(file)
            if im.mode is not 'P':
                sys.stderr.write("Error: input file {} must have indexed colors".format(file) + "\n")
                if args.errors:
                    exitcode=-1
                continue

            im.load()

            width, height = im.size
            width = min(width, args.max_width)
            height = min(height, args.max_height)

            image_name = os.path.splitext(os.path.basename(file))[0]
            # if image name has already been done, then don't do it twice
            if image_name in processed_image_names:
                continue;
            processed_image_names.append(image_name)

            num_colors = len(im.getcolors())
            if num_colors > MAX_COLORS:
                sys.stderr.write("Error: input file {} has too many colors".format(file) + "\n")
                if args.errors:
                    exitcode=-1
                continue

            # Round number of colors to a power of 2
            if not is_power2(num_colors):
                num_colors = int(pow(2, math.ceil(math.log(num_colors, 2))))

            bits_per_pixel = int(math.log(num_colors, 2))

            # Reorder color map by luminance
            palette = im.getpalette()
            opalette = {}
            for i in range(num_colors):
                red, green, blue = palette[3 * i: 3 * i + 3]
                hue, saturation, value = colorsys.rgb_to_hsv(red / 255., green / 255., blue / 255.)

                # Several colors could have the same luminance
                if value * 255. not in opalette:
                    opalette[value * 255.] = []
                opalette[value * 255.].append([i, (red << 16) + (green << 8) + blue])
            opalette = collections.OrderedDict(sorted(opalette.items()))

            # Compute the remapping index
            i = 0
            new_indices = {}
            new_palette = []
            color_array_serialized=""
            for lum, values in opalette.items():
                # Old index to new index
                for v in values:
                    new_indices[v[0]] = i
                    new_palette.append(v[1])
                    color_array_serialized += hex(v[1])
                    i += 1

            if args.hexbitmaponly:
                # write BPP
                header = struct.pack(">B",bits_per_pixel)
                # LE color array, it is meant to be embedded as is in an array
                for i in range(num_colors):
                    header += struct.pack(">I",new_palette[i])

                image_data = image_to_packed_buffer(im, new_indices, bits_per_pixel)
                print(binascii.hexlify(header + image_data).decode('utf-8'))
                continue

            else:
                # General definitions
                if not args.glyphcfile:
                    print("""#ifndef GLYPH_{0}_BPP
        #define GLYPH_{0}_WIDTH {1}
        #define GLYPH_{0}_HEIGHT {2}
        #define GLYPH_{0}_BPP {3}""".format(image_name, width, height, bits_per_pixel))
                else:
                    print("#include \"glyphs.h\"")

                # Print palette
                if args.glyphcheader:
                    print("extern unsigned int const C_{0}_colors[];".format(image_name))
                else:
                    print("unsigned int const C_{0}_colors[] = {{".format(image_name))
                    # add the color palette as reference for factorized mode
                    if not color_array_serialized in colors_array:
                        colors_array[color_array_serialized] = image_name

                    # Color index encoding
                    for i in range(num_colors):
                        # Endian less value
                        print("  0x{0:08x},".format(new_palette[i]))
                    print("};\n")

                # Print image data
                if args.glyphcheader:
                    print("extern unsigned char const C_{0}_bitmap[];".format(image_name))
                else:
                    print("unsigned char const C_{0}_bitmap[] = {{".format(image_name))

                    # Packed, row preferred
                    if not args.glyphcheader:
                        image_data = image_to_packed_buffer(im, new_indices, bits_per_pixel)
                        for i in range(0, len(image_data), 16):
                            print("  " + ", ".join("0x{0:02x}".format(c) for c in image_data[i:i+16]) + ",")
                        print("};")

            if args.glyphcheader:
                print("""#ifdef OS_IO_SEPROXYHAL
        #include \"os_io_seproxyhal.h\"
        extern const bagl_icon_details_t C_{0};
        #endif // GLYPH_{0}_BPP
        #endif // OS_IO_SEPROXYHAL""".format(image_name))
            elif args.glyphcfile:
                color_ref = image_name;
                if args.factorize:
                    if color_array_serialized in colors_array:
                        color_ref = colors_array[color_array_serialized]
                print("""#ifdef OS_IO_SEPROXYHAL
        #include \"os_io_seproxyhal.h\"
        const bagl_icon_details_t C_{0} = {{ GLYPH_{0}_WIDTH, GLYPH_{0}_HEIGHT, {1}, C_{2}_colors, C_{0}_bitmap }};
        #endif // OS_IO_SEPROXYHAL""".format(image_name, int(math.log(num_colors, 2)), color_ref))
            else:
                # Origin 0,0 is left top for blue, instead of left bottom for all image encodings
                print("{{ {0:d}, {1:d}, {2:d}, C_{3}_colors, C_{3}_bitmap }},".format(
                    width, height, int(math.log(num_colors, 2)), image_name))
        except:
            sys.stderr.write("Exception while processing {}\n".format(file))
            try:
                traceback.print_tb()
            except:
                pass
            if args.errors:
                exitcode=-1
    if (exitcode != 0):
        sys.exit(exitcode)

if __name__ == "__main__":
    main()
