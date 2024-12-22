# What is this?
This folder contains the template files for each exporter. By renaming these files you can edit them and change the output of the Makefile exporter. The files must be renamed in-place to "custom.Makefile" and "custom.project.mak" otherwise the exporter will overwrite them when generating.

Note: These files are global and edits will affect all projects that are exported from this tool.

# Resetting
Deleting, moving, or renaming these files will cause the exporter to copy the original templates back. 