/**
 * @file nbgl_use_case.c
 * @brief Implementation of typical pages (or sets of pages) for Applications
 */

/*********************
 *      INCLUDES
 *********************/
#include <string.h>
#include <stdio.h>
#include "nbgl_debug.h"
#include "nbgl_use_case.h"
#include "glyphs.h"
#include "os_pic.h"

/*********************
 *      DEFINES
 *********************/
#define APP_DESCRIPTION_MAX_LEN 128
// maximum number of lines for value field in details pages
#define NB_MAX_LINES_IN_DETAILS  12
// maximum number of lines for value field in review pages
#define NB_MAX_LINES_IN_REVIEW    6

// height available for tag/value pairs display
#define TAG_VALUE_AREA_HEIGHT   400

/**********************
 *      TYPEDEFS
 **********************/
enum {
  BACK_TOKEN=0,
  NEXT_TOKEN,
  QUIT_TOKEN,
  NAV_TOKEN,
  CONTINUE_TOKEN,
  BUTTON_TOKEN,
  CHOICE_TOKEN,
  DETAILS_BUTTON_TOKEN,
  LONG_PRESS_TOKEN,
  REJECT_TOKEN
};

typedef struct DetailsContext_s {
  uint8_t nbPages;
  uint8_t currentPage;
  int8_t previousPage;
  char *tag;
  char *value;
  char *previousPageStart;
  char *currentPageStart;
  char *nextPageStart;
} DetailsContext_t;

typedef struct StaticReviewContext_s {
  nbgl_layoutTagValueList_t *tagValueList;
  nbgl_pageInfoLongPress_t *infoLongPress;
  uint8_t currentPairIndex;
  uint8_t nbPairsInCurrentPage;
} StaticReviewContext_t;

typedef struct AddressConfirmationContext_s {
  char *address;
  nbgl_layout_t layout;
  nbgl_layout_t modalLayout;
} AddressConfirmationContext_t;


/**********************
 *  STATIC VARIABLES
 **********************/
// char buffer to build some strings
static char appDescription[APP_DESCRIPTION_MAX_LEN];

// multi-purposes callbacks
static nbgl_callback_t onQuit;
static nbgl_callback_t onContinue;
static nbgl_navCallback_t onNav;
static nbgl_layoutTouchCallback_t onControls;
static nbgl_choiceCallback_t onChoice;

// contexts for background and modal pages
static nbgl_page_t *pageContext;
static nbgl_page_t *modalPageContext;

// context for settings pages
static char *settingsTitle;
static bool touchableTitle;
static nbgl_pageNavigationInfo_t navInfo;
static bool forwardNavOnly;

static DetailsContext_t detailsContext;

static StaticReviewContext_t staticReviewContext;

static AddressConfirmationContext_t addressConfirmationContext;

/**********************
 *  STATIC FUNCTIONS
 **********************/
static void displayReviewPage(uint8_t page);
static void displayDetailsPage(uint8_t page);
static void displaySettingsPage(uint8_t page);
static void displayStaticReviewPage(uint8_t page);
static void pageCallback(int token, uint8_t index);
static uint8_t getNbTagValuesInPage(uint8_t nbPairs, nbgl_layoutTagValue_t *pairs, bool forward, bool *tooLongToFit);

// function called when navigating (or exiting) modal details pages
static void pageModalCallback(int token, uint8_t index) {
  nbgl_pageRelease(modalPageContext);
  modalPageContext = NULL;
  if (token == NAV_TOKEN) {
    if (index == EXIT_PAGE) {
      // redraw the background layer
      nbgl_screenRedraw();
      nbgl_refresh();
    }
    else {
      displayDetailsPage(index);
    }
  }
}

