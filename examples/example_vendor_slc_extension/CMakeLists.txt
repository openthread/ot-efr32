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

# ==============================================================================
# CMake project definitions
# ==============================================================================
set(EXAMPLE_VENDOR_DIR ${CMAKE_CURRENT_LIST_DIR})

# Overwrite the defaults for where each platform library definition is located
#   This will tell CMake to use the platform libraries from this extension
set(RCP_SPI_DIR     "${EXAMPLE_VENDOR_DIR}/generated_projects/rcp_spi"    CACHE STRING "Path to RCP (SPI) library definition" FORCE)
set(RCP_UART_DIR    "${EXAMPLE_VENDOR_DIR}/generated_projects/rcp_uart"   CACHE STRING "Path to RCP (UART) library definition" FORCE)
set(SOC_DIR         "${EXAMPLE_VENDOR_DIR}/generated_projects/soc"        CACHE STRING "Path to SoC library definition" FORCE)

set(OT_PLATFORM_LIB             "example_vendor-soc"        CACHE STRING "default efr32 SoC platform library" FORCE)
set(OT_PLATFORM_LIB_RCP_SPI     "example_vendor-rcp-spi"    CACHE STRING "default efr32 RCP platform library" FORCE)
set(OT_PLATFORM_LIB_RCP_UART    "example_vendor-rcp-uart"   CACHE STRING "default efr32 RCP platform library" FORCE)
