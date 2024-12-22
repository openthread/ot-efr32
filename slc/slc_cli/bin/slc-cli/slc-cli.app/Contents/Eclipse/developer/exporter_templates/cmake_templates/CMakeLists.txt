# Define minimal required version of CMake.
cmake_minimum_required(VERSION "3.25")

# Project definition
project(
	{{PROJECT_NAME}}
	VERSION 1.0
	LANGUAGES C CXX ASM
)

# Include the definition of the slc_{{PROJECT_NAME}} target,
# which contains the content of the SLC project
include({{PROJECT_NAME}}.cmake)

add_executable({{PROJECT_NAME}}
    # Add additional sources here
)

target_include_directories({{PROJECT_NAME}} PUBLIC
    # Add additional include paths here
)

target_compile_definitions({{PROJECT_NAME}} PUBLIC
    # Add additional macros here
)

target_compile_options({{PROJECT_NAME}} PUBLIC
    # Set additional compiler flags here
)

target_link_options({{PROJECT_NAME}} PUBLIC
    # Set additional linker flags here
)

# Link with the content defined in the SLC project
target_link_libraries({{PROJECT_NAME}} PRIVATE
    slc_{{PROJECT_NAME}}
)

# Include managed project content if available
include({{PROJECT_NAME}}_project.cmake OPTIONAL RESULT_VARIABLE managed_project)
if(managed_project)
    message(STATUS "Using managed project content from ${managed_project}")
endif()

# Force the gcc linker command
set_target_properties({{PROJECT_NAME}} PROPERTIES LINKER_LANGUAGE C)

# Create .bin, .hex and .s37 artifacts after building the project
add_custom_command(TARGET {{PROJECT_NAME}}
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O srec "$<TARGET_FILE:{{PROJECT_NAME}}>" "$<TARGET_FILE_DIR:{{PROJECT_NAME}}>/$<TARGET_FILE_BASE_NAME:{{PROJECT_NAME}}>.s37"
    COMMAND ${CMAKE_OBJCOPY} -O ihex "$<TARGET_FILE:{{PROJECT_NAME}}>" "$<TARGET_FILE_DIR:{{PROJECT_NAME}}>/$<TARGET_FILE_BASE_NAME:{{PROJECT_NAME}}>.hex"
    COMMAND ${CMAKE_OBJCOPY} -O binary "$<TARGET_FILE:{{PROJECT_NAME}}>" "$<TARGET_FILE_DIR:{{PROJECT_NAME}}>/$<TARGET_FILE_BASE_NAME:{{PROJECT_NAME}}>.bin" 
)

# Run post-build pipeline to perform additional post-processing
if(post_build_command)
add_custom_command(TARGET {{PROJECT_NAME}}
    POST_BUILD
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/..
    COMMAND ${post_build_command}
)
endif()