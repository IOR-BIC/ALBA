#
# Copyright and description to be placed here
#
#
PROJECT(albaDoxygen)
DBG_MESSAGE("Configuring project albaDoxygen")

INCLUDE (${CMAKE_ROOT}/Modules/Documentation.cmake OPTIONAL)

SET (DOXYFILE ${PROJECT_BINARY_DIR}/Doxyfile)
CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/Doxyfile.in
		 ${PROJECT_BINARY_DIR}/Doxyfile
		 @ONLY)
IF (WIN32)
CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/makedoc.bat.in
		 ${PROJECT_BINARY_DIR}/makedoc.bat
		 IMMEDIATE)
ENDIF (WIN32)
