#
# Program:   MULTIMOD APPLICATION FRAMEWORK (MAF)
# Module:    $RCSfile: CMakeLists.txt,v $
# Language:  CMake 1.2
# Date:      $Date: 2005-04-01 09:46:06 $
# Version:   $Revision: 1.1 $
#
# Description:
# Project file for configuring and building the CRYPTO library.

MESSAGE(STATUS "Configuring CRYPTO")
PROJECT (CRYPTO)

# place all binaries inside the same output directory
SET (LIBRARY_OUTPUT_PATH ${CRYPTO_BINARY_DIR}/bin/ CACHE INTERNAL 
   "Single output directory for building all libraries.")

SET (EXECUTABLE_OUTPUT_PATH ${CRYPTO_BINARY_DIR}/bin/ CACHE INTERNAL 
     "Single output directory for building all executables.")

MARK_AS_ADVANCED (
  LIBRARY_OUTPUT_PATH 
  EXECUTABLE_OUTPUT_PATH
)

INCLUDE (${CRYPTO_SOURCE_DIR}/Configure.cmake)

IF (CONFIGURE_ERROR)
  MESSAGE (FATAL_ERROR "CRYPTO: Error configuring CRYPTO library, cannot build!")
ENDIF (CONFIGURE_ERROR)