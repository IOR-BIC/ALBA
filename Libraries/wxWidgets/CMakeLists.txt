#
# Program:   ALBA (Agile Library for Biomedical Applications)
# Module:    $RCSfile: CMakeLists.txt,v $
# Language:  CMake 1.2
# Date:      $Date: 2023-03-21 09:18:02 $
# Version:   $Revision: 1.2 $
#
# Description:
# Project file for configuring and building the WXWIN library.

MESSAGE(STATUS "Configuring WXWIN")
PROJECT (WXWIN)

# place all binaries inside the same output directory
SET (LIBRARY_OUTPUT_PATH ${WXWIN_BINARY_DIR}/bin/ CACHE INTERNAL 
   "Single output directory for building all libraries.")

SET (EXECUTABLE_OUTPUT_PATH ${WXWIN_BINARY_DIR}/bin/ CACHE INTERNAL 
     "Single output directory for building all executables.")

MARK_AS_ADVANCED (
  LIBRARY_OUTPUT_PATH 
  EXECUTABLE_OUTPUT_PATH
)

set(wxBUILD_SHARED FALSE)

INCLUDE (${WXWIN_SOURCE_DIR}/Configure.cmake)

add_subdirectory(${PROJECT_BINARY_DIR}/Libraries/wxWidgets/Sources build)

add_library(Libraries::wx::net ALIAS wxnet)
add_library(Libraries::wx::base ALIAS wxbase)
add_library(Libraries::wx::core ALIAS wxcore)

target_link_libraries(${PROJECT_NAME} Libraries::wx::net Libraries::wx::core Libraries::wx::base)

IF (CONFIGURE_ERROR)
  MESSAGE (FATAL_ERROR "WXWIN: Error configuring WXWIN library, cannot build!")
ENDIF (CONFIGURE_ERROR)