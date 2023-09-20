/**
 * @file nbgl_step.c
 * @brief Implementation of predefined pages management for Applications
 */

#ifdef NBGL_STEP
/*********************
 *      INCLUDES
 *********************/
#include <string.h>
#include "nbgl_debug.h"
#include "nbgl_step.h"
#include "glyphs.h"
#include "os_pic.h"
#include "os_print.h"

/*********************
 *      DEFINES
 *********************/
// string to store the title for a multi-pages text+subtext
#define TMP_STRING_MAX_LEN 24

///< Maximum number of layers for steps, cannot be greater than max number of layout layers
#define NB_MAX_LAYERS 3

/**********************
 *      TYPEDEFS
 **********************/
/**
 * type of step
 */
typedef enum {
    TEXT_STEP = 0,       ///< for a simple text step
    CENTERED_INFO_STEP,  ///< for a centered info step
    MENU_LIST_STEP       ///< for a menu list step
} StepStype_t;

/**
 * definition of context for a @ref TEXT_STEP or @ref CENTERED_INFO_STEP step
 */
typedef struct TextContext_s {
    uint8_t             nbPages;        ///< number of pages for this text step
    uint8_t             currentPage;    ///< current page for this text step
    const char         *txtStart;       ///< pointer on the start point of text (first page)
    const char         *nextPageStart;  ///< pointer on the start point of text at the next page
    const char         *subTxtStart;    ///< pointer on the start point of sub-text (first page)
    nbgl_stepPosition_t pos;  ///< position of the step within a flow (used for navigation arrows)
    nbgl_stepButtonCallback_t onActionCallback;  ///< function called when key actions done on this
                                                 ///< step (other than internal navigation)
    char tmpString[TMP_STRING_MAX_LEN];          ///< temporary string used for title when text +
                                                 ///< multi-pages subText
    nbgl_centeredInfoStyle_t style;  ///< style to be used with a @ref CENTERED_INFO_STEP step
} TextContext_t;

/**
 * definition of context for a @ref MENU_LIST_STEP step
 */
typedef struct MenuListContext_s {
    nbgl_stepMenuListCallback_t
        selectedCallback;  ///< function to call when a menu list item is selected
    nbgl_layoutMenuList_t
        list;  ///< structure to store the number of menu items and the way to select them
} MenuListContext_t;

typedef struct StepContext_s {
    union {
        TextContext_t     textContext;  ///< if type is @ref TEXT_STEP or @ref CENTERED_INFO_STEP
        MenuListContext_t menuListContext;  ///< if type is @ref MENU_LIST_STEP
    };
    nbgl_screenTickerConfiguration_t ticker;  ///< structure containing information about ticker
    nbgl_layout_t                   *layout;  ///< handler of the used layout
    StepStype_t                      type;    ///< type of step
    bool                             modal;   ///< true if modal
} StepContext_t;

/**********************
 *  STATIC VARIABLES
 **********************/
///< array of step contexts. Index 0 is reserved for background
static StepContext_t contexts[NB_MAX_LAYERS];

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void actionCallback(nbgl_layout_t *layout, nbgl_buttonEvent_t event);
static void menuListActionCallback(nbgl_layout_t *layout, nbgl_buttonEvent_t event);

// returns a non-used step context from the contexts[] array, or NULL if not found
static StepContext_t *getFreeContext(StepStype_t type, bool modal)
{
    StepContext_t *ctx = NULL;

    if (!modal) {
        // Index 0 is reserved for background
        ctx = &contexts[0];
    }
    else {
        uint32_t i = 1;
        while (i < NB_MAX_LAYERS) {
            if (contexts[i].layout == NULL) {
                ctx = &contexts[i];
                break;
            }
            i++;
        }
    }
    if (ctx == NULL) {
        LOG_FATAL(STEP_LOGGER, "getFreeContext(): no available context\n");
    }
    else {
        memset(ctx, 0, sizeof(StepContext_t));
        ctx->type  = type;
        ctx->modal = modal;
    }
    return ctx;
}

