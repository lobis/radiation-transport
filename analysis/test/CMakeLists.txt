set(TESTING_EXECUTABLE AnalysisTest)

add_executable(${TESTING_EXECUTABLE})

target_sources(
        ${TESTING_EXECUTABLE} PUBLIC
        src
)

target_link_libraries(
        ${TESTING_EXECUTABLE} PUBLIC
        gtest_main
        Analysis
)

include(GoogleTest)

gtest_discover_tests(${TESTING_EXECUTABLE})

file(COPY files
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/files)
