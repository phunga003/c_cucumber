cmake_minimum_required(VERSION 3.20)
project(c-cucumber C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)      # no GNU extensions, keep it portable

# Base compiler flags — always on
add_compile_options(
    -Wall
    -Wextra
    -Werror
    -Wpedantic
    -Wvla              # ban variable length arrays
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build types
# Usage: cmake -DCMAKE_BUILD_TYPE=Debug|Release|Asan
if(CMAKE_BUILD_TYPE STREQUAL "Asan")
    add_compile_options(-g -O0 -fsanitize=address -fsanitize=undefined)
    add_link_options(-fsanitize=address -fsanitize=undefined)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O2)
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -O0)
endif()

# Library target — what gets linked into tests and the final binary
add_library(cucumber_internal STATIC
    src/util.c
)

target_include_directories(cucumber_internal
    PUBLIC  ${CMAKE_SOURCE_DIR}/include/public
    PRIVATE ${CMAKE_SOURCE_DIR}/include/internal
)

# Enable testing
enable_testing()
add_subdirectory(tests)