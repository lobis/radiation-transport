
find_package(Garfield REQUIRED)

target_link_libraries(
        ${LIBRARY} PUBLIC
        Garfield::Garfield
)

THIS_INSTALL_HEADERS()

THIS_ADD_TEST()

# TODO: fix cling error
