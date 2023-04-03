####################################################################
# Automatically-generated file. Do not edit!                       #
# CMake Version 1                                                  #
####################################################################
#
#  Copyright (c) 2022, The OpenThread Authors.
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
{%  from
        'macros.jinja'
    import
        prepare_path,
        compile_flags,
        print_linker_flags,
        print_all_jinja_vars,
        openthread_device_type,
        dict_contains_key_starting_with
    with context -%}

include(${PROJECT_SOURCE_DIR}/third_party/silabs/cmake/utility.cmake)
include({{PROJECT_NAME}}-mbedtls.cmake)

# ==============================================================================
# Platform library
# ==============================================================================

# Create the platform as an OBJECT library target
#
# **NOTE**: Defining the library target as STATIC will cause linking issues
#           where the WEAK implementation of a function is taken when both
#           weakly and strongly defined implementations of the same function
#           exist.
add_library({{PROJECT_NAME}} OBJECT
    $<TARGET_OBJECTS:openthread-platform-utils>
)

# Interface lib for sharing efr32 config to relevant targets
add_library({{PROJECT_NAME}}-config INTERFACE)

set_target_properties({{PROJECT_NAME}}
    PROPERTIES
        C_STANDARD 99
        CXX_STANDARD 11
)

# ==============================================================================
# Includes
# ==============================================================================
target_include_directories(ot-config INTERFACE
{%- for include in C_CXX_INCLUDES %}
    {%- set include = prepare_path(include) | replace('-I', '') | replace('\"', '') %}
    {{ include }}
{%- endfor %}
)

target_include_directories({{PROJECT_NAME}}-config BEFORE INTERFACE
    autogen
    config
)

target_include_directories({{PROJECT_NAME}} PRIVATE
    ${OT_PUBLIC_INCLUDES}
)

