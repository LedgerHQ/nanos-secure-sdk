#!/usr/bin/env python3

"""
Converts the given icons, in bmp or gif format, to Stax compatible glyphs, as C arrays
"""

import argparse
import math
import os
import binascii
import sys
import traceback
import gzip

from enum import IntEnum
from PIL import Image as img, ImageOps
from PIL.Image import Image
from typing import Tuple, Optional

from nbgl_rle import Rle1bpp, Rle4bpp


class NbglFileCompression(IntEnum):
    NoCompression = 0
    Gzlib = 1,
    Rle = 2


def is_power2(n):
    return n != 0 and ((n & (n - 1)) == 0)


def open_image(file_path) -> Optional[Tuple[Image, int]]:
    """
    Open and prepare image for processing.
    Returns None if image has too many colors or does not exist.
    else returns tuple: Image, bpp
    """
    # Check existente
    if not os.path.exists(file_path):
        sys.stderr.write("Error: {} does not exist!".format(file_path) + "\n")

    # Load Image in mode L
    im = img.open(file_path)
    im.load()
    im = im.convert('L')

    # Do not open image with more than 16 colors
    num_colors = len(im.getcolors())
    if num_colors > 16:
        sys.stderr.write(
            "Warn: input file {} has too many colors".format(file_path) + "\n")
        num_colors = 16

    # Compute bits_per_pixel
    # Round number of colors to a power of 2
    if not is_power2(num_colors):
        num_colors = int(pow(2, math.ceil(math.log(num_colors, 2))))

    bits_per_pixel = int(math.log(num_colors, 2))
    if bits_per_pixel == 3:
        bits_per_pixel = 4

    if bits_per_pixel == 0:
        bits_per_pixel = 1

    # Invert if bpp is 1
    if bits_per_pixel == 1:
        im = ImageOps.invert(im)

    return im, bits_per_pixel


def image_to_packed_buffer(img, bpp: int, reverse_1bpp):
    """
    Rotate and pack bitmap data of the character.
    """
    width, height = img.size

    current_byte = 0
    current_bit = 0
    image_data = []
    nb_colors = pow(2, bpp)
    base_threshold = int(256 / nb_colors)
    half_threshold = int(base_threshold / 2)

    # col first
    for col in reversed(range(width)):
        for row in range(height):
            # Return an index in the indexed colors list
            # top to bottom
            # Perform implicit rotation here (0,0) is left top in NBGL,
            # and generally left bottom for various canvas
            color_index = img.getpixel((col, row))
            color_index = int((color_index + half_threshold) / base_threshold)

            if color_index >= nb_colors:
                color_index = nb_colors - 1

            if bpp == 1 and reverse_1bpp:
                    color_index = (color_index+1)&0x1

            # le encoded
            current_byte += color_index << ((8-bpp)-current_bit)
            current_bit += bpp

            if current_bit >= 8:
                image_data.append(current_byte & 0xFF)
                current_bit = 0
                current_byte = 0

    # Handle last byte if any
    if current_bit > 0:
        image_data.append(current_byte & 0xFF)

    return bytes(image_data)


# Compressions functions


def rle_compress(im: Image, bpp, reverse) -> Optional[bytes]:
    """
    Run RLE compression on input image
    """
    if bpp == 1:
        return Rle1bpp.rle_1bpp(im, reverse)[1]
    elif bpp == 2:
        # No compression supports BPP2
        return None
    elif bpp == 4:
        return Rle4bpp.rle_4bpp(im)[1]


def gzlib_compress(im: Image, bpp: int, reverse) -> bytes:
    """
    Run gzlib compression on input image
    """
    pixels_buffer = image_to_packed_buffer(im, bpp, reverse)
    output_buffer = []
    # cut into chunks of 2048 bytes max of uncompressed data (because decompression needs the full buffer)
    full_uncompressed_size = len(pixels_buffer)
    i = 0
    while full_uncompressed_size > 0:
        chunk_size = min(2048, full_uncompressed_size)
        tmp = bytes(pixels_buffer[i:i+chunk_size])
        compressed_buffer = gzip.compress(tmp, mtime=0)
        output_buffer += [len(compressed_buffer) & 0xFF,
                          (len(compressed_buffer) >> 8) & 0xFF]
        output_buffer += compressed_buffer
        full_uncompressed_size -= chunk_size
        i += chunk_size

    return bytearray(output_buffer)


NBGL_IMAGE_FILE_HEADER_SIZE = 8


def compress(im: Image, bpp, reverse) -> Tuple[NbglFileCompression, bytes]:
    """
    Compute multiple compression methods on the input image,
    and return a tuple containing:
        - The best compression method achieved
        - The associated compressed bytes
    """
    # GZlib is not supported on Nanos
    if not reverse:
        compressed_bufs = {
            NbglFileCompression.NoCompression: image_to_packed_buffer(im, bpp, reverse),
            NbglFileCompression.Gzlib: gzlib_compress(im, bpp, reverse),
            NbglFileCompression.Rle: rle_compress(im, bpp, reverse)
        }
    else:
        compressed_bufs = {
            NbglFileCompression.NoCompression: image_to_packed_buffer(im, bpp, reverse),
            NbglFileCompression.Rle: rle_compress(im, bpp, reverse)
        }

    min_len = len(compressed_bufs[NbglFileCompression.NoCompression])
    min_comp = NbglFileCompression.NoCompression

    for compression, buffer in compressed_bufs.items():
        if buffer is None:
            continue

        final_length = len(buffer)
        if compression != NbglFileCompression.NoCompression:
            final_length += NBGL_IMAGE_FILE_HEADER_SIZE

        if min_len > final_length:
            min_len = final_length
            min_comp = compression

    return min_comp, compressed_bufs[min_comp]


