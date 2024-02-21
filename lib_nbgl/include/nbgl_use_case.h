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

#include "nbgl_content.h"
#ifdef NBGL_PAGE
#include "nbgl_page.h"
#else  // NBGL_PAGE
#include "nbgl_flow.h"
#endif  // NBGL_PAGE

/*********************
 *      DEFINES
 *********************/
/**
 *  @brief when using controls in page content (@ref nbgl_pageContent_t), this is the first token
 * value usable for these controls
 */
#define FIRST_USER_TOKEN 20

/**
 *  @brief value of page parameter used with navigation callback when "skip" button is touched, to
 * display the long press button to confirm review.
 */
#define LAST_PAGE_FOR_REVIEW 0xFF

/**
 *  @brief maximum number of lines for value field in details pages
 */
#ifdef TARGET_STAX
#define NB_MAX_LINES_IN_DETAILS 12
#else  // TARGET_STAX
#define NB_MAX_LINES_IN_DETAILS 11
#endif  // TARGET_STAX

/**
 *  @brief maximum number of lines for value field in review pages
 */
#define NB_MAX_LINES_IN_REVIEW 9

/**
 *  @brief maximum number of simultaneously displayed pairs in review pages.
 *         Can be useful when using nbgl_useCaseStaticReview() with the
 *         callback mechanism to retrieve the item/value pairs.
 */
#define NB_MAX_DISPLAYED_PAIRS_IN_REVIEW 4

/**
 *  @brief height available for tag/value pairs display
 */
#define TAG_VALUE_AREA_HEIGHT 400

/**
 *  @brief Default strings used in the Home tagline
 */
#define TAGLINE_PART1 "This app enables signing\ntransactions on the"
#define TAGLINE_PART2 "network."

/**
 *  @brief Length of buffer used for the default Home tagline
 */
#define APP_DESCRIPTION_MAX_LEN 74

/**
 *  @brief Max supported length of appName used for the default Home tagline
 */
#define MAX_APP_NAME_FOR_SDK_TAGLINE \
    (APP_DESCRIPTION_MAX_LEN - 1 - (sizeof(TAGLINE_PART1) + sizeof(TAGLINE_PART2)))

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

/**
 * @brief prototype of function to be called when an page of settings is double-pressed
 * @param page page index (0->(nb_pages-1))
 */
typedef void (*nbgl_actionCallback_t)(uint8_t page);

/**
 * @brief prototype of pin validation callback function
 * @param content pin value
 * @param length pin length
 */
typedef void (*nbgl_pinValidCallback_t)(const uint8_t *content, uint8_t page);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

#ifdef HAVE_SE_TOUCH
// utils
uint8_t nbgl_useCaseGetNbTagValuesInPage(uint8_t                          nbPairs,
                                         const nbgl_layoutTagValueList_t *tagValueList,
                                         uint8_t                          startIndex,
                                         bool                            *tooLongToFit);
uint8_t nbgl_useCaseGetNbPagesForTagValueList(const nbgl_layoutTagValueList_t *tagValueList);

// use case drawing
void nbgl_useCaseHome(const char                *appName,
                      const nbgl_icon_details_t *appIcon,
                      const char                *tagline,
                      bool                       withSettings,
                      nbgl_callback_t            topRightCallback,
                      nbgl_callback_t            quitCallback);
void nbgl_useCaseHomeExt(const char                *appName,
                         const nbgl_icon_details_t *appIcon,
                         const char                *tagline,
                         bool                       withSettings,
                         const char                *actionButtonText,
                         nbgl_callback_t            actionCallback,
                         nbgl_callback_t            topRightCallback,
                         nbgl_callback_t            quitCallback);
void nbgl_useCasePlugInHome(const char                *plugInName,
                            const char                *appName,
                            const nbgl_icon_details_t *appIcon,
                            const char                *tagline,
                            const char                *subTagline,
                            bool                       withSettings,
                            nbgl_callback_t            topRightCallback,
                            nbgl_callback_t            quitCallback);
void nbgl_useCaseSettings(const char                *settingsTitle,
                          uint8_t                    initPage,
                          uint8_t                    nbPages,
                          bool                       touchableTitle,
                          nbgl_callback_t            quitCallback,
                          nbgl_navCallback_t         navCallback,
                          nbgl_layoutTouchCallback_t controlsCallback);
void nbgl_useCaseChoice(const nbgl_icon_details_t *icon,
                        const char                *message,
                        const char                *subMessage,
                        const char                *confirmText,
                        const char                *rejectString,
                        nbgl_choiceCallback_t      callback);
