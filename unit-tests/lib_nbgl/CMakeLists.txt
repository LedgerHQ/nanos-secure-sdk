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
add_compile_definitions(HAVE_BAGL_FONT_INTER_REGULAR_24PX)
add_compile_definitions(HAVE_BAGL_FONT_INTER_SEMIBOLD_24PX)
add_compile_definitions(HAVE_BAGL_FONT_INTER_MEDIUM_32PX)
add_compile_definitions(HAVE_BAGL_FONT_HMALPHAMONO_MEDIUM_32PX)
add_compile_definitions(HAVE_LANGUAGE_PACK)
add_compile_definitions(HAVE_UNICODE_SUPPORT)
add_compile_definitions(USB_SEGMENT_SIZE=64)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall ${DEFINES} -g -O0 --coverage")

set(GCC_COVERAGE_LINK_FLAGS "--coverage -lgcov")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")

# guard against in-source builds
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there. You may need to remove CMakeCache.txt. ")
endif()

add_compile_definitions(TEST)

include_directories(../../target/stax/include)
include_directories(../../include)
include_directories(../../lib_nbgl/include)
include_directories(../../lib_ux_stax)

add_executable(test_nbgl_fonts test_nbgl_fonts.c)
add_executable(test_nbgl_screen test_nbgl_screen.c)

add_library(nbgl_obj_pool SHARED ../../lib_nbgl/src/nbgl_obj_pool.c)
add_library(nbgl_fonts SHARED ../../lib_nbgl/src/nbgl_fonts.c)
add_library(nbgl_screen SHARED ../../lib_nbgl/src/nbgl_screen.c)

target_link_libraries(test_nbgl_fonts PUBLIC cmocka gcov nbgl_fonts)
target_link_libraries(test_nbgl_screen PUBLIC cmocka gcov nbgl_screen nbgl_obj_pool)

add_test(test_nbgl_fonts test_nbgl_fonts)
add_test(test_nbgl_screen test_nbgl_screen)
