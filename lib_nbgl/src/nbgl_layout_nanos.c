/**
 * @file nbgl_layout.c
 * @brief Implementation of predefined layouts management for Applications
 */

#ifndef HAVE_SE_TOUCH
/*********************
 *      INCLUDES
 *********************/
#include <string.h>
#include <stdlib.h>
#include "nbgl_debug.h"
#include "nbgl_front.h"
#include "nbgl_layout.h"
#include "nbgl_obj.h"
#include "nbgl_draw.h"
#include "nbgl_screen.h"
#include "nbgl_touch.h"
#include "glyphs.h"
#include "os_pic.h"
#include "os_helpers.h"
#include "lcx_rng.h"

/*********************
 *      DEFINES
 *********************/

#define NB_MAX_LAYOUTS 3

// used by screen
#define NB_MAX_SCREEN_CHILDREN 7

/**********************
 *      MACROS
 **********************/
#define ASSERT_ENOUGH_PLACE_FOR_MAIN_PANEL_CHILD(__layout)                    \
    {                                                                         \
        if ((__layout)->panel.nbChildren == (NB_MAX_MAIN_PANEL_CHILDREN - 1)) \
            return NO_MORE_OBJ_ERROR;                                         \
    }

/**********************
 *      TYPEDEFS
 **********************/

/**
 * @brief Structure containing all information about the current layout.
 * @note It shall not be used externally
 *
 */
typedef struct nbgl_layoutInternal_s {
    bool    modal;  ///< if true, means the screen is a modal
    uint8_t layer;  ///< if >0, puts the layout on top of screen stack (modal). Otherwise puts on
                    ///< background (for apps)
    uint8_t      nbChildren;  ///< number of children in above array
    nbgl_obj_t **children;    ///< children for main screen

    nbgl_layoutButtonCallback_t callback;  // user callback for all controls
} nbgl_layoutInternal_t;

/**********************
 *      VARIABLES
 **********************/

/**
 * @brief array of layouts, if used by modal
 *
 */
static nbgl_layoutInternal_t gLayout[NB_MAX_LAYOUTS] = {0};

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void buttonCallback(nbgl_screen_t *screen, nbgl_buttonEvent_t buttonEvent)
{
    uint8_t                i      = NB_MAX_LAYOUTS;
    nbgl_layoutInternal_t *layout = NULL;

    // parse all layouts (starting with modals) to find the object
    while (i > 0) {
        i--;
        if ((screen->index == gLayout[i].layer) && (gLayout[i].nbChildren > 0)) {
            // found
            layout = &gLayout[i];
            break;
        }
    }
    if (layout == NULL) {
        LOG_WARN(
            LAYOUT_LOGGER,
            "touchCallback(): screen->index = %d, buttonEvent = %d, no matching active layout\n",
            screen->index,
            buttonEvent);
        return;
    }

#ifdef NBGL_KEYPAD
    // special case of keypad
    nbgl_obj_t *kpd = nbgl_screenContainsObjType(screen, KEYPAD);
    if (kpd) {
        nbgl_keypadCallback(kpd, buttonEvent);
        return;
    }
    else
#endif  // NBGL_KEYPAD
#ifdef NBGL_KEYBOARD
    {
        nbgl_obj_t *kbd = nbgl_screenContainsObjType(screen, KEYBOARD);
        if (kbd) {
            nbgl_keyboardCallback(kbd, buttonEvent);
            return;
        }
    }
#endif  // NBGL_KEYBOARD
    if (layout->callback != NULL) {
        layout->callback((nbgl_layout_t *) layout, buttonEvent);
    }
}

/**
 * @brief adds the given obj to the layout
 *
 * @param layout
 * @param obj
 */
