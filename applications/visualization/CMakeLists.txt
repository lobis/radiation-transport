set(LIBRARY Visualization)
string(TOLOWER ${LIBRARY} APPLICATION_EXECUTABLE) # application is CamelCase, library is PascalCase
set(APPLICATION ${APPLICATION_EXECUTABLE}-executable)

find_package(ROOT REQUIRED COMPONENTS RIO Geom Eve Gui RGL)

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
        Simulation
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

# MY_GENERATE_ROOT_DICTIONARIES() # This does not work for some reason, but its OK
ROOT_GENERATE_DICTIONARY(${LIBRARY}RootDict include/Visualization.h MODULE ${LIBRARY})

install(TARGETS ${LIBRARY} DESTINATION lib)
install(TARGETS ${APPLICATION} DESTINATION bin)

THIS_INSTALL_HEADERS()

