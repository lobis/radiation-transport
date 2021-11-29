macro(MY_GENERATE_ROOT_DICTIONARIES)
    # Create LinkDef.h file
    file(WRITE LinkDef.h)
    file(APPEND LinkDef.h "#ifdef __CLING__\n")
    file(APPEND LinkDef.h "#pragma link off all globals;\n")
    file(APPEND LinkDef.h "#pragma link off all classes;\n")
    file(APPEND LinkDef.h "#pragma link off all functions;\n")
    file(APPEND LinkDef.h "#pragma link C++ nestedclasses;\n")

    FILE(GLOB HEADERS "include/*.h")

    foreach (header ${HEADERS})
        message(${header})
        get_filename_component(class ${header} NAME_WE)
        file(APPEND LinkDef.h "#pragma link C++ class ${class}+;\n")
    endforeach ()

    file(APPEND LinkDef.h "#endif\n")

    ROOT_GENERATE_DICTIONARY(${LIBRARY}RootDict ${HEADERS} LINKDEF LinkDef.h MODULE ${LIBRARY})
endmacro()