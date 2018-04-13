#
# Program:   MULTIMOD APPLICATION FRAMEWORK (MAF)
# Module:    $RCSfile: Configure.cmake,v $
# Language:  CMake 1.2
# Date:      $Date: 2007-06-29 08:54:08 $
# Version:   $Revision: 1.18 $
#
# Description:
# Project file for configuring the the Multimod Fundation Layer.

# this defines MFL_SUBPROJECT
INCLUDE (${MFL_SOURCE_DIR}/modules/MFLConfigMacro.cmake)
INCLUDE (${MFL_SOURCE_DIR}/modules/DebugMacro.cmake)
SET (DEBUG FALSE)

# here we configure all the libraries available and distributed witihn the MFL tree
SET (MFL_TARGETS "")

# options for configuring MFL libraries
OPTION(MAF_USE_VTK "Find and Link the VTK library." ON)
OPTION(MAF_USE_ITK "Find and Link the ITK library." ON)
OPTION(MAF_USE_WX "Find and Link the wxWidgets library." ON)
OPTION(MAF_USE_VCOLLIDE "Find and Link the VCOLLIDE library." OFF)
OPTION(MAF_USE_XercesC "Find and Link the XercesC library." ON)

OPTION(MAF_USE_CURL "Find and Link the cURL library." ON)

IF (EXISTS ${MFL_SOURCE_DIR}/wxMozilla)
  OPTION(MAF_USE_WXMOZILLA "Find and Link the wxMozilla library." OFF)
ENDIF (EXISTS ${MFL_SOURCE_DIR}/wxMozilla)

IF (${CMAKE_GENERATOR} STREQUAL "Visual Studio 6")
  OPTION(MAF_USE_CRYPTO "Find and Link the CRYPTO library." ON)
ELSE (${CMAKE_GENERATOR} STREQUAL "Visual Studio 6")
  OPTION(MAF_USE_CRYPTO "Find and Link the CRYPTO library." ON)
ENDIF (${CMAKE_GENERATOR} STREQUAL "Visual Studio 6")

# options for GDCM and BTK from MAF2Medical
OPTION(MAF_USE_GDCM "Find and Link the GDCM library." ON)
OPTION(MAF_USE_BTK "Find and Link the BTK library." ON)


#
# configure CMake modules for MFL
#
MFL_SUBPROJECT (MFL_MODULES modules)

#
# this is to build VTK inside the MAF tree
#
IF (MAF_USE_VTK)
  MFL_SUBPROJECT(VTK VTK)
ENDIF (MAF_USE_VTK)

#
# GDCM Library
#
IF (MAF_USE_GDCM)
  MFL_SUBPROJECT(GDCM GDCM)
ENDIF(MAF_USE_GDCM)

#
# this is to build ITK inside the MAF tree
#
IF (MAF_USE_ITK)
  MFL_SUBPROJECT(ITK ITK)
ENDIF (MAF_USE_ITK)

#
# this is to build VCOLLIDE inside the MAF tree
#
#IF (MAF_USE_VCOLLIDE)
#MFL_SUBPROJECT(VCollide20 VCollide20)
#ENDIF (MAF_USE_VCOLLIDE)

#
# wxWindows Library
#
IF (MAF_USE_WX)
  MFL_SUBPROJECT(WXWIN wxWin)
ENDIF (MAF_USE_WX)

#
# XercesC Library
#
IF (MAF_USE_XercesC)
  MFL_SUBPROJECT(XercesC XercesC)
ENDIF(MAF_USE_XercesC)

#
# cURL Library
#
IF (MAF_USE_CURL)
  MFL_SUBPROJECT(CURL CURL)
ENDIF(MAF_USE_CURL)

#
# Crypto Library
#
IF (MAF_USE_CRYPTO)
  MFL_SUBPROJECT(CRYPTO CRYPTO)
ENDIF(MAF_USE_CRYPTO)


#
# BTK Library
#
IF (MAF_USE_BTK)
  MFL_SUBPROJECT(BTK BTK)
ENDIF(MAF_USE_BTK)

#
# wxMozilla Library
#
IF (MAF_USE_WXMOZILLA)
  MFL_SUBPROJECT(wxMozilla wxMozilla)
ENDIF(MAF_USE_WXMOZILLA)


# Create module files in build directory
IF (NOT CONFIGURE_ERROR)
  DBG_MESSAGE ("Custom targets for building MFL: ${MFL_TARGETS}")
  SET (MFL_BUILD_TARGETS ${MFL_TARGETS})   
  CONFIGURE_FILE(${MFL_SOURCE_DIR}/UseMFL.cmake.in ${MFL_BINARY_DIR}/UseMFL.cmake @ONLY IMMEDIATE)
ENDIF (NOT CONFIGURE_ERROR)

MARK_AS_ADVANCED (
  MFL_BUILD_TARGETS
)

#Write the list of variables and their values that are defined by CMake when running this CMakeList file
IF (DEBUG_MESSAGES)
  #FILE(WRITE ${MFL_BINARY_DIR}/AllVariables.txt "")
  GET_CMAKE_PROPERTY(VARS VARIABLES)
  FOREACH(var ${VARS})
  	FILE(APPEND ${MFL_BINARY_DIR}/AllVariables.txt 
  							"${var} \"${${var}}\"\n")
  ENDFOREACH(var ${VARS})
ENDIF (DEBUG_MESSAGES)
