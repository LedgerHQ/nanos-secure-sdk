cmake_minimum_required(VERSION 3.10)

if(${CMAKE_VERSION} VERSION_LESS 3.10)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

# project information
project(unit_tests
        VERSION 0.1
	      DESCRIPTION "Unit tests for NBGL"
        LANGUAGES C)


# guard against bad build-type strings
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

include(CTest)
ENABLE_TESTING()

# specify C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
if(DEFINED ENV{STAX})
add_compile_definitions(HAVE_BAGL_FONT_INTER_REGULAR_24PX)
add_compile_definitions(HAVE_BAGL_FONT_INTER_SEMIBOLD_24PX)
add_compile_definitions(HAVE_BAGL_FONT_INTER_MEDIUM_32PX)
add_compile_definitions(HAVE_BAGL_FONT_HMALPHAMONO_MEDIUM_32PX)
add_compile_definitions(HAVE_SE_TOUCH)
add_compile_definitions(SCREEN_SIZE_WALLET)
else()
add_compile_definitions(HAVE_BAGL_FONT_OPEN_SANS_REGULAR_11PX)
add_compile_definitions(HAVE_BAGL_FONT_OPEN_SANS_EXTRABOLD_11PX)
add_compile_definitions(HAVE_BAGL_FONT_OPEN_SANS_LIGHT_16PX)
add_compile_definitions(SCREEN_SIZE_NANO)
endif()
add_compile_definitions(HAVE_LANGUAGE_PACK)
add_compile_definitions(HAVE_UNICODE_SUPPORT)
add_compile_definitions(USB_SEGMENT_SIZE=64)
add_compile_definitions(HAVE_NBGL)
add_compile_definitions(WITH_STDIO)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall ${DEFINES} -g -O0 --coverage")

set(GCC_COVERAGE_LINK_FLAGS "--coverage -lgcov")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")

# guard against in-source builds
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there. You may need to remove CMakeCache.txt. ")
endif()

add_compile_definitions(TEST)

include_directories(.)
if(DEFINED ENV{STAX})
include_directories(../../target/stax/include)
else()
include_directories(../../target/nanox/include)
endif()
include_directories(../../include)
include_directories(../../lib_nbgl/include)
include_directories(../../lib_nbgl/include/fonts)
include_directories(../../lib_ux_nbgl)

add_executable(test_nbgl_fonts test_nbgl_fonts.c)
add_executable(test_nbgl_obj_pool test_nbgl_obj_pool.c)
add_executable(test_nbgl_screen test_nbgl_screen.c)
add_executable(test_nbgl_obj test_nbgl_obj.c)

add_library(nbgl_stubs SHARED nbgl_stubs.c)
add_library(nbgl_fonts SHARED ../../lib_nbgl/src/nbgl_fonts.c)
add_library(nbgl_obj_pool SHARED ../../lib_nbgl/src/nbgl_obj_pool.c)
add_library(nbgl_screen SHARED ../../lib_nbgl/src/nbgl_screen.c)
add_library(nbgl_obj SHARED ../../lib_nbgl/src/nbgl_obj.c)

target_link_libraries(test_nbgl_fonts PUBLIC cmocka gcov nbgl_fonts nbgl_stubs)
target_link_libraries(test_nbgl_obj_pool PUBLIC cmocka gcov nbgl_obj_pool nbgl_stubs)
target_link_libraries(test_nbgl_screen PUBLIC cmocka gcov nbgl_screen nbgl_obj_pool nbgl_stubs)
target_link_libraries(test_nbgl_obj PUBLIC cmocka gcov nbgl_obj nbgl_screen nbgl_obj_pool nbgl_fonts nbgl_stubs)

add_test(test_nbgl_fonts test_nbgl_fonts)
add_test(test_nbgl_obj_pool test_nbgl_obj_pool)
add_test(test_nbgl_screen test_nbgl_screen)
add_test(test_nbgl_obj test_nbgl_obj)
