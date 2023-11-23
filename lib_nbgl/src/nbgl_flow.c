/**
 * @file nbgl_flow.c
 * @brief Implementation of flow management
 */

#ifdef NBGL_STEP
/*********************
 *      INCLUDES
 *********************/
#include <string.h>
#include "nbgl_debug.h"
#include "nbgl_flow.h"
#include "glyphs.h"
#include "os_pic.h"
#include "ux.h"

/*********************
 *      DEFINES
 *********************/
///< Maximum number of layers for flow, cannot be greater than max number of step layers
#define NB_MAX_LAYERS 3

/**********************
 *      TYPEDEFS
 **********************/
typedef struct FlowContext_s {
    const nbgl_stepDesc_t *steps;
    uint8_t                curStep;
    uint8_t                nbSteps;
    bool                   loop;
    bool                   modal;
    nbgl_step_t            stepCtx;
} FlowContext_t;

/**********************
 *  STATIC VARIABLES
 **********************/
static FlowContext_t contexts[NB_MAX_LAYERS];

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void actionCallback(nbgl_step_t stepCtx, nbgl_buttonEvent_t event);

// returns a non-used flow context from the contexts[] array, or NULL if not found
static FlowContext_t *getFreeContext(bool modal)
{
    FlowContext_t *ctx = NULL;

    if (!modal) {
        // Index 0 is reserved for background
        ctx = &contexts[0];
    }
    else {
        uint32_t i = 1;
        while (i < NB_MAX_LAYERS) {
            if (contexts[i].stepCtx == NULL) {
                ctx = &contexts[i];
                break;
            }
            i++;
        }
    }
    if (ctx == NULL) {
        LOG_FATAL(FLOW_LOGGER, "getFreeContext(): no available context\n");
    }
    else {
        ctx->modal = modal;
    }
    return ctx;
}

// returns the flow context from the contexts[] array matching with the given step handler, or NULL
// if not found
static FlowContext_t *getContextFromStepCtx(nbgl_step_t stepCtx)
{
    FlowContext_t *ctx = NULL;
    uint32_t       i   = 0;
    while (i < NB_MAX_LAYERS) {
        if (contexts[i].stepCtx == stepCtx) {
            ctx = &contexts[i];
            break;
        }
        i++;
    }
    if (ctx == NULL) {
        LOG_WARN(FLOW_LOGGER, "getContextFromStepCtx(): no matching context\n");
    }
    return ctx;
}

// draws a step with the provided parameters, using the context provided as @ref ctx
static void drawStep(FlowContext_t             *ctx,
                     nbgl_stepPosition_t        pos,
                     bool                       modal,
                     const nbgl_icon_details_t *icon,
                     const char                *txt,
                     const char                *subTxt)
{
    nbgl_layoutCenteredInfo_t info;
    if ((ctx->loop) && (ctx->nbSteps > 1)) {
        pos |= NEITHER_FIRST_NOR_LAST_STEP;
    }
    else {
        pos |= GET_POS_OF_STEP(ctx->curStep, ctx->nbSteps);
    }

    if (icon == NULL) {
        ctx->stepCtx
            = nbgl_stepDrawText(pos, actionCallback, NULL, txt, subTxt, REGULAR_INFO, modal);
    }
    else {
        info.icon    = icon;
        info.text1   = txt;
        info.text2   = subTxt;
        info.onTop   = false;
        info.style   = REGULAR_INFO;
        ctx->stepCtx = nbgl_stepDrawCenteredInfo(pos, actionCallback, NULL, &info, modal);
    }
}

// function called on key action of the current step, if not an internal navigation in a multi-pages
// text step
static void actionCallback(nbgl_step_t stepCtx, nbgl_buttonEvent_t event)
{
    nbgl_stepPosition_t pos;
    FlowContext_t      *ctx = getContextFromStepCtx(stepCtx);

    if (!ctx) {
        return;
    }
    LOG_DEBUG(FLOW_LOGGER, "actionCallback: event = 0x%X, step = %d\n", event, ctx->curStep);
    // if navigation to the previous step
    if ((event == BUTTON_LEFT_PRESSED) && (ctx->curStep > 0)) {
        ctx->curStep--;
        pos = BACKWARD_DIRECTION;
    }
    // if navigation to the next step
    else if ((event == BUTTON_RIGHT_PRESSED) && (ctx->curStep < (int) (ctx->nbSteps - 1))) {
        ctx->curStep++;
        pos = FORWARD_DIRECTION;
    }
    // if action on the current step
    else if (event == BUTTON_BOTH_PRESSED) {
        if (ctx->steps[ctx->curStep].callback != NULL) {
            ctx->steps[ctx->curStep].callback();
        }
        return;
    }
    else {
        return;
    }
    const nbgl_stepDesc_t *step = &ctx->steps[ctx->curStep];
#ifdef HAVE_LANGUAGE_PACK
    const char *txt = (step->text != NULL)
                          ? step->text
                          : ((step->textId != INVALID_ID) ? get_ux_loc_string(step->textId) : NULL);
#else   // HAVE_LANGUAGE_PACK
    const char *txt = step->text;
#endif  // HAVE_LANGUAGE_PACK
    // release the current step before opening new one
    nbgl_stepRelease((nbgl_step_t) ctx->stepCtx);
    if (step->init != NULL) {
        step->init();
    }
    drawStep(ctx, pos, ctx->modal, step->icon, txt, step->subText);
    nbgl_refresh();
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * @brief draw the given flow, starting at the given step
 *
 * @param steps array of step descriptions
 * @param nbSteps number of steps in above array
 * @param initStep init step in above array
 * @param loop if true, loop between last and first step, in both direction
 * @param modal if true, screens are modal
 * @return >= 0 if OK, < 0 otherwise
 */
nbgl_flow_t nbgl_flowDraw(const nbgl_stepDesc_t *steps,
                          uint8_t                nbSteps,
                          uint8_t                initStep,
                          bool                   loop,
                          bool                   modal)
{
    const nbgl_stepDesc_t *step = &steps[initStep];
#ifdef HAVE_LANGUAGE_PACK
    const char *txt = (step->text != NULL)
                          ? step->text
                          : ((step->textId != INVALID_ID) ? get_ux_loc_string(step->textId) : NULL);
#else   // HAVE_LANGUAGE_PACK
    const char *txt = step->text;
#endif  // HAVE_LANGUAGE_PACK
    nbgl_stepPosition_t pos = FORWARD_DIRECTION;
    FlowContext_t      *ctx = getFreeContext(modal);

    if (!ctx) {
        return NULL;
    }

    ctx->nbSteps = nbSteps;
    ctx->curStep = initStep;
    ctx->steps   = steps;
    ctx->loop    = loop;
    if (step->init != NULL) {
        step->init();
    }

    drawStep(ctx, pos, ctx->modal, step->icon, txt, step->subText);
    nbgl_refresh();
    return (nbgl_flow_t) ctx;
}

/**
 * @brief release the given flow
 *
 * @param flow flow to release
 */
void nbgl_flowRelease(nbgl_flow_t flow)
{
    FlowContext_t *ctx = (FlowContext_t *) flow;

    if (!ctx) {
        LOG_WARN(FLOW_LOGGER, "nbgl_flowRelease: NULL context!");
        return;
    }
    nbgl_stepRelease(ctx->stepCtx);
    ctx->stepCtx = NULL;
}
#endif  // NBGL_STEP
