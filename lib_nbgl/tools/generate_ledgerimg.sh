# Generate a ledgerimg from any input image file
#
# Example:
# ```
# sh tools/generate_ledgerimg.sh ~/Downloads/cryptopunk.png
# Write /home/abonnaudet/Downloads/cryptopunk.bmp
# Write /home/abonnaudet/Downloads/cryptopunk.ledgerimg
# ```

## Configs

# Front screen dimension
FRONT_SCREEN_WIDTH=400
FRONT_SCREEN_HEIGHT=672

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
