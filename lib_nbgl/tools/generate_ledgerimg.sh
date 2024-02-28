# Generate a ledgerimg from any input image file
#
# sh tools/generate_ledgerimg.sh <IMAGE_FILE> [<PRODUCT>]
#
# where:
#  - <FILE_TO_COMPRESS> is the path to the input image file
#  - <PRODUCT> can be --europa (--stax is the default)
#
# Example for Stax:
#
# ```
# sh tools/generate_ledgerimg.sh ~/Downloads/cryptopunk.png
# Write /home/abonnaudet/Downloads/cryptopunk.bmp
# Write /home/abonnaudet/Downloads/cryptopunk.ledgerimg
# ```
# Example for Europa:
#
# ```
# sh tools/generate_ledgerimg.sh ~/Downloads/cryptopunk.png --europa
# Write /home/abonnaudet/Downloads/cryptopunk.bmp
# Write /home/abonnaudet/Downloads/cryptopunk.ledgerimg
# ```

## Product detection for screen height & width
if [ "$2" = "--europa" ]
then
	FRONT_SCREEN_WIDTH=480
	FRONT_SCREEN_HEIGHT=600
else
	FRONT_SCREEN_WIDTH=400
	FRONT_SCREEN_HEIGHT=672
fi

# Path to bmp2display python script
BMP2DISPLAY_PY=$BOLOS_NG/public_sdk/lib_nbgl/tools/bmp2display.py

# Input file is the first argument
FILE_INPUT=$1

## Generate a 24 bits BMP file
FRONT_SCREEN_DIM=$FRONT_SCREEN_WIDTH"x"$FRONT_SCREEN_HEIGHT
FILE_OUTPUT_BMP=${FILE_INPUT%.*}.bmp
convert $FILE_INPUT -resize $FRONT_SCREEN_DIM -background white -compose Copy -gravity center -type truecolor -extent $FRONT_SCREEN_DIM $FILE_OUTPUT_BMP
echo "Write" $FILE_OUTPUT_BMP

## Convert BMP file into ledgerimg
FILE_OUTPUT_LEDGERIMG=${FILE_INPUT%.*}.ledgerimg
python3 $BMP2DISPLAY_PY --file --compress --input $FILE_OUTPUT_BMP --bpp 4 --outfile $FILE_OUTPUT_LEDGERIMG
