/**
 * @file nbgl_layout.h
 * @brief API of the Advanced BOLOS Graphical Library, for predefined layouts
 *
 */

#ifndef NBGL_LAYOUT_H
#define NBGL_LAYOUT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nbgl_obj.h"
#include "nbgl_screen.h"
#include "nbgl_types.h"
#include "os_io_seproxyhal.h"

/*********************
 *      INCLUDES
 *********************/


/*********************
 *      DEFINES
 *********************/
#define NO_MORE_OBJ_ERROR -3
#define NBGL_NO_TUNE NB_TUNES

#define NB_MAX_SUGGESTION_BUTTONS 4

/**********************
 *      TYPEDEFS
 **********************/

/**
 * @brief prototype of function to be called an object is touched
 * @param token integer passed when registering callback
 * @param index when the object touched is a list of radio buttons, gives the index of the activated button
 */
typedef void (*nbgl_layoutTouchCallback_t)(int token, uint8_t index);


/**
 * @brief type shared externally
 *
 */
typedef void* nbgl_layout_t;

/**
 * @brief This structure contains info to build a navigation bar at the bottom of the screen
 * @note this widget is incompatible with a footer.
 *
 */
typedef struct {
    uint8_t token; ///< the token that will be used as argument of the callback
    uint8_t nbPages; ///< number of pages. (if 0, no navigation)
    uint8_t activePage; ///< index of active page (from 0 to nbPages-1).
    bool withExitKey; ///< if set to true, an exit button is drawn, either on the left of navigation keys or in the center if no navigation
    bool withSeparationLine; ///< if set to true, an horizontal line is drawn on top of bar in light gray
    tune_index_e tuneId; ///< if not @ref NBGL_NO_TUNE, a tune will be played when pressing keys)
} nbgl_layoutNavigationBar_t;


/**
 * @brief Structure containing all information when creating a layout. This structure must be passed as argument to @ref nbgl_layoutGet
 * @note It shall not be used
 *
 */
typedef struct nbgl_layoutDescription_s {
    bool modal; ///< if true, puts the layout on top of screen stack (modal). Otherwise puts on background (for apps)
    bool withLeftBorder; ///< if true, draws a light gray left border on the whole height of the screen
    char *tapActionText; ///< Light gray text used when main container is "tapable"
    uint8_t tapActionToken; ///< the token that will be used as argument of the onActionCallback when main container is "tapped"
    tune_index_e tapTuneId; ///< if not @ref NBGL_NO_TUNE, a tune will be played when tapping on main container
    nbgl_layoutTouchCallback_t onActionCallback; ///< the callback to be called on any action on the layout
    nbgl_screenTickerConfiguration_t ticker; // configuration of ticker (timeout)
} nbgl_layoutDescription_t;


/**
 * @brief This structure contains info to build a clickable "bar" with a text and an icon
 *
 */
typedef struct {
    const nbgl_icon_details_t *iconLeft; ///< a buffer containing the 1BPP icon for icon on left (can be NULL)
    char *text; ///< text (can be NULL)
    const nbgl_icon_details_t *iconRight; ///< a buffer containing the 1BPP icon for icon 2 (can be NULL). Dimensions must be the same as iconLeft
    char *subText; ///< sub text (can be NULL)
    uint8_t token; ///< the token that will be used as argument of the callback
    bool inactive; ///< if set to true, the bar is grayed-out and cannot be touched
    bool centered; ///< if set to true, the text is centered horizontaly in the bar
    tune_index_e tuneId; ///< if not @ref NBGL_NO_TUNE, a tune will be played
} nbgl_layoutBar_t;

/**
 * @brief This structure contains info to build a switch (on the right) with a description (on the left), with a
 * potential sub-description (in gray)
 *
 */
typedef struct {
    char *text; ///< main text for the switch
    char *subText; ///< description under main text (NULL terminated, single line, may be null)
    nbgl_state_t initState; ///< initial state of the switch
    uint8_t token; ///< the token that will be used as argument of the callback
    tune_index_e tuneId; ///< if not @ref NBGL_NO_TUNE, a tune will be played
} nbgl_layoutSwitch_t;

/**
 * @brief This structure contains a list of names to build a list of radio
 * buttons (on the right part of screen), with for each a description (names array)
 * The chosen item index is provided is the "index" argument of the callback
 */
