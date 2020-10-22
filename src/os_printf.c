
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

#include <stdarg.h>
#include <string.h>

#include "os.h"

#if defined(HAVE_PRINTF) || defined(HAVE_SPRINTF)

static const char g_pcHex[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f',
};
static const char g_pcHex_cap[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
};
#endif // defined(HAVE_PRINTF) || defined(HAVE_SPRINTF)

#ifdef HAVE_PRINTF
#include "os_io_seproxyhal.h"
#include "usbd_def.h"
#include "usbd_core.h"

#ifndef BOLOS_RELEASE

#ifdef TARGET_NANOX
void mcu_usb_prints(const char* str, unsigned int charcount) {
  while(charcount--) {
    mcu_usb_printc(*str++);
  }
}

#else
void mcu_usb_prints(const char* str, unsigned int charcount) {
  if(USBD_Device.dev_state != USBD_STATE_CONFIGURED){
    return;
  }
  unsigned char buf[4];
  if(io_seproxyhal_spi_is_status_sent()){
      io_seproxyhal_spi_recv(buf, 3, 0);
  }
  buf[0] = SEPROXYHAL_TAG_PRINTF_STATUS;
  buf[1] = charcount >> 8;
  buf[2] = charcount;
  io_seproxyhal_spi_send(buf, 3);
  io_seproxyhal_spi_send((const unsigned char *)str, charcount);
#ifndef IO_SEPROXYHAL_DEBUG
  // wait printf ack (no race kthx)
  io_seproxyhal_spi_recv(buf, 3, 0);
  buf[0] = 0; // consume tag to avoid misinterpretation (due to IO_CACHE)
#endif // IO_SEPROXYHAL_DEBUG
}
#endif // TARGET_NANOX

/**
 * Common printf code, relies on 2 FAL:
 * - mcu_usb_prints
 * - screen_printc
 */

void screen_printf(const char* format, ...) __attribute__ ((weak, alias ("mcu_usb_printf")));

