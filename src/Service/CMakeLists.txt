include(CMake_SrcFiles.cmake)

option(MOCK_BCM2835 "Mock the implementation of the Bcm2835" OFF)

find_package(mp-units CONFIG REQUIRED)

add_library(Service ${header_files} ${src_files})
add_library(${PROJECT_NAME}::Service ALIAS Service)

generate_export_header(Service)

set_property(
  TARGET Service
  PROPERTY PUBLIC_HEADER
    ${header_files}
    ${CMAKE_CURRENT_BINARY_DIR}/service_export.h)

target_include_directories(
  Service
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
         $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/include>
  PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/PiFan/Service>)

if(MOCK_BCM2835)
  target_compile_definitions(Service PRIVATE MOCK_BCM2835)
endif()

target_link_libraries(Service PUBLIC
  bcm2835::lib 
  mp-units::core
  mp-units::isq)

install(TARGETS Service
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
  PUBLIC_HEADER
    DESTINATION
    "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/Service/")

