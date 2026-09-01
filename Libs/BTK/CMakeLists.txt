#
# Program:   ALBA (Agile Library for Biomedical Applications)
# Module:    $RCSfile: CMakeLists.txt,v $
# Language:  CMake 1.2
# Date:      $Date: 2010-02-09 14:06:12 $
# Version:   $Revision: 1.1.2.1 $
#
# Description:
# Project file for configuring and building the BTK library.

MESSAGE(STATUS "Configuring BTK")
PROJECT (BTK)

# place all binaries inside the same output directory
SET (LIBRARY_OUTPUT_PATH ${BTK_BINARY_DIR}/bin/ CACHE INTERNAL 
   "Single output directory for building all libraries.")

SET (EXECUTABLE_OUTPUT_PATH ${BTK_BINARY_DIR}/bin/ CACHE INTERNAL 
     "Single output directory for building all executables.")

MARK_AS_ADVANCED (
  LIBRARY_OUTPUT_PATH 
  EXECUTABLE_OUTPUT_PATH
)

INCLUDE (${BTK_SOURCE_DIR}/Configure.cmake)

IF (CONFIGURE_ERROR)
  MESSAGE (FATAL_ERROR "BTK: Error configuring BTK library, cannot build!")
ENDIF (CONFIGURE_ERROR)