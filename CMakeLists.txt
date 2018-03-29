cmake_minimum_required(VERSION 3.9)
project(P3)

set(CMAKE_CXX_STANDARD 11)

list(APPEND CMAKE_PREFIX_PATH "E:/Tools/glew-2.1.0")
include_directories(
        lib/glfw/include
        lib/glad/include
        lib/freeglut/include
)
add_subdirectory(lib/glfw)

add_executable(P3 mandelbrot.cpp lib/glad/src/glad.c stb_image.cpp)
target_link_libraries(P3 glfw)