/**
 * @file nbgl_content.h
 * @brief common content for Graphical Library
 *
 */

#ifndef NBGL_CONTENT_H
#define NBGL_CONTENT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#include <stdbool.h>

#include "bolos_target.h"

#ifdef TARGET_NANOS
#include "nbgl_lns.h"
#else
#include "nbgl_types.h"
#include "nbgl_obj.h"
#ifdef HAVE_PIEZO_SOUND
#include "os_io_seproxyhal.h"
#endif
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**
 * @brief possible styles for Centered Info Area
 *
 */
typedef enum {
#ifdef HAVE_SE_TOUCH
    LARGE_CASE_INFO,  ///< text in BLACK and large case (INTER 32px), subText in black in Inter24px
    LARGE_CASE_BOLD_INFO,  ///< text in BLACK and large case (INTER 32px), subText in black bold
                           ///< Inter24px, text3 in black Inter24px
    LARGE_CASE_GRAY_INFO,  ///< text in BLACK and large case (INTER 32px), subText in black
                           ///< Inter24px text3 in dark gray Inter24px
    NORMAL_INFO,  ///< Icon in black, a potential text in black bold 24px under it, a potential text
                  ///< in dark gray (24px) under it, a potential text in black (24px) under it
    PLUGIN_INFO   ///< A potential text in black 32px, a potential text in black (24px) under it, a
                 ///< small horizontal line under it, a potential icon under it, a potential text in
                 ///< black (24px) under it
#else   // HAVE_SE_TOUCH
    REGULAR_INFO = 0,  ///< both texts regular (but '\\b' can switch to bold)
    BOLD_TEXT1_INFO    ///< bold is used for text1 (but '\\b' can switch to regular)
#endif  // HAVE_SE_TOUCH
} nbgl_contentCenteredInfoStyle_t;

/**
 * @brief This structure contains info to build a centered (vertically and horizontally) area, with
 * a possible Icon, a possible text under it, and a possible sub-text gray under it.
 *
 */
typedef struct {
    const char *text1;  ///< first text (can be null)
    const char *text2;  ///< second text (can be null)
#ifdef HAVE_SE_TOUCH
    const char *text3;                      ///< third text (can be null)
#endif                                      // HAVE_SE_TOUCH
    const nbgl_icon_details_t      *icon;   ///< a buffer containing the 1BPP icon
    bool                            onTop;  ///< if set to true, align only horizontally
    nbgl_contentCenteredInfoStyle_t style;  ///< style to apply to this info
#ifdef HAVE_SE_TOUCH
    int16_t offsetY;  ///< vertical shift to apply to this info (if >0, shift to bottom)
#endif                // HAVE_SE_TOUCH
} nbgl_contentCenteredInfo_t;

/**
 * @brief This structure contains data to build a centered info + long press button content
 */
typedef struct {
    const char                *text;           ///< centered text in large case
    const nbgl_icon_details_t *icon;           ///< a buffer containing the 1BPP icon
    const char                *longPressText;  ///< text of the long press button
    uint8_t longPressToken;  ///< the token used as argument of the onActionCallback when button is
                             ///< long pressed
#ifdef HAVE_PIEZO_SOUND
    tune_index_e
        tuneId;  ///< if not @ref NBGL_NO_TUNE, a tune will be played when button is touched
#endif
} nbgl_contentInfoLongPress_t;

/**
 * @brief This structure contains data to build a centered info + simple black button content
 */
typedef struct {
    const char                *text;        ///< centered text in large case
    const nbgl_icon_details_t *icon;        ///< a buffer containing the 1BPP icon
    const char                *buttonText;  ///< text of the long press button
    uint8_t buttonToken;  ///< the token used as argument of the onActionCallback when button is
                          ///< long pressed
#ifdef HAVE_PIEZO_SOUND
    tune_index_e
        tuneId;  ///< if not @ref NBGL_NO_TUNE, a tune will be played when button is touched
#endif
} nbgl_contentInfoButton_t;

/**
 * @brief This structure contains a [tag,value] pair
 */
typedef struct {
    const char *item;   ///< string giving the tag name
    const char *value;  ///< string giving the value name
#ifdef SCREEN_SIZE_WALLET
    const nbgl_icon_details_t *valueIcon;  ///< a buffer containing the 32px 1BPP icon for icon on
                                           ///< right of value (can be NULL)
    int8_t forcePageStart : 1;  ///< if set to 1, the tag will be displayed at the top of a new
                                ///< review page
    int8_t centeredInfo : 1;    ///< if set to 1, the tag will be displayed as a centered info
#endif
} nbgl_contentTagValue_t;

