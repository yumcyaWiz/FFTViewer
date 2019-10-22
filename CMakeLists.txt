cmake_minimum_required(VERSION 3.12)

project(FFTViewer LANGUAGES C CXX)

add_executable(main main.cpp)

#glad
add_library(glad STATIC ext/glad/src/glad.c)
target_include_directories(glad PUBLIC ext/glad/include)
target_include_directories(main PUBLIC glad)
target_link_libraries(main PUBLIC glad)
target_link_libraries(main PUBLIC dl)

#glfw
find_package(glfw3 3.3 REQUIRED)
target_include_directories(main PUBLIC glfw)
target_link_libraries(main PUBLIC glfw)

#imgui
add_library(imgui STATIC ext/imgui/imgui.cpp ext/imgui/imgui_draw.cpp ext/imgui/imgui_widgets.cpp ext/imgui/examples/imgui_impl_glfw.cpp ext/imgui/examples/imgui_impl_opengl3.cpp)
target_include_directories(imgui PUBLIC ext/imgui)
target_include_directories(main PUBLIC ext/imgui)
target_link_libraries(main PUBLIC imgui) 
