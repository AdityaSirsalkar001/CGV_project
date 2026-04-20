cmake_minimum_required(VERSION 3.14)
project(SoftwareRasterizerPaint)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(SoftwareRasterizerPaint
    src/main.cpp
    src/Canvas.cpp
    src/Algorithms.cpp
    src/Renderer.cpp
)

# ImGui source files (Assuming libimgui-dev provides headers but we have to compile implementation backends if they aren't precompiled).
# For standard Ubuntu libimgui-dev, it provides headers in /usr/include/imgui and backends in /usr/include/imgui/backends (or we can just skip it if it's too complicated to find the backend CPPs). Let's use pkg-config.

find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
pkg_search_module(GLEW REQUIRED glew)
pkg_search_module(IMGUI REQUIRED imgui)

# Assuming libimgui-dev and libglm-dev are installed, they should be in standard include paths.
target_include_directories(SoftwareRasterizerPaint PRIVATE
    src
    ${GLFW_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    /usr/include/imgui
    /usr/include/imgui/backends
)

target_link_directories(SoftwareRasterizerPaint PRIVATE
    ${GLFW_LIBRARY_DIRS}
    ${GLEW_LIBRARY_DIRS}
    ${IMGUI_LIBRARY_DIRS}
)

# The user must install these first!
target_link_libraries(SoftwareRasterizerPaint PRIVATE
    ${GLFW_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${IMGUI_LIBRARIES}
    GL
    m
    X11
    dl
)