// generic callback for all pages except modal
static void pageCallback(int token, uint8_t index) {
  nbgl_pageRelease(pageContext);
  pageContext = NULL;
  if (token == QUIT_TOKEN) {
    if (onQuit != NULL)
      onQuit();
  }
  else if (token == CONTINUE_TOKEN) {
    if (onContinue != NULL)
      onContinue();
  }
  else if (token == CHOICE_TOKEN) {
    if (onChoice != NULL)
      onChoice((index == 0)?true:false);
  }
  else if (token == LONG_PRESS_TOKEN) {
    if (onChoice != NULL)
      onChoice(true);
  }
  else if (token == REJECT_TOKEN) {
    if (onChoice != NULL)
      onChoice(false);
  }
  else if (token == DETAILS_BUTTON_TOKEN) {
    nbgl_useCaseViewDetails(staticReviewContext.tagValueList->pairs[staticReviewContext.currentPairIndex].item,
                            staticReviewContext.tagValueList->pairs[staticReviewContext.currentPairIndex].value);
  }
  else if (token == NAV_TOKEN) {
    if (index == EXIT_PAGE) {
      if (onQuit != NULL)
        onQuit();
    }
    else {
      displaySettingsPage(index);
    }
  }
  else if (token == NEXT_TOKEN) {
    if (onNav != NULL)
      displayReviewPage(navInfo.activePage+1);
    else
      displayStaticReviewPage(navInfo.activePage+1);
  }
  else if (token == BACK_TOKEN) {
    if (onNav != NULL)
      displayReviewPage(navInfo.activePage-1);
    else
      displayStaticReviewPage(navInfo.activePage-1);
  }
  else { // probably a control provided by caller
    if (onControls != NULL) {
      if (onControls != NULL)
        onControls(token, index);
    }
  }
}

// callback used for confirmation
static void tickerCallback(void) {
  nbgl_pageRelease(pageContext);
  if (onQuit != NULL)
    onQuit();
}

// function used to display the current page in review
static void displaySettingsPage(uint8_t page) {
  nbgl_pageContent_t content;

  if ((onNav == NULL) || (onNav(page, &content) == false))
    return;

  // override some fields
  content.title = settingsTitle;
  content.isTouchableTitle = touchableTitle;
  content.titleToken = QUIT_TOKEN;
  content.tuneId = TUNE_TAP_CASUAL;

  navInfo.activePage = page;
  pageContext = nbgl_pageDrawGenericContent(&pageCallback, &navInfo, &content);
  nbgl_refresh();
}

// function used to display the current page in review
static void displayReviewPage(uint8_t page) {
  nbgl_pageContent_t content;

  navInfo.activePage = page;
  if ((onNav == NULL) || (onNav(navInfo.activePage, &content) == false))
    return;

  // override some fields
  content.title = NULL;
  content.isTouchableTitle = false;
  content.tuneId = TUNE_TAP_CASUAL;

  if (!forwardNavOnly)
    navInfo.navWithTap.backButton = (navInfo.activePage==0)?false:true;

  if (content.type == INFO_LONG_PRESS) { // last page
    navInfo.navWithTap.nextPageText = NULL;
    content.infoLongPress.longPressToken = LONG_PRESS_TOKEN;
  }
  else {
    navInfo.navWithTap.nextPageText = "Tap to continue";
  }

  if (content.type == TAG_VALUE_DETAILS) {
    content.tagValueList.smallCaseForValue = false;
    content.tagValueList.nbMaxLinesForValue = NB_MAX_LINES_IN_REVIEW;
  }

  pageContext = nbgl_pageDrawGenericContent(&pageCallback, &navInfo, &content);
  nbgl_refresh();
}

