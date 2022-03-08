
set(LIBRARY Analysis)

find_package(REST QUIET)

set(LIBRARIES yaml-cpp spdlog)
set(INCLUDE_DIRECTORIES include)

message(STATUS "REST: $ENV{REST_PATH}")

execute_process(COMMAND rest-config --libs OUTPUT_VARIABLE REST_LIBRARIES)
string(STRIP ${REST_LIBRARIES} REST_LIBRARIES) # It is necessary to strip the whitespaces, or it will give error

execute_process(COMMAND rest-config --incdir OUTPUT_VARIABLE REST_INCLUDE_DIRS)
string(STRIP ${REST_INCLUDE_DIRS} REST_INCLUDE_DIRS) # It is necessary to strip the whitespaces, or it will give error

execute_process(COMMAND rest-config --version OUTPUT_VARIABLE REST_VERSION)
string(STRIP ${REST_VERSION} REST_VERSION) # It is necessary to strip the whitespaces, or it will give error

message(STATUS "$ENV{REST_PATH}")
set(REST_PATH $ENV{REST_PATH})
message(STATUS "${REST_PATH}")

FILE(GLOB SOURCES src/*.cpp)
FILE(GLOB HEADERS include/*.h)

IF (DEFINED REST_PATH)
    message(STATUS "REST FOUND!")
    set(LIBRARIES ${LIBRARIES} ${REST_LIBRARIES})
    set(INCLUDE_DIRECTORIES ${INCLUDE_DIRECTORIES} ${REST_INCLUDE_DIRS})
    add_definitions(-DREST_INSTALLED)
    message(STATUS "${CMAKE_SOURCE_DIR}")

ELSE ()
    message(STATUS "REST not found!")

    list(REMOVE_ITEM SOURCES "${CMAKE_SOURCE_DIR}/libraries/Analysis/src/RestSignalProcess.cpp")
    list(REMOVE_ITEM HEADERS "${CMAKE_SOURCE_DIR}/libraries/Analysis/include/RestSignalProcess.h")
ENDIF ()

add_library(${LIBRARY} SHARED)
install(TARGETS ${LIBRARY} DESTINATION lib)

message(STATUS "SOURCES: ${SOURCES}")

target_sources(
        ${LIBRARY} PUBLIC
        ${SOURCES}
)

target_include_directories(
        ${LIBRARY} SYSTEM PUBLIC
        ${ROOT_INCLUDE_DIRS}
        ${INCLUDE_DIRECTORIES}
        ${CMAKE_INSTALL_PREFIX}/include # Need to include yaml-cpp headers explicitly here
)

target_link_libraries(
        ${LIBRARY} PUBLIC
        ${ROOT_LIBRARIES}
        ${LIBRARIES}
        Simulation
)

THIS_GENERATE_ROOT_DICTIONARIES("${HEADERS}")

THIS_INSTALL_HEADERS()

THIS_ADD_TEST()
