
/*******************************************************************************
 *   Ledger Nano S - Secure firmware
 *   (c) 2022 Ledger
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

#pragma once

#include "nbgl_types.h"

#if defined(HAVE_LANGUAGE_PACK)
// Structure used for language packs:
typedef struct language_pack {
    uint32_t target_id;             // 0x33000004:NanoX, 0x31100004:NanoS 1.5.x etc
    uint16_t language;              // Language contained in this pack (<NB_LANG)
    uint16_t nb_strings;            // MUST be the same than NB_BOLOS_UX_LOC_STRINGS
    uint16_t nb_fonts;              // Number of fonts
    uint16_t nb_characters;         // Number of unicode characters in each font
    uint16_t characters_offset;     // Offset to the characters x nb_fonts
    uint16_t fonts_offset;          // Offset to the nbgl_font_unicode_t structures
    uint16_t bitmaps_offset;        // Offset to the bitmaps x nb_fonts
    uint16_t bolos_version_offset;  // Offset to the Bolos version number string
    uint16_t pack_version_offset;   // Offset to the Language Pack version number string
    uint16_t offsets[];             // Offsets from the BEGINNING of the language pack
} LANGUAGE_PACK;

// A language pack is a binary file containing:
/*
- the language pack header, including infos about used fonts & glyphs
- offsets for each string (2 x nb_strings), from the beginning of the pack!
- 2 bytes padding if nb_strings is odd
- nbgl_font_unicode_character_t x nb_characters x 4 fonts
- nbgl_font_unicode_t x 4 fonts
- bitmap data for font 1
- bitmap data for font 2
- bitmap data for font 3
- bitmap data for font 4
- a NULL terminated string containing Bolos version number
- a NULL terminated string containing Language Pack version number
- all strings, concatenated.

NB:
- there are 197 countries in the world, so 1 byte might be enough to store language information,
unless we plan to integrate dialects (>7000) => use a u_int16_t;
- we may store some CRC, to check language pack's integrity.
*/

typedef unsigned short UX_LOC_STRINGS_INDEX;

#endif  // defined(HAVE_LANGUAGE_PACK)
