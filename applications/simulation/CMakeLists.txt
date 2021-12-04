set(LIBRARY SimulationGeant4)
# string(TOLOWER ${LIBRARY} APPLICATION_EXECUTABLE) # application is CamelCase, library is PascalCase
set(APPLICATION_EXECUTABLE "simulation")
set(APPLICATION ${APPLICATION_EXECUTABLE}-executable)

find_package(ROOT REQUIRED COMPONENTS RIO Geom)

find_package(Geant4 REQUIRED vis_all ui_all)

message("-- Found Geant4 version: ${Geant4_VERSION}")

set(Geant4_VERSION_MIN 10.7.1)
if (${Geant4_VERSION} VERSION_LESS ${Geant4_VERSION_MIN})
    message(SEND_ERROR "Geant4 version ${Geant4_VERSION} less than minimum of ${Geant4_VERSION_MIN}")
endif ()

add_library(${LIBRARY} SHARED)

FILE(GLOB SOURCES src/*.cpp)
target_sources(
        ${LIBRARY} PUBLIC
        ${SOURCES}
)

target_include_directories(
        ${LIBRARY} SYSTEM PUBLIC
        ${ROOT_INCLUDE_DIRS}
        ${Geant4_INCLUDE_DIR}
        include
)

target_link_libraries(
        ${LIBRARY} PUBLIC
        Simulation
        ${ROOT_LIBRARIES}
        ${Geant4_LIBRARIES}
        yaml-cpp
        spdlog
        stdc++fs # <filesystem>
)

add_executable(${APPLICATION} main.cpp)

target_link_libraries(
        ${APPLICATION} PUBLIC
        ${LIBRARY}
)

set_target_properties(
        ${APPLICATION}
        PROPERTIES OUTPUT_NAME ${APPLICATION_EXECUTABLE}
)

THIS_ADD_TEST()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/examples
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# No dictionaries here
THIS_INSTALL_HEADERS()

install(TARGETS ${LIBRARY} DESTINATION lib)
install(TARGETS ${APPLICATION} DESTINATION bin)

FILE(GLOB EXAMPLES "${CMAKE_CURRENT_SOURCE_DIR}/examples/*")
install(DIRECTORY ${EXAMPLES} DESTINATION examples/simulation)
