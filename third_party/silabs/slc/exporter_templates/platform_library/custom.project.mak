####################################################################
# Automatically-generated file. Do not edit!                       #
# CMake Version 1                                                  #
#                                                                  #
# This file will be used to generate a .cmake file that will       #
# replace all existing CMake files for the GSDK.                   #
#                                                                  #
####################################################################
{%  from
        'macros.jinja'
    import
        prepare_path,
        compile_flags,
        print_linker_flags,
        print_all_jinja_vars
    with context -%}

include(${PROJECT_SOURCE_DIR}/third_party/silabs/cmake/utility.cmake)

# ==============================================================================
# Library of platform dependencies from GSDK and generated config files
# ==============================================================================
add_library({{PROJECT_NAME}}-sdk)

set_target_properties({{PROJECT_NAME}}-sdk
    PROPERTIES
        C_STANDARD 99
        CXX_STANDARD 11
)

# ==============================================================================
# Includes
# ==============================================================================
target_include_directories({{PROJECT_NAME}}-sdk PUBLIC
{%- for include in C_CXX_INCLUDES %}
    {%- if ('sample-apps' not in include) %}
    {{ prepare_path(include) | replace('-I', '') | replace('\"', '') }}
    {%- endif %}
{%- endfor %}
)

target_include_directories({{PROJECT_NAME}}-sdk PRIVATE
    ${OT_PUBLIC_INCLUDES}
)

# ==============================================================================
# Sources
# ==============================================================================
target_sources({{PROJECT_NAME}}-sdk PRIVATE
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

{%- for source in (ALL_SOURCES | sort) %}
    {%- set source = prepare_path(source) -%}

    {#- Ignore crypto sources, PAL sources, and openthread sources #}
    {%- if ('util/third_party/crypto/mbedtls' not in source)
            and ('${PROJECT_SOURCE_DIR}/src/src' not in source)
            and ('${PROJECT_SOURCE_DIR}/openthread' not in source) %}
        {%- if source.endswith('.s') or source.endswith('.S') %}

target_sources({{PROJECT_NAME}}-sdk PRIVATE {{source}})
set_property(SOURCE {{source}} PROPERTY LANGUAGE C)
        {%- endif %}
    {%- endif %}
{%- endfor %}

{% if EXT_CFLAGS+EXT_CXX_FLAGS -%}
# ==============================================================================
#  Compile Options
# ==============================================================================
target_compile_options({{PROJECT_NAME}}-sdk PRIVATE
    -Wno-unused-parameter
    -Wno-missing-field-initializers
    {{ compile_flags() }}
)
{%- endif %} {#- compile_options #}

# ==============================================================================
# Linking
# ==============================================================================
target_link_libraries({{PROJECT_NAME}}-sdk
    PUBLIC
        {{PROJECT_NAME}}-mbedtls
    PRIVATE
{%- for source in SYS_LIBS+USER_LIBS %}
        {{prepare_path(source)}}
{%- endfor %}
        {{PROJECT_NAME}}-config
        ot-config
)

{% set linker_flags = EXT_LD_FLAGS + EXT_DEBUG_LD_FLAGS + EXT_RELEASE_LD_FLAGS %}
{%- if linker_flags -%}
# ==============================================================================
#  Linker Flags
# ==============================================================================
target_link_options({{PROJECT_NAME}}-sdk PRIVATE {{ print_linker_flags() }}
)
{%- endif %} {# linker_flags #}

{#- ========================================================================= #}
{#- Debug                                                                     #}
{#- ========================================================================= #}

{#- Change debug_template to true to print all jinja vars #}
{%- set debug_template = false %}
{%- if debug_template %}
{{ print_all_jinja_vars() }}
{%- endif %}
