#include <stdio.h>
#include <string.h>
#include "nbgl_obj.h"
#include "nbgl_serialize.h"
#include "nbgl_image_utils.h"
#include "os_pic.h"

// Utility functions

static void nbgl_appendU8(uint8_t value, uint8_t *out, size_t *w_cnt, size_t max_len)
{
    if (*w_cnt < max_len) {
        out[*w_cnt] = value;
        (*w_cnt)++;
    }
}

static void nbgl_appendU32(uint32_t value, uint8_t *out, size_t *w_cnt, size_t max_len)
{
    nbgl_appendU8((uint8_t) ((value >> 24) & 0xFF), out, w_cnt, max_len);
    nbgl_appendU8((uint8_t) ((value >> 16) & 0xFF), out, w_cnt, max_len);
    nbgl_appendU8((uint8_t) ((value >> 8) & 0xFF), out, w_cnt, max_len);
    nbgl_appendU8((uint8_t) (value & 0xFF), out, w_cnt, max_len);
}

static void nbgl_appendPtr(const void *value, uint8_t *out, size_t *w_cnt, size_t max_len)
{
    if (max_len < (*w_cnt + sizeof(void *))) {
        return;
    }
    memcpy(&out[*w_cnt], &value, sizeof(void *));
    (*w_cnt) = (*w_cnt) + sizeof(void *);
}

static void nbgl_appendU16(uint16_t value, uint8_t *out, size_t *w_cnt, size_t max_len)
{
    nbgl_appendU8((uint8_t) ((value >> 8) & 0xFF), out, w_cnt, max_len);
    nbgl_appendU8((uint8_t) (value & 0xFF), out, w_cnt, max_len);
}

// serialize a nbgl_area_t structure
static void nbgl_serializeArea(nbgl_area_t *area, uint8_t *out, size_t *w_cnt, size_t max_len)
{
    nbgl_appendU16(area->x0, out, w_cnt, max_len);
    nbgl_appendU16(area->y0, out, w_cnt, max_len);
    nbgl_appendU16(area->width, out, w_cnt, max_len);
    nbgl_appendU16(area->height, out, w_cnt, max_len);
    nbgl_appendU8((uint8_t) area->backgroundColor, out, w_cnt, max_len);
    nbgl_appendU8((uint8_t) area->bpp, out, w_cnt, max_len);
}

static uint8_t getObjId(nbgl_obj_t *obj)
{
    if (obj->type != SCREEN) {
        // return the internal offset + 1
        return nbgl_objPoolGetId(obj) + 1;
    }
    // no ID for screen, because it's not a real object, and not taken from pool
    return 0;
}

// serialize a nbgl_obj_t structure (including nbgl_area_t)
static void nbgl_serializeObj(nbgl_obj_t *obj, uint8_t *out, size_t *w_cnt, size_t max_len)
{
    // at first serialize an id
    nbgl_appendU8(getObjId(obj), out, w_cnt, max_len);
    // the only field to be serialized is the object type
    nbgl_appendU8((uint8_t) obj->type, out, w_cnt, max_len);
    // then serialize the area
    nbgl_serializeArea((nbgl_area_t *) &obj->area, out, w_cnt, max_len);
}

// Serialization functions

static void nbgl_serializeText(const char *text,
                               uint16_t    text_len,
                               uint8_t    *out,
                               size_t     *w_cnt,
                               size_t      max_len)
{
    if (text == NULL) {
        nbgl_appendU8('\0', out, w_cnt, max_len);
        return;
    }
    while (*w_cnt < max_len) {
        nbgl_appendU8(*text, out, w_cnt, max_len);
        // if the text_len is provided, use it
        if (text_len > 0) {
            text_len--;
            if (text_len == 0) {
                return;
            }
        }
        if (*text == '\0') {
            return;
        }
        else {
            text++;
        }
    }
}

/**
 * Mapping of nbgl_bpp_t type and their associated driver configuration
 * and actual number of bit per pixel.
 */
static const uint8_t NBGL_BPP_MAPPING[NB_NBGL_BPP]
    = {[NBGL_BPP_1] = 1, [NBGL_BPP_2] = 2, [NBGL_BPP_4] = 4};

