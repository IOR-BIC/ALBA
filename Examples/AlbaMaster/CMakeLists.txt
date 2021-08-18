#==========================================================================
# Program:   AlbaMaster
# Module:    CMakeLists.txt
# Language:  CMake
# Date:      $Date: 2018-01-01 12:00:00 $
# Version:   $Revision: 1.0.0.0 $
# Authors:   Gianluigi Crimi, Nicola Vanella
#==========================================================================
# Copyright (c) BIC-IOR 2018 (https://github.com/IOR-BIC)
#
# This software is distributed WITHOUT ANY WARRANTY; without even
# the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
# PURPOSE. See the above copyright notice for more information.
#==========================================================================

PROJECT (AlbaMaster)

SET(PROJECT_SRCS 
	AlbaMaster.cpp					AlbaMaster.h
	albaOpInteractionOp.cpp	albaOpInteractionOp.h
	exWizardSample.cpp			exWizardSample.h
	exAppUtils.cpp					exAppUtils.h
)

IF (NOT EXECUTABLE_OUTPUT_PATH)
  SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin/ CACHE INTERNAL 
     "Single output directory for building all executables.")
ENDIF (NOT EXECUTABLE_OUTPUT_PATH)

IF (WIN32)	
	#Newer versions of CMakes supports CXX_COMPILER info, so it is not necessary to test CMAKE_GENERATOR
	IF (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC AND NOT ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 18.0)		
		SET(NO_MANIFEST 1)
	ELSE (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC AND NOT ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 18.0)
		#Prehaps an older version, i.e., it does not support VS 10, 12, 13
		IF(${CMAKE_GENERATOR} STREQUAL "Visual Studio 9 2008" OR ${CMAKE_GENERATOR} STREQUAL "Visual Studio 8 2005" OR ${CMAKE_GENERATOR} STREQUAL "Visual Studio 10" OR ${CMAKE_GENERATOR} STREQUAL "Visual Studio 11" OR
		${CMAKE_GENERATOR} STREQUAL "Visual Studio 10 Win64" OR ${CMAKE_GENERATOR} STREQUAL "Visual Studio 11 Win64")
		  SET(NO_MANIFEST 1)
		ELSE(${CMAKE_GENERATOR} STREQUAL "Visual Studio 9 2008" OR ${CMAKE_GENERATOR} STREQUAL "Visual Studio 8 2005" OR ${CMAKE_GENERATOR} STREQUAL "Visual Studio 10" OR ${CMAKE_GENERATOR} STREQUAL "Visual Studio 11" OR
        ${CMAKE_GENERATOR} STREQUAL "Visual Studio 10 Win64" OR ${CMAKE_GENERATOR} STREQUAL "Visual Studio 11 Win64")		
		  SET(NO_MANIFEST 0)
		ENDIF(${CMAKE_GENERATOR} STREQUAL "Visual Studio 9 2008" OR ${CMAKE_GENERATOR} STREQUAL "Visual Studio 8 2005" OR ${CMAKE_GENERATOR} STREQUAL "Visual Studio 10" OR ${CMAKE_GENERATOR} STREQUAL "Visual Studio 11" OR
        ${CMAKE_GENERATOR} STREQUAL "Visual Studio 10 Win64" OR ${CMAKE_GENERATOR} STREQUAL "Visual Studio 11 Win64")		
	ENDIF (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC AND NOT ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 18.0)
		
	CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/app.rc.in ${PROJECT_BINARY_DIR}/app.rc @ONLY)
	CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/app.ico ${PROJECT_BINARY_DIR}/app.ico COPYONLY)
  ADD_EXECUTABLE(${PROJECT_NAME} WIN32 ${PROJECT_SRCS} ${PROJECT_BINARY_DIR}/app.rc)
  
    # automatic values from parabuild created temporary environment variable
  SET(BUILD_NUMBER $ENV{BUILD_NUMBER} CACHE INTERNAL "build number")
  
  # create the main version variable:
  SET(APP_VERSION "Build_${BUILD_NUMBER}" CACHE INTERNAL "big version identifier concatenating other identifier")
  
  # config installer file
  # set the installer directory variable
  SET(APP_INSTALLER_DIR ${PROJECT_SOURCE_DIR}/Installer)
  # convert it to Windows format
  STRING (REPLACE "/" "\\" APP_INSTALLER_DIR_WIN ${APP_INSTALLER_DIR} )
  # MESSAGE("variable APP_INSTALLER_DIR_WIN = " ${APP_INSTALLER_DIR_WIN})
  
  # installer executable will be written to this directory
  SET(INSTALLER_EXECUTABLE_OUTPUT_DIR ${EXECUTABLE_OUTPUT_PATH} CACHE PATH "the installer executable will be written to this directory ")
  # convert it to Windows format  
  STRING (REPLACE "/" "\\" APP_INSTALLER_EXECUTABLE_OUTPUT_DIR_WIN ${INSTALLER_EXECUTABLE_OUTPUT_DIR})
  SET(APP_INSTALLER_EXECUTABLE_OUTPUT_DIR_WIN ${APP_INSTALLER_EXECUTABLE_OUTPUT_DIR_WIN} CACHE INTERNAL "get the executable  from this directory")
  # MESSAGE("variable APP_INSTALLER_EXECUTABLE_OUTPUT_DIR_WIN = " ${APP_INSTALLER_EXECUTABLE_OUTPUT_DIR_WIN})
  
  CONFIGURE_FILE(${APP_INSTALLER_DIR}/AlbaMasterInstallerScript.nsi.in  ${EXECUTABLE_OUTPUT_PATH}/Release/AlbaMasterInstallerScript.nsi @ONLY)
  CONFIGURE_FILE(${ALBA_BINARY_DIR}/INSTALLER/EnumUsersReg.nsh  ${EXECUTABLE_OUTPUT_PATH}/Release/EnumUsersReg.nsh @ONLY)
  CONFIGURE_FILE(${ALBA_BINARY_DIR}/INSTALLER/InstallerCommon.nsh  ${EXECUTABLE_OUTPUT_PATH}/Release/InstallerCommon.nsh @ONLY)
ELSE (WIN32)
  ADD_EXECUTABLE(${PROJECT_NAME} ${PROJECT_SRCS} )
ENDIF (WIN32)


IF(ALBA_BUILD_ALBADLL)
  SET(PROJECT_LIBS XercesC albaDLL vtkALBA)
ELSE(ALBA_BUILD_ALBADLL)
  SET(PROJECT_LIBS albaOperations albaGui albaViews albaVME albaVisualPipes albaInteraction albaCommon albaService vtkALBA GPUAPI)
ENDIF(ALBA_BUILD_ALBADLL)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
IF (WIN32)
  SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

IF (ALBA_USE_GDCM)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_GDCM_LIBRARY)
ENDIF (ALBA_USE_GDCM)

  # Generate Version File Info  
  INCLUDE (${ALBA_SOURCE_DIR}/modules/DateTimeMacro.cmake)
  DATE2(date) #(dd-mm-yyyy)
  STRING (REPLACE "-" "/" APP_DATE ${date} )
  message(${PROJECT_NAME} " " ${APP_VERSION} " " ${APP_DATE})
  CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/Version.txt.in ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.txt @ONLY)
