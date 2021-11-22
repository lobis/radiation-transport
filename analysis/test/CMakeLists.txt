set(TESTING_EXECUTABLE AnalysisTest)

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
        Analysis
)

include(GoogleTest)

gtest_discover_tests(${TESTING_EXECUTABLE})

file(COPY files
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
