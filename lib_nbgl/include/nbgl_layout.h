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
#include "nbgl_content.h"
#ifdef HAVE_PIEZO_SOUND
#include "os_io_seproxyhal.h"
#endif

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/
#define NO_MORE_OBJ_ERROR -3
#define NBGL_NO_TUNE      NB_TUNES

#ifdef HAVE_SE_TOUCH
#define NB_MAX_SUGGESTION_BUTTONS 4
#ifdef TARGET_STAX
#define NB_MAX_VISIBLE_SUGGESTION_BUTTONS NB_MAX_SUGGESTION_BUTTONS
#else  // TARGET_STAX
#define NB_MAX_VISIBLE_SUGGESTION_BUTTONS 2
#endif  // TARGET_STAX

#define AVAILABLE_WIDTH (SCREEN_WIDTH - 2 * BORDER_MARGIN)
#else  // HAVE_SE_TOUCH
// 7 pixels on each side
#define AVAILABLE_WIDTH (SCREEN_WIDTH - 2 * 7)
// maximum number of lines in screen
#define NB_MAX_LINES    4

#endif  // HAVE_SE_TOUCH

/**********************
 *      TYPEDEFS
 **********************/

/**
 * @brief type shared externally
 *
 */
typedef void *nbgl_layout_t;

/**
 * @brief prototype of function to be called when an object is touched
 * @param token integer passed when registering callback
 * @param index when the object touched is a list of radio buttons, gives the index of the activated
 * button
 */
typedef void (*nbgl_layoutTouchCallback_t)(int token, uint8_t index);

/**
 * @brief prototype of function to be called when buttons are touched on a screen
 * @param layout layout concerned by the event
 * @param event type of button event
 */
typedef void (*nbgl_layoutButtonCallback_t)(nbgl_layout_t *layout, nbgl_buttonEvent_t event);

/**
 * @brief This structure contains info to build a navigation bar at the bottom of the screen
 * @note this widget is incompatible with a footer.
 *
 */
typedef struct {
    uint8_t token;            ///< the token that will be used as argument of the callback
    uint8_t nbPages;          ///< number of pages. (if 0, no navigation)
    uint8_t activePage;       ///< index of active page (from 0 to nbPages-1).
    bool    withExitKey;      ///< if set to true, an exit button is drawn, either on the left of
                              ///< navigation keys or in the center if no navigation
    bool withSeparationLine;  ///< if set to true, an horizontal line is drawn on top of bar in
                              ///< light gray
#ifdef HAVE_PIEZO_SOUND
    tune_index_e tuneId;  ///< if not @ref NBGL_NO_TUNE, a tune will be played when pressing keys)
#endif                    // HAVE_PIEZO_SOUND
} nbgl_layoutNavigationBar_t;

/**
 * @brief possible directions for Navigation arrows
 *
 */
typedef enum {
    HORIZONTAL_NAV,  ///< '<' and '>' are displayed, to navigate between pages and steps
    VERTICAL_NAV     ///< '\/' and '/\' are displayed, to navigate in a list (vertical scrolling)
} nbgl_layoutNavDirection_t;

/**
 * @brief possible styles for Navigation arrows (it's a bit field)
 *
 */
typedef enum {
    NO_ARROWS = 0,
    LEFT_ARROW,   ///< left arrow is used
    RIGHT_ARROW,  ///< right arrow is used
} nbgl_layoutNavIndication_t;

/**
 * @brief This structure contains info to build a navigation bar at the bottom of the screen
 * @note this widget is incompatible with a footer.
 *
 */
typedef struct {
    nbgl_layoutNavDirection_t  direction;   ///< vertical or horizontal navigation
    nbgl_layoutNavIndication_t indication;  ///< specifies which arrows to use (left or right)
} nbgl_layoutNavigation_t;

/**
 * @brief Structure containing all information when creating a layout. This structure must be passed
 * as argument to @ref nbgl_layoutGet
 * @note It shall not be used
 *
 */
