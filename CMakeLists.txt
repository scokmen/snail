cmake_minimum_required(VERSION 3.28)
project(snail LANGUAGES C)

include(CMakeDependentOption)

set(CMAKE_C_STANDARD 11)
set(LIBRARY_PATH "lib")
set(VENDORS_PATH "vendors")

# -BUILD_SAMPLES=ON|OFF
cmake_dependent_option(S_BUILD_SAMPLES "Build sample projects" ON "BUILD_SAMPLES" OFF)

# LIBUV
# https://github.com/libuv/libuv
# Include libuv project
add_subdirectory("${VENDORS_PATH}/libuv" "${VENDORS_PATH}/libuv")

# PICO_HTTP_PARSER
# https://github.com/h2o/picohttpparser
# Include picohttpparser project
add_library(pico STATIC
        "${VENDORS_PATH}/picohttpparser/picohttpparser.c"
        "${VENDORS_PATH}/picohttpparser/picohttpparser.h")
set_target_properties(pico PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${VENDORS_PATH}/picohttpparser")
target_include_directories(pico
        INTERFACE "${VENDORS_PATH}/picohttpparser")

# SNAIL
# Include snail project
add_library(snail STATIC
        lib/snail.h
        lib/core/http/s_sock_loop.c
        lib/core/http/s_sock_loop.h
        lib/core/s_core_buf.h
        lib/core/s_core_buf.c
        lib/core/http/s_http_serv.h
        lib/core/http/s_http_serv.c
)
target_include_directories(snail
        INTERFACE "${LIBRARY_PATH}"
        PRIVATE   "${LIBRARY_PATH}/core"
        PRIVATE   "${LIBRARY_PATH}/core/http")

target_link_libraries(snail pico uv)

if(S_BUILD_SAMPLES)
    add_executable(sample_1 samples/simple/main.c)
    target_link_libraries(sample_1 uv pico snail)
endif ()
