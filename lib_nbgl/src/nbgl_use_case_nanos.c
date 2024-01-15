/**
 * @file nbgl_use_case_nanos.c
 * @brief Implementation of typical pages (or sets of pages) for Applications, for Nanos (X, SP)
 */

#ifdef NBGL_USE_CASE
#ifndef HAVE_SE_TOUCH
/*********************
 *      INCLUDES
 *********************/
#include <string.h>
#include <stdio.h>
#include "nbgl_debug.h"
#include "nbgl_use_case.h"
#include "glyphs.h"
#include "os_pic.h"
#include "os_helpers.h"
#include "ux.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct ReviewContext_s {
    nbgl_navCallback_t         onNav;
    nbgl_choiceCallback_t      onChoice;
    nbgl_layoutTagValueList_t  tagValueList;
    const nbgl_icon_details_t *icon;
    const char                *reviewTitle;
    const char                *address;  // for address confirmation review
    const char                *acceptText;
    const char                *rejectText;
    bool                       forwardNavOnly;
} ReviewContext_t;

typedef struct HomeContext_s {
    const char                *appName;
    const nbgl_icon_details_t *appIcon;
    const char                *appVersion;
    const char                *tagline;
    nbgl_callback_t            aboutCallback;
    nbgl_callback_t            quitCallback;
} HomeContext_t;

typedef struct SettingsContext_s {
    nbgl_navCallback_t    onNav;
    nbgl_callback_t       quitCallback;
    nbgl_actionCallback_t actionCallback;
} SettingsContext_t;

typedef enum {
    REVIEW_USE_CASE,
    HOME_USE_CASE,
    SETTINGS_USE_CASE
} ContextType_t;

typedef struct UseCaseContext_s {
    ContextType_t   type;
    uint8_t         nbPages;
    int8_t          currentPage;
    nbgl_step_t     stepCtx;
    nbgl_stepDesc_t step;
    union {
        ReviewContext_t   review;
        HomeContext_t     home;
        SettingsContext_t settings;
    };
} UseCaseContext_t;

/**********************
 *  STATIC VARIABLES
 **********************/
// char buffers to build some strings
static char appDescription[APP_DESCRIPTION_MAX_LEN];

static UseCaseContext_t context;

/**********************
 *  STATIC FUNCTIONS
 **********************/

static void buttonCallback(nbgl_step_t stepCtx, nbgl_buttonEvent_t event);
static void displayReviewPage(nbgl_stepPosition_t pos);
static void displayHomePage(nbgl_stepPosition_t pos);

static void onAccept(void)
{
    if (context.review.onChoice) {
        context.review.onChoice(true);
    }
}

static void onReject(void)
{
    if (context.review.onChoice) {
        context.review.onChoice(false);
    }
}

static void onSettingsAction(void)
{
    if (context.settings.actionCallback) {
        context.settings.actionCallback(context.currentPage);
    }
}

static void drawStep(nbgl_stepPosition_t        pos,
                     const nbgl_icon_details_t *icon,
                     const char                *txt,
                     const char                *subTxt)
{
    if (context.nbPages > 1) {
        pos |= NEITHER_FIRST_NOR_LAST_STEP;
    }
    else {
        pos |= GET_POS_OF_STEP(context.currentPage, context.nbPages);
    }

    if (icon == NULL) {
        context.stepCtx
            = nbgl_stepDrawText(pos, buttonCallback, NULL, txt, subTxt, BOLD_TEXT1_INFO, false);
    }
    else {
        nbgl_layoutCenteredInfo_t info;
        info.icon       = icon;
        info.text1      = txt;
        info.text2      = subTxt;
        info.onTop      = false;
        info.style      = BOLD_TEXT1_INFO;
        context.stepCtx = nbgl_stepDrawCenteredInfo(pos, buttonCallback, NULL, &info, false);
    }
}