typedef struct {
    union {
        char **names; ///< array of strings giving the choices (nbChoices)
#if defined(HAVE_LANGUAGE_PACK)
        UX_LOC_STRINGS_INDEX *nameIds; ///< array of string Ids giving the choices (nbChoices)
#endif // HAVE_LANGUAGE_PACK
    };
    bool localized; ///< if set to true, use nameIds and not names
    uint8_t nbChoices; ///< number of choices
    uint8_t initChoice; ///< index of the current choice
    uint8_t token; ///< the token that will be used as argument of the callback
    tune_index_e tuneId; ///< if not @ref NBGL_NO_TUNE, a tune will be played when selecting a radio button)
} nbgl_layoutRadioChoice_t;

/**
 * @brief This structure contains a  [tag,value] pair
 */
typedef struct {
    char *item; ///< string giving the tag name
    char *value; ///< string giving the value name
    const nbgl_icon_details_t *valueIcon; ///< a buffer containing the 32px 1BPP icon for icon on right of value (can be NULL)
} nbgl_layoutTagValue_t;

/**
 * @brief prototype of tag/value pair retrieval callback
 * @param pairIndex index of the tag/value pair to retrieve (from 0 (to nbPairs-1))
 * @return a pointer on a static tag/value pair
 */
typedef nbgl_layoutTagValue_t* (*nbgl_tagValueCallback_t)(uint8_t pairIndex);


/**
 * @brief This structure contains a list of [tag,value] pairs
 */
typedef struct {
    nbgl_layoutTagValue_t *pairs; ///< array of [tag,value] pairs (nbPairs items). If NULL, callback is used instead
    nbgl_tagValueCallback_t callback; ///< function to call to retrieve a given pair
    uint8_t nbPairs; ///< number of pairs in pairs array (or max number of pairs to retrieve with callback)
    uint8_t startIndex; ///< index of the first pair to get with callback
    uint8_t nbMaxLinesForValue; ///< if > 0, set the max number of lines for value field. And the last line is ended with "..." instead of the 3 last chars
    uint8_t token; ///< the token that will be used as argument of the callback if icon in any tag/value pair is touched (index is the index of the pair in pairs[])
    bool smallCaseForValue; ///< if set to true, a 24px font is used for value text, otherwise a 32px font is used
    bool wrapping; ///< if set to true, value text will be wrapped on ' ' to avoid cutting words
} nbgl_layoutTagValueList_t;

/**
 * @brief possible styles for Centered Info Area
 *
 */
typedef enum  {
    LARGE_CASE_INFO, ///< text in BLACK and large case (INTER 32px), subText in black in Inter24px
    LARGE_CASE_BOLD_INFO, ///< text in BLACK and large case (INTER 32px), subText in black bold Inter24px, text3 in black Inter24px
    LEDGER_INFO, ///< only text, in BLACK and upper case (HM ALPHA MONO), with Ledger border
    NORMAL_INFO, ///< Icon in black, a potential text in black bold 24px under it, a potential text in dark gray (24px) under it, a potential text in black (24px) under it
    PLUGIN_INFO ///< A potential text in black 32px, a potential text in black (24px) under it, a small horizontal line under it, a potential icon under it, a potential text in black (24px) under it
} nbgl_centeredInfoStyle_t;

/**
 * @brief This structure contains info to build a centered (vertically and horizontally) area, with a possible Icon,
 * a possible text under it, and a possible sub-text gray under it.
 *
 */
typedef struct {
    char *text1; ///< first text (can be null)
    char *text2; ///< second text (can be null)
    char *text3; ///< third text (can be null)
    const nbgl_icon_details_t *icon; ///< a buffer containing the 1BPP icon
    bool onTop; ///< if set to true, align only horizontaly
    nbgl_centeredInfoStyle_t style; ///< style to apply to this info
    int16_t offsetY; ///< vertical shift to apply to this info (if >0, shift to bottom)
} nbgl_layoutCenteredInfo_t;

/**
 * @brief This structure contains info to build a centered (vertically and horizontally) area, with a QR Code,
 * a possible text (black, bold) under it, and a possible sub-text (black, regular) under it.
 *
 */
