#!/usr/bin/env python3
# coding: utf-8
"""
Converts input bmp file in 24 bits format to Stax bitmap, as buffer in stdout
"""
# -----------------------------------------------------------------------------
import argparse
import gzip

def get4BPPval(buffer,i):
  pixel_val = buffer[i]+(buffer[i+1]<<8)+(buffer[i+2]<<16)

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

def parse_bmp_file(bmp_file_name: str, bpp: int):
    pixels_buffer = []
    with open(bmp_file_name, 'rb') as f:
        buffer = f.read()
        offset = buffer[0xA] + (buffer[0xB]<<8) + (buffer[0xC]<<16) + (buffer[0xD]<<24)
        width = buffer[0x12] + (buffer[0x13]<<8)
        height = buffer[0x16] + (buffer[0x17]<<8)
        bmp_bpp = buffer[0x1C] + (buffer[0x1D]<<8)
        row_size = int(((int(bmp_bpp)*int(width)+31)/32))<<2

        #BMP header is <offset> bytes long in 24BPP mode
        #lines are given from bottom to top, from left to right
        nb_bytes = 0
        cur_byte = 0
        pixel_idx = 0
        line = '  '
        for x in range(width):
            for y in range (height-1,-1,-1):
                i = int(offset+(y*row_size+x*bmp_bpp/8))
                if (bpp == 1):
                    bpp_format = 0
                    if (buffer[i] != 0xFF):
                        cur_byte |= (1<<(7-pixel_idx))
                elif (bpp == 2):
                    bpp_format = 1
                    pixel_val = buffer[i]+(buffer[i+1]<<8)+(buffer[i+2]<<16)
                    if (pixel_val == 0xFFFFFF):
                        cur_byte |= (0x3<<(6-pixel_idx))

                    elif (pixel_val > 0):
                        cur_byte |= (0x2<<(6-pixel_idx))
                elif (bpp == 4):
                    bpp_format = 2
                    cur_byte |= get4BPPval(buffer, i)<<(4-pixel_idx)

                pixel_idx+=bpp
                # the byte is fully filled, let's save its content
                if (pixel_idx%8 == 0):
                    pixels_buffer.append(cur_byte)
                    nb_bytes+=1
                    cur_byte = 0
                    pixel_idx = 0

    return (bytes(pixels_buffer), width, height)

def apply_compression(pixels: bytearray) -> bytes:
    output_buffer = []
    # cut into chunks of 2048 bytes max of uncompressed data (because decompression needs the full buffer)
    full_uncompressed_size = len(pixels)
    i = 0
    while full_uncompressed_size>0:
        chunk_size = min(2048,full_uncompressed_size)
        tmp = bytes(pixels_buffer[i:i+chunk_size])
        #print("len = %d"%len(tmp))
        #print("0x%X"%tmp[chunk_size-1])
        compressed_buffer = gzip.compress(tmp)
        output_buffer += [len(compressed_buffer)&0xFF, (len(compressed_buffer)>>8)&0xFF]
        output_buffer += compressed_buffer
        full_uncompressed_size -= chunk_size
        i+=chunk_size

    return bytearray(output_buffer)

def format_image(img: bytearray, width: int, height: int, compression: bool) -> bytes:
    BPP_FORMATS = {
    1: 0,
    2: 1,
    4: 2
    }

    result = [width&0xFF, width>>8, height&0xFF, height>>8,
        (BPP_FORMATS[args.bpp]<<4) | compression, len(img)&0xFF, (len(img)>>8)&0xFF,
        (len(img)>>16)&0xFF]
    result.extend(output_buffer)
    return bytearray(result)


# -----------------------------------------------------------------------------
# Program entry point:
# -----------------------------------------------------------------------------
if __name__ == "__main__":
    # -------------------------------------------------------------------------
    # Parse arguments:
    parser = argparse.ArgumentParser(
        description="Converts input bmp file in 24 bits format to Stax bitmap, as buffer in stdout")

    parser.add_argument(
        "-i", "--input",
        dest="input", type=str,
        required=True,
        help="bmp file")

    parser.add_argument(
        "-b", "--bpp",
        dest="bpp", type=int,
        required=True,
        help="number of bit per pixel (1, 2 or 4)")

    parser.add_argument(
        '--compress',
        action='store_true',
        default=False,
        help="compress data")

    parser.add_argument(
        '--file',
        action='store_true',
        default=False,
        help="store in Ledger image format")

    parser.add_argument(
      '--outfile',
      dest='outfile', type=str,
      required=False,
      help='Optional outfile name'
    )

    parser.add_argument(
        '--check-size',
        dest='check_size', type=str,
        required=False,
        help='Check size of the input bmp (example: 480x512)'
    )

    args = parser.parse_args()

    pixels_buffer, width, height = parse_bmp_file(args.input, args.bpp)

    # Check size
    if args.check_size:
        xref, yref = args.check_size.split('x')
        assert ((int(xref), int(yref)) == (width, height)), \
          f" Input file does not match required size {args.check_size}"

    # Apply compression
    if args.compress:
      output_buffer = apply_compression(bytes(pixels_buffer))
    else:
      output_buffer = pixels_buffer

    # Apply file formatting
    if args.file:
        result = format_image(output_buffer, width, height, args.compress)
    else:
        result = output_buffer

    if args.outfile is None:
      # Print result
      print(result.hex())
    else:
      # Write to output file
      with open(args.outfile, 'wb') as out_file:
        print(f'Write {out_file.name}')
        out_file.write(bytes(result))
