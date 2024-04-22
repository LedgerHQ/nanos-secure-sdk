#ifdef HAVE_NBGL

#include "ux_sync.h"

static ux_sync_ret_t g_ret;
static bool          g_ended;

static void choice_callback(bool confirm)
{
    if (confirm) {
        g_ret = UX_SYNC_RET_APPROVED;
    }
    else {
        g_ret = UX_SYNC_RET_REJECTED;
    }

    g_ended = true;
}

static void quit_callback(void)
{
    g_ret   = UX_SYNC_RET_QUITTED;
    g_ended = true;
}

static void ux_sync_init(void)
{
    g_ended = false;
    g_ret   = UX_SYNC_RET_ERROR;
}

static ux_sync_ret_t ux_sync_wait(bool exitOnApdu)
{
    bool apduReceived;

    while (!g_ended) {
        apduReceived = io_recv_and_process_event();
        if (exitOnApdu && apduReceived) {
            return UX_SYNC_RET_APDU_RECEIVED;
        }
    }

    return g_ret;
}

/**
 * @brief Draws the extended version of home page of an app (page on which we land when launching it
 *        from dashboard) with automatic support of setting display.
 * @note it enables to use an action button (black on Stax, white on Flex)
 *
 * @param appName app name
 * @param appIcon app icon
 * @param tagline text under app name (if NULL, it will be "This app enables signing transactions on
 * the <appName> network.")
 * @param initSettingPage if not INIT_HOME_PAGE, start directly the corresponding setting page
 * @param settingContents setting contents to be displayed
 * @param infosList infos to be displayed (version, license, developer, ...)
 * @param action if not NULL, info used for an action button (on top of "Quit
 * App" button/footer)
 *
 * @return ret code:
 *         - UX_SYNC_RET_QUITTED
 *         - UX_SYNC_RET_APDU_RECEIVED
 */
ux_sync_ret_t ux_sync_homeAndSettings(const char                   *appName,
                                      const nbgl_icon_details_t    *appIcon,
                                      const char                   *tagline,
                                      const uint8_t                 initSettingPage,
                                      const nbgl_genericContents_t *settingContents,
                                      const nbgl_contentInfoList_t *infosList,
                                      const nbgl_homeAction_t      *action)
{
    ux_sync_init();
    nbgl_useCaseHomeAndSettings(appName,
                                appIcon,
                                tagline,
                                initSettingPage,
                                settingContents,
                                infosList,
                                action,
                                quit_callback);
    return ux_sync_wait(true);
}

/**
 * @brief Draws a flow of pages of a review. A back key is available on top-left of the screen,
 * except in first page It is possible to go to next page thanks to "tap to continue".
 * @note  All tag/value pairs are provided in the API and the number of pages is automatically
 * computed, the last page being a long press one
 *
 * @param operationType type of operation (Operation, Transaction, Message)
 * @param tagValueList list of tag/value pairs
 * @param icon icon used on first and last review page
 * @param reviewTitle string used in the first review page
 * @param reviewSubTitle string to set under reviewTitle (can be NULL)
 * @param finishTitle string used in the last review page
 *
 * @return ret code:
 *         - UX_SYNC_RET_APPROVED
 *         - UX_SYNC_RET_REJECTED
 */
ux_sync_ret_t ux_sync_review(nbgl_operationType_t              operationType,
                             const nbgl_contentTagValueList_t *tagValueList,
                             const nbgl_icon_details_t        *icon,
                             const char                       *reviewTitle,
                             const char                       *reviewSubTitle,
                             const char                       *finishTitle)
{
    ux_sync_init();
    nbgl_useCaseReview(operationType,
                       tagValueList,
                       icon,
                       reviewTitle,
                       reviewSubTitle,
                       finishTitle,
                       choice_callback);
    return ux_sync_wait(false);
}

/**
 * @brief Draws a flow of pages of a light review. A back key is available on top-left of the
 * screen,except in first page It is possible to go to next page thanks to "tap to continue".
 * @note  All tag/value pairs are provided in the API and the number of pages is automatically
 * computed, the last page being a short press one
 *
 * @param operationType type of operation (Operation, Transaction, Message)
 * @param tagValueList list of tag/value pairs
 * @param icon icon used on first and last review page
 * @param reviewTitle string used in the first review page
 * @param reviewSubTitle string to set under reviewTitle (can be NULL)
 * @param finishTitle string used in the last review page
 *
 * @return ret code:
 *         - UX_SYNC_RET_APPROVED
 *         - UX_SYNC_RET_REJECTED
 */
