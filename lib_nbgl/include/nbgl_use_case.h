/**
 * @file nbgl_use_case.h
 * @brief API of the Advanced BOLOS Graphical Library, for typical application use-cases
 *
 */

#ifndef NBGL_USE_CASE_H
#define NBGL_USE_CASE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "nbgl_page.h"

/*********************
 *      DEFINES
 *********************/
/**
 *  @brief when using controls in page content (@ref nbgl_pageContent_t), this is the first token value
 * usable for these controls
 */
#define FIRST_USER_TOKEN 20

/**
 *  @brief value of page parameter used with navigation callback when "skip" button is touched, to display the long press button to confirm
 *         review.
 */
#define LAST_PAGE_FOR_REVIEW 0xFF

/**********************
 *      MACROS
 **********************/

/**********************
 *      TYPEDEFS
 **********************/
/**
 * @brief prototype of generic callback function
 */
typedef void (*nbgl_callback_t)(void);

/**
 * @brief prototype of navigation callback function
 * @param page page index (0->(nb_pages-1)) on which we go
 * @param content content to fill (only type and union)
 * @return true if the page content is valid, false if no more page
 */
typedef bool (*nbgl_navCallback_t)(uint8_t page, nbgl_pageContent_t *content);

/**
 * @brief prototype of choice callback function
 * @param confirm if true, means that the confirmation button has been pressed
 */
typedef void (*nbgl_choiceCallback_t)(bool confirm);

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void nbgl_useCaseHome(char *appName, const nbgl_icon_details_t *appIcon, char *tagline, bool withSettings,
                      nbgl_callback_t topRightCallback, nbgl_callback_t quitCallback);
void nbgl_useCaseHomeExt(char *appName, const nbgl_icon_details_t *appIcon, char *tagline, bool withSettings,
                         char *actionButtonText, nbgl_callback_t actionCallback,
                         nbgl_callback_t topRightCallback, nbgl_callback_t quitCallback);
void nbgl_useCaseSettings(char *settingsTitle, uint8_t initPage, uint8_t nbPages, bool touchableTitle,
                          nbgl_callback_t quitCallback, nbgl_navCallback_t navCallback,
                          nbgl_layoutTouchCallback_t controlsCallback);
void nbgl_useCaseChoice(char *message, char *subMessage, char *confirmText, char *rejectString, nbgl_choiceCallback_t callback);
void nbgl_useCaseStatus(char *message, bool isSuccess, nbgl_callback_t quitCallback);
void nbgl_useCaseReviewStart(const nbgl_icon_details_t *icon, char *reviewTitle, char *reviewSubTitle, char *rejectText,
                             nbgl_callback_t continueCallback, nbgl_callback_t rejectCallback);
void nbgl_useCaseRegularReview(uint8_t initPage, uint8_t nbPages, char *rejectText, nbgl_layoutTouchCallback_t buttonCallback,
                               nbgl_navCallback_t navCallback, nbgl_choiceCallback_t choiceCallback);
void nbgl_useCaseForwardOnlyReview(char *rejectText, nbgl_layoutTouchCallback_t buttonCallback,
                                   nbgl_navCallback_t navCallback, nbgl_choiceCallback_t choiceCallback);
void nbgl_useCaseStaticReview(nbgl_layoutTagValueList_t *tagValueList, nbgl_pageInfoLongPress_t *infoLongPress,
                              char *rejectText, nbgl_choiceCallback_t callback);
void nbgl_useCaseStaticReviewLight(nbgl_layoutTagValueList_t *tagValueList, nbgl_pageInfoLongPress_t *infoLongPress,
                                   char *rejectText, nbgl_choiceCallback_t callback);
void nbgl_useCaseViewDetails(char *tag, char *value, bool wrapping);
void nbgl_useCaseAddressConfirmation(char *address, nbgl_choiceCallback_t callback);
void nbgl_useCaseAddressConfirmationExt(char *address, nbgl_choiceCallback_t callback, nbgl_layoutTagValueList_t *tagValueList);
void nbgl_useCaseSpinner(char* text);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NBGL_USE_CASE_H */