void nbgl_useCaseConfirm(const char     *message,
                         const char     *subMessage,
                         const char     *confirmText,
                         const char     *rejectText,
                         nbgl_callback_t callback);
void nbgl_useCaseStatus(const char *message, bool isSuccess, nbgl_callback_t quitCallback);
void nbgl_useCaseReviewStart(const nbgl_icon_details_t *icon,
                             const char                *reviewTitle,
                             const char                *reviewSubTitle,
                             const char                *rejectText,
                             nbgl_callback_t            continueCallback,
                             nbgl_callback_t            rejectCallback);
void nbgl_useCaseRegularReview(uint8_t                    initPage,
                               uint8_t                    nbPages,
                               const char                *rejectText,
                               nbgl_layoutTouchCallback_t buttonCallback,
                               nbgl_navCallback_t         navCallback,
                               nbgl_choiceCallback_t      choiceCallback);
void nbgl_useCaseForwardOnlyReview(const char                *rejectText,
                                   nbgl_layoutTouchCallback_t buttonCallback,
                                   nbgl_navCallback_t         navCallback,
                                   nbgl_choiceCallback_t      choiceCallback);
void nbgl_useCaseForwardOnlyReviewNoSkip(const char                *rejectText,
                                         nbgl_layoutTouchCallback_t buttonCallback,
                                         nbgl_navCallback_t         navCallback,
                                         nbgl_choiceCallback_t      choiceCallback);
void nbgl_useCaseStaticReview(const nbgl_layoutTagValueList_t *tagValueList,
                              const nbgl_pageInfoLongPress_t  *infoLongPress,
                              const char                      *rejectText,
                              nbgl_choiceCallback_t            callback);
void nbgl_useCaseStaticReviewLight(const nbgl_layoutTagValueList_t *tagValueList,
                                   const nbgl_pageInfoLongPress_t  *infoLongPress,
                                   const char                      *rejectText,
                                   nbgl_choiceCallback_t            callback);
void nbgl_useCaseViewDetails(const char *tag, const char *value, bool wrapping);
void nbgl_useCaseAddressConfirmation(const char *address, nbgl_choiceCallback_t callback);
void nbgl_useCaseAddressConfirmationExt(const char                      *address,
                                        nbgl_choiceCallback_t            callback,
                                        const nbgl_layoutTagValueList_t *tagValueList);
#ifdef NBGL_KEYPAD
void nbgl_useCaseKeypad(const char                *title,
                        uint8_t                    minDigits,
                        uint8_t                    maxDigits,
                        uint8_t                    backToken,
                        bool                       shuffled,
                        tune_index_e               tuneId,
                        nbgl_pinValidCallback_t    validatePinCallback,
                        nbgl_layoutTouchCallback_t actionCallback);
#endif
#else   // HAVE_SE_TOUCH
void nbgl_useCaseHome(const char                *appName,
                      const nbgl_icon_details_t *appIcon,
                      const char                *appVersion,
                      const char                *tagline,
                      nbgl_callback_t            aboutCallback,
                      nbgl_callback_t            quitCallback);
void nbgl_useCaseSettings(uint8_t               initPage,
                          uint8_t               nbPages,
                          nbgl_callback_t       quitCallback,
                          nbgl_navCallback_t    navCallback,
                          nbgl_actionCallback_t actionCallback);
void nbgl_useCaseRegularReview(uint8_t initPage, uint8_t nbPages, nbgl_navCallback_t navCallback);
void nbgl_useCaseForwardOnlyReview(nbgl_navCallback_t navCallback);
void nbgl_useCaseStaticReview(nbgl_layoutTagValueList_t *tagValueList,
                              const nbgl_icon_details_t *icon,
                              const char                *reviewTitle,
                              const char                *acceptText,
                              const char                *rejectText,
                              nbgl_choiceCallback_t      callback);
void nbgl_useCaseAddressConfirmation(const nbgl_icon_details_t *icon,
                                     const char                *title,
                                     const char                *address,
                                     nbgl_choiceCallback_t      callback);
void nbgl_useCaseAddressConfirmationExt(const nbgl_icon_details_t       *icon,
                                        const char                      *title,
                                        const char                      *address,
                                        nbgl_choiceCallback_t            callback,
                                        const nbgl_layoutTagValueList_t *tagValueList);
#endif  // HAVE_SE_TOUCH
void nbgl_useCaseSpinner(const char *text);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NBGL_USE_CASE_H */
