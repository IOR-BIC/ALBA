#
# Program:   MULTIMOD APPLICATION FRAMEWORK (MAF)
# Module:    $RCSfile: CMakeLists.txt,v $
# Language:  CMake 1.2
# Date:      $Date: 2004-11-10 15:59:34 $
# Version:   $Revision: 1.2 $
#
# Description:
# Project file for configuring and building the VTK library.

MESSAGE(STATUS "Configuring VTK")
PROJECT (VTK)

# place all binaries inside the same output directory
SET (LIBRARY_OUTPUT_PATH ${VTK_BINARY_DIR}/bin/ CACHE INTERNAL 
   "Single output directory for building all libraries.")

SET (EXECUTABLE_OUTPUT_PATH ${VTK_BINARY_DIR}/bin/ CACHE INTERNAL 
     "Single output directory for building all executables.")

MARK_AS_ADVANCED (
  LIBRARY_OUTPUT_PATH 
  EXECUTABLE_OUTPUT_PATH
)

INCLUDE (${VTK_SOURCE_DIR}/Configure.cmake)

IF (CONFIGURE_ERROR)
  MESSAGE (FATAL_ERROR "VTK: Error configuring VTK library, cannot build!")
ENDIF (CONFIGURE_ERROR)