static void buttonCallback(nbgl_step_t stepCtx, nbgl_buttonEvent_t event)
{
    nbgl_stepPosition_t pos;

    UNUSED(stepCtx);
    // create text_area for main text
    if (event == BUTTON_LEFT_PRESSED) {
        if (context.currentPage > 0) {
            context.currentPage--;
        }
        else {
            context.currentPage = (context.nbPages - 1);
        }
        pos = BACKWARD_DIRECTION;
    }
    else if (event == BUTTON_RIGHT_PRESSED) {
        if (context.currentPage < (int) (context.nbPages - 1)) {
            context.currentPage++;
        }
        else {
            context.currentPage = 0;
        }
        pos = FORWARD_DIRECTION;
    }
    else {
        if ((event == BUTTON_BOTH_PRESSED) && (context.step.callback != NULL)) {
            context.step.callback();
        }
        return;
    }
    if ((context.type == REVIEW_USE_CASE) || (context.type == SETTINGS_USE_CASE)) {
        displayReviewPage(pos);
    }
    else {
        displayHomePage(pos);
    }
}

// function used to display the current page in review
static void displayReviewPage(nbgl_stepPosition_t pos)
{
    memset(&context.step, 0, sizeof(context.step));

    if (context.type == REVIEW_USE_CASE) {
        if (context.review.onNav != NULL) {
            // try to get content for this page/step
            if (context.review.onNav(context.currentPage, &context.step) == false) {
                return;
            }
        }
        else {
            if (context.currentPage == 0) {  // title page
                context.step.icon = context.review.icon;
                context.step.text = context.review.reviewTitle;
            }
            else if (context.currentPage == (context.nbPages - 2)) {  // accept page
                context.step.icon     = &C_icon_validate_14;
                context.step.text     = context.review.acceptText;
                context.step.callback = onAccept;
            }
            else if (context.currentPage == (context.nbPages - 1)) {  // reject page
                context.step.icon     = &C_icon_crossmark;
                context.step.text     = context.review.rejectText;
                context.step.callback = onReject;
            }
            else if ((context.review.address != NULL)
                     && (context.currentPage == 1)) {  // address confirmation and 2nd page
                context.step.text    = "Address";
                context.step.subText = context.review.address;
            }
            else {
                uint8_t pairIndex    = (context.review.address != NULL) ? (context.currentPage - 2)
                                                                        : (context.currentPage - 1);
                context.step.text    = context.review.tagValueList.pairs[pairIndex].item;
                context.step.subText = context.review.tagValueList.pairs[pairIndex].value;
            }
        }
    }
    else if (context.type == SETTINGS_USE_CASE) {
        if (context.currentPage < (context.nbPages - 1)) {
            // try to get content for this page/step
            if ((context.settings.onNav == NULL)
                || (context.settings.onNav(context.currentPage, &context.step) == false)) {
                return;
            }
            context.step.callback = onSettingsAction;
        }
        else {  // last page is for quit
            context.step.icon     = &C_icon_back_x;
            context.step.text     = "Back";
            context.step.callback = context.settings.quitCallback;
        }
    }

    const char *txt = NULL;
    if (context.step.text != NULL) {
        txt = context.step.text;
    }
    if (context.step.init != NULL) {
        context.step.init();
    }
    drawStep(pos, context.step.icon, txt, context.step.subText);
    nbgl_refresh();
}

// function used to display the current page in home
static void displayHomePage(nbgl_stepPosition_t pos)
{
    memset(&context.step, 0, sizeof(context.step));

    switch (context.currentPage) {
        case 0:
            context.step.icon = context.home.appIcon;
            context.step.text = context.home.tagline;
            break;
        case 1:
            context.step.text    = "Version";
            context.step.subText = context.home.appVersion;
            break;
        case 2:
            context.step.icon     = &C_icon_certificate;
            context.step.text     = "About";
            context.step.callback = context.home.aboutCallback;
            break;
        case 3:
            context.step.icon     = &C_icon_dashboard_x;
            context.step.text     = "Quit";
            context.step.callback = context.home.quitCallback;
            break;
        default:
            break;
    }

    const char *txt = NULL;
    if (context.step.text != NULL) {
        txt = context.step.text;
    }
    if (context.step.init != NULL) {
        context.step.init();
    }
    drawStep(pos, context.step.icon, txt, context.step.subText);
    nbgl_refresh();
}

