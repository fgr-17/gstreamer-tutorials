cmake_minimum_required(VERSION 3.16)

project(FLUID_DEV)

### C++
enable_language(CXX)
set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ standard required for build")
set(CMAKE_CXX_STANDARD_REQUIRED ON CACHE BOOL "is C++ stndard a requirement")
set(CMAKE_CXX_EXTENSIONS OFF CACHE BOOL "enable/disable compiler specific extensions")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -W -Wall -Wextra -Werror -std=c++2a `pkg-config --cflags --libs gstreamer-1.0`")

add_executable (playback-tutorial-1 playback-tutorial-1.cpp)
