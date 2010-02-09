#
# Program:   MULTIMOD APPLICATION FRAMEWORK (MAF)
# Module:    $RCSfile: Configure.cmake,v $
# Language:  CMake 1.2
# Date:      $Date: 2010-02-09 14:05:38 $
# Version:   $Revision: 1.2.2.9 $
#
# Description:
# Project file for configuring the the Multimod Fundation Layer.

# this defines MedFL_SUBPROJECT
INCLUDE (${MedFL_SOURCE_DIR}/modules/MedFLConfigMacro.cmake)
SET (DEBUG FALSE)

# options for configuring MedFL libraries
OPTION(MED_USE_Offis "Find and Link the Offis library." ON)
IF (${CMAKE_GENERATOR} STREQUAL "Visual Studio 9 2008")
  OPTION(MED_USE_BTK "Find and Link the BTK library." ON)
ELSE (${CMAKE_GENERATOR} STREQUAL "Visual Studio 9 2008")
  SET(MED_USE_BTK OFF)
ENDIF (${CMAKE_GENERATOR} STREQUAL "Visual Studio 9 2008")

#
# configure CMake modules for MedFL
#
MedFL_SUBPROJECT (MedFL_MODULES modules)

#
# Offis Library
#
IF (MED_USE_Offis)
  MedFL_SUBPROJECT(Offis Offis)
ENDIF(MED_USE_Offis)

#
# BTK Library
#
IF (MED_USE_BTK)
  MedFL_SUBPROJECT(BTK BTK)
ENDIF(MED_USE_BTK)


# Create module files in build directory
IF (NOT CONFIGURE_ERROR)
  CONFIGURE_FILE(${MedFL_SOURCE_DIR}/UseMedFL.cmake.in ${MedFL_BINARY_DIR}/UseMedFL.cmake @ONLY IMMEDIATE)
ENDIF (NOT CONFIGURE_ERROR)

MARK_AS_ADVANCED (
  MedFL_BUILD_TARGETS
)

#Write the list of variables and their values that are defined by CMake when running this CMakeList file
IF (DEBUG_MESSAGES)
  #FILE(WRITE ${MedFL_BINARY_DIR}/AllVariables.txt "")
  GET_CMAKE_PROPERTY(VARS VARIABLES)
  FOREACH(var ${VARS})
  	FILE(APPEND ${MedFL_BINARY_DIR}/AllVariables.txt 
  							"${var} \"${${var}}\"\n")
  ENDFOREACH(var ${VARS})
ENDIF (DEBUG_MESSAGES)