static void addObjectToLayout(nbgl_layoutInternal_t *layout, nbgl_obj_t *obj)
{
    if (layout->nbChildren == NB_MAX_SCREEN_CHILDREN) {
        LOG_FATAL(LAYOUT_LOGGER, "addObjectToLayout(): No more object\n");
    }
    layout->children[layout->nbChildren] = obj;
    layout->nbChildren++;
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * @brief returns a layout of the given type. The layout is reset
 *
 * @param description description of layout
 * @return a pointer to the corresponding layout
 */
nbgl_layout_t *nbgl_layoutGet(const nbgl_layoutDescription_t *description)
{
    nbgl_layoutInternal_t *layout = NULL;

    // find an empty layout in the proper "layer"
    if (description->modal) {
        if (gLayout[1].nbChildren == 0) {
            layout = &gLayout[1];
        }
        else if (gLayout[2].nbChildren == 0) {
            layout = &gLayout[2];
        }
    }
    else {
        // automatically "release" a potentially opened non-modal layout
        gLayout[0].nbChildren = 0;
        layout                = &gLayout[0];
    }
    if (layout == NULL) {
        LOG_WARN(LAYOUT_LOGGER, "nbgl_layoutGet(): impossible to get a layout!\n");
        return NULL;
    }

    // reset globals
    memset(layout, 0, sizeof(nbgl_layoutInternal_t));

    layout->callback = (nbgl_layoutButtonCallback_t) PIC(description->onActionCallback);
    layout->modal    = description->modal;
    if (description->modal) {
        layout->layer = nbgl_screenPush(&layout->children,
                                        NB_MAX_SCREEN_CHILDREN,
                                        &description->ticker,
                                        (nbgl_buttonCallback_t) buttonCallback);
    }
    else {
        nbgl_screenSet(&layout->children,
                       NB_MAX_SCREEN_CHILDREN,
                       &description->ticker,
                       (nbgl_buttonCallback_t) buttonCallback);
        layout->layer = 0;
    }

    return (nbgl_layout_t *) layout;
}

/**
 * @brief Creates navigation arrows on side(s) of the screen
 *
 * @param layout the current layout
 * @param info structure giving the description of the navigation
 * @return >= 0 if OK
 */
int nbgl_layoutAddNavigation(nbgl_layout_t *layout, nbgl_layoutNavigation_t *info)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutAddNavigation():\n");
    if (layout == NULL) {
        return -1;
    }

    nbgl_image_t *image;
    if (info->indication & LEFT_ARROW) {
        image                  = (nbgl_image_t *) nbgl_objPoolGet(IMAGE, layoutInt->layer);
        image->foregroundColor = WHITE;
        image->buffer          = (info->direction == HORIZONTAL_NAV) ? &C_icon_left : &C_icon_up;
        image->obj.area.bpp    = NBGL_BPP_1;
        image->obj.alignment   = MID_LEFT;
        addObjectToLayout(layoutInt, (nbgl_obj_t *) image);
    }
    if (info->indication & RIGHT_ARROW) {
        image                  = (nbgl_image_t *) nbgl_objPoolGet(IMAGE, layoutInt->layer);
        image->foregroundColor = WHITE;
        image->buffer          = (info->direction == HORIZONTAL_NAV) ? &C_icon_right : &C_icon_down;
        image->obj.area.bpp    = NBGL_BPP_1;
        image->obj.alignment   = MID_RIGHT;
        addObjectToLayout(layoutInt, (nbgl_obj_t *) image);
    }
    return 0;
}

/**
 * @brief Creates an area with given text and sub text, using the given style
 *
 * @param layout the current layout
 * @param text main text for the switch
 * @param subText description under main text (NULL terminated, single line, may be null)
 * @param style if @ref REGULAR_INFO, use regular font for text, otherwise use bold font for text
 * @return >= 0 if OK
 */
