#!/usr/bin/env python3

"""
Converts the given icons, in bmp or gif format, to Fatstacks compatible glyphs, as C arrays
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

def get4BPPval(color_index):
    pixel_val = color_index[0]<<16+color_index[1]<<8+color_index[0]
    if (pixel_val > 0xF0F0F0):
        return 0xF
    elif (pixel_val > 0xE0E0E0):
        return 0xE
    elif (pixel_val > 0xD0D0D0):
        return 0xD
    elif (pixel_val > 0xC0C0C0):
        return 0xC
    elif (pixel_val > 0xB0B0B0):
        return 0xB
    elif (pixel_val > 0xA0A0A0):
        return 0xA
    elif (pixel_val > 0x909090):
        return 0x9
    elif (pixel_val > 0x808080):
        return 0x8
    elif (pixel_val > 0x707070):
        return 0x7
    elif (pixel_val > 0x606060):
        return 0x6
    elif (pixel_val > 0x505050):
        return 0x5
    elif (pixel_val > 0x404040):
        return 0x4
    elif (pixel_val > 0x303030):
        return 0x3
    elif (pixel_val > 0x202020):
        return 0x2
    elif (pixel_val > 0x101010):
        return 0x1
    else:
        return 0

def get2BPPval(color_index):
    pixel_val = color_index[0]<<16+color_index[1]<<8+color_index[0]
    if (pixel_val == 0xFFFFFF):
        return 3
    elif (pixel_val > 0x808080):
        return 2
    elif (pixel_val > 0x404080):
        return 1
    else:
        return 0

def get1BPPval(color_index):
    pixel_val = color_index[0]<<16+color_index[1]<<8+color_index[0]
    if (pixel_val > 0xF0F0F0):
        return 0
    else:
        return 1

def image_to_packed_buffer(im, palette, bits_per_pixel):
    width, height = im.size

    current_byte = 0
    current_bit = 0
    image_data = []
    if palette == None:
        palette_func = {
            1: get1BPPval, # palette for 1BPP
            2: get2BPPval, # palette for 2BPP
            4: get4BPPval, # palette for 4BPP
        }

    # col first
    for col in reversed(range(width)):
        for row in range(height):
            # Return an index in the indexed colors list for indexed address spaces
            # left to right
            # Perform implicit rotation here (0,0) is left top in BAGL, and generally left bottom for various canvas
            color_index = im.getpixel((col, row))

            # Remap index by luminance
            if palette == None:
                color_index = palette_func[bits_per_pixel](color_index)
            else:
                if bits_per_pixel == 1:
                    # in Stax we want 1BPP to be 1 for Black and 0 for White, in GIF it's the opposite
                    if palette[color_index] == 0:
                        color_index = 1
                    else:
                        color_index = 0
                else:
                    color_index = palette[color_index]

            # le encoded
            current_byte += color_index << ((8-bits_per_pixel)-current_bit)
            current_bit += bits_per_pixel

            if current_bit >= 8:
                image_data.append(current_byte & 0xFF)
                current_bit = 0
                current_byte = 0

        # Handle last byte if any
    if current_bit > 0:
        image_data.append(current_byte & 0xFF)
    return bytes(image_data)


def main():
    parser = argparse.ArgumentParser(description='Generate source code for NBGL icons.')
    parser.add_argument('image_file', help="Icons to process", nargs='+')
    parser.add_argument('--max_width', type=int, default=4096, help="Max width")
    parser.add_argument('--max_height', type=int, default=4096, help="Max height")
    parser.add_argument('--hexbitmaponly', action='store_true')
    parser.add_argument('--glyphcheader', action='store_true')
    parser.add_argument('--glyphcfile', action='store_true')
    parser.add_argument('--errors', action='store_true')
    args = parser.parse_args()

    exitcode = 0
    for file in args.image_file:
        if not os.path.exists(file):
            sys.stderr.write("Error: {} does not exist!".format(file) + "\n")
            if args.errors:
                exitcode = -1
            continue

    if args.glyphcfile:
        print("#include \"glyphs.h\"")
    if args.glyphcfile or args.glyphcheader:
        print("""#ifdef HAVE_NBGL
