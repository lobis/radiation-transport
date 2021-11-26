set(LIBRARY Visualization)
string(TOLOWER ${LIBRARY} APPLICATION_EXECUTABLE) # application is CamelCase, library is PascalCase
set(APPLICATION ${APPLICATION_EXECUTABLE}-executable)

find_package(ROOT REQUIRED COMPONENTS RIO Geom Eve Gui)

add_library(${LIBRARY} SHARED)

target_include_directories(
        ${LIBRARY} SYSTEM PUBLIC
        ${ROOT_INCLUDE_DIRS}
        include
)

FILE(GLOB SOURCES src/*.cpp)
target_sources(
        ${LIBRARY} PUBLIC
        ${SOURCES}
)

target_link_libraries(
        ${LIBRARY} PUBLIC
        SimulationData
        SimulationMetadata
        ${ROOT_LIBRARIES}
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

FILE(GLOB HEADERS include/*.h)
ROOT_GENERATE_DICTIONARY(${LIBRARY}RootDict ${HEADERS} MODULE ${LIBRARY})

install(TARGETS ${LIBRARY} DESTINATION lib)
install(TARGETS ${APPLICATION} DESTINATION bin)