/**
 * @brief Returns the number of bits per pixel associated to the input nbgl_bpp
 * @param nbgl_bpp Input nbgl_bpp
 *
 * @return
 * - Number of bits per pixel if nbgl_bpp is valid
 * - -1 otherwise
 */
static int8_t nbgl_bpp_get_number_of_bits_per_pixel(nbgl_bpp_t nbgl_bpp)
{
    if (nbgl_bpp >= NB_NBGL_BPP) {
        return -1;
    }

    return NBGL_BPP_MAPPING[nbgl_bpp];
}

/**
 * @brief Calculate the size in bytes of a window
 * @param width Width of the input window
 * @param height Height of the input window
 * @param nbgl_bpp Bpp of the input window
 *
 * @return
 *  - Window size if parameters are correct
 *  - (-1) otherwise
 */
static int32_t nbgl_bpp_get_window_size(uint16_t width, uint16_t height, nbgl_bpp_t nbgl_bpp)
{
    if ((width > SCREEN_WIDTH) || (height > SCREEN_HEIGHT) || (nbgl_bpp >= NB_NBGL_BPP)) {
        return -1;
    }

    int32_t nb_bits = (width * height * nbgl_bpp_get_number_of_bits_per_pixel(nbgl_bpp));
    uint8_t remain  = nb_bits % 8;

    // Ceil division to ensure the output number of bytes contains
    // all the bits of the window.
    if (remain == 0) {
        return nb_bits / 8;
    }
    else {
        return (nb_bits + (8 - remain)) / 8;
    }
}

static void nbgl_serializeIcon(const nbgl_icon_details_t *icon,
                               uint8_t                   *out,
                               size_t                    *w_cnt,
                               size_t                     max_len)
{
    int32_t size = 0;
    if (icon == NULL) {
        nbgl_appendU16(0, out, w_cnt, max_len);
        nbgl_appendU16(0, out, w_cnt, max_len);
        nbgl_appendU8(0, out, w_cnt, max_len);
        nbgl_appendU8(0, out, w_cnt, max_len);
    }
    else {
        nbgl_appendU16(icon->width, out, w_cnt, max_len);
        nbgl_appendU16(icon->height, out, w_cnt, max_len);
        nbgl_appendU8(icon->bpp, out, w_cnt, max_len);
        nbgl_appendU8(icon->isFile, out, w_cnt, max_len);
        if (!icon->isFile) {
            size = nbgl_bpp_get_window_size(icon->width, icon->height, icon->bpp);
        }
        else {
            size = GET_IMAGE_FILE_BUFFER_LEN(((const uint8_t *) PIC(icon->bitmap)))
                   + IMAGE_FILE_HEADER_SIZE;
        }
    }
    nbgl_appendU32(size, out, w_cnt, max_len);
#ifdef SERIALIZE_DATA
    for (int32_t i = 0; i < size; i++) {
        nbgl_appendU8(icon->bitmap[i], out, w_cnt, max_len);
    }
#endif  // SERIALIZE_DATA
}

static void nbgl_serializeTextArea(nbgl_text_area_t *obj,
                                   uint8_t          *out,
                                   size_t           *w_cnt,
                                   size_t            max_len)
{
    nbgl_serializeObj((nbgl_obj_t *) &obj->obj, out, w_cnt, max_len);

    nbgl_appendU8((uint8_t) obj->textColor, out, w_cnt, max_len);
    nbgl_appendU8((uint8_t) obj->textAlignment, out, w_cnt, max_len);
    nbgl_appendU8((uint8_t) obj->style, out, w_cnt, max_len);
    nbgl_appendU8((uint8_t) obj->fontId, out, w_cnt, max_len);
    nbgl_appendU8((uint8_t) obj->localized, out, w_cnt, max_len);
    nbgl_appendU8((uint8_t) obj->autoHideLongLine, out, w_cnt, max_len);
    nbgl_appendU16((uint16_t) obj->len, out, w_cnt, max_len);

    nbgl_serializeText(PIC(obj->text), obj->len, out, w_cnt, max_len);
}

