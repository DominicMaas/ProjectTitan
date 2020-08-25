# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)
project (ProjectTitan)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add source files
file(GLOB_RECURSE SOURCE_FILES
		${CMAKE_SOURCE_DIR}/src/*.c
		${CMAKE_SOURCE_DIR}/src/*.cpp)

# Add header files
file(GLOB_RECURSE HEADER_FILES
		${CMAKE_SOURCE_DIR}/src/*.h
		${CMAKE_SOURCE_DIR}/src/*.hpp)

# Add dep files
file(GLOB_RECURSE DEP_FILES
		${CMAKE_SOURCE_DIR}/deps/*.c
		${CMAKE_SOURCE_DIR}/deps/*.cpp)

# Need OpenGL (macOS)
find_package(OpenGL REQUIRED)

# Need external libraries
add_subdirectory(extern/glfw)
add_subdirectory(extern/glm)
add_subdirectory(extern/freetype2)
add_subdirectory(extern/reactphysics3d)
add_subdirectory(extern/spdlog)

# Add libraries
SET(PLATFORM_LIBRARIES glfw OpenGL::GL glm freetype reactphysics3d spdlog)

# Includes
include_directories("${CMAKE_SOURCE_DIR}/includes")

# Executable for source
add_executable (${PROJECT_NAME} ${DEP_FILES} ${HEADER_FILES} ${SOURCE_FILES})

# Link libraries
target_link_libraries (${PROJECT_NAME} ${PLATFORM_LIBRARIES})

# Copy resources
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${PROJECT_SOURCE_DIR}/res"
		$<TARGET_FILE_DIR:${PROJECT_NAME}>)