void mcu_usb_printf(const char* format, ...) {

    /* dummy version
    unsigned short i;
    unsigned short len = strlen(str);
    for(i=0;i<len;i++){

        screen_printc(str[i]);
    }
    */

    unsigned long ulIdx, ulValue, ulPos, ulCount, ulBase, ulNeg, ulStrlen, ulCap;
    char *pcStr, pcBuf[16], cFill;
    va_list vaArgP;
    char cStrlenSet;

    //
    // Check the arguments.
    //
    if(format == 0) {
      return;
    }

    //
    // Start the varargs processing.
    //
    va_start(vaArgP, format);

    //
    // Loop while there are more characters in the string.
    //
    while(*format)
    {
        //
        // Find the first non-% character, or the end of the string.
        //
        for(ulIdx = 0; (format[ulIdx] != '%') && (format[ulIdx] != '\0');
            ulIdx++)
        {
        }

        //
        // Write this portion of the string.
        //
        mcu_usb_prints(format, ulIdx);

        //
        // Skip the portion of the string that was written.
        //
        format += ulIdx;

        //
        // See if the next character is a %.
        //
        if(*format == '%')
        {
            //
            // Skip the %.
            //
            format++;

            //
            // Set the digit count to zero, and the fill character to space
            // (i.e. to the defaults).
            //
            ulCount = 0;
            cFill = ' ';
            ulStrlen = 0;
            cStrlenSet = 0;
            ulCap = 0;
            ulBase = 10;

            //
            // It may be necessary to get back here to process more characters.
            // Goto's aren't pretty, but effective.  I feel extremely dirty for
            // using not one but two of the beasts.
            //
again:

            //
            // Determine how to handle the next character.
            //
            switch(*format++)
            {
                //
                // Handle the digit characters.
                //
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                {
                    //
                    // If this is a zero, and it is the first digit, then the
                    // fill character is a zero instead of a space.
                    //
                    if((format[-1] == '0') && (ulCount == 0))
                    {
                        cFill = '0';
                    }

                    //
                    // Update the digit count.
                    //
                    ulCount *= 10;
                    ulCount += format[-1] - '0';

                    //
                    // Get the next character.
                    //
                    goto again;
                }

                //
                // Handle the %c command.
                //
                case 'c':
                {
                    //
                    // Get the value from the varargs.
                    //
                    ulValue = va_arg(vaArgP, unsigned long);

                    //
                    // Print out the character.
                    //
                    mcu_usb_prints((char *)&ulValue, 1);

                    //
                    // This command has been handled.
                    //
                    break;
                }

                //
                // Handle the %d command.
                //
                case 'd':
                {
                    //
                    // Get the value from the varargs.
                    //
                    ulValue = va_arg(vaArgP, unsigned long);

                    //
                    // Reset the buffer position.
                    //
                    ulPos = 0;

                    //
                    // If the value is negative, make it positive and indicate
                    // that a minus sign is needed.
                    //
                    if((long)ulValue < 0)
                    {
                        //
                        // Make the value positive.
                        //
                        ulValue = -(long)ulValue;

                        //
                        // Indicate that the value is negative.
                        //
                        ulNeg = 1;
                    }
                    else
                    {
                        //
                        // Indicate that the value is positive so that a minus
                        // sign isn't inserted.
                        //
                        ulNeg = 0;
                    }

                    //
                    // Set the base to 10.
                    //
                    ulBase = 10;

                    //
                    // Convert the value to ASCII.
                    //
                    goto convert;
                }

                //
                // Handle ths %.*s command
                // special %.*H or %.*h format to print a given length of hex digits (case: H UPPER, h lower)
                //
                case '.':
                {
                  // ensure next char is '*' and next one is 's'
                  if (format[0] == '*' && (format[1] == 's' || format[1] == 'H' || format[1] == 'h')) {
                    
                    // skip '*' char
                    format++;
                    
                    ulStrlen = va_arg(vaArgP, unsigned long);
                    cStrlenSet = 1;
                    
                    // interpret next char (H/h/s)
                    goto again;
                  }
                  
                  // does not support %.2x for example
                  goto error;
                }
                
                case '*':
                {
                  if (*format == 's' ) {                    
                    
                    ulStrlen = va_arg(vaArgP, unsigned long);
                    cStrlenSet = 2;
                    goto again;
                  }
                  
                  goto error;
                }
                
                case '-': // -XXs
                {
                  cStrlenSet = 0;
                  // read a number of space to post pad with ' ' the string to display
                  goto again;
                }

                //
                // Handle the %s command.
                // %H and %h also
                case 'H':
                  ulCap = 1; // uppercase base 16
                  ulBase = 16;
                  goto case_s;
                case 'h':
                  ulCap = 0;
                  ulBase = 16; // lowercase base 16
                  goto case_s;
                case 's':
                case_s:
                {
                    //
                    // Get the string pointer from the varargs.
                    //
                    pcStr = va_arg(vaArgP, char *);

                    //
                    // Determine the length of the string. (if not specified using .*)
                    //
                    switch(cStrlenSet) {
                      // compute length with strlen
                      case 0:
                        for(ulIdx = 0; pcStr[ulIdx] != '\0'; ulIdx++)
                        {
                        }
                        break;
                        
                      // use given length
                      case 1:
                        ulIdx = ulStrlen;
                        break;
                        
                      // printout prepad
                      case 2:
                        // if string is empty, then, ' ' padding
                        if (pcStr[0] == '\0') {
                        
                          // padd ulStrlen white space
                          do {
                            mcu_usb_prints(" ", 1);
                          } while(ulStrlen-- > 0);
                        
                          goto s_pad;
                        }
                        goto error; // unsupported if replicating the same string multiple times
                      case 3:
                        // skip '-' still buggy ...
                        goto again;
                    }

                    //
                    // Write the string.
                    //
                    switch(ulBase) {
                      default:
                        mcu_usb_prints(pcStr, ulIdx);
                        break;
                      case 16: {
                        unsigned char nibble1, nibble2;
                        for (ulCount = 0; ulCount < ulIdx; ulCount++) {
                          nibble1 = (pcStr[ulCount]>>4)&0xF;
                          nibble2 = pcStr[ulCount]&0xF;
                          switch(ulCap) {
                            case 0:
                              mcu_usb_printc(g_pcHex[nibble1]);
                              mcu_usb_printc(g_pcHex[nibble2]);
                              break;
                            case 1:
                              mcu_usb_printc(g_pcHex_cap[nibble1]);
                              mcu_usb_printc(g_pcHex_cap[nibble2]);
                              break;
                          }
                        }
                        break;
                      }
                    }

s_pad:
                    //
                    // Write any required padding spaces
                    //
                    if(ulCount > ulIdx)
                    {
                        ulCount -= ulIdx;
                        while(ulCount--)
                        {
                            mcu_usb_prints(" ", 1);
                        }
                    }
                    //
                    // This command has been handled.
                    //
                    break;
                }

                //
                // Handle the %u command.
                //
                case 'u':
                {
                    //
                    // Get the value from the varargs.
                    //
                    ulValue = va_arg(vaArgP, unsigned long);

                    //
                    // Reset the buffer position.
                    //
                    ulPos = 0;

                    //
                    // Set the base to 10.
                    //
                    ulBase = 10;

                    //
                    // Indicate that the value is positive so that a minus sign
                    // isn't inserted.
                    //
                    ulNeg = 0;

                    //
                    // Convert the value to ASCII.
                    //
                    goto convert;
                }

                //
                // Handle the %x and %X commands.  Note that they are treated
                // identically; i.e. %X will use lower case letters for a-f
                // instead of the upper case letters is should use.  We also
                // alias %p to %x.
                //
                case 'X':
                    ulCap = 1;
                case 'x':
                case 'p':
                {
                    //
                    // Get the value from the varargs.
                    //
                    ulValue = va_arg(vaArgP, unsigned long);

                    //
                    // Reset the buffer position.
                    //
                    ulPos = 0;

                    //
                    // Set the base to 16.
                    //
                    ulBase = 16;

                    //
                    // Indicate that the value is positive so that a minus sign
                    // isn't inserted.
                    //
                    ulNeg = 0;

                    //
                    // Determine the number of digits in the string version of
                    // the value.
                    //
convert:
                    for(ulIdx = 1;
                        (((ulIdx * ulBase) <= ulValue) &&
                         (((ulIdx * ulBase) / ulBase) == ulIdx));
                        ulIdx *= ulBase, ulCount--)
                    {
                    }

                    //
                    // If the value is negative, reduce the count of padding
                    // characters needed.
                    //
                    if(ulNeg)
                    {
                        ulCount--;
                    }

                    //
                    // If the value is negative and the value is padded with
                    // zeros, then place the minus sign before the padding.
                    //
                    if(ulNeg && (cFill == '0'))
                    {
                        //
                        // Place the minus sign in the output buffer.
                        //
                        pcBuf[ulPos++] = '-';

                        //
                        // The minus sign has been placed, so turn off the
                        // negative flag.
                        //
                        ulNeg = 0;
                    }

                    //
                    // Provide additional padding at the beginning of the
                    // string conversion if needed.
                    //
                    if((ulCount > 1) && (ulCount < 16))
                    {
                        for(ulCount--; ulCount; ulCount--)
                        {
                            pcBuf[ulPos++] = cFill;
                        }
                    }

                    //
                    // If the value is negative, then place the minus sign
                    // before the number.
                    //
                    if(ulNeg)
                    {
                        //
                        // Place the minus sign in the output buffer.
                        //
                        pcBuf[ulPos++] = '-';
                    }

                    //
                    // Convert the value into a string.
                    //
                    for(; ulIdx; ulIdx /= ulBase)
                    {
                        if (!ulCap) {
                          pcBuf[ulPos++] = g_pcHex[(ulValue / ulIdx) % ulBase];
                        }
                        else {
                          pcBuf[ulPos++] = g_pcHex_cap[(ulValue / ulIdx) % ulBase];
                        }
                    }

                    //
                    // Write the string.
                    //
                    mcu_usb_prints(pcBuf, ulPos);

                    //
                    // This command has been handled.
                    //
                    break;
                }

                //
                // Handle the %% command.
                //
                case '%':
                {
                    //
                    // Simply write a single %.
                    //
                    mcu_usb_prints(format - 1, 1);

                    //
                    // This command has been handled.
                    //
                    break;
                }

error:
                //
                // Handle all other commands.
                //
                default:
                {
                    //
                    // Indicate an error.
                    //
                    mcu_usb_prints("ERROR", 5);

                    //
                    // This command has been handled.
                    //
                    break;
                }
            }
        }
    }

    //
    // End the varargs processing.
    //
    va_end(vaArgP);
}
#endif // BOLOS_RELEASE
 
