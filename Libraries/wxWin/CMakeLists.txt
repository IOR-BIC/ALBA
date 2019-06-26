#
# Program:   ALBA (Agile Library for Biomedical Applications)
# Module:    $RCSfile: CMakeLists.txt,v $
# Language:  CMake 1.2
# Date:      $Date: 2006-06-12 15:33:14 $
# Version:   $Revision: 1.2 $
#
# Description:
# Project file for configuring and building the wxWidgets library.

MESSAGE(STATUS "Configuring wxWidgets")
PROJECT (WXWIN)

INCLUDE (${WXWIN_PROJECT_DIR}/Configure.cmake)

IF (NOT WX_CONFIGURED)
  MESSAGE(FATAL_ERROR "wxWidgets configuration error, cannot build!")
ENDIF (NOT WX_CONFIGURED)
