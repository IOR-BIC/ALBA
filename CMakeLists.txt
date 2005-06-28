#
# Program:   MULTIMOD APPLICATION FRAMEWORK (MAF)
# Module:    $RCSfile: CMakeLists.txt,v $
# Language:  CMake 2.0
# Date:      $Date: 2005-06-28 10:22:15 $
# Version:   $Revision: 1.24 $
#
# Description:
# CMake Project file for the MAF.

SET (CMAKE_BACKWARDS_COMPATIBILITY "2.0" CACHE INSTERNAL "This project is for CMake 2.0")
CMAKE_MINIMUM_REQUIRED(VERSION 2.0) 

PROJECT (MAF)

#-----------------------------------------------------------------------------
# Macro for DEBUG logging
#-----------------------------------------------------------------------------
INCLUDE (${MAF_SOURCE_DIR}/modules/DebugMacro.cmake)
OPTION(DEBUG_MESSAGES "Enable debug Messages" OFF)

#-----------------------------------------------------------------------------
# this define the MAF_SUBPROJECT macro
#-----------------------------------------------------------------------------
INCLUDE (${MAF_SOURCE_DIR}/modules/MAFConfigMacro.cmake)

#-----------------------------------------------------------------------------
# where to place output binaries
#-----------------------------------------------------------------------------
SET (LIBRARY_OUTPUT_PATH ${MAF_BINARY_DIR}/bin/ CACHE INTERNAL
     "Single output directory for building all libraries.")
SET (EXECUTABLE_OUTPUT_PATH ${MAF_BINARY_DIR}/bin/ CACHE INTERNAL 
     "Single output directory for building all executables.")

MARK_AS_ADVANCED (
  LIBRARY_OUTPUT_PATH 
  EXECUTABLE_OUTPUT_PATH
)

#-----------------------------------------------------------------------------
# VTK version number.  An even minor number corresponds to releases.
#-----------------------------------------------------------------------------
SET(MAF_MAJOR_VERSION 2)
SET(MAF_MINOR_VERSION 0)
SET(MAF_BUILD_VERSION 0)
SET(MAF_VERSION
    "${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}.${VTK_BUILD_VERSION}")

#-----------------------------------------------------------------------------
# this is to enable precompiled headers under VC6.0
#-----------------------------------------------------------------------------
IF(WIN32)
  IF (NOT UNIX)
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /YX" CACHE STRING "Compiler flags")
  ENDIF (NOT UNIX)
ENDIF(WIN32)

#-----------------------------------------------------------------------------
# Options for MAF building
#-----------------------------------------------------------------------------
OPTION(BUILD_SHARED_LIBS "Build with shared libraries." 0 )
OPTION(BUILD_EXAMPLES "Build examples." OFF )
OPTION(BUILD_TESTING "Build the testing tree." OFF)

SET (CONFIGURE_ERROR 0)

#-----------------------------------------------------------------------------
# Platform configuration tests.
#-----------------------------------------------------------------------------
INCLUDE (${CMAKE_ROOT}/Modules/FindThreads.cmake)

# Create the header to configure the classes.
CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/mafConfigure.h.in
             ${PROJECT_BINARY_DIR}/mafConfigure.h
             @ONLY)

#-----------------------------------------------------------------------------
# Check if MFL is inside the MAF tree.
#-----------------------------------------------------------------------------
MAF_SUBPROJECT(MFL Libraries)

IF (MFL_CONFIG_FOUND)
  FIND_FILE (USE_MFL_FILE UseMFL.cmake "${MAF_BINARY_DIR}/Libraries") 
  IF (USE_MFL_FILE)
    DBG_MESSAGE("MAF: MFL Configured")
    SET (MAF_CAN_BUILD 1)
  ENDIF (USE_MFL_FILE)   
ELSE (MFL_CONFIG_FOUND)
  # Search for MFL outside the MAF tree: MFL must be already built!
  # If found the USE_MFL_FILE variable is set 
  MESSAGE(STATUS "MAF: Searching for MFL libraries")
  INCLUDE (${MAF_SOURCE_DIR}/modules/FindMFL.cmake)
  IF (MFL_FOUND)
    MESSAGE(STATUS "MAF: Found MFL use file at: ${USE_MFL_FILE}")  
    SET (MAF_CAN_BUILD 1)
  ENDIF (MFL_FOUND)
