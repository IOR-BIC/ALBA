#
# Copyright and description to be placed here
#

PROJECT(mafDoxygen)
DBG_MESSAGE("Configuring project mafDoxygen")

INCLUDE (${CMAKE_ROOT}/Modules/Documentation.cmake OPTIONAL)

IF (BUILD_DOCUMENTATION)
  SET (DOXYFILE ${PROJECT_BINARY_DIR}/maf_doxyfile)
  GET_FILENAME_COMPONENT(DOT_PATH ${DOT} PATH)
  CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/maf_doxyfile.in
             ${PROJECT_BINARY_DIR}/maf_doxyfile
             @ONLY)
  IF (WIN32)
    CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/makedoc.bat.in
             ${PROJECT_BINARY_DIR}/makedoc.bat
             IMMEDIATE)
  ENDIF (WIN32)
ENDIF (BUILD_DOCUMENTATION)