int nbgl_layoutAddText(nbgl_layout_t                  *layout,
                       const char                     *text,
                       const char                     *subText,
                       nbgl_contentCenteredInfoStyle_t style)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    nbgl_container_t      *container;
    nbgl_text_area_t      *textArea;
    uint16_t               fullHeight = 0;

    UNUSED(subText);

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutAddText():\n");
    if (layout == NULL) {
        return -1;
    }
    container = (nbgl_container_t *) nbgl_objPoolGet(CONTAINER, layoutInt->layer);

    // get container children
    container->nbChildren = 1;
    if (subText != NULL) {
        container->nbChildren++;
    }

    container->children       = nbgl_containerPoolGet(container->nbChildren, layoutInt->layer);
    container->obj.area.width = AVAILABLE_WIDTH;

    textArea                 = (nbgl_text_area_t *) nbgl_objPoolGet(TEXT_AREA, layoutInt->layer);
    textArea->textColor      = WHITE;
    textArea->text           = PIC(text);
    textArea->textAlignment  = CENTER;
    textArea->fontId         = (style == REGULAR_INFO) ? BAGL_FONT_OPEN_SANS_REGULAR_11px_1bpp
                                                       : BAGL_FONT_OPEN_SANS_EXTRABOLD_11px_1bpp;
    textArea->obj.area.width = AVAILABLE_WIDTH;

    uint16_t nbLines
        = nbgl_getTextNbLinesInWidth(textArea->fontId, textArea->text, AVAILABLE_WIDTH, true);
    // if more than available lines on screen
    if (nbLines > NB_MAX_LINES) {
        uint16_t len;

        nbLines              = NB_MAX_LINES;
        textArea->nbMaxLines = NB_MAX_LINES;
        nbgl_getTextMaxLenInNbLines(
            textArea->fontId, textArea->text, AVAILABLE_WIDTH, nbLines, &len, true);
        textArea->len = len;
    }
    const nbgl_font_t *font   = nbgl_getFont(textArea->fontId);
    textArea->obj.area.height = nbLines * font->line_height;
    textArea->wrapping        = true;
    textArea->obj.alignment   = TOP_MIDDLE;
    fullHeight += textArea->obj.area.height;
    container->children[0] = (nbgl_obj_t *) textArea;

    if (subText != NULL) {
        textArea            = (nbgl_text_area_t *) nbgl_objPoolGet(TEXT_AREA, layoutInt->layer);
        textArea->textColor = WHITE;
        textArea->text      = PIC(subText);
        textArea->wrapping  = true;
        textArea->fontId    = BAGL_FONT_OPEN_SANS_REGULAR_11px_1bpp;
        textArea->obj.area.width = AVAILABLE_WIDTH;
        nbLines
            = nbgl_getTextNbLinesInWidth(textArea->fontId, textArea->text, AVAILABLE_WIDTH, true);
        // if more than available lines on screen
        if (nbLines > (NB_MAX_LINES - 1)) {
            uint16_t len;
            nbLines              = NB_MAX_LINES - 1;
            textArea->nbMaxLines = nbLines;
            nbgl_getTextMaxLenInNbLines(
                textArea->fontId, textArea->text, AVAILABLE_WIDTH, nbLines, &len, true);
            textArea->len = len;
        }
        textArea->obj.area.height      = nbLines * font->line_height;
        textArea->textAlignment        = CENTER;
        textArea->obj.alignment        = NO_ALIGNMENT;
        textArea->obj.alignmentMarginY = 2;
        fullHeight += textArea->obj.area.height + textArea->obj.alignmentMarginY;
        container->children[1] = (nbgl_obj_t *) textArea;
    }
    container->obj.area.height = fullHeight;
    container->layout          = VERTICAL;
    container->obj.alignment   = CENTER;
    // set this new obj as child of main container
    addObjectToLayout(layoutInt, (nbgl_obj_t *) container);

    return 0;
}

/**
 * @brief Creates a menu list (only for nanos) with the given parameters. The navigation (and
 * selection) must be handled by the caller
 *
 * @param layout the current layout
 * @param list structure giving the list of choices and the current selected one
 * @return >= 0 if OK
 */
int nbgl_layoutAddMenuList(nbgl_layout_t *layout, nbgl_layoutMenuList_t *list)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    uint8_t                i;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutAddMenuList():\n");
    if (layout == NULL) {
        return -1;
    }
    for (i = 0; i < list->nbChoices; i++) {
        nbgl_text_area_t *textArea;

        // check whether this object is visible or not
        // only the two objects above or below the selected one are visible
        if (((list->selectedChoice > 2) && (i < (list->selectedChoice - 2)))
            || (i > (list->selectedChoice + 2))) {
            continue;
        }

        textArea = (nbgl_text_area_t *) nbgl_objPoolGet(TEXT_AREA, layoutInt->layer);

        // init text area for this choice
        textArea->text                 = list->callback(i);
        textArea->textAlignment        = CENTER;
        textArea->obj.area.width       = AVAILABLE_WIDTH;
        textArea->obj.area.height      = 12;
        textArea->style                = NO_STYLE;
        textArea->obj.alignment        = CENTER;
        textArea->obj.alignmentMarginY = ((i - list->selectedChoice) * 16);
        textArea->textColor            = WHITE;

        // highlight init choice
        if (i == list->selectedChoice) {
            textArea->fontId = BAGL_FONT_OPEN_SANS_EXTRABOLD_11px_1bpp;
        }
        else {
            textArea->fontId = BAGL_FONT_OPEN_SANS_REGULAR_11px_1bpp;
        }

        // set this new obj as child of main container
        addObjectToLayout(layoutInt, (nbgl_obj_t *) textArea);
    }

    return 0;
}

/**
 * @brief Creates an area on the center of the main panel, with a possible icon/image,
 * a possible text in black under it, and a possible text in gray under it
 *
 * @param layout the current layout
 * @param info structure giving the description of buttons (texts, icons, layout)
 * @return >= 0 if OK
 */
