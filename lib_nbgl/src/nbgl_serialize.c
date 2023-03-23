#include "nbgl_obj.h"
#include "nbgl_serialize.h"

// Utility functions

static void nbgl_appendU8(uint8_t value, uint8_t *out, size_t *w_cnt, size_t max_len) {
	if(*w_cnt < max_len) {
		out[*w_cnt] = value;
		(*w_cnt)++;
	}
}

static void nbgl_appendU32(uint32_t value, uint8_t *out, size_t *w_cnt, size_t max_len) {
	nbgl_appendU8((uint8_t) ((value >> 24) & 0xFF), out, w_cnt, max_len);
	nbgl_appendU8((uint8_t) ((value >> 16) & 0xFF), out, w_cnt, max_len);
	nbgl_appendU8((uint8_t) ((value >> 8) & 0xFF), out, w_cnt, max_len);
	nbgl_appendU8((uint8_t) (value & 0xFF), out, w_cnt, max_len);
}

static void nbgl_appendU16(uint16_t value, uint8_t *out, size_t *w_cnt, size_t max_len) {
	nbgl_appendU8((uint8_t) ((value >> 8) & 0xFF), out, w_cnt, max_len);
	nbgl_appendU8((uint8_t) (value & 0xFF), out, w_cnt, max_len);
}

static void nbgl_serializeType(nbgl_obj_t *obj, uint8_t *out, size_t *w_cnt, size_t max_len) {
	nbgl_appendU8((uint8_t) obj->type, out, w_cnt, max_len);
}

static void nbgl_serializeArea(nbgl_area_t *area, uint8_t *out, size_t *w_cnt, size_t max_len) {
	nbgl_appendU16(area->x0, out, w_cnt, max_len);
	nbgl_appendU16(area->y0, out, w_cnt, max_len);
	nbgl_appendU16(area->width, out, w_cnt, max_len);
	nbgl_appendU16(area->height, out, w_cnt, max_len);
	nbgl_appendU8((uint8_t) area->backgroundColor, out, w_cnt, max_len);
	nbgl_appendU8((uint8_t) area->bpp, out, w_cnt, max_len);
}

// Serialization functions

static void nbgl_serializeText(const char* text, uint8_t *out, size_t *w_cnt, size_t max_len) {
	if (text == NULL) {
		nbgl_appendU8('\0', out, w_cnt, max_len);
		return;
	} else {
		while(*w_cnt < max_len) {
			nbgl_appendU8(*text, out, w_cnt, max_len);
			if(*text == '\0') {
				return;
			} else {
				text++;
			}
		}
	}
}

static void nbgl_serializeTextArea(nbgl_text_area_t *obj, uint8_t *out, size_t *w_cnt, size_t max_len) {
	nbgl_serializeType((nbgl_obj_t *) obj, out, w_cnt, max_len);
	nbgl_serializeArea((nbgl_area_t *) obj, out, w_cnt, max_len);

	nbgl_appendU8((uint8_t) obj->textColor, out, w_cnt, max_len);
	nbgl_appendU8((uint8_t) obj->textAlignment, out, w_cnt, max_len);
	nbgl_appendU8((uint8_t) obj->style, out, w_cnt, max_len);
	nbgl_appendU8((uint8_t) obj->fontId, out, w_cnt, max_len);
	nbgl_appendU8((uint8_t) obj->localized, out, w_cnt, max_len);
	nbgl_appendU8((uint8_t) obj->autoHideLongLine, out, w_cnt, max_len);

	nbgl_serializeText(obj->text, out, w_cnt, max_len);
}

static void nbgl_serializeLine(nbgl_line_t *obj, uint8_t *out, size_t *w_cnt, size_t max_len) {
	nbgl_serializeType((nbgl_obj_t *) obj, out, w_cnt, max_len);
	nbgl_serializeArea((nbgl_area_t *) obj, out, w_cnt, max_len);

	nbgl_appendU8((uint8_t) obj->direction, out, w_cnt, max_len);
	nbgl_appendU8((uint8_t) obj->lineColor, out, w_cnt, max_len);
	nbgl_appendU8((uint8_t) obj->thickness, out, w_cnt, max_len);
	nbgl_appendU8((uint8_t) obj->offset, out, w_cnt, max_len);
}

