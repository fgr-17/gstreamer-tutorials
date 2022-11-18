cmake_minimum_required(VERSION 3.16)

set(app_name playback-tutorial-3)

project(GSTREAMER_TUTORIALS)

### C++
enable_language(CXX)
set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ standard required for build")
set(CMAKE_CXX_STANDARD_REQUIRED ON CACHE BOOL "is C++ stndard a requirement")
set(CMAKE_CXX_EXTENSIONS OFF CACHE BOOL "enable/disable compiler specific extensions")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -W -Wall -Wextra")

add_executable (${app_name}
playback-tutorial-3.cpp)

message("App name: " ${app_name})

find_package(PkgConfig REQUIRED)
pkg_check_modules(GSTREAMER REQUIRED IMPORTED_TARGET gstreamer-1.0)
message("GSTREAMER_FOUND:" ${GSTREAMER_FOUND})
message("GSTREAMER_LINK_LIBRARIES: " ${GSTREAMER_LINK_LIBRARIES})

message("GST_INCLUDE_DIRS: " ${GST_INCLUDE_DIRS})
message("GST_LIBRARIES: " ${GST_LIBRARIES})

target_include_directories(${app_name} PRIVATE  ${GSTREAMER_INCLUDE_DIRS})
target_link_libraries(${app_name} ${GSTREAMER_LINK_LIBRARIES})