int nbgl_layoutAddCenteredInfo(nbgl_layout_t *layout, const nbgl_layoutCenteredInfo_t *info)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    nbgl_container_t      *container;
    nbgl_text_area_t      *textArea   = NULL;
    nbgl_image_t          *image      = NULL;
    uint16_t               fullHeight = 0;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutAddCenteredInfo():\n");
    if (layout == NULL) {
        return -1;
    }

    container = (nbgl_container_t *) nbgl_objPoolGet(CONTAINER, layoutInt->layer);

    // 3 children at max
    container->children   = nbgl_containerPoolGet(3, layoutInt->layer);
    container->nbChildren = 0;
    if (info->icon != NULL) {
        image                  = (nbgl_image_t *) nbgl_objPoolGet(IMAGE, layoutInt->layer);
        image->foregroundColor = WHITE;
        image->buffer          = PIC(info->icon);
        image->obj.area.bpp    = NBGL_BPP_1;
        image->obj.alignment   = TOP_MIDDLE;
        image->obj.alignTo     = NULL;

        fullHeight += image->buffer->height;
        container->children[container->nbChildren] = (nbgl_obj_t *) image;
        container->nbChildren++;
    }
    if (info->text1 != NULL) {
        textArea                = (nbgl_text_area_t *) nbgl_objPoolGet(TEXT_AREA, layoutInt->layer);
        textArea->textColor     = WHITE;
        textArea->text          = PIC(info->text1);
        textArea->textAlignment = CENTER;
        textArea->fontId = (info->style == REGULAR_INFO) ? BAGL_FONT_OPEN_SANS_REGULAR_11px_1bpp
                                                         : BAGL_FONT_OPEN_SANS_EXTRABOLD_11px_1bpp;
        textArea->obj.area.width = AVAILABLE_WIDTH;
        textArea->wrapping       = true;
        uint16_t nbLines
            = nbgl_getTextNbLinesInWidth(textArea->fontId, textArea->text, AVAILABLE_WIDTH, true);
        // if more than available lines on screen
        if (nbLines > NB_MAX_LINES) {
            uint16_t len;
            nbLines              = NB_MAX_LINES;
            textArea->nbMaxLines = NB_MAX_LINES;
            nbgl_getTextMaxLenInNbLines(
                textArea->fontId, textArea->text, AVAILABLE_WIDTH, nbLines, &len, true);
            textArea->len = len;
        }
        const nbgl_font_t *font   = nbgl_getFont(textArea->fontId);
        textArea->obj.area.height = nbLines * font->line_height;
        textArea->style           = NO_STYLE;
        if (info->icon != NULL) {
            textArea->obj.alignment = BOTTOM_MIDDLE;  // under icon
            textArea->obj.alignTo   = (nbgl_obj_t *) container->children[container->nbChildren - 1];
            textArea->obj.alignmentMarginY = (nbLines < 3) ? 4 : 0;
        }
        else if (info->text2 == NULL) {
            textArea->obj.alignment = CENTER;
            textArea->obj.alignTo   = NULL;
        }
        else {
            textArea->obj.alignment = TOP_MIDDLE;
            textArea->obj.alignTo   = NULL;
        }

        fullHeight += textArea->obj.area.height + textArea->obj.alignmentMarginY;

        container->children[container->nbChildren] = (nbgl_obj_t *) textArea;
        container->nbChildren++;
    }
    if (info->text2 != NULL) {
        textArea                = (nbgl_text_area_t *) nbgl_objPoolGet(TEXT_AREA, layoutInt->layer);
        textArea->textColor     = WHITE;
        textArea->text          = PIC(info->text2);
        textArea->textAlignment = CENTER;
        textArea->fontId        = BAGL_FONT_OPEN_SANS_REGULAR_11px_1bpp;
        textArea->obj.area.width = AVAILABLE_WIDTH;
        uint16_t nbLines
            = nbgl_getTextNbLinesInWidth(textArea->fontId, textArea->text, AVAILABLE_WIDTH, true);
        // if more than available lines on screen
        if (nbLines > (NB_MAX_LINES - 1)) {
            uint16_t len;
            nbLines              = NB_MAX_LINES - 1;
            textArea->nbMaxLines = nbLines;
            nbgl_getTextMaxLenInNbLines(
                textArea->fontId, textArea->text, AVAILABLE_WIDTH, nbLines, &len, true);
            textArea->len = len;
        }
        const nbgl_font_t *font   = nbgl_getFont(textArea->fontId);
        textArea->obj.area.height = nbLines * font->line_height;

        textArea->style         = NO_STYLE;
        textArea->obj.alignment = BOTTOM_MIDDLE;
        textArea->obj.alignTo   = (nbgl_obj_t *) container->children[container->nbChildren - 1];
        textArea->obj.alignmentMarginY = 2;

        fullHeight += textArea->obj.area.height + textArea->obj.alignmentMarginY;

        container->children[container->nbChildren] = (nbgl_obj_t *) textArea;
        container->nbChildren++;
    }
    container->obj.area.height      = fullHeight;
    container->layout               = VERTICAL;
    container->obj.alignmentMarginY = 0;
    if (info->onTop) {
        container->obj.alignment = TOP_MIDDLE;
    }
    else {
        container->obj.alignment = CENTER;
    }

    container->obj.area.width = AVAILABLE_WIDTH;

    // set this new container as child of main container
    addObjectToLayout(layoutInt, (nbgl_obj_t *) container);

    return 0;
}

