
/*******************************************************************************
 *   Ledger Nano S - Secure firmware
 *   (c) 2022 Ledger
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 ********************************************************************************/

#pragma once

#include "os_seed.h"
#include "os_helpers.h"

/**
 * Define a flow step given both its error and validation flow.
 */
#define UX_LOC_STEP_FLOWS(                                                     \
    stepname, layoutkind, preinit, timeout_ms, validate_flow, error_flow, ...) \
    void stepname##_init(unsigned int stack_slot)                              \
    {                                                                          \
        preinit;                                                               \
        ux_layout_##layoutkind##_init(stack_slot);                             \
        ux_layout_set_timeout(stack_slot, timeout_ms);                         \
    }                                                                          \
    const ux_loc_layout_##layoutkind##_params_t stepname##_val = __VA_ARGS__;  \
    const ux_flow_step_t                        stepname       = {             \
        stepname##_init,                          \
        &stepname##_val,                          \
        validate_flow,                            \
        error_flow,                               \
    }

#define UX_LOC_STEP(stepname, layoutkind, preinit, timeout_ms, validate_cb, error_flow, ...) \
    UX_FLOW_CALL(stepname##_validate, { validate_cb; })                                      \
    void stepname##_init(unsigned int stack_slot)                                            \
    {                                                                                        \
        preinit;                                                                             \
        ux_layout_##layoutkind##_init(stack_slot);                                           \
        ux_layout_set_timeout(stack_slot, timeout_ms);                                       \
    }                                                                                        \
    const ux_loc_layout_##layoutkind##_params_t stepname##_val = __VA_ARGS__;                \
    const ux_flow_step_t                        stepname       = {                           \
        stepname##_init,                                        \
        &stepname##_val,                                        \
        stepname##_validate,                                    \
        error_flow,                                             \
    }

/**
 * Define a flow step with a validation callback
 */
#define UX_LOC_STEP_CB(stepname, layoutkind, validate_cb, ...)                \
    UX_FLOW_CALL(stepname##_validate, { validate_cb; })                       \
    const ux_loc_layout_##layoutkind##_params_t stepname##_val = __VA_ARGS__; \
    const ux_flow_step_t                        stepname       = {            \
        ux_layout_##layoutkind##_init,           \
        &stepname##_val,                         \
        stepname##_validate,                     \
        NULL,                                    \
    }
// deprecated
#define UX_LOC_FLOW_DEF_VALID UX_LOC_STEP_VALID
// deprecated
#define UX_LOC_STEP_VALID     UX_LOC_STEP_CB

/**
 * Define a flow step with a validation flow and error flow
 */
#define UX_LOC_STEP_FLOWCB(stepname, layoutkind, validate_flow, error_flow, ...) \
    const ux_loc_layout_##layoutkind##_params_t stepname##_val = __VA_ARGS__;    \
    const ux_flow_step_t                        stepname       = {               \
        ux_layout_##layoutkind##_init,              \
        &stepname##_val,                            \
        validate_flow,                              \
        error_flow,                                 \
    }

/**
 * Define a flow step with a validation callback and a preinit function to
 * render data into shared variable before screen is displayed
 */
#define UX_LOC_STEP_CB_INIT(stepname, layoutkind, preinit, validate_cb, ...)  \
    UX_FLOW_CALL(stepname##_validate, { validate_cb; })                       \
    void stepname##_init(unsigned int stack_slot)                             \
    {                                                                         \
        preinit;                                                              \
        ux_layout_##layoutkind##_init(stack_slot);                            \
    }                                                                         \
    const ux_loc_layout_##layoutkind##_params_t stepname##_val = __VA_ARGS__; \
    const ux_flow_step_t                        stepname       = {            \
        stepname##_init,                         \
        &stepname##_val,                         \
        stepname##_validate,                     \
        NULL,                                    \
    }

/**
 * Define a flow step with autovalidation after a given timeout (in ms)
 */
#define UX_LOC_STEP_TIMEOUT(stepname, layoutkind, timeout_ms, validate_flow, ...) \
    void stepname##_init(unsigned int stack_slot)                                 \
    {                                                                             \
        ux_layout_##layoutkind##_init(stack_slot);                                \
        ux_layout_set_timeout(stack_slot, timeout_ms);                            \
    }                                                                             \
    const ux_loc_layout_##layoutkind##_params_t stepname##_val = __VA_ARGS__;     \
    const ux_flow_step_t                        stepname       = {                \
        stepname##_init,                             \
        &stepname##_val,                             \
        validate_flow,                               \
        NULL,                                        \
    }

/**
 * Define a simple flow step, given its name, layout and content.
 */
#define UX_LOC_STEP_NOCB(stepname, layoutkind, ...)                           \
    const ux_loc_layout_##layoutkind##_params_t stepname##_val = __VA_ARGS__; \
    const ux_flow_step_t                        stepname       = {            \
        ux_layout_##layoutkind##_init,           \
        &stepname##_val,                         \
        NULL,                                    \
        NULL,                                    \
    }
// deprecated
#define UX_FLOW_DEF_NOCB UX_STEP_NOCB

/**
 * A Step with variant data (which could be overwritten by a flow idplsayed
 * in an higher flow and therefore need to be recomputed when step is redisplayed)
 */
#define UX_LOC_STEP_NOCB_INIT(stepname, layoutkind, preinit, ...)             \
    void stepname##_init(unsigned int stack_slot)                             \
    {                                                                         \
        preinit;                                                              \
        ux_layout_##layoutkind##_init(stack_slot);                            \
    }                                                                         \
    const ux_loc_layout_##layoutkind##_params_t stepname##_val = __VA_ARGS__; \
    const ux_flow_step_t                        stepname       = {            \
        stepname##_init,                         \
        &stepname##_val,                         \
        NULL,                                    \
        NULL,                                    \
    }

/**
 * A Step with variant data (which could be overwritten by a flow idplsayed
 * in an higher flow and therefore need to be recomputed when step is redisplayed)
 */
#define UX_LOC_STEP_NOCB_POSTINIT(stepname, layoutkind, postinit, ...)        \
    void stepname##_init(unsigned int stack_slot)                             \
    {                                                                         \
        ux_layout_##layoutkind##_init(stack_slot);                            \
        postinit;                                                             \
    }                                                                         \
    const ux_loc_layout_##layoutkind##_params_t stepname##_val = __VA_ARGS__; \
    const ux_flow_step_t                        stepname       = {            \
        stepname##_init,                         \
        &stepname##_val,                         \
        NULL,                                    \
        NULL,                                    \
    }

// Define a flow that will display several pages, using a specific formatting:
// ie \b to switch Bold On/Off, \n to force a new line  etc
#define UX_LOC_PAGES(stepname, index)                      \
    const ux_loc_layout_params_t stepname##_val = {index}; \
    const ux_flow_step_t         stepname       = {        \
        ux_layout_pages_init,                \
        &stepname##_val,                     \
        NULL,                                \
        NULL,                                \
    }
#define UX_LOC_PAGES_NOCB UX_LOC_PAGES

#define UX_LOC_PAGES_INIT(stepname, preinit, index)        \
    void stepname##_init(unsigned int stack_slot)          \
    {                                                      \
        preinit;                                           \
        ux_layout_pages_init(stack_slot);                  \
    }                                                      \
    const ux_loc_layout_params_t stepname##_val = {index}; \
    const ux_flow_step_t         stepname       = {        \
        stepname##_init,                     \
        &stepname##_val,                     \
        NULL,                                \
        NULL,                                \
    }

#define UX_LOC_PAGES_INIT_ERROR(stepname, preinit, error, index) \
    void stepname##_init(unsigned int stack_slot)                \
    {                                                            \
        preinit;                                                 \
        ux_layout_pages_init(stack_slot);                        \
    }                                                            \
    const ux_loc_layout_params_t stepname##_val = {index};       \
    const ux_flow_step_t         stepname       = {              \
        stepname##_init,                           \
        &stepname##_val,                           \
        NULL,                                      \
        error,                                     \
    }

#define UX_LOC_PAGES_CB(stepname, validate_cb, index)      \
    UX_FLOW_CALL(stepname##_validate, { validate_cb; })    \
    const ux_loc_layout_params_t stepname##_val = {index}; \
    const ux_flow_step_t         stepname       = {        \
        ux_layout_pages_init,                \
        &stepname##_val,                     \
        stepname##_validate,                 \
        NULL,                                \
    }

#define UX_LOC_PAGES_VALID UX_LOC_PAGES_CB

#define UX_LOC_PAGES_CB_INIT(stepname, preinit, validate_cb, index) \
    UX_FLOW_CALL(stepname##_validate, { validate_cb; })             \
    void stepname##_init(unsigned int stack_slot)                   \
    {                                                               \
        preinit;                                                    \
        ux_layout_pages_init(stack_slot);                           \
    }                                                               \
    const ux_loc_layout_params_t stepname##_val = {index};          \
    const ux_flow_step_t         stepname       = {                 \
        stepname##_init,                              \
        &stepname##_val,                              \
        stepname##_validate,                          \
        NULL,                                         \
    }

#define UX_LOC_PAGES_POSTINIT(stepname, postinit, index)   \
    void stepname##_init(unsigned int stack_slot)          \
    {                                                      \
        ux_layout_pages_init(stack_slot);                  \
        postinit;                                          \
    }                                                      \
    const ux_loc_layout_params_t stepname##_val = {index}; \
    const ux_flow_step_t         stepname       = {        \
        stepname##_init,                     \
        &stepname##_val,                     \
        NULL,                                \
        NULL,                                \
    }

#define UX_LOC_PAGES_TIMEOUT(stepname, timeout_ms, validate_cb, index) \
    void stepname##_init(unsigned int stack_slot)                      \
    {                                                                  \
        ux_layout_pages_init(stack_slot);                              \
        ux_layout_set_timeout(stack_slot, timeout_ms);                 \
    }                                                                  \
    const ux_loc_layout_params_t stepname##_val = {index};             \
    const ux_flow_step_t         stepname       = {                    \
        stepname##_init,                                 \
        &stepname##_val,                                 \
        validate_cb,                                     \
        NULL,                                            \
    }

// Define a flow that will display several pages, using a specific formatting:
// ie \b to switch Bold On/Off, \n to force a new line  etc with a pager
// The provided index will contain the title, and index+1 the remaining text
#define UX_LOC_PAGING(stepname, index)                     \
    const ux_loc_layout_params_t stepname##_val = {index}; \
    const ux_flow_step_t         stepname       = {        \
        ux_loc_layout_paging_init,           \
        &stepname##_val,                     \
        NULL,                                \
        NULL,                                \
    }

// Define a flow that will display several pages, using a specific formatting:
// ie \b to switch Bold On/Off, \n to force a new line  etc
#define UX_PAGES(stepname, text)                            \
    const ux_layout_pages_params_t stepname##_val = {text}; \
    const ux_flow_step_t           stepname       = {       \
        ux_layout_pages_init,               \
        &stepname##_val,                    \
        NULL,                               \
        NULL,                               \
    }

#define UX_PAGES_INIT(stepname, preinit, text)              \
    void stepname##_init(unsigned int stack_slot)           \
    {                                                       \
        preinit;                                            \
        ux_layout_pages_init(stack_slot);                   \
    }                                                       \
    const ux_layout_pages_params_t stepname##_val = {text}; \
    const ux_flow_step_t           stepname       = {       \
        stepname##_init,                    \
        &stepname##_val,                    \
        NULL,                               \
        NULL,                               \
    }

#define UX_PAGES_CB(stepname, validate_cb, text)            \
    UX_FLOW_CALL(stepname##_validate, { validate_cb; })     \
    const ux_layout_pages_params_t stepname##_val = {text}; \
    const ux_flow_step_t           stepname       = {       \
        ux_layout_pages_init,               \
        &stepname##_val,                    \
        stepname##_validate,                \
        NULL,                               \
    }

#define UX_PAGES_CB_INIT(stepname, preinit, validate_cb, text) \
    UX_FLOW_CALL(stepname##_validate, { validate_cb; })        \
    void stepname##_init(unsigned int stack_slot)              \
    {                                                          \
        preinit;                                               \
        ux_layout_pages_init(stack_slot);                      \
    }                                                          \
    const ux_layout_pages_params_t stepname##_val = {text};    \
    const ux_flow_step_t           stepname       = {          \
        stepname##_init,                       \
        &stepname##_val,                       \
        stepname##_validate,                   \
        NULL,                                  \
    }

#define UX_PAGES_POSTINIT(stepname, postinit, text)         \
    void stepname##_init(unsigned int stack_slot)           \
    {                                                       \
        ux_layout_pages_init(stack_slot);                   \
        postinit;                                           \
    }                                                       \
    const ux_layout_pages_params_t stepname##_val = {text}; \
    const ux_flow_step_t           stepname       = {       \
        stepname##_init,                    \
        &stepname##_val,                    \
        NULL,                               \
        NULL,                               \
    }

#define UX_PAGES_TIMEOUT(stepname, timeout_ms, validate_cb, text) \
    void stepname##_init(unsigned int stack_slot)                 \
    {                                                             \
        ux_layout_pages_init(stack_slot);                         \
        ux_layout_set_timeout(stack_slot, timeout_ms);            \
    }                                                             \
    const ux_layout_pages_params_t stepname##_val = {text};       \
    const ux_flow_step_t           stepname       = {             \
        stepname##_init,                          \
        &stepname##_val,                          \
        validate_cb,                              \
        NULL,                                     \
    }

#define UX_PAGES_NOCB UX_PAGES

// deprecated
#define UX_PAGES_VALID UX_PAGES_CB

#include "ux.h"