static void nbgl_serializeQrCode(nbgl_qrcode_t *obj, uint8_t *out, size_t *w_cnt, size_t max_len) {
	nbgl_serializeType((nbgl_obj_t *) obj, out, w_cnt, max_len);
	nbgl_serializeArea((nbgl_area_t *) obj, out, w_cnt, max_len);

	nbgl_appendU8((uint8_t) obj->foregroundColor, out, w_cnt, max_len);
	nbgl_appendU8((uint8_t) obj->version, out, w_cnt, max_len);
	nbgl_serializeText(obj->text, out, w_cnt, max_len);
}

static void nbgl_serializeRadio(nbgl_radio_t *obj, uint8_t *out, size_t *w_cnt, size_t max_len) {
	nbgl_serializeType((nbgl_obj_t *) obj, out, w_cnt, max_len);
	nbgl_serializeArea((nbgl_area_t *) obj, out, w_cnt, max_len);

	nbgl_appendU8((uint8_t) obj->activeColor, out, w_cnt, max_len);
	nbgl_appendU8((uint8_t) obj->borderColor, out, w_cnt, max_len);
	nbgl_appendU8((uint8_t) obj->state, out, w_cnt, max_len);
}

static void nbgl_serializeSwitch(nbgl_switch_t *obj, uint8_t *out, size_t *w_cnt, size_t max_len) {
	nbgl_serializeType((nbgl_obj_t *) obj, out, w_cnt, max_len);
	nbgl_serializeArea((nbgl_area_t *) obj, out, w_cnt, max_len);

	nbgl_appendU8((uint8_t) obj->onColor, out, w_cnt, max_len);
	nbgl_appendU8((uint8_t) obj->offColor, out, w_cnt, max_len);
	nbgl_appendU8((uint8_t) obj->state, out, w_cnt, max_len);
}


static void nbgl_serializeProgressBar(nbgl_progress_bar_t *obj,
				  uint8_t *out,
				  size_t *w_cnt,
				  size_t max_len) {
	nbgl_serializeType((nbgl_obj_t *) obj, out, w_cnt, max_len);
	nbgl_serializeArea((nbgl_area_t *) obj, out, w_cnt, max_len);

	nbgl_appendU8((uint8_t) obj->withBorder, out, w_cnt, max_len);
	nbgl_appendU8((uint8_t) obj->state, out, w_cnt, max_len);
}


static void nbgl_serializeNavigationBar(nbgl_page_indicator_t *obj,
				    uint8_t *out,
				    size_t *w_cnt,
				    size_t max_len) {
	nbgl_serializeType((nbgl_obj_t *) obj, out, w_cnt, max_len);
	nbgl_serializeArea((nbgl_area_t *) obj, out, w_cnt, max_len);

	nbgl_appendU8((uint8_t) obj->activePage, out, w_cnt, max_len);
	nbgl_appendU8((uint8_t) obj->nbPages, out, w_cnt, max_len);
}

static void nbgl_serializeButton(nbgl_button_t *obj, uint8_t *out, size_t *w_cnt, size_t max_len) {
	nbgl_serializeType((nbgl_obj_t *) obj, out, w_cnt, max_len);
	nbgl_serializeArea((nbgl_area_t *) obj, out, w_cnt, max_len);

	nbgl_appendU8((uint8_t) obj->innerColor, out, w_cnt, max_len);
	nbgl_appendU8((uint8_t) obj->borderColor, out, w_cnt, max_len);
	nbgl_appendU8((uint8_t) obj->foregroundColor, out, w_cnt, max_len);
	nbgl_appendU8((uint8_t) obj->radius, out, w_cnt, max_len);
	nbgl_appendU8((uint8_t) obj->fontId, out, w_cnt, max_len);
	nbgl_appendU8((uint8_t) obj->localized, out, w_cnt, max_len);
	nbgl_serializeText(obj->text, out, w_cnt, max_len);
}

static void nbgl_serializeImage(nbgl_image_t *obj, uint8_t *out, size_t *w_cnt, size_t max_len) {
	nbgl_serializeType((nbgl_obj_t *) obj, out, w_cnt, max_len);
	nbgl_serializeArea((nbgl_area_t *) obj, out, w_cnt, max_len);

	nbgl_appendU8((uint8_t) obj->foregroundColor, out, w_cnt, max_len);
}

