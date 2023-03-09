/**
 * @file nbgl_use_case.c
 * @brief Implementation of typical pages (or sets of pages) for Applications
 */

#ifdef NBGL_USE_CASE
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
#define APP_DESCRIPTION_MAX_LEN 64
// maximum number of lines for value field in details pages
#define NB_MAX_LINES_IN_DETAILS  12
// maximum number of lines for value field in review pages
#define NB_MAX_LINES_IN_REVIEW    9

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
  SKIP_TOKEN,
  CONTINUE_TOKEN,
  BUTTON_TOKEN,
  ACTION_BUTTON_TOKEN,
  CHOICE_TOKEN,
  DETAILS_BUTTON_TOKEN,
  CONFIRM_TOKEN,
  REJECT_TOKEN,
  ADDR_BACK_TOKEN,
  ADDR_NEXT_TOKEN
};

typedef struct DetailsContext_s {
  uint8_t nbPages;
  uint8_t currentPage;
  bool wrapping;
  char *tag;
  char *value;
  char *nextPageStart;
} DetailsContext_t;

typedef struct StaticReviewContext_s {
  nbgl_layoutTagValueList_t tagValueList;
  bool withLongPress;
  nbgl_pageInfoLongPress_t infoLongPress;
  uint8_t currentPairIndex;
  uint8_t nbPairsInCurrentPage;
} StaticReviewContext_t;

typedef struct AddressConfirmationContext_s {
  char *address;
  nbgl_layout_t modalLayout;
  nbgl_layoutTagValueList_t *tagValueList;
} AddressConfirmationContext_t;


/**********************
 *  STATIC VARIABLES
 **********************/
// char buffers to build some strings
static char appDescription[APP_DESCRIPTION_MAX_LEN];
static char plugInDescription[APP_DESCRIPTION_MAX_LEN];

// multi-purposes callbacks
static nbgl_callback_t onQuit;
static nbgl_callback_t onContinue;
static nbgl_callback_t onAction;
static nbgl_navCallback_t onNav;
static nbgl_layoutTouchCallback_t onControls;
static nbgl_choiceCallback_t onChoice;
static nbgl_callback_t onModalConfirm;

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

// buffer of bits to store all numbers of tag/value pairs per page in static review
// this number is from 1 to 4, so we can use 2 bits per page
// there are up to 256 pages, so a buffer of 256/4 bytes is enough
static uint8_t nbPairsPerPage[64];
// same for too long to fit but with only one bit per page
static uint8_t tooLongToFitPerPage[32];

/**********************
 *  STATIC FUNCTIONS
 **********************/
static void displayReviewPage(uint8_t page, bool forceFullRefresh);
static void displayDetailsPage(uint8_t page, bool forceFullRefresh);
static void displaySettingsPage(uint8_t page, bool forceFullRefresh);
static void displayStaticReviewPage(uint8_t page, bool forceFullRefresh);
static void pageCallback(int token, uint8_t index);
#ifdef NBGL_QRCODE
static void addressLayoutTouchCallbackQR(int token, uint8_t index);
#endif // NBGL_QRCODE
static void displayAddressPage(uint8_t page, bool forceFullRefresh);
static void displaySkipWarning(void);
static uint8_t getNbPairs(uint8_t page, bool *tooLongToFit);

// function called when navigating (or exiting) modal details pages
// or when skip choice is displayed
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
      displayDetailsPage(index, false);
    }
  }
  else if (token == SKIP_TOKEN) {
    if (index == 0) {
      // display the last forward only review page, whatever it is
      displayReviewPage(LAST_PAGE_FOR_REVIEW, true);
    }
    else {
      // display background, which should be the page where skip has been touched
      nbgl_screenRedraw();
      nbgl_refresh();
    }
  }
  else if (token == CHOICE_TOKEN) {
    if (index == 0) {
      onModalConfirm();
    }
    else {
      // display background, which should be the page where skip has been touched
      nbgl_screenRedraw();
      nbgl_refresh();
    }
  }
}

