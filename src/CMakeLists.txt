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

add_library(template::template ALIAS template)

install(
    TARGETS template
    EXPORT templateTargets
    FILE_SET HEADERS DESTINATION include/template
    FILE_SET template_module DESTINATION lib
)
install(EXPORT templateTargets NAMESPACE template:: DESTINATION lib/cmake)