typedef struct nbgl_layoutDescription_s {
    bool modal;  ///< if true, puts the layout on top of screen stack (modal). Otherwise puts on
                 ///< background (for apps)
#ifdef HAVE_SE_TOUCH
    bool withLeftBorder;  ///< if true, draws a light gray left border on the whole height of the
                          ///< screen
    const char *tapActionText;  ///< Light gray text used when main container is "tapable"
    uint8_t tapActionToken;     ///< the token that will be used as argument of the onActionCallback
                                ///< when main container is "tapped"
#ifdef HAVE_PIEZO_SOUND
    tune_index_e tapTuneId;  ///< if not @ref NBGL_NO_TUNE, a tune will be played when tapping on
                             ///< main container
#endif                       // HAVE_PIEZO_SOUND
    nbgl_layoutTouchCallback_t
        onActionCallback;  ///< the callback to be called on any action on the layout
#else                      // HAVE_SE_TOUCH
    nbgl_layoutButtonCallback_t
            onActionCallback;  ///< the callback to be called on any action on the layout
#endif                     // HAVE_SE_TOUCH
    nbgl_screenTickerConfiguration_t ticker;  // configuration of ticker (timeout)
} nbgl_layoutDescription_t;

/**
 * @brief This structure contains info to build a clickable "bar" with a text and an icon
 *
 */
typedef struct {
    const nbgl_icon_details_t
               *iconLeft;  ///< a buffer containing the 1BPP icon for icon on left (can be NULL)
    const char *text;      ///< text (can be NULL)
    const nbgl_icon_details_t *iconRight;  ///< a buffer containing the 1BPP icon for icon 2 (can be
                                           ///< NULL). Dimensions must be the same as iconLeft
    const char *subText;                   ///< sub text (can be NULL)
    uint8_t     token;     ///< the token that will be used as argument of the callback
    bool        inactive;  ///< if set to true, the bar is grayed-out and cannot be touched
    bool        centered;  ///< if set to true, the text is centered horizontaly in the bar
#ifdef HAVE_PIEZO_SOUND
    tune_index_e tuneId;  ///< if not @ref NBGL_NO_TUNE, a tune will be played
#endif                    // HAVE_PIEZO_SOUND
} nbgl_layoutBar_t;

/**
 * @brief Deprecated, kept for retro compatibility
 *
 */
typedef nbgl_contentSwitch_t nbgl_layoutSwitch_t;

/**
 * @brief Deprecated, kept for retro compatibility
 */
typedef nbgl_contentRadioChoice_t nbgl_layoutRadioChoice_t;

/**
 * @brief prototype of menu list item retrieval callback
 * @param choiceIndex index of the menu list item to retrieve (from 0 (to nbChoices-1))
 * @return a pointer on a string
 */
typedef const char *(*nbgl_menuListCallback_t)(uint8_t choiceIndex);

/**
 * @brief This structure contains a list of names to build a menu list on Nanos, with for each item
 * a description (names array)
 */
typedef struct {
    nbgl_menuListCallback_t callback;        ///< function to call to retrieve a menu list item text
    uint8_t                 nbChoices;       ///< total number of choices in the menu list
    uint8_t                 selectedChoice;  ///< index of the selected choice (centered, in bold)
} nbgl_layoutMenuList_t;

/**
 * @brief Deprecated, kept for retro compatibility
 */
typedef nbgl_contentTagValue_t nbgl_layoutTagValue_t;

/**
 * @brief Deprecated, kept for retro compatibility
 */
typedef nbgl_contentTagValueList_t nbgl_layoutTagValueList_t;

/**
 * @brief Deprecated, kept for retro compatibility
 *
 */
typedef nbgl_contentCenteredInfo_t nbgl_layoutCenteredInfo_t;

/**
 * @brief This structure contains info to build a centered (vertically and horizontally) area, with
 * a QR Code, a possible text (black, bold) under it, and a possible sub-text (black, regular) under
 * it.
 *
 */
