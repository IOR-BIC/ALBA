#
# Program:   MULTIMOD APPLICATION FRAMEWORK (MAF)
# Module:    $RCSfile: CMakeLists.txt,v $
# Language:  CMake 1.2
# Date:      $Date: 2009-03-04 15:17:50 $
# Version:   $Revision: 1.1.2.1 $
#
# Description:
# Project file for configuring and building the Grassroots library.

MESSAGE(STATUS "Configuring Grassroots")
PROJECT (Grassroots)

# place all binaries inside the same output directory
SET (LIBRARY_OUTPUT_PATH ${Grassroots_BINARY_DIR}/bin/ CACHE INTERNAL 
   "Single output directory for building all libraries.")

SET (EXECUTABLE_OUTPUT_PATH ${Grassroots_BINARY_DIR}/bin/ CACHE INTERNAL 
     "Single output directory for building all executables.")

MARK_AS_ADVANCED (
  LIBRARY_OUTPUT_PATH 
  EXECUTABLE_OUTPUT_PATH
)

INCLUDE (${Grassroots_SOURCE_DIR}/Configure.cmake)

IF (CONFIGURE_ERROR)
  MESSAGE (FATAL_ERROR "Grassroots: Error configuring Grassroots library, cannot build!")
ENDIF (CONFIGURE_ERROR)