/**********************
 *  GLOBAL FUNCTIONS
 **********************/

/**
 * @brief draws the home page of an app (page on which we land when launching it from dashboard)
 *
 * @param appName app name
 * @param appIcon app icon
 * @param appVersion app version
 * @param tagline text under app name (if NULL, it will be "<appName>\nisready")
 * @param aboutCallback callback called when the "about" step is selected (double key)
 * @param quitCallback callback called when the "quit" step is selected (double key)
 */
void nbgl_useCaseHome(const char                *appName,
                      const nbgl_icon_details_t *appIcon,
                      const char                *appVersion,
                      const char                *tagline,
                      nbgl_callback_t            aboutCallback,
                      nbgl_callback_t            quitCallback)
{
    memset(&context, 0, sizeof(UseCaseContext_t));
    context.type               = HOME_USE_CASE;
    context.home.aboutCallback = aboutCallback;
    context.home.quitCallback  = quitCallback;

    if (tagline == NULL) {
        snprintf(appDescription, APP_DESCRIPTION_MAX_LEN, "%s\nis ready", appName);
        context.home.tagline = appDescription;
    }
    else {
        context.home.tagline = tagline;
    }

    context.home.appName    = appName;
    context.home.appIcon    = appIcon;
    context.home.appVersion = appVersion;

    context.nbPages     = 4;
    context.currentPage = 0;

    displayHomePage(FORWARD_DIRECTION);
}

/**
 * @brief Draws the settings pages of an app with as many pages as given
 *        For each page, the given navCallback will be called to get the content. Only 'type' and
 * union has to be set in this content
 *
 * @param initPage page on which to start [0->(nbPages-1)]
 * @param nbPages number of pages
 * @param quitCallback callback called when "quit" step is selected (double button)
 * @param navCallback callback called when pages are navigated with buttons
 * @param actionCallback callback called when one of the navigations page is selected (double
 * button)
 */
void nbgl_useCaseSettings(uint8_t               initPage,
                          uint8_t               nbPages,
                          nbgl_callback_t       quitCallback,
                          nbgl_navCallback_t    navCallback,
                          nbgl_actionCallback_t actionCallback)
{
    memset(&context, 0, sizeof(UseCaseContext_t));
    // memorize context
    context.type                    = SETTINGS_USE_CASE;
    context.settings.onNav          = navCallback;
    context.settings.quitCallback   = quitCallback;
    context.settings.actionCallback = actionCallback;

    context.nbPages     = nbPages + 1;
    context.currentPage = initPage;
    displayReviewPage(FORWARD_DIRECTION);
}

/**
 * @brief Draws a flow of pages of a review. Navigation is available for all pages
 *        For each page, the given navCallback will be called to get the content.
 *        When navigating before the first page of after the last page, the page number will be -1
 *
 * @param initPage page on which to start [0->(nbPages-1)]
 * @param nbPages number of pages.
 * @param navCallback callback called when navigation is touched
 */
void nbgl_useCaseRegularReview(uint8_t initPage, uint8_t nbPages, nbgl_navCallback_t navCallback)
{
    memset(&context, 0, sizeof(UseCaseContext_t));
    context.type = REVIEW_USE_CASE;
    // memorize context
    context.review.onNav          = navCallback;
    context.review.forwardNavOnly = false;

    context.currentPage = initPage;
    context.nbPages     = nbPages;

    displayReviewPage(FORWARD_DIRECTION);
}

/**
 * @brief Draws a flow of pages of a review, without back key.
 *        It is possible to go to next page thanks to "tap to continue".
 *        For each page, the given navCallback will be called to get the content. Only 'type' and
 * union has to be set in this content
 *
 * @param navCallback callback called when navigation "tap to continue" is touched, to get the
 * content of next page
 */
void nbgl_useCaseForwardOnlyReview(nbgl_navCallback_t navCallback)
{
    // memorize context
    context.type                  = REVIEW_USE_CASE;
    context.review.onNav          = navCallback;
    context.review.forwardNavOnly = true;
}