// returns the step context from the contexts[] array matching with the given layout handler, or
// NULL if not found
static StepContext_t *getContextFromLayout(nbgl_layout_t layout)
{
    StepContext_t *ctx = NULL;
    uint32_t       i   = 0;
    while (i < NB_MAX_LAYERS) {
        if (contexts[i].layout == layout) {
            ctx = &contexts[i];
            break;
        }
        i++;
    }
    if (ctx == NULL) {
        LOG_WARN(STEP_LOGGER, "getContextFromLayout(): no matching context\n");
    }
    return ctx;
}

// from the current details context, return a pointer on the details at the given page
static const char *getTextPageAt(StepContext_t *ctx, uint8_t textPage)
{
    uint8_t     page        = 0;
    const char *currentChar = ctx->textContext.txtStart;
    while (page < textPage) {
        if (page < (ctx->textContext.nbPages - 1)) {
            uint16_t len;
            nbgl_getTextMaxLenInNbLines(BAGL_FONT_OPEN_SANS_REGULAR_11px_1bpp,
                                        currentChar,
                                        AVAILABLE_WIDTH,
                                        NB_MAX_LINES,
                                        &len,
                                        true);
            currentChar = currentChar + len;
        }
        page++;
    }
    return currentChar;
}

// from the current details context, return a pointer on the details at the given page, for subText
static const char *getSubTextPageAt(StepContext_t *ctx, uint8_t textPage)
{
    uint8_t     page        = 0;
    const char *currentChar = ctx->textContext.subTxtStart;
    while (page < textPage) {
        if (page < (ctx->textContext.nbPages - 1)) {
            uint16_t len;
            nbgl_getTextMaxLenInNbLines(BAGL_FONT_OPEN_SANS_REGULAR_11px_1bpp,
                                        currentChar,
                                        AVAILABLE_WIDTH,
                                        NB_MAX_LINES - 1,
                                        &len,
                                        true);
            currentChar = currentChar + len;
        }
        page++;
    }
    return currentChar;
}

// utility function to compute navigation arrows
static nbgl_layoutNavIndication_t getNavigationInfo(nbgl_stepPosition_t pos,
                                                    uint8_t             nbPages,
                                                    uint8_t             currentPage)
{
    nbgl_layoutNavIndication_t indication = NO_ARROWS;

    if (nbPages > 1) {
        if (currentPage > 0) {
            indication |= LEFT_ARROW;
        }
        if (currentPage < (nbPages - 1)) {
            indication |= RIGHT_ARROW;
        }
    }
    if (pos == FIRST_STEP) {
        indication |= RIGHT_ARROW;
    }
    else if (pos == LAST_STEP) {
        indication |= LEFT_ARROW;
    }
    else if (pos == NEITHER_FIRST_NOR_LAST_STEP) {
        indication |= RIGHT_ARROW | LEFT_ARROW;
    }
    return indication;
}

// function used to display the current page in details review mode
static void displayTextPage(StepContext_t *ctx, uint8_t textPage)
{
    const char *txt;

    // if move backward or first page
    if (textPage <= ctx->textContext.currentPage) {
        // recompute current start from beginning
        if (ctx->textContext.subTxtStart == NULL) {
            txt = getTextPageAt(ctx, textPage);
        }
        else {
            txt = getSubTextPageAt(ctx, textPage);
        }
    }
    // else move forward
    else {
        txt = ctx->textContext.nextPageStart;
    }
    ctx->textContext.currentPage = textPage;

    if (ctx->textContext.currentPage < (ctx->textContext.nbPages - 1)) {
        uint16_t len;
        uint8_t  nbLines
            = (ctx->textContext.subTxtStart == NULL) ? NB_MAX_LINES : (NB_MAX_LINES - 1);
        nbgl_getTextMaxLenInNbLines(
            BAGL_FONT_OPEN_SANS_REGULAR_11px_1bpp, txt, AVAILABLE_WIDTH, nbLines, &len, true);
        // memorize next position to save processing
        ctx->textContext.nextPageStart = txt + len;
    }
    else {
        ctx->textContext.nextPageStart = NULL;
    }
    nbgl_layoutDescription_t layoutDescription;
    nbgl_layoutNavigation_t  navInfo = {
         .direction = HORIZONTAL_NAV,
    };

    layoutDescription.modal                  = ctx->modal;
    layoutDescription.onActionCallback       = actionCallback;
    layoutDescription.ticker.tickerCallback  = ctx->ticker.tickerCallback;
    layoutDescription.ticker.tickerIntervale = ctx->ticker.tickerIntervale;
    layoutDescription.ticker.tickerValue     = ctx->ticker.tickerValue;
    ctx->layout                              = nbgl_layoutGet(&layoutDescription);

    navInfo.indication = getNavigationInfo(
        ctx->textContext.pos, ctx->textContext.nbPages, ctx->textContext.currentPage);

    if (ctx->textContext.subTxtStart == NULL) {
        nbgl_layoutAddText(ctx->layout, txt, NULL, ctx->textContext.style);
    }
    else {
        if (ctx->textContext.nbPages == 1) {
            nbgl_layoutAddText(ctx->layout, ctx->textContext.txtStart, txt, ctx->textContext.style);
        }
        else {
            SPRINTF(ctx->textContext.tmpString,
                    "%s (%d/%d)",
                    ctx->textContext.txtStart,
                    ctx->textContext.currentPage + 1,
                    ctx->textContext.nbPages);
            nbgl_layoutAddText(
                ctx->layout, ctx->textContext.tmpString, txt, ctx->textContext.style);
        }
    }
    if (navInfo.indication != NO_ARROWS) {
        nbgl_layoutAddNavigation(ctx->layout, &navInfo);
    }
    nbgl_layoutDraw(ctx->layout);
    nbgl_refresh();
}

