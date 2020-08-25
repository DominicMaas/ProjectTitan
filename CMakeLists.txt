# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.17)
project (ProjectTitan)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Read in all source files
file(GLOB SOURCE_FILES "src/*.cpp" "src/*.c")

# Need OpenGL (macOS)
find_package(OpenGL REQUIRED)

# Need external libraries
add_subdirectory(extern/glfw)
add_subdirectory(extern/freetype2)
add_subdirectory(extern/glm)

# Add libraries
SET(PLATFORM_LIBRARIES glfw OpenGL::GL glm freetype)

# Includes
include_directories("${CMAKE_SOURCE_DIR}/include")

# Executable for source
add_executable (ProjectTitan ${SOURCE_FILES})

# Link libraries
target_link_libraries (ProjectTitan ${PLATFORM_LIBRARIES})

# Copy resources
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${PROJECT_SOURCE_DIR}/res"
		$<TARGET_FILE_DIR:${PROJECT_NAME}>)