// function used to display the current page in static review (no call to user to get content)
static void displayStaticReviewPage(uint8_t page) {
  nbgl_pageContent_t content;

  // if it's the last page, display a long press button with the info provided by user (except token)
  if (page == (navInfo.nbPages - 1)) {
    content.type = INFO_LONG_PRESS;
    content.infoLongPress.icon = staticReviewContext.infoLongPress->icon;
    content.infoLongPress.longPressText = staticReviewContext.infoLongPress->longPressText;
    content.infoLongPress.text = staticReviewContext.infoLongPress->text;
    content.infoLongPress.longPressToken = LONG_PRESS_TOKEN;
    content.tuneId = TUNE_TAP_NEXT;
  }
  else {
    bool tooLongToFit;
    bool forward = true;

    // if isn't the last page, display either a TAG_VALUE_DETAILS or a TAG_VALUE_LIST

    // if the page is not the last tag_value or when going forward, adapt indexes
    if ((page != (navInfo.nbPages - 2))||(page > navInfo.activePage)) {
      // we have to recompute the number of pairs to display in the page, and the index in which to start
      if (page < navInfo.activePage) { // backward
        forward = false;
      }
      else if (page > navInfo.activePage) { // forward
        staticReviewContext.currentPairIndex += staticReviewContext.nbPairsInCurrentPage;
      }
    }
    staticReviewContext.nbPairsInCurrentPage = getNbTagValuesInPage(forward?staticReviewContext.tagValueList->nbPairs-staticReviewContext.currentPairIndex:staticReviewContext.currentPairIndex,
                                                  &staticReviewContext.tagValueList->pairs[staticReviewContext.currentPairIndex],
                                                  forward,
                                                  &tooLongToFit);
    if (forward == false) {
      staticReviewContext.currentPairIndex -= staticReviewContext.nbPairsInCurrentPage;
    }
    // if the pair is too long to fit, we use a TAG_VALUE_DETAILS content
    if (tooLongToFit) {
      content.type = TAG_VALUE_DETAILS;
      content.tagValueDetails.detailsButtonText = "More";
      content.tagValueDetails.detailsButtonToken = DETAILS_BUTTON_TOKEN;
      content.tagValueDetails.tagValueList.nbMaxLinesForValue = NB_MAX_LINES_IN_REVIEW;
      content.tagValueDetails.tagValueList.nbPairs = 1;
      content.tagValueDetails.tagValueList.pairs = &staticReviewContext.tagValueList->pairs[staticReviewContext.currentPairIndex];
      content.tagValueDetails.tagValueList.smallCaseForValue = false;
    }
    else {
      content.type = TAG_VALUE_LIST;
      content.tagValueList.nbPairs = staticReviewContext.nbPairsInCurrentPage;
      content.tagValueList.pairs = &staticReviewContext.tagValueList->pairs[staticReviewContext.currentPairIndex];
      content.tagValueList.smallCaseForValue = false;
      content.tagValueList.nbMaxLinesForValue = NB_MAX_LINES_IN_REVIEW;
    }
    content.tuneId = TUNE_TAP_CASUAL;
  }
  navInfo.activePage = page;

  // common fields
  content.title = NULL;
  content.isTouchableTitle = false;

  navInfo.navWithTap.backButton = (navInfo.activePage==0)?false:true;

  if (navInfo.activePage == (navInfo.nbPages-1)) {
    navInfo.navWithTap.nextPageText = NULL;
  }
  else {
    navInfo.navWithTap.nextPageText = "Tap to continue";
  }

  pageContext = nbgl_pageDrawGenericContent(&pageCallback, &navInfo, &content);
  nbgl_refresh();
}

// function used to display the current page in details review mode
static void displayDetailsPage(uint8_t detailsPage) {
  static nbgl_layoutTagValue_t currentPair;
  nbgl_pageNavigationInfo_t info = {
    .activePage = detailsPage,
    .nbPages = detailsContext.nbPages,
    .navType = NAV_WITH_BUTTONS,
    .navWithButtons.navToken = NAV_TOKEN,
    .navWithButtons.quitButton = true,
    .progressIndicator = true,
    .tuneId = TUNE_TAP_CASUAL
  };
  nbgl_pageContent_t content = {
    .type = TAG_VALUE_LIST,
    .tagValueList.nbPairs = 1,
    .tagValueList.pairs = &currentPair,
    .tagValueList.smallCaseForValue = true
  };

  if (modalPageContext != NULL) {
    nbgl_pageRelease(modalPageContext);
  }
  currentPair.item = detailsContext.tag;
  // if move backward
  if (detailsPage == detailsContext.previousPage) {
    detailsContext.currentPageStart = detailsContext.previousPageStart;
    // if first page, reset some context variables
    if (detailsContext.previousPage == 0) {
      detailsContext.previousPage = -1;
      detailsContext.previousPageStart = NULL;
    }
  }
  // else if move backward
  else if (detailsPage>detailsContext.currentPage) {
    detailsContext.previousPage = detailsContext.currentPage;
    detailsContext.previousPageStart = detailsContext.currentPageStart;
    detailsContext.currentPageStart = detailsContext.nextPageStart;
  }
  else {
    // init case
    detailsContext.currentPageStart = detailsContext.value;
  }
  detailsContext.currentPage = detailsPage;
  currentPair.value = detailsContext.currentPageStart;
  uint16_t nbLines = nbgl_getTextNbLinesInWidth(BAGL_FONT_INTER_REGULAR_24px, currentPair.value, SCREEN_WIDTH-2*BORDER_MARGIN);
  if (nbLines>NB_MAX_LINES_IN_DETAILS) {
    uint16_t len;
    nbgl_getTextMaxLenInNbLines(BAGL_FONT_INTER_REGULAR_24px, currentPair.value,SCREEN_WIDTH-2*BORDER_MARGIN,NB_MAX_LINES_IN_DETAILS,&len);
    len-=3;
    detailsContext.nextPageStart = currentPair.value+len;
    // use special feature to keep only NB_MAX_LINES_IN_DETAILS lines and replace the last 3 chars by "..."
    content.tagValueList.nbMaxLinesForValue = NB_MAX_LINES_IN_DETAILS;
  }
  else {
    detailsContext.nextPageStart = NULL;
    content.tagValueList.nbMaxLinesForValue = 0;
  }
  modalPageContext = nbgl_pageDrawGenericContentExt(&pageModalCallback, &info, &content, true);
  nbgl_refresh();
}

