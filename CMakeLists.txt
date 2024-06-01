cmake_minimum_required(VERSION 3.28)
project(snail LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(INCLUDE_FOLDER "lib/include")

# LIBUV
# https://github.com/libuv/libuv
# Include libuv project
add_subdirectory("${INCLUDE_FOLDER}/libuv" "${INCLUDE_FOLDER}/libuv")

# PICO_HTTP_PARSER
# https://github.com/h2o/picohttpparser
# Include picohttpparser project
include_directories("${INCLUDE_FOLDER}/picohttpparser")
add_library(pico "${INCLUDE_FOLDER}/picohttpparser/picohttpparser.c" "${INCLUDE_FOLDER}/picohttpparser/picohttpparser.h")
set_target_properties(pico PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${INCLUDE_FOLDER}/picohttpparser")

# Initialize executable
add_executable(main samples/simple/main.c)

# Link LIBUV
target_link_libraries(main uv pico)