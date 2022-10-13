/**
 * @file nbgl_page.c
 * @brief Implementation of predefined pages management for Applications
 */

/*********************
 *      INCLUDES
 *********************/
#include <string.h>
#include "nbgl_debug.h"
#include "nbgl_page.h"
#include "glyphs.h"
#include "os_pic.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct AddressConfirmationContext_s {
  nbgl_pageAddressConfirmationDescription_t description;
  nbgl_layoutTouchCallback_t callback;
  nbgl_layout_t layout;
  nbgl_layout_t modalLayout;
} AddressConfirmationContext_t;

/**********************
 *  STATIC VARIABLES
 **********************/
static AddressConfirmationContext_t addressConfirmationContext;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void addContent(nbgl_pageContent_t* content, nbgl_layout_t *layout) {
  if (content->title != NULL) {
    nbgl_layoutBar_t bar;
    bar.text = (char*)content->title;
    bar.subText = NULL;
    bar.iconRight = NULL;
    bar.iconLeft = content->isTouchableTitle? &C_leftArrow32px:NULL;
    bar.token = content->titleToken;
    bar.centered = true;
    bar.inactive = false;
    bar.tuneId = content->tuneId;
    nbgl_layoutAddTouchableBar(layout, &bar);
    nbgl_layoutAddSeparationLine(layout);
  }
  switch (content->type) {
    case INFO_LONG_PRESS:
    {
      nbgl_layoutCenteredInfo_t centeredInfo;
      centeredInfo.icon = content->infoLongPress.icon;
      centeredInfo.text1 = content->infoLongPress.text;
      centeredInfo.text2 = NULL;
      centeredInfo.text3 = NULL;
      centeredInfo.style = LARGE_CASE_INFO;
      centeredInfo.offsetY = -32;
      centeredInfo.onTop = false;
      nbgl_layoutAddCenteredInfo(layout,&centeredInfo);
      nbgl_layoutAddLongPressButton(layout,(char*)content->infoLongPress.longPressText,content->infoLongPress.longPressToken,content->infoLongPress.tuneId);
      break;
    }
    case CENTERED_INFO:
      nbgl_layoutAddCenteredInfo(layout,&content->centeredInfo);
      break;
    case TAG_VALUE_LIST:
      nbgl_layoutAddTagValueList(layout,&content->tagValueList);
      break;
    case TAG_VALUE_DETAILS:
    {
      nbgl_layoutButton_t buttonInfo;
      nbgl_layoutAddTagValueList(layout,&content->tagValueDetails.tagValueList);
      buttonInfo.fittingContent = true;
      buttonInfo.icon = NULL;
      buttonInfo.style = WHITE_BACKGROUND;
      buttonInfo.text = (char*)content->tagValueDetails.detailsButtonText;
      buttonInfo.token = content->tagValueDetails.detailsButtonToken;
      buttonInfo.tuneId = content->tagValueDetails.tuneId;
      buttonInfo.onBottom = false;
      nbgl_layoutAddButton(layout,&buttonInfo);
      break;
    }
    case SWITCHES_LIST:
    {
      uint8_t i;
      for (i=0;i<content->switchesList.nbSwitches;i++) {
        nbgl_layoutAddSwitch(layout, &content->switchesList.switches[i]);
        nbgl_layoutAddSeparationLine(layout);
      }
      break;
    }
    case INFOS_LIST:
    {
      uint8_t i;
      for (i=0;i<content->infosList.nbInfos;i++) {
        nbgl_layoutAddText(layout,(char*)content->infosList.infoTypes[i],(char*)content->infosList.infoContents[i]);
        nbgl_layoutAddSeparationLine(layout);
      }
      break;
    }
    case CHOICES_LIST:
      nbgl_layoutAddRadioChoice(layout,&content->choicesList);
      break;
    case BARS_LIST:
    {
      uint8_t i;
      for (i=0;i<content->barsList.nbBars;i++) {
        nbgl_layoutBar_t bar;
        bar.text = (char*)content->barsList.barTexts[i];
        bar.subText = NULL;
        bar.iconRight = &C_Next32px;
        bar.iconLeft = NULL;
        bar.token = content->barsList.tokens[i];
        bar.centered = false;
        bar.tuneId = content->barsList.tuneId;
        nbgl_layoutAddTouchableBar(layout, &bar);
        nbgl_layoutAddSeparationLine(layout);
      }
      break;
    }
  }
}

