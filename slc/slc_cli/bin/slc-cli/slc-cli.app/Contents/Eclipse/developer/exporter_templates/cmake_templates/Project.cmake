set(SDK_PATH "{{SDK_PATH  | replace('\\', '/')}}")
set(COPIED_SDK_PATH "{{COPIED_SDK_PATH  | replace('\\', '/')}}")

add_library(slc_{{PROJECT_NAME}} OBJECT
{%- for source in (ALL_SOURCES | sort) %}
{%- if source.endswith(('.c', '.cpp', '.cc', '.s', '.S')) %}
{%- if '$(COPIED_SDK_PATH)' in source %}
    "../{{ source | replace('\\', '/') | replace('$(COPIED_SDK_PATH)','${COPIED_SDK_PATH}') }}"
{%- elif '$(SDK_PATH)' in source %}
    "{{ source | replace('\\', '/') | replace('$(SDK_PATH)','${SDK_PATH}') }}"
{%- else %}
    "../{{ source | replace('\\', '/') }}"
{%- endif %}
{%- endif %}
{%- endfor %}
)

target_include_directories(slc_{{PROJECT_NAME}} PUBLIC
{%- for include in C_CXX_INCLUDES %}
{%- if '$(COPIED_SDK_PATH)' in include %}
    "../{{include | replace("-I","",1) | replace('\\', '/') | replace('"','') | replace('$(COPIED_SDK_PATH)','${COPIED_SDK_PATH}') }}{# " #}"
{%- elif '$(SDK_PATH)' in include %}
    "{{include | replace("-I","",1) | replace('\\', '/') | replace('"','') | replace('$(SDK_PATH)','${SDK_PATH}') }}{# " #}"
{%- else %}
   "../{{include | replace("-I","",1) | replace('\\', '/') | replace('"','') }}{# " #}"
{%- endif %}
{%- endfor %}
)

target_compile_definitions(slc_{{PROJECT_NAME}} PUBLIC
{%- for define in C_CXX_DEFINE_STR %}
    "{{define | replace("'-D", "") | reverse | replace("'","",1) | reverse | replace("\"", "\\\"") }}"
{%- endfor %}
)

target_link_libraries(slc_{{PROJECT_NAME}} PUBLIC
    "-Wl,--start-group"
{%- for source in SYS_LIBS+USER_LIBS %}
{%- if source.startswith("-l") %}
    "{{source | replace('-l','',1) }}"
{%- elif '$(COPIED_SDK_PATH)' in source %}
   "${CMAKE_CURRENT_LIST_DIR}/../{{source | replace('\\', '/') | replace('"','') | replace('$(COPIED_SDK_PATH)','${COPIED_SDK_PATH}') }}{# " #}"
{%- elif '$(SDK_PATH)' in source %}
   "{{source | replace('\\', '/') | replace('"','') | replace('$(SDK_PATH)','${SDK_PATH}') }}{# " #}"
{%- else %}
    "${CMAKE_CURRENT_LIST_DIR}/../{{source | replace('\\', '/') | replace('"','') }}{# " #}"
{%- endif %}
{%- endfor %}
    "-Wl,--end-group"
)

