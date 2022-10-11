#!/usr/bin/env python3
# coding: utf-8
"""
Just a simple script to compress a given file in gzip format
"""
# -----------------------------------------------------------------------------
import argparse
import sys
import gzip

# -----------------------------------------------------------------------------
# Program entry point:
# -----------------------------------------------------------------------------
if __name__ == "__main__":
    # -------------------------------------------------------------------------
    # Parse arguments:
    parser = argparse.ArgumentParser(
        description="Compress the given file in zlib")

    parser.add_argument(
        "-i", "--input",
        dest="input", type=str,
        help="file to compress")

    parser.add_argument(
        "-o", "--output",
        dest="output", type=str,
        help="Output file")
    args = parser.parse_args()

    with open(args.input, 'rb') as f:
        s = f.read()
    z = gzip.compress(s)
    with open(args.output, 'wb') as f:
        f.write(z)
