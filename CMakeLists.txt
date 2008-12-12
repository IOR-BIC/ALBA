#
# Program:   MULTIMOD APPLICATION FRAMEWORK (MAF)
# Module:    $RCSfile: CMakeLists.txt,v $
# Language:  CMake 2.0
# Date:      $Date: 2008-12-12 11:02:56 $
# Version:   $Revision: 1.44.2.1 $
#
# Description:
# CMake Project file for the MAF.

CMAKE_MINIMUM_REQUIRED(VERSION 2.0) 
SET (CMAKE_BACKWARDS_COMPATIBILITY 2.0)

PROJECT (MAF)

#-----------------------------------------------------------------------------
# Macro for DEBUG logging
#-----------------------------------------------------------------------------
INCLUDE (${MAF_SOURCE_DIR}/modules/DebugMacro.cmake)
MARK_AS_ADVANCED (
  DEBUG_MESSAGES
)

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
  #EXECUTABLE_OUTPUT_PATH
)

#-----------------------------------------------------------------------------
# MAF version number.  An even minor number corresponds to releases.
#-----------------------------------------------------------------------------
SET(MAF_MAJOR_VERSION 2)
SET(MAF_MINOR_VERSION 2)
SET(MAF_BUILD_VERSION 0)
SET(MAF_VERSION
    "${MAF_MAJOR_VERSION}.${MAF_MINOR_VERSION}.${MAF_BUILD_VERSION}")

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
#Remove temporarily the BUILD_SHARED_LIBS because it is not possible to switch the compilation to dynamic.
#SET(BUILD_SHARED_LIBS 0)
OPTION(BUILD_SHARED_LIBS "Build with shared libraries." OFF )
OPTION(BUILD_EXAMPLES "Build examples." ON )
OPTION(BUILD_TESTING "Build the testing tree." OFF)

