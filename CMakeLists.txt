cmake_minimum_required(VERSION 3.16.3)
project(snail LANGUAGES C)

include(CMakeDependentOption)

set(CMAKE_C_STANDARD 11)
set(LIBRARY_PATH "src")
set(VENDORS_PATH "vendors")
set(TEST_PATH "test")
set(SAMPLES_PATH "samples")
set(BENCHMARKS_PATH "benchmark")

enable_testing()

########################################################################
# LIBUV                                                                #
# https://github.com/libuv/libuv                                       #
########################################################################
add_subdirectory("${VENDORS_PATH}/libuv" "${VENDORS_PATH}/libuv")
########################################################################


########################################################################
# PICO_HTTP_PARSER                                                     #
# https://github.com/h2o/picohttpparser                                #
########################################################################
add_library(pico STATIC
        "${VENDORS_PATH}/picohttpparser/picohttpparser.c"
        "${VENDORS_PATH}/picohttpparser/picohttpparser.h")

set_target_properties(pico PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${VENDORS_PATH}/picohttpparser")

target_include_directories(pico
        INTERFACE "${VENDORS_PATH}/picohttpparser")
########################################################################


########################################################################
# SNAIL                                                                #
# https://github.com/scokmen/snail                                     #
########################################################################
set(sn_sources
        src/types/map.c
        src/server/http.c
        src/server/event_loop.c
        src/sn_logger.h
        src/sn_logger.c
        src/sn_common.h)

add_library(snail STATIC ${sn_sources})

target_include_directories(snail
        PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        PRIVATE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>)

target_link_libraries(snail pico uv)
########################################################################


########################################################################
# SAMPLES                                                              #
# /samples/*                                                           #
########################################################################
set(sn_sample_apps
        "simple_server")

foreach (sample_app IN LISTS sn_sample_apps)
    add_executable("${sample_app}_sample" "${SAMPLES_PATH}/${sample_app}.c")
    target_link_libraries("${sample_app}_sample" snail)
endforeach()
########################################################################


########################################################################
# TEST_SUITS                                                           #
# /test/*                                                              #
########################################################################
set(sn_test_resources
        ${TEST_PATH}/runner/test_runner.h
        ${TEST_PATH}/runner/test_runner.c
        ${TEST_PATH}/helpers/test_helpers.h
        ${TEST_PATH}/helpers/test_helpers.c)

set(sn_test_suits
        "map"
        "event_loop")

foreach (test_suit IN LISTS sn_test_suits)
    add_executable("${test_suit}_test" ${sn_test_resources} "${TEST_PATH}/${test_suit}_test.c")
    target_link_libraries("${test_suit}_test" snail)
    add_test(NAME "${test_suit}_test" COMMAND "$<TARGET_FILE:${test_suit}_test>")
endforeach()
########################################################################


########################################################################
# BENCHMARKS                                                           #
# /memcheck/*                                                          #
########################################################################
set(sn_benchmarks
        "map")

foreach (benchmark IN LISTS sn_benchmarks)
    add_executable("${benchmark}_benchmark" "${BENCHMARKS_PATH}/${benchmark}_benchmark.c")
    target_link_libraries("${benchmark}_benchmark" snail)
endforeach()
########################################################################