ux_sync_ret_t ux_sync_reviewLight(nbgl_operationType_t              operationType,
                                  const nbgl_contentTagValueList_t *tagValueList,
                                  const nbgl_icon_details_t        *icon,
                                  const char                       *reviewTitle,
                                  const char                       *reviewSubTitle,
                                  const char                       *finishTitle)
{
    ux_sync_init();
    nbgl_useCaseReviewLight(operationType,
                            tagValueList,
                            icon,
                            reviewTitle,
                            reviewSubTitle,
                            finishTitle,
                            choice_callback);
    return ux_sync_wait(false);
}
/**
 * @brief Draws a flow of pages of an extended address verification page.
 * A back key is available on top-left of the screen,
 * except in first page It is possible to go to next page thanks to "tap to continue".
 * @note  All tag/value pairs are provided in the API and the number of pages is automatically
 * computed, the last page being a long press one
 *
 * @param address address to confirm (NULL terminated string)
 * @param additionalTagValueList list of tag/value pairs (can be NULL) (must fit in a single page,
 * and be persistent because no copy)
 * @param callback callback called when button or footer is touched (if true, button, if false
 * footer)
 * @param icon icon used on the first review page
 * @param reviewTitle string used in the first review page
 * @param reviewSubTitle string to set under reviewTitle (can be NULL)
 *
 * @return ret code:
 *         - UX_SYNC_RET_APPROVED
 *         - UX_SYNC_RET_REJECTED
 */
ux_sync_ret_t ux_sync_addressReview(const char                       *address,
                                    const nbgl_contentTagValueList_t *additionalTagValueList,
                                    const nbgl_icon_details_t        *icon,
                                    const char                       *reviewTitle,
                                    const char                       *reviewSubTitle)
{
    ux_sync_init();
    nbgl_useCaseAddressReview(
        address, additionalTagValueList, icon, reviewTitle, reviewSubTitle, choice_callback);
    return ux_sync_wait(false);
}

/**
 * @brief Draws a transient (3s) status page for the reviewStatusType
 *
 * @param reviewStatusType type of status to display
 *
 * @return ret code:
 *         - UX_SYNC_RET_QUITTED
 */
ux_sync_ret_t ux_sync_reviewStatus(nbgl_reviewStatusType_t reviewStatusType)
{
    ux_sync_init();
    nbgl_useCaseReviewStatus(reviewStatusType, quit_callback);
    return ux_sync_wait(false);
}

/**
 * @brief Draws a transient (3s) status page, either of success or failure, with the given message
 *
 * @param message string to set in middle of page (Upper case for success)
 * @param isSuccess if true, message is drawn in a Ledger style (with corners)
 *
 * @return ret code:
 *         - UX_SYNC_RET_QUITTED
 */
ux_sync_ret_t ux_sync_status(const char *message, bool isSuccess)
{
    ux_sync_init();
    nbgl_useCaseStatus(message, isSuccess, quit_callback);
    return ux_sync_wait(false);
}

/**
 * @brief Start drawing the flow of pages of a review.
 * @note  This should be followed by calls to nbgl_useCaseReviewStreamingContinue and finally to
 *        nbgl_useCaseReviewStreamingFinish.
 *
 * @param operationType type of operation (Operation, Transaction, Message)
 * @param icon icon used on first and last review page
 * @param reviewTitle string used in the first review page
 * @param reviewSubTitle string to set under reviewTitle (can be NULL)
 *
 * @return ret code:
 *         - UX_SYNC_RET_APPROVED
 *         - UX_SYNC_RET_REJECTED
 */
ux_sync_ret_t ux_sync_reviewStreamingStart(nbgl_operationType_t       operationType,
                                           const nbgl_icon_details_t *icon,
                                           const char                *reviewTitle,
                                           const char                *reviewSubTitle)

{
    ux_sync_init();
    nbgl_useCaseReviewStreamingStart(
        operationType, icon, reviewTitle, reviewSubTitle, choice_callback);
    return ux_sync_wait(false);
}

/**
 * @brief Continue drawing the flow of pages of a review.
 * @note  This should be called after a call to nbgl_useCaseReviewStreamingStart and can be followed
 *        by others calls to nbgl_useCaseReviewStreamingContinue and finally to
 *        nbgl_useCaseReviewStreamingFinish.
 *
 * @param tagValueList list of tag/value pairs
 *
 * @return ret code:
 *         - UX_SYNC_RET_APPROVED
 *         - UX_SYNC_RET_REJECTED
 */
ux_sync_ret_t ux_sync_reviewStreamingContinue(const nbgl_contentTagValueList_t *tagValueList)

{
    ux_sync_init();
    nbgl_useCaseReviewStreamingContinue(tagValueList, choice_callback);
    return ux_sync_wait(false);
}

/**
 * @brief finish drawing the flow of pages of a review.
 * @note  This should be called after a call to nbgl_useCaseReviewStreamingContinue.
 *
 * @param finishTitle string used in the last review page
 *
 * @return ret code:
 *         - UX_SYNC_RET_APPROVED
 *         - UX_SYNC_RET_REJECTED
 */
ux_sync_ret_t ux_sync_reviewStreamingFinish(const char *finishTitle)

{
    ux_sync_init();
    nbgl_useCaseReviewStreamingFinish(finishTitle, choice_callback);
    return ux_sync_wait(false);
}

#endif
