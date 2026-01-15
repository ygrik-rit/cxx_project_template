add_library(template)

target_sources(
    template
    PUBLIC FILE_SET template_module TYPE CXX_MODULES FILES template.cxxm
    PRIVATE template.cxx
)

target_include_directories(
    template
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(template PRIVATE SDL3::SDL3-shared Vulkan::HppModule)
target_compile_options(template PRIVATE -Werror -Wpedantic -Wall)

add_library(template::template ALIAS template)
