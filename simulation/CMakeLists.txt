set(APPLICATION simulation)

find_package(ROOT REQUIRED COMPONENTS RIO Geom)

find_package(Geant4 REQUIRED)

message("-- Found Geant4 version: ${Geant4_VERSION}")

set(Geant4_VERSION_MIN 10.7.1)
if (${Geant4_VERSION} VERSION_LESS ${Geant4_VERSION_MIN})
    message(SEND_ERROR "Geant4 version ${Geant4_VERSION} less than minimum of ${Geant4_VERSION_MIN}")
endif ()

add_executable(${APPLICATION} main.cpp)

target_include_directories(
        ${APPLICATION} SYSTEM PUBLIC
        ${ROOT_INCLUDE_DIRS}
        ${Geant4_INCLUDE_DIR}
        #include
        ../analysis/include
)

FILE(GLOB SOURCES src/*.cpp)
target_sources(
        ${APPLICATION} PUBLIC
        #${SOURCES}
)

target_link_libraries(
        ${APPLICATION}
        Analysis
        ${ROOT_LIBRARIES}
        ${Geant4_LIBRARIES}
        yaml-cpp
        spdlog
)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/examples
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