# ==============================================================================
# Sources
# ==============================================================================
target_sources({{PROJECT_NAME}} PRIVATE
{%- for source in (ALL_SOURCES | sort) %}
    {%- set source = prepare_path(source) -%}

    {#- Only take non-crypto sources #}
    {%- if not ('util/third_party/crypto' in source) %}
        {%- if source.endswith('.c') or source.endswith('.cpp') or source.endswith('.h') or source.endswith('.hpp') %}
    {{source}}
        {%- endif %}
    {%- endif %}
{%- endfor %}
)

{%- for source in (ALL_SOURCES | sort) %}
    {%- set source = prepare_path(source) -%}

    {#- Only take PAL sources #}
        {%- if source.endswith('.s') or source.endswith('.S') %}
target_sources({{PROJECT_NAME}} PRIVATE {{source}})
set_property(SOURCE {{source}} PROPERTY LANGUAGE C)
        {%- endif %}
{%- endfor %}

# ==============================================================================
# Compile definitions
# ==============================================================================

target_compile_definitions(ot-config INTERFACE
{%- for define in C_CXX_DEFINES %}
    {%- if not (define.startswith("MBEDTLS_")
                or define.startswith("OPENTHREAD_")
                ) %}
    {{define}}={{C_CXX_DEFINES[define]}}
    {%- endif %}
{%- endfor %}
)

{% if dict_contains_key_starting_with(C_CXX_DEFINES, "OPENTHREAD_") -%}
target_compile_definitions(ot-config INTERFACE
{%- for define in C_CXX_DEFINES %}
    {%- if define.startswith("OPENTHREAD_")
            and not ( ("OPENTHREAD_RADIO" == define)
                or ("OPENTHREAD_FTD" == define)
                or ("OPENTHREAD_MTD" == define)
                or ("OPENTHREAD_COPROCESSOR" == define) ) %}
    {{define}}={{C_CXX_DEFINES[define]}}
    {%- endif %}
{%- endfor %}
)

{% endif -%}

{% if dict_contains_key_starting_with(C_CXX_DEFINES, "MBEDTLS_")
        or dict_contains_key_starting_with(C_CXX_DEFINES, "PSA_")
        -%}
target_compile_definitions({{PROJECT_NAME}}-mbedtls-config INTERFACE
{%- for define in C_CXX_DEFINES %}
    {%- if define.startswith("MBEDTLS_") or
           define.startswith("PSA_")
     %}
    {{define}}={{C_CXX_DEFINES[define]}}
    {%- endif %}
{%- endfor %}
)

{% endif -%}

target_include_directories({{PROJECT_NAME}}-mbedtls-config BEFORE INTERFACE
    autogen
    config
)

target_link_libraries({{PROJECT_NAME}}-mbedtls PUBLIC {{PROJECT_NAME}}-mbedtls-config)
target_link_libraries({{PROJECT_NAME}} PUBLIC {{PROJECT_NAME}}-mbedtls-config)

{% if PROJECT_NAME.startswith("openthread-efr32-rcp") -%}
target_link_libraries(ot-config-radio INTERFACE {{PROJECT_NAME}}-mbedtls-config)
target_link_libraries(ot-config-radio INTERFACE {{PROJECT_NAME}}-config)
{% elif PROJECT_NAME.startswith("openthread-efr32-mtd") -%}
target_link_libraries(ot-config-ftd INTERFACE {{PROJECT_NAME}}-mbedtls-config)
target_link_libraries(ot-config-ftd INTERFACE {{PROJECT_NAME}}-config)
{% elif PROJECT_NAME.startswith("openthread-efr32-ftd") -%}
target_link_libraries(ot-config-mtd INTERFACE {{PROJECT_NAME}}-mbedtls-config)
target_link_libraries(ot-config-mtd INTERFACE {{PROJECT_NAME}}-config)
{% elif PROJECT_NAME.startswith("openthread-efr32-soc") -%}
target_link_libraries(ot-config-ftd INTERFACE {{PROJECT_NAME}}-mbedtls-config)
target_link_libraries(ot-config-mtd INTERFACE {{PROJECT_NAME}}-mbedtls-config)
target_link_libraries(ot-config-ftd INTERFACE {{PROJECT_NAME}}-config)
target_link_libraries(ot-config-mtd INTERFACE {{PROJECT_NAME}}-config)
{% endif -%}

{% if dict_contains_key_starting_with(C_CXX_DEFINES, "MBEDTLS_PSA_CRYPTO_CLIENT") -%}
target_compile_definitions({{PROJECT_NAME}}-config INTERFACE
{%- for define in C_CXX_DEFINES %}
    {%- if define.startswith("MBEDTLS_PSA_CRYPTO_CLIENT") %}
    {{define}}={{C_CXX_DEFINES[define]}}
    {%- endif %}
{%- endfor %}
)

{% endif -%}

{% if openthread_device_type() -%}
target_compile_definitions({{PROJECT_NAME}} PRIVATE {{ openthread_device_type() }}
)

{% endif -%}

{% if EXT_CFLAGS+EXT_CXX_FLAGS -%}
target_compile_options({{PROJECT_NAME}} PRIVATE {{ compile_flags() }}
)

{% endif -%}

# ==============================================================================
# Linking
# ==============================================================================
set(LD_FILE "${CMAKE_CURRENT_SOURCE_DIR}/autogen/linkerfile.ld")

target_link_libraries({{PROJECT_NAME}}
    PUBLIC
{%- for lib_name in SYS_LIBS+USER_LIBS %}
    {%- set lib_name = prepare_path(lib_name) -%}

    {#- Ignore GSDK static libs. These will be added below #}
    {%- if 'SILABS_GSDK_DIR' not in lib_name %}
        {{lib_name | replace('\\', '/') | replace(' ', '\\ ') | replace('"','')}}
    {%- endif %}
{%- endfor %}
        {{PROJECT_NAME}}-config

    PRIVATE
        -T${LD_FILE}
        -Wl,--gc-sections

        ot-config
)

{% set linker_flags = EXT_LD_FLAGS + EXT_DEBUG_LD_FLAGS + EXT_RELEASE_LD_FLAGS -%}
{%- if linker_flags -%}
target_link_options({{PROJECT_NAME}} PRIVATE {{ print_linker_flags() }}
)
{%- endif %} {#- linker_flags #}

{% set lib_list = SYS_LIBS + USER_LIBS %}
{%- if lib_list -%}
# ==============================================================================
# Static libraries from GSDK
# ==============================================================================
{#- Generate a list of GSDK libs #}
set(GSDK_LIBS
{%- for lib_name in lib_list -%}
    {#- Replace SDK_PATH with SILABS_GSDK_DIR #}
    {%- set lib_name = prepare_path(lib_name) -%}

    {%- if ('SILABS_GSDK_DIR' in lib_name) and ('jlink' not in lib_name) %}
    {{lib_name}}
    {%- endif %}
{%- endfor %}
)

# Import GSDK static libs and set a dependency on the GSDK library
# This will ensure proper linking order
foreach(lib_file ${GSDK_LIBS})
    # Parse lib name, stripping .a extension
    get_filename_component(lib_name ${lib_file} NAME_WE)
    set(imported_lib_name "silabs-${lib_name}")

    # Add as an IMPORTED lib
    add_library(${imported_lib_name} STATIC IMPORTED)
    set_target_properties(${imported_lib_name}
        PROPERTIES
            IMPORTED_LOCATION "${lib_file}"
            IMPORTED_LINK_INTERFACE_LIBRARIES {{PROJECT_NAME}}
    )
    target_link_libraries({{PROJECT_NAME}} PUBLIC ${imported_lib_name})
endforeach()

{%- endif %} {#- lib_list #}

{#- ========================================================================= #}
{#- Debug                                                                     #}
{#- ========================================================================= #}

{#- Change debug_template to true to print all jinja vars #}
{%- set debug_template = false %}
{%- if debug_template %}
{{ print_all_jinja_vars() }}
{%- endif %}