typedef struct {
    char *url; ///< URL for QR code
    char *text1; ///< first text (can be null)
    char *text2; ///< second text (can be null)
    bool largeText1; ///< if set to true, use 32px font for text1
} nbgl_layoutQRCode_t;

/**
 * @brief The different styles for a pair of buttons
 *
 */
typedef enum {
    ROUNDED_AND_FOOTER_STYLE = 0, ///< A rounded black background full width button on top of a footer
    BOTH_ROUNDED_STYLE  ///< A rounded black background full width button on top of a rounded white background full width button
} nbgl_layoutChoiceButtonsStyle_t;


/**
 * @brief This structure contains info to build a pair of buttons, one on top of the other.
 *
 * @note the pair of button is automatically put on bottom of screen
 */
typedef struct {
    char *topText; ///< up-button text (index 0)
    char *bottomText; ///< bottom-button text (index 1)
    uint8_t token; ///< the token that will be used as argument of the callback
    nbgl_layoutChoiceButtonsStyle_t style; ///< the style of the pair
    tune_index_e tuneId; ///< if not @ref NBGL_NO_TUNE, a tune will be played
} nbgl_layoutChoiceButtons_t;

/**
 * @brief The different styles for a button
 *
 */
typedef enum {
    BLACK_BACKGROUND = 0, ///< rounded bordered button, with text/icon in white, on black background
    WHITE_BACKGROUND, ///< rounded bordered button, with text/icon in black, on white background
    NO_BORDER, ///< simple clickable text, in black
    LONG_PRESS ///< long press button, with progress indicator
} nbgl_layoutButtonStyle_t;

/**
 * @brief This structure contains info to build a single button
 */
typedef struct {
    char *text; ///< button text
    const nbgl_icon_details_t *icon; ///< a buffer containing the 1BPP icon for button1
    uint8_t token; ///< the token that will be used as argument of the callback
    nbgl_layoutButtonStyle_t style;
    bool fittingContent; ///< if set to true, fit the width of button to text, otherwise full width
    bool onBottom; ///< if set to true, align on bottom of page, otherwise put on bottom of previous object
    tune_index_e tuneId; ///< if not @ref NBGL_NO_TUNE, a tune will be played
} nbgl_layoutButton_t;

/**
 * @brief This structure contains info to build a progress bar with info. The progress bar itself is 120px width * 12px height
 *
 */
typedef struct {
    uint8_t percentage; ///< percentage of completion, from 0 to 100.
    char *text; ///< text in black, on top of progress bar
    char *subText; ///< text in gray, under progress bar
} nbgl_layoutProgressBar_t;


/**
 * @brief This structure contains info to build a keyboard with @ref nbgl_layoutAddKeyboard()
 *
 */
typedef struct {
    bool lettersOnly; ///< if true, only display letter keys and Backspace
    keyboardCase_t casing; ///< keyboard casing mode (lower, upper once or upper locked)
    keyboardMode_t mode; ///< keyboard mode to start with
    uint32_t keyMask; ///< mask used to disable some keys in letters only mod. The 26 LSB bits of mask are used, for the 26 letters of a QWERTY keyboard. Bit[0] for Q, Bit[1] for W and so on
    keyboardCallback_t callback; ///< function called when an active key is pressed
} nbgl_layoutKbd_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
nbgl_layout_t *nbgl_layoutGet(nbgl_layoutDescription_t *description);

int nbgl_layoutAddTopRightButton(nbgl_layout_t *layout, const nbgl_icon_details_t *icon, uint8_t token, tune_index_e tuneId);
int nbgl_layoutAddTouchableBar(nbgl_layout_t *layout, nbgl_layoutBar_t *barLayout);
int nbgl_layoutAddSwitch(nbgl_layout_t *layout, nbgl_layoutSwitch_t *switchLayout);
int nbgl_layoutAddText(nbgl_layout_t *layout, char *text, char *subText);
int nbgl_layoutAddRadioChoice(nbgl_layout_t *layout, nbgl_layoutRadioChoice_t *choices);
int nbgl_layoutAddCenteredInfo(nbgl_layout_t *layout, nbgl_layoutCenteredInfo_t *info);
int nbgl_layoutAddQRCode(nbgl_layout_t *layout, nbgl_layoutQRCode_t *info);
int nbgl_layoutAddChoiceButtons(nbgl_layout_t *layout, nbgl_layoutChoiceButtons_t *info);
int nbgl_layoutAddTagValueList(nbgl_layout_t *layout, nbgl_layoutTagValueList_t *list);
int nbgl_layoutAddProgressBar(nbgl_layout_t *layout, nbgl_layoutProgressBar_t *barLayout);
int nbgl_layoutAddLargeCaseText(nbgl_layout_t *layout, char *text);
int nbgl_layoutAddSeparationLine(nbgl_layout_t *layout);

