cmake_minimum_required(VERSION 3.16)

project(radiation-transport)

set(CMAKE_CXX_STANDARD 17)

message("--> ${CMAKE_PROJECT_NAME} <--")

add_subdirectory(external)

add_subdirectory(analysis)

add_subdirectory(simulation)