ENDIF (MFL_CONFIG_FOUND)

#-----------------------------------------------------------------------------
# If MFL found or configure continue with MAF Configuration
#-----------------------------------------------------------------------------
IF (MAF_CAN_BUILD)
  # include all necessary for using the MFL
  INCLUDE (${USE_MFL_FILE})

  # Add single dirs to inclusion path
  INCLUDE_DIRECTORIES(BEFORE ".")
  INCLUDE_DIRECTORIES(${MAF_BINARY_DIR})

  #todo: add a FOREACH on all subdirs but "modules"
  INCLUDE_DIRECTORIES(${MAF_SOURCE_DIR}/Base)
  INCLUDE_DIRECTORIES(${MAF_SOURCE_DIR}/Core)
  INCLUDE_DIRECTORIES(${MAF_SOURCE_DIR}/Gui)
  INCLUDE_DIRECTORIES(${MAF_SOURCE_DIR}/Interaction)
  INCLUDE_DIRECTORIES(${MAF_SOURCE_DIR}/IO)
  INCLUDE_DIRECTORIES(${MAF_SOURCE_DIR}/Operations)
  INCLUDE_DIRECTORIES(${MAF_SOURCE_DIR}/VME)
  INCLUDE_DIRECTORIES(${MAF_SOURCE_DIR}/vtkMAF)
  INCLUDE_DIRECTORIES(${MAF_BINARY_DIR}/vtkMAF)

  # Configuring MAF kits
  DBG_MESSAGE("MAF: Configuring MAF Kits")
  MESSAGE(STATUS "MAF: Configuring MAF Kits")
  SUBDIRS (Base)
  SUBDIRS (Interaction)
  SUBDIRS (Gui)
  SUBDIRS (IO)
  SUBDIRS (Core)
  SUBDIRS (vtkMAF)
  SUBDIRS (VME)
  SUBDIRS (Operations)
  
  # Configuring Doxygen documentation  
  SUBDIRS (Docs)
  
  #-----------------------------------------------------------------------------
  # Build examples too ?
  #-----------------------------------------------------------------------------
  IF (BUILD_EXAMPLES AND MAF_CAN_BUILD)
    SUBDIRS(Examples)
  ENDIF (BUILD_EXAMPLES AND MAF_CAN_BUILD)
  
  #-----------------------------------------------------------------------------
  # Testing stuff
  #-----------------------------------------------------------------------------
  IF(BUILD_TESTING)
    FIND_PATH(MAF_DATA_ROOT "TestingDataRoot.txt" "${MAF_SOURCE_DIR}/../TestingData" "${MAF_SOURCE_DIR}/../mmData/TestingData" "${MAF_SOURCE_DIR}/../../TestingData" DOC "Test data repository")
    INCLUDE (${CMAKE_ROOT}/Modules/Dart.cmake)
    ENABLE_TESTING()
  
    INCLUDE (${CMAKE_ROOT}/Modules/Dart.cmake)
    
    # where will executable tests be written ?
    IF (EXECUTABLE_OUTPUT_PATH)
        SET (CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})
    ELSE (EXECUTABLE_OUTPUT_PATH)
        SET (CXX_TEST_PATH .)
    ENDIF (EXECUTABLE_OUTPUT_PATH)
    
    SUBDIRS (Testing)
    
  ENDIF(BUILD_TESTING)
ELSE (MAF_CAN_BUILD)
  MESSAGE (FATAL_ERROR "MFL library not found or configuration error, cannot build MAF")
ENDIF (MAF_CAN_BUILD)

MARK_AS_ADVANCED(
   USE_MFL_FILE
)

#-----------------------------------------------------------------------------
# Write the list of variables and their values that are defined by CMake 
# when running this CMakeList file
#-----------------------------------------------------------------------------
IF (DEBUG_MESSAGES)
  GET_CMAKE_PROPERTY(VARS VARIABLES)
  FOREACH(var ${VARS})
  	FILE(APPEND ${CMAKE_CURRENT_BINARY_DIR}/AllVariables.txt 
  							"${var} \"${${var}}\"\n")
  ENDFOREACH(var ${VARS})
ENDIF (DEBUG_MESSAGES)


