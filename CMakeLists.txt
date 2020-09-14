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

# We need these to recompile shaders when they change
file(GLOB_RECURSE DEP_SHADER_FILES
		${CMAKE_SOURCE_DIR}/res/shaders/*.*)

# Need Vulkan
find_package(Vulkan REQUIRED FATAL_ERROR)

# Need external libraries
add_subdirectory(extern/glfw)
add_subdirectory(extern/glm)
add_subdirectory(extern/freetype2)
add_subdirectory(extern/reactphysics3d)
add_subdirectory(extern/spdlog)
add_subdirectory(extern/assimp)

# Add libraries
SET(PLATFORM_LIBRARIES glfw glm freetype reactphysics3d spdlog assimp)

if(MSVC)
	target_compile_options(assimp PRIVATE /W3)
endif()

# Includes
include_directories("${CMAKE_SOURCE_DIR}/includes")

# Executable for source
add_executable (${PROJECT_NAME} ${DEP_FILES} ${HEADER_FILES} ${SOURCE_FILES} ${DEP_SHADER_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})

# Link libraries
target_link_libraries (${PROJECT_NAME} ${PLATFORM_LIBRARIES} ${Vulkan_LIBRARIES})

# Function to compile shaders
function(build_shader TARGET_OBJ SHADER_NAME)
	set(FILE_NAME "${CMAKE_SOURCE_DIR}/res/shaders/${SHADER_NAME}")
	set(OUT_NAME "${CMAKE_BINARY_DIR}/shaders/${SHADER_NAME}.spv")
	add_custom_command(OUTPUT ${OUT_NAME}
			COMMAND glslc ${FILE_NAME} -o ${OUT_NAME}
			DEPENDS ${FILE_NAME}
			COMMENT "Compiling ${SHADER_NAME} (${CMAKE_BINARY_DIR})...")
	add_custom_target(MAKE_${SHADER_NAME} ALL DEPENDS ${OUT_NAME})
	add_dependencies(${TARGET_OBJ} MAKE_${SHADER_NAME})
endfunction()

# Compile shaders
foreach(file ${DEP_SHADER_FILES})
	get_filename_component(FILE_NAME ${file} NAME)
	build_shader(${PROJECT_NAME} ${FILE_NAME})
endforeach()

# Copy resources
add_custom_command(
		TARGET ${PROJECT_NAME} PRE_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${PROJECT_SOURCE_DIR}/res"
		"$<TARGET_FILE_DIR:${PROJECT_NAME}>"
		COMMENT "Copying Resources..."
)

if (CMAKE_BUILD_TYPE MATCHES Debug)
	message(" ")
	message("CMAKE IN DEBUG MODE")
	target_compile_definitions(${PROJECT_NAME} PUBLIC DEBUG=1)
	message(" ")
elseif(CMAKE_BUILD_TYPE MATCHES Release)
	message(" ")
	message("CMAKE IN RELEASE MODE")
	message(" ")
endif ()