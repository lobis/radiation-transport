set(LIBRARY Analysis)

find_package(ROOT REQUIRED COMPONENTS RIO Geom)

message("-- Found ROOT version: ${ROOT_VERSION}")

add_library(${LIBRARY} STATIC)

target_sources(
        ${LIBRARY} PUBLIC
        src/SimulationConfig.cpp
)

target_include_directories(
        ${LIBRARY} PUBLIC
        include
        ${ROOT_INCLUDE_DIRS}
)

target_link_libraries(
        ${LIBRARY} PUBLIC
        ${ROOT_LIBRARIES}
        yaml-cpp
        spdlog
)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test/files
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/test/files)

add_subdirectory(test)
