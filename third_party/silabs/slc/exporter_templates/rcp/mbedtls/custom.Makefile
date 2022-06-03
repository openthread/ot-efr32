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
{% from 'macros.jinja' import prepare_path %}

include(${PROJECT_SOURCE_DIR}/third_party/silabs/cmake/utility.cmake)

add_library(openthread-efr32-rcp-mbedtls)

set_target_properties(openthread-efr32-rcp-mbedtls
    PROPERTIES
        C_STANDARD 99
        CXX_STANDARD 11
)

set(SILABS_MBEDTLS_DIR "${SILABS_GSDK_DIR}/util/third_party/crypto/mbedtls")

target_compile_definitions(openthread-efr32-rcp-mbedtls PRIVATE ${OT_PLATFORM_DEFINES})

{%- set linker_flags = EXT_LD_FLAGS + EXT_DEBUG_LD_FLAGS %}
{%- if linker_flags %}
target_link_options(openthread-efr32-rcp-mbedtls PRIVATE
{%- for flag in linker_flags %}
    {{ prepare_path(flag) }}
{%- endfor %}
)
{%- endif %}

target_link_libraries(openthread-efr32-rcp-mbedtls
    PRIVATE
        ot-config
        openthread-efr32-rcp-config
)

{%- if C_CXX_INCLUDES %}
target_include_directories(openthread-efr32-rcp-mbedtls
    PUBLIC
{%- for include in C_CXX_INCLUDES %}
{%- if ('util/third_party/crypto' in include) or ('platform' in include) %}
        {{ prepare_path(include) | replace('-I', '') | replace('\"', '') }}
{%- endif %}
{%- endfor %}
)
{%- endif %}

set(SILABS_MBEDTLS_SOURCES
{%- for source in (ALL_SOURCES | sort) %}
    {%- set source = prepare_path(source) -%}

    {#- Filter-out non-mbedtls sources #}
    {%- if 'SILABS_MBEDTLS_DIR' in source %}
    {{source}}
    {%- endif %}
{%- endfor %}
)

target_sources(openthread-efr32-rcp-mbedtls PRIVATE ${SILABS_MBEDTLS_SOURCES})



# ==============================================================================
#  C_CXX_INCLUDES
# ==============================================================================
{%- for include in C_CXX_INCLUDES %}
#    {{ prepare_path(include) | replace('-I', '') | replace('\"', '') }}
{%- endfor %}

# ==============================================================================
#  SOURCES
# ==============================================================================
{%- for source in (ALL_SOURCES | sort) %}
#    {{ prepare_path(source) }}
{%- endfor %}

# ==============================================================================
#  C_CXX_DEFINES
# ==============================================================================
{%- for define in C_CXX_DEFINES %}
#    {{define}}={{C_CXX_DEFINES[define]}}
{%- endfor %}

# ==============================================================================
#  SYS_LIBS+USER_LIBS
# ==============================================================================
{%- for lib_name in SYS_LIBS+USER_LIBS %}
#    {{ prepare_path(lib_name) | replace('\\', '/') | replace(' ', '\\ ') | replace('"','') }}
{%- endfor %}

# ==============================================================================
#  EXT_CFLAGS + EXT_CXX_FLAGS
# ==============================================================================
{%- set compile_options = EXT_CFLAGS + EXT_CXX_FLAGS %}
{%- for flag in compile_options %}
#    {{flag}}
{%- endfor %}

# ==============================================================================
#  EXT_CFLAGS + EXT_CXX_FLAGS
# ==============================================================================
{%- set linker_flags = EXT_LD_FLAGS + EXT_DEBUG_LD_FLAGS %}
{%- for flag in linker_flags %}
#    {{ prepare_path(flag) }}
{%- endfor %}
