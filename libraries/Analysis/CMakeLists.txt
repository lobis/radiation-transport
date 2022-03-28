
set(LIBRARY Analysis)

find_package(REST)

set(LIBRARIES yaml-cpp spdlog)
set(INCLUDE_DIRECTORIES include)

FILE(GLOB SOURCES src/*.cpp)
FILE(GLOB HEADERS include/*.h)

IF (DEFINED REST_FOUND)
    set(LIBRARIES ${LIBRARIES} ${REST_LIBRARIES})
    set(INCLUDE_DIRECTORIES ${INCLUDE_DIRECTORIES} ${REST_INCLUDE_DIRS})
    add_definitions(-DREST_INSTALLED)
ELSE ()
    message(STATUS "REST not found!")

    list(REMOVE_ITEM SOURCES "${CMAKE_SOURCE_DIR}/libraries/Analysis/src/RestSignalProcess.cpp")
    list(REMOVE_ITEM HEADERS "${CMAKE_SOURCE_DIR}/libraries/Analysis/include/RestSignalProcess.h")
ENDIF ()

add_library(${LIBRARY} SHARED)
install(TARGETS ${LIBRARY} DESTINATION lib)

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
