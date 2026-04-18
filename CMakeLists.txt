cmake_minimum_required(VERSION 3.20)
project(cucumber C)

# Enforce C11, we use stdbool, stdint, and _Generic
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)      # no GNU extensions, keep it portable

# Base compiler flags — always on
add_compile_options(
    -Wall
    -Wextra
    -Werror
    -Wpedantic
)

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
    PUBLIC  include/           # cucumber.h — public API
    PRIVATE internal/          # util.h — internal only
)

# Enable testing
enable_testing()
add_subdirectory(tests)