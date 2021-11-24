set(TESTING_EXECUTABLE testSimulation)

enable_testing()

add_executable(${TESTING_EXECUTABLE})

FILE(GLOB SOURCES src/*.cpp)
target_sources(
        ${TESTING_EXECUTABLE} PUBLIC
        ${SOURCES}
)

target_link_libraries(
        ${TESTING_EXECUTABLE} PUBLIC
        gtest_main
        Simulation
)

include(GoogleTest)

gtest_discover_tests(${TESTING_EXECUTABLE})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../examples
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