static void nbgl_serializeSpinner(nbgl_spinner_t *obj, uint8_t *out, size_t *w_cnt, size_t max_len) {
	nbgl_serializeType((nbgl_obj_t *) obj, out, w_cnt, max_len);
	nbgl_serializeArea((nbgl_area_t *) obj, out, w_cnt, max_len);

	nbgl_appendU8(obj->position, out, w_cnt, max_len);
}

static void nbgl_serializeKeyboard(nbgl_keyboard_t *obj, uint8_t *out, size_t *w_cnt, size_t max_len) {
	nbgl_serializeType((nbgl_obj_t *) obj, out, w_cnt, max_len);
	nbgl_serializeArea((nbgl_area_t *) obj, out, w_cnt, max_len);

	nbgl_appendU8((uint8_t) obj->textColor, out, w_cnt, max_len);
	nbgl_appendU8((uint8_t) obj->borderColor, out, w_cnt, max_len);
	nbgl_appendU8((uint8_t) obj->lettersOnly, out, w_cnt, max_len);
	nbgl_appendU8((uint8_t) obj->casing, out, w_cnt, max_len);
	nbgl_appendU8((uint8_t) obj->mode, out, w_cnt, max_len);
	nbgl_appendU32(obj->keyMask, out, w_cnt, max_len);
}

static void nbgl_serializeKeypad(nbgl_keypad_t *obj, uint8_t *out, size_t *w_cnt, size_t max_len) {
	nbgl_serializeType((nbgl_obj_t *) obj, out, w_cnt, max_len);
	nbgl_serializeArea((nbgl_area_t *) obj, out, w_cnt, max_len);

	nbgl_appendU8((uint8_t) obj->textColor, out, w_cnt, max_len);
	nbgl_appendU8((uint8_t) obj->borderColor, out, w_cnt, max_len);
	nbgl_appendU8((uint8_t) obj->enableBackspace, out, w_cnt, max_len);
	nbgl_appendU8((uint8_t) obj->enableValidate, out, w_cnt, max_len);
}

static void nbgl_serializeImageFile(nbgl_image_file_t *obj, uint8_t *out, size_t *w_cnt, size_t max_len) {
	nbgl_serializeType((nbgl_obj_t *) obj, out, w_cnt, max_len);
	nbgl_serializeArea((nbgl_area_t *) obj, out, w_cnt, max_len);
}

static void nbgl_serializeContainer(nbgl_container_t *obj, uint8_t *out, size_t *w_cnt, size_t max_len) {
	nbgl_serializeType((nbgl_obj_t *) obj, out, w_cnt, max_len);
	nbgl_serializeArea((nbgl_area_t *) obj, out, w_cnt, max_len);

	nbgl_appendU8((uint8_t) obj->layout, out, w_cnt, max_len);
	nbgl_appendU8((uint8_t) obj->nbChildren, out, w_cnt, max_len);
	nbgl_appendU8((uint8_t) obj->forceClean, out, w_cnt, max_len);
}

static uint8_t nbgl_serializeObject(nbgl_obj_t *obj, uint8_t *out, size_t *w_cnt, size_t max_len) {
	switch(obj->type) {
		case SCREEN:
			return NBGL_SERIALIZE_ERROR;
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
			nbgl_serializeNavigationBar((nbgl_page_indicator_t *) obj, out, w_cnt, max_len);
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


static uint8_t nbgl_serializeRefreshEvent(nbgl_area_t *obj, uint8_t *out, size_t *w_cnt, size_t max_len) {
	nbgl_serializeArea(obj, out, w_cnt, max_len);
	return NBGL_SERIALIZE_OK;
}

uint8_t nbgl_serializeNbglEvent(nbgl_serialized_event_type_e type,
				nbgl_obj_t *obj,
				uint8_t *out,
				size_t *w_cnt,
				size_t max_len) {
	// Write type
	nbgl_appendU8((uint8_t) type, out, w_cnt, max_len);

	// Write content
	switch(type) {
		case NBGL_DRAW_OBJ:
			return nbgl_serializeObject(obj, out, w_cnt, max_len);
		case NBGL_REFRESH_AREA:
			return nbgl_serializeRefreshEvent((nbgl_area_t *) obj, out, w_cnt, max_len);
		default:
			return NBGL_SERIALIZE_ERROR;
	}

	return NBGL_SERIALIZE_OK;
}