#include \"nbgl_types.h\"
#endif // HAVE_NBGL
""")

    colors_array = {}
    processed_image_names = []
    for file in args.image_file:
        try:
            im = Image.open(file)

            im.load()

            width, height = im.size
            width = min(width, args.max_width)
            height = min(height, args.max_height)

            image_name = os.path.splitext(os.path.basename(file))[0]
            # if image name has already been done, then don't do it twice
            if image_name in processed_image_names:
                continue
            processed_image_names.append(image_name)

            num_colors = len(im.getcolors())
            if num_colors > MAX_COLORS:
                sys.stderr.write("Error: input file {} has too many colors".format(file) + "\n")
                if args.errors:
                    exitcode = -1
                continue

            # Round number of colors to a power of 2
            if not is_power2(num_colors):
                num_colors = int(pow(2, math.ceil(math.log(num_colors, 2))))

            bits_per_pixel = int(math.log(num_colors, 2))
            if bits_per_pixel == 3:
                bits_per_pixel = 4

            # if palettized, create palette
            if im.mode == 'P':
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
                color_array_serialized = ""
                for lum, values in opalette.items():
                    # Old index to new index
                    for v in values:
                        new_indices[v[0]] = i
                        new_palette.append(v[1])
                        color_array_serialized += hex(v[1])
                        i += 1

            if args.hexbitmaponly:
                # if not palettized
                if im.mode != 'P':
                    image_data = image_to_packed_buffer(im, None, bits_per_pixel)
                else:
                    image_data = image_to_packed_buffer(im, new_indices, bits_per_pixel)
                print(binascii.hexlify(image_data).decode('utf-8'))
                continue

            else:
                # General definitions
                if not args.glyphcfile:
                    print("""#ifndef GLYPH_{0}_BPP
  #define GLYPH_{0}_WIDTH {1}
  #define GLYPH_{0}_HEIGHT {2}
  #define GLYPH_{0}_BPP {3}""".format(image_name, width, height, bits_per_pixel))

                # Print image data
                if args.glyphcheader:
                    # if not palettized
                    if im.mode != 'P':
                        image_data = image_to_packed_buffer(im, None, bits_per_pixel)
                    else:
                        image_data = image_to_packed_buffer(im, new_indices, bits_per_pixel)
                    print("  extern uint8_t const C_{0}_bitmap[{1:d}];".format(image_name, len(image_data)))
                else:
                    print("uint8_t const C_{0}_bitmap[] = {{".format(image_name))

                    # Packed, row preferred
                    if not args.glyphcheader:
                        # if not palettized
                        if im.mode != 'P':
                            image_data = image_to_packed_buffer(im, None, bits_per_pixel)
                        else:
                            image_data = image_to_packed_buffer(im, new_indices, bits_per_pixel)
                        for i in range(0, len(image_data), 16):
                            print("  " + ", ".join("0x{0:02x}".format(c) for c in image_data[i:i+16]) + ",")
                        print("};")

            if args.glyphcheader:
                print("""  #ifdef HAVE_NBGL
    extern const nbgl_icon_details_t C_{0};
  #endif // HAVE_NBGL
#endif // GLYPH_{0}_BPP
""".format(image_name))
            elif args.glyphcfile:
                print("""#ifdef HAVE_NBGL
const nbgl_icon_details_t C_{0} = {{ GLYPH_{0}_WIDTH, GLYPH_{0}_HEIGHT, NBGL_BPP_{1}, C_{0}_bitmap }};
#endif // HAVE_NBGL
""".format(image_name, bits_per_pixel))
        except:
            sys.stderr.write("Exception while processing {}\n".format(file))
            try:
                traceback.print_tb()
            except:
                pass
            if args.errors:
                exitcode = -1
    if exitcode != 0:
        sys.exit(exitcode)


if __name__ == "__main__":
    main()
