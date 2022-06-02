####################################################################
# Automatically-generated file. Do not edit!                       #
# CMake Version 0                                                  #
#                                                                  #
#                                                                  #
# This file will be used to generate a .cmake file that will       #
# replace all existing CMake files for the GSDK.                   #
#                                                                  #
####################################################################
{% from 'macros.jinja' import prepare_path,compile_flags,linker_flags with context -%}

include(${PROJECT_SOURCE_DIR}/third_party/silabs/cmake/utility.cmake)
include(silabs-efr32-sdk-rcp.cmake)

# ==============================================================================
# Platform library
# ==============================================================================
add_library(openthread-efr32-rcp
    $<TARGET_OBJECTS:openthread-platform-utils>
)

add_library(openthread-efr32-rcp-config INTERFACE)

set(OT_PLATFORM_LIB_RCP openthread-efr32-rcp)
set(OT_MBEDTLS_RCP silabs-mbedtls-rcp)

set_target_properties(openthread-efr32-rcp
    PROPERTIES
        C_STANDARD 99
        CXX_STANDARD 11
)

target_include_directories(ot-config INTERFACE
{%- for include in C_CXX_INCLUDES %}
    {%- set include = prepare_path(include) | replace('-I', '') | replace('\"', '') %}
    {%- if not (('sample-apps' in include) or ('autogen' == include) or ('config' == include)) %}
    {{ include }}
    {%- endif %}
{%- endfor %}
)

target_include_directories(openthread-efr32-rcp-config INTERFACE
    autogen
    config
)

target_link_libraries(openthread-radio PUBLIC openthread-efr32-rcp-config)

target_include_directories(openthread-efr32-rcp
    PRIVATE
        ${OT_PUBLIC_INCLUDES}
)

target_sources(openthread-efr32-rcp
    PRIVATE
{%- for source in (ALL_SOURCES | sort) %}
    {%- set source = prepare_path(source) -%}

    {#- Only take PAL sources #}
    {%- if ('{PROJECT_SOURCE_DIR}/src/src' in source) -%}
        {%- if source.endswith('.c') or source.endswith('.cpp') or source.endswith('.h') or source.endswith('.hpp') %}
        {{source}}
        {%- endif %}
    {%- endif %}
{%- endfor %}
)

{%- for source in (ALL_SOURCES | sort) %}
    {%- set source = prepare_path(source) -%}

    {#- Only take PAL sources #}
    {%- if ('${PROJECT_SOURCE_DIR}/src/src' in source) -%}
        {%- if source.endswith('.s') or source.endswith('.S') %}
target_sources(openthread-efr32-rcp PRIVATE {{source}})
set_property(SOURCE {{source}} PROPERTY LANGUAGE C)
        {%- endif %}
    {%- endif %}
{%- endfor %}

target_compile_definitions(ot-config INTERFACE
    {%- for define in C_CXX_DEFINES %}
        {%- if not ( define.startswith("MBEDTLS_PSA_CRYPTO_CLIENT") or ("OPENTHREAD_RADIO" == define) or ("OPENTHREAD_FTD" == define) or ("OPENTHREAD_MTD" == define) or ("OPENTHREAD_COPROCESSOR" == define) ) %}
        {{define}}={{C_CXX_DEFINES[define]}}
        {%- endif %}
    {%- endfor %}
)

target_compile_definitions(openthread-efr32-rcp-config INTERFACE
    {%- for define in C_CXX_DEFINES %}
        {%- if define.startswith("MBEDTLS_PSA_CRYPTO_CLIENT") %}
        {{define}}={{C_CXX_DEFINES[define]}}
        {%- endif %}
    {%- endfor %}
)

target_compile_definitions(openthread-efr32-rcp PUBLIC
    {%- for define in C_CXX_DEFINES %}
        {%- if ( ("OPENTHREAD_RADIO" == define) or ("OPENTHREAD_FTD" == define) or ("OPENTHREAD_MTD" == define) or ("OPENTHREAD_COPROCESSOR" == define) ) %}
        {{define}}={{C_CXX_DEFINES[define]}}
        {%- endif %}
    {%- endfor %}
)

set(LD_FILE "${CMAKE_CURRENT_SOURCE_DIR}/autogen/linkerfile.ld")
set(silabs-efr32-sdk-rcp_location $<TARGET_FILE:silabs-efr32-sdk-rcp>)
target_link_libraries(openthread-efr32-rcp
    PUBLIC
{%- for lib_name in SYS_LIBS+USER_LIBS %}
    {%- set lib_name = prepare_path(lib_name) -%}

    {#- Ignore GSDK static libs. These will be added below #}
    {%- if 'SILABS_GSDK_DIR' not in lib_name %}
        {{lib_name | replace('\\', '/') | replace(' ', '\\ ') | replace('"','')}}
    {%- endif %}
{%- endfor %}
        openthread-efr32-rcp-config

    PRIVATE
        -T${LD_FILE}
        -Wl,--gc-sections
        -Wl,--whole-archive ${silabs-efr32-sdk-rcp_location} -Wl,--no-whole-archive
        jlinkrtt
        ot-config
)

{% if EXT_CFLAGS+EXT_CXX_FLAGS -%}
target_compile_options(openthread-efr32-rcp PRIVATE {{ compile_flags() }}
)
{%- endif %} {# compile_options #}

target_compile_definitions(openthread-efr32-rcp PRIVATE
    RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT=1
)

{# ========================================================================= #}
{#- Linker Flags #}
{%- if (EXT_LD_FLAGS + EXT_DEBUG_LD_FLAGS + EXT_RELEASE_LD_FLAGS) %}
target_link_options(openthread-efr32-rcp PRIVATE {{ linker_flags() }}
)
{%- endif %} {# linker_flags #}

{% set lib_list = SYS_LIBS + USER_LIBS %}
{%- if lib_list %}
# ==============================================================================
# Static libraries from GSDK
# ==============================================================================
{# Generate a list of GSDK libs #}
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
    target_link_libraries(openthread-efr32-rcp PUBLIC ${imported_lib_name})
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
