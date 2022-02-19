# CMake Toolchain file for crosscompiling on ARM. There is duplication with the profiles, but for the moment there is
# not a good way to avoid it https://github.com/conan-io/conan/issues/8922

set(CMAKE_SYSTEM_NAME
    Linux)
set(CMAKE_SYSTEM_PROCESSOR
    aarch64)

set(CMAKE_C_COMPILER
    "/usr/bin/aarch64-linux-gnu-gcc-10")
set(CMAKE_CXX_COMPILER
    "/usr/bin/aarch64-linux-gnu-g++-10")
set(CMAKE_FIND_ROOT_PATH
    /usr/aarch64-linux-gnu)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM
    NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY
    ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE
    ONLY)

set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE
    arm64)
