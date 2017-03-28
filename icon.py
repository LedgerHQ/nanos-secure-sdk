"""
*******************************************************************************
*   Ledger - Non secure firmware 
*   (c) 2016, 2017 Ledger 
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
import colorsys
import sys
import os
import math
import collections
import traceback

widthmax = 4096
heightmax = 4096
forcedBPP = None
hexbitmaponly = False
glyphcfile = False
glyphcheader = False

try:

	# 2: python imagefile
	if (len(sys.argv) == 2):
		filename = sys.argv[1];
		if not os.path.exists(filename):
			sys.sterr.write(sys.argv[0] + ": [<Wmax> <Hmax>] <imagefile> [<forcedBPP>] [hexbitmaponly|glyphcfile|glyphcheader]\n")
			sys.sterr.write("Error: " + sys.argv[1] + " does not exists !\n")
			sys.exit(2)
	# 3: python imagefile forcedBPP
	# 3: python imagefile hexbitmaponly
	elif (len(sys.argv) == 3):
		filename = sys.argv[1];
		if not os.path.exists(filename):
			sys.sterr.write(sys.argv[0] + ": [<Wmax> <Hmax>] <imagefile> [<forcedBPP>] [hexbitmaponly|glyphcfile|glyphcheader]\n")
			sys.sterr.write("Error: " + filename + " does not exists !\n")
			sys.exit(2)
		if (sys.argv[2] == "hexbitmaponly"):
			hexbitmaponly = True
		elif (sys.argv[2] == "glyphcfile"):
			glyphcfile = True
		elif (sys.argv[2] == "glyphcheader"):
			glyphcheader = True
		else:
			forcedBPP = int(sys.argv[2])
	# 4: python max max imagefile
	# 4: python imagefile forcedBPP hexbitmaponly
	elif (len(sys.argv) == 4):
		filename = sys.argv[1];
		if not os.path.exists(filename):
			filename = sys.argv[3];
			if not os.path.exists(filename):
				sys.sterr.write(sys.argv[0] + ": [<Wmax> <Hmax>] <imagefile> [<forcedBPP>] [hexbitmaponly|glyphcfile|glyphcheader]\n")
				sys.sterr.write("Error: " + filename + " does not exists !\n")
				sys.exit(2)
			widthmax = int(sys.argv[1])
			heightmax = int(sys.argv[2])		
		else:
			forcedBPP = int(sys.argv[2])
			if (sys.argv[3] == "hexbitmaponly"):
				hexbitmaponly = True
			elif (sys.argv[2] == "glyphcfile"):
				glyphcfile = True
			elif (sys.argv[2] == "glyphcheader"):
				glyphcheader = True
			else:
				sys.sterr.write(sys.argv[0] + ": [<Wmax> <Hmax>] <imagefile> [<forcedBPP>] [hexbitmaponly|glyphcfile|glyphcheader]\n")
				sys.exit(2)
	# 5: python max max imagefile forcedBPP
	# 5: python max max imagefile hexbitmaponly
	elif (len(sys.argv) == 5):
		filename = sys.argv[3];
		if not os.path.exists(filename):
			sys.sterr.write(sys.argv[0] + ": [<Wmax> <Hmax>] <imagefile> [<forcedBPP>] [hexbitmaponly|glyphcfile|glyphcheader]\n")
			sys.sterr.write("Error: " + filename + " does not exists !\n")
			sys.exit(2)
		widthmax = int(sys.argv[1])
		heightmax = int(sys.argv[2])		

		if (sys.argv[4] == "hexbitmaponly"):
			hexbitmaponly = True
		elif (sys.argv[2] == "glyphcfile"):
			glyphcfile = True
		elif (sys.argv[2] == "glyphcheader"):
			glyphcheader = True
		else:
			forcedBPP = int(sys.argv[4])

	# 6: python max max imagefile forcedBPP hexbitmaponly
	elif (len(sys.argv) == 6):
		filename = sys.argv[3]
		if not os.path.exists(filename):
			sys.sterr.write(sys.argv[0] + ": [<Wmax> <Hmax>] <imagefile> [<forcedBPP>] [hexbitmaponly|glyphcfile|glyphcheader]\n")
			sys.sterr.write("Error: " + filename + " does not exists !\n")
			sys.exit(2)
		widthmax = int(sys.argv[1])
		heightmax = int(sys.argv[2])		
		forcedBPP = int(sys.argv[4])
		if (sys.argv[5] == "hexbitmaponly"):
			hexbitmaponly = True
		elif (sys.argv[2] == "glyphcfile"):
			glyphcfile = True
		elif (sys.argv[2] == "glyphcheader"):
			glyphcheader = True
		else:
			sys.sterr.write(sys.argv[0] + ": [<Wmax> <Hmax>] <imagefile> [<forcedBPP>] [hexbitmaponly|glyphcfile|glyphcheader]\n")
			sys.sterr.write("Error: " + filename + " does not exists !\n")
			sys.exit(2)
	else:
		sys.sterr.write(sys.argv[0] + ": [<Wmax> <Hmax>] <imagefile> [<forcedBPP>] [hexbitmaponly|glyphcfile|glyphcheader]\n")
		sys.sterr.write("Error: " + sys.argv[1] + " does not exists !\n")
		sys.exit(2)


	im = Image.open(filename);
	im.load()
	width, height = im.size
	colors = {}
	palette = im.getpalette()

	bname = os.path.splitext(os.path.basename(filename))[0]

	if width>widthmax:
		width = widthmax
	if height>heightmax:
		height = heightmax

	def hexbyte(b):
		return hex(0x100+b)[-2:]

	maxcolor=0
	for row in range(height):
		for col in range(width):
			# return an index in the indexed colors list for indexed address spaces
			color_index = im.getpixel((col,row))
			#print color_index
			if (color_index>maxcolor):
				maxcolor = color_index

	if (maxcolor+1 > 16):
		sys.sterr.write("toomuch colors\n")
		sys.exit(3)


	#not a pow 2 max color count
	if (math.pow(2, math.log(maxcolor+1, 2)) != maxcolor+1):
		maxcolor = int(math.pow(2, math.log(maxcolor+1, 2)+1))

	# enforce if lower
	#maxcolor=15

	bits_per_pixel = int(math.log(maxcolor+1, 2))

	# reorder color map by luminance
	opalette = {}
	for i in range(maxcolor+1):
		rd = palette[i*3]
		gn = palette[i*3+1]
		bl = palette[i*3+2]
		h,s,v = colorsys.rgb_to_hsv(rd/255.,gn/255.,bl/255.)

		#multiple color may have the same luminance
		if not v*255. in opalette:
			opalette[v*255.] = []
		opalette[v*255.].append([i, (rd<<16)+(gn<<8)+bl])

	opalette = collections.OrderedDict(sorted(opalette.items()))

	# computed the remapping index
	i = 0
	palette_remapping = {}
	new_palette = []
	for lum, values in opalette.iteritems(): 
		# old index to new index
		for v in values:
			palette_remapping[v[0]] = i
			new_palette.append(v[1])
			i+=1

	# if forced bpp, ensure sufficient color in the palette
	if forcedBPP:
		if i < (1<<(forcedBPP)):
			# padd the color array with dummy color to fit the expected size
			while i < (1<<(forcedBPP)):
				palette_remapping[v[0]] = i
				new_palette.append(0x00000000)
				i+=1

			#sys.stdout.write("ERROR")
			#raise BaseException("Not enough color in the bitmap: " + str(i) + " expected: " + str(1<<(forcedBPP)))

	# display color array

	if not hexbitmaponly:
		if not glyphcfile:
			sys.stdout.write("""#ifndef GLYPH_""" + bname + """_BPP
