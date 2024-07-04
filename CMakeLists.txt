cmake_minimum_required(VERSION 3.28)
project(snail LANGUAGES C)

include(CMakeDependentOption)

set(CMAKE_C_STANDARD 11)
set(LIBRARY_PATH "src")
set(VENDORS_PATH "vendors")

enable_testing()

# -BUILD_SAMPLES=ON|OFF
cmake_dependent_option(SN_BUILD_SAMPLES "Build sample projects" ON "BUILD_SAMPLES" OFF)


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
        src/sn_logger.c)

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
if(SN_BUILD_SAMPLES)
    add_executable(snail_sample_1
            samples/simple/main.c)

    target_link_libraries(snail_sample_1 uv pico snail)
endif ()
########################################################################


########################################################################
# TEST_SUITS                                                           #
# /test/*                                                              #
########################################################################
set(sn_test_resources
        test/runner/test_runner.h
        test/runner/test_runner.c
        test/helpers/test_helpers.h
        test/helpers/test_helpers.c)

set(sn_test_suits
        "map"
        "event_loop")

foreach (test_suit IN LISTS sn_test_suits)
    add_executable("${test_suit}_test" ${sn_test_resources} "test/${test_suit}_test.c")
    target_link_libraries("${test_suit}_test" snail)
    add_test(NAME "${test_suit}_test" COMMAND "$<TARGET_FILE:${test_suit}_test>")
endforeach()
########################################################################