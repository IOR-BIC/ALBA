#
# Copyright and description to be placed here
#

PROJECT(medDoxygen)

INCLUDE (${CMAKE_ROOT}/Modules/Documentation.cmake OPTIONAL)

IF (BUILD_DOCUMENTATION)
  SET (DOXYFILE ${PROJECT_BINARY_DIR}/med_doxyfile)
  GET_FILENAME_COMPONENT(DOT_PATH ${DOT} PATH)
  CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/med_doxyfile.in
             ${PROJECT_BINARY_DIR}/med_doxyfile
             @ONLY)
ENDIF (BUILD_DOCUMENTATION)
