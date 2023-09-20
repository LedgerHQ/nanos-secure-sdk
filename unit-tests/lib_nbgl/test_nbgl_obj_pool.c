#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <string.h>

#include <cmocka.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include "nbgl_obj.h"
#include "nbgl_debug.h"
#include "ux_loc.h"

#define UNUSED(x) (void) x

unsigned long gLogger = 0;

static bool fatal_reached = false;

void mainExit(int exitCode)
{
    fatal_reached = true;
}

static void test_alloc_obj(void **state __attribute__((unused)))
{
    nbgl_obj_t *obj1, *obj2, *objArray[100];

    obj1 = nbgl_objPoolGet(IMAGE, 0);
    assert_non_null(obj1);
    assert_int_equal(obj1->type, IMAGE);
    obj2 = nbgl_objPoolGet(TEXT_AREA, 0);
    assert_non_null(obj2);
    assert_int_equal(obj2->type, TEXT_AREA);

    assert_ptr_equal(obj1, nbgl_objPoolGetPrevious(obj2, 0));

    assert_int_equal(nbgl_objPoolGetArray(IMAGE, 10, 0, objArray), 0);

    assert_int_equal(nbgl_objPoolGetNbUsed(0), 12);
    nbgl_objPoolRelease(0);
    assert_int_equal(nbgl_objPoolGetNbUsed(0), 0);

    // try to allocate too many objects
    nbgl_objPoolGetArray(IMAGE, 100, 0, objArray);
    assert_true(fatal_reached);
    fatal_reached = false;
}

static void test_alloc_container(void **state __attribute__((unused)))
{
    nbgl_obj_t **container1, **container2;

    container1 = nbgl_containerPoolGet(10, 0);
    assert_non_null(container1);

    container2 = nbgl_containerPoolGet(12, 0);
    assert_non_null(container2);

    assert_int_equal(nbgl_containerPoolGetNbUsed(0), 22);
    nbgl_containerPoolRelease(0);
    assert_int_equal(nbgl_containerPoolGetNbUsed(0), 0);

    // try to allocate too many containers
    nbgl_containerPoolGet(200, 0);
    assert_true(fatal_reached);
    fatal_reached = false;
}

int main(int argc, char **argv)
{
    const struct CMUnitTest tests[]
        = {cmocka_unit_test(test_alloc_obj), cmocka_unit_test(test_alloc_container)};
    return cmocka_run_group_tests(tests, NULL, NULL);
}