#endif // HAVE_PRINTF

#ifdef HAVE_SPRINTF
//unsigned int snprintf(unsigned char * str, unsigned int str_size, const char* format, ...)
int snprintf(char * str, size_t str_size, const char * format, ...)
 {
    unsigned int ulIdx, ulValue, ulPos, ulCount, ulBase, ulNeg, ulStrlen, ulCap;
    char *pcStr, pcBuf[16], cFill;
    va_list vaArgP;
    char cStrlenSet;
    
    //
    // Check the arguments.
    //
    if(format == NULL || str == NULL ||str_size < 2) {
      return 0;
    }

    // ensure terminating string with a \0
    memset(str, 0, str_size);
    str_size--;


    //
    // Start the varargs processing.
    //
    va_start(vaArgP, format);

    //
    // Loop while there are more characters in the string.
    //
    while(*format)
    {
        //
        // Find the first non-% character, or the end of the string.
        //
        for(ulIdx = 0; (format[ulIdx] != '%') && (format[ulIdx] != '\0');
            ulIdx++)
        {
        }

        //
        // Write this portion of the string.
        //
        ulIdx = MIN(ulIdx, str_size);
        memmove(str, format, ulIdx);
        str+= ulIdx;
        str_size -= ulIdx;
        if (str_size == 0) {
            return 0;
        }

        //
        // Skip the portion of the string that was written.
        //
        format += ulIdx;

        //
        // See if the next character is a %.
        //
        if(*format == '%')
        {
            //
            // Skip the %.
            //
            format++;

            //
            // Set the digit count to zero, and the fill character to space
            // (i.e. to the defaults).
            //
            ulCount = 0;
            cFill = ' ';
            ulStrlen = 0;
            cStrlenSet = 0;
            ulCap = 0;
            ulBase = 10;

            //
            // It may be necessary to get back here to process more characters.
            // Goto's aren't pretty, but effective.  I feel extremely dirty for
            // using not one but two of the beasts.
            //
again:

            //
            // Determine how to handle the next character.
            //
            switch(*format++)
            {

                //
                // Handle the digit characters.
                //
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                {
                    //
                    // If this is a zero, and it is the first digit, then the
                    // fill character is a zero instead of a space.
                    //
                    if((format[-1] == '0') && (ulCount == 0))
                    {
                        cFill = '0';
                    }

                    //
                    // Update the digit count.
                    //
                    ulCount *= 10;
                    ulCount += format[-1] - '0';

                    //
                    // Get the next character.
                    //
                    goto again;
                }

                //
                // Handle the %c command.
                //
                case 'c':
                {
                    //
                    // Get the value from the varargs.
                    //
                    ulValue = va_arg(vaArgP, unsigned long);

                    //
                    // Print out the character.
                    //
                    str[0] = ulValue;
                    str++;
                    str_size -= 1;
                    if (str_size == 0) {
                        return 0;
                    }

                    //
                    // This command has been handled.
                    //
                    break;
                }

                //
                // Handle the %d command.
                //
                case 'd':
                {
                    //
                    // Get the value from the varargs.
                    //
                    ulValue = va_arg(vaArgP, unsigned long);

                    //
                    // Reset the buffer position.
                    //
                    ulPos = 0;

                    //
                    // If the value is negative, make it positive and indicate
                    // that a minus sign is needed.
                    //
                    if((long)ulValue < 0)
                    {
                        //
                        // Make the value positive.
                        //
                        ulValue = -(long)ulValue;

                        //
                        // Indicate that the value is negative.
                        //
                        ulNeg = 1;
                    }
                    else
                    {
                        //
                        // Indicate that the value is positive so that a minus
                        // sign isn't inserted.
                        //
                        ulNeg = 0;
                    }

                    //
                    // Set the base to 10.
                    //
                    ulBase = 10;

                    //
                    // Convert the value to ASCII.
                    //
                    goto convert;
                }

                //
                // Handle ths %.*s command
                // special %.*H or %.*h format to print a given length of hex digits (case: H UPPER, h lower)
                //
                case '.':
                {
                  // ensure next char is '*' and next one is 's'/'h'/'H'
                  if (format[0] == '*' && (format[1] == 's' || format[1] == 'H' || format[1] == 'h')) {
                    
                    // skip '*' char
                    format++;
                    
                    ulStrlen = va_arg(vaArgP, unsigned long);
                    cStrlenSet = 1;
                    
                    // interpret next char (H/h/s)
                    goto again;
                  }
                  
                  // does not support %.2x for example
                  goto error;
                }
                
                case '*':
                {
                  if (*format == 's' ) {                    
                    
                    ulStrlen = va_arg(vaArgP, unsigned long);
                    cStrlenSet = 2;
                    goto again;
                  }
                  
                  goto error;
                }
                
                case '-': // -XXs
                {
                  cStrlenSet = 0;
                  // read a number of space to post pad with ' ' the string to display
                  goto again;
                }

                //
                // Handle the %s command.
                // %H and %h also
                case 'H':
                  ulCap = 1; // uppercase base 16
                  ulBase = 16;
                  goto case_s;
                case 'h':
                  ulBase = 16; // lowercase base 16
                  goto case_s;
                case 's':
                case_s:
                {
                    //
                    // Get the string pointer from the varargs.
                    //
                    pcStr = va_arg(vaArgP, char *);

                    //
                    // Determine the length of the string. (if not specified using .*)
                    //
                    switch(cStrlenSet) {
                      // compute length with strlen
                      case 0:
                        for(ulIdx = 0; pcStr[ulIdx] != '\0'; ulIdx++)
                        {
                        }
                        break;
                        
                      // use given length
                      case 1:
                        ulIdx = ulStrlen;
                        break;
                        
                      // printout prepad
                      case 2:
                        // if string is empty, then, ' ' padding
                        if (pcStr[0] == '\0') {
                        
                          // padd ulStrlen white space
                          ulStrlen = MIN(ulStrlen, str_size);
                          memset(str, ' ', ulStrlen);
                          str+= ulStrlen;
                          str_size -= ulStrlen;
                          if (str_size == 0) {
                              return 0;
                          }
                        
                          goto s_pad;
                        }
                        goto error; // unsupported if replicating the same string multiple times
                      case 3:
                        // skip '-' still buggy ...
                        goto again;
                    }

                    //
                    // Write the string.
                    //
                    switch(ulBase) {
                      default:
                        ulIdx = MIN(ulIdx, str_size);
                        memmove(str, pcStr, ulIdx);
                        str+= ulIdx;
                        str_size -= ulIdx;
                        if (str_size == 0) {
                            return 0;
                        }
                        break;
                      case 16: {
                        unsigned char nibble1, nibble2;
                        for (ulCount = 0; ulCount < ulIdx; ulCount++) {
                          nibble1 = (pcStr[ulCount]>>4)&0xF;
                          nibble2 = pcStr[ulCount]&0xF;
                          if (str_size < 2) {
                              return 0;
                          }
                          switch(ulCap) {
                            case 0:
                                str[0] = g_pcHex[nibble1];
                                str[1] = g_pcHex[nibble2];
                                break;
                            case 1:
                                str[0] = g_pcHex_cap[nibble1];
                                str[1] = g_pcHex_cap[nibble2];
                              break;
                          }
                          str+= 2;
                          str_size -= 2;
                          if (str_size == 0) {
                              return 0;
                          }
                        }
                        break;
                      }
                    }

s_pad:
                    //
                    // Write any required padding spaces
                    //
                    if(ulCount > ulIdx)
                    {
                        ulCount -= ulIdx;
                        ulCount = MIN(ulCount, str_size);
                        memset(str, ' ', ulCount);
                        str+= ulCount;
                        str_size -= ulCount;
                        if (str_size == 0) {
                            return 0;
                        }
                    }
                    //
                    // This command has been handled.
                    //
                    break;
                }

#ifdef HAVE_SNPRINTF_FORMAT_U
                //
                // Handle the %u command.
                //
                case 'u':
                {
                    //
                    // Get the value from the varargs.
                    //
                    ulValue = va_arg(vaArgP, unsigned long);

                    //
                    // Reset the buffer position.
                    //
                    ulPos = 0;

                    //
                    // Set the base to 10.
                    //
                    ulBase = 10;

                    //
                    // Indicate that the value is positive so that a minus sign
                    // isn't inserted.
                    //
                    ulNeg = 0;

                    //
                    // Convert the value to ASCII.
                    //
                    goto convert;
                }
#endif // HAVE_SNPRINTF_FORMAT_U

                //
                // Handle the %x and %X commands.  Note that they are treated
                // identically; i.e. %X will use lower case letters for a-f
                // instead of the upper case letters is should use.  We also
                // alias %p to %x.
                //
                case 'X':
                    ulCap = 1;
                    /* fallthrough */
                case 'x':
                case 'p':
                {
                    //
                    // Get the value from the varargs.
                    //
                    ulValue = va_arg(vaArgP, unsigned long);

                    //
                    // Reset the buffer position.
                    //
                    ulPos = 0;

                    //
                    // Set the base to 16.
                    //
                    ulBase = 16;

                    //
                    // Indicate that the value is positive so that a minus sign
                    // isn't inserted.
                    //
                    ulNeg = 0;

                    //
                    // Determine the number of digits in the string version of
                    // the value.
                    //
convert:
                    for(ulIdx = 1;
                        (((ulIdx * ulBase) <= ulValue) &&
                         (((ulIdx * ulBase) / ulBase) == ulIdx));
                        ulIdx *= ulBase, ulCount--)
                    {
                    }

                    //
                    // If the value is negative, reduce the count of padding
                    // characters needed.
                    //
                    if(ulNeg)
                    {
                        ulCount--;
                    }

                    //
                    // If the value is negative and the value is padded with
                    // zeros, then place the minus sign before the padding.
                    //
                    if(ulNeg && (cFill == '0'))
                    {
                        //
                        // Place the minus sign in the output buffer.
                        //
                        pcBuf[ulPos++] = '-';

                        //
                        // The minus sign has been placed, so turn off the
                        // negative flag.
                        //
                        ulNeg = 0;
                    }

                    //
                    // Provide additional padding at the beginning of the
                    // string conversion if needed.
                    //
                    if((ulCount > 1) && (ulCount < 16))
                    {
                        for(ulCount--; ulCount; ulCount--)
                        {
                            pcBuf[ulPos++] = cFill;
                        }
                    }

                    //
                    // If the value is negative, then place the minus sign
                    // before the number.
                    //
                    if(ulNeg)
                    {
                        //
                        // Place the minus sign in the output buffer.
                        //
                        pcBuf[ulPos++] = '-';
                    }

                    //
                    // Convert the value into a string.
                    //
                    for(; ulIdx; ulIdx /= ulBase)
                    {
                        if (!ulCap) {
                          pcBuf[ulPos++] = g_pcHex[(ulValue / ulIdx) % ulBase];
                        }
                        else {
                          pcBuf[ulPos++] = g_pcHex_cap[(ulValue / ulIdx) % ulBase];
                        }
                    }

                    //
                    // Write the string.
                    //
                    ulPos = MIN(ulPos, str_size);
                    memmove(str, pcBuf, ulPos);
                    str+= ulPos;
                    str_size -= ulPos;
                    if (str_size == 0) {
                        return 0;
                    }

                    //
                    // This command has been handled.
                    //
                    break;
                }

                //
                // Handle the %% command.
                //
                case '%':
                {
                    //
                    // Simply write a single %.
                    //
                    str[0] = '%';
                    str++;
                    str_size --;
                    if (str_size == 0) {
                        return 0;
                    }

                    //
                    // This command has been handled.
                    //
                    break;
                }

error:
                //
                // Handle all other commands.
                //
                default:
                {
#ifdef HAVE_SNPRINTF_DEBUG
                    //
                    // Indicate an error.
                    //
                    ulPos = MIN(strlen("ERROR"), str_size);
                    memmove(str, "ERROR", ulPos);
                    str+= ulPos;
                    str_size -= ulPos;
                    if (str_size == 0) {
                        return 0;
                    }
#endif // HAVE_SNPRINTF_DEBUG

                    //
                    // This command has been handled.
                    //
                    break;
                }
            }
        }
    }

    //
    // End the varargs processing.
    //
    va_end(vaArgP);

    return 0;
}
#endif // HAVE_SPRINTF

