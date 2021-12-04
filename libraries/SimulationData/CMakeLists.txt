set(LIBRARY SimulationData)

add_library(${LIBRARY} SHARED)
install(TARGETS ${LIBRARY} DESTINATION lib)

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

THIS_GENERATE_ROOT_DICTIONARIES()

THIS_INSTALL_HEADERS()

THIS_ADD_TEST()
