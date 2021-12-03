cmake_minimum_required(VERSION 3.16)

project(radiation-transport LANGUAGES CXX HOMEPAGE_URL https://github.com/lobis/radiation-transport)
message(STATUS "--> ${CMAKE_PROJECT_NAME} | ${CMAKE_PROJECT_HOMEPAGE_URL}")

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC" CACHE STRING "Default CXX options" FORCE)

if (NOT DEFINED TEST)
    set(TEST OFF)
    message(STATUS "Testing disabled (Disabled by default, enabled via -DTEST=ON flag)")
endif ()
if (TEST)
    message(STATUS "Testing enabled (Disabled by default, enabled via -DTEST=ON flag)")
    enable_testing()
endif ()

include(ThisGenerateRootDictionaries)
include(ThisAddTest)
include(ThisInstallHeaders)
include(MySource)

find_package(ROOT REQUIRED COMPONENTS RIO Geom XMLIO)

message(STATUS "Found ROOT version: ${ROOT_VERSION}")

add_subdirectory(extern)

add_subdirectory(libraries)

add_subdirectory(applications)
