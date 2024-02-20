cmake_minimum_required(VERSION 3.10)

project(Fuzzer
        VERSION 1.0
	      DESCRIPTION "SDK Fuzzer"
        LANGUAGES C)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CPP_COMPILER clang)

set(CMAKE_BUILD_TYPE "Debug")

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS
    "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic -DFUZZ -g -O0 -fsanitize=fuzzer,address,undefined"
)

include_directories(
  ../lib_standard_app/
  ../qrcode/include/
  mock/
)

add_library(base58 SHARED ../../lib_standard_app/base58.c)
add_library(bip32 SHARED ../../lib_standard_app/bip32.c)
add_library(read SHARED ../../lib_standard_app/read.c)
add_library(apdu_parser SHARED ../../lib_standard_app/parser.c)
add_library(qrcodegen SHARED ../../qrcode/src/qrcodegen.c mock/os_task.c)

add_executable(fuzz_apdu_parser fuzzer_apdu_parser.c)
add_executable(fuzz_base58 fuzzer_base58.c)
add_executable(fuzz_bip32 fuzzer_bip32.c)
add_executable(fuzz_qrcodegen fuzzer_qrcodegen.c)

target_link_libraries(fuzz_apdu_parser apdu_parser)
target_link_libraries(fuzz_base58 base58)
target_link_libraries(fuzz_bip32 bip32 read)
target_link_libraries(fuzz_qrcodegen qrcodegen)