/**
 * @brief prototype of tag/value pair retrieval callback
 * @param pairIndex index of the tag/value pair to retrieve (from 0 (to nbPairs-1))
 * @return a pointer on a static tag/value pair
 */
typedef nbgl_contentTagValue_t *(*nbgl_contentTagValueCallback_t)(uint8_t pairIndex);

/**
 * @brief This structure contains a list of [tag,value] pairs
 */
typedef struct {
    const nbgl_contentTagValue_t
        *pairs;  ///< array of [tag,value] pairs (nbPairs items). If NULL, callback is used instead
    nbgl_contentTagValueCallback_t callback;  ///< function to call to retrieve a given pair
    uint8_t nbPairs;  ///< number of pairs in pairs array (or max number of pairs to retrieve with
                      ///< callback)
    uint8_t startIndex;          ///< index of the first pair to get with callback
    uint8_t nbMaxLinesForValue;  ///< if > 0, set the max number of lines for value field. And the
                                 ///< last line is ended with "..." instead of the 3 last chars
    uint8_t token;  ///< the token that will be used as argument of the callback if icon in any
                    ///< tag/value pair is touched (index is the index of the pair in pairs[])
    bool smallCaseForValue;  ///< if set to true, a 24px font is used for value text, otherwise a
                             ///< 32px font is used
    bool wrapping;  ///< if set to true, value text will be wrapped on ' ' to avoid cutting words
} nbgl_contentTagValueList_t;

/**
 * @brief This structure contains a [item,value] pair and info about "details" button
 */
typedef struct {
    nbgl_contentTagValueList_t tagValueList;       ///< list of tag/value pairs
    const nbgl_icon_details_t *detailsButtonIcon;  ///< icon to use in details button
    const char                *detailsButtonText;  ///< this text is used for "details" button
    uint8_t detailsButtonToken;  ///< the token used as argument of the actionCallback when the
                                 ///< "details" button is touched
#ifdef HAVE_PIEZO_SOUND
    tune_index_e
        tuneId;  ///< if not @ref NBGL_NO_TUNE, a tune will be played when details button is touched
#endif
} nbgl_contentTagValueDetails_t;

/**
 * @brief This structure contains [item,value] pair(s) and info about a potential "details" button,
 * but also a black button + footer to confirm/cancel
 */
typedef struct {
    nbgl_contentTagValueList_t tagValueList;       ///< list of tag/value pairs
    const nbgl_icon_details_t *detailsButtonIcon;  ///< icon to use in details button
    const char *detailsButtonText;  ///< this text is used for "details" button (if NULL, no button)
    uint8_t     detailsButtonToken;  ///< the token used as argument of the actionCallback when the
                                     ///< "details" button is touched
#ifdef HAVE_PIEZO_SOUND
    tune_index_e
        tuneId;  ///< if not @ref NBGL_NO_TUNE, a tune will be played when details button is touched
#endif
    const char
        *confirmationText;  ///< text of the confirmation button, if NULL "It matches" is used
    const char
           *cancelText;  ///< the text used for cancel action, if NULL "It doesn't matches" is used
    uint8_t confirmationToken;  ///< the token used as argument of the onActionCallback
    uint8_t cancelToken;  ///< the token used as argument of the onActionCallback when the cancel
                          ///< button is pressed
} nbgl_contentTagValueConfirm_t;

/**
 * @brief This structure contains info to build a switch (on the right) with a description (on the
 * left), with a potential sub-description (in gray)
 *
 */
typedef struct {
    const char *text;  ///< main text for the switch
    const char
        *subText;  ///< description under main text (NULL terminated, single line, may be null)
    nbgl_state_t initState;  ///< initial state of the switch
    uint8_t      token;      ///< the token that will be used as argument of the callback
#ifdef HAVE_PIEZO_SOUND
    tune_index_e tuneId;  ///< if not @ref NBGL_NO_TUNE, a tune will be played
#endif                    // HAVE_PIEZO_SOUND
} nbgl_contentSwitch_t;

/**
 * @brief This structure contains data to build a @ref SWITCHES_LIST content
 */
typedef struct nbgl_pageSwitchesList_s {
    const nbgl_contentSwitch_t *switches;    ///< array of switches (nbSwitches items)
    uint8_t                     nbSwitches;  ///< number of elements in switches and tokens array
} nbgl_contentSwitchesList_t;

/**
 * @brief This structure contains data to build a @ref INFOS_LIST content
 */
