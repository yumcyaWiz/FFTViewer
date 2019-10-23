cmake_minimum_required(VERSION 3.12)

project(FFTViewer LANGUAGES C CXX)

add_executable(main src/main.cpp)
target_compile_options(main PUBLIC -Wall -Wextra -pedantic-errors)
target_compile_features(main PUBLIC cxx_std_17)
target_include_directories(main PUBLIC ext)

#stb_image
target_compile_definitions(main PUBLIC STB_IMAGE_IMPLEMENTATION)

#glad
add_library(glad STATIC ext/glad/src/glad.c)
target_include_directories(glad PUBLIC ext/glad/include)
target_include_directories(main PUBLIC glad)
target_link_libraries(main PUBLIC glad)
target_link_libraries(main PUBLIC dl)

#glfw
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(ext/glfw)
target_link_libraries(main PUBLIC glfw)

#imgui
add_library(imgui STATIC ext/imgui/imgui.cpp ext/imgui/imgui_draw.cpp ext/imgui/imgui_widgets.cpp ext/imgui/examples/imgui_impl_glfw.cpp ext/imgui/examples/imgui_impl_opengl3.cpp)
target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)
target_include_directories(imgui PUBLIC ext/imgui ext/glad/include)
target_include_directories(main PUBLIC imgui)
target_link_libraries(main PUBLIC imgui) 

#fftw
add_subdirectory(ext/fftw)
target_link_libraries(main PUBLIC ${fftw3_lib})
