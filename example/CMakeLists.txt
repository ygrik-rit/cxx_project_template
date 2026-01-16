if(CMAKE_SYSTEM_NAME STREQUAL "Android")
    add_library(example SHARED)
else()
    add_executable(example)
endif()

target_sources(example PRIVATE example.cxx)
target_link_libraries(example PRIVATE template::template template::base)

if(CMAKE_SYSTEM_NAME STREQUAL "Android")
    add_library(examples SHARED)
else()
    add_executable(examples)
endif()

target_sources(examples PRIVATE example_sdl.cxx)
target_include_directories(examples PRIVATE ${CMAKE_SOURCE_DIR}/lib/SDL/test)
target_link_libraries(examples PRIVATE SDL3::SDL3-shared template::base)