static void layoutTouchCallbackQR(int token, uint8_t index);

static void displayQR(void) {
  nbgl_layoutDescription_t layoutDescription = {
    .modal = true,
    .onActionCallback = &layoutTouchCallbackQR,
    .tapActionText = NULL
  };

  addressConfirmationContext.modalLayout = nbgl_layoutGet(&layoutDescription);
    nbgl_layoutQRCode_t qrCode = {
      .url = (char*)addressConfirmationContext.description.address,
    .text1 = NULL,
    .text2 = (char*)addressConfirmationContext.description.address // display as gray text
    };
  nbgl_layoutAddQRCode(addressConfirmationContext.modalLayout, &qrCode);

  nbgl_layoutAddBottomButton(addressConfirmationContext.modalLayout, &C_cross32px, 0, true, TUNE_TAP_CASUAL);
  nbgl_layoutDraw(addressConfirmationContext.modalLayout);
}

static void layoutTouchCallback(int token, uint8_t index) {
  if ((token == addressConfirmationContext.description.cancelToken) ||
      (token == addressConfirmationContext.description.confirmationToken)) {
    addressConfirmationContext.callback(token, index);
  }
  else {
    // display the address as QR Code
    displayQR();
  }
}

static void layoutTouchCallbackQR(int token, uint8_t index) {
  UNUSED(token);
  UNUSED(index);

  // dismiss modal
  nbgl_layoutRelease(addressConfirmationContext.modalLayout);
  nbgl_screenRedraw();
  nbgl_refresh();
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * @brief draw a page with a centered text with LEDGER style
 *
 * @param onActionCallback common callback for all actions on this page
 * @param ticker ticker configuration, set to NULL to disable it
 * @param text text in LEDGER style (UPPER CASE only)
 * @return the page context (or NULL if error)
 */
nbgl_page_t* nbgl_pageDrawLedgerInfo(nbgl_layoutTouchCallback_t onActionCallback, nbgl_screenTickerConfiguration_t *ticker, const char* text) {
  nbgl_layoutDescription_t layoutDescription;
  nbgl_layout_t *layout;
  nbgl_layoutCenteredInfo_t centeredInfo = {
    .text1 = (char*)text,
    .text2 = NULL,
    .text3 = NULL,
    .style = LEDGER_INFO,
    .icon = NULL,
    .offsetY = 0
  };

  layoutDescription.modal = false;

  layoutDescription.onActionCallback = onActionCallback;
  layoutDescription.tapActionText = NULL;

  layoutDescription.ticker.tickerCallback = ticker->tickerCallback;
  layoutDescription.ticker.tickerIntervale = ticker->tickerIntervale;
  layoutDescription.ticker.tickerValue = ticker->tickerValue;
  layout = nbgl_layoutGet(&layoutDescription);

  nbgl_layoutAddCenteredInfo(layout, &centeredInfo);

  nbgl_layoutDraw(layout);

  return (nbgl_page_t*)layout;
}

/**
 * @brief draw a spinner page with the given parameters. The spinner will "turn" automatically every 800 ms
 *
 * @param onActionCallback common callback for all actions on this page
 * @param text text to use under spinner
 * @return the page context (or NULL if error)
 */
nbgl_page_t* nbgl_pageDrawSpinner(nbgl_layoutTouchCallback_t onActionCallback, const char* text) {
  nbgl_layoutDescription_t layoutDescription;
  nbgl_layout_t *layout;

  layoutDescription.modal = false;

  layoutDescription.onActionCallback = onActionCallback;
  layoutDescription.tapActionText = NULL;

  layoutDescription.ticker.tickerCallback = NULL;
  layout = nbgl_layoutGet(&layoutDescription);

  nbgl_layoutAddSpinner(layout, (char*)text, false);

  nbgl_layoutDraw(layout);

  return (nbgl_page_t*)layout;
}

/**
 * @brief draw a page with a centered info (icon and/or texts) with a touchable footer,
 * in a potential "tapable" area, with an optional top-right button, with an optional bottom button
 *
 * @param onActionCallback common callback for all actions on this page
 * @param ticker ticker configuration, set to NULL to disable it
 * @param info structure describing the centered info and other controls of this page
 * @return the page context (or NULL if error)
 */
nbgl_page_t* nbgl_pageDrawInfo(nbgl_layoutTouchCallback_t onActionCallback, nbgl_screenTickerConfiguration_t *ticker, nbgl_pageInfoDescription_t *info) {
  nbgl_layoutDescription_t layoutDescription;
  nbgl_layout_t *layout;

  layoutDescription.modal = false;

  layoutDescription.onActionCallback = onActionCallback;
  layoutDescription.tapActionText = (char*)info->tapActionText;
  layoutDescription.tapActionToken = info->tapActionToken;
  layoutDescription.tapTuneId = info->tuneId;

  if (ticker != NULL) {
    layoutDescription.ticker.tickerCallback = ticker->tickerCallback;
    layoutDescription.ticker.tickerIntervale = ticker->tickerIntervale;
    layoutDescription.ticker.tickerValue = ticker->tickerValue;
  }
  else {
    layoutDescription.ticker.tickerCallback = NULL;
  }
  layout = nbgl_layoutGet(&layoutDescription);

  nbgl_layoutAddCenteredInfo(layout,&info->centeredInfo);

  if (info->footerText != NULL) {
    nbgl_layoutAddFooter(layout, (char*)PIC(info->footerText), info->footerToken, info->tuneId);
  }
  if (info->topRightStyle != NO_BUTTON_STYLE) {
    const nbgl_icon_details_t *icon;
    if (info->topRightStyle == SETTINGS_ICON)
      icon = &C_wheel32px;
    else if (info->topRightStyle == INFO_ICON)
      icon = &C_info_i_32px;
    else if (info->topRightStyle == QUIT_ICON)
      icon = &C_cross32px;
    else
      return NULL;
    nbgl_layoutAddTopRightButton(layout, PIC(icon), info->topRightToken, info->tuneId);
  }
  if (info->bottomButtonStyle != NO_BUTTON_STYLE) {
    const nbgl_icon_details_t *icon;
    if (info->bottomButtonStyle == SETTINGS_ICON)
      icon = &C_wheel32px;
    else if (info->bottomButtonStyle == INFO_ICON)
      icon = &C_info_i_32px;
    else if (info->bottomButtonStyle == QUIT_ICON)
      icon = &C_cross32px;
    else
      return NULL;
    nbgl_layoutAddBottomButton(layout, PIC(icon), info->bottomButtonToken, false, info->tuneId);
  }
  nbgl_layoutDraw(layout);

  return (nbgl_page_t*)layout;
}

/**
 * @brief draw a confirmation page, with a centered info (icon and/or text), a button to confirm and a footer to cancel
 *
 * @param onActionCallback common callback for all actions on this page
 * @param info structure describing the centered info and other controls of this page
 * @return the page context (or NULL if error)
 */
nbgl_page_t* nbgl_pageDrawConfirmation(nbgl_layoutTouchCallback_t onActionCallback, nbgl_pageConfirmationDescription_t *info) {
  nbgl_layoutDescription_t layoutDescription;
  nbgl_layout_t *layout;

  layoutDescription.modal = false;

  layoutDescription.onActionCallback = onActionCallback;
  layoutDescription.tapActionText = NULL;

  layoutDescription.ticker.tickerCallback = NULL;
  layout = nbgl_layoutGet(&layoutDescription);
  if (info->cancelText == NULL) {
    nbgl_layoutButton_t buttonInfo = {
      .style = BLACK_BACKGROUND,
      .text = (char*)info->confirmationText,
      .icon = NULL,
      .token = info->confirmationToken,
      .fittingContent = false,
      .tuneId = info->tuneId,
      .onBottom = true
    };
    nbgl_layoutAddBottomButton(layout, PIC(&C_cross32px), info->cancelToken, true, info->tuneId);
    nbgl_layoutAddButton(layout, &buttonInfo);
  }
  else {
    nbgl_layoutChoiceButtons_t buttonsInfo = {
      .bottomText = (char*)PIC(info->cancelText),
      .token = info->confirmationToken,
      .topText = (char*)PIC(info->confirmationText),
      .tuneId = info->tuneId
    };
    nbgl_layoutAddChoiceButtons(layout,&buttonsInfo);
  }
  nbgl_layoutAddCenteredInfo(layout,&info->centeredInfo);

  nbgl_layoutDraw(layout);

  return (nbgl_page_t*)layout;
}

/**
 * @brief draw an address confirmation page. This page contains a "page selector" to display either address in text or QRCode format,
 * and at the bottom a button to confirm and a footer to cancel
 *
 * @param onActionCallback common callback for all actions on this page (confirmation and cancelation)
 * @param info structure describing the all controls of this page
 * @return the page context (or NULL if error)
 */
nbgl_page_t* nbgl_pageDrawAddressConfirmation(nbgl_layoutTouchCallback_t onActionCallback, nbgl_pageAddressConfirmationDescription_t *info) {
  nbgl_layoutButton_t buttonInfo;
  nbgl_layoutDescription_t layoutDescription = {
    .modal = false,
    .onActionCallback = &layoutTouchCallback,
    .tapActionText = NULL
  };
  uint8_t pageModeToken;

  // save context
  addressConfirmationContext.callback = onActionCallback;
  memcpy(&addressConfirmationContext.description,info,sizeof(nbgl_pageAddressConfirmationDescription_t));

  // choose an unused token
  pageModeToken = addressConfirmationContext.description.confirmationToken+1;
  if (pageModeToken == addressConfirmationContext.description.cancelToken)
    pageModeToken++;

  if (addressConfirmationContext.layout)
    nbgl_layoutRelease(addressConfirmationContext.layout);

  addressConfirmationContext.layout = nbgl_layoutGet(&layoutDescription);
  if (addressConfirmationContext.description.cancelText != NULL)
    nbgl_layoutAddFooter(addressConfirmationContext.layout, (char*)addressConfirmationContext.description.cancelText,
                       addressConfirmationContext.description.cancelToken, TUNE_TAP_CASUAL);
  else
    nbgl_layoutAddFooter(addressConfirmationContext.layout, "It doesn't match",
                       addressConfirmationContext.description.cancelToken, TUNE_TAP_CASUAL);

  nbgl_layoutTagValue_t tagValuePair = {
    .item = "Address",
    .value = (char*)addressConfirmationContext.description.address
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
  if (addressConfirmationContext.description.qrCodeButtonText != NULL)
    buttonInfo.text = (char*)addressConfirmationContext.description.qrCodeButtonText;
  else
    buttonInfo.text = "Show as QR";
  buttonInfo.token = pageModeToken;
  buttonInfo.tuneId = TUNE_TAP_NEXT;
  buttonInfo.onBottom = false;
  nbgl_layoutAddButton(addressConfirmationContext.layout,&buttonInfo);
  buttonInfo.style = BLACK_BACKGROUND;
  buttonInfo.icon = NULL;
  buttonInfo.token = addressConfirmationContext.description.confirmationToken;
  buttonInfo.fittingContent = false;
  buttonInfo.tuneId = addressConfirmationContext.description.tuneId;
  buttonInfo.onBottom = true;
  if (addressConfirmationContext.description.confirmationText != NULL)
    buttonInfo.text = (char*)addressConfirmationContext.description.confirmationText;
  else
    buttonInfo.text = "It matches";
  nbgl_layoutAddButton(addressConfirmationContext.layout, &buttonInfo);
  nbgl_layoutDraw(addressConfirmationContext.layout);

  return (nbgl_page_t*)addressConfirmationContext.layout;
}

/**
 * @brief draw a generic content page, with the given content, and if nav parameter is not NULL, with the given
 * navigation controls (either with navigation bar or with "tap" and "back")
 *
 * @param onActionCallback common callback for all actions on this page
 * @param nav structure describing the navigation controls of this page (no navigation if NULL)
 * @param content structure describing the main content of this page
 * @param modal set to true to draw as a modal
 * @return the page context (or NULL if error)
 */
nbgl_page_t* nbgl_pageDrawGenericContentExt(nbgl_layoutTouchCallback_t onActionCallback,
                        nbgl_pageNavigationInfo_t *nav,
                                            nbgl_pageContent_t* content,
                                            bool modal) {
  nbgl_layoutDescription_t layoutDescription;
  nbgl_layout_t *layout;

  layoutDescription.modal = modal;
  layoutDescription.onActionCallback = onActionCallback;
  layoutDescription.ticker.tickerCallback = NULL;

  if ((nav != NULL) && (nav->navType == NAV_WITH_TAP)) {
    layoutDescription.tapActionText = (char*)nav->navWithTap.nextPageText;
    layoutDescription.tapActionToken = nav->navWithTap.nextPageToken;
    layoutDescription.tapTuneId = nav->tuneId;
  }
  else {
    layoutDescription.tapActionText = NULL;
  }

  layout = nbgl_layoutGet(&layoutDescription);
  if (nav != NULL) {
    if (nav->navType == NAV_WITH_TAP) {
      nbgl_layoutAddFooter(layout, (char*)nav->navWithTap.quitText, nav->quitToken, nav->tuneId);
      if (nav->progressIndicator)
        nbgl_layoutAddProgressIndicator(layout, nav->activePage, nav->nbPages, nav->navWithTap.backButton, nav->navWithTap.backToken, nav->tuneId);
    }
    else if (nav->navType == NAV_WITH_BUTTONS) {
      nbgl_layoutNavigationBar_t navInfo = {
        .activePage = nav->activePage,
        .nbPages = nav->nbPages,
        .token = nav->navWithButtons.navToken,
        .withExitKey = nav->navWithButtons.quitButton,
        .withSeparationLine = false,
        .tuneId = nav->tuneId
      };
      nbgl_layoutAddNavigationBar(layout,&navInfo);
      if (nav->progressIndicator)
        nbgl_layoutAddProgressIndicator(layout, nav->activePage, nav->nbPages, false, 0, nav->tuneId);
    }
  }
  addContent(content, layout);
  nbgl_layoutDraw(layout);

  return (nbgl_page_t*)layout;
}

/**
 * @brief draw a generic content page, with the given content, and if nav parameter is not NULL, with the given
 * navigation controls (either with navigation bar or with "tap" and "back")
 *
 * @param onActionCallback common callback for all actions on this page
 * @param nav structure describing the navigation controls of this page (no navigation if NULL)
 * @param content structure describing the main content of this page
 * @return the page context (or NULL if error)
 */
nbgl_page_t* nbgl_pageDrawGenericContent(nbgl_layoutTouchCallback_t onActionCallback,
                                         nbgl_pageNavigationInfo_t *nav,
                                         nbgl_pageContent_t* content) {
  return nbgl_pageDrawGenericContentExt(onActionCallback,nav,content,false);
}

/**
 * @brief Release the page obtained with any of the nbgl_pageDrawXXX() functions
 *
 * @param page page to release
 * @return >= 0 if OK
 */
int nbgl_pageRelease(nbgl_page_t* page) {
  int ret;

  LOG_DEBUG(PAGE_LOGGER,"nbgl_pageRelease(): \n");
  ret = nbgl_layoutRelease((nbgl_layout_t *)page);

  return ret;
}
