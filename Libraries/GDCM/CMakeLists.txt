#
# Program:   ALBA (Agile Library for Biomedical Applications)
# Module:    $RCSfile: CMakeLists.txt,v $
# Language:  CMake 1.2
# Date:      $Date: 2006-12-13 09:18:02 $
# Version:   $Revision: 1.2 $
#
# Description:
# Project file for configuring and building the GDCM library.

MESSAGE(STATUS "Configuring GDCM")
PROJECT (GDCM)

# place all binaries inside the same output directory
SET (LIBRARY_OUTPUT_PATH ${GDCM_BINARY_DIR}/bin/ CACHE INTERNAL 
   "Single output directory for building all libraries.")

SET (EXECUTABLE_OUTPUT_PATH ${GDCM_BINARY_DIR}/bin/ CACHE INTERNAL 
     "Single output directory for building all executables.")

MARK_AS_ADVANCED (
  LIBRARY_OUTPUT_PATH 
  EXECUTABLE_OUTPUT_PATH
)

INCLUDE (${GDCM_SOURCE_DIR}/Configure.cmake)

IF (CONFIGURE_ERROR)
  MESSAGE (FATAL_ERROR "GDCM: Error configuring GDCM library, cannot build!")
ENDIF (CONFIGURE_ERROR)