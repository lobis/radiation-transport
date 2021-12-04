set(MODULEMAP_FILENAME ${CMAKE_BINARY_DIR}/module.modulemap)
file(WRITE ${MODULEMAP_FILENAME})
install(FILES ${MODULEMAP_FILENAME} DESTINATION lib)

macro(THIS_GENERATE_ROOT_DICTIONARIES)
    # Create LinkDef.h file
    file(WRITE LinkDef.h)
    file(APPEND LinkDef.h "#ifdef __CLING__\n")
    file(APPEND LinkDef.h "#pragma link off all globals;\n")
    file(APPEND LinkDef.h "#pragma link off all classes;\n")
    file(APPEND LinkDef.h "#pragma link off all functions;\n")
    file(APPEND LinkDef.h "#pragma link C++ nestedclasses;\n")

    FILE(GLOB HEADERS "include/*.h")

    foreach (header ${HEADERS})
        get_filename_component(class ${header} NAME_WE)
        file(APPEND LinkDef.h "#pragma link C++ class ${class}+;\n")
    endforeach ()

    file(APPEND LinkDef.h "#endif\n")

    ROOT_GENERATE_DICTIONARY(${LIBRARY}RootDict ${HEADERS} LINKDEF LinkDef.h MODULE ${LIBRARY})

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/lib${LIBRARY}_rdict.pcm DESTINATION lib)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/lib${LIBRARY}.rootmap DESTINATION lib)

    file(READ ${CMAKE_CURRENT_BINARY_DIR}/module.modulemap MODULEMAP_CONTENTS)

    string(REGEX REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_INSTALL_PREFIX}/include" MODULEMAP_CONTENTS ${MODULEMAP_CONTENTS})

    file(APPEND ${MODULEMAP_FILENAME} "${MODULEMAP_CONTENTS}\n")

endmacro()