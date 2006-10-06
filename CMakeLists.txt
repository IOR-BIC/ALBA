#
# Program:   MULTIMOD APPLICATION FRAMEWORK (MAF)
# Module:    $RCSfile: CMakeLists.txt,v $
# Language:  CMake 2.0
# Date:      $Date: 2006-10-06 09:10:08 $
# Version:   $Revision: 1.4 $
#
# Description:
# CMake Project file for the Medical Library.

SET (CMAKE_BACKWARDS_COMPATIBILITY "2.0" CACHE INSTERNAL "This project is for CMake 2.0")
CMAKE_MINIMUM_REQUIRED(VERSION 2.0) 

PROJECT (MED)


# Create the header to configure the classes.
CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/medDefines.h.in
             ${PROJECT_BINARY_DIR}/medDefines.h
             @ONLY)

#-----------------------------------------------------------------------------
# where to place output binaries
#-----------------------------------------------------------------------------
SET (LIBRARY_OUTPUT_PATH ${MED_BINARY_DIR}/bin/ CACHE INTERNAL
     "Single output directory for building all libraries.")
SET (EXECUTABLE_OUTPUT_PATH ${MED_BINARY_DIR}/bin/ CACHE INTERNAL 
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

SET (CONFIGURE_ERROR 0)

#-----------------------------------------------------------------------------
# If MAF found or configure continue with MED Configuration
#-----------------------------------------------------------------------------
INCLUDE("${PROJECT_SOURCE_DIR}/FindMAF.cmake")
INCLUDE("${MAF_BINARY_PATH}/MAFConfig.cmake")

IF(MAF_FOUND)
  INCLUDE("${MAF_USE_FILE}")

  # Add single dirs to inclusion path
  INCLUDE_DIRECTORIES(BEFORE ".")
  INCLUDE_DIRECTORIES(${MED_BINARY_DIR})

  #todo: add a FOREACH on all subdirs but "modules"
#  INCLUDE_DIRECTORIES(${MED_BINARY_DIR}/Interaction)
#  INCLUDE_DIRECTORIES(${MED_SOURCE_DIR}/Interaction)
#  INCLUDE_DIRECTORIES(${MED_SOURCE_DIR}/Interaction/Drivers)
#  INCLUDE_DIRECTORIES(${MED_SOURCE_DIR}/Gui)
  INCLUDE_DIRECTORIES(${MED_SOURCE_DIR}/Common)
  INCLUDE_DIRECTORIES(${MED_SOURCE_DIR}/Operations)
  INCLUDE_DIRECTORIES(${MED_SOURCE_DIR}/VME)
  INCLUDE_DIRECTORIES(${MED_SOURCE_DIR}/Views)
  INCLUDE_DIRECTORIES(${MED_SOURCE_DIR}/vtkMED)
  INCLUDE_DIRECTORIES(${MED_BINARY_DIR}/vtkMED)

  # Configuring MAF kits
  MESSAGE(STATUS "MAF: Configuring MAF Kits")
#  SUBDIRS (Interaction)
#  SUBDIRS (Gui)
  SUBDIRS (vtkMED)
  SUBDIRS (Common)
  SUBDIRS (Operations)
  SUBDIRS (Views)
  SUBDIRS (VME)
  
  # Configuring Doxygen documentation  
  SUBDIRS (Docs)
  
  # Export Build settings and library dependences
  INCLUDE(CMakeExportBuildSettings)
  
  CMAKE_EXPORT_BUILD_SETTINGS(
    ${MED_BINARY_DIR}/MEDBuildSettings.cmake
  )
  
  EXPORT_LIBRARY_DEPENDENCIES(
    ${MED_BINARY_DIR}/MEDLibraryDepends.cmake
  )

  CONFIGURE_FILE(
    ${MED_SOURCE_DIR}/MEDConfig.cmake.in
    ${MED_BINARY_DIR}/MEDConfig.cmake
  @ONLY IMMEDIATE
  )
  
  CONFIGURE_FILE(
    ${MED_SOURCE_DIR}/MEDUse.cmake.in
    ${MED_BINARY_DIR}/MEDUse.cmake
  @ONLY IMMEDIATE
  )

  #-----------------------------------------------------------------------------
  # Build examples too ?
  #-----------------------------------------------------------------------------
#  IF (BUILD_EXAMPLES)
#    SUBDIRS(Examples)
#  ENDIF (BUILD_EXAMPLES)
  
  #-----------------------------------------------------------------------------
  # Testing stuff
  #-----------------------------------------------------------------------------
  IF(BUILD_TESTING)
    FIND_PATH(MED_DATA_ROOT "TestingDataRoot.txt" "${MED_SOURCE_DIR}/Testing/unittestData" "${MED_SOURCE_DIR}/../TestingData" "${MED_SOURCE_DIR}/../mmData/TestingData" "${MED_SOURCE_DIR}/../../TestingData"  DOC "Test data repository")
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
ELSE (MAF_FOUND)
  MESSAGE (FATAL_ERROR "MAF library not found or configuration error, cannot build MED")
ENDIF (MAF_FOUND)


#-----------------------------------------------------------------------------
# MEDICAL_PARABUILD deploy stuff.  
#-----------------------------------------------------------------------------

# This is specific for automatic deploy of maf medical through Parabuild.
# A MEDICAL_PARABUILD_VERSION variable is created to gather versioning information
      
# this values are set by hand when major or minor is released
SET(MEDICAL_PARABUILD_MAJOR_VERSION 1 CACHE INTERNAL "major version")
SET(MEDICAL_PARABUILD_MINOR_VERSION 0 CACHE INTERNAL "minor version")

# automatic values from parabuild created temporary environment variable
SET(MEDICAL_PARABUILD_BUILD_TIMESTAMP $ENV{PARABUILD_BUILD_TIMESTAMP} CACHE INTERNAL "build timestamp")
SET(MEDICAL_PARABUILD_BUILD_NUMBER $ENV{PARABUILD_BUILD_NUMBER} CACHE INTERNAL "build number")

# create the main version variable:
SET(MEDICAL_PARABUILD_VERSION "${MEDICAL_PARABUILD_MAJOR_VERSION}_${MEDICAL_PARABUILD_MINOR_VERSION}_TS_${MEDICAL_PARABUILD_BUILD_TIMESTAMP}_BUILD_${MEDICAL_PARABUILD_BUILD_NUMBER}" CACHE INTERNAL "big version identifier concatenating other identifier")
 