{%- set standard = namespace(c=0, cxx=0, cxx_extensions=false) %}
target_compile_options(slc_{{PROJECT_NAME}} PUBLIC
{%- for flag in EXT_CFLAGS %}
{%- if flag == "-std=c99" %}{% set standard.c = 99 %}
{%- elif flag == "-std=c11" %}{% set standard.c = 11 %}
{%- elif flag == "-std=c17" %}{% set standard.c = 17 %}
{%- elif flag == "-std=c18" %}{% set standard.c = 17 %}
{%- elif flag == "-std=c23" %}{% set standard.c = 23 %}
{%- elif " " in flag %}
    "$<$<COMPILE_LANGUAGE:C>:SHELL:{{flag}}>"
{%- else %}
    $<$<COMPILE_LANGUAGE:C>:{{flag}}>
{%- endif %}
{%- endfor %}
{%- for flag in EXT_CXX_FLAGS %}
{%- if flag == "-std=c++11" %}{% set standard.cxx = 11 %}
{%- elif flag == "-std=c++14" %}{% set standard.cxx = 14 %}
{%- elif flag == "-std=c++17" %}{% set standard.cxx = 17 %}
{%- elif flag == "-std=c++20" %}{% set standard.cxx = 20 %}
{%- elif flag == "-std=c++23" %}{% set standard.cxx = 23 %}
{%- elif flag == "-std=gnu++11" %}{% set standard.cxx = 11 %}{% set standard.cxx_extensions = true %}
{%- elif flag == "-std=gnu++14" %}{% set standard.cxx = 14 %}{% set standard.cxx_extensions = true %}
{%- elif flag == "-std=gnu++17" %}{% set standard.cxx = 17 %}{% set standard.cxx_extensions = true %}
{%- elif flag == "-std=gnu++20" %}{% set standard.cxx = 20 %}{% set standard.cxx_extensions = true %}
{%- elif flag == "-std=gnu++23" %}{% set standard.cxx = 23 %}{% set standard.cxx_extensions = true %}
{%- elif " " in flag %}
    "$<$<COMPILE_LANGUAGE:CXX>:SHELL:{{flag}}>"
{%- else %}
    $<$<COMPILE_LANGUAGE:CXX>:{{flag}}>
{%- endif %}
{%- endfor %}
{%- for flag in EXT_ASM_FLAGS %}
{%- if " " in flag %}
    "$<$<COMPILE_LANGUAGE:ASM>:SHELL:{{flag}}>"
{%- else %}
    $<$<COMPILE_LANGUAGE:ASM>:{{flag}}>
{%- endif %}
{%- endfor %}
)

set(post_build_command {{ POST_BUILD_ARGS | replace('@"$(POST_BUILD_EXE)"',"${POST_BUILD_EXE}") | replace("$(POST_BUILD_EXE)","${POST_BUILD_EXE}") | replace("$(OUTPUT_DIR)","$<TARGET_FILE_DIR:" + PROJECT_NAME + ">") }})

{%- if standard.c > 0 %}
set_property(TARGET slc_{{PROJECT_NAME}} PROPERTY C_STANDARD {{ standard.c }})
{%- endif %}
{%- if standard.cxx > 0 %}
set_property(TARGET slc_{{PROJECT_NAME}} PROPERTY CXX_STANDARD {{ standard.cxx }})
set_property(TARGET slc_{{PROJECT_NAME}} PROPERTY CXX_EXTENSIONS {% if standard.cxx_extensions %}ON{% else %}OFF{% endif %})
{%- endif %}

target_link_options(slc_{{PROJECT_NAME}} INTERFACE
{%- for flag in EXT_LD_FLAGS %}
{%- if flag.startswith("-T") %}
    {{ flag | replace('-T"', '-T${CMAKE_CURRENT_LIST_DIR}/../') | reverse | replace('"','',1) | reverse }}
{%- elif flag.startswith("-L") and not flag.startswith("-L/") and not flag.startswith("-LC:") %}
    {{ flag | replace("-L", "-L${CMAKE_CURRENT_LIST_DIR}/../") }}
{%- elif flag.startswith("@") and not flag.startswith("@/") and not flag.startswith("@C:") %}
    {{ flag | replace("@", "@${CMAKE_CURRENT_LIST_DIR}/../") }}
{%- elif " " in flag %}
    "SHELL:{{ flag | replace("$(OUTPUT_DIR)","$<TARGET_FILE_DIR:" + PROJECT_NAME + ">") | replace("$(PROJECTNAME)", PROJECT_NAME) }}"
{%- else %}
    {{ flag | replace("$(OUTPUT_DIR)","$<TARGET_FILE_DIR:" + PROJECT_NAME + ">") | replace("$(PROJECTNAME)", PROJECT_NAME) }}
{%- endif %}
{%- endfor %}
)

# BEGIN_SIMPLICITY_STUDIO_METADATA={{SIMPLICITY_STUDIO_METADATA}}=END_SIMPLICITY_STUDIO_METADATA
