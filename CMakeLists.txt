cmake_minimum_required(VERSION 3.9)
project(mandelbrot)

set(CMAKE_CXX_STANDARD 11)

# Find packages of additional libraries
 #   - GLEW
 find_package( GLEW REQUIRED )
 if ( GLEW_FOUND )
     include_directories( ${GLEW_INCLUDE_DIRS} )
     link_libraries     ( ${GLEW_LIBRARIES}    )
 endif()

 #   - GLUT
 find_package( GLUT REQUIRED )
 if ( GLUT_FOUND )
     include_directories( ${GLUT_INCLUDE_DIR} )
     link_libraries     ( ${GLUT_LIBRARIES}    )
 endif()

 #   - OpenGL
 find_package( OpenGL REQUIRED )
 if ( OPENGL_FOUND )
     include_directories( ${OPENGL_INCLUDE_DIRS} )
     link_libraries     ( ${OPENGL_LIBRARIES}    )
 endif()


#include_directories(
#        lib/glfw/include
#        lib/glad/include
#        lib/freeglut/include
#)

#add_subdirectory(lib/glfw)

add_executable(mandelbrot glad.c mandelbrot.cpp stb_image.cpp)
target_link_libraries(mandelbrot glfw3 X11 Xxf86vm Xrandr dl pthread)