/**
 * @brief Draws a flow of pages of a review.
 * @note  All tag/value pairs are provided in the API and the number of pages is automatically
 * computed, the last page being a long press one
 *
 * @param tagValueList list of tag/value pairs
 * @param icon icon to use in first page
 * @param reviewTitle text to use in title page of the transaction
 * @param acceptText text to use in validation page
 * @param rejectText text to use in rejection page
 * @param callback callback called when transaction is accepted (param is true) or rejected (param
 * is false)
 */
void nbgl_useCaseStaticReview(nbgl_layoutTagValueList_t *tagValueList,
                              const nbgl_icon_details_t *icon,
                              const char                *reviewTitle,
                              const char                *acceptText,
                              const char                *rejectText,
                              nbgl_choiceCallback_t      callback)
{
    // memorize context
    memset(&context, 0, sizeof(UseCaseContext_t));
    context.review.forwardNavOnly = false;
    context.type                  = REVIEW_USE_CASE;

    memcpy(&context.review.tagValueList, tagValueList, sizeof(nbgl_layoutTagValueList_t));

    context.review.reviewTitle = reviewTitle;
    context.review.icon        = icon;
    context.review.acceptText  = acceptText;
    context.review.rejectText  = rejectText;
    context.review.onChoice    = callback;

    context.currentPage = 0;
    // + 3 because 1 page for title and 2 pages at the end for accept/reject
    context.nbPages = tagValueList->nbPairs + 3;

    displayReviewPage(FORWARD_DIRECTION);
}

/**
 * @brief Draws an address confirmation use case. This page contains the given address in a
 * tag/value layout
 *
 * @param icon icon to be used on first page of address review
 * @param title text to be used on first page of address review (NULL terminated string)
 * @param address address to confirm (NULL terminated string)
 * @param callback callback called when either confirm or reject page is double pressed
 */
void nbgl_useCaseAddressConfirmation(const nbgl_icon_details_t *icon,
                                     const char                *title,
                                     const char                *address,
                                     nbgl_choiceCallback_t      callback)
{
    nbgl_useCaseAddressConfirmationExt(icon, title, address, callback, NULL);
}

/**
 * @brief draws an extended address verification page. This page contains the given address in a
 * tag/value layout.
 *
 * @param icon icon to be used on first page of address review
 * @param title text to be used on first page of address review (NULL terminated string)
 * @param address address to confirm (NULL terminated string)
 * @param callback callback called when either confirm or reject page is double pressed
 * @param tagValueList list of tag/value pairs (must be persistent because no copy)
 */
void nbgl_useCaseAddressConfirmationExt(const nbgl_icon_details_t       *icon,
                                        const char                      *title,
                                        const char                      *address,
                                        nbgl_choiceCallback_t            callback,
                                        const nbgl_layoutTagValueList_t *tagValueList)
{
    // memorize context
    memset(&context, 0, sizeof(UseCaseContext_t));
    context.review.forwardNavOnly = false;
    context.type                  = REVIEW_USE_CASE;

    if (tagValueList) {
        memcpy(&context.review.tagValueList, tagValueList, sizeof(nbgl_layoutTagValueList_t));
    }

    context.review.address     = address;
    context.review.reviewTitle = title;
    context.review.icon        = icon;
    context.review.acceptText  = "Approve";
    context.review.rejectText  = "Reject";
    context.review.onChoice    = callback;

    context.currentPage = 0;
    // + 4 because 1 page for title, 1 for address and 2 pages at the end for approve/reject
    context.nbPages = 4;
    if (tagValueList) {
        context.nbPages += tagValueList->nbPairs;
    }

    displayReviewPage(FORWARD_DIRECTION);
}

/**
 * @brief draw a spinner page with the given parameters. The spinner will "turn" automatically every
 * 800 ms
 *
 * @param text text to use under spinner
 */
void nbgl_useCaseSpinner(const char *text)
{
    // pageContext = nbgl_pageDrawSpinner(NULL, (const char*)text);
    UNUSED(text);
    nbgl_refresh();
}

#endif  // HAVE_SE_TOUCH
#endif  // NBGL_USE_CASE
