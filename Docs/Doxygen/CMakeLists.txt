#
# Copyright and description to be placed here
#

PROJECT(mafDoxygen)
DBG_MESSAGE("Configuring project mafDoxygen")

INCLUDE (${CMAKE_ROOT}/Modules/Documentation.cmake OPTIONAL)

IF (BUILD_DOCUMENTATION)  
  CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/maf_doxyfile.in
             ${PROJECT_BINARY_DIR}/maf_doxyfile
             @ONLY)
ENDIF (BUILD_DOCUMENTATION)