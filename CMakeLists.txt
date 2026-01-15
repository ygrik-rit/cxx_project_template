cmake_minimum_required(VERSION 3.31.6 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")
set(CMAKE_CXX_MODULE_STD 1)

add_link_options(-fuse-ld=mold)
# add_link_options(--rtlib=compiler-rt)
# add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-stdlib=libc++>)

project(
    template
    VERSION 0.0.1.0
    DESCRIPTION "blah blah blah"
    HOMEPAGE_URL "http://localhost"
    LANGUAGES C CXX
)

list(APPEND CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(options REQUIRED)

add_library(template_base INTERFACE)
target_compile_options(template_base INTERFACE -Werror -Wpedantic -Wall)
add_library(template::base ALIAS template_base)

add_subdirectory(lib)
add_subdirectory(src)
add_subdirectory(example)
