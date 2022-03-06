set(LIBRARY Analysis)

add_library(${LIBRARY} SHARED)
install(TARGETS ${LIBRARY} DESTINATION lib)

FILE(GLOB SOURCES src/*.cpp)

target_sources(
        ${LIBRARY} PUBLIC
        ${SOURCES}
)

target_include_directories(
        ${LIBRARY} SYSTEM PUBLIC
        ${ROOT_INCLUDE_DIRS}
        include
        ${CMAKE_INSTALL_PREFIX}/include # Need to include yaml-cpp headers explicitly here
)

target_link_libraries(
        ${LIBRARY} PUBLIC
        ${ROOT_LIBRARIES}
        yaml-cpp
        spdlog
        Simulation
)

FILE(GLOB HEADERS include/*.h)

THIS_GENERATE_ROOT_DICTIONARIES("${HEADERS}")

THIS_INSTALL_HEADERS()

THIS_ADD_TEST()
