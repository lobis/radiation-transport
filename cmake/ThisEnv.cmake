set(FILENAME env.sh)

message(STATUS "Project will be installed in '${CMAKE_INSTALL_PREFIX}'")
message(STATUS "source ${CMAKE_INSTALL_PREFIX}/${FILENAME} to load all required environment variables")

file(WRITE ${FILENAME})
file(APPEND ${FILENAME} "#!/bin/bash\n\n")

execute_process(COMMAND root-config --prefix OUTPUT_VARIABLE ROOT_PREFIX)
string(REGEX REPLACE "\n$" "" ROOT_PREFIX ${ROOT_PREFIX})
file(APPEND ${FILENAME} "source ${ROOT_PREFIX}/bin/thisroot.sh\n")

execute_process(COMMAND geant4-config --prefix OUTPUT_VARIABLE GEANT4_PREFIX)
string(REGEX REPLACE "\n$" "" GEANT4_PREFIX ${GEANT4_PREFIX})
file(APPEND ${FILENAME} "source ${GEANT4_PREFIX}/bin/geant4.sh\n")

if ($ENV{GARFIELD_INSTALL})
    file(APPEND ${FILENAME} "source $ENV{GARFIELD_INSTALL}/share/Garfield/setupGarfield.sh\n")
endif ()

file(APPEND ${FILENAME} "\n")

file(APPEND ${FILENAME} "export PATH=${CMAKE_INSTALL_PREFIX}/bin:$PATH\n")
file(APPEND ${FILENAME} "export LD_LIBRARY_PATH=${CMAKE_INSTALL_PREFIX}/lib:$LD_LIBRARY_PATH\n")

file(APPEND ${FILENAME} "\n")

install(FILES ${FILENAME} DESTINATION ${CMAKE_INSTALL_PREFIX})
