#
# Program:   MULTIMOD APPLICATION FRAMEWORK (MAF)
# Module:    $RCSfile: CMakeLists.txt,v $
# Language:  CMake 1.2
# Date:      $Date: 2006-12-13 09:17:48 $
# Version:   $Revision: 1.2 $
#
# Description:
# Project file of the Multimod Fundation Layer.

MESSAGE(STATUS "MedFL: Configuring MedFL")
PROJECT (MedFL)

# place all binaries inside the same output directory
SET (LIBRARY_OUTPUT_PATH ${MedFL_BINARY_DIR}/bin/ CACHE INTERNAL 
   "Single output directory for building all libraries.")

SET (EXECUTABLE_OUTPUT_PATH ${MedFL_BINARY_DIR}/bin/ CACHE INTERNAL 
     "Single output directory for building all executables.")

MARK_AS_ADVANCED (
  LIBRARY_OUTPUT_PATH 
  EXECUTABLE_OUTPUT_PATH
)

INCLUDE (${MedFL_SOURCE_DIR}/Configure.cmake)

IF (CONFIGURE_ERROR)
  MESSAGE (FATAL_ERROR "MedFL: Error configuring MedFL library, cannot build!")
ENDIF (CONFIGURE_ERROR)
