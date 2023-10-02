/**
 * @file nbgl_page.h
 * @brief API of the Advanced BOLOS Graphical Library, for predefined pages
 *
 */

#ifndef NBGL_PAGE_H
#define NBGL_PAGE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "nbgl_layout.h"
#include "nbgl_obj.h"
#include "nbgl_types.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * @brief The different types of top-right / bottom button
 *
 */
typedef enum {
    NO_BUTTON_STYLE=0, ///< no button.
    SETTINGS_ICON, ///< settings (wheel) icon in the button.
    QUIT_ICON, ///< quit (X) icon in the button.
    INFO_ICON, ///< info (i) icon in the button.
    QUIT_APP_TEXT ///< A full width button with "Quit app" text (only for bottom button)
} nbgl_pageButtonStyle_t;

/**
 * @brief The different types of predefined page contents
 *
 */
typedef enum {
    CENTERED_INFO=0, ///< a centered info
    INFO_LONG_PRESS, ///< a centered info and a long press button
    INFO_BUTTON, ///< a centered info and a simple black button
    TAG_VALUE_LIST, ///< list of tag/value pairs
    TAG_VALUE_DETAILS, ///< a tag/value pair and a small button to get details.
    TAG_VALUE_CONFIRM, ///< tag/value pairs and a black button/footer to confirm/cancel.
    SWITCHES_LIST, ///< list of switches with descriptions
    INFOS_LIST, ///< list of infos with titles
    CHOICES_LIST, ///< list of choices through radio buttons
    BARS_LIST ///< list of touchable bars (with > on the right to go to sub-pages)
} nbgl_pageContentType_t;

/**
 * @brief This structure contains a [item,value] pair and info about "details" button
 */
typedef struct nbgl_pageTagValueDetails_s {
    nbgl_layoutTagValueList_t tagValueList; ///< list of tag/value pairs
    const nbgl_icon_details_t *detailsButtonIcon; ///< icon to use in details button
    const char *detailsButtonText; ///< this text is used for "details" button
    uint8_t detailsButtonToken; ///< the token used as argument of the actionCallback when the "details" button is touched
    tune_index_e tuneId; ///< if not @ref NBGL_NO_TUNE, a tune will be played when details button is touched
} nbgl_pageTagValueDetails_t;

/**
 * @brief This structure contains [item,value] pair(s) and info about a potential "details" button, but also a black button + footer to confirm/cancel
 */
typedef struct nbgl_pageTagValueConfirm_s {
    nbgl_layoutTagValueList_t tagValueList; ///< list of tag/value pairs
    const nbgl_icon_details_t *detailsButtonIcon; ///< icon to use in details button
    const char *detailsButtonText; ///< this text is used for "details" button (if NULL, no button)
    uint8_t detailsButtonToken; ///< the token used as argument of the actionCallback when the "details" button is touched
    tune_index_e tuneId; ///< if not @ref NBGL_NO_TUNE, a tune will be played when details button is touched
    const char *confirmationText; ///< text of the confirmation button, if NULL "It matches" is used
    const char *cancelText; ///< the text used for cancel action, if NULL "It doesn't matches" is used
    uint8_t confirmationToken; ///< the token used as argument of the onActionCallback
    uint8_t cancelToken; ///< the token used as argument of the onActionCallback when the cancel button is pressed
} nbgl_pageTagValueConfirm_t;

/**
 * @brief This structure contains data to build a centered info + long press button page content
 */
typedef struct nbgl_pageInfoLongPress_s {
    char *text; ///< centered text in large case
    const nbgl_icon_details_t *icon; ///< a buffer containing the 1BPP icon
    const char *longPressText; ///< text of the long press button
    uint8_t longPressToken; ///< the token used as argument of the onActionCallback when button is long pressed
    tune_index_e tuneId; ///< if not @ref NBGL_NO_TUNE, a tune will be played when button is touched
} nbgl_pageInfoLongPress_t;

/**
 * @brief This structure contains data to build a centered info + simple black button page content
 */
typedef struct nbgl_pageInfoButton_s {
    char *text; ///< centered text in large case
    const nbgl_icon_details_t *icon; ///< a buffer containing the 1BPP icon
    const char *buttonText; ///< text of the long press button
    uint8_t buttonToken; ///< the token used as argument of the onActionCallback when button is long pressed
    tune_index_e tuneId; ///< if not @ref NBGL_NO_TUNE, a tune will be played when button is touched
} nbgl_pageInfoButton_t;

/**
 * @brief This structure contains data to build a @ref SWITCHES_LIST page content
 */
typedef struct nbgl_pageSwitchesList_s {
    nbgl_layoutSwitch_t *switches; ///< array of switches (nbSwitches items)
    uint8_t nbSwitches; ///< number of elements in switches and tokens array
} nbgl_pageSwitchesList_t;

