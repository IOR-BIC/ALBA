#
# Program:   MULTIMOD APPLICATION FRAMEWORK (MAF)
# Module:    $RCSfile: CMakeLists.txt,v $
# Language:  CMake 1.2
# Date:      $Date: 2004-12-03 18:41:45 $
# Version:   $Revision: 1.1 $
#
# Description:
# Project file for configuring and building the XercesC library.

MESSAGE(STATUS "Configuring XercesC")
PROJECT (XercesC)

# place all binaries inside the same output directory
SET (LIBRARY_OUTPUT_PATH ${XercesC_BINARY_DIR}/bin/ CACHE INTERNAL 
   "Single output directory for building all libraries.")

SET (EXECUTABLE_OUTPUT_PATH ${XercesC_BINARY_DIR}/bin/ CACHE INTERNAL 
     "Single output directory for building all executables.")

MARK_AS_ADVANCED (
  LIBRARY_OUTPUT_PATH 
  EXECUTABLE_OUTPUT_PATH
)

INCLUDE (${XercesC_SOURCE_DIR}/Configure.cmake)

IF (CONFIGURE_ERROR)
  MESSAGE (FATAL_ERROR "XercesC: Error configuring XercesC library, cannot build!")
ENDIF (CONFIGURE_ERROR)