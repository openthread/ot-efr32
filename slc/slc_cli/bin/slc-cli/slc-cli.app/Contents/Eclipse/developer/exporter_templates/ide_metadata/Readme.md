# External IDE and Build System Support

## Background
Simplicity Studio aims to provide a way to easily support external IDEs and build systems.
Users are empowered to use their preferred IDE and build system by providing input files, which can be transformed into resources for use in their development environment.

## Usage
**Users should always create new IDE metadata files. Simplicity Studio will overwrite the builtin metadata files! Any user edits to the builtin IDE Metadata (IMD) or Template files will be lost.**

This system requires 2 types of files:
1. IDE Metadata (IMD): This allows the user to define the external IDE or build system. File format is YAML.
2. Template files: The IMD file references these files. The template files will be transformed by SLC Tooling to generate resources for a project (e.g. Makefiles) into their build environment (e.g. VSCode, CMake, etc). File format is Jinja2.
3. The IMD files may be provided by Simplicity Studio or the user.

### IDE Metadata File (*.imd) 
- External IDEs and build systems are supported via a generic template defined by *.imd (metadata) files. 
- Location: IMD file must be stored at `<SimplicityStudio_Install_Directory>/developer/exporter_templates/ide_metadata/<ide_name>.imd`, where `<ide_name>` is the name of the IDE, e.g. platformio, vscode, etc. 
- Simplicity Studio will search for and load the IDE metadata and template files it discovers **on start up**. If you make a change to the `*.imd` file, you will need to restart Studio.
- **IMPORTANT**: By default, files will be provided as templates and are named `<ide_name>.imd.template`. In order to make a template active, so that it can be discovered by Studio, a user will need to remove the `.template` extension from the filename, e.g. platformio.imd.

#### IMD File Format
- The IMD file format is *YAML*. Please vist https://yaml.org/ for more details. 
- The sample file below describes the required properties and their value types.
- Note: `templates` property in the file must have at least one `template` definition, and more than one `template` definition is allowed. Users have the potential to provide multiple template files to generate multiple output files for the creation of a single project.

```
ideName: String
ideDescription: String
projectFileExtensions:
    - String
idePath: String path
keywords: String[]
templates:
    - template:
      toolchains:
        - id: String
      partCompatibility:
        - id: String
      files:
        - file: String with relative path to Template File (see below). Path must be relative to `exporter_templates` directory, e.g. `<ide_name>/filename.projectFileExtension`
          generated: Boolean
          alwaysGenerated: Boolean
          studioMetadataFile: Boolean
          targetPath: String
```

### IDE Metadata File Properties
All properties are *required* unless denoted as *(optional)*.

| Property Name | Type | Description |
| --------------| ------------ | ------------ |
| `ideName` | `String`| name of the IDE (example: `PlatformIO`).<br> Requirement: this value should be unique from `ideName` values used in other *.imd files.<br> If there are multiple files with the same `ideName`, the first *.imd file read will be used.<br> *NOTE*: Simplicity Studio will transform the `ideName` value to create a unique ID for the IDE; regarding the transformation, the `ideName` value will be changed to lowercase characters format and replace space characters (' ') with dash ('-') characters. This unique ID will be used internally in Simplicity Studio to reference the IDE/build system. |
| `ideDescription`| `String`| description of the IDE |
| `projectFileExtensions`| `String` list| a list of file extensions for templates (ex: `ini`) |
| `ideCompatibility` | `String` | *default ID* value is `generic-template`.<br> This ID value is used by Simplicity Studio to find Software Examples in a Silicon Labs SDK that are compatible with this IDE/build system.<br> Metadata for software examples in Silicon Labs SDK declare ID values of IDEs they are compatible with; `generic-template` is ID value used by IMD files.<br> If the metadata for software example(s) in Silicon Labs SDK do not declare ID `generic-template`, and a user wishes to use the software example, the user can manually update the metadata to include it. Find files that end with `_templates.xml` in SDK, and update `value` attribute of property `ideCompatibility`. | 
| `idePath` *(optional)*| `String`| path to the IDE on the system |
| `keywords` *(optional)*| `String[]`| an array of keywords |
| `templates` | `template` list| a list of `template` objects |
| `template` | `template` object | contains `toolchains`, `partCompatibility`, and `files` |
| `toolchains` | list of `toolchain` ids| Use String `id` to indicate the toolchains associated with the template.<br> Currently, only 1 `toolchain` should be defined per template.<br><br> Current supported options: <ul><li>`gcc`</li><li>`iar`</li></ul> |
| `partCompatibility` | list of `toolchain` part compatibilities | Use String `id` to indicate the parts compatible with the template.<br> Currently, only 1 `partCompatibility` should be defined per template.<br><br> Current supported options: <ul><li>`arm`</li><li>`8051`</li><li>`host`</li><li>`risc-v`</li></ul> |
| `files` | `file` list | contains a list of `file` objects |
| `file` | `String` path | a path to the template file on the system (see Template File Properties below for more details). Path must be relative to ide_metadata folder. The output file generated from template file will be stored at root-level of project and filename will be in format <project_name>.<template_file_extension>. If property `targetPath` has non-empty value, then that value will override the default behavior for this field. (see `targetPath` for more details) |
| `generated` | `boolean` within `file`| <ul><li>`true` *(default)*: file runs through the Jinja templating engine</li><li> `false`: file is simply copied to the generation directory</li></ul> |
| `studioMetadataFile` | `boolean` within `file`| This attribute requires a unique `file` extension.<br><ul><li> `true`: file contains the `SIMPLICITY_STUDIO_METADATA` tags and will always be regenerated.</li><li> `false` *(default)*: file does not contain the `SIMPLICITY_STUDIO_METADATA` tags.</li></ul> |
| `alwaysGenerated` | `boolean` within `file`| This attribute requires a unique `file` extension. This attribute is ignored if `studioMetadataFile` is set to `true`.<br><ul><li> `true` *(default)*: file is always overwritten when generating</li><li> `false`: file is only copied if the file does not already exist</li></ul> |
| `targetPath` *(optional)*| `String` within `file` | An optional property specifying the target path of the file on the system.  When not specified, the template file will be located at described above for `file`.  The path must be relative (not absolute) and will be evaluated relative to the project folder.  Use this property to place files in subfolders or to specify a file name other than the default created using `file` (see above).  The property value may include the following macros using the format '${&lt;macroname&gt;}' where &lt;macroname&gt; is any of: PROJECT_NAME.  Note, specifying the property value as '~' effectively acts as if the property has not been specified. |