/**
 * @brief This structure contains data to build a @ref INFOS_LIST page content
 */
typedef struct nbgl_pageInfoList_s {
    const char **infoTypes; ///< array of types of infos (in black/bold)
    const char **infoContents; ///< array of contents of infos (in black)
    uint8_t nbInfos; ///< number of elements in infoTypes and infoContents array
} nbgl_pageInfoList_t;

/**
 * @brief This structure contains data to build a @ref BARS_LIST page content
 */
typedef struct nbgl_pageBarsList_s {
    const char **barTexts; ///< array of texts for each bar (nbBars items, in black/bold)
    uint8_t *tokens; ///< array of tokens, one for each bar (nbBars items)
    uint8_t nbBars; ///< number of elements in barTexts and tokens array
    tune_index_e tuneId; ///< if not @ref NBGL_NO_TUNE, a tune will be played when a bar is touched
} nbgl_pageBarsList_t;

/**
 * @brief This structure contains data to build a page in multi-pages mode (@ref nbgl_pageDrawGenericContent)
 */
typedef struct nbgl_pageContent_s {
    const char *title; ///< text for the title of the page (if NULL, no title)
    bool isTouchableTitle; ///< if set to true, the title is preceded by <- arrow to go back
    uint8_t titleToken; ///< if isTouchableTitle set to true, this is the token used when touching title
    tune_index_e tuneId; ///< if not @ref NBGL_NO_TUNE, a tune will be played when title is touched
    nbgl_pageContentType_t type; ///< type of page content in the following union
    union {
        nbgl_layoutTagValueList_t tagValueList;    ///< @ref TAG_VALUE_LIST type
        nbgl_layoutCenteredInfo_t centeredInfo;     ///< @ref CENTERED_INFO type
        nbgl_pageInfoLongPress_t  infoLongPress;   ///< @ref INFO_LONG_PRESS type
        nbgl_pageInfoButton_t  infoButton;   ///< @ref INFO_BUTTON type
        nbgl_pageTagValueDetails_t tagValueDetails; ///< @ref TAG_VALUE_DETAILS type
        nbgl_pageTagValueConfirm_t tagValueConfirm; ///< @ref TAG_VALUE_CONFIRM type
        nbgl_pageSwitchesList_t switchesList;    ///< @ref SWITCHES_LIST type
        nbgl_pageInfoList_t infosList;              ///< @ref INFOS_LIST type
        nbgl_layoutRadioChoice_t choicesList;           ///< @ref CHOICES_LIST type
        nbgl_pageBarsList_t barsList;               ///< @ref BARS_LIST type
    };
} nbgl_pageContent_t;

/**
 * @brief type shared externally
 *
 */
typedef void* nbgl_page_t;


/**
 * @brief The different types of navigation in a multi-screens page
 *
 */
typedef enum {
    NAV_WITH_TAP, ///< move forward with "tap" and possibly backward with top left arrow
    NAV_WITH_BUTTONS, ///< move forward and backward with buttons in bottom nav bar
} nbgl_pageNavigationType_t;

/**
 * @brief Structure containing all information to create a navigation with "tap"
 *
 */
typedef struct nbgl_pageNavWithTap_s {
    bool backButton; ///< if set to true, a back button (left arrow) is displayed in the top left corner (if page >=1)
    uint8_t backToken; ///< the token used as argument of the actionCallback when the back button is touched
    uint8_t nextPageToken; ///< the token used as argument of the actionCallback when the main panel is "tapped"
    const char *nextPageText; ///< this text is used as indication for "Tap to continue", to navigate forward
    const char * quitText; ///< the text displayed in footer, used to quit
    const char *skipText; ///< if not NULL the text displayed in right part of footer, used for example to skip pages
    uint8_t skipToken; ///< if skipText is NULL the token used when right part of footer is touched
} nbgl_pageNavWithTap_t;

/**
 * @brief Structure containing all information to create a navigation with buttons in bottom nav bar
 *
 */
typedef struct nbgl_pageNavWithButtons_s {
    bool quitButton; ///< if set to true, a quit button (X) is displayed in the nav bar
    uint8_t navToken; ///< the token used as argument of the actionCallback when the nav buttons are pressed (index param gives the page)
} nbgl_pageNavWithButtons_t;

/**
 * @brief Structure containing all specific information when creating a multi-screens page.
 *
 */