/**
 * @brief Creates an area in main panel to display a progress bar, with a title text and a
 * description under the progress
 *
 * @param layout the current layout
 * @param barLayout structure giving the description of progress bar
 * @return >= 0 if OK
 */
int nbgl_layoutAddProgressBar(nbgl_layout_t *layout, const nbgl_layoutProgressBar_t *barLayout)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    nbgl_progress_bar_t   *progress;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutAddProgressBar():\n");
    if (layout == NULL) {
        return -1;
    }
    if (barLayout->text != NULL) {
        nbgl_text_area_t *textArea;

        textArea                       = (nbgl_text_area_t *) nbgl_objPoolGet(TEXT_AREA,
                                                        ((nbgl_layoutInternal_t *) layout)->layer);
        textArea->textColor            = WHITE;
        textArea->text                 = PIC(barLayout->text);
        textArea->textAlignment        = CENTER;
        textArea->fontId               = BAGL_FONT_OPEN_SANS_REGULAR_11px_1bpp;
        textArea->obj.area.width       = AVAILABLE_WIDTH;
        textArea->obj.area.height      = nbgl_getTextHeight(textArea->fontId, textArea->text);
        textArea->obj.alignment        = TOP_MIDDLE;
        textArea->obj.alignmentMarginX = 0;
        textArea->obj.alignmentMarginY = 16;  // 16 px from top
        addObjectToLayout(layoutInt, (nbgl_obj_t *) textArea);
    }
    progress                       = (nbgl_progress_bar_t *) nbgl_objPoolGet(PROGRESS_BAR,
                                                       ((nbgl_layoutInternal_t *) layout)->layer);
    progress->foregroundColor      = WHITE;
    progress->withBorder           = true;
    progress->state                = barLayout->percentage;
    progress->obj.area.width       = 102;
    progress->obj.area.height      = 14;
    progress->obj.alignment        = TOP_MIDDLE;
    progress->obj.alignmentMarginX = 0;
    progress->obj.alignmentMarginY = 33;  // 33px from top
    addObjectToLayout(layoutInt, (nbgl_obj_t *) progress);

    if (barLayout->subText != NULL) {
        nbgl_text_area_t *subTextArea;

        subTextArea = (nbgl_text_area_t *) nbgl_objPoolGet(
            TEXT_AREA, ((nbgl_layoutInternal_t *) layout)->layer);
        subTextArea->textColor       = WHITE;
        subTextArea->text            = PIC(barLayout->subText);
        subTextArea->textAlignment   = CENTER;
        subTextArea->fontId          = BAGL_FONT_OPEN_SANS_REGULAR_11px_1bpp;
        subTextArea->obj.area.width  = AVAILABLE_WIDTH;
        subTextArea->obj.area.height = nbgl_getTextHeight(subTextArea->fontId, subTextArea->text);
        subTextArea->obj.alignment   = BOTTOM_MIDDLE;
        subTextArea->obj.alignTo     = (nbgl_obj_t *) progress;
        subTextArea->obj.alignmentMarginX = 0;
        subTextArea->obj.alignmentMarginY = 4;
        addObjectToLayout(layoutInt, (nbgl_obj_t *) subTextArea);
    }

    return 0;
}

#ifdef NBGL_KEYBOARD
/**
 * @brief Creates a keyboard on bottom of the screen, with the given configuration
 *
 * @param layout the current layout
 * @param kbdInfo configuration of the keyboard to draw (including the callback when touched)
 * @return the index of keyboard, to use in @ref nbgl_layoutUpdateKeyboard()
 */
