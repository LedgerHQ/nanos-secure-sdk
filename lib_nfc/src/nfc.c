#ifdef HAVE_NFC
#include "nfc.h"
#include "string.h"
#include "os_apdu.h"


/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * @brief Writes uri header string in uri_header matching uri_id
 *
 * @param uri_id URI string ID
 * @param uri_header URI header string
 * @return URI header string length
 */
uint16_t os_get_uri_header(uint8_t uri_id, char *uri_header) {
  switch (uri_id) {
    case URI_ID_0x01:
      strcpy(uri_header, URI_ID_0x01_STRING);
      break;
    case URI_ID_0x02:
      strcpy(uri_header, URI_ID_0x02_STRING);
      break;
    case URI_ID_0x03:
      strcpy(uri_header, URI_ID_0x03_STRING);
      break;
    case URI_ID_0x04:
      strcpy(uri_header, URI_ID_0x04_STRING);
      break;
    case URI_ID_0x05:
      strcpy(uri_header, URI_ID_0x05_STRING);
      break;
    case URI_ID_0x06:
      strcpy(uri_header, URI_ID_0x06_STRING);
      break;
    case URI_ID_0x07:
      strcpy(uri_header, URI_ID_0x07_STRING);
      break;
    case URI_ID_0x08:
      strcpy(uri_header, URI_ID_0x08_STRING);
      break;
    case URI_ID_0x09:
      strcpy(uri_header, URI_ID_0x09_STRING);
      break;
    case URI_ID_0x0A:
      strcpy(uri_header, URI_ID_0x0A_STRING);
      break;
    case URI_ID_0x0B:
      strcpy(uri_header, URI_ID_0x0B_STRING);
      break;
    case URI_ID_0x0C:
      strcpy(uri_header, URI_ID_0x0C_STRING);
      break;
    case URI_ID_0x0D:
      strcpy(uri_header, URI_ID_0x0D_STRING);
      break;
    case URI_ID_0x0E:
      strcpy(uri_header, URI_ID_0x0E_STRING);
      break;
    case URI_ID_0x0F:
      strcpy(uri_header, URI_ID_0x0F_STRING);
      break;
    case URI_ID_0x10:
      strcpy(uri_header, URI_ID_0x10_STRING);
      break;
    case URI_ID_0x11:
      strcpy(uri_header, URI_ID_0x11_STRING);
      break;
    case URI_ID_0x12:
      strcpy(uri_header, URI_ID_0x12_STRING);
      break;
    case URI_ID_0x13:
      strcpy(uri_header, URI_ID_0x13_STRING);
      break;
    case URI_ID_0x14:
      strcpy(uri_header, URI_ID_0x14_STRING);
      break;
    case URI_ID_0x15:
      strcpy(uri_header, URI_ID_0x15_STRING);
      break;
    case URI_ID_0x16:
      strcpy(uri_header, URI_ID_0x16_STRING);
      break;
    case URI_ID_0x17:
      strcpy(uri_header, URI_ID_0x17_STRING);
      break;
    case URI_ID_0x18:
      strcpy(uri_header, URI_ID_0x18_STRING);
      break;
    case URI_ID_0x19:
      strcpy(uri_header, URI_ID_0x19_STRING);
      break;
    case URI_ID_0x1A:
      strcpy(uri_header, URI_ID_0x1A_STRING);
      break;
    case URI_ID_0x1B:
      strcpy(uri_header, URI_ID_0x1B_STRING);
      break;
    case URI_ID_0x1C:
      strcpy(uri_header, URI_ID_0x1C_STRING);
      break;
    case URI_ID_0x1D:
      strcpy(uri_header, URI_ID_0x1D_STRING);
      break;
    case URI_ID_0x1E:
      strcpy(uri_header, URI_ID_0x1E_STRING);
      break;
    case URI_ID_0x1F:
      strcpy(uri_header, URI_ID_0x1F_STRING);
      break;
    case URI_ID_0x20:
      strcpy(uri_header, URI_ID_0x20_STRING);
      break;
    case URI_ID_0x21:
      strcpy(uri_header, URI_ID_0x21_STRING);
      break;
    case URI_ID_0x22:
      strcpy(uri_header, URI_ID_0x22_STRING);
      break;
    case URI_ID_0x23:
      strcpy(uri_header, URI_ID_0x23_STRING);
      break;
    default:
      return 0;
      break;
  }
  return strlen(uri_header);
}

/**
 * @brief deserializes an encoded NDEF message to an ndef_struct_t
 *
 * @param in_buffer input buffer to deseialize
 * @param parsed deserialized output
 * @return bolos error
 */
bolos_err_t os_parse_ndef(uint8_t *in_buffer, ndef_struct_t *parsed) {
    uint8_t text_length, info_length;
    parsed->ndef_type = in_buffer[APDU_OFF_P1];
    parsed->uri_id = in_buffer[APDU_OFF_P2];
    text_length = in_buffer[APDU_OFF_DATA];
    // -1 for '\0'
    if (text_length > NFC_TEXT_MAX_LEN-1) {
        return SWO_APD_LEN_2F;
    }
    memcpy(parsed->text, &in_buffer[APDU_OFF_DATA+1], text_length);
    info_length = in_buffer[APDU_OFF_DATA+1+text_length];
    // -1 for '\0'
    if (info_length > NFC_INFO_MAX_LEN-1) {
        return SWO_APD_LEN_30;
    }
    if (info_length) {
        memcpy(parsed->info, &in_buffer[APDU_OFF_DATA+1+text_length+1], info_length);
    }
    return SWO_OK;
}

/**
 * @brief converts an NDEF message to string
 *
 * @param ndef_message NDEF message to convert
 * @param out_string output string
 * @return output string length
 */
uint16_t os_ndef_to_string(ndef_struct_t *ndef_message, char * out_string) {
    uint16_t tot_length = 0;
    if (ndef_message->ndef_type == NFC_NDEF_TYPE_TEXT) {
        strcpy(out_string, ndef_message->text);
    }
    else if (ndef_message->ndef_type == NFC_NDEF_TYPE_URI) {
        tot_length += os_get_uri_header(ndef_message->uri_id, out_string);
        strcpy(&out_string[tot_length], ndef_message->text);
        tot_length += strlen(ndef_message->text);
        if (ndef_message->info[0] != '\0') {
            out_string[tot_length++] = '\n';
            strcpy(&out_string[tot_length], ndef_message->info);
            tot_length += strlen(ndef_message->info);
        }
    }
    return tot_length;
}
#endif
