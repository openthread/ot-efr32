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

cmake_minimum_required(VERSION 3.10.2)
project(ot-efr32 VERSION 0.2.0)

set(EFR32_PLATFORM_VALUES
    "efr32mg1"
    "efr32mg12"
    "efr32mg13"
    "efr32mg21"
)
set_property(CACHE EFR32_PLATFORM PROPERTY STRINGS ${EFR32_PLATFORM_VALUES})
if(NOT EFR32_PLATFORM IN_LIST EFR32_PLATFORM_VALUES)
    message(FATAL_ERROR "Please select a supported platform: ${EFR32_PLATFORM_VALUES}")
endif()

set(OT_PLATFORM_LIB "openthread-${EFR32_PLATFORM}")

set(OT_PLATFORM "external" CACHE STRING "disable example platforms" FORCE)
set(OT_BUILTIN_MBEDTLS_MANAGEMENT OFF CACHE BOOL "disable builtin mbedtls management" FORCE)
set(OT_EXTERNAL_MBEDTLS "silabs-mbedtls" CACHE STRING "use silabs mbedtls" FORCE)
set(OT_MBEDTLS ${OT_EXTERNAL_MBEDTLS})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

add_subdirectory(openthread)

target_compile_definitions(ot-config INTERFACE
    OPENTHREAD_CONFIG_FILE="openthread-core-efr32-config.h"
    OPENTHREAD_PROJECT_CORE_CONFIG_FILE="openthread-core-efr32-config.h"
    OPENTHREAD_CORE_CONFIG_PLATFORM_CHECK_FILE="openthread-core-efr32-config-check.h"
    OPENTHREAD_CONFIG_NCP_UART_ENABLE=1
)

target_compile_options(ot-config INTERFACE
    -Wno-shadow
    -Wno-pedantic
)

target_include_directories(ot-config INTERFACE
    ${PROJECT_SOURCE_DIR}/src/src
    ${PROJECT_SOURCE_DIR}/src/${EFR32_PLATFORM}
    ${PROJECT_SOURCE_DIR}/src/${EFR32_PLATFORM}/crypto
)

add_subdirectory(src)
add_subdirectory(third_party)