int nbgl_layoutAddButton(nbgl_layout_t *layout, nbgl_layoutButton_t *buttonInfo);
int nbgl_layoutAddLongPressButton(nbgl_layout_t *layout, char *text, uint8_t token, tune_index_e tuneId);
int nbgl_layoutAddFooter(nbgl_layout_t *layout, char *text, uint8_t token, tune_index_e tuneId);
int nbgl_layoutAddSplitFooter(nbgl_layout_t *layout, char *leftText, uint8_t leftToken, char *rightText, uint8_t rightToken, tune_index_e tuneId);
int nbgl_layoutAddNavigationBar(nbgl_layout_t *layout, nbgl_layoutNavigationBar_t *info);
int nbgl_layoutAddBottomButton(nbgl_layout_t *layout, const nbgl_icon_details_t *icon, uint8_t token, bool separationLine, tune_index_e tuneId);
int nbgl_layoutAddProgressIndicator(nbgl_layout_t *layout, uint8_t activePage, uint8_t nbPages, bool withBack, uint8_t backToken, tune_index_e tuneId);
int nbgl_layoutAddSpinner(nbgl_layout_t *layout, char *text, bool fixed);

#ifdef NBGL_KEYBOARD
/* layout objects for page with keyboard */
int nbgl_layoutAddKeyboard(nbgl_layout_t *layout, nbgl_layoutKbd_t *kbdInfo);
int nbgl_layoutUpdateKeyboard(nbgl_layout_t *layout, uint8_t index, uint32_t keyMask, bool updateCasing, keyboardCase_t casing);
bool nbgl_layoutKeyboardNeedsRefresh(nbgl_layout_t *layout, uint8_t index);
int nbgl_layoutAddSuggestionButtons(nbgl_layout_t *layout, uint8_t nbUsedButtons,
                                    char *buttonTexts[NB_MAX_SUGGESTION_BUTTONS],
                                    int firstButtonToken, tune_index_e tuneId);
int nbgl_layoutUpdateSuggestionButtons(nbgl_layout_t *layout, uint8_t index, uint8_t nbUsedButtons,
                                    char *buttonTexts[NB_MAX_SUGGESTION_BUTTONS]);
int nbgl_layoutAddEnteredText(nbgl_layout_t *layout, bool numbered, uint8_t number, char *text, bool grayedOut, int offsetY, int token);
int nbgl_layoutUpdateEnteredText(nbgl_layout_t *layout, uint8_t index, bool numbered, uint8_t number, char *text, bool grayedOut);
int nbgl_layoutAddConfirmationButton(nbgl_layout_t *layout, bool active, char *text, int token, tune_index_e tuneId);
int nbgl_layoutUpdateConfirmationButton(nbgl_layout_t *layout, uint8_t index, bool active, char *text);
#endif // NBGL_KEYBOARD

#ifdef NBGL_KEYPAD
/* layout objects for page with keypad */
int nbgl_layoutAddKeypad(nbgl_layout_t *layout, keyboardCallback_t callback, bool shuffled);
int nbgl_layoutUpdateKeypad(nbgl_layout_t *layout, uint8_t index, bool enableValidate, bool enableBackspace, bool enableDigits);
int nbgl_layoutAddHiddenDigits(nbgl_layout_t *layout, uint8_t nbDigits);
int nbgl_layoutUpdateHiddenDigits(nbgl_layout_t *layout, uint8_t index, uint8_t nbActive);
#endif // NBGL_KEYPAD

/* generic functions */
int nbgl_layoutDraw(nbgl_layout_t *layout);
int nbgl_layoutRelease(nbgl_layout_t *layout);

/**********************
 *      MACROS
 **********************/


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NBGL_LAYOUT_H */
