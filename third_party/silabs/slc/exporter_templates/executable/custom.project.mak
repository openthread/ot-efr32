####################################################################
# Automatically-generated file. Do not edit!                       #
# CMake Version 0                                                  #
#                                                                  #
#                                                                  #
# This file will be used to generate a .cmake file that will       #
# replace all existing CMake files for the GSDK.                   #
#                                                                  #
####################################################################
{% from 'macros.jinja' import prepare_path,compile_flags,print_linker_flags with context -%}

include(${PROJECT_SOURCE_DIR}/third_party/silabs/cmake/utility.cmake)

# ==============================================================================
# Platform library
# ==============================================================================
add_library(silabs-efr32-sdk-rcp)

set_target_properties(silabs-efr32-sdk-rcp
    PROPERTIES
        C_STANDARD 99
        CXX_STANDARD 11
)

# ==============================================================================
# Includes
# ==============================================================================
target_include_directories(silabs-efr32-sdk-rcp PUBLIC
{%- for include in C_CXX_INCLUDES %}
    {%- if ('sample-apps' not in include) %}
    {{ prepare_path(include) | replace('-I', '') | replace('\"', '') }}
    {%- endif %}
{%- endfor %}
)

target_include_directories(silabs-efr32-sdk-rcp
    PRIVATE
        ${OT_PUBLIC_INCLUDES}
)

# ==============================================================================
# Sources
# ==============================================================================
target_sources(silabs-efr32-sdk-rcp PRIVATE
{%- for source in (ALL_SOURCES | sort) %}
    {%- set source = prepare_path(source) -%}

    {#- Ignore crypto sources, PAL sources, and openthread sources #}
    {%- if ('util/third_party/crypto/mbedtls' not in source)
            and ('${PROJECT_SOURCE_DIR}/src/src' not in source)
            and ('${PROJECT_SOURCE_DIR}/openthread' not in source) %}
        {%- if source.endswith('.c') or source.endswith('.cpp') or source.endswith('.h') or source.endswith('.hpp') %}
    {{source}}
        {%- endif %}
    {%- endif %}
{%- endfor %}
)

{%- for source in (ALL_SOURCES | sort) -%}
    {%- set source = prepare_path(source) -%}

    {#- Ignore crypto sources, PAL sources, and openthread sources #}
    {%- if ('util/third_party/crypto/mbedtls' not in source)
            and ('${PROJECT_SOURCE_DIR}/src/src' not in source)
            and ('${PROJECT_SOURCE_DIR}/openthread' not in source) %}
        {%- if source.endswith('.s') or source.endswith('.S') %}
target_sources(silabs-efr32-sdk-rcp PRIVATE {{source}})
set_property(SOURCE {{source}} PROPERTY LANGUAGE C)
        {%- endif %}
    {%- endif %}
{%- endfor %}

target_link_libraries(silabs-efr32-sdk-rcp
    PUBLIC
        silabs-mbedtls-rcp
{%- for source in SYS_LIBS+USER_LIBS %}
        {{prepare_path(source)}}
{%- endfor %}
        openthread-efr32-rcp-config
        ot-config
)

{% if EXT_CFLAGS+EXT_CXX_FLAGS -%}
# ==============================================================================
#  Compile Options
# ==============================================================================
target_compile_options(silabs-efr32-sdk-rcp PRIVATE
    -Wno-unused-parameter
    -Wno-missing-field-initializers
    {{ compile_flags() }}
)
{%- endif %} {# compile_options #}


{%- if (EXT_LD_FLAGS + EXT_DEBUG_LD_FLAGS + EXT_RELEASE_LD_FLAGS) %}
# ==============================================================================
#  Linker Flags
# ==============================================================================
target_link_options(silabs-efr32-sdk-rcp PRIVATE {{ print_linker_flags() }}
)
{%- endif %} {# linker_flags #}



# ==============================================================================
#  EXT_C_FLAGS
# ==============================================================================
{%- for flag in EXT_C_FLAGS %}
#    {{flag}}
{%- endfor %}

# ==============================================================================
#  EXT_DEBUG_C_FLAGS
# ==============================================================================
{%- for flag in EXT_DEBUG_C_FLAGS %}
#    {{flag}}
{%- endfor %}

# ==============================================================================
#  EXT_RELEASE_C_FLAGS
# ==============================================================================
{%- for flag in EXT_RELEASE_C_FLAGS %}
#    {{flag}}
{%- endfor %}

# ==============================================================================
#  EXT_CXX_FLAGS
# ==============================================================================
{%- for flag in EXT_CXX_FLAGS %}
#    {{flag}}
{%- endfor %}

# ==============================================================================
#  EXT_DEBUG_CXX_FLAGS
# ==============================================================================
{%- for flag in EXT_DEBUG_CXX_FLAGS %}
#    {{flag}}
{%- endfor %}

# ==============================================================================
#  EXT_RELEASE_CXX_FLAGS
# ==============================================================================
{%- for flag in EXT_RELEASE_CXX_FLAGS %}
#    {{flag}}
{%- endfor %}

# ==============================================================================
#  EXT_ASM_FLAGS
# ==============================================================================
{%- for flag in EXT_ASM_FLAGS %}
#    {{flag}}
{%- endfor %}

# ==============================================================================
#  EXT_DEBUG_ASM_FLAGS
# ==============================================================================
{%- for flag in EXT_DEBUG_ASM_FLAGS %}
#    {{flag}}
{%- endfor %}

# ==============================================================================
#  EXT_RELEASE_ASM_FLAGS
# ==============================================================================
{%- for flag in EXT_RELEASE_ASM_FLAGS %}
#    {{flag}}
{%- endfor %}

# ==============================================================================
#  EXT_LD_FLAGS
# ==============================================================================
{%- for flag in EXT_LD_FLAGS %}
#    {{flag}}
{%- endfor %}

# ==============================================================================
#  EXT_DEBUG_LD_FLAGS
# ==============================================================================
{%- for flag in EXT_DEBUG_LD_FLAGS %}
#    {{flag}}
{%- endfor %}

# ==============================================================================
#  EXT_RELEASE_LD_FLAGS
# ==============================================================================
{%- for flag in EXT_RELEASE_LD_FLAGS %}
#    {{flag}}
{%- endfor %}