// callback on key touch
static void actionCallback(nbgl_layout_t *layout, nbgl_buttonEvent_t event)
{
    StepContext_t *ctx = getContextFromLayout(layout);

    if (!ctx) {
        return;
    }
    if (event == BUTTON_LEFT_PRESSED) {
        if (ctx->textContext.currentPage > 0) {
            displayTextPage(ctx, ctx->textContext.currentPage - 1);
            return;
        }
        else if ((ctx->textContext.pos == LAST_STEP)
                 || (ctx->textContext.pos == NEITHER_FIRST_NOR_LAST_STEP)) {
            ctx->textContext.onActionCallback((nbgl_step_t) ctx, event);
        }
    }
    else if (event == BUTTON_RIGHT_PRESSED) {
        if (ctx->textContext.currentPage < (ctx->textContext.nbPages - 1)) {
            displayTextPage(ctx, ctx->textContext.currentPage + 1);
            return;
        }
        else if ((ctx->textContext.pos == FIRST_STEP)
                 || (ctx->textContext.pos == NEITHER_FIRST_NOR_LAST_STEP)) {
            ctx->textContext.onActionCallback((nbgl_step_t) ctx, event);
        }
    }
    else if (event == BUTTON_BOTH_PRESSED) {
        ctx->textContext.onActionCallback((nbgl_step_t) ctx, event);
    }
}

static void displayMenuList(StepContext_t *ctx)
{
    nbgl_layoutDescription_t layoutDescription
        = {.modal = ctx->modal, .onActionCallback = menuListActionCallback};
    nbgl_layoutMenuList_t *list = &ctx->menuListContext.list;

    layoutDescription.ticker.tickerCallback  = ctx->ticker.tickerCallback;
    layoutDescription.ticker.tickerIntervale = ctx->ticker.tickerIntervale;
    layoutDescription.ticker.tickerValue     = ctx->ticker.tickerValue;

    ctx->layout = nbgl_layoutGet(&layoutDescription);
    nbgl_layoutAddMenuList(ctx->layout, list);
    if (list->nbChoices > 1) {
        nbgl_layoutNavigation_t navInfo = {.direction = VERTICAL_NAV};
        navInfo.indication              = 0;
        if (list->selectedChoice > 0) {
            navInfo.indication |= LEFT_ARROW;
        }
        if (list->selectedChoice < (list->nbChoices - 1)) {
            navInfo.indication |= RIGHT_ARROW;
        }

        if (navInfo.indication != NO_ARROWS) {
            nbgl_layoutAddNavigation(ctx->layout, &navInfo);
        }
    }
    nbgl_layoutDraw(ctx->layout);
    nbgl_refresh();
}

