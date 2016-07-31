"""
*******************************************************************************
*   Ledger - Non secure firmware 
*   (c) 2016 Ledger 
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
*   Unless required by applicable law or agreed to in writing, software
*   distributed under the License is distributed on an "AS IS" BASIS,
*   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*   See the License for the specific language governing permissions and
*   limitations under the License.
********************************************************************************
"""

from PIL import Image
import sys
import os
import math

if (len(sys.argv) < 4):
	print sys.argv[0] + ": <Wmax> <Hmax> <imagefile> [hexbitmaponly]"
	sys.exit(-1)

hexbitmaponly = False
if (len(sys.argv) == 5):
	if (sys.argv[4] == "hexbitmaponly"):
		hexbitmaponly = True

widthmax = int(sys.argv[1])
heightmax = int(sys.argv[2])
im = Image.open(sys.argv[3]);
im.load()
width, height = im.size
colors = {}
palette = im.getpalette()

bname = os.path.splitext(os.path.basename(sys.argv[3]))[0]

if width>widthmax:
	width = widthmax
if height>heightmax:
	height = heightmax

def hexbyte(b):
	return hex(0x100+b)[3:]

maxcolor=0
for row in range(height):
	for col in range(width):
		# return an index in the indexed colors list for indexed address spaces
		color_index = im.getpixel((col,row))
		if (color_index>maxcolor):
			maxcolor = color_index

if (maxcolor+1 > 16):
	raise "toomuch colors"


#not a pow 2 max color count
if (math.pow(2, math.log(maxcolor+1, 2)) != maxcolor+1):
	maxcolor = int(math.pow(2, math.log(maxcolor+1, 2)+1))

# enforce if lower
#maxcolor=15

# display color array

if not hexbitmaponly:
	sys.stdout.write("""
	unsigned int const C_""" + bname + """_colors[] = {
	""");
	# color index encoding
	for i in range(maxcolor+1):
		sys.stdout.write("  0x00" + hexbyte(palette[i*3]) + hexbyte(palette[i*3+1]) + hexbyte(palette[i*3+2]) + ", \n")
	sys.stdout.write("""
	};

	unsigned char const C_""" + bname + """_bitmap[] = {
	  """);
else:
	# write BPP
	sys.stdout.write(hexbyte(int(math.log(maxcolor+1, 2))))
	# write clor table U4BE for each
	for i in range(maxcolor+1):
		sys.stdout.write("00" + hexbyte(palette[i*3]) + hexbyte(palette[i*3+1]) + hexbyte(palette[i*3+2]))


current_byte = 0
current_bit = 0
bits_per_pixel = int(math.log(maxcolor+1, 2))
byte_count = 0;

#packed, row preferred
for row in range(height):
	# row first
	for col in range(width):
		# return an index in the indexed colors list for indexed address spaces
		# left to right
		#perform implicit rotation here (0,0 is left top in bagl, and generally left bottom for various canvas)
		color_index = im.getpixel((col,row))

		# le encoded
		current_byte += (color_index<<current_bit)
		current_bit+=bits_per_pixel

		if current_bit >= 8:
			if not hexbitmaponly:
				sys.stdout.write("0x" + hexbyte(current_byte) + ", ")
				byte_count+=1
				if (byte_count >= 16):
					byte_count = 0;
					sys.stdout.write("\n  ")
			else:
				sys.stdout.write(hexbyte(current_byte))
			current_bit = 0
			current_byte = 0
		
# print last byte if any
if (current_bit > 0):
	if not hexbitmaponly:
		sys.stdout.write("0x" + hexbyte(current_byte) + ", ")
		byte_count+=1
		if (byte_count >= 16):
			byte_count = 0;
			sys.stdout.write("\n  ")
	else:
		sys.stdout.write(hexbyte(current_byte))


if not hexbitmaponly:
	# origin 0,0 is left top for blue, instead of left bottom for all image encodings
	sys.stdout.write("""
	};

	  { """+str(width)+""", """+str(height)+""", """+str(int(math.log(maxcolor+1, 2)))+""", C_"""+bname+"""_colors, C_"""+bname+"""_bitmap},

	""")
