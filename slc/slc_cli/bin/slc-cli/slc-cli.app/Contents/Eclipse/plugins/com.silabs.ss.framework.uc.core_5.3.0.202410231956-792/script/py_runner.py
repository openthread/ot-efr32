#! python3
"""Python Based UC Object Model

This is a primary entry point for any Studio functions that drop
down to python as part of scripts defined in different parts of the
UC spec. Currently, this entry point supports the same, consistent
object model for both validation and upgrade rule scripts.

Each wrapper object is given a reference to a PyProjectApi and a list of 
scripts that should be invoked.
It is up to Studio to decide how to partition scripts
into however many wrappers are needed.
"""
import os
import sys
import types
import importlib.util
import importlib.machinery

from com.silabs.ss.framework.uc.api.python.validator import PyTarget

# This MUST be the same name as the script, otherwise standard import
# directives in the other validation scripts will fail.
ROOT_VALIDATION_MODULE = "validation"


def calc_name(script):
    """Calculate name of module from a full script path."""
    # This may fail if the path style is different from the OS style.
    # java should be converting to OS specific path when injecting
    filename = os.path.basename(script)
    if filename.endswith(".py"):
        filename = filename[0 : len(filename) - 3]
    return filename


# Inputs to this script from Java
global scripts  # list of validation python script paths to run

# list of python libraries loaded before any scripts run this is a list of
# PyScriptLibrary.
global libraries

# object to inject into scripts.
# This is a sub-interface of PyProjectApi specific to the method
# being used.
# PyProjectValidationApi: validation scripts
# PyProjectUpgradeApi   : upgrade scripts (both rule types)
global project
global debug  # boolean -- to print out not print debugging info.

# Name of the function in the script to call that is passed the PyProjectApi model object.
# validate              : for validation scripts
# configuration_upgrade : upgrade rules specific to changing configurables.
# component_upgrade     : upgrade rules specific to add/removing project components
global module_function

global logger


def log_error(logger, prefix, module, exception):
    msg = (
        prefix
        + " "
        + module.__name__
        + " at "
        + module.__file__
        + " with exception "
        + str(exception)
    )

    # Specific to validation issues only: if the api can accept error messages,
    # send one now.
    if hasattr(project, "error"):
        local_error = getattr(project, "error")
        try:
            local_error(
                "Script Failure " + module.__name__,
                target_for_script(module.__file__),
                msg,
            )
        except Exception as e:
            logger.log("Cannot call project.error due to " + str(e), e)

    logger.log(msg, exception)


def target_for_script(script_name):
    return PyTarget([script_name], "SCRIPT")


# This script could be executed when scripts import libraries.
# Only allow execution when Java is doing so directly.
if __name__ == "__main__":
    # First, install the validation root module for a namespace to import
    # libraries from.
    root_validation_module = types.ModuleType(ROOT_VALIDATION_MODULE)
    sys.modules[ROOT_VALIDATION_MODULE] = root_validation_module

    # convert script file references into actual module references
    # We cannot import by file location so we have to do a little
    # moving around.
    script_specs = [
        importlib.util.spec_from_file_location(calc_name(script), script)
        for script in scripts
    ]

    library_specs = [
        importlib.util.spec_from_file_location(
            ROOT_VALIDATION_MODULE + "." + lib.name(), lib.path()
        )
        for lib in libraries
    ]

    for lib in library_specs:
        module = importlib.util.module_from_spec(lib)
        lib.loader.exec_module(module)
        sys.modules[module.__name__] = module
        # Strip out the 'validation.' prefix
        root_validation_module.__dict__[module.__name__.split(".")[-1]] = module

    # ------ Load up scripts next ---------
    script_modules = []
    for spec in script_specs:
        module = importlib.util.module_from_spec(spec)
        # This MUST be done, or module.validate will fail
        # (the attribute list will not be built)
        try:
            spec.loader.exec_module(module)
            script_modules.append(module)
        except Exception as e:
            log_error(logger, "Could not load", module, e)

    # ------- Everything loaded up, so actually run now ---------
    # depending on the mode, the scripts will either run and set validation errors,
    # or run and set upgrade rule changesets.
    for module in script_modules:
        if debug:
            print("Running " + module.__name__ + " at " + module.__file__)
        try:
            if hasattr(module, module_function):
                execute_this = getattr(module, module_function)
                return_value = execute_this(project)
                if debug:
                    print("Return from script was " + str(return_value))

                if not return_value == None:
                    # The assumption for return value hanlding for all cases, if it
                    # exists (only for upgrade rules so far) is list of dictionaries
                    # (list of maps in Java)
                    return_issues = project.handle_return_value(return_value)

                    # Script could had made semantic errors (not putting in required
                    # fields) that we must handle and report.
                    if return_issues == None or not len(return_issues) == 0:
                        if debug:
                            print("Issues existed. Attempting to log them")
                        for issue in return_issues:
                            log_error(logger, str(issue), module, None)
            else:
                if debug:
                    print("No execution entry point")
                log_error(
                    logger,
                    "entry point '"
                    + str(module_function)
                    + "' not part of module "
                    + module.__name__,
                    module,
                    None,
                )
        except Exception as e:
            if debug:
                print(
                    "Script execution error: "
                    + str(module_function)
                    + " in "
                    + module.__file__
                )
            log_error(logger, "Could not run " + str(module_function), module, e)
