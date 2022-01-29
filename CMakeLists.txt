
cmake_minimum_required(VERSION 3.22.1)
project (pi_fan_pwn VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")


set(CMAKE_TOOLCHAIN_FILE toolchain-RaspberryPi-arm64)

find_program(MAKE_EXECUTABLE
    NAMES gmake mingw32-make nmake make
    DOC "GNU Make path"
    REQUIRED)

include(GetTriplet)
get_host_triplet(HOST_NAME)
get_build_triplet(BUILD_NAME)
message(STATUS "Host:   ${HOST_NAME}")
message(STATUS "Build:  ${BUILD_NAME}")

include(ExternalProject)
ExternalProject_add(bcm2835
  URL http://www.airspayce.com/mikem/bcm2835/bcm2835-1.71.tar.gz
  BUILD_IN_SOURCE TRUE
  CONFIGURE_COMMAND <SOURCE_DIR>/configure --build ${BUILD_NAME} --host ${HOST_NAME}
  BUILD_COMMAND ${CMAKE_COMMAND} -E env CC=${CMAKE_C_COMPILER} ${MAKE_EXECUTABLE} -C <SOURCE_DIR>
  INSTALL_COMMAND ${MAKE_EXECUTABLE} DESTDIR=<INSTALL_DIR> install
)

ExternalProject_Get_Property(bcm2835 INSTALL_DIR)
# Necessary, because CMake will complain for non existent paths
file(MAKE_DIRECTORY "${INSTALL_DIR}/usr/local/include")

add_library(bcm2835::lib STATIC IMPORTED)
set_target_properties(bcm2835::lib PROPERTIES 
  IMPORTED_LOCATION "${INSTALL_DIR}/usr/local/lib/libbcm2835.a"
  INTERFACE_INCLUDE_DIRECTORIES "${INSTALL_DIR}/usr/local/include")

add_dependencies(bcm2835::lib bcm2835)

add_executable(pi_fan_hwpwm main.cpp)
target_link_libraries(pi_fan_hwpwm bcm2835::lib)

