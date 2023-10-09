cmake_minimum_required(VERSION 3.25)
project(EngiGraph)

set(CMAKE_CXX_STANDARD 17)

#Install arrayfire on your system before building.
find_package(ArrayFire REQUIRED)
find_package(OpenCL REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(${PROJECT_SOURCE_DIR} external/Eigen/Eigen external/TaskFlow ${ArrayFire_INCLUDE_DIRS} external/CL external/STB external/GLFW/include external/IMGUI external/IMGUI/backends external/glad/include external)

#imgui files
file(GLOB imgui_sources CONFIGURE_DEPENDS external/IMGUI/*.h external/IMGUI/*.cpp external/IMGUI/backends/imgui_impl_glfw.h external/IMGUI/backends/imgui_impl_glfw.cpp external/IMGUI/backends/imgui_impl_opengl3.cpp external/IMGUI/backends/imgui_impl_opengl3.h)

#find internal and external source files
file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.h src/*.cpp external/glad/src/glad.c)
add_executable(EngiGraph ${sources} ${imgui_sources})
add_library(EngiGraphLib ${sources} ${imgui_sources})

#build GLFW for the platform
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(external/GLFW)

target_link_libraries(EngiGraph ${ArrayFire_LIBRARIES} OpenCL::OpenCL glfw OpenGL::GL)

#add tests
add_subdirectory(test)