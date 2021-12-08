set(LIBRARY Simulation)

add_library(${LIBRARY} SHARED)
install(TARGETS ${LIBRARY} DESTINATION lib)

FILE(GLOB SOURCES EventGeant4/src/*.cpp EventAnalysis/src/*.cpp Config/src/*.cpp Garfield/src/*.cpp)
target_sources(
        ${LIBRARY} PUBLIC
        ${SOURCES}
)

set(EXTERN_HEADERS)
FetchContent_GetProperties(yaml-cpp)
set(EXTERN_HEADERS "${yaml-cpp_SOURCE_DIR}/include;${EXTERNAL_HEADER}")
FetchContent_GetProperties(spdlog)
set(EXTERN_HEADERS "${spdlog_SOURCE_DIR}/include;${EXTERNAL_HEADER}")
message(STATUS "Extern headers: ${EXTERN_HEADERS}")

target_include_directories(
        ${LIBRARY} SYSTEM PUBLIC
        ${EXTERN_HEADERS}
        EventGeant4/include
        EventAnalysis/include
        Config/include
        Garfield/include
        ${ROOT_INCLUDE_DIRS}
)

target_link_libraries(
        ${LIBRARY} PUBLIC
        ${ROOT_LIBRARIES}
        yaml-cpp
        spdlog
)

FILE(GLOB HEADERS EventGeant4/include/*.h EventAnalysis/include/*.h Config/include/*.h Garfield/include/*.h)

THIS_GENERATE_ROOT_DICTIONARIES("${HEADERS}")

add_subdirectory(EventGeant4)
add_subdirectory(EventAnalysis)
add_subdirectory(Config)
add_subdirectory(Garfield)
