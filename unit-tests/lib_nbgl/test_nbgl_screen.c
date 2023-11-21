#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <string.h>

#include <cmocka.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include "nbgl_screen.h"
#include "nbgl_debug.h"
#include "ux_loc.h"

#define UNUSED(x) (void) x

unsigned long gLogger = 0;

void mainExit(int exitCode)
{
    exit(exitCode);
}

uint8_t touch_exclude_borders(uint8_t excluded_borders)
{
    return excluded_borders;
}

void nbgl_screen_reinit(void)
{
    return;
}

void nbgl_redrawObject(nbgl_obj_t *obj, nbgl_obj_t *prevObj, bool computePosition)
{
    UNUSED(obj);
    UNUSED(prevObj);
    UNUSED(computePosition);
}

static void test_push_pop(void **state __attribute__((unused)))
{
    nbgl_obj_t   **elements0, **elements1, **elements2, **elements3;
    nbgl_screen_t *screen;
    uint8_t        screen1, screen2, screen3;

    // assert no screen at start-up
    assert_null(nbgl_screenGetTop());

    // add a first screen and assert the top is not null anymore
    nbgl_screenSet(&elements0, 4, NULL, NULL);
    assert_non_null(nbgl_screenGetTop());

    // push a second screen and assert its index is 1, and that it is on top of stack
    screen1 = nbgl_screenPush(&elements1, 1, NULL, NULL);
    assert_int_equal(screen1, 1);
    screen = (nbgl_screen_t *) nbgl_screenGetTop();
    assert_int_equal(screen->container.nbChildren, 1);
    assert_ptr_equal(screen->container.children, nbgl_screenGetElements(screen1));

    // push a third screen and assert its index is 2, and that it is on top of stack
    screen2 = nbgl_screenPush(&elements2, 2, NULL, NULL);
    assert_int_equal(screen2, 2);
    screen = (nbgl_screen_t *) nbgl_screenGetTop();
    assert_int_equal(screen->container.nbChildren, 2);
    assert_ptr_equal(screen->container.children, nbgl_screenGetElements(screen2));

    // push a fourth screen and assert its index is 3, and that it is on top of stack
    screen3 = nbgl_screenPush(&elements3, 3, NULL, NULL);
    assert_int_equal(screen3, 3);
    screen = (nbgl_screen_t *) nbgl_screenGetTop();
    assert_int_equal(screen->container.nbChildren, 3);
    assert_ptr_equal(screen->container.children, nbgl_screenGetElements(screen3));

    // pop the fourth screen and assert the third one is on top of stack
    nbgl_screenPop(screen3);
    screen = (nbgl_screen_t *) nbgl_screenGetTop();
    assert_int_equal(screen->container.nbChildren, 2);
    assert_ptr_equal(screen->container.children, nbgl_screenGetElements(screen2));

    // pop the second screen and assert the third one is on top of stack
    nbgl_screenPop(screen1);
    screen = (nbgl_screen_t *) nbgl_screenGetTop();
    assert_int_equal(screen->container.nbChildren, 2);
    assert_ptr_equal(screen->container.children, nbgl_screenGetElements(screen2));

    // push a screen and assert its index is 1 (first unused index)
    screen3 = nbgl_screenPush(&elements3, 3, NULL, NULL);
    assert_int_equal(screen3, 1);
}

int main(int argc, char **argv)
{
    const struct CMUnitTest tests[] = {cmocka_unit_test(test_push_pop)};
    return cmocka_run_group_tests(tests, NULL, NULL);
}
