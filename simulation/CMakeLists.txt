
find_package(Geant4 REQUIRED)

message("-- Found Geant4 version: ${Geant4_VERSION}")

add_executable(simulation main.cpp)

target_link_libraries(simulation Analysis yaml-cpp spdlog)