// callback on key touch
static void menuListActionCallback(nbgl_layout_t *layout, nbgl_buttonEvent_t event)
{
    StepContext_t *ctx = getContextFromLayout(layout);
    if (!ctx) {
        return;
    }

    if (event == BUTTON_LEFT_PRESSED) {
        if (ctx->menuListContext.list.selectedChoice > 0) {
            ctx->menuListContext.list.selectedChoice--;
            displayMenuList(ctx);
        }
    }
    else if (event == BUTTON_RIGHT_PRESSED) {
        if (ctx->menuListContext.list.selectedChoice < (ctx->menuListContext.list.nbChoices - 1)) {
            ctx->menuListContext.list.selectedChoice++;
            displayMenuList(ctx);
        }
    }
    else if (event == BUTTON_BOTH_PRESSED) {
        ctx->menuListContext.selectedCallback(ctx->menuListContext.list.selectedChoice);
    }
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * @brief draws a text type step, that can be multi-pages, depending of the length of text and
 * subText. The navigation arrows are displayed depending of the given position of this step in a
 * flow, and depending of the page in case of multi-pages
 *
 * @param pos position of this step in the flow (first, last, single, not_first_nor_last)
 * @param onActionCallback common callback for all actions on this page
 * @param ticker ticker configuration, set to NULL to disable it
 * @param text text to display (depending of style)
 * @param subText text to display under text (depending of style)
 * @param style style to use for text and subText
 * @param modal if true, means this step shall be displayed on top of existing one
 * @return the step context (or NULL if error)
 */
nbgl_step_t nbgl_stepDrawText(nbgl_stepPosition_t               pos,
                              nbgl_stepButtonCallback_t         onActionCallback,
                              nbgl_screenTickerConfiguration_t *ticker,
                              const char                       *text,
                              const char                       *subText,
                              nbgl_centeredInfoStyle_t          style,
                              bool                              modal)
{
    StepContext_t *ctx = getFreeContext(TEXT_STEP, modal);
    if (!ctx) {
        return NULL;
    }
    // initialize context (already set to 0 by getFreeContext())
    ctx->textContext.onActionCallback = onActionCallback;
    if (ticker) {
        ctx->ticker.tickerCallback  = ticker->tickerCallback;
        ctx->ticker.tickerIntervale = ticker->tickerIntervale;
        ctx->ticker.tickerValue     = ticker->tickerValue;
    }

    // if no subText, get the number of pages for main text
    if (subText == NULL) {
        ctx->textContext.nbPages = nbgl_getTextNbPagesInWidth(
            BAGL_FONT_OPEN_SANS_REGULAR_11px_1bpp, text, NB_MAX_LINES, AVAILABLE_WIDTH);
    }
    else {
        // NB_MAX_LINES-1 because first line is for main text
        ctx->textContext.nbPages = nbgl_getTextNbPagesInWidth(
            BAGL_FONT_OPEN_SANS_REGULAR_11px_1bpp, subText, NB_MAX_LINES - 1, AVAILABLE_WIDTH);
    }
    LOG_DEBUG(STEP_LOGGER,
              "nbgl_stepDrawText: ctx = %p, nbPages = %d, pos = 0x%X\n",
              ctx,
              ctx->textContext.nbPages,
              pos);
    if (pos & BACKWARD_DIRECTION) {
        // start with last page
        ctx->textContext.currentPage = ctx->textContext.nbPages - 1;
    }
    ctx->textContext.txtStart    = text;
    ctx->textContext.subTxtStart = subText;
    // keep only direction part of position
    ctx->textContext.pos   = pos & (RIGHT_ARROW | LEFT_ARROW);
    ctx->textContext.style = style;
    displayTextPage(ctx, ctx->textContext.currentPage);

    return (nbgl_step_t) ctx;
}

/**
 * @brief draw a step with a centered info (icon + text). This is always a single page step
 *
 * @param pos position of this step in the flow (first, last, single, not_first_nor_last)
 * @param onActionCallback common callback for all actions on this page
 * @param ticker ticker configuration, set to NULL to disable it
 * @param info all information about the cenetered info to be displayed
 * @param modal if true, means this step shall be displayed on top of existing one
 * @return the step context (or NULL if error)
 */
nbgl_step_t nbgl_stepDrawCenteredInfo(nbgl_stepPosition_t               pos,
                                      nbgl_stepButtonCallback_t         onActionCallback,
                                      nbgl_screenTickerConfiguration_t *ticker,
                                      nbgl_layoutCenteredInfo_t        *info,
                                      bool                              modal)
{
    nbgl_layoutDescription_t layoutDescription
        = {.modal = modal, .onActionCallback = (nbgl_layoutButtonCallback_t) actionCallback};
    nbgl_layoutNavigation_t navInfo = {
        .direction = HORIZONTAL_NAV,
    };
    StepContext_t *ctx = getFreeContext(CENTERED_INFO_STEP, modal);
    if (!ctx) {
        return NULL;
    }

    // initialize context (already set to 0 by getFreeContext())
    ctx->textContext.onActionCallback = onActionCallback;
    if (ticker) {
        ctx->ticker.tickerCallback  = ticker->tickerCallback;
        ctx->ticker.tickerIntervale = ticker->tickerIntervale;
        ctx->ticker.tickerValue     = ticker->tickerValue;
    }

    ctx->textContext.nbPages = 1;
    // keep only direction part of position
    ctx->textContext.pos = pos & (RIGHT_ARROW | LEFT_ARROW);
    navInfo.indication   = getNavigationInfo(
        ctx->textContext.pos, ctx->textContext.nbPages, ctx->textContext.currentPage);

    ctx->layout = nbgl_layoutGet(&layoutDescription);
    nbgl_layoutAddCenteredInfo(ctx->layout, info);
    if (navInfo.indication != NO_ARROWS) {
        nbgl_layoutAddNavigation(ctx->layout, &navInfo);
    }
    nbgl_layoutDraw(ctx->layout);
    nbgl_refresh();

    LOG_DEBUG(STEP_LOGGER, "nbgl_stepDrawCenteredInfo(): step = %p\n", ctx);
    return (nbgl_step_t) ctx;
}

/**
 * @brief draw a step page with a menu list and navigation arrows to parse it. This step must be
 * alone
 *
 * @param onActionCallback common callback for all actions on this page
 * @param ticker ticker configuration, set to NULL to disable it
 * @param list configuration of the menu list
 * @param modal if true, means this step shall be displayed on top of existing one
 * @return the step context (or NULL if error)
 */
nbgl_step_t nbgl_stepDrawMenuList(nbgl_stepMenuListCallback_t       onActionCallback,
                                  nbgl_screenTickerConfiguration_t *ticker,
                                  nbgl_layoutMenuList_t            *list,
                                  bool                              modal)
{
    StepContext_t *ctx = getFreeContext(MENU_LIST_STEP, modal);
    if (!ctx) {
        return NULL;
    }

    // initialize context (already set to 0 by getFreeContext())
    if (ticker) {
        ctx->ticker.tickerCallback  = ticker->tickerCallback;
        ctx->ticker.tickerIntervale = ticker->tickerIntervale;
        ctx->ticker.tickerValue     = ticker->tickerValue;
    }

    ctx->menuListContext.list.nbChoices      = list->nbChoices;
    ctx->menuListContext.list.selectedChoice = list->selectedChoice;
    ctx->menuListContext.list.callback       = list->callback;
    ctx->menuListContext.selectedCallback    = onActionCallback;

    displayMenuList(ctx);

    LOG_DEBUG(STEP_LOGGER, "nbgl_stepDrawMenuList(): step = %p\n", ctx);

    return (nbgl_step_t) ctx;
}

/**
 * @brief Get the index of the currently selected item in the menulist
 *
 * @param step step from which to get the current menulist choice
 * @return current menulist choice
 */
uint8_t nbgl_stepGetMenuListCurrent(nbgl_step_t step)
{
    StepContext_t *ctx = (StepContext_t *) step;
    if (!ctx) {
        return 0;
    }
    return (ctx->menuListContext.list.selectedChoice);
}

/**
 * @brief Release the step obtained with any of the nbgl_stepDrawXXX() functions
 *
 * @param step step to release
 * @return >= 0 if OK
 */
int nbgl_stepRelease(nbgl_step_t step)
{
    StepContext_t *ctx = (StepContext_t *) step;
    int            ret;

    LOG_DEBUG(STEP_LOGGER, "nbgl_stepRelease(): ctx = %p\n", ctx);
    if (!ctx) {
        return -1;
    }
    ret = nbgl_layoutRelease((nbgl_layout_t *) ctx->layout);

    ctx->layout = NULL;

    return ret;
}

#endif  // NBGL_STEP