### Template File Properties
- Template file(s) will contain variables that are replaced during the transformation process to generate output files.
- Location: Template file(s) can be stored at a directory of a user's choice. IMD must be stored at `<SimplicityStudio_Install_Directory>/developer/exporter_templates/<ide_name>` and IMD file must use relative paths to the template file(s).
- Variables in the template files need to be formatted in [Jinja2](https://palletsprojects.com/p/jinja/) syntax, which is the syntax the template engine expects to process the variables.
- The format of the template file (e.g. JSON, makefile, HTML, etc) is chosen by the developer.

The following is a table of variables with types and descriptions. Variables are optional unless denoted as **Required**.

| Variable | Type | Description |
| --------------| ------------ | ------------ |
| `PROJECT_NAME` | `string` | The name of the UC Project. This will generally be the name of the slcp and the generated resources but is allowed to be different.
| `SDK_PATH` | `directory path` | The path to the SDK. If available all source files will be passed in relative to the SDK path and prepended with the $(SDK_PATH) variable. This is usually always defined but may have cases where it isn't.
| `COPIED_SDK_PATH` | `string` |  SDK version of files copied into project
| `EXT_C_FLAGS` | `list` |  All of the general C compiler flags that should be applied for all configurations.
| `EXT_CXX_FLAGS` | `list` |  	All of the general C++ compiler flags that should be applied for all configurations.
| `EXT_ASM_FLAGS` | `list` |  All of the general assembly flags that should be applied for all assembly files.
| `EXT_LD_FLAGS` | `list` |  All of the general linker flags that should be applied when linking.
| `C_CXX_DEFINE_STR` | `list` |  A formatted list of -D defines. These are formatted using the same mechanism as Simplicity IDE. The defines will be single quoted and leave double quotes unescaped.
| `ASM_DEFINE_STR` | `list` |  A formatted list of -D defines. These are formatted using the same mechanism as Simplicity IDE. The defines will be single quoted and leave double quotes unescaped.
| `C_CXX_INCLUDES` | `list` |  All of the C and C++ include paths.
| `ASM_INCLUDES` | `list` |  All of the assembly include paths.
| `SYS_LIBS` | `list` |  System libraries (see Libraries). These will generally be system library names.
| `USER_LIBS` | `list` |  Non-system libraries (see Libraries). These will generally be full paths to an object or library 
| `ALL_SOURCES` | `list` |  All files that are added to the project. These are all C/C++/ASM sources including any sources defined by the exported project (e.g. other files, header files, etc.)
| `SIMPLICITY_STUDIO_METADATA` | `string` | **Required** Store serialized data for generated output. In addition, the template also needs to have these delimiters to indicate the start (`BEGIN_SIMPLICITY_STUDIO_METADATA=`) and end (`=END_SIMPLICITY_STUDIO_METADATA`) of the variable. Example: `BEGIN_SIMPLICITY_STUDIO_METADATA={{SIMPLICITY_STUDIO_METADATA}}=END_SIMPLICITY_STUDIO_METADATA`