int nbgl_layoutAddKeyboard(nbgl_layout_t *layout, const nbgl_layoutKbd_t *kbdInfo)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    nbgl_keyboard_t       *keyboard;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutAddKeyboard():\n");
    if (layout == NULL) {
        return -1;
    }

    // create keyboard
    keyboard = (nbgl_keyboard_t *) nbgl_objPoolGet(KEYBOARD, layoutInt->layer);
    keyboard->obj.alignmentMarginY = 0;
    keyboard->obj.alignment        = CENTER;
    keyboard->enableBackspace      = kbdInfo->enableBackspace;
    keyboard->enableValidate       = kbdInfo->enableValidate;
    if (kbdInfo->lettersOnly) {
        keyboard->selectedCharIndex = cx_rng_u32() % 26;
        keyboard->mode              = MODE_LOWER_LETTERS;
    }
    else {
        keyboard->mode = MODE_NONE;
    }
    keyboard->callback    = PIC(kbdInfo->callback);
    keyboard->lettersOnly = kbdInfo->lettersOnly;
    keyboard->keyMask     = kbdInfo->keyMask;
    // set this new keyboard as child of the container
    addObjectToLayout(layoutInt, (nbgl_obj_t *) keyboard);

    // return index of keyboard to be modified later on
    return (layoutInt->nbChildren - 1);
}

/**
 * @brief Updates an existing keyboard on bottom of the screen, with the given configuration
 *
 * @param layout the current layout
 * @param index index returned by @ref nbgl_layoutAddKeyboard()
 * @param keyMask mask of keys to activate/deactivate on keyboard
 * @return >=0 if OK
 */
int nbgl_layoutUpdateKeyboard(nbgl_layout_t *layout, uint8_t index, uint32_t keyMask)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    nbgl_keyboard_t       *keyboard;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutUpdateKeyboard(): keyMask = 0x%X\n", keyMask);
    if (layout == NULL) {
        return -1;
    }

    // get keyboard at given index
    keyboard = (nbgl_keyboard_t *) layoutInt->children[index];
    if ((keyboard == NULL) || (keyboard->obj.type != KEYBOARD)) {
        return -1;
    }
    keyboard->keyMask = keyMask;
    if (keyboard->lettersOnly) {
        if (keyMask & (1 << 26)) {
            keyboard->selectedCharIndex = cx_rng_u32() % 26;
        }
        else {
            keyboard->selectedCharIndex = 0;
        }
    }

    nbgl_redrawObject((nbgl_obj_t *) keyboard, NULL, false);

    return 0;
}

/**
 * @brief Adds a "text entry" area under the previously entered object.
 *        The max number of really displayable characters is 8, even if there are 9 placeholders (_)
 *        If longer than 8 chars, the first ones are replaced by a '..'
 *        The 9th placeholder is never filled
 *
 * @param layout the current layout
 * @param text string to display in the area
 * @param lettersOnly if true, display 8 chars placeholders, otherwise 9
 * @return >= 0 if OK
 */
int nbgl_layoutAddEnteredText(nbgl_layout_t *layout, const char *text, bool lettersOnly)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    nbgl_text_entry_t     *textEntry;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutAddEnteredText():\n");
    if (layout == NULL) {
        return -1;
    }

    // create text area
    textEntry          = (nbgl_text_entry_t *) nbgl_objPoolGet(TEXT_ENTRY, layoutInt->layer);
    textEntry->text    = text;
    textEntry->fontId  = BAGL_FONT_OPEN_SANS_EXTRABOLD_11px_1bpp;
    textEntry->nbChars = lettersOnly ? 8 : 9;
    textEntry->obj.alignmentMarginY = 5;
    textEntry->obj.alignment        = BOTTOM_MIDDLE;
    textEntry->obj.area.width       = 98;
    textEntry->obj.area.height      = 16;

    // set this new text area as child of the container
    addObjectToLayout(layoutInt, (nbgl_obj_t *) textEntry);

    // return index of text area to be modified later on
    return (layoutInt->nbChildren - 1);
}

/**
 * @brief Updates an existing "text entry" area, created with @ref nbgl_layoutAddEnteredText()
 *
 * @param layout the current layout
 * @param index index of the text (return value of @ref nbgl_layoutAddEnteredText())
 * @param text string to display in the area
 * @return <0 if error, 0 if OK with text fitting the area, 1 of 0K with text
 * not fitting the area
 */
int nbgl_layoutUpdateEnteredText(nbgl_layout_t *layout, uint8_t index, const char *text)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    nbgl_text_entry_t     *textEntry;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutUpdateEnteredText():\n");
    if (layout == NULL) {
        return -1;
    }

    // update main text area
    textEntry = (nbgl_text_entry_t *) layoutInt->children[index];
    if ((textEntry == NULL) || (textEntry->obj.type != TEXT_ENTRY)) {
        return -1;
    }
    textEntry->text = text;
    nbgl_redrawObject((nbgl_obj_t *) textEntry, NULL, false);

    return 0;
}
#endif  // NBGL_KEYBOARD

