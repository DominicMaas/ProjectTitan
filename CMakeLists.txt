# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)
project ("ProjectTitan")

if(WIN32)
    link_directories(${CMAKE_SOURCE_DIR}/lib)
    SET(PLATFORM_LIBRARIES opengl32 glfw3)
endif(WIN32)

include_directories("${CMAKE_SOURCE_DIR}/include")

add_executable (ProjectTitan "src/ProjectTitan.cpp")