MARK_AS_ADVANCED (
  BUILD_SHARED_LIBS
)

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

  ###############add all subdirs but "modules"
  SET(INCLUDE_DIRS "${MAF_BINARY_DIR}" "${MAF_SOURCE_DIR}/Base" "${MAF_BINARY_DIR}/Interaction" "${MAF_SOURCE_DIR}/Interaction" "${MAF_SOURCE_DIR}/Interaction/Drivers" 
              "${MAF_SOURCE_DIR}/Core" "${MAF_SOURCE_DIR}/Gui" "${MAF_SOURCE_DIR}/IO" "${MAF_SOURCE_DIR}/Operations" "${MAF_SOURCE_DIR}/VME" 
              "${MAF_SOURCE_DIR}/VME/DataPipes" "${MAF_SOURCE_DIR}/vtkMAF" "${MAF_BINARY_DIR}/vtkMAF" "${MAF_SOURCE_DIR}/GPUAPI")
  INCLUDE_DIRECTORIES(${INCLUDE_DIRS})

	LINK_DIRECTORIES(${MAF_SOURCE_DIR}/GPUAPI/GLEW)
	
  # Configuring MAF kits
  DBG_MESSAGE("MAF: Configuring MAF Kits")
  MESSAGE(STATUS "MAF: Configuring MAF Kits")
  SUBDIRS (GPUAPI)
  SUBDIRS (Base)
  IF (MAF_USE_VTK)
    SUBDIRS (Interaction)
  ENDIF (MAF_USE_VTK)	
  SUBDIRS (Gui)
  SUBDIRS (IO)
  SUBDIRS (Core)
  IF (MAF_USE_VTK)
    SUBDIRS (vtkMAF)
    SUBDIRS (VME)
    SUBDIRS (Operations)
  ENDIF (MAF_USE_VTK)

  IF(BUILD_SHARED_LIBS)
    SET(BUILD_SHARED_LIBS 1)
  ENDIF (BUILD_SHARED_LIBS)
  # Configuring Doxygen documentation  
  SUBDIRS (Docs)
  
  IF(WIN32)
		# Disable automatic manifest generation.
		STRING(REPLACE "/MANIFEST" "/MANIFEST:NO" CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS})
		# Explicitly disable it since it is the default for newer versions of VS
    STRING(REGEX MATCH "MANIFEST:NO" tmp_manifest ${CMAKE_EXE_LINKER_FLAGS})
    IF(NOT tmp_manifest)
        SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO")
    ENDIF(NOT tmp_manifest)
	ENDIF(WIN32)

  # Export Build settings and library dependences
  INCLUDE(CMakeExportBuildSettings)
  
  CMAKE_EXPORT_BUILD_SETTINGS(
  ${MAF_BINARY_DIR}/MAFBuildSettings.cmake
  )
  
  EXPORT_LIBRARY_DEPENDENCIES(
  ${MAF_BINARY_DIR}/MAFLibraryDepends.cmake
  )

  CONFIGURE_FILE(
  ${MAF_SOURCE_DIR}/MAFConfig.cmake.in
  ${MAF_BINARY_DIR}/MAFConfig.cmake
  @ONLY IMMEDIATE
  )
  
  #### Include directory for binary distribution of MAF
  SET(MAF_INSTALL_INCLUDE_DIRS ${MAF_BINARY_DIR}/Include)
  FILE(MAKE_DIRECTORY ${MAF_INSTALL_INCLUDE_DIRS})
  
  FOREACH(dirs ${INCLUDE_DIRS})
    FILE(GLOB include_file_list "${dirs}/*.h")
    FILE(GLOB template_file_list "${dirs}/*.txx")
    FILE(GLOB icons_file_list "${dirs}/*.xpm")
    
    FOREACH(file ${include_file_list})
      FILE(RELATIVE_PATH file_name "${dirs}" "${file}")
    	CONFIGURE_FILE("${file}" "${MAF_INSTALL_INCLUDE_DIRS}/${file_name}")
    ENDFOREACH(file ${include_file_list})
    
    FOREACH(template_file ${template_file_list})
      FILE(RELATIVE_PATH template_file_name "${dirs}" "${template_file}")
    	CONFIGURE_FILE("${template_file}" "${MAF_INSTALL_INCLUDE_DIRS}/${template_file_name}")
    ENDFOREACH(template_file ${template_file_list})
    
    FOREACH(icons_file ${icons_file_list})
      FILE(RELATIVE_PATH icons_file_name "${dirs}" "${icons_file}")
    	CONFIGURE_FILE("${icons_file}" "${MAF_INSTALL_INCLUDE_DIRS}/${icons_file_name}")
    ENDFOREACH(icons_file ${icons_file_list})
  ENDFOREACH(dirs ${MAF_INCLUDE_DIRS})
  
  CONFIGURE_FILE(
  ${MAF_SOURCE_DIR}/MAFUse.cmake.in
  ${MAF_BINARY_DIR}/MAFUse.cmake
  @ONLY IMMEDIATE
  )

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
    FIND_PATH(MAF_DATA_ROOT "TestingDataRoot.txt" "${MAF_SOURCE_DIR}/Testing/unittestData" "${MAF_SOURCE_DIR}/../mmData/TestingData" "${MAF_SOURCE_DIR}/../unittestData" DOC "Test data repository")
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
# MAF_PARABUILD deploy stuff.  
#-----------------------------------------------------------------------------

# This is specific for automatic deploy of maf through Parabuild.
# A MAF_PARABUILD_VERSION variable is created to gather versioning information
      
# this values are set by hand when major or minor is released
SET(MAF_PARABUILD_MAJOR_VERSION 2 CACHE INTERNAL "major version")
SET(MAF_PARABUILD_MINOR_VERSION 2 CACHE INTERNAL "minor version")

# automatic values from parabuild created temporary environment variable
SET(MAF_PARABUILD_BUILD_TIMESTAMP $ENV{PARABUILD_BUILD_TIMESTAMP} CACHE INTERNAL "build timestamp")
SET(MAF_PARABUILD_BUILD_NUMBER $ENV{PARABUILD_BUILD_NUMBER} CACHE INTERNAL "build number")

# create the main version variable:
SET(MAF_PARABUILD_VERSION "${MAF_PARABUILD_MAJOR_VERSION}_${MAF_PARABUILD_MINOR_VERSION}_TS_${MAF_PARABUILD_BUILD_TIMESTAMP}_BUILD_${MAF_PARABUILD_BUILD_NUMBER}" CACHE INTERNAL "big version identifier concatenating other identifier")
 
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
