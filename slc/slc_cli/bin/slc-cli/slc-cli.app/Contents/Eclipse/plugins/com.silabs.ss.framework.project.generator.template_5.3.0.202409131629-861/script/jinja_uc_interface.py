#! python3
"""Jinja-UC Interface.

This actually runs the Jinja process on a file using variables injected from
the generator class. This script is tied to the implementation in 
PythonJinjaTemplateGenerator. Changing values in that class will affect this
script (and vice-versa).

The Java side is expected to do most of the heavy lifting. This only exists
so that UC can even invoke Jinja to begin with.
"""
import os
import jinja2

# Inputs to this script
global template_file
global jinjaContext

# Output from this script
global content

# Throw any exceptions up the call stack when there are errors
# in the jinja template itself
def raise_jinja_exception(msg):
	raise jinja2.TemplateError(msg)

# For some reason we have to create a new map or Jinja will barf
# It seems like the JEP Map/Dict Object works differently from
# the standard python dict object since the double iterators 
# don't really work :/
# for key,value in jinjaContext: fails with key size errors
jinjaContext = {(key,jinjaContext[key]) for key in jinjaContext}

template_folder, template_filename = os.path.split(template_file)
loader = jinja2.FileSystemLoader(template_folder)
env = jinja2.Environment(loader=loader, keep_trailing_newline=True) # NOSONAR

# Hook up the exception handler
env.globals['raise'] = raise_jinja_exception

try :
	content = env.get_template(template_filename).render(jinjaContext)
except jinja2.TemplateSyntaxError as templExc:
	# Jinja sometimes only adds the character to the error
	raise jinja2.TemplateSyntaxError('Syntax error line ' + str(templExc.lineno) + ' ' + templExc.message, templExc.lineno)