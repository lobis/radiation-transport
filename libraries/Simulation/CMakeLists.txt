set(LIBRARY Simulation)

add_library(${LIBRARY} SHARED)
install(TARGETS ${LIBRARY} DESTINATION lib)

FILE(GLOB SOURCES
        EventGeant4/src/*.cpp
        # EventAnalysis/src/*.cpp
        Config/src/*.cpp
        # Garfield/src/*.cpp
        )

target_sources(
        ${LIBRARY} PUBLIC
        ${SOURCES}
)

target_include_directories(
        ${LIBRARY} SYSTEM PUBLIC
        ${ROOT_INCLUDE_DIRS}
        EventGeant4/include
        # EventAnalysis/include
        Config/include
        # Garfield/include
        ${CMAKE_INSTALL_PREFIX}/include # Need to include yaml-cpp headers explicitly here
)

target_link_libraries(
        ${LIBRARY} PUBLIC
        ${ROOT_LIBRARIES}
        yaml-cpp
        spdlog
)

add_subdirectory(EventGeant4)
# add_subdirectory(EventAnalysis)
add_subdirectory(Config)
# add_subdirectory(Garfield)

FILE(GLOB HEADERS
        EventGeant4/include/*.h
        # EventAnalysis/include/*.h
        Config/include/*.h
        # Garfield/include/*.h
        )

THIS_GENERATE_ROOT_DICTIONARIES("${HEADERS}")
