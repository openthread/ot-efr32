#
#  Copyright (c) 2021, The OpenThread Authors.
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#  3. Neither the name of the copyright holder nor the
#     names of its contributors may be used to endorse or promote products
#     derived from this software without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
#  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
#  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
#  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
#  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
#  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#  POSSIBILITY OF SUCH DAMAGE.
#

cmake_minimum_required(VERSION 3.16.0)
project(ot-efr32 VERSION 1.0.0)

include(${PROJECT_SOURCE_DIR}/openthread/etc/cmake/functions.cmake)
include(${PROJECT_SOURCE_DIR}/third_party/silabs/cmake/utility.cmake)

set(OT_PACKAGE_VERSION "" CACHE STRING "OpenThread Package Version")
if(OT_PACKAGE_VERSION STREQUAL "")
    ot_git_version(OT_PACKAGE_VERSION)
    message(STATUS "Setting default package version: ${OT_PACKAGE_VERSION}")
endif()
message(STATUS "Package Version: ${OT_PACKAGE_VERSION}")

set(OT_PLATFORM_LIB "openthread-efr32")
set(OT_PLATFORM_LIB_RCP openthread-efr32-rcp)

set(OT_PLATFORM "external" CACHE STRING "disable example platforms" FORCE)

# ==============================================================================
# mbedtls
# ==============================================================================
set(OT_EXTERNAL_MBEDTLS "silabs-mbedtls-soc" CACHE STRING "use silabs mbedtls" FORCE)
set(OT_BUILTIN_MBEDTLS_MANAGEMENT OFF CACHE BOOL "disable builtin mbedtls management" FORCE)
set(OT_MBEDTLS ${OT_EXTERNAL_MBEDTLS})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

add_subdirectory(openthread)

target_compile_options(ot-config INTERFACE
    -Wno-shadow
    -Wno-pedantic
)

add_subdirectory(examples)

set(slc_generated_projects
    ${PROJECT_SOURCE_DIR}/build/${BOARD_LOWERCASE}/slc/soc/platform
    ${PROJECT_SOURCE_DIR}/build/${BOARD_LOWERCASE}/slc/soc/mbedtls
    ${PROJECT_SOURCE_DIR}/build/${BOARD_LOWERCASE}/slc/rcp/platform
    ${PROJECT_SOURCE_DIR}/build/${BOARD_LOWERCASE}/slc/rcp/mbedtls
)
foreach(slc_project IN LISTS slc_generated_projects)
    message("Checking ${slc_project}")
    if(EXISTS ${slc_project})
        message("......... EXISTS")
        add_subdirectory(${slc_project})
    else()
        message("......... NOT EXISTS")
    endif()
endforeach()
add_subdirectory(third_party)
