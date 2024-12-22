####################################################################
# Automatically-generated file. Do not edit!                       #
####################################################################
.SUFFIXES:				# ignore builtin rules
.NOTPARALLEL:
.PHONY: all debug release clean pre-build post-build

# Default goal
all: debug

####################################################################
# Definitions                                                      #
####################################################################

# Values set by the initial generation
SOLUTION_NAME = {{SOLUTION_NAME}}
POST_BUILD_EXE_WIN = {{POST_BUILD_EXE_WIN | replace('\\', '/')}}
POST_BUILD_EXE_OSX = {{POST_BUILD_EXE_OSX | replace('\\', '/')}}
POST_BUILD_EXE_LINUX = {{POST_BUILD_EXE_LINUX | replace('\\', '/')}}

# Pre-defined definitions in this file
ifeq ($(OS),Windows_NT)
  POST_BUILD_EXE ?= $(POST_BUILD_EXE_WIN)
else
  UNAME_S := $(shell uname -s)
  ifeq ($(UNAME_S),Darwin)
    POST_BUILD_EXE ?= $(POST_BUILD_EXE_OSX)
  else
    POST_BUILD_EXE ?= $(POST_BUILD_EXE_LINUX)
  endif
endif

# Command output is hidden by default, it can be enabled by
# setting VERBOSE=true on the commandline.
ifeq ($(VERBOSE),)
  ECHO = @
endif

####################################################################
# Define Project Makefiles                                         #
####################################################################
PROJECT_MAKEFILES = \
{% for makefile in PROJECT_MAKEFILES %} {{makefile.id}}{{ " \\" if not loop.last}}
{% endfor %}
.PHONY: $(PROJECT_MAKEFILES)

####################################################################
# Rules                                                            #
####################################################################
clean: CMD_ARGS = clean
debug: CMD_ARGS = debug
release: CMD_ARGS = release

clean: $(PROJECT_MAKEFILES)

debug release: | pre-build $(PROJECT_MAKEFILES) post-build

# Generated Content
pre-build:
{%- if PRE_BUILD_ARGS %}
{%- if 'POST_BUILD_EXE' in POST_BUILD_ARGS %}
ifeq ($(POST_BUILD_EXE),)
		$(error POST_BUILD_EXE is not defined. Pre-Build cannot run. Please set the STUDIO_ADAPTER_PACK_PATH to the post-build tool when generating or override the variable for this makefile)
endif
{%- endif %}
	@echo 'Running Solution Pre-Build'
	$(ECHO) {{PRE_BUILD_ARGS}}
{%- else %}
	# No pre-build defined
{%- endif %}

post-build:
{%- if POST_BUILD_ARGS %}
{%- if 'POST_BUILD_EXE' in POST_BUILD_ARGS %}
ifeq ($(POST_BUILD_EXE),)
		$(error POST_BUILD_EXE is not defined. Post-Build cannot run. Please set the STUDIO_ADAPTER_PACK_PATH to the post-build tool when generating or override the variable for this makefile)
endif
{%- endif %}
	@echo 'Running Solution Post-Build'
	$(ECHO) {{POST_BUILD_ARGS}}
{%- else %}
	# No post-build defined
{%- endif %}

# Per Project Build Commands
{% for makefile in PROJECT_MAKEFILES -%}
{{makefile.id}}:
	@echo 'Running {{makefile.path}}'
	$(ECHO)@$(MAKE) {% if makefile.dir -%} -C {{makefile.dir}} {% endif -%} -f {{makefile.name}} $(CMD_ARGS)

{% endfor -%}

# Automatically-generated Simplicity Studio Metadata
# Please do not edit or delete these lines!
# SIMPLICITY_STUDIO_METADATA={{SIMPLICITY_STUDIO_METADATA}}=END_SIMPLICITY_STUDIO_METADATA
# END OF METADATA