#ifdef NBGL_KEYPAD
/**
 * @brief Adds a keypad on bottom of the screen, with the associated callback
 *
 * @note Validate and Backspace keys are not enabled at start-up
 *
 * @param layout the current layout
 * @param callback function called when any of the key is touched
 * @param text text to use as title for the keypad
 * @param shuffled if set to true, digits are shuffled in keypad
 * @return the index of keypad in layout, to use in @ref nbgl_layoutUpdateKeypad()
 */
int nbgl_layoutAddKeypad(nbgl_layout_t     *layout,
                         keyboardCallback_t callback,
                         const char        *text,
                         bool               shuffled)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    nbgl_keypad_t         *keypad;
    nbgl_text_area_t      *textArea;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutAddKeypad():\n");
    if (layout == NULL) {
        return -1;
    }

    textArea                  = (nbgl_text_area_t *) nbgl_objPoolGet(TEXT_AREA, layoutInt->layer);
    textArea->textColor       = WHITE;
    textArea->text            = PIC(text);
    textArea->textAlignment   = CENTER;
    textArea->fontId          = BAGL_FONT_OPEN_SANS_REGULAR_11px_1bpp;
    textArea->obj.area.width  = AVAILABLE_WIDTH;
    textArea->obj.area.height = 12;
    textArea->wrapping        = false;
    textArea->obj.alignment   = TOP_MIDDLE;
    textArea->obj.alignmentMarginY = 3;
    // set this new obj as child of main container
    addObjectToLayout(layoutInt, (nbgl_obj_t *) textArea);

    // create keypad
    keypad                       = (nbgl_keypad_t *) nbgl_objPoolGet(KEYPAD, layoutInt->layer);
    keypad->obj.alignment        = BOTTOM_MIDDLE;
    keypad->obj.alignmentMarginY = 6;
    keypad->obj.alignTo          = NULL;
    keypad->callback             = PIC(callback);
    keypad->enableBackspace      = false;
    keypad->enableValidate       = false;
    keypad->selectedKey          = 0xFF;  // to be picked
    keypad->shuffled             = shuffled;
    // set this new keypad as child of the container
    addObjectToLayout(layoutInt, (nbgl_obj_t *) keypad);

    // return index of keypad to be modified later on
    return (layoutInt->nbChildren - 1);
}

/**
 * @brief Updates an existing keypad on bottom of the screen, with the given configuration
 *
 * @param layout the current layout
 * @param index index returned by @ref nbgl_layoutAddKeypad()
 * @param enableValidate if true, enable Validate key
 * @param enableBackspace if true, enable Backspace key
 * @return >=0 if OK
 */
int nbgl_layoutUpdateKeypad(nbgl_layout_t *layout,
                            uint8_t        index,
                            bool           enableValidate,
                            bool           enableBackspace)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    nbgl_keypad_t         *keypad;

    LOG_DEBUG(LAYOUT_LOGGER,
              "nbgl_layoutUpdateKeypad(): enableValidate = %d, enableBackspace = %d\n",
              enableValidate,
              enableBackspace);
    if (layout == NULL) {
        return -1;
    }

    // get existing keypad
    keypad = (nbgl_keypad_t *) layoutInt->children[index];
    if ((keypad == NULL) || (keypad->obj.type != KEYPAD)) {
        LOG_WARN(LAYOUT_LOGGER, "nbgl_layoutUpdateKeypad(): keypad not found\n");
        return -1;
    }
    if (enableValidate && !keypad->enableValidate) {
        // if validate key is enabled and was not, select it directly
        keypad->selectedKey = 11;
    }
    else {
        // otherwise let the draw function pick a new selected
        keypad->selectedKey = 0xFF;
    }
    keypad->enableValidate  = enableValidate;
    keypad->enableBackspace = enableBackspace;

    nbgl_redrawObject((nbgl_obj_t *) keypad, NULL, false);

    return 0;
}

/**
 * @brief Adds a placeholder for hidden digits on top of a keypad, to represent the entered digits,
 * as full circles
 *
 * @note It must be the last added object, after keypad. Vertical positions of title and hidden
 * digits will be computed here
 *
 * @param layout the current layout
 * @param nbDigits number of digits to be displayed
 * @return the index of digits set, to use in @ref nbgl_layoutUpdateHiddenDigits()
 */
