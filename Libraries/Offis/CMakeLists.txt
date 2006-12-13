#
# Program:   MULTIMOD APPLICATION FRAMEWORK (MAF)
# Module:    $RCSfile: CMakeLists.txt,v $
# Language:  CMake 1.2
# Date:      $Date: 2006-12-13 09:18:02 $
# Version:   $Revision: 1.2 $
#
# Description:
# Project file for configuring and building the Offis library.

MESSAGE(STATUS "Configuring Offis")
PROJECT (Offis)

# place all binaries inside the same output directory
SET (LIBRARY_OUTPUT_PATH ${Offis_BINARY_DIR}/bin/ CACHE INTERNAL 
   "Single output directory for building all libraries.")

SET (EXECUTABLE_OUTPUT_PATH ${Offis_BINARY_DIR}/bin/ CACHE INTERNAL 
     "Single output directory for building all executables.")

MARK_AS_ADVANCED (
  LIBRARY_OUTPUT_PATH 
  EXECUTABLE_OUTPUT_PATH
)

INCLUDE (${Offis_SOURCE_DIR}/Configure.cmake)

IF (CONFIGURE_ERROR)
  MESSAGE (FATAL_ERROR "Offis: Error configuring Offis library, cannot build!")
ENDIF (CONFIGURE_ERROR)