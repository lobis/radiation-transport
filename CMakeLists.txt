cmake_minimum_required(VERSION 3.16)

enable_testing()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC" CACHE STRING "Default CXX options" FORCE)

project(radiation-transport)

set(CMAKE_CXX_STANDARD 17)

message("--> ${CMAKE_PROJECT_NAME} <--")

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

add_subdirectory(extern)
add_subdirectory(analysis)
add_subdirectory(simulation)

