#
# Program:   ALBA (Agile Library for Biomedical Applications)
# Module:    $RCSfile: CMakeLists.txt,v $
# Language:  CMake 1.2
# Date:      $Date: 2006-10-17 14:58:13 $
# Version:   $Revision: 1.2 $
#
# Description:
# Project file for configuring and building the CURL library

MESSAGE(STATUS "Configuring CURL")
PROJECT (CURL)
 
# place all binaries inside the same output directory
SET (LIBRARY_OUTPUT_PATH ${CURL_BINARY_DIR}/bin/ CACHE INTERNAL 
   "Single output directory for building all libraries.")

SET (EXECUTABLE_OUTPUT_PATH ${CURL_BINARY_DIR}/bin/ CACHE INTERNAL 
   "Single output directory for building all executables.")

MARK_AS_ADVANCED (
  LIBRARY_OUTPUT_PATH 
  EXECUTABLE_OUTPUT_PATH
)

INCLUDE (${CURL_SOURCE_DIR}/Configure.cmake)

IF (CONFIGURE_ERROR)
  MESSAGE (FATAL_ERROR "CURL: Error configuring CURL library, cannot build!")
ENDIF (CONFIGURE_ERROR)

