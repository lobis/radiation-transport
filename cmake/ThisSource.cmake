set(FILENAME source.sh)

message(STATUS "Project will be installed in '${CMAKE_INSTALL_PREFIX}'. Load '${CMAKE_INSTALL_PREFIX}/${FILENAME}' first to export all required environment variables")

file(WRITE ${FILENAME})
file(APPEND ${FILENAME} "#!/bin/bash\n\n")
file(APPEND ${FILENAME} "export PATH=${CMAKE_INSTALL_PREFIX}/bin:$PATH\n")
file(APPEND ${FILENAME} "export LD_LIBRARY_PATH=${CMAKE_INSTALL_PREFIX}/lib:$LD_LIBRARY_PATH\n")

install(FILES ${FILENAME} DESTINATION ${CMAKE_INSTALL_PREFIX})
