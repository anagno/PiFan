include(CMake_SrcFiles.cmake)

find_package(mp-units CONFIG REQUIRED)

add_library(
    Business
    ${header_files} ${src_files})
add_library(
    ${PROJECT_NAME}::Business ALIAS Business)

generate_export_header(
    Business)

set_property(
    TARGET Business
    PROPERTY PUBLIC_HEADER ${header_files} ${CMAKE_CURRENT_BINARY_DIR}/business_export.h)

target_include_directories(
    Business
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
           $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/include>
    PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/PiFan/Business>)

target_link_libraries(
    Business
    PUBLIC PiFan::Service mp-units::core)

install(TARGETS Business
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/Business/")
