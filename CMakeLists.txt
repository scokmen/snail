cmake_minimum_required(VERSION 3.16.3)
project(snail LANGUAGES C)

# Variables
set(CMAKE_C_STANDARD  11)
set(LIBRARY_PATH      "src")
set(VENDORS_PATH      "vendors")
set(TEST_PATH         "test")
set(SAMPLES_PATH      "samples")
set(BENCHMARKS_PATH   "benchmark")
set(TEST_CASES_SOURCE "CTestCases.txt")

# CTEST
include(CTest)
set(CMAKE_CTEST_ARGUMENTS "--output-on-failure")
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
set(snail_resources
        src/server/http.c
        src/server/router.c
        src/server/event_loop.c
        src/sn_logger.h
        src/sn_logger.c
        src/sn_common.h
        src/sn_common.c
        src/types/map.c
        src/types/dlist.c)

add_library(snail STATIC ${snail_resources})

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
set(sample_applications
        "simple_server")

foreach (sample_application IN LISTS sample_applications)
    add_executable("${sample_application}_sample" "${SAMPLES_PATH}/${sample_application}.c")
    target_link_libraries("${sample_application}_sample" snail)
endforeach()
########################################################################


########################################################################
# TEST_SUITS                                                           #
# /test/*                                                              #
########################################################################
set(test_resources
        ${TEST_PATH}/runner/test_runner.h
        ${TEST_PATH}/runner/test_runner.c
        ${TEST_PATH}/helpers/test_helpers.h
        ${TEST_PATH}/helpers/test_helpers.c)

# Discover the test files using glob "*_test.c"
file(GLOB TEST_FILES "${TEST_PATH}/*_test.c")

# Read test cases from "CTestCases.txt"
file(STRINGS "${TEST_CASES_SOURCE}" TEST_FILE_AND_CASES)

# Discover available test cases and add test for each of them.
foreach(TEST_FILE_PATH IN LISTS TEST_FILES)
    get_filename_component(TEST_FILE_WITH_SUFFIX "${TEST_FILE_PATH}" NAME_WLE)
    string(REPLACE "_test" "" TEST_FILE "${TEST_FILE_WITH_SUFFIX}")
    add_executable("${TEST_FILE_WITH_SUFFIX}" ${test_resources} "${TEST_FILE_PATH}")
    target_link_libraries("${TEST_FILE_WITH_SUFFIX}" snail)
    foreach(TEST_FILE_AND_CASE IN LISTS TEST_FILE_AND_CASES)
        if (TEST_FILE_AND_CASE MATCHES "^#.*")
            continue()
        endif()
        if (TEST_FILE_AND_CASE MATCHES "^${TEST_FILE}:.*")
            string(REPLACE "${TEST_FILE}:" "" TEST_CASE "${TEST_FILE_AND_CASE}")
            add_test(NAME "${TEST_FILE_AND_CASE}" COMMAND "$<TARGET_FILE:${TEST_FILE_WITH_SUFFIX}>" "${TEST_CASE}")
        endif ()
    endforeach()
endforeach()
########################################################################


########################################################################
# BENCHMARKS                                                           #
# /memcheck/*                                                          #
########################################################################
# Discover the bench files using glob "*_bench.c"
file(GLOB BENCHMARKS_FILES "${BENCHMARKS_PATH}/*_bench.c")

foreach (BENCHMARKS_FILE IN LISTS BENCHMARKS_FILES)
    get_filename_component(BENCHMARKS_FILE_NAME "${BENCHMARKS_FILE}" NAME_WLE)
    add_executable("${BENCHMARKS_FILE_NAME}" "${BENCHMARKS_PATH}/${BENCHMARKS_FILE_NAME}.c")
    target_link_libraries("${BENCHMARKS_FILE_NAME}" snail)
endforeach()
########################################################################
