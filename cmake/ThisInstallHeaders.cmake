macro(THIS_INSTALL_HEADERS)
    FILE(GLOB HEADERS "include/*.h")
    install(FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)

    include_directories(${CMAKE_INSTALL_PREFIX}/include)
endmacro()