typedef struct {
    const char *const *infoTypes;     ///< array of types of infos (in black/bold)
    const char *const *infoContents;  ///< array of contents of infos (in black)
    uint8_t            nbInfos;       ///< number of elements in infoTypes and infoContents array
} nbgl_contentInfoList_t;

/**
 * @brief This structure contains a list of names to build a list of radio
 * buttons (on the right part of screen), with for each a description (names array)
 * The chosen item index is provided is the "index" argument of the callback
 */
typedef struct {
    union {
        const char *const *names;  ///< array of strings giving the choices (nbChoices)
#if defined(HAVE_LANGUAGE_PACK)
        UX_LOC_STRINGS_INDEX *nameIds;  ///< array of string Ids giving the choices (nbChoices)
#endif                                  // HAVE_LANGUAGE_PACK
    };
    bool    localized;   ///< if set to true, use nameIds and not names
    uint8_t nbChoices;   ///< number of choices
    uint8_t initChoice;  ///< index of the current choice
    uint8_t token;       ///< the token that will be used as argument of the callback
#ifdef HAVE_PIEZO_SOUND
    tune_index_e
        tuneId;  ///< if not @ref NBGL_NO_TUNE, a tune will be played when selecting a radio button)
#endif           // HAVE_PIEZO_SOUND
} nbgl_contentRadioChoice_t;

/**
 * @brief This structure contains data to build a @ref BARS_LIST content
 */
typedef struct {
    const char *const *barTexts;  ///< array of texts for each bar (nbBars items, in black/bold)
    const uint8_t     *tokens;    ///< array of tokens, one for each bar (nbBars items)
    uint8_t            nbBars;    ///< number of elements in barTexts and tokens array
#ifdef HAVE_PIEZO_SOUND
    tune_index_e tuneId;  ///< if not @ref NBGL_NO_TUNE, a tune will be played when a bar is touched
#endif                    // HAVE_PIEZO_SOUND
} nbgl_contentBarsList_t;

/**
 * @brief The different types of predefined contents
 *
 */
typedef enum {
    CENTERED_INFO = 0,  ///< a centered info
    INFO_LONG_PRESS,    ///< a centered info and a long press button
    INFO_BUTTON,        ///< a centered info and a simple black button
    TAG_VALUE_LIST,     ///< list of tag/value pairs
    TAG_VALUE_DETAILS,  ///< a tag/value pair and a small button to get details.
    TAG_VALUE_CONFIRM,  ///< tag/value pairs and a black button/footer to confirm/cancel.
    SWITCHES_LIST,      ///< list of switches with descriptions
    INFOS_LIST,         ///< list of infos with titles
    CHOICES_LIST,       ///< list of choices through radio buttons
    BARS_LIST           ///< list of touchable bars (with > on the right to go to sub-pages)
} nbgl_contentType_t;

/**
 * @brief Union of the different type of contents
 */
typedef union {
    nbgl_contentCenteredInfo_t    centeredInfo;     ///< @ref CENTERED_INFO type
    nbgl_contentInfoLongPress_t   infoLongPress;    ///< @ref INFO_LONG_PRESS type
    nbgl_contentInfoButton_t      infoButton;       ///< @ref INFO_BUTTON type
    nbgl_contentTagValueList_t    tagValueList;     ///< @ref TAG_VALUE_LIST type
    nbgl_contentTagValueDetails_t tagValueDetails;  ///< @ref TAG_VALUE_DETAILS type
    nbgl_contentTagValueConfirm_t tagValueConfirm;  ///< @ref TAG_VALUE_CONFIRM type
    nbgl_contentSwitchesList_t    switchesList;     ///< @ref SWITCHES_LIST type
    nbgl_contentInfoList_t        infosList;        ///< @ref INFOS_LIST type
    nbgl_contentRadioChoice_t     choicesList;      ///< @ref CHOICES_LIST type
    nbgl_contentBarsList_t        barsList;         ///< @ref BARS_LIST type
} nbgl_content_u;

/**
 * @brief prototype of function to be called when an action on a content object occurs
 * @param token integer passed at content object initialization
 * @param index when the object touched is a list of radio buttons, gives the index of the activated
 * @param page index of the current page, can be used to restart the use_case directly at the right
 * page button
 */
typedef void (*nbgl_contentActionCallback_t)(int token, uint8_t index, int page);

/**
 * @brief This structure contains data to build a content
 */
typedef struct {
    nbgl_contentType_t type;  ///< type of page content in the content union
    nbgl_content_u     content;
    nbgl_contentActionCallback_t
        contentActionCallback;  ///< callback to be called when an action on an object occurs
} nbgl_content_t;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NBGL_CONTENT_H */