typedef struct {
    const char *url;         ///< URL for QR code
    const char *text1;       ///< first text (can be null)
    const char *text2;       ///< second text (can be null)
    int16_t     offsetY;     ///< vertical shift to apply to this info (if > 0, shift to bottom)
    bool        centered;    ///< if set to true, center vertically
    bool        largeText1;  ///< if set to true, use 32px font for text1
} nbgl_layoutQRCode_t;

/**
 * @brief The different styles for a pair of buttons
 *
 */
typedef enum {
    ROUNDED_AND_FOOTER_STYLE
        = 0,            ///< A rounded black background full width button on top of a footer
    BOTH_ROUNDED_STYLE  ///< A rounded black background full width button on top of a rounded white
                        ///< background full width button
} nbgl_layoutChoiceButtonsStyle_t;

/**
 * @brief This structure contains info to build a pair of buttons, one on top of the other.
 *
 * @note the pair of button is automatically put on bottom of screen
 */
typedef struct {
    const char *topText;     ///< up-button text (index 0)
    const char *bottomText;  ///< bottom-button text (index 1)
    uint8_t     token;       ///< the token that will be used as argument of the callback
    nbgl_layoutChoiceButtonsStyle_t style;  ///< the style of the pair
#ifdef HAVE_PIEZO_SOUND
    tune_index_e tuneId;  ///< if not @ref NBGL_NO_TUNE, a tune will be played
#endif                    // HAVE_PIEZO_SOUND
} nbgl_layoutChoiceButtons_t;

/**
 * @brief The different styles for a button
 *
 */
typedef enum {
    BLACK_BACKGROUND
        = 0,           ///< rounded bordered button, with text/icon in white, on black background
    WHITE_BACKGROUND,  ///< rounded bordered button, with text/icon in black, on white background
    NO_BORDER,         ///< simple clickable text, in black
    LONG_PRESS         ///< long press button, with progress indicator
} nbgl_layoutButtonStyle_t;

/**
 * @brief This structure contains info to build a single button
 */
typedef struct {
    const char                *text;   ///< button text
    const nbgl_icon_details_t *icon;   ///< a buffer containing the 1BPP icon for button1
    uint8_t                    token;  ///< the token that will be used as argument of the callback
    nbgl_layoutButtonStyle_t   style;
    bool fittingContent;  ///< if set to true, fit the width of button to text, otherwise full width
    bool onBottom;        ///< if set to true, align on bottom of page, otherwise put on bottom of
                          ///< previous object
#ifdef HAVE_PIEZO_SOUND
    tune_index_e tuneId;  ///< if not @ref NBGL_NO_TUNE, a tune will be played
#endif                    // HAVE_PIEZO_SOUND
} nbgl_layoutButton_t;

/**
 * @brief This structure contains info to build a progress bar with info. The progress bar itself is
 * 120px width * 12px height
 *
 */
typedef struct {
    uint8_t     percentage;  ///< percentage of completion, from 0 to 100.
    const char *text;        ///< text in black, on top of progress bar
    const char *subText;     ///< text in gray, under progress bar
} nbgl_layoutProgressBar_t;

/**
 * @brief This structure contains info to build a keyboard with @ref nbgl_layoutAddKeyboard()
 *
 */
typedef struct {
    uint32_t keyMask;  ///< mask used to disable some keys in letters only mod. The 26 LSB bits of
                       ///< mask are used, for the 26 letters of a QWERTY keyboard. Bit[0] for Q,
                       ///< Bit[1] for W and so on
    keyboardCallback_t callback;     ///< function called when an active key is pressed
    bool               lettersOnly;  ///< if true, only display letter keys and Backspace
    keyboardMode_t     mode;         ///< keyboard mode to start with
#ifdef HAVE_SE_TOUCH
    keyboardCase_t casing;  ///< keyboard casing mode (lower, upper once or upper locked)
#else                       // HAVE_SE_TOUCH
    bool    enableBackspace;   ///< if true, Backspace key is enabled
    bool    enableValidate;    ///< if true, Validate key is enabled
    uint8_t selectedCharIndex;
#endif                      // HAVE_SE_TOUCH
} nbgl_layoutKbd_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
nbgl_layout_t *nbgl_layoutGet(const nbgl_layoutDescription_t *description);
int nbgl_layoutAddCenteredInfo(nbgl_layout_t *layout, const nbgl_layoutCenteredInfo_t *info);
int nbgl_layoutAddProgressBar(nbgl_layout_t *layout, const nbgl_layoutProgressBar_t *barLayout);

