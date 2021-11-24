set(LIBRARY SimulationMetadata)

add_library(${LIBRARY} SHARED)

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

add_subdirectory(test)
