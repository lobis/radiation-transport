
if (NOT TARGET Garfield::Garfield)
    find_package(Garfield REQUIRED)
endif ()

message(STATUS "Found Garfield++ version: ${Garfield_VERSION}")

target_link_libraries(
        ${LIBRARY} PUBLIC
        Garfield::Garfield
)

THIS_INSTALL_HEADERS()

THIS_ADD_TEST()