int nbgl_layoutAddHiddenDigits(nbgl_layout_t *layout, uint8_t nbDigits)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    nbgl_container_t      *container;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutAddHiddenDigits():\n");
    if (layout == NULL) {
        return -1;
    }

    // create a container, invisible or bordered
    container             = (nbgl_container_t *) nbgl_objPoolGet(CONTAINER, layoutInt->layer);
    container->nbChildren = nbDigits;
    container->children   = nbgl_containerPoolGet(container->nbChildren, layoutInt->layer);
    // 1 pixel between each icon (knowing that the effective bullets are 8px large)
    container->obj.area.width  = nbDigits * C_pin_bullet_empty.width + (nbDigits - 1);
    container->obj.area.height = C_pin_bullet_empty.height;
    // distance from top to digits is fixed to 24 px
    container->obj.alignmentMarginY = 24;
    container->obj.alignTo          = NULL;
    container->obj.alignment        = TOP_MIDDLE;

    // set this new container as child of the main container
    addObjectToLayout(layoutInt, (nbgl_obj_t *) container);

    // create children of the container, as images (empty circles)
    nbgl_objPoolGetArray(IMAGE, nbDigits, layoutInt->layer, (nbgl_obj_t **) container->children);
    for (int i = 0; i < nbDigits; i++) {
        nbgl_image_t *image    = (nbgl_image_t *) container->children[i];
        image->buffer          = &C_pin_bullet_empty;
        image->foregroundColor = WHITE;
        if (i > 0) {
            image->obj.alignment        = MID_RIGHT;
            image->obj.alignTo          = (nbgl_obj_t *) container->children[i - 1];
            image->obj.alignmentMarginX = 1;
        }
        else {
            image->obj.alignment = NO_ALIGNMENT;
        }
    }
    // return index of container to be modified later on
    return (layoutInt->nbChildren - 1);
}

/**
 * @brief Updates an existing set of hidden digits, with the given configuration
 *
 * @param layout the current layout
 * @param index index returned by @ref nbgl_layoutAddHiddenDigits()
 * @param nbActive number of "active" digits (represented by discs instead of circles)
 * @return >=0 if OK
 */
int nbgl_layoutUpdateHiddenDigits(nbgl_layout_t *layout, uint8_t index, uint8_t nbActive)
{
    nbgl_layoutInternal_t *layoutInt = (nbgl_layoutInternal_t *) layout;
    nbgl_container_t      *container;
    nbgl_image_t          *image;

    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutUpdateHiddenDigits(): nbActive = %d\n", nbActive);
    if (layout == NULL) {
        return -1;
    }

    // get container
    container = (nbgl_container_t *) layoutInt->children[index];
    // sanity check
    if ((container == NULL) || (container->obj.type != CONTAINER)) {
        return -1;
    }
    if (nbActive > container->nbChildren) {
        return -1;
    }
    if (nbActive == 0) {
        // deactivate the first digit
        image = (nbgl_image_t *) container->children[0];
        if ((image == NULL) || (image->obj.type != IMAGE)) {
            return -1;
        }
        image->buffer = &C_pin_bullet_empty;
    }
    else {
        image = (nbgl_image_t *) container->children[nbActive - 1];
        if ((image == NULL) || (image->obj.type != IMAGE)) {
            return -1;
        }
        // if the last "active" is already active, it means that we are decreasing the number of
        // active otherwise we are increasing it
        if (image->buffer == &C_pin_bullet_filled) {
            // all digits are already active
            if (nbActive == container->nbChildren) {
                return 0;
            }
            // deactivate the next digit
            image         = (nbgl_image_t *) container->children[nbActive];
            image->buffer = &C_pin_bullet_empty;
        }
        else {
            image->buffer = &C_pin_bullet_filled;
        }
    }

    nbgl_redrawObject((nbgl_obj_t *) image, NULL, false);

    return 0;
}
#endif  // NBGL_KEYPAD

/**
 * @brief Applies given layout. The screen will be redrawn
 *
 * @param layoutParam layout to redraw
 * @return a pointer to the corresponding layout
 */
int nbgl_layoutDraw(nbgl_layout_t *layoutParam)
{
    nbgl_layoutInternal_t *layout = (nbgl_layoutInternal_t *) layoutParam;

    if (layout == NULL) {
        return -1;
    }
    LOG_DEBUG(LAYOUT_LOGGER, "nbgl_layoutDraw(): layout->nbChildren = %d\n", layout->nbChildren);
    nbgl_screenRedraw();

    return 0;
}

/**
 * @brief Release the layout obtained with @ref nbgl_layoutGet()
 *
 * @param layoutParam layout to release
 * @return >= 0 if OK
 */
int nbgl_layoutRelease(nbgl_layout_t *layoutParam)
{
    nbgl_layoutInternal_t *layout = (nbgl_layoutInternal_t *) layoutParam;
    LOG_DEBUG(PAGE_LOGGER, "nbgl_layoutRelease(): \n");
    if (layout == NULL) {
        return -1;
    }
    // if modal
    if (layout->modal) {
        nbgl_screenPop(layout->layer);
    }
    layout->nbChildren = 0;
    return 0;
}
#endif  // HAVE_SE_TOUCH