typedef struct nbgl_pageMultiScreensDescription_s {
    uint8_t activePage; ///< the index of the page to display at start-up
    uint8_t nbPages; ///< the number of pages to display (if <2, no navigation bar)
    uint8_t quitToken; ///< the token used as argument of the actionCallback when the footer is touched
    nbgl_pageNavigationType_t navType; ///< type of navigation, it will tell which structure in the following union will be used
    bool progressIndicator; ///< if set to true, display a progress indicator on top of the page
    tune_index_e tuneId; ///< if not @ref NBGL_NO_TUNE, a tune will be played when next or back is pressed
    union {
        nbgl_pageNavWithTap_t navWithTap; ///< structure used when navigation with "tap"
        nbgl_pageNavWithButtons_t navWithButtons; ///< structure used when navigation with buttons
    };
} nbgl_pageNavigationInfo_t;

/**
 * @brief Structure containing all specific information when creating a confirmation page.
 */
typedef struct nbgl_pageConfirmationDescription_s {
    nbgl_layoutCenteredInfo_t centeredInfo; ///< description of the centered info to be used
    const char *confirmationText; ///< text of the confirmation button
    const char *cancelText; ///< the text used for cancel action, if NULL a simple X button is used
    uint8_t confirmationToken; ///< the token used as argument of the onActionCallback
    uint8_t cancelToken; ///< the token used as argument of the onActionCallback when the cancel button is pressed
    tune_index_e tuneId; ///< if not @ref NBGL_NO_TUNE, a tune will be played when button is pressed
    bool modal; ///< if true, page is open as a modal
} nbgl_pageConfirmationDescription_t;

/**
 * @brief Structure containing all specific information when creating a address confirmation page.
 * This page contains the address in a tag/value format, and a white button to dipsplay the same address as a QRCode in
 * a modal window. At the bottom 2 buttons allow to confirm or invalidate the address
 */
typedef struct nbgl_pageAddressConfirmationDescription_s {
    const char *address; ///< address to confirm
    const char *qrCodeButtonText; ///< text to display in "QR code button", if NULL "Show as QR is used"
    const char *confirmationText; ///< text of the confirmation button, if NULL "It matches" is used
    const char *cancelText; ///< the text used for cancel action, if NULL "It doesn't matches" is used
    uint8_t confirmationToken; ///< the token used as argument of the onActionCallback
    uint8_t cancelToken; ///< the token used as argument of the onActionCallback when the cancel button is pressed
    tune_index_e tuneId; ///< if not @ref NBGL_NO_TUNE, a tune will be played when button is pressed
} nbgl_pageAddressConfirmationDescription_t;

/**
 * @brief Structure containing all specific information when creating an information page.
 */
typedef struct nbgl_pageInfoDescription_s {
    nbgl_layoutCenteredInfo_t centeredInfo; ///< description of the centered info to be used
    nbgl_pageButtonStyle_t topRightStyle; ///< style to apply to the Top-Right button
    nbgl_pageButtonStyle_t bottomButtonStyle; ///< style to apply to the Bottom button
    uint8_t topRightToken; ///< the token that will be used as argument of the onActionCallback
    uint8_t bottomButtonsToken; ///< the token that will be used as argument of the onActionCallback if action/bottom button is touched
    const char *footerText; ///< if not NULL, add a touchable footer
    uint8_t footerToken; ///< the token that will be used as argument of the onActionCallback
    const char *tapActionText; ///< if set to true, main area is "tapable", with this text as indication
    uint8_t tapActionToken; ///< the token that will be used as argument of the onActionCallback
    const char *actionButtonText; ///< if not NULL a black "action" button is set under the centered info
    tune_index_e tuneId; ///< if not @ref NBGL_NO_TUNE, a tune will be played when button/footer is pressed
} nbgl_pageInfoDescription_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

nbgl_page_t* nbgl_pageDrawLedgerInfo(nbgl_layoutTouchCallback_t onActionCallback, nbgl_screenTickerConfiguration_t *ticker, const char* text, int tapActionToken);
nbgl_page_t* nbgl_pageDrawSpinner(nbgl_layoutTouchCallback_t onActionCallback, const char* text);
nbgl_page_t* nbgl_pageDrawInfo(nbgl_layoutTouchCallback_t onActionCallback, nbgl_screenTickerConfiguration_t *ticker, nbgl_pageInfoDescription_t *info);
nbgl_page_t* nbgl_pageDrawConfirmation(nbgl_layoutTouchCallback_t onActionCallback, nbgl_pageConfirmationDescription_t *info);
nbgl_page_t* nbgl_pageDrawGenericContentExt(nbgl_layoutTouchCallback_t onActionCallback,
                                            nbgl_pageNavigationInfo_t *nav,
                                            nbgl_pageContent_t* content,
                                            bool modal);
nbgl_page_t* nbgl_pageDrawGenericContent(nbgl_layoutTouchCallback_t onActionCallback,
                        nbgl_pageNavigationInfo_t *nav,
                        nbgl_pageContent_t* content);
int nbgl_pageRelease(nbgl_page_t*);

/**********************
 *      MACROS
 **********************/


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NBGL_PAGE_H */
