/*******************************************************************************
*   Ledger Nano S - Secure firmware
*   (c) 2016, 2017, 2018, 2019 Ledger
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

// gloomy fake definition to avoid problem with O3 and llvm with ignored return value in the caller
unsigned int pic_internal(unsigned int link_address);

// only apply PIC conversion if link_address is in linked code (over 0xC0D00000 in our example)
// this way, PIC call are armless if the address is not meant to be converted
extern unsigned int _nvram;
extern unsigned int _envram;
unsigned int pic(unsigned int link_address) {
//  screen_printf(" %08X", link_address);
	if (link_address >= ((unsigned int)&_nvram) && link_address < ((unsigned int)&_envram)) {
		link_address = pic_internal(link_address);
//    screen_printf(" -> %08X\n", link_address);
  }
	return link_address;
}

