cmake_minimum_required(VERSION 3.25)
project(OpenGL)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(OpenGL
    main.cpp
    WavefrontObject.cpp
    WavefrontObject.hpp)