""")
			sys.stdout.write("""#define GLYPH_""" + bname + """_WIDTH """ + str(width) + """
""")
			sys.stdout.write("""#define GLYPH_""" + bname + """_HEIGHT """ + str(height) + """
""")
			sys.stdout.write("""#define GLYPH_""" + bname + """_BPP """ + str(bits_per_pixel) + """
""")
		else:
			sys.stdout.write("""#include "glyphs.h"
""")
		if glyphcheader:
			sys.stdout.write("""extern
""")		
		sys.stdout.write("""unsigned int const C_""" + bname + """_colors[]
""");
		if glyphcheader:
			sys.stdout.write(""";
""")
		else:
			sys.stdout.write(""" = {
""")
			# color index encoding
			for i in range(maxcolor+1):
				# Endian less value
				sys.stdout.write("  0x00" + hexbyte(new_palette[i]>>16) + hexbyte(new_palette[i]>>8) + hexbyte(new_palette[i]) + ", \n")
			sys.stdout.write("""};
""")
		if glyphcheader:
			sys.stdout.write("""extern""")	
		sys.stdout.write("""	
unsigned char const C_""" + bname + """_bitmap[]""");
		if glyphcheader:
			sys.stdout.write(""";
""")
		else:
			sys.stdout.write(""" = {
""")
	else:
		# write BPP
		sys.stdout.write(hexbyte(bits_per_pixel))
		# write color table U4BE for each
		for i in range(maxcolor+1):
			# LE color array, it is meant to be embedded as is in an array
			sys.stdout.write(hexbyte(new_palette[i]) + hexbyte(new_palette[i]>>8) + hexbyte(new_palette[i]>>16) + "00")


	current_byte = 0
	current_bit = 0
	byte_count = 0

	#packed, row preferred
	if not glyphcheader:
		for row in range(height):
			# row first
			for col in range(width):
				# return an index in the indexed colors list for indexed address spaces
				# left to right
				#perform implicit rotation here (0,0 is left top in bagl, and generally left bottom for various canvas)
				color_index = im.getpixel((col,row))

				#remap index by luminance
				color_index = palette_remapping[color_index]

				# le encoded
				if (forcedBPP) and False:
					current_byte += ((color_index)<<current_bit)<<(forcedBPP-bits_per_pixel)
					current_bit+=forcedBPP
				else:
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
			sys.stdout.write("""};

""")

	if not hexbitmaponly:
		if not glyphcfile and not glyphcheader:
			# origin 0,0 is left top for blue, instead of left bottom for all image encodings
			sys.stdout.write("""
			  { """+str(width)+""", """+str(height)+""", """+str(int(math.log(maxcolor+1, 2)))+""", C_"""+bname+"""_colors, C_"""+bname+"""_bitmap },
""")
		else:
			sys.stdout.write("""#ifdef OS_IO_SEPROXYHAL
#include \"os_io_seproxyhal.h\"
""")	
			if glyphcheader:
						sys.stdout.write("""extern
""")	
			sys.stdout.write("""const bagl_icon_details_t C_""" + bname)

			if glyphcheader: 
				sys.stdout.write(""";
""")
				sys.stdout.write("""#endif // GLYPH_""" + bname + """_BPP
""")
			else:
				sys.stdout.write(" = { GLYPH_" + bname + "_WIDTH, GLYPH_" + bname + "_HEIGHT, " + str(bits_per_pixel) + ", C_" + bname + "_colors, C_" + bname + """_bitmap };
""");
			sys.stdout.write("""#endif // OS_IO_SEPROXYHAL
""")
except:
	sys.stderr.write("An error occured\n")
	#traceback.print_exc()
