# https://github.com/vitasdk/buildscripts/blob/master/cmake/GetTriplet.cmake The MIT License (MIT)
#
# Copyright (c) 2016 codestation
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
# documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit
# persons to whom the Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
# Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
# WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
# OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

function(get_host_triplet triplet)
    set(host_arch ${CMAKE_SYSTEM_PROCESSOR})
    if(host_arch STREQUAL "x86")
        set(host_arch "i686")
    endif()
    if(host_arch STREQUAL "amd64")
        set(host_arch "x86_64")
    endif()

    if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
        string(TOLOWER ${CMAKE_SYSTEM_NAME} host_os)
        set(host_release "gnu")
    elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
        set(host_os "w64")
        set(host_release "mingw32")
    elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
        set(host_os "apple")
        set(host_release "darwin")
    elseif(${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
        string(REGEX REPLACE "-.*" "" host_version "${CMAKE_SYSTEM_VERSION}")
        set(host_os "portbld")
        set(host_release "freebsd${host_version}")
    else()
        message(FATAL "Unsupported system: ${CMAKE_SYSTEM_NAME}")
    endif()

    set(${triplet}
        "${host_arch}-${host_os}-${host_release}"
        PARENT_SCOPE)
endfunction()

function(get_build_triplet triplet)
    set(host_arch ${CMAKE_HOST_SYSTEM_PROCESSOR})
    if(host_arch STREQUAL "x86")
        set(host_arch "i686")
    endif()
    if(host_arch STREQUAL "amd64")
        set(host_arch "x86_64")
    endif()

    if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
        string(TOLOWER ${CMAKE_HOST_SYSTEM_NAME} build_os)
        set(build_release "gnu")
    elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
        set(build_os "w64")
        set(build_release "mingw32")
    elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Darwin")
        set(build_os "apple")
        set(build_release "darwin")
    elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "FreeBSD")
        string(REGEX REPLACE "-.*" "" build_version "${CMAKE_HOST_SYSTEM_VERSION}")
        set(build_os "portbld")
        set(build_release "freebsd${build_version}")
    else()
        message(FATAL "Unsupported system: ${CMAKE_HOST_SYSTEM_NAME}")
    endif()

    set(${triplet}
        "${host_arch}-${build_os}-${build_release}"
        PARENT_SCOPE)
endfunction()
