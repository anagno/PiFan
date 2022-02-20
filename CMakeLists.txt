cmake_minimum_required(VERSION 3.22.1)
project(PiFan VERSION 0.0.1)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

include(GetTriplet)
if(NOT HOST_NAME)
    get_host_triplet(HOST_NAME)
endif()
get_build_triplet(BUILD_NAME)
message(STATUS "Host:   ${HOST_NAME}")
message(STATUS "Build:  ${BUILD_NAME}")

if(NOT CONAN_EXPORTED)

    # Download automatically, you can also just copy the conan.cmake file
    if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")

        message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
        file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/0.17.0/conan.cmake"
             "${CMAKE_BINARY_DIR}/conan.cmake")

    endif()

    if(NOT EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.txt" OR FORCE_CONAN_INSTALL)

        include(${CMAKE_BINARY_DIR}/conan.cmake)
        conan_check(VERSION 1.44.1 REQUIRED)

        conan_cmake_install(
            PATH_OR_REFERENCE ${CMAKE_SOURCE_DIR}/conanfile.py
            BUILD missing
            PROFILE_HOST ${CMAKE_SOURCE_DIR}/conan/${HOST_NAME}
            PROFILE_BUILD ${CMAKE_SOURCE_DIR}/conan/${BUILD_NAME})

    endif()

    set(FORCE_CONAN_INSTALL
        "FALSE"
        CACHE BOOL "Force the execution of conan install
        for the next time (and only the next time) we configure Cmake" FORCE)
endif()

include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}")

find_program(
    MAKE_EXECUTABLE
    NAMES gmake mingw32-make nmake make
    DOC "GNU Make path" REQUIRED)

# Should have been a conan package, but there is not yet a recipe.
include(ExternalProject)
externalproject_add(
    bcm2835
    URL http://www.airspayce.com/mikem/bcm2835/bcm2835-1.71.tar.gz
    BUILD_IN_SOURCE TRUE
    CONFIGURE_COMMAND ${CMAKE_COMMAND} -E env CC=${CMAKE_C_COMPILER} <SOURCE_DIR>/configure --host ${HOST_NAME}
    BUILD_COMMAND ${CMAKE_COMMAND} -E env CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER} ${MAKE_EXECUTABLE} -C
                  <SOURCE_DIR>
    INSTALL_COMMAND ${MAKE_EXECUTABLE} DESTDIR=<INSTALL_DIR> install)

externalproject_get_property(bcm2835 INSTALL_DIR)
# Necessary, because CMake will complain for non existent paths
file(MAKE_DIRECTORY "${INSTALL_DIR}/usr/local/include")

add_library(
    bcm2835::lib STATIC IMPORTED)
set_target_properties(
    bcm2835::lib
    PROPERTIES IMPORTED_LOCATION "${INSTALL_DIR}/usr/local/lib/libbcm2835.a" INTERFACE_INCLUDE_DIRECTORIES
                                                                             "${INSTALL_DIR}/usr/local/include")

add_dependencies(bcm2835::lib bcm2835)

include(GenerateExportHeader)
include(GNUInstallDirs)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    option(ENABLE_COMPILER_WARNINGS "" OFF)
    # https://github.com/cpp-best-practices/project_options/blob/main/src/CompilerWarnings.cmake
    if(ENABLE_COMPILER_WARNINGS)
        add_compile_options(
            -Wall
            -Wextra # reasonable and standard
            -Wshadow # warn the user if a variable declaration shadows one from a parent context
            -Wnon-virtual-dtor # warn the user if a class with virtual functions has a non-virtual destructor. This helps
            # catch hard to track down memory errors
            -Wold-style-cast # warn for c-style casts
            -Wcast-align # warn for potential performance problem casts
            -Wunused # warn on anything being unused
            -Woverloaded-virtual # warn if you overload (not override) a virtual function
            -Wpedantic # warn if non-standard C++ is used
            -Wconversion # warn on type conversions that may lose data
            -Wsign-conversion # warn on sign conversions
            -Wnull-dereference # warn if a null dereference is detected
            -Wdouble-promotion # warn if float is implicit promoted to double
            -Wformat=2 # warn on security issues around functions that format output (ie printf)
            -Wimplicit-fallthrough # warn on statements that fallthrough without an explicit annotation
            -Wmisleading-indentation # warn if indentation implies blocks where blocks do not exist
            -Wduplicated-cond # warn if if / else chain has duplicated conditions
            -Wduplicated-branches # warn if if / else branches have duplicated code
            -Wlogical-op # warn about logical operations being used where bitwise were probably wanted
            -Wuseless-cast # warn if you perform a cast to the same type
            -Werror # fail to compile when there are errors
        )
    endif()
endif()

option(ENABLE_CPPCHECK "" OFF)
if(ENABLE_CPPCHECK)
    find_program(CPPCHECK cppcheck REQUIRED)
    # https://github.com/cpp-best-practices/project_options/blob/7371578d79b0fbb922e29306c4cc09318adb15c5/src/StaticAnalyzers.cmake
    set(CMAKE_CXX_CPPCHECK
        ${CPPCHECK}
        --template=gcc
        --enable=style,performance,warning,portability
        --inline-suppr
        # We cannot act on a bug/missing feature of cppcheck
        --suppress=internalAstError
        # if a file does not have an internalAstError, we get an unmatchedSuppression error
        --suppress=unmatchedSuppression
        --inconclusive
        --std=c++${CMAKE_CXX_STANDARD}
        --error-exitcode=2)
endif()

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

add_subdirectory(src)

# Tests https://gitlab.kitware.com/cmake/community/wikis/doc/ctest/Testing-With-CTest#dynamic-analysis
option(BUILD_TESTING "" ON)
include(CTest)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
