####################################################################
# Automatically-generated file. Do not edit!                       #
# CMake Version 1                                                  #
#                                                                  #
# This file will be used to generate a .cmake file that will       #
# replace all existing CMake files for the GSDK.                   #
#                                                                  #
####################################################################
{% from 'macros.jinja' import prepare_path,compile_flags,print_linker_flags with context -%}

include(${PROJECT_SOURCE_DIR}/third_party/silabs/cmake/utility.cmake)
include(silabs-efr32-sdk-rcp.cmake)

# ==============================================================================
# Platform library
# ==============================================================================
add_executable({{ PROJECT_NAME }})

target_include_directories({{ PROJECT_NAME }}
{%- for include in C_CXX_INCLUDES %}
    {%- set include = prepare_path(include) | replace('-I', '') | replace('\"', '') %}
    {%- if not (('sample-apps' in include) or ('autogen' == include) or ('config' == include)) %}
    {{ include }}
    {%- endif %}
{%- endfor %}
    ${OT_PUBLIC_INCLUDES}
)

target_sources({{ PROJECT_NAME }}
{%- for source in (ALL_SOURCES | sort) %}
    {%- set source = prepare_path(source) -%}

    {%- if source.endswith('.c') or source.endswith('.cpp') or source.endswith('.h') or source.endswith('.hpp') %}
    {{source}}
    {%- endif %}
{%- endfor %}
)

{%- for source in (ALL_SOURCES | sort) %}
    {%- set source = prepare_path(source) -%}

    {%- if source.endswith('.s') or source.endswith('.S') %}
target_sources({{ PROJECT_NAME }} PRIVATE {{source}})
set_property(SOURCE {{source}} PROPERTY LANGUAGE C)
    {%- endif %}
{%- endfor %}

target_compile_definitions({{ PROJECT_NAME }}
    {%- for define in C_CXX_DEFINES %}
        {{define}}={{C_CXX_DEFINES[define]}}
    {%- endfor %}
)

set(LD_FILE "${CMAKE_CURRENT_SOURCE_DIR}/autogen/linkerfile.ld")
set(silabs-efr32-sdk-rcp_location $<TARGET_FILE:silabs-efr32-sdk-rcp>)
target_link_libraries({{ PROJECT_NAME }}
    PUBLIC
{%- for lib_name in SYS_LIBS+USER_LIBS %}
    {%- set lib_name = prepare_path(lib_name) -%}

    {#- Ignore GSDK static libs. These will be added below #}
    {%- if 'SILABS_GSDK_DIR' not in lib_name %}
        {{lib_name | replace('\\', '/') | replace(' ', '\\ ') | replace('"','')}}
    {%- endif %}
{%- endfor %}
        {{ PROJECT_NAME }}-config

    PRIVATE
        -T${LD_FILE}
        -Wl,--gc-sections
        -Wl,--whole-archive ${silabs-efr32-sdk-rcp_location} -Wl,--no-whole-archive
        jlinkrtt
        ot-config
)

{% if EXT_CFLAGS+EXT_CXX_FLAGS -%}
target_compile_options({{ PROJECT_NAME }} PRIVATE {{ compile_flags() }}
)
{%- endif %} {# compile_options #}

{# ========================================================================= #}
{#- Linker Flags #}
{%- if (EXT_LD_FLAGS + EXT_DEBUG_LD_FLAGS + EXT_RELEASE_LD_FLAGS) %}
target_link_options({{ PROJECT_NAME }} PRIVATE {{ print_linker_flags() }}
)
{%- endif %} {# linker_flags #}

{% set lib_list = SYS_LIBS + USER_LIBS %}
{%- if lib_list %}
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

foreach(lib_file ${GSDK_LIBS})
    # Parse lib name, stripping .a extension
    get_filename_component(lib_name ${lib_file} NAME_WE)
    set(imported_lib_name "silabs-${lib_name}")

    # Add as an IMPORTED lib
    add_library(${imported_lib_name} STATIC IMPORTED)
    set_target_properties(${imported_lib_name}
        PROPERTIES
            IMPORTED_LOCATION "${lib_file}"
            IMPORTED_LINK_INTERFACE_LIBRARIES silabs-efr32-sdk-rcp
    )
    target_link_libraries({{ PROJECT_NAME }} PUBLIC ${imported_lib_name})
endforeach()

{%- endif %} {# lib_list #}



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
#  EXT_CFLAGS
# ==============================================================================
{%- for flag in EXT_CFLAGS %}
#    {{flag}}
{%- endfor %}

# ==============================================================================
#  EXT_DEBUG_CFLAGS
# ==============================================================================
{%- for flag in EXT_DEBUG_CFLAGS %}
#    {{flag}}
{%- endfor %}

# ==============================================================================
#  EXT_RELEASE_CFLAGS
# ==============================================================================
{%- for flag in EXT_RELEASE_CFLAGS %}
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
