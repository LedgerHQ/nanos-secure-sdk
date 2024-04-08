#ifdef HAVE_NBGL

#include "nbgl_use_case.h"

typedef enum {
    UX_SYNC_RET_APPROVED,
    UX_SYNC_RET_REJECTED,
    UX_SYNC_RET_QUITTED,
    UX_SYNC_RET_APDU_RECEIVED,
    UX_SYNC_RET_ERROR
} ux_sync_ret_t;

ux_sync_ret_t ux_sync_homeAndSettings(const char                   *appName,
                                      const nbgl_icon_details_t    *appIcon,
                                      const char                   *tagline,
                                      const uint8_t                 initSettingPage,
                                      const nbgl_genericContents_t *settingContents,
                                      const nbgl_contentInfoList_t *infosList,
                                      const nbgl_homeAction_t      *action);

ux_sync_ret_t ux_sync_review(nbgl_operationType_t             operationType,
                             const nbgl_layoutTagValueList_t *tagValueList,
                             const nbgl_icon_details_t       *icon,
                             const char                      *reviewTitle,
                             const char                      *reviewSubTitle,
                             const char                      *finishTitle);

ux_sync_ret_t ux_sync_addressReview(const char                      *address,
                                    const nbgl_layoutTagValueList_t *additionalTagValueList,
                                    const nbgl_icon_details_t       *icon,
                                    const char                      *reviewTitle,
                                    const char                      *reviewSubTitle);

ux_sync_ret_t ux_sync_reviewStatus(nbgl_reviewStatusType_t reviewStatusType);

ux_sync_ret_t ux_sync_status(const char *message, bool isSuccess);

ux_sync_ret_t ux_sync_reviewStreamingStart(nbgl_operationType_t       operationType,
                                           const nbgl_icon_details_t *icon,
                                           const char                *reviewTitle,
                                           const char                *reviewSubTitle);

ux_sync_ret_t ux_sync_reviewStreamingContinue(const nbgl_layoutTagValueList_t *tagValueList);

ux_sync_ret_t ux_sync_reviewStreamingFinish(const char *finishTitle);

ux_sync_ret_t ux_sync_genericReview(const nbgl_genericContents_t *contents, const char *rejectText);

ux_sync_ret_t ux_sync_genericConfiguration(const char                   *title,
                                           uint8_t                       initPage,
                                           const nbgl_genericContents_t *contents);

/*
 * This function must be implemented by the caller.
 * It must wait for the next seph event and process it except for APDU events.
 * It must return:
 * - true when an APDU has been received in the processed event
 * - false otherwise
 *
 * Note on C apps using SDK lib_standard_app, this is already provided in io.c by the lib.
 */
extern bool io_recv_and_process_event(void);

#endif