// called when quit button is touched on Address verification page
static void addressLayoutTouchCallbackQR(int token, uint8_t index) {
  UNUSED(token);
  UNUSED(index);

  // dismiss modal
  nbgl_layoutRelease(addressConfirmationContext.modalLayout);
  nbgl_screenRedraw();
  nbgl_refresh();
}

// called when buttons (or footer) is touched on Address verification page
static void addressLayoutTouchCallback(int token, uint8_t index) {
  UNUSED(index);
  if (token == QUIT_TOKEN) {
    if (onChoice != NULL)
      onChoice(false);
  }
  else if (token == CONTINUE_TOKEN) {
    if (onChoice != NULL)
      onChoice(true);
  }
  else if (token == BUTTON_TOKEN) {
    // display the address as QR Code
    nbgl_layoutDescription_t layoutDescription = {
      .modal = true,
      .onActionCallback = &addressLayoutTouchCallbackQR,
      .tapActionText = NULL
    };

    addressConfirmationContext.modalLayout = nbgl_layoutGet(&layoutDescription);
    nbgl_layoutQRCode_t qrCode = {
      .url = (char*)addressConfirmationContext.address,
      .text1 = NULL,
      .text2 = (char*)addressConfirmationContext.address // display as gray text
    };
    nbgl_layoutAddQRCode(addressConfirmationContext.modalLayout, &qrCode);

    nbgl_layoutAddBottomButton(addressConfirmationContext.modalLayout, &C_cross32px, 0, true, TUNE_TAP_CASUAL);
    nbgl_layoutDraw(addressConfirmationContext.modalLayout);
    nbgl_refresh();
  }
}

static uint8_t getNbTagValuesInPage(uint8_t nbPairs, nbgl_layoutTagValue_t *pairs, bool forward, bool *tooLongToFit) {
  uint8_t nbPairsInPage = 0;
  uint16_t currentHeight = 24; // upper margin

  *tooLongToFit = false;
  while (nbPairsInPage < nbPairs) {
    if (nbPairsInPage>0)
      currentHeight += 12; // margin between pairs
    currentHeight += 32; // tag height
    currentHeight += 4; // space between tag and value
    currentHeight += nbgl_getTextHeightInWidth(BAGL_FONT_INTER_REGULAR_32px,
                                pairs[forward?nbPairsInPage:-nbPairsInPage-1].value,SCREEN_WIDTH-2*BORDER_MARGIN); // value height
    if (currentHeight >= TAG_VALUE_AREA_HEIGHT)
      break;
    nbPairsInPage++;
  }
  if ((nbPairsInPage == 0)&&(currentHeight>=TAG_VALUE_AREA_HEIGHT)) {
    *tooLongToFit = true;
    nbPairsInPage = 1;
  }
  return nbPairsInPage;
}

