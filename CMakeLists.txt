cmake_minimum_required(VERSION 3.28)
project(snail LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(INCLUDE_FOLDER "lib/include")

# LIBUV
# https://github.com/libuv/libuv
# Include libuv project
add_subdirectory("${INCLUDE_FOLDER}/libuv" "${INCLUDE_FOLDER}/libuv")

# Initialize executable
add_executable(main samples/simple/main.c)

# Link LIBUV
target_link_libraries(main uv)