static void nbgl_serializeLine(nbgl_line_t *obj, uint8_t *out, size_t *w_cnt, size_t max_len)
{
    nbgl_serializeObj((nbgl_obj_t *) &obj->obj, out, w_cnt, max_len);

    nbgl_appendU8((uint8_t) obj->direction, out, w_cnt, max_len);
    nbgl_appendU8((uint8_t) obj->lineColor, out, w_cnt, max_len);
    nbgl_appendU8((uint8_t) obj->thickness, out, w_cnt, max_len);
    nbgl_appendU8((uint8_t) obj->offset, out, w_cnt, max_len);
}

static void nbgl_serializeQrCode(nbgl_qrcode_t *obj, uint8_t *out, size_t *w_cnt, size_t max_len)
{
    nbgl_serializeObj((nbgl_obj_t *) &obj->obj, out, w_cnt, max_len);

    nbgl_appendU8((uint8_t) obj->foregroundColor, out, w_cnt, max_len);
    nbgl_appendU8((uint8_t) obj->version, out, w_cnt, max_len);
    nbgl_serializeText(PIC(obj->text), 0, out, w_cnt, max_len);
}

static void nbgl_serializeRadio(nbgl_radio_t *obj, uint8_t *out, size_t *w_cnt, size_t max_len)
{
    nbgl_serializeObj((nbgl_obj_t *) &obj->obj, out, w_cnt, max_len);

    nbgl_appendU8((uint8_t) obj->activeColor, out, w_cnt, max_len);
    nbgl_appendU8((uint8_t) obj->borderColor, out, w_cnt, max_len);
    nbgl_appendU8((uint8_t) obj->state, out, w_cnt, max_len);
}

static void nbgl_serializeSwitch(nbgl_switch_t *obj, uint8_t *out, size_t *w_cnt, size_t max_len)
{
    nbgl_serializeObj((nbgl_obj_t *) &obj->obj, out, w_cnt, max_len);

    nbgl_appendU8((uint8_t) obj->onColor, out, w_cnt, max_len);
    nbgl_appendU8((uint8_t) obj->offColor, out, w_cnt, max_len);
    nbgl_appendU8((uint8_t) obj->state, out, w_cnt, max_len);
}

static void nbgl_serializeProgressBar(nbgl_progress_bar_t *obj,
                                      uint8_t             *out,
                                      size_t              *w_cnt,
                                      size_t               max_len)
{
    nbgl_serializeObj((nbgl_obj_t *) &obj->obj, out, w_cnt, max_len);

    nbgl_appendU8((uint8_t) obj->withBorder, out, w_cnt, max_len);
    nbgl_appendU8((uint8_t) obj->state, out, w_cnt, max_len);
}

static void nbgl_serializePageIndicator(nbgl_page_indicator_t *obj,
                                        uint8_t               *out,
                                        size_t                *w_cnt,
                                        size_t                 max_len)
{
    nbgl_serializeObj((nbgl_obj_t *) &obj->obj, out, w_cnt, max_len);

    nbgl_appendU8((uint8_t) obj->activePage, out, w_cnt, max_len);
    nbgl_appendU8((uint8_t) obj->nbPages, out, w_cnt, max_len);
}

static void nbgl_serializeButton(nbgl_button_t *obj, uint8_t *out, size_t *w_cnt, size_t max_len)
{
    nbgl_serializeObj((nbgl_obj_t *) &obj->obj, out, w_cnt, max_len);

    nbgl_appendU8((uint8_t) obj->innerColor, out, w_cnt, max_len);
    nbgl_appendU8((uint8_t) obj->borderColor, out, w_cnt, max_len);
    nbgl_appendU8((uint8_t) obj->foregroundColor, out, w_cnt, max_len);
    nbgl_appendU8((uint8_t) obj->radius, out, w_cnt, max_len);
    nbgl_appendU8((uint8_t) obj->fontId, out, w_cnt, max_len);
    nbgl_appendU8((uint8_t) obj->localized, out, w_cnt, max_len);
    nbgl_serializeText(PIC(obj->text), 0, out, w_cnt, max_len);
    nbgl_serializeIcon(PIC(obj->icon), out, w_cnt, max_len);
}

