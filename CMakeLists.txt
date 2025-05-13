cmake_minimum_required(VERSION 3.10)
project(RemoteLink)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add GLFW (already has its own CMakeLists.txt)
set(GLFW_BUILD_WAYLAND OFF)
set(GLFW_BUILD_X11 ON)
add_subdirectory(libs/GLFW)

# ImGui source files (no CMakeLists.txt in ImGui folder)
file(GLOB IMGUI_SRC
        libs/imgui/*.cpp
        libs/imgui/backends/imgui_impl_glfw.cpp
        libs/imgui/backends/imgui_impl_opengl3.cpp
)

# Your project source files
file(GLOB_RECURSE PROJECT_SRC
        src/*.cpp
        src/*.h
)

# Executable
add_executable(RemoteLink
        ${PROJECT_SRC}
        ${IMGUI_SRC}
        src/PortTypeDef.h
)

# Includes
target_include_directories(RemoteLink PRIVATE
        src
        libs/imgui
        libs/imgui/backends
        libs/GLFW/include
)

# Link libraries
target_link_libraries(RemoteLink PRIVATE glfw)

# Link Linux system libraries (X11, OpenGL, etc.)
# Use modern GLVND OpenGL (suppresses CMake warning)
cmake_policy(SET CMP0072 NEW)
find_package(OpenGL REQUIRED)
target_link_libraries(RemoteLink PRIVATE glfw ${OPENGL_LIBRARIES} dl pthread X11 Xrandr Xi Xxf86vm Xcursor)
