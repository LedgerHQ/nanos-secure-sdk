
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

#include "string.h"
#include "bagl.h"

/*******************************************************************************
 * FONT REQUIRED FORMAT                                                        *
 * Data length: 8 bits                                                         *
 * Invert bits: No                                                             *
 * Data format: Little Endian, Row based, Row preferred, Packed                *
 *******************************************************************************/

#include "bagl_font_rom.inc"

const bagl_font_t fontNONE = {
	-1UL, /* last font id */
	 0 , /* font bit per pixels */
   0 , /* font height in pixels   */
   0 , /* baseline distance from top */
   0, /* kerning */
   0x0000, /* first character */
   0x0000, /* last character  */
   NULL,  /* address of first character */
   NULL   /* bitmap address*/
};

const bagl_font_t* const C_bagl_fonts[] = {

#include "bagl_font_rom_struct.inc"


};

const unsigned int C_bagl_fonts_count = sizeof(C_bagl_fonts)/sizeof(C_bagl_fonts[0]);