static void nbgl_serializeImage(nbgl_image_t *obj, uint8_t *out, size_t *w_cnt, size_t max_len)
{
    nbgl_serializeObj((nbgl_obj_t *) &obj->obj, out, w_cnt, max_len);
    nbgl_serializeIcon(PIC(obj->buffer), out, w_cnt, max_len);
    nbgl_appendU8((uint8_t) obj->foregroundColor, out, w_cnt, max_len);
}

static void nbgl_serializeSpinner(nbgl_spinner_t *obj, uint8_t *out, size_t *w_cnt, size_t max_len)
{
    nbgl_serializeObj((nbgl_obj_t *) &obj->obj, out, w_cnt, max_len);
    nbgl_appendU8(obj->position, out, w_cnt, max_len);
}

static void nbgl_serializeKeyboard(nbgl_keyboard_t *obj,
                                   uint8_t         *out,
                                   size_t          *w_cnt,
                                   size_t           max_len)
{
    nbgl_serializeObj((nbgl_obj_t *) &obj->obj, out, w_cnt, max_len);

    nbgl_appendU8((uint8_t) obj->textColor, out, w_cnt, max_len);
    nbgl_appendU8((uint8_t) obj->borderColor, out, w_cnt, max_len);
    nbgl_appendU8((uint8_t) obj->lettersOnly, out, w_cnt, max_len);
#ifdef HAVE_SE_TOUCH
    nbgl_appendU8((uint8_t) obj->casing, out, w_cnt, max_len);
#else   // HAVE_SE_TOUCH
    nbgl_appendU8((uint8_t) obj->selectedCharIndex, out, w_cnt, max_len);
#endif  // HAVE_SE_TOUCH
    nbgl_appendU8((uint8_t) obj->mode, out, w_cnt, max_len);
    nbgl_appendU32(obj->keyMask, out, w_cnt, max_len);
}

static void nbgl_serializeKeypad(nbgl_keypad_t *obj, uint8_t *out, size_t *w_cnt, size_t max_len)
{
    nbgl_serializeObj((nbgl_obj_t *) &obj->obj, out, w_cnt, max_len);

#ifdef HAVE_SE_TOUCH
    nbgl_appendU8((uint8_t) obj->textColor, out, w_cnt, max_len);
    nbgl_appendU8((uint8_t) obj->borderColor, out, w_cnt, max_len);
#endif  // HAVE_SE_TOUCH
    nbgl_appendU8((uint8_t) obj->enableBackspace, out, w_cnt, max_len);
    nbgl_appendU8((uint8_t) obj->enableValidate, out, w_cnt, max_len);
#ifdef HAVE_SE_TOUCH
    nbgl_appendU8((uint8_t) obj->enableDigits, out, w_cnt, max_len);
#endif  // HAVE_SE_TOUCH
    nbgl_appendU8((uint8_t) obj->shuffled, out, w_cnt, max_len);
#ifdef HAVE_SE_TOUCH
    nbgl_appendU8((uint8_t) obj->digitIndexes[0], out, w_cnt, max_len);
    nbgl_appendU8((uint8_t) obj->digitIndexes[1], out, w_cnt, max_len);
    nbgl_appendU8((uint8_t) obj->digitIndexes[2], out, w_cnt, max_len);
    nbgl_appendU8((uint8_t) obj->digitIndexes[3], out, w_cnt, max_len);
    nbgl_appendU8((uint8_t) obj->digitIndexes[4], out, w_cnt, max_len);
#else   // HAVE_SE_TOUCH
    nbgl_appendU8((uint8_t) obj->selectedKey, out, w_cnt, max_len);
#endif  // HAVE_SE_TOUCH
}

static void nbgl_serializeImageFile(nbgl_image_file_t *obj,
                                    uint8_t           *out,
                                    size_t            *w_cnt,
                                    size_t             max_len)
{
    nbgl_serializeObj((nbgl_obj_t *) &obj->obj, out, w_cnt, max_len);
    nbgl_appendPtr((const void *) obj->buffer, out, w_cnt, max_len);
}