static uint8_t getNbPagesForTagValueList(nbgl_layoutTagValueList_t *tagValueList) {
  uint8_t nbPages = 0;
  uint8_t nbPairs = tagValueList->nbPairs;
  uint8_t nbPairsInPage;
  uint16_t currentHeight = 0;
  uint8_t i=0;
  bool tooLongToFit;

  while (i < tagValueList->nbPairs) {
    // upper margin
    currentHeight += 24;
    nbPairsInPage = getNbTagValuesInPage(nbPairs, &tagValueList->pairs[i],true, &tooLongToFit);
    i += nbPairsInPage;
    nbPairs -= nbPairsInPage;
    nbPages++;
  }
  return nbPages;
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * @brief draws the welcome page of an app (page on which we land when launching it from dashboard)
 *
 * @param appName app name
 * @param appIcon app icon
 * @param tagline text under app name (if NULL, it will be "This app confirms actions on the <appName> network.")
 * @param withSettings if true, use a "settings" (wheel) icon in bottom button, otherwise a "info" (i)
 * @param bottomCallback callback called when bottom button is pressed
 * @param quitCallback callback called when quit button is pressed
 */
void nbgl_useCaseHome(char *appName, const nbgl_icon_details_t *appIcon, char *tagline, bool withSettings,
                      nbgl_callback_t bottomCallback, nbgl_callback_t quitCallback) {
  nbgl_pageInfoDescription_t info = {
    .centeredInfo.icon = appIcon,
    .centeredInfo.text1 = appName,
    .centeredInfo.text3 = NULL,
    .centeredInfo.style = LARGE_CASE_INFO,
    .centeredInfo.offsetY = 32,
    .footerText = NULL,
    .bottomButtonStyle = withSettings? SETTINGS_ICON:INFO_ICON,
    .bottomButtonToken = CONTINUE_TOKEN,
    .tapActionText = NULL,
    .topRightStyle = QUIT_ICON,
    .topRightToken = QUIT_TOKEN,
    .tuneId = TUNE_TAP_CASUAL
  };
  if (tagline == NULL) {
    snprintf(appDescription, APP_DESCRIPTION_MAX_LEN, "This app confirms actions on the %s network.", appName);
    info.centeredInfo.text2 = appDescription;
  }
  else {
    info.centeredInfo.text2 = tagline;
  }

  onContinue = bottomCallback;
  onQuit = quitCallback;
  pageContext = nbgl_pageDrawInfo(&pageCallback, NULL, &info);
  nbgl_refresh();
}

/**
 * @brief Draws the settings pages of an app with as many pages as given
 *        For each page, the given navCallback will be called to get the content. Only 'type' and union has to be set in this content
 *
 * @param title string to set in touchable (or not) title
 * @param initPage page on which to start [0->(nbPages-1)]
 * @param nbPages number of pages
 * @param touchable if true, the title is used to quit and quitCallback is called
 * @param quitCallback callback called when quit button (or title) is pressed
 * @param navCallback callback called when navigation arrows are pressed
 * @param controlsCallback callback called when any controls in the settings (radios, switches) is called (the tokens must be >= @ref FIRST_USER_TOKEN)
 */
void nbgl_useCaseSettings(char *title, uint8_t initPage, uint8_t nbPages, bool touchable,
                          nbgl_callback_t quitCallback, nbgl_navCallback_t navCallback,
                          nbgl_layoutTouchCallback_t controlsCallback) {
  // memorize context
  onQuit = quitCallback;
  onNav = navCallback;
  onControls = controlsCallback;
  settingsTitle = title;
  touchableTitle = touchable;

  // fill navigation structure
  navInfo.activePage = initPage;
  navInfo.navType = NAV_WITH_BUTTONS;
  navInfo.navWithButtons.navToken = NAV_TOKEN;
  if (!touchableTitle) {
    navInfo.navWithButtons.quitButton = true;
    navInfo.quitToken = QUIT_TOKEN;
  }
  else {
    navInfo.navWithButtons.quitButton = false;
  }
  navInfo.nbPages = nbPages;
  navInfo.progressIndicator = false;
  navInfo.tuneId = TUNE_TAP_CASUAL;

  displaySettingsPage(navInfo.activePage);
}

/**
 * @brief Draws a transient (1s) status page, either of success or failure, with the given message
 *
 * @param message string to set in middle of page (Upper case for success)
 * @param isSuccess if true, message is drawn in a Ledger style (with corners)
 * @param quitCallback callback called when quit timer times out
 */
void nbgl_useCaseStatus(char *message, bool isSuccess, nbgl_callback_t quitCallback) {
  nbgl_screenTickerConfiguration_t ticker = {
    .tickerCallback = &tickerCallback,
    .tickerIntervale = 0, // not periodic
    .tickerValue = 3000 // 3 seconds
  };
  onQuit = quitCallback;
  if (isSuccess) {
#ifdef HAVE_PIEZO_SOUND
    // Play success tune
    io_seproxyhal_play_tune(TUNE_SUCCESS);
#endif // HAVE_PIEZO_SOUND

    pageContext = nbgl_pageDrawLedgerInfo(&pageCallback, &ticker,message);
  }
  else {
    nbgl_pageInfoDescription_t info = {
      .bottomButtonStyle = NO_BUTTON_STYLE,
      .footerText = NULL,
      .centeredInfo.icon = &C_round_cross_64px,
      .centeredInfo.offsetY = 0,
      .centeredInfo.onTop = false,
      .centeredInfo.style = LARGE_CASE_INFO,
      .centeredInfo.text1 = message,
      .centeredInfo.text2 = NULL,
      .centeredInfo.text3 = NULL,
      .tapActionText = NULL,
      .topRightStyle = NO_BUTTON_STYLE,
      .tuneId = TUNE_TAP_CASUAL
    };
    pageContext = nbgl_pageDrawInfo(&pageCallback, &ticker,&info);
  }
  nbgl_refresh();
}

/**
 * @brief Draws a page to confirm or not an action, described in a centered info (with info icon), thanks to a button and a footer
 *        at the bottom of the page. The given callback is called with true as argument if the button is touched, false if footer is touched
 *
 * @param message string to set in center of page (32px)
 * @param subMessage string to set under message (24px) (can be NULL)
 * @param confirmText string to set in button, to confirm
 * @param rejectText string to set in footer, to reject
 * @param callback callback called when button or footer is touched
 */
void nbgl_useCaseChoice(char *message, char *subMessage, char *confirmText, char *rejectText, nbgl_choiceCallback_t callback) {
  nbgl_pageConfirmationDescription_t info = {
    .cancelToken = CHOICE_TOKEN,
    .cancelText = rejectText,
    .centeredInfo.text1 = message,
    .centeredInfo.text2 = subMessage,
    .centeredInfo.text3 = NULL,
    .centeredInfo.style = LARGE_CASE_INFO,
    .centeredInfo.icon = &C_round_warning_64px,
    .centeredInfo.offsetY = -64,
    .confirmationText = confirmText,
    .confirmationToken = CHOICE_TOKEN,
    .tuneId = TUNE_TAP_CASUAL
  };
  onChoice = callback;
  pageContext = nbgl_pageDrawConfirmation(&pageCallback, &info);
  nbgl_refresh();
}

/**
 * @brief Draws a review start page, with a centered message, a "tap to continue" container and a "reject" footer
 *
 * @param icon icon to use in centered info
 * @param reviewTitle string to set in middle of page (in 32px font)
 * @param reviewSubTitle string to set under reviewTitle (in 24px font) (can be NULL)
 * @param rejectText string to set in footer, to reject review
 * @param continueCallback callback called when main panel is touched
 * @param rejectCallback callback called when footer is touched
 */
void nbgl_useCaseReviewStart(const nbgl_icon_details_t *icon, char *reviewTitle, char *reviewSubTitle, char *rejectText,
                             nbgl_callback_t continueCallback, nbgl_callback_t rejectCallback) {
  nbgl_pageInfoDescription_t info = {
    .centeredInfo.icon = icon,
    .centeredInfo.text1 = reviewTitle,
    .centeredInfo.text2 = reviewSubTitle,
    .centeredInfo.text3 = NULL,
    .centeredInfo.style = LARGE_CASE_INFO,
    .centeredInfo.offsetY = -32,
    .footerText = rejectText,
    .footerToken = QUIT_TOKEN,
    .tapActionText = "Tap to continue",
    .tapActionToken = CONTINUE_TOKEN,
    .topRightStyle = NO_BUTTON_STYLE,
    .tuneId = TUNE_TAP_CASUAL
  };
  onQuit = rejectCallback;
  onContinue = continueCallback;

#ifdef HAVE_PIEZO_SOUND
  // Play notification sound
  io_seproxyhal_play_tune(TUNE_LOOK_AT_ME);
#endif // HAVE_PIEZO_SOUND

  pageContext = nbgl_pageDrawInfo(&pageCallback, NULL, &info);
  nbgl_refresh();
}
/**
 * @brief Draws a flow of pages of a review. A back key is available on top-left of the screen, except in first page
 *        It is possible to go to next page thanks to "tap to continue".
 *        For each page, the given navCallback will be called to get the content. Only 'type' and union has to be set in this content
 *
 * @param initPage page on which to start [0->(nbPages-1)]
 * @param nbPages number of pages. If 0, no progress indicator is used
 * @param rejectText text to use in footer
 * @param navCallback callback called when navigation (back or "tap to continue") is touched
 * @param buttonCallback callback called when a potential button (details) in the content is touched
 * @param choiceCallback callback called when either long_press or footer is called (parm is true for long press)
 */
void nbgl_useCaseRegularReview(uint8_t initPage, uint8_t nbPages, char *rejectText, nbgl_layoutTouchCallback_t buttonCallback,
                               nbgl_navCallback_t navCallback, nbgl_choiceCallback_t choiceCallback) {
  // memorize context
  onChoice = choiceCallback;
  onNav = navCallback;
  onControls = buttonCallback;
  forwardNavOnly = false;

  // fill navigation structure
  navInfo.nbPages = nbPages;
  navInfo.navType = NAV_WITH_TAP;
  navInfo.quitToken = REJECT_TOKEN;
  navInfo.navWithTap.nextPageToken = NEXT_TOKEN;
  navInfo.navWithTap.quitText = rejectText;
  navInfo.navWithTap.backToken = BACK_TOKEN;
  navInfo.progressIndicator = true;
  navInfo.tuneId = TUNE_TAP_CASUAL;

  displayReviewPage(initPage);
}

/**
 * @brief Draws a flow of pages of a review, without back key.
 *        It is possible to go to next page thanks to "tap to continue".
 *        For each page, the given navCallback will be called to get the content. Only 'type' and union has to be set in this content
 *
 * @param rejectText text to use in footer
 * @param buttonCallback callback called when a potential button (details or long press) in the content is touched
 * @param navCallback callback called when navigation "tap to continue" is touched, to get the content of next page
 * @param choiceCallback callback called when either long_press or footer is called (parm is true for long press)
 */
void nbgl_useCaseForwardOnlyReview(char *rejectText, nbgl_layoutTouchCallback_t buttonCallback,
                                   nbgl_navCallback_t navCallback, nbgl_choiceCallback_t choiceCallback) {
  // memorize context
  onChoice = choiceCallback;
  onNav = navCallback;
  onControls = buttonCallback;
  forwardNavOnly = true;

  // fill navigation structure
  navInfo.nbPages = 0;
  navInfo.navType = NAV_WITH_TAP;
  navInfo.quitToken = QUIT_TOKEN;
  navInfo.navWithTap.nextPageToken = NEXT_TOKEN;
  navInfo.navWithTap.quitText = rejectText;
  navInfo.navWithTap.backToken = BACK_TOKEN;
  navInfo.progressIndicator = true;
  navInfo.tuneId = TUNE_TAP_CASUAL;

  displayReviewPage(0);
}

/**
 * @brief Draws a flow of pages of a review. A back key is available on top-left of the screen, except in first page
 *        It is possible to go to next page thanks to "tap to continue".
 * @note  All tag/value pairs are provided in the API and the number of pages is automatically computed, the last page
 *        being a long press one
 *
 * @param tagValueList list of tag/value pairs (must be persistent because no copy)
 * @param infoLongPress information to build the last page (must be persistent because no copy)
 * @param rejectText text to use in footer
 * @param callback callback called when transaction is accepted (param is true) or rejected (param is false)
 */
void nbgl_useCaseStaticReview(nbgl_layoutTagValueList_t *tagValueList, nbgl_pageInfoLongPress_t *infoLongPress,
                              char *rejectText, nbgl_choiceCallback_t callback) {
  // memorize context
  onChoice = callback;
  onNav = NULL;
  forwardNavOnly = false;

  staticReviewContext.tagValueList = tagValueList;
  staticReviewContext.infoLongPress = infoLongPress;
  staticReviewContext.currentPairIndex = 0;
  staticReviewContext.nbPairsInCurrentPage = 0;

  // compute number of pages & fill navigation structure
  navInfo.nbPages = getNbPagesForTagValueList(tagValueList)+1;
  navInfo.activePage = 0;
  navInfo.navType = NAV_WITH_TAP;
  navInfo.quitToken = REJECT_TOKEN;
  navInfo.navWithTap.nextPageToken = NEXT_TOKEN;
  navInfo.navWithTap.quitText = rejectText;
  navInfo.navWithTap.backToken = BACK_TOKEN;
  navInfo.progressIndicator = true;
  navInfo.tuneId = TUNE_TAP_CASUAL;

  displayStaticReviewPage(0);
}


/**
 * @brief Draws a flow of pages to view details on a given tag/value pair that doesn't fit in a single page
 *
 * @param tag tag name (in gray)
 * @param value full value string, that will be split in multiple pages
 */
void nbgl_useCaseViewDetails(char *tag, char *value) {
  uint16_t nbLines = nbgl_getTextNbLinesInWidth(BAGL_FONT_INTER_REGULAR_24px, value, SCREEN_WIDTH-2*BORDER_MARGIN);

  // initialize context
  detailsContext.tag = tag;
  detailsContext.value = value;
  detailsContext.nbPages = (nbLines-1)/NB_MAX_LINES_IN_DETAILS +1;
  detailsContext.currentPage = 0;
  detailsContext.previousPage = -1;
  // add some spare for room lost with "..." substitution
  if (detailsContext.nbPages > 1) {
    uint16_t nbLostChars = (detailsContext.nbPages - 1)*3;
    uint16_t nbLostLines = (nbLostChars+((SCREEN_WIDTH-2*BORDER_MARGIN)/16)-1)/((SCREEN_WIDTH-2*BORDER_MARGIN)/16); // 16 for average char width
    uint8_t nbLinesInLastPage = nbLines - ((detailsContext.nbPages - 1)*NB_MAX_LINES_IN_DETAILS);

    detailsContext.nbPages += nbLostLines/NB_MAX_LINES_IN_DETAILS;
    if ((nbLinesInLastPage + (nbLostLines%NB_MAX_LINES_IN_DETAILS))>NB_MAX_LINES_IN_DETAILS) {
      detailsContext.nbPages ++;
    }
  }

  displayDetailsPage(0);
}

/**
 * @brief draw an address confirmation page. This page contains a "page selector" to display either address in text or QRCode format,
 *        and at the bottom a button to confirm and a footer to cancel
 *
 * @param address address to confirm (NULL terminated string)
 * @param callback callback called when button or footer is touched (if true, button, if false footer)
  */
void nbgl_useCaseAddressConfirmation(char *address, nbgl_choiceCallback_t callback) {
  nbgl_layoutButton_t buttonInfo;
  nbgl_layoutDescription_t layoutDescription = {
    .modal = false,
    .onActionCallback = &addressLayoutTouchCallback,
    .tapActionText = NULL
  };

  // save context
  onChoice = callback;
  addressConfirmationContext.address = address;

  if (addressConfirmationContext.layout)
    nbgl_layoutRelease(addressConfirmationContext.layout);

  addressConfirmationContext.layout = nbgl_layoutGet(&layoutDescription);
  nbgl_layoutAddFooter(addressConfirmationContext.layout, "It doesn't match",
                       QUIT_TOKEN, TUNE_TAP_CASUAL);

  nbgl_layoutTagValue_t tagValuePair = {
    .item = "Address",
    .value = (char*)address
  };
  nbgl_layoutTagValueList_t tagValueList = {
    .nbPairs = 1,
    .pairs = &tagValuePair,
    .smallCaseForValue = false
  };
  nbgl_layoutAddTagValueList(addressConfirmationContext.layout,&tagValueList);
  buttonInfo.fittingContent = true;
  buttonInfo.icon = &C_QRcode32px;
  buttonInfo.style = WHITE_BACKGROUND;
  buttonInfo.text = "Show as QR";
  buttonInfo.token = BUTTON_TOKEN;
  buttonInfo.tuneId = TUNE_TAP_NEXT;
  buttonInfo.onBottom = false;
  nbgl_layoutAddButton(addressConfirmationContext.layout,&buttonInfo);
  buttonInfo.style = BLACK_BACKGROUND;
  buttonInfo.icon = NULL;
  buttonInfo.token = CONTINUE_TOKEN;
  buttonInfo.fittingContent = false;
  buttonInfo.tuneId = TUNE_TAP_CASUAL;
  buttonInfo.onBottom = true;
  buttonInfo.text = "It matches";
  nbgl_layoutAddButton(addressConfirmationContext.layout, &buttonInfo);
  nbgl_layoutDraw(addressConfirmationContext.layout);
  nbgl_refresh();
}

/**
 * @brief draw a spinner page with the given parameters. The spinner will "turn" automatically every 800 ms
 *
 * @param text text to use under spinner
 */
void nbgl_useCaseSpinner(char* text) {
  pageContext = nbgl_pageDrawSpinner(NULL, (const char*)text);
  nbgl_refresh();
}
