#
#  Copyright (c) 2023, The OpenThread Authors.
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

# This file provides a target that copies the 'openthread-platform-utils' target
# from openthread/examples/platforms/utils/CMakeLists.txt.
#
# If a source in 'openthread-platform-utils' also exists in this directory, the
# (efr32-platform-utils' target will use the source from this
# directory instead, allowing for efr32-specific version of platform utils
#

add_library(efr32-platform-utils OBJECT)

# ==============================================================================
# SOURCES   --  Sources that will be taken from "openthread/examples/platforms/utils"
#
#   DEFAULT_OPENTHREAD_PLATFORM_UTILS_SOURCES = OPENTHREAD_PLATFORM_UTILS_SOURCES - EFR32_PAL_SOURCES
# ==============================================================================

# Create a list with all sources in "openthread-platform-utils"
get_target_property(OPENTHREAD_PLATFORM_UTILS_SOURCES openthread-platform-utils SOURCES)

# A list of sources that should be taken from the current directory instead of "openthread/examples/platforms/utils"
file(GLOB EFR32_PAL_SOURCES
    LIST_DIRECTORIES false
    RELATIVE ${CMAKE_CURRENT_LIST_DIR}
    "*")

# Remove sources that will be taken from "src/src"
set(DEFAULT_OPENTHREAD_PLATFORM_UTILS_SOURCES ${OPENTHREAD_PLATFORM_UTILS_SOURCES})
message(DEBUG "DEFAULT_OPENTHREAD_PLATFORM_UTILS_SOURCES (Original)\n\t" "${DEFAULT_OPENTHREAD_PLATFORM_UTILS_SOURCES}")
list(REMOVE_ITEM DEFAULT_OPENTHREAD_PLATFORM_UTILS_SOURCES ${EFR32_PAL_SOURCES})

# Prepend SOURCE_DIR from  "openthread-platform-utils" for all default sources for use with target_sources()
get_target_property(OPENTHREAD_PLATFORM_UTILS_SOURCE_DIR openthread-platform-utils SOURCE_DIR)
list(TRANSFORM DEFAULT_OPENTHREAD_PLATFORM_UTILS_SOURCES
    PREPEND
    "${OPENTHREAD_PLATFORM_UTILS_SOURCE_DIR}/"
    OUTPUT_VARIABLE DEFAULT_OPENTHREAD_PLATFORM_UTILS_SOURCES_FULL_PATH)

# Add default sources
target_sources(efr32-platform-utils PRIVATE
    ${DEFAULT_OPENTHREAD_PLATFORM_UTILS_SOURCES_FULL_PATH}
)

# ==============================================================================
# SOURCES   --  Sources that will be taken from "src/src"
#
#   EFR32_PLATFORM_UTILS_SOURCES = OPENTHREAD_PLATFORM_UTILS_SOURCES - DEFAULT_OPENTHREAD_PLATFORM_UTILS_SOURCES
# ==============================================================================

# List of efr32 platform utils sources
set(EFR32_PLATFORM_UTILS_SOURCES ${OPENTHREAD_PLATFORM_UTILS_SOURCES})

# Remove sources that will be taken from "openthread/examples/platforms/utils"
list(REMOVE_ITEM EFR32_PLATFORM_UTILS_SOURCES ${DEFAULT_OPENTHREAD_PLATFORM_UTILS_SOURCES})

# Add efr32-specific platform utils sources
target_sources(efr32-platform-utils PRIVATE
    ${EFR32_PLATFORM_UTILS_SOURCES}
)

# ==============================================================================
# COMPILE_DEFINITIONS
# ==============================================================================
target_compile_definitions(efr32-platform-utils PRIVATE
    # Copy from openthread-platform-utils
    $<TARGET_PROPERTY:openthread-platform-utils,COMPILE_DEFINITIONS>
)

# ==============================================================================
# INCLUDE_DIRECTORIES
# ==============================================================================
target_include_directories(efr32-platform-utils PRIVATE
    # Copy from openthread-platform-utils
    $<TARGET_PROPERTY:openthread-platform-utils,INCLUDE_DIRECTORIES>
)

# ==============================================================================
# Debug prints
# ==============================================================================
message(DEBUG "EFR32_PAL_SOURCES\n\t" "${EFR32_PAL_SOURCES}")
message(DEBUG "OPENTHREAD_PLATFORM_UTILS_SOURCES\n\t" "${OPENTHREAD_PLATFORM_UTILS_SOURCES}")
message(DEBUG "DEFAULT_OPENTHREAD_PLATFORM_UTILS_SOURCES\n\t" "${DEFAULT_OPENTHREAD_PLATFORM_UTILS_SOURCES}")
message(DEBUG "EFR32_PLATFORM_UTILS_SOURCES\n\t" "${EFR32_PLATFORM_UTILS_SOURCES}")
message(DEBUG "DEFAULT_OPENTHREAD_PLATFORM_UTILS_SOURCES_FULL_PATH\n\t" "${DEFAULT_OPENTHREAD_PLATFORM_UTILS_SOURCES_FULL_PATH}")

# Provide a static library implementation of platform-utils for non-cmake platforms
add_library(efr32-platform-utils-static $<TARGET_OBJECTS:efr32-platform-utils>)