static void nbgl_serializeContainer(nbgl_container_t *obj,
                                    uint8_t          *out,
                                    size_t           *w_cnt,
                                    size_t            max_len)
{
    nbgl_serializeObj((nbgl_obj_t *) &obj->obj, out, w_cnt, max_len);

    nbgl_appendU8((uint8_t) obj->layout, out, w_cnt, max_len);
    nbgl_appendU8((uint8_t) obj->nbChildren, out, w_cnt, max_len);
    nbgl_appendU8((uint8_t) obj->forceClean, out, w_cnt, max_len);
}

static uint8_t nbgl_serializeObject(nbgl_obj_t *obj, uint8_t *out, size_t *w_cnt, size_t max_len)
{
    switch (obj->type) {
        case SCREEN:
            nbgl_serializeObj((nbgl_obj_t *) obj, out, w_cnt, max_len);
            break;
        case CONTAINER:
            nbgl_serializeContainer((nbgl_container_t *) obj, out, w_cnt, max_len);
            break;
        case IMAGE:
            nbgl_serializeImage((nbgl_image_t *) obj, out, w_cnt, max_len);
            break;
        case LINE:
            nbgl_serializeLine((nbgl_line_t *) obj, out, w_cnt, max_len);
            break;
        case TEXT_AREA:
            nbgl_serializeTextArea((nbgl_text_area_t *) obj, out, w_cnt, max_len);
            break;
        case BUTTON:
            nbgl_serializeButton((nbgl_button_t *) obj, out, w_cnt, max_len);
            break;
        case SWITCH:
            nbgl_serializeSwitch((nbgl_switch_t *) obj, out, w_cnt, max_len);
            break;
        case PAGE_INDICATOR:
            nbgl_serializePageIndicator((nbgl_page_indicator_t *) obj, out, w_cnt, max_len);
            break;
        case PROGRESS_BAR:
            nbgl_serializeProgressBar((nbgl_progress_bar_t *) obj, out, w_cnt, max_len);
            break;
        case RADIO_BUTTON:
            nbgl_serializeRadio((nbgl_radio_t *) obj, out, w_cnt, max_len);
            break;
        case QR_CODE:
            nbgl_serializeQrCode((nbgl_qrcode_t *) obj, out, w_cnt, max_len);
            break;
        case KEYBOARD:
            nbgl_serializeKeyboard((nbgl_keyboard_t *) obj, out, w_cnt, max_len);
            break;
        case KEYPAD:
            nbgl_serializeKeypad((nbgl_keypad_t *) obj, out, w_cnt, max_len);
            break;
        case SPINNER:
            nbgl_serializeSpinner((nbgl_spinner_t *) obj, out, w_cnt, max_len);
            break;
        case IMAGE_FILE:
            nbgl_serializeImageFile((nbgl_image_file_t *) obj, out, w_cnt, max_len);
            break;
        default:
            return NBGL_SERIALIZE_ERROR;
    }
    return NBGL_SERIALIZE_OK;
}

static uint8_t nbgl_serializeRefreshEvent(nbgl_area_t *obj,
                                          uint8_t     *out,
                                          size_t      *w_cnt,
                                          size_t       max_len)
{
    nbgl_serializeArea(obj, out, w_cnt, max_len);
    return NBGL_SERIALIZE_OK;
}

uint8_t nbgl_serializeNbglEvent(nbgl_serialized_event_type_e type,
                                nbgl_obj_t                  *obj,
                                uint8_t                     *out,
                                size_t                      *w_cnt,
                                size_t                       max_len)
{
    // Write type
    nbgl_appendU8((uint8_t) type, out, w_cnt, max_len);

    // Write content
    switch (type) {
        case NBGL_DRAW_OBJ:
            return nbgl_serializeObject(obj, out, w_cnt, max_len);
        case NBGL_REFRESH_AREA:
            return nbgl_serializeRefreshEvent((nbgl_area_t *) obj, out, w_cnt, max_len);
        default:
            return NBGL_SERIALIZE_ERROR;
    }

    return NBGL_SERIALIZE_OK;
}
