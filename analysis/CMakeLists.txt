
find_package(ROOT REQUIRED COMPONENTS RIO Geom)

message("-- Found ROOT version: ${ROOT_VERSION}")

include_directories(include)

add_library(Analysis SHARED src/TestClass.cpp)

target_link_libraries(Analysis yaml-cpp spdlog)
