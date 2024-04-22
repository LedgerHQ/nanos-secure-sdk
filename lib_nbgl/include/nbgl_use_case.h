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
#include "nbgl_lns.h"

/*********************
 *      DEFINES
 *********************/
/**
 *  @brief when using controls in page content (@ref nbgl_pageContent_t), this is the first token
 * value usable for these controls
 */
#define FIRST_USER_TOKEN 20

/**
 *  @brief maximum number of simultaneously displayed pairs in review pages.
 *         Can be useful when using nbgl_useCaseStaticReview() with the
 *         callback mechanism to retrieve the item/value pairs.
 */
#define NB_MAX_DISPLAYED_PAIRS_IN_REVIEW 1

/**
 *  @brief Value to pass to nbgl_useCaseHomeAndSettings() initSettingPage parameter
 *         to initialize the use case on the Home page and not on a specific setting
 *         page.
 */
#define INIT_HOME_PAGE 0xff

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
 * @brief prototype of choice callback function
 * @param confirm if true, means that the confirmation button has been pressed
 */
typedef void (*nbgl_choiceCallback_t)(bool confirm);

/**
 * @brief prototype of content navigation callback function
 * @param contentIndex content index (0->(nbContents-1)) that is needed by the lib
 * @param content content to fill
 */
typedef void (*nbgl_contentCallback_t)(uint8_t contentIndex, nbgl_content_t *content);

typedef struct {
    bool callbackCallNeeded;  ///< indicates whether contents should be retrieved using
                              ///< contentsList or contentGetterCallback
    union {
        const nbgl_content_t *contentsList;  ///< array of nbgl_content_t (nbContents items).
        nbgl_contentCallback_t
            contentGetterCallback;  ///< function to call to retrieve a given content
    };
    uint8_t nbContents;  ///< number of contents
} nbgl_genericContents_t;

typedef struct {
    const char                *text;
    const nbgl_icon_details_t *icon;
    nbgl_callback_t            callback;
} nbgl_homeAction_t;

/**
 * @brief The different types of operation to review
 *
 */
typedef enum {
    TYPE_TRANSACTION = 0,  // For operations transferring a coin or taken from an account to another
    TYPE_MESSAGE,  // For operations signing a message that will not be broadcast on the blockchain
    TYPE_OPERATION,  // For other types of operation (generic type)
} nbgl_operationType_t;

/**
 * @brief The different types of review status
 *
 */
typedef enum {
    STATUS_TYPE_TRANSACTION_SIGNED = 0,
    STATUS_TYPE_TRANSACTION_REJECTED,
    STATUS_TYPE_MESSAGE_SIGNED,
    STATUS_TYPE_MESSAGE_REJECTED,
    STATUS_TYPE_OPERATION_SIGNED,
    STATUS_TYPE_OPERATION_REJECTED,
    STATUS_TYPE_ADDRESS_VERIFIED,
    STATUS_TYPE_ADDRESS_REJECTED,
} nbgl_reviewStatusType_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void nbgl_useCaseHomeAndSettings(const char                   *appName,
                                 const nbgl_icon_details_t    *appIcon,
                                 const char                   *tagline,
                                 const uint8_t                 initSettingPage,
                                 const nbgl_genericContents_t *settingContents,
                                 const nbgl_contentInfoList_t *infosList,
                                 const nbgl_homeAction_t      *action,
                                 nbgl_callback_t               quitCallback);

void nbgl_useCaseReview(nbgl_operationType_t              operationType,
                        const nbgl_contentTagValueList_t *tagValueList,
                        const nbgl_icon_details_t        *icon,
                        const char                       *reviewTitle,
                        const char                       *reviewSubTitle,
                        const char                       *finishTitle,
                        nbgl_choiceCallback_t             choiceCallback);

void nbgl_useCaseReviewLight(nbgl_operationType_t              operationType,
                             const nbgl_contentTagValueList_t *tagValueList,
                             const nbgl_icon_details_t        *icon,
                             const char                       *reviewTitle,
                             const char                       *reviewSubTitle,
                             const char                       *finishTitle,
                             nbgl_choiceCallback_t             choiceCallback);

void nbgl_useCaseAddressReview(const char                       *address,
                               const nbgl_contentTagValueList_t *additionalTagValueList,
                               const nbgl_icon_details_t        *icon,
                               const char                       *reviewTitle,
                               const char                       *reviewSubTitle,
                               nbgl_choiceCallback_t             choiceCallback);

void nbgl_useCaseStatus(const char *message, bool isSuccess, nbgl_callback_t quitCallback);

void nbgl_useCaseReviewStatus(nbgl_reviewStatusType_t reviewStatusType,
                              nbgl_callback_t         quitCallback);

void nbgl_useCaseReviewStreamingStart(nbgl_operationType_t       operationType,
                                      const nbgl_icon_details_t *icon,
                                      const char                *reviewTitle,
                                      const char                *reviewSubTitle,
                                      nbgl_choiceCallback_t      choiceCallback);

void nbgl_useCaseReviewStreamingContinue(const nbgl_contentTagValueList_t *tagValueList,
                                         nbgl_choiceCallback_t             choiceCallback);

void nbgl_useCaseReviewStreamingFinish(const char           *finishTitle,
                                       nbgl_choiceCallback_t choiceCallback);

void nbgl_useCaseSpinner(const char *text);

void nbgl_useCaseChoice(const nbgl_icon_details_t *icon,
                        const char                *message,
                        const char                *subMessage,
                        const char                *confirmText,
                        const char                *rejectString,
                        nbgl_choiceCallback_t      callback);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NBGL_USE_CASE_H */
