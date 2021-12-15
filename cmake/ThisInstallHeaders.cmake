macro(THIS_INSTALL_HEADERS)
    FILE(GLOB HEADERS "include/*.h")
    install(FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
endmacro()
