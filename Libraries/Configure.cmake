#
# Program:   MULTIMOD APPLICATION FRAMEWORK (MAF)
# Module:    $RCSfile: Configure.cmake,v $
# Language:  CMake 1.2
# Date:      $Date: 2009-03-04 15:17:50 $
# Version:   $Revision: 1.2.2.1 $
#
# Description:
# Project file for configuring the the Multimod Fundation Layer.

# this defines MedFL_SUBPROJECT
INCLUDE (${MedFL_SOURCE_DIR}/modules/MedFLConfigMacro.cmake)
SET (DEBUG FALSE)

# options for configuring MedFL libraries
OPTION(MED_USE_Offis "Find and Link the Offis library." ON)
OPTION(MED_USE_Grassroots "Find and Link the Grassroots library." ON)

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
# Grassroots Library
#
IF (MED_USE_Grassroots)
  MedFL_SUBPROJECT(Grassroots Grassroots)
ENDIF(MED_USE_Grassroots)

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
