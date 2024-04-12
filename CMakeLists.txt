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


# ==============================================================================
# CMake project definitions
# ==============================================================================
cmake_minimum_required(VERSION 3.16.0)
project(ot-efr32 VERSION 1.0.0 LANGUAGES C CXX ASM)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

# Toolchain file
message(STATUS "Toolchain file: ${CMAKE_TOOLCHAIN_FILE}")

# ==============================================================================
# OpenThread
# ==============================================================================
include(${PROJECT_SOURCE_DIR}/openthread/etc/cmake/functions.cmake)
include(${PROJECT_SOURCE_DIR}/third_party/silabs/cmake/utility.cmake)

# Package Version
set(OT_PACKAGE_VERSION "" CACHE STRING "OpenThread Package Version")
if(OT_PACKAGE_VERSION STREQUAL "")
    ot_git_version(OT_PACKAGE_VERSION)
    message(STATUS "Setting default package version: ${OT_PACKAGE_VERSION}")
endif()
message(STATUS "Package Version: ${OT_PACKAGE_VERSION}")

# ==============================================================================
# Pull in Vendor library target names and paths
# ==============================================================================
if(VENDOR_EXTENSION)
    message(STATUS "Using vendor libraries from ${VENDOR_EXTENSION}")
    add_subdirectory(${VENDOR_EXTENSION})
endif()

# ==============================================================================
# Set platform libs and allow overriding
# ==============================================================================
# SoC (mtd/ftd) platform library
if(NOT OT_PLATFORM_LIB)
    set(OT_PLATFORM_LIB "openthread-efr32-soc" CACHE STRING "default efr32 SoC platform library")
endif()

# RCP platform library
if(NOT OT_PLATFORM_LIB_RCP_SPI)
    set(OT_PLATFORM_LIB_RCP_SPI "openthread-efr32-rcp-spi" CACHE STRING "default efr32 RCP platform library")
endif()

if(NOT OT_PLATFORM_LIB_RCP_UART)
    set(OT_PLATFORM_LIB_RCP_UART "openthread-efr32-rcp-uart" CACHE STRING "default efr32 RCP platform library")
endif()
unset(OT_PLATFORM_LIB_RCP CACHE)

if(NOT OT_PLATFORM_LIB_RCP)
    if(OT_NCP_SPI)
        set(OT_PLATFORM_LIB_RCP "${OT_PLATFORM_LIB_RCP_SPI}" CACHE STRING "default efr32 RCP platform library")
    else()
        set(OT_PLATFORM_LIB_RCP "${OT_PLATFORM_LIB_RCP_UART}" CACHE STRING "default efr32 RCP platform library")
    endif()
endif()

set(OT_PLATFORM "external" CACHE STRING "disable example platforms" FORCE)

# ==============================================================================
# mbedtls
# ==============================================================================
# Specify a default mbedtls library if not provided
unset(OT_EXTERNAL_MBEDTLS CACHE)
unset(OT_MBEDTLS_RCP CACHE)

if(NOT OT_EXTERNAL_MBEDTLS)
    if(OT_FTD OR OT_MTD)
        # SoC mbedtls library
        set(OT_EXTERNAL_MBEDTLS "${OT_PLATFORM_LIB}-mbedtls" CACHE STRING "external mbedtls library")
    elseif(OT_RCP)
        # RCP mbedtls library
        set(OT_MBEDTLS_RCP "${OT_PLATFORM_LIB_RCP}-mbedtls" CACHE STRING "default efr32 RCP mbedtls library")
        set(OT_EXTERNAL_MBEDTLS "${OT_MBEDTLS_RCP}" CACHE STRING "external mbedtls library")
    endif()
endif()

# If using
if(OT_EXTERNAL_MBEDTLS)
    set(OT_BUILTIN_MBEDTLS_MANAGEMENT OFF CACHE BOOL "disable builtin mbedtls management" FORCE)
endif()

# ==============================================================================
# openthread
# ==============================================================================
# NOTE: The "openthread" subdirectory needs to be added after all OT_* variables are defined
add_subdirectory(openthread)

add_subdirectory(src/src)

# Add any efr32 example projects
add_subdirectory(examples)

target_compile_options(ot-config INTERFACE
    -Wno-shadow
    -Wno-pedantic
)

# ==============================================================================
# Generated platform projects
# ==============================================================================
message("OT_RCP     = ${OT_RCP}")
message("OT_FTD     = ${OT_FTD}")
message("OT_MTD     = ${OT_MTD}")
message("OT_APP_RCP = ${OT_APP_RCP}")
message("OT_APP_NCP = ${OT_APP_NCP}")
message("OT_APP_CLI = ${OT_APP_CLI}")

message("===================================================================")
if(OT_PLATFORM_LIB_DIR)
    message(STATUS "Using platform library from ${OT_PLATFORM_LIB_DIR}")
    # Parse the basename of the platform library directory
    get_filename_component(OT_PLATFORM_LIB_DIR_BASE ${OT_PLATFORM_LIB_DIR} NAME)

    # Add the platform library directory
    add_subdirectory(${OT_PLATFORM_LIB_DIR} ${PROJECT_BINARY_DIR}/${OT_PLATFORM_LIB_DIR_BASE})
else()
    message(FATAL_ERROR "No platform library directory found")
endif()

message("===================================================================")
message("Using the following platform library targets:")
if(OT_RCP)
    if(OT_NCP_SPI)
        message("-- OT_PLATFORM_LIB_RCP_SPI  = ${OT_PLATFORM_LIB_RCP_SPI}")
    else()
        message("-- OT_PLATFORM_LIB_RCP_UART = ${OT_PLATFORM_LIB_RCP_UART}")
    endif()
    message("-- OT_PLATFORM_LIB_RCP      = ${OT_PLATFORM_LIB_RCP}")
elseif(OT_FTD OR OT_MTD)
    message("-- OT_PLATFORM_LIB          = ${OT_PLATFORM_LIB}")
endif()
message("-- OT_EXTERNAL_MBEDTLS      = ${OT_EXTERNAL_MBEDTLS}")
message("===================================================================")


# efr32mg12 Sub-GHz support
set(EFR32MG12_915MHZ_PHY_SOURCES
    ${PROJECT_SOURCE_DIR}/src/efr32mg12/phy/PHY_IEEE802154_915MHZ_OQPSK_EFR32XG12.c
    ${PROJECT_SOURCE_DIR}/src/efr32mg12/phy/PHY_IEEE802154_915MHZ_2GFSK_EFR32XG12.c
)
string(COMPARE EQUAL "${PLATFORM_LOWERCASE}" "efr32mg12" _cmp)
if (_cmp AND TARGET ${OT_PLATFORM_LIB})
    target_sources(${OT_PLATFORM_LIB} PUBLIC ${EFR32MG12_915MHZ_PHY_SOURCES})
endif()
