#
# Program:   MULTIMOD APPLICATION FRAMEWORK (MAF)
# Module:    $RCSfile: CMakeLists.txt,v $
# Language:  CMake 2.0
# Date:      $Date: 2004-11-10 07:01:18 $
# Version:   $Revision: 1.9 $
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
SET (DEBUG FALSE)

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
OPTION(BUILD_MAF_DOCUMENTATION 0)

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
MAF_SUBPROJECT(MFL mfl)

IF (MFL_CONFIG_FOUND)
  FIND_FILE (USE_MFL_FILE UseMFL.cmake "${MAF_BINARY_DIR}/mfl") 
  IF (USE_MFL_FILE)
    DBG_MESSAGE("MAF: MFL Configured" ${DEBUG})
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

  # Configuring MAF kits
  DBG_MESSAGE("MAF: Configuring MAF Kits" ${DEBUG})
  MESSAGE(STATUS "MAF: Configuring MAF Kits")
  SUBDIRS (Base)
  SUBDIRS (Core)
  SUBDIRS (vtkMAF)
  SUBDIRS (Gui)
  
  #-----------------------------------------------------------------------------
  # Build examples too ?
  #-----------------------------------------------------------------------------
  IF (BUILD_EXAMPLES AND MFL_CONFIGURED)
    SUBDIRS(Examples)
  ENDIF (BUILD_EXAMPLES AND MFL_CONFIGURED)
  
  #-----------------------------------------------------------------------------
  # Testing stuff
  #-----------------------------------------------------------------------------
  IF(BUILD_TESTING)
    INCLUDE (${CMAKE_ROOT}/Modules/Dart.cmake)
    ENABLE_TESTING()
  
    INCLUDE (${CMAKE_ROOT}/Modules/Dart.cmake)
    
    # where will executable tests be written ?
    IF (EXECUTABLE_OUTPUT_PATH)
        SET (CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})
    ELSE (EXECUTABLE_OUTPUT_PATH)
        SET (CXX_TEST_PATH .)
    ENDIF (EXECUTABLE_OUTPUT_PATH)
    
    #IF (WIN32)
    #  FIND_PROGRAM (PKZIP_COMMAND pkzipc.exe "c:/program files/pkware/pkzipc" "c:/programmi/pkware/pkzipc" DOC "Utility used for packing ditribution files")
    #  SET(PACKAGES_OUTPUT_PATH "c:/temp" CACHE PATH "Path for distribution zip files" )
    #  CONFIGURE_FILE(${MAF_SOURCE_DIR}/RunDart.tcl.in ${MAF_SOURCE_DIR}/RunDart.tcl @ONLY)
    #ENDIF (WIN32)
    
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
GET_CMAKE_PROPERTY(VARS VARIABLES)
FOREACH(var ${VARS})
	FILE(APPEND ${CMAKE_CURRENT_BINARY_DIR}/AllVariables.txt 
							"${var} \"${${var}}\"\n")
ENDFOREACH(var ${VARS})


