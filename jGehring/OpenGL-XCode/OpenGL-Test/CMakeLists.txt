cmake_minimum_required(VERSION 3.25)
project(OpenGL_Core_Kopie)

set(CMAKE_CXX_STANDARD 14)

include_directories(OpenGL)

cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(OpenGL VERSION 1.0)

add_definitions(-std=c++17)
set(CXX_FLAGS "-Wall" "-Werror" "-Wextra" "-fsanitize=undefined,address")
set(CMAKE_CXX_FLAGS, "${CXX_FLAGS}")
set(CMAKE_BUILD_TYPE Debug)

project(OpenGL-Test)

find_package(glfw3 3.2.1 REQUIRED)
find_package(OpenGL REQUIRED)

target_include_directories(OpenGL-Test PUBLIC ${OPENGL_INCLUDE_DIR})

if (APPLE)
    target_link_libraries(OpenGL-Test "-framework Cocoa")
    target_link_libraries(OpenGL-Test "-framework OpenGL")
    target_link_libraries(OpenGL-Test "-framework IOKit")
endif (APPLE)
target_link_libraries(OpenGL-Test glfw ${OPENGL_gl_LIBRARY})

add_executable(OpenGL-Test
        OpenGL/main.cpp
        OpenGL/WavefrontObject.cpp
        OpenGL/WavefrontObject.hpp)
