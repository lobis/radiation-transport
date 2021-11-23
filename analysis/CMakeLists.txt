set(LIBRARY Analysis)

find_package(ROOT REQUIRED COMPONENTS RIO Geom)

message("-- Found ROOT version: ${ROOT_VERSION}")

add_library(${LIBRARY} STATIC)

FILE(GLOB SOURCES src/*.cpp)
target_sources(
        ${LIBRARY} PUBLIC
        ${SOURCES}
)

target_include_directories(
        ${LIBRARY} SYSTEM PUBLIC
        include
        ${ROOT_INCLUDE_DIRS}
)

target_link_libraries(
        ${LIBRARY} PUBLIC
        ${ROOT_LIBRARIES}
        stdc++fs # <filesystem>
        yaml-cpp
        spdlog
)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test/files
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/test/files)

add_subdirectory(test)
