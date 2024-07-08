####################################################################
# Automatically-generated file. Do not edit!                       #
# yaml Version 1                                                   #
####################################################################
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
PROJECT_NAME: {{ PROJECT_NAME }}

# ==============================================================================
C_CXX_INCLUDES:
{%- for include in C_CXX_INCLUDES %}
  - {{ include }}
{%- endfor %}

# ==============================================================================
ALL_SOURCES:
{%- for source in (ALL_SOURCES | sort) %}
  - {{ source }}
{%- endfor %}

# ==============================================================================
C_CXX_DEFINES:
{%- for define in C_CXX_DEFINES %}
  {%- if C_CXX_DEFINES[define].startswith('"') and C_CXX_DEFINES[define].endswith('"') %}
  {{define}}: '{{ C_CXX_DEFINES[define] }}'
  {%- else %}
  {{define}}: {{ C_CXX_DEFINES[define] }}
  {%- endif %}
{%- endfor %}

# ==============================================================================
SYS_LIBS:
{%- for lib_name in SYS_LIBS %}
  - {{ lib_name }}
{%- endfor %}

# ==============================================================================
USER_LIBS:
{%- for lib_name in USER_LIBS %}
  - {{ lib_name }}
{%- endfor %}

# ==============================================================================
EXT_CFLAGS:
{%- for flag in EXT_CFLAGS %}
  - {{ flag }}
{%- endfor %}

# ==============================================================================
EXT_DEBUG_CFLAGS:
{%- for flag in EXT_DEBUG_CFLAGS %}
  - {{ flag }}
{%- endfor %}

# ==============================================================================
EXT_RELEASE_CFLAGS:
{%- for flag in EXT_RELEASE_CFLAGS %}
  - {{ flag }}
{%- endfor %}

# ==============================================================================
EXT_CXX_FLAGS:
{%- for flag in EXT_CXX_FLAGS %}
  - {{ flag }}
{%- endfor %}

# ==============================================================================
EXT_DEBUG_CXX_FLAGS:
{%- for flag in EXT_DEBUG_CXX_FLAGS %}
  - {{ flag }}
{%- endfor %}

# ==============================================================================
EXT_RELEASE_CXX_FLAGS:
{%- for flag in EXT_RELEASE_CXX_FLAGS %}
  - {{ flag }}
{%- endfor %}

# ==============================================================================
EXT_ASM_FLAGS:
{%- for flag in EXT_ASM_FLAGS %}
  - {{ flag }}
{%- endfor %}

# ==============================================================================
EXT_DEBUG_ASM_FLAGS:
{%- for flag in EXT_DEBUG_ASM_FLAGS %}
  - {{ flag }}
{%- endfor %}

# ==============================================================================
EXT_RELEASE_ASM_FLAGS:
{%- for flag in EXT_RELEASE_ASM_FLAGS %}
  - {{ flag }}
{%- endfor %}

# ==============================================================================
EXT_LD_FLAGS:
{%- for flag in EXT_LD_FLAGS %}
  - {{ flag }}
{%- endfor %}

# ==============================================================================
EXT_DEBUG_LD_FLAGS:
{%- for flag in EXT_DEBUG_LD_FLAGS %}
  - {{ flag }}
{%- endfor %}

# ==============================================================================
EXT_RELEASE_LD_FLAGS:
{%- for flag in EXT_RELEASE_LD_FLAGS %}
  - {{ flag }}
{%- endfor %}

