cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC" CACHE STRING "Default CXX options" FORCE)

if (NOT DEFINED TEST)
    set(TEST OFF)
    message(STATUS "--> Testing disabled")
endif ()
if (TEST)
    message(STATUS "--> Testing enabled")
    enable_testing()
endif ()

include(cmake/MyGenerateRootDictionaries.cmake)
include(cmake/MyAddTest.cmake)

project(radiation-transport LANGUAGES CXX HOMEPAGE_URL https://github.com/lobis/radiation-transport)

message("--> ${CMAKE_PROJECT_NAME} | ${CMAKE_PROJECT_HOMEPAGE_URL}")

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

find_package(ROOT REQUIRED COMPONENTS RIO Geom XMLIO)

message("-- Found ROOT version: ${ROOT_VERSION}")

add_subdirectory(extern)

add_subdirectory(libraries)

add_subdirectory(applications)