#ifdef HAVE_SE_TOUCH
int nbgl_layoutAddTopRightButton(nbgl_layout_t             *layout,
                                 const nbgl_icon_details_t *icon,
                                 uint8_t                    token,
                                 tune_index_e               tuneId);
int nbgl_layoutAddTouchableBar(nbgl_layout_t *layout, const nbgl_layoutBar_t *barLayout);
int nbgl_layoutAddSwitch(nbgl_layout_t *layout, const nbgl_layoutSwitch_t *switchLayout);
int nbgl_layoutAddText(nbgl_layout_t *layout, const char *text, const char *subText);
int nbgl_layoutAddRadioChoice(nbgl_layout_t *layout, const nbgl_layoutRadioChoice_t *choices);
int nbgl_layoutAddQRCode(nbgl_layout_t *layout, const nbgl_layoutQRCode_t *info);
int nbgl_layoutAddChoiceButtons(nbgl_layout_t *layout, const nbgl_layoutChoiceButtons_t *info);
int nbgl_layoutAddTagValueList(nbgl_layout_t *layout, const nbgl_layoutTagValueList_t *list);
int nbgl_layoutAddLargeCaseText(nbgl_layout_t *layout, const char *text);
int nbgl_layoutAddSeparationLine(nbgl_layout_t *layout);

int nbgl_layoutAddButton(nbgl_layout_t *layout, const nbgl_layoutButton_t *buttonInfo);
int nbgl_layoutAddLongPressButton(nbgl_layout_t *layout,
                                  const char    *text,
                                  uint8_t        token,
                                  tune_index_e   tuneId);
int nbgl_layoutAddFooter(nbgl_layout_t *layout,
                         const char    *text,
                         uint8_t        token,
                         tune_index_e   tuneId);
int nbgl_layoutAddSplitFooter(nbgl_layout_t *layout,
                              const char    *leftText,
                              uint8_t        leftToken,
                              const char    *rightText,
                              uint8_t        rightToken,
                              tune_index_e   tuneId);
int nbgl_layoutAddNavigationBar(nbgl_layout_t *layout, const nbgl_layoutNavigationBar_t *info);
int nbgl_layoutAddBottomButton(nbgl_layout_t             *layout,
                               const nbgl_icon_details_t *icon,
                               uint8_t                    token,
                               bool                       separationLine,
                               tune_index_e               tuneId);
int nbgl_layoutAddProgressIndicator(nbgl_layout_t *layout,
                                    uint8_t        activePage,
                                    uint8_t        nbPages,
                                    bool           withBack,
                                    uint8_t        backToken,
                                    tune_index_e   tuneId);
int nbgl_layoutAddSpinner(nbgl_layout_t *layout, const char *text, bool fixed);
#else   // HAVE_SE_TOUCH
int nbgl_layoutAddText(nbgl_layout_t                  *layout,
                       const char                     *text,
                       const char                     *subText,
                       nbgl_contentCenteredInfoStyle_t style);
int nbgl_layoutAddNavigation(nbgl_layout_t *layout, nbgl_layoutNavigation_t *info);
int nbgl_layoutAddMenuList(nbgl_layout_t *layout, nbgl_layoutMenuList_t *list);
#endif  // HAVE_SE_TOUCH

