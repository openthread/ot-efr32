####################################################################
# Automatically-generated file. Do not edit!                       #
# Makefile Version 15                                              #
####################################################################

BASE_SDK_PATH = {{SDK_PATH | replace('\\', '/') | replace(' ', '\\ ')}}
UNAME:=$(shell $(POSIX_TOOL_PATH)uname -s | $(POSIX_TOOL_PATH)sed -e 's/^\(CYGWIN\).*/\1/' | $(POSIX_TOOL_PATH)sed -e 's/^\(MINGW\).*/\1/')
ifeq ($(UNAME),MINGW)
# Translate "C:/super" into "/C/super" for MinGW make.
SDK_PATH := /$(shell $(POSIX_TOOL_PATH)echo $(BASE_SDK_PATH) | sed s/://)
endif
SDK_PATH ?= $(BASE_SDK_PATH)
COPIED_SDK_PATH ?= {{COPIED_SDK_PATH | replace('\\', '/') | replace(' ', '\\ ')}}

# This uses the explicit build rules below
PROJECT_SOURCE_FILES =

C_SOURCE_FILES   += $(filter %.c, $(PROJECT_SOURCE_FILES))
CXX_SOURCE_FILES += $(filter %.cpp, $(PROJECT_SOURCE_FILES))
CXX_SOURCE_FILES += $(filter %.cc, $(PROJECT_SOURCE_FILES))
ASM_SOURCE_FILES += $(filter %.s, $(PROJECT_SOURCE_FILES))
ASM_SOURCE_FILES += $(filter %.S, $(PROJECT_SOURCE_FILES))
LIB_FILES        += $(filter %.a, $(PROJECT_SOURCE_FILES))

C_DEFS += \
{% for define in C_CXX_DEFINE_STR %} {{define}}{{ " \\" if not loop.last}}
{% endfor %}
ASM_DEFS += \
{% for define in ASM_DEFINE_STR %} {{define}}{{ " \\" if not loop.last}}
{% endfor %}
INCLUDES += \
{% for include in C_CXX_INCLUDES %} {{include | replace('\\', '/') | replace(' ', '\\ ') | replace('"','')}}{{ " \\" if not loop.last}}
{% endfor %}
GROUP_START =
GROUP_END =

PROJECT_LIBS = \
{% for source in SYS_LIBS+USER_LIBS %} {{source | replace('\\', '/') | replace(' ', '\\ ') | replace('"','')}}{{ " \\" if not loop.last}}
{% endfor %}
LIBS += $(GROUP_START) $(PROJECT_LIBS) $(GROUP_END)

LIB_FILES += $(filter %.a, $(PROJECT_LIBS))

C_FLAGS += \
{% for flag in EXT_CFLAGS %} {{flag}}{{ " \\" if not loop.last}}
{% endfor %}
CXX_FLAGS += \
{% for flag in EXT_CXX_FLAGS %} {{flag}}{{ " \\" if not loop.last}}
{% endfor %}
ASM_FLAGS += \
{% for flag in EXT_ASM_FLAGS %} {{flag}}{{ " \\" if not loop.last}}
{% endfor %}
LD_FLAGS += \
{% for flag in EXT_LD_FLAGS %} {{flag}}{{ " \\" if not loop.last}}
{% endfor %}

####################################################################
# Pre/Post Build Rules                                             #
####################################################################
pre-build:
{%- if PRE_BUILD_ARGS %}
{%- if 'POST_BUILD_EXE' in POST_BUILD_ARGS %}
ifeq ($(POST_BUILD_EXE),)
		$(error POST_BUILD_EXE is not defined. Pre-Build cannot run. Please set the STUDIO_ADAPTER_PACK_PATH to the post-build tool when generating or override the variable for this makefile)
endif
{%- endif %}
	@$(POSIX_TOOL_PATH)echo 'Running Project Pre-Build'
	$(ECHO) {{PRE_BUILD_ARGS}}
{%- else %}
	# No pre-build defined
{%- endif %}

post-build: $(OUTPUT_DIR)/$(PROJECTNAME).out
{%- if POST_BUILD_ARGS %}
{%- if 'POST_BUILD_EXE' in POST_BUILD_ARGS %}
ifeq ($(POST_BUILD_EXE),)
		$(error POST_BUILD_EXE is not defined. Post-Build cannot run. Please set the STUDIO_ADAPTER_PACK_PATH to the post-build tool when generating or override the variable for this makefile)
endif
{%- endif %}
	@$(POSIX_TOOL_PATH)echo 'Running Project Post-Build'
	$(ECHO) {{POST_BUILD_ARGS}}
{%- else %}
	# No post-build defined
{%- endif %}

####################################################################
# SDK Build Rules                                                  #
####################################################################
{%- for source in (ALL_SOURCES | sort) %}
{%- if source.startswith('$(SDK_PATH)') or source.startswith('$(COPIED_SDK_PATH)') %}
{%- set base_path = 'sdk' %}
{%- else %}
{%- set base_path = 'project' %}
{%- endif %}
{%- if source.endswith('.c') %}
{%- set suffix = 'c.' %}
{%- set dep_var = 'CDEPS' %}
{%- set compiler = '$(CC) $(CFLAGS)' %}
{%- elif source.endswith('.cpp') %}
{%- set suffix = 'ppc.' %}
{%- set dep_var = 'CXXDEPS' %}
{%- set compiler = '$(CXX) $(CXXFLAGS)' %}
{%- elif source.endswith('.cc') %}
{%- set suffix = 'cc.' %}
{%- set dep_var = 'CXXDEPS' %}
{%- set compiler = '$(CXX) $(CXXFLAGS)' %}
{%- elif source.endswith('.s') %}
{%- set suffix = 's.' %}
{%- set dep_var = 'ASMDEPS_s' %}
{%- set compiler = '$(CC) $(ASMFLAGS)' %}
{%- elif source.endswith('.S') %}
{%- set suffix = 'S.' %}
{%- set dep_var = 'ASMDEPS_S' %}
{%- set compiler = '$(CC) $(ASMFLAGS)' %}
{%- else %}
{%- set suffix = 'invalid' %}
{%- endif %}
{%- if suffix != 'invalid' %}
{%- set source_file = source | replace('\\', '/') | replace(' ', '\\ ') -%}
{%- set dest_path = source_file | replace('$(SDK_PATH)/','') | replace('$(COPIED_SDK_PATH)/','') | replace('../', '_/') -%}
{%- set out_file = dest_path | reverse | replace(suffix,'o.',1) | reverse %}
{%- set dep_file = dest_path | reverse | replace(suffix,'d.',1) | reverse %}
$(OUTPUT_DIR)/{{base_path}}/{{out_file}}: {{source_file}}
	@$(POSIX_TOOL_PATH)echo 'Building {{source_file}}'
	@$(POSIX_TOOL_PATH)mkdir -p $(@D)
	$(ECHO){{compiler}} -c -o $@ {{source_file}}
{{dep_var}} += $(OUTPUT_DIR)/{{base_path}}/{{dep_file}}
OBJS += $(OUTPUT_DIR)/{{base_path}}/{{out_file}}
{% else -%}
{%- endif %}{% endfor %}
# Automatically-generated Simplicity Studio Metadata
# Please do not edit or delete these lines!
# SIMPLICITY_STUDIO_METADATA={{SIMPLICITY_STUDIO_METADATA}}=END_SIMPLICITY_STUDIO_METADATA
# END OF METADATA