// generic callback for all pages except modal
static void pageCallback(int token, uint8_t index) {
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
  else if (token == ACTION_BUTTON_TOKEN) {
    if ((index == 0) && (onAction != NULL)) {
      onAction();
    }
    else if ((index == 1) && (onQuit != NULL)) {
      onQuit();
    }
  }
  else if (token == BUTTON_TOKEN) {
#ifdef NBGL_QRCODE
    // display the address as QR Code
    nbgl_layoutDescription_t layoutDescription = {
      .modal = true,
      .withLeftBorder = true,
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
#endif // NBGL_QRCODE
  }
  else if (token == CONFIRM_TOKEN) {
    if (onChoice != NULL)
      onChoice(true);
  }
  else if (token == REJECT_TOKEN) {
    if (onChoice != NULL)
      onChoice(false);
  }
  else if (token == DETAILS_BUTTON_TOKEN) {
    nbgl_layoutTagValue_t *pair;
    if (staticReviewContext.tagValueList.pairs != NULL) {
      pair = &staticReviewContext.tagValueList.pairs[staticReviewContext.currentPairIndex];
    }
    else {
      pair = staticReviewContext.tagValueList.callback(staticReviewContext.currentPairIndex);
    }
    nbgl_useCaseViewDetails(pair->item,
                            pair->value,
                            staticReviewContext.tagValueList.wrapping);
  }
  else if (token == NAV_TOKEN) {
    if (index == EXIT_PAGE) {
      if (onQuit != NULL)
        onQuit();
    }
    else {
      displaySettingsPage(index, false);
    }
  }
  else if (token == NEXT_TOKEN) {
    if (onNav != NULL)
      displayReviewPage(navInfo.activePage+1, false);
    else
      displayStaticReviewPage(navInfo.activePage+1, false);
  }
  else if (token == BACK_TOKEN) {
    if (onNav != NULL)
      displayReviewPage(navInfo.activePage-1, true);
    else
      displayStaticReviewPage(navInfo.activePage-1, true);
  }
  else if (token == SKIP_TOKEN) {
    // display a modal warning to confirm skip
    displaySkipWarning();
  }
  else if (token == ADDR_BACK_TOKEN) {
    displayAddressPage(navInfo.activePage-1, true);
  }
  else if (token == ADDR_NEXT_TOKEN) {
    displayAddressPage(navInfo.activePage+1, false);
  }
  else { // probably a control provided by caller
    if (onControls != NULL) {
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
static void displaySettingsPage(uint8_t page, bool forceFullRefresh) {
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

  if (forceFullRefresh) {
    nbgl_refreshSpecial(FULL_COLOR_CLEAN_REFRESH);
  } else {
    nbgl_refreshSpecial(FULL_COLOR_PARTIAL_REFRESH);
  }
}

// function used to display the current page in review
static void displayReviewPage(uint8_t page, bool forceFullRefresh) {
  nbgl_pageContent_t content;

  // ensure the page is valid
  if ((navInfo.nbPages != 0) && (page >= (navInfo.nbPages))) {
    return;
  }
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
    content.infoLongPress.longPressToken = CONFIRM_TOKEN;
    if (forwardNavOnly) {
      // remove the "Skip" button in Footer
      navInfo.navWithTap.skipText = NULL;
    }
  }
  else {
    navInfo.navWithTap.nextPageText = "Tap to continue";
  }

  // override smallCaseForValue for tag/value types to false
  if (content.type == TAG_VALUE_DETAILS) {
    content.tagValueDetails.tagValueList.smallCaseForValue = false;
    // the maximum displayable number of lines for value is NB_MAX_LINES_IN_REVIEW (without More button)
    content.tagValueDetails.tagValueList.nbMaxLinesForValue = NB_MAX_LINES_IN_REVIEW;
  }
  else if (content.type == TAG_VALUE_LIST) {
    content.tagValueList.smallCaseForValue = false;
  }
  else if (content.type == TAG_VALUE_CONFIRM) {
    content.tagValueConfirm.tagValueList.smallCaseForValue = false;
    // no next because confirmation is always the last page
    navInfo.navWithTap.nextPageText = NULL;
    // use confirm token for black button
    content.tagValueConfirm.confirmationToken = CONFIRM_TOKEN;
  }

  pageContext = nbgl_pageDrawGenericContent(&pageCallback, &navInfo, &content);

  if (forceFullRefresh) {
    nbgl_refreshSpecial(FULL_COLOR_CLEAN_REFRESH);
  } else {
    nbgl_refreshSpecial(FULL_COLOR_PARTIAL_REFRESH);
  }
}

// function used to display the current page in static review (no call to user to get content)
static void displayStaticReviewPage(uint8_t page, bool forceFullRefresh) {
  nbgl_pageContent_t content;

  // if it's the last page, display a long press button (or a simple button) with the info provided by user (except token)
  if (page == (navInfo.nbPages - 1)) {
    if (staticReviewContext.withLongPress) {
      content.type = INFO_LONG_PRESS;
      content.infoLongPress.icon = staticReviewContext.infoLongPress.icon;
      content.infoLongPress.longPressText = staticReviewContext.infoLongPress.longPressText;
      content.infoLongPress.text = staticReviewContext.infoLongPress.text;
      content.infoLongPress.longPressToken = CONFIRM_TOKEN;
      content.tuneId = TUNE_TAP_CASUAL;
    }
    else {
      // simple button
      content.type = INFO_BUTTON;
      content.infoButton.icon = staticReviewContext.infoLongPress.icon;
      content.infoButton.buttonText = staticReviewContext.infoLongPress.longPressText;
      content.infoButton.text = staticReviewContext.infoLongPress.text;
      content.infoButton.buttonToken = CONFIRM_TOKEN;
      content.tuneId = TUNE_TAP_CASUAL;
    }
  }
  else {
    bool tooLongToFit;

    if (page > navInfo.activePage) { // forward, so add to current index the number of pairs of the previous page
      staticReviewContext.currentPairIndex += staticReviewContext.nbPairsInCurrentPage;
    }
    staticReviewContext.nbPairsInCurrentPage = getNbPairs(page,&tooLongToFit);
    // if the page is not the last tag/value or when going forward, adapt index
    if ((page != (navInfo.nbPages - 2)) && (page < navInfo.activePage)) { // backward
       staticReviewContext.currentPairIndex -= staticReviewContext.nbPairsInCurrentPage;
    }
    // if the pair is too long to fit, we use a TAG_VALUE_DETAILS content
    if (tooLongToFit) {
      content.type = TAG_VALUE_DETAILS;
      content.tagValueDetails.detailsButtonText = "More";
      content.tagValueDetails.detailsButtonIcon = NULL;
      content.tagValueDetails.detailsButtonToken = DETAILS_BUTTON_TOKEN;
      content.tagValueDetails.tagValueList.nbMaxLinesForValue = NB_MAX_LINES_IN_REVIEW;
      content.tagValueDetails.tagValueList.nbPairs = 1;
      if (staticReviewContext.tagValueList.pairs != NULL) {
        content.tagValueDetails.tagValueList.pairs = &staticReviewContext.tagValueList.pairs[staticReviewContext.currentPairIndex];
      }
      else {
        content.tagValueDetails.tagValueList.pairs = staticReviewContext.tagValueList.callback(staticReviewContext.currentPairIndex);
      }
      content.tagValueDetails.tagValueList.smallCaseForValue = false;
      content.tagValueDetails.tagValueList.wrapping = staticReviewContext.tagValueList.wrapping;
    }
    else {
      content.type = TAG_VALUE_LIST;
      content.tagValueList.nbPairs = staticReviewContext.nbPairsInCurrentPage;
      if (staticReviewContext.tagValueList.pairs != NULL) {
        content.tagValueList.pairs = &staticReviewContext.tagValueList.pairs[staticReviewContext.currentPairIndex];
      }
      else {
        content.tagValueList.pairs = NULL;
        content.tagValueList.callback = staticReviewContext.tagValueList.callback;
        content.tagValueList.startIndex = staticReviewContext.currentPairIndex;
      }
      content.tagValueList.smallCaseForValue = false;
      content.tagValueList.nbMaxLinesForValue = NB_MAX_LINES_IN_REVIEW;
      content.tagValueList.wrapping = staticReviewContext.tagValueList.wrapping;
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

  if (forceFullRefresh) {
    nbgl_refreshSpecial(FULL_COLOR_CLEAN_REFRESH);
  } else {
    nbgl_refreshSpecial(FULL_COLOR_PARTIAL_REFRESH);
  }
}

// from the current details context, return a pointer on the details at the given page
static char *getDetailsPageAt(uint8_t detailsPage) {
  uint8_t page = 0;
  char *currentChar = detailsContext.value;
  while (page<detailsPage) {
    uint16_t nbLines = nbgl_getTextNbLinesInWidth(BAGL_FONT_INTER_REGULAR_24px, currentChar, SCREEN_WIDTH-2*BORDER_MARGIN, false);
    if (nbLines>NB_MAX_LINES_IN_DETAILS) {
      uint16_t len;
      nbgl_getTextMaxLenInNbLines(BAGL_FONT_INTER_REGULAR_24px, currentChar,SCREEN_WIDTH-2*BORDER_MARGIN,NB_MAX_LINES_IN_DETAILS,&len);
      len-=3;
      currentChar = currentChar+len;
    }
    page++;
  }
  return currentChar;
}

// function used to display the current page in details review mode
static void displayDetailsPage(uint8_t detailsPage, bool forceFullRefresh) {
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
    .tagValueList.smallCaseForValue = true,
    .tagValueList.wrapping = detailsContext.wrapping
  };

  if (modalPageContext != NULL) {
    nbgl_pageRelease(modalPageContext);
  }
  currentPair.item = detailsContext.tag;
  // if move backward or first page
  if (detailsPage <= detailsContext.currentPage) {
    // recompute current start from beginning
    currentPair.value = getDetailsPageAt(detailsPage);
    forceFullRefresh = true;
  }
  // else move forward
  else {
    currentPair.value = detailsContext.nextPageStart;
  }
  detailsContext.currentPage = detailsPage;
  uint16_t nbLines = nbgl_getTextNbLinesInWidth(BAGL_FONT_INTER_REGULAR_24px, currentPair.value, SCREEN_WIDTH-2*BORDER_MARGIN, false);

  if (nbLines>NB_MAX_LINES_IN_DETAILS) {
    uint16_t len;
    nbgl_getTextMaxLenInNbLines(BAGL_FONT_INTER_REGULAR_24px, currentPair.value,SCREEN_WIDTH-2*BORDER_MARGIN,NB_MAX_LINES_IN_DETAILS,&len);
    len-=3;
    // memorize next position to save processing
    detailsContext.nextPageStart = currentPair.value+len;
    // use special feature to keep only NB_MAX_LINES_IN_DETAILS lines and replace the last 3 chars by "..."
    content.tagValueList.nbMaxLinesForValue = NB_MAX_LINES_IN_DETAILS;
  }
  else {
    detailsContext.nextPageStart = NULL;
    content.tagValueList.nbMaxLinesForValue = 0;
  }
  modalPageContext = nbgl_pageDrawGenericContentExt(&pageModalCallback, &info, &content, true);

  if (forceFullRefresh) {
    nbgl_refreshSpecial(FULL_COLOR_CLEAN_REFRESH);
  } else {
    nbgl_refreshSpecial(FULL_COLOR_PARTIAL_REFRESH);
  }
}

#ifdef NBGL_QRCODE
// called when quit button is touched on Address verification page
static void addressLayoutTouchCallbackQR(int token, uint8_t index) {
  UNUSED(token);
  UNUSED(index);

  // dismiss modal
  nbgl_layoutRelease(addressConfirmationContext.modalLayout);
  nbgl_screenRedraw();
  nbgl_refresh();
}
#endif // NBGL_QRCODE

// called when navigation is touched on Address verification page
static void displayAddressPage(uint8_t page, bool forceFullRefresh) {
  nbgl_pageContent_t content;
  nbgl_layoutTagValue_t tagValuePair = {
    .item = "Address",
    .value = (char*)addressConfirmationContext.address
  };

  content.type = TAG_VALUE_CONFIRM;
  content.title = NULL;
  content.isTouchableTitle = false;
  if (page == 0) {
#ifdef NBGL_QRCODE
    content.tagValueConfirm.detailsButtonIcon = &C_QRcode32px;
    content.tagValueConfirm.detailsButtonText = "Show as QR";
    content.tagValueConfirm.detailsButtonToken = BUTTON_TOKEN;
#else // NBGL_QRCODE
    content.tagValueConfirm.detailsButtonText = NULL;
    content.tagValueConfirm.detailsButtonIcon = NULL;
#endif // NBGL_QRCODE
    content.tagValueConfirm.tuneId = TUNE_TAP_CASUAL;
    content.tagValueConfirm.tagValueList.nbPairs = 1;
    content.tagValueConfirm.tagValueList.pairs = &tagValuePair;
    content.tagValueConfirm.tagValueList.smallCaseForValue = false;
    content.tagValueConfirm.tagValueList.nbMaxLinesForValue = 0;
    content.tagValueConfirm.tagValueList.wrapping = false;
    // if it's an extended address verif, it takes 2 pages, so display a "Tap to continue", and no confirmation button
    if (navInfo.nbPages > 1) {
      navInfo.navWithTap.nextPageText = "Tap to continue";
      content.tagValueConfirm.confirmationText = NULL;
    }
    else {
      // otherwise no tap to continue but a confirmation button
      content.tagValueConfirm.confirmationText = "Confirm";
      content.tagValueConfirm.confirmationToken = CONFIRM_TOKEN;
    }
  }
  else if (page == 1) {
    // the second page is dedicated to the extended tag/value pairs
    content.type = TAG_VALUE_CONFIRM;
    content.tagValueConfirm.confirmationText = "Confirm";
    content.tagValueConfirm.confirmationToken = CONFIRM_TOKEN;
    content.tagValueConfirm.detailsButtonText = NULL;
    content.tagValueConfirm.detailsButtonIcon = NULL;
    content.tagValueConfirm.tuneId = TUNE_TAP_CASUAL;
    content.tagValueConfirm.tagValueList.nbPairs = addressConfirmationContext.tagValueList->nbPairs;
    content.tagValueConfirm.tagValueList.pairs = addressConfirmationContext.tagValueList->pairs;
    content.tagValueConfirm.tagValueList.smallCaseForValue = addressConfirmationContext.tagValueList->smallCaseForValue;
    content.tagValueConfirm.tagValueList.nbMaxLinesForValue = addressConfirmationContext.tagValueList->nbMaxLinesForValue;
    content.tagValueConfirm.tagValueList.wrapping = addressConfirmationContext.tagValueList->wrapping;

    // no next page
    navInfo.navWithTap.nextPageText = NULL;
  }
  // fill navigation structure
  navInfo.activePage = page;

  nbgl_pageDrawGenericContent(&pageCallback, &navInfo, &content);

  if (forceFullRefresh) {
    nbgl_refreshSpecial(FULL_COLOR_CLEAN_REFRESH);
  } else {
    nbgl_refreshSpecial(FULL_COLOR_PARTIAL_REFRESH);
  }
}

// called when skip button is touched in footer, during forward only review
static void displaySkipWarning(void) {
  nbgl_pageConfirmationDescription_t info = {
    .cancelText = "Go back to review",
    .centeredInfo.text1 = "Skip message review?",
    .centeredInfo.text2 = "You can skip directly to signing\nif you're sure you don't need to\nreview all the fields.",
    .centeredInfo.text3 = NULL,
    .centeredInfo.style = LARGE_CASE_INFO,
    .centeredInfo.icon = &C_warning64px,
    .centeredInfo.offsetY = -64,
    .confirmationText = "Yes, skip",
    .confirmationToken = SKIP_TOKEN,
    .tuneId = TUNE_TAP_CASUAL,
    .modal = true
  };
  modalPageContext = nbgl_pageDrawConfirmation(&pageModalCallback, &info);
  nbgl_refreshSpecial(FULL_COLOR_PARTIAL_REFRESH);
}

///////////// STATIC REVIEW UTILITIES /////////////

// sets the number of pairs fitting in the given page, with tooLongToFit param
static void setNbPairs(uint8_t page, uint8_t nbPairs, bool tooLongToFit) {
  nbPairsPerPage[page/4] &= ~(3<<((page%4)*2));
  nbPairsPerPage[page/4] |= ((nbPairs-1)<<((page%4)*2));
  if (tooLongToFit)
    tooLongToFitPerPage[page/8] |= 1<<(page%8);
  else
    tooLongToFitPerPage[page/8] &= ~(1<<(page%8));
}

// returns the number of pairs fitting in the given page, setting tooLongToFit at the appropriate value
static uint8_t getNbPairs(uint8_t page, bool *tooLongToFit) {
  uint8_t nbPairs = (nbPairsPerPage[page/4] >> ((page%4)*2))&3;
  *tooLongToFit = (tooLongToFitPerPage[page/8] & (1<<(page%8)))?true:false;
  return nbPairs+1;
}

// computes and returns the number of tag/values pairs displayable in a page, with the given list of tag/value pairs
static uint8_t getNbTagValuesInPage(uint8_t nbPairs, nbgl_layoutTagValueList_t *tagValueList, uint8_t startIndex, bool *tooLongToFit) {
  uint8_t nbPairsInPage = 0;
  uint16_t currentHeight = 12; // upper margin

  *tooLongToFit = false;
  while (nbPairsInPage < nbPairs) {
    char *value;

    if (nbPairsInPage>0)
      currentHeight += 12; // margin between pairs
    currentHeight += 32; // tag height
    currentHeight += 4; // space between tag and value

    if (tagValueList->pairs != NULL) {
      value = tagValueList->pairs[startIndex+nbPairsInPage].value;
    }
    else {
      value = tagValueList->callback(startIndex+nbPairsInPage)->value;
    }
    currentHeight += nbgl_getTextHeightInWidth(BAGL_FONT_INTER_REGULAR_32px,
                                value,SCREEN_WIDTH-2*BORDER_MARGIN, tagValueList->wrapping); // value height
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

// computes and returns the number of pages necessary to display the given list of tag/value pairs
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
    nbPairsInPage = getNbTagValuesInPage(nbPairs, tagValueList, i, &tooLongToFit);
    i += nbPairsInPage;
    setNbPairs(nbPages,nbPairsInPage,tooLongToFit);
    nbPairs -= nbPairsInPage;
    nbPages++;
  }
  return nbPages;
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * @brief draws the home page of an app (page on which we land when launching it from dashboard)
 *
 * @param appName app name
 * @param appIcon app icon
 * @param tagline text under app name (if NULL, it will be "This app confirms actions on the <appName> network.")
 * @param withSettings if true, use a "settings" (wheel) icon in bottom button, otherwise a "info" (i)
 * @param topRightCallback callback called when top-right button is touched
 * @param quitCallback callback called when quit button is touched
 */
void nbgl_useCaseHome(char *appName, const nbgl_icon_details_t *appIcon, char *tagline, bool withSettings,
                      nbgl_callback_t topRightCallback, nbgl_callback_t quitCallback) {
  nbgl_useCaseHomeExt(appName, appIcon, tagline, withSettings, NULL, NULL,
                      topRightCallback, quitCallback);
}

/**
 * @brief draws the extended version of home page of an app (page on which we land when launching it from dashboard)
 * @note it enables to use an action button (black)
 *
 * @param appName app name
 * @param appIcon app icon
 * @param tagline text under app name (if NULL, it will be "This app confirms actions on the <appName> network.")
 * @param withSettings if true, use a "settings" (wheel) icon in bottom button, otherwise a "info" (i)
 * @param actionButtonText if not NULL, text used for an action button (in black, on top of "Quit App" button)
 * @param actionCallback callback called when action button is touched (if actionButtonText is not NULL)
 * @param topRightCallback callback called when top-right button is touched
 * @param quitCallback callback called when quit button is touched
 */
void nbgl_useCaseHomeExt(char *appName, const nbgl_icon_details_t *appIcon, char *tagline, bool withSettings,
                         char *actionButtonText, nbgl_callback_t actionCallback,
                         nbgl_callback_t topRightCallback, nbgl_callback_t quitCallback) {
  nbgl_pageInfoDescription_t info = {
    .centeredInfo.icon = appIcon,
    .centeredInfo.text1 = appName,
    .centeredInfo.text3 = NULL,
    .centeredInfo.style = LARGE_CASE_INFO,
    .centeredInfo.offsetY = -16,
    .footerText = NULL,
    .bottomButtonStyle = QUIT_APP_TEXT,
    .tapActionText = NULL,
    .topRightStyle = withSettings? SETTINGS_ICON:INFO_ICON,
    .topRightToken = CONTINUE_TOKEN,
    .actionButtonText = actionButtonText,
    .tuneId = TUNE_TAP_CASUAL
  };
  if (actionButtonText != NULL) {
    // trick to use ACTION_BUTTON_TOKEN for action and quit, with index used to distinguish
    info.bottomButtonsToken = ACTION_BUTTON_TOKEN;
    onAction = actionCallback;
  }
  else {
    info.bottomButtonsToken = QUIT_TOKEN;
    onAction = actionCallback;
  }
  if (tagline == NULL) {
    snprintf(appDescription, APP_DESCRIPTION_MAX_LEN, "This app confirms actions on\nthe %s network.", appName);
    info.centeredInfo.text2 = appDescription;
  }
  else {
    info.centeredInfo.text2 = tagline;
  }

  onContinue = topRightCallback;
  onQuit = quitCallback;
  if (actionButtonText != NULL) {
    info.centeredInfo.offsetY -= 40;
  }
  pageContext = nbgl_pageDrawInfo(&pageCallback, NULL, &info);
  nbgl_refreshSpecial(FULL_COLOR_CLEAN_REFRESH);
}

/**
 * @brief draws the home page of a plug-in app (page on which we land when launching it from dashboard)
 *
 * @param plugInName plug-in app name
 * @param appName master app name (app used by plug-in)
 * @param appIcon master app icon
 * @param tagline text under plug-in name (if NULL, it will be "This app confirms actions for\n<plugInName>.")
 * @param subTagline text under master app icon (if NULL, it will be "This app relies on\n<appName>")
 * @param withSettings if true, use a "settings" (wheel) icon in bottom button, otherwise a "info" (i)
 * @param topRightCallback callback called when top-right button is touched
 * @param quitCallback callback called when quit button is touched
 */
void nbgl_useCasePlugInHome(char *plugInName, char *appName,
                            const nbgl_icon_details_t *appIcon, char *tagline,
                            char *subTagline, bool withSettings,
                            nbgl_callback_t topRightCallback, nbgl_callback_t quitCallback) {
  nbgl_pageInfoDescription_t info = {
    .centeredInfo.icon = appIcon,
    .centeredInfo.text1 = plugInName,
    .centeredInfo.style = PLUGIN_INFO,
    .centeredInfo.offsetY = -16,
    .footerText = NULL,
    .bottomButtonStyle = QUIT_APP_TEXT,
    .tapActionText = NULL,
    .topRightStyle = withSettings? SETTINGS_ICON:INFO_ICON,
    .topRightToken = CONTINUE_TOKEN,
    .actionButtonText = NULL,
    .tuneId = TUNE_TAP_CASUAL
  };
  info.bottomButtonsToken = QUIT_TOKEN;
  onAction = NULL;
  if (tagline == NULL) {
    snprintf(appDescription, APP_DESCRIPTION_MAX_LEN, "This app confirms actions for\n%s.", plugInName);
    info.centeredInfo.text2 = appDescription;
  }
  else {
    info.centeredInfo.text2 = tagline;
  }
  if (subTagline == NULL) {
    snprintf(plugInDescription, APP_DESCRIPTION_MAX_LEN, "This app relies on\n%s", appName);
    info.centeredInfo.text3 = plugInDescription;
  }
  else {
    info.centeredInfo.text3 = subTagline;
  }

  onContinue = topRightCallback;
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

  displaySettingsPage(navInfo.activePage, true);
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
    io_seproxyhal_play_tune(TUNE_LEDGER_MOMENT);
#endif // HAVE_PIEZO_SOUND

    pageContext = nbgl_pageDrawLedgerInfo(&pageCallback, &ticker, message, QUIT_TOKEN);
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
      .tapActionText = "",
      .tapActionToken = QUIT_TOKEN,
      .topRightStyle = NO_BUTTON_STYLE,
      .actionButtonText = NULL,
      .tuneId = TUNE_TAP_CASUAL
    };
    pageContext = nbgl_pageDrawInfo(&pageCallback, &ticker,&info);
  }
  nbgl_refreshSpecial(FULL_COLOR_PARTIAL_REFRESH);
}

/**
 * @brief Draws a generic choice page, described in a centered info (with configurable icon), thanks to a button and a footer
 *        at the bottom of the page. The given callback is called with true as argument if the button is touched, false if footer is touched
 *
 * @param icon icon to set in center of page
 * @param message string to set in center of page (32px)
 * @param subMessage string to set under message (24px) (can be NULL)
 * @param confirmText string to set in button, to confirm
 * @param cancelText string to set in footer, to reject
 * @param callback callback called when button or footer is touched
 */
void nbgl_useCaseChoice(const nbgl_icon_details_t *icon, char *message, char *subMessage, char *confirmText, char *cancelText, nbgl_choiceCallback_t callback) {
  nbgl_pageConfirmationDescription_t info = {
    .cancelText = cancelText,
    .centeredInfo.text1 = message,
    .centeredInfo.text2 = subMessage,
    .centeredInfo.text3 = NULL,
    .centeredInfo.style = LARGE_CASE_INFO,
    .centeredInfo.icon = icon,
    .centeredInfo.offsetY = -40,
    .confirmationText = confirmText,
    .confirmationToken = CHOICE_TOKEN,
    .tuneId = TUNE_TAP_CASUAL,
    .modal = false
  };
  onChoice = callback;
  pageContext = nbgl_pageDrawConfirmation(&pageCallback, &info);
  nbgl_refreshSpecial(FULL_COLOR_PARTIAL_REFRESH);
}

/**
 * @brief Draws a page to confirm or not an action, described in a centered info (with info icon), thanks to a button and a footer
 *        at the bottom of the page. The given callback is called if the button is touched. If the footer is touched, the page is only "dismissed"
 * @note This page is displayed as a modal (so the content of the previous page will be visible when dismissed).
 *
 * @param message string to set in center of page (32px)
 * @param subMessage string to set under message (24px) (can be NULL)
 * @param confirmText string to set in button, to confirm
 * @param cancelText string to set in footer, to reject
 * @param callback callback called when confirmation button is touched
 */
void nbgl_useCaseConfirm(char *message, char *subMessage, char *confirmText, char *cancelText, nbgl_callback_t callback) {
  nbgl_pageConfirmationDescription_t info = {
    .cancelText = cancelText,
    .centeredInfo.text1 = message,
    .centeredInfo.text2 = subMessage,
    .centeredInfo.text3 = NULL,
    .centeredInfo.style = LARGE_CASE_INFO,
    .centeredInfo.icon = &C_round_warning_64px,
    .centeredInfo.offsetY = -40,
    .confirmationText = confirmText,
    .confirmationToken = CHOICE_TOKEN,
    .tuneId = TUNE_TAP_CASUAL,
    .modal = true
  };
  onModalConfirm = callback;
  modalPageContext = nbgl_pageDrawConfirmation(&pageModalCallback, &info);
  nbgl_refreshSpecial(FULL_COLOR_PARTIAL_REFRESH);
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
    .centeredInfo.offsetY = 0,
    .footerText = rejectText,
    .footerToken = QUIT_TOKEN,
    .tapActionText = "Tap to continue",
    .tapActionToken = CONTINUE_TOKEN,
    .topRightStyle = NO_BUTTON_STYLE,
    .actionButtonText = NULL,
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
 * @param choiceCallback callback called when either long_press or footer is called (param is true for long press)
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
  navInfo.navWithTap.skipText = NULL;
  navInfo.progressIndicator = true;
  navInfo.tuneId = TUNE_TAP_CASUAL;

  displayReviewPage(initPage, true);
}

/**
 * @brief Draws a flow of pages of a review, without back key.
 *        It is possible to go to next page thanks to "tap to continue".
 *        For each page, the given navCallback will be called to get the content. Only 'type' and union has to be set in this content
 *
 * @param rejectText text to use in footer
 * @param buttonCallback callback called when a potential button (details or long press) in the content is touched
 * @param navCallback callback called when navigation "tap to continue" is touched, to get the content of next page
 * @param choiceCallback callback called when either long_press or footer is called (param is true for long press)
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
  navInfo.navWithTap.skipText = "Skip >>";
  navInfo.navWithTap.skipToken = SKIP_TOKEN;
  navInfo.progressIndicator = true;
  navInfo.tuneId = TUNE_TAP_CASUAL;

  displayReviewPage(0, false);
}

/**
 * @brief Draws a flow of pages of a review. A back key is available on top-left of the screen, except in first page
 *        It is possible to go to next page thanks to "tap to continue".
 * @note  All tag/value pairs are provided in the API and the number of pages is automatically computed, the last page
 *        being a long press one
 *
 * @param tagValueList list of tag/value pairs
 * @param infoLongPress information to build the last page
 * @param rejectText text to use in footer
 * @param callback callback called when transaction is accepted (param is true) or rejected (param is false)
 */
void nbgl_useCaseStaticReview(nbgl_layoutTagValueList_t *tagValueList, nbgl_pageInfoLongPress_t *infoLongPress,
                              char *rejectText, nbgl_choiceCallback_t callback) {
  // memorize context
  onChoice = callback;
  onNav = NULL;
  forwardNavOnly = false;

  staticReviewContext.withLongPress = true;
  memcpy(&staticReviewContext.tagValueList,tagValueList,sizeof(nbgl_layoutTagValueList_t));
  memcpy(&staticReviewContext.infoLongPress,infoLongPress,sizeof(nbgl_pageInfoLongPress_t));
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
  navInfo.navWithTap.skipText = NULL;
  navInfo.progressIndicator = true;
  navInfo.tuneId = TUNE_TAP_CASUAL;

  displayStaticReviewPage(0, true);
}

/**
 * @brief Similar to @ref nbgl_useCaseStaticReview() but with a simple button/footer pair instead of a long press button/footer pair.
 * @note  All tag/value pairs are provided in the API and the number of pages is automatically computed, the last page
 *        being a long press one
 *
 * @param tagValueList list of tag/value pairs
 * @param infoLongPress information to build the last page (even if not a real long press, the info is the same)
 * @param rejectText text to use in footer
 * @param callback callback called when transaction is accepted (param is true) or rejected (param is false)
 */
void nbgl_useCaseStaticReviewLight(nbgl_layoutTagValueList_t *tagValueList, nbgl_pageInfoLongPress_t *infoLongPress,
                                   char *rejectText, nbgl_choiceCallback_t callback) {
  // memorize context
  onChoice = callback;
  onNav = NULL;
  forwardNavOnly = false;

  staticReviewContext.withLongPress = false;
  memcpy(&staticReviewContext.tagValueList,tagValueList,sizeof(nbgl_layoutTagValueList_t));
  memcpy(&staticReviewContext.infoLongPress,infoLongPress,sizeof(nbgl_pageInfoLongPress_t));
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
  navInfo.navWithTap.skipText = NULL;
  navInfo.progressIndicator = true;
  navInfo.tuneId = TUNE_TAP_CASUAL;

  displayStaticReviewPage(0, true);
}

/**
 * @brief Draws a flow of pages to view details on a given tag/value pair that doesn't fit in a single page
 *
 * @param tag tag name (in gray)
 * @param value full value string, that will be split in multiple pages
 * @param wrapping if set to true, value text is wrapped on ' ' characters
 */
void nbgl_useCaseViewDetails(char *tag, char *value, bool wrapping) {
  uint16_t nbLines = nbgl_getTextNbLinesInWidth(BAGL_FONT_INTER_REGULAR_24px, value, SCREEN_WIDTH-2*BORDER_MARGIN, wrapping);

  // initialize context
  detailsContext.tag = tag;
  detailsContext.value = value;
  detailsContext.nbPages = (nbLines+NB_MAX_LINES_IN_DETAILS-1)/NB_MAX_LINES_IN_DETAILS;
  detailsContext.currentPage = 0;
  detailsContext.wrapping = wrapping;
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

  displayDetailsPage(0, true);
}

/**
 * @brief draws an address confirmation page. This page contains the given address in a tag/value layout, with a button to
 *        open a modal to see address as a QR Code,
 *        and at the bottom a button to confirm and a footer to cancel
 *
 * @param address address to confirm (NULL terminated string)
 * @param callback callback called when button or footer is touched (if true, button, if false footer)
  */
void nbgl_useCaseAddressConfirmation(char *address, nbgl_choiceCallback_t callback) {
  nbgl_useCaseAddressConfirmationExt(address, callback, NULL);
}

/**
 * @brief draws an extended address verification page. This page contains the given address in a tag/value layout, with a button to
 *        open a modal to see address as a QR Code.
 *        A "tap to continue" enables to open a second review page to display the other given tag/value pairs, with a button to confirm and a footer to cancel
 *
 * @param address address to confirm (NULL terminated string)
 * @param callback callback called when button or footer is touched (if true, button, if false footer)
 * @param tagValueList list of tag/value pairs (must fit in a single page, and be persistent because no copy)
  */
void nbgl_useCaseAddressConfirmationExt(char *address, nbgl_choiceCallback_t callback, nbgl_layoutTagValueList_t *tagValueList) {
  // save context
  onChoice = callback;
  addressConfirmationContext.address = address;
  addressConfirmationContext.tagValueList = tagValueList;

  // fill navigation structure, common to all pages
  navInfo.navType = NAV_WITH_TAP;
  navInfo.nbPages = (tagValueList == NULL)? 0 : 2;
  navInfo.progressIndicator = true;
  navInfo.tuneId = TUNE_TAP_CASUAL;
  navInfo.navWithTap.backButton = (tagValueList != NULL);
  navInfo.navWithTap.quitText = "Cancel";
  navInfo.navWithTap.nextPageToken = ADDR_NEXT_TOKEN;
  navInfo.navWithTap.backToken = ADDR_BACK_TOKEN;
  navInfo.navWithTap.skipText = NULL;
  navInfo.quitToken = REJECT_TOKEN;

  displayAddressPage(0, true);
}

/**
 * @brief draw a spinner page with the given parameters. The spinner will "turn" automatically every 800 ms
 *
 * @param text text to use under spinner
 */
void nbgl_useCaseSpinner(char* text) {
  pageContext = nbgl_pageDrawSpinner(NULL, (const char*)text);
  nbgl_refreshSpecial(FULL_COLOR_PARTIAL_REFRESH);
}
#endif // NBGL_USE_CASE