#ifdef NBGL_KEYBOARD
/* layout objects for page with keyboard */
int nbgl_layoutAddKeyboard(nbgl_layout_t *layout, const nbgl_layoutKbd_t *kbdInfo);
#ifdef HAVE_SE_TOUCH
int  nbgl_layoutUpdateKeyboard(nbgl_layout_t *layout,
                               uint8_t        index,
                               uint32_t       keyMask,
                               bool           updateCasing,
                               keyboardCase_t casing);
bool nbgl_layoutKeyboardNeedsRefresh(nbgl_layout_t *layout, uint8_t index);
int  nbgl_layoutAddSuggestionButtons(nbgl_layout_t *layout,
                                     uint8_t        nbUsedButtons,
                                     const char    *buttonTexts[NB_MAX_SUGGESTION_BUTTONS],
                                     int            firstButtonToken,
                                     tune_index_e   tuneId);
int  nbgl_layoutUpdateSuggestionButtons(nbgl_layout_t *layout,
                                        uint8_t        index,
                                        uint8_t        nbUsedButtons,
                                        const char    *buttonTexts[NB_MAX_SUGGESTION_BUTTONS]);
int  nbgl_layoutAddEnteredText(nbgl_layout_t *layout,
                               bool           numbered,
                               uint8_t        number,
                               const char    *text,
                               bool           grayedOut,
                               int            offsetY,
                               int            token);
int  nbgl_layoutUpdateEnteredText(nbgl_layout_t *layout,
                                  uint8_t        index,
                                  bool           numbered,
                                  uint8_t        number,
                                  const char    *text,
                                  bool           grayedOut);
int  nbgl_layoutAddConfirmationButton(nbgl_layout_t *layout,
                                      bool           active,
                                      const char    *text,
                                      int            token,
                                      tune_index_e   tuneId);
int  nbgl_layoutUpdateConfirmationButton(nbgl_layout_t *layout,
                                         uint8_t        index,
                                         bool           active,
                                         const char    *text);
#else   // HAVE_SE_TOUCH
int nbgl_layoutUpdateKeyboard(nbgl_layout_t *layout, uint8_t index, uint32_t keyMask);
int nbgl_layoutAddEnteredText(nbgl_layout_t *layout, const char *text, bool lettersOnly);
int nbgl_layoutUpdateEnteredText(nbgl_layout_t *layout, uint8_t index, const char *text);
#endif  // HAVE_SE_TOUCH
#endif  // NBGL_KEYBOARD

#ifdef NBGL_KEYPAD
#ifdef HAVE_SE_TOUCH
/* layout objects for page with keypad (Stax) */
int nbgl_layoutAddKeypad(nbgl_layout_t *layout, keyboardCallback_t callback, bool shuffled);
int nbgl_layoutUpdateKeypad(nbgl_layout_t *layout,
                            uint8_t        index,
                            bool           enableValidate,
                            bool           enableBackspace,
                            bool           enableDigits);
int nbgl_layoutAddHiddenDigits(nbgl_layout_t *layout, uint8_t nbDigits);
int nbgl_layoutUpdateHiddenDigits(nbgl_layout_t *layout, uint8_t index, uint8_t nbActive);
int nbgl_layoutAddSwipe(nbgl_layout_t *layout, uint8_t token, uint16_t swipesMask);

#else   // HAVE_SE_TOUCH
/* layout objects for pages with keypad (nanos) */
int nbgl_layoutAddKeypad(nbgl_layout_t     *layout,
                         keyboardCallback_t callback,
                         const char        *text,
                         bool               shuffled);
int nbgl_layoutUpdateKeypad(nbgl_layout_t *layout,
                            uint8_t        index,
                            bool           enableValidate,
                            bool           enableBackspace);
int nbgl_layoutAddHiddenDigits(nbgl_layout_t *layout, uint8_t nbDigits);
int nbgl_layoutUpdateHiddenDigits(nbgl_layout_t *layout, uint8_t index, uint8_t nbActive);
#endif  // HAVE_SE_TOUCH
#endif  // NBGL_KEYPAD

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
