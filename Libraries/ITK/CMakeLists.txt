#
# Program:   ALBA (Agile Library for Biomedical Applications)
# Module:    $RCSfile: CMakeLists.txt,v $
# Language:  CMake 1.2
# Date:      $Date: 2005-06-10 15:29:27 $
# Version:   $Revision: 1.1.1.1 $
#
# Description:
# Project file for configuring and building the ITK library.

MESSAGE(STATUS "Configuring ITK")
PROJECT (ITK)

# place all binaries inside the same output directory
SET (LIBRARY_OUTPUT_PATH ${ITK_BINARY_DIR}/bin/ CACHE INTERNAL 
   "Single output directory for building all libraries.")

SET (EXECUTABLE_OUTPUT_PATH ${ITK_BINARY_DIR}/bin/ CACHE INTERNAL 
     "Single output directory for building all executables.")

MARK_AS_ADVANCED (
  LIBRARY_OUTPUT_PATH 
  EXECUTABLE_OUTPUT_PATH
)

INCLUDE (${ITK_SOURCE_DIR}/Configure.cmake)

IF (CONFIGURE_ERROR)
  MESSAGE (FATAL_ERROR "ITK: Error configuring ITK library, cannot build!")
ENDIF (CONFIGURE_ERROR)