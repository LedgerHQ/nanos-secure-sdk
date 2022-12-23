
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

// Nano S
#include "bagl_font_open_sans_extrabold_11px.inc"
#include "bagl_font_open_sans_light_16px.inc"
#include "bagl_font_open_sans_regular_11px.inc"
#if defined(HAVE_UNICODE_SUPPORT)
#if !defined(HAVE_LANGUAGE_PACK)
  #if defined(DISPLAY_FLOWS)
    #include "bagl_font_open_sans_extrabold_11px_unicode.inc"
    #include "bagl_font_open_sans_light_16px_unicode.inc"
    #include "bagl_font_open_sans_regular_11px_unicode.inc"
  #else //defined(DISPLAY_FLOWS)
    #if (OS_LANGUAGE==LANG_ENGLISH)
      #include "bolos_pack_en_glyphes.inc"
    #elif (OS_LANGUAGE==LANG_FRENCH)
      #include "bolos_pack_fr_glyphes.inc"
    #elif (OS_LANGUAGE==LANG_SPANISH)
      #include "bolos_pack_es_glyphes.inc"
    #else //OS_LANGUAGE
      #error No default language defined!
    #endif //OS_LANGUAGE
  #endif //defined(DISPLAY_FLOWS)
#endif //!defined(HAVE_LANGUAGE_PACK)
#endif //defined(HAVE_UNICODE_SUPPORT)