def convert_to_image_file(image_data: bytes, width: int, height: int,
                          bpp: int, compression: NbglFileCompression) -> bytes:
    """
    Returns an image file version of the input image data and parameters
    """
    BPP_FORMATS = {
        1: 0,
        2: 1,
        4: 2
    }

    result = [width & 0xFF, width >> 8,
              height & 0xFF, height >> 8,
              (BPP_FORMATS[bpp] << 4) | compression,
              len(image_data) & 0xFF, (len(image_data) >> 8) & 0xFF, (len(image_data) >> 16) & 0xFF]
    result.extend(image_data)
    return bytes(bytearray(result))


def compute_app_icon_data(im: Image, bpp, reverse) -> Tuple[bool, bytes]:
    """
    Process image as app icon:
    - App icon are always image file
    - Compression is not limited to 64x64
    """
    compression, image_data = compress(im, bpp, reverse)
    is_file = True
    width, height = im.size
    image_data = convert_to_image_file(
        image_data, width, height, bpp, compression)
    return is_file, image_data


def compute_regular_icon_data(no_comp: bool, im: Image, bpp, reverse) -> Tuple[bool, bytes]:
    """
    Process image as regular icon:
    - Regular icon are image file only if compressed
    - Compression is limited to images bigger than 64x64
    """
    width, height = im.size

    if not no_comp:
        compression, image_data = compress(im, bpp, reverse)
        if compression != NbglFileCompression.NoCompression:
            is_file = True
            image_data = convert_to_image_file(
                image_data, width, height, bpp, compression)
        else:
            is_file = False
    else:
        is_file = False
        image_data = image_to_packed_buffer(im, bpp, reverse)
    return is_file, image_data

# glyphs.c/.h chunk files generators


def print_glyphfile_top(add_include: bool):
    if add_include:
        print("#include \"glyphs.h\"")
    print(
        """\
#ifdef HAVE_NBGL
#include \"nbgl_types.h\"
#else
#include <stdint.h>
#endif
""")


def print_glyphcfile_data(image_name, bpp, image_data):
    print("uint8_t const C_{0}_bitmap[] = {{".format(image_name))
    for i in range(0, len(image_data), 16):
        print("  " + ", ".join("0x{0:02x}".format(c)
                               for c in image_data[i:i+16]) + ",")
    print("};")
    print("""#ifdef HAVE_NBGL
const nbgl_icon_details_t C_{0} = {{ GLYPH_{0}_WIDTH, GLYPH_{0}_HEIGHT, NBGL_BPP_{1}, GLYPH_{0}_ISFILE, C_{0}_bitmap }};
#endif // HAVE_NBGL
""".format(image_name, bpp))


def print_glyphcheader_data(image_name, bpp, width, height, is_file, image_data):
    if is_file:
        is_file = 'true'
    else:
        is_file = 'false'

    print("""#ifndef GLYPH_{0}_BPP
    #define GLYPH_{0}_WIDTH {1}
    #define GLYPH_{0}_HEIGHT {2}
    #define GLYPH_{0}_ISFILE {3}
    #define GLYPH_{0}_BPP {4}""".format(image_name, width, height, is_file, bpp))
    print("  extern uint8_t const C_{0}_bitmap[{1:d}];".format(
        image_name, len(image_data)))
    print("""  #ifdef HAVE_NBGL
    extern const nbgl_icon_details_t C_{0};
  #endif // HAVE_NBGL
#endif // GLYPH_{0}_BPP
""".format(image_name))


def main():
    parser = argparse.ArgumentParser(
        description='Generate source code for NBGL icons.')
    parser.add_argument('image_file', help="""
                        Icons to process.
                        Images that will be transformed through rotation or symmetry
                        must be suffixed by '_nocomp' (example: image_nocomp.png)
                        """, nargs='+')
    parser.add_argument('--hexbitmaponly', action='store_true')
    parser.add_argument('--glyphcheader', action='store_true')
    parser.add_argument('--glyphcfile', action='store_true')
    parser.add_argument('--reverse', help="Reverse B&W for 1BPP icons", action='store_true')
    args = parser.parse_args()

    # Print C header
    if args.glyphcfile or args.glyphcheader:
        print_glyphfile_top(args.glyphcfile)

    processed_image_names = []
    for file in args.image_file:
        try:
            # Get image name
            image_name = os.path.splitext(os.path.basename(file))[0]

            # if image name has already been done, then don't do it twice
            if image_name in processed_image_names:
                continue
            processed_image_names.append(image_name)

            # Open image in luminance format
            opened = open_image(file)
            if opened is not None:
                im, bpp = opened
            else:
                continue

            if args.hexbitmaponly:
                # Prepare and print app icon data
                _, image_data = compute_app_icon_data(im, bpp, args.reverse)
                print(binascii.hexlify(image_data).decode('utf-8'))
            else:
                # Prepare and print regular icon data
                width, height = im.size

                # Forbid compression if the image name ends with nocomp.
                if image_name.endswith('_nocomp'):
                    no_comp = True
                    image_name = image_name[:-7] # Remove nocomp suffix
                else:
                    no_comp = False

                is_file, image_data = compute_regular_icon_data(no_comp, im, bpp, args.reverse)

                if args.glyphcfile:
                    print_glyphcfile_data(image_name, bpp, image_data)

                if args.glyphcheader:
                    print_glyphcheader_data(
                        image_name, bpp, width, height, is_file, image_data)

        except Exception as e:
            sys.stderr.write(
                    "Exception while processing {}: {}\n".format(file, e))
            try:
                traceback.print_tb()
            except:
                pass


if __name__ == "__main__":
    main()
