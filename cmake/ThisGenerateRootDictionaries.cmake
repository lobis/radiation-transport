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

endmacro()