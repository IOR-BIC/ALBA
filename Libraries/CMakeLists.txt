#
# Program:   MULTIMOD APPLICATION FRAMEWORK (MAF)
# Module:    $RCSfile: CMakeLists.txt,v $
# Language:  CMake 1.2
# Date:      $Date: 2004-11-10 15:59:18 $
# Version:   $Revision: 1.11 $
#
# Description:
# Project file of the Multimod Fundation Layer.

MESSAGE(STATUS "MFL: Configuring MFL")
PROJECT (MFL)

# place all binaries inside the same output directory
SET (LIBRARY_OUTPUT_PATH ${MFL_BINARY_DIR}/bin/ CACHE INTERNAL 
   "Single output directory for building all libraries.")

SET (EXECUTABLE_OUTPUT_PATH ${MFL_BINARY_DIR}/bin/ CACHE INTERNAL 
     "Single output directory for building all executables.")

MARK_AS_ADVANCED (
  LIBRARY_OUTPUT_PATH 
  EXECUTABLE_OUTPUT_PATH
)

INCLUDE (${MFL_SOURCE_DIR}/Configure.cmake)

IF (CONFIGURE_ERROR)
  MESSAGE (FATAL_ERROR "MFL: Error configuring MFL library, cannot build!")
ENDIF (CONFIGURE_ERROR)
