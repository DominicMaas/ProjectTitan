# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)
project (ProjectTitan)

# Read in all source files
file(GLOB SOURCE_FILES "src/*.cpp" "src/*.c")

# Ger libraries
link_directories("${CMAKE_SOURCE_DIR}/lib")
SET(PLATFORM_LIBRARIES opengl32 glfw3.lib glfw3dll.lib freetype.lib)

# Include includes
include_directories("${CMAKE_SOURCE_DIR}/dependencies")
include_directories("${CMAKE_SOURCE_DIR}/include")

# EXE for source
add_executable (ProjectTitan ${SOURCE_FILES})

# Link libraries
target_link_libraries (ProjectTitan ${PLATFORM_LIBRARIES})

# Copy resources
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${PROJECT_SOURCE_DIR}/res"
		$<TARGET_FILE_DIR:${PROJECT_NAME}>)