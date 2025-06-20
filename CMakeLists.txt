#
# Program:   ALBA (Agile Library for Biomedical Applications)
# Module:    $RCSfile: CMakeLists.txt,v $
# Language:  CMake 2.0
# Date:      $Date: 2011-09-28 09:28:59 $
# Version:   $Revision: 1.44.2.12 $
#
# Description:
# CMake Project file for the ALBA.

CMAKE_MINIMUM_REQUIRED(VERSION 2.4) 

PROJECT (ALBA)



IF(CMAKE_VERSION VERSION_LESS 2.8)
MESSAGE (FATAL_ERROR "CMAKE version must be greater than 2.8")
ENDIF (CMAKE_VERSION VERSION_LESS 2.8)

IF(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)

#-----------------------------------------------------------------------------
# This enables support for Visual Studio: Edit & Continue, OpenMP, and parallel compilation
#-----------------------------------------------------------------------------
IF (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
	#Add OpenMP Support
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /openmp")

	#Add support for Edit & Continue
	SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI")
	IF (NOT ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 17.0)
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
		SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /SAFESEH:NO")
	ENDIF (NOT ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 17.0)
ENDIF (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)



IF(MSVC AND NOT (${CMAKE_VERSION} VERSION_LESS 2.8.11))
		# set stack reserved size to ~10MB
		# CMake previously automatically set this value for MSVC builds, but the
		# behavior was changed in CMake 2.8.11 (Issue 12437) to use the MSVC default
		# value (1 MB) which is not enough for us. Test albaOpExporterGRFWSTest fails 
		# due to stackoverflow 
		SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /STACK:10000000")
ENDIF()

#-----------------------------------------------------------------------------
# Macro for DEBUG logging
#-----------------------------------------------------------------------------
INCLUDE (${ALBA_SOURCE_DIR}/modules/DebugMacro.cmake)
MARK_AS_ADVANCED (
  DEBUG_MESSAGES
)

OPTION(DEBUG_MESSAGES "Enable debug Messages" OFF)

#-----------------------------------------------------------------------------
# this define the ALBA_SUBPROJECT macro
#-----------------------------------------------------------------------------
INCLUDE (${ALBA_SOURCE_DIR}/modules/ALBAConfigMacro.cmake)

#-----------------------------------------------------------------------------
# where to place output binaries
#-----------------------------------------------------------------------------
SET (LIBRARY_OUTPUT_PATH ${ALBA_BINARY_DIR}/bin/ CACHE INTERNAL
     "Single output directory for building all libraries.")
SET (EXECUTABLE_OUTPUT_PATH ${ALBA_BINARY_DIR}/bin/ CACHE INTERNAL 
     "Single output directory for building all executables.")

MARK_AS_ADVANCED (
  LIBRARY_OUTPUT_PATH 
  #EXECUTABLE_OUTPUT_PATH
)

#-----------------------------------------------------------------------------
# ALBA version number.  An even minor number corresponds to releases.
#-----------------------------------------------------------------------------
SET(ALBA_MAJOR_VERSION 2)
SET(ALBA_MINOR_VERSION 2)
SET(ALBA_BUILD_VERSION 0)
SET(ALBA_VERSION
    "${ALBA_MAJOR_VERSION}.${ALBA_MINOR_VERSION}.${ALBA_BUILD_VERSION}")

#-----------------------------------------------------------------------------
# this is to enable precompiled headers under VC6.0
#-----------------------------------------------------------------------------
IF(WIN32)
  IF (NOT UNIX)
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /YX" CACHE STRING "Compiler flags")
  ENDIF (NOT UNIX)
ENDIF(WIN32)

#-----------------------------------------------------------------------------
# This enables support for Visual Studio Edit & Continue, OpenMP, and parallel compilation
#-----------------------------------------------------------------------------
IF (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
	#Add OpenMP Support
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /openmp")

	#Add support for Edit & Continue
	SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI")
	IF (NOT ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 17.0)
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
	ENDIF (NOT ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 17.0)
ENDIF (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)


#-----------------------------------------------------------------------------
# Options for ALBA building
#-----------------------------------------------------------------------------
#Remove temporarily the BUILD_SHARED_LIBS because it is not possible to switch the compilation to dynamic.
#SET(BUILD_SHARED_LIBS 0)
# OPTION(BUILD_SHARED_LIBS "Build with shared libraries." OFF )
OPTION(BUILD_EXAMPLE_APP "Build Example App." ON )
OPTION(BUILD_TESTING "Build the testing tree." OFF)
OPTION(ALBA_BUILD_ALBADLL "Build albaDLL.dll." OFF)
OPTION(ALBA_QA "Generate QA" OFF)

FIND_FILE( VISUAL_STUDIO_RUNTIME vcredist_"@INSTALLER_ARCH@".exe )
STRING(REPLACE "/" "\\" VISUAL_STUDIO_RUNTIME ${VISUAL_STUDIO_RUNTIME})

# MARK_AS_ADVANCED (
  # BUILD_SHARED_LIBS
# )

IF(ALBA_BUILD_ALBADLL)
  SET(ALBA_BUILD_ALBADLL 1)
ENDIF (ALBA_BUILD_ALBADLL)

SET (CONFIGURE_ERROR 0)

#-----------------------------------------------------------------------------
# Platform configuration tests.
#-----------------------------------------------------------------------------
INCLUDE (${CMAKE_ROOT}/Modules/FindThreads.cmake)

# Create the header to configure the classes.
CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/albaConfigure.h.in
             ${PROJECT_BINARY_DIR}/albaConfigure.h
             @ONLY)


#-----------------------------------------------------------------------------
# Installer configuration
#-----------------------------------------------------------------------------
if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
  set( INSTALLER_ARCH "x64" )
else( CMAKE_SIZEOF_VOID_P EQUAL 8 )
  set( INSTALLER_ARCH "x86" )
endif( CMAKE_SIZEOF_VOID_P EQUAL 8 )

CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/Installer/AddToPathPage.nsh ${PROJECT_BINARY_DIR}/Installer/AddToPathPage.nsh @ONLY)
CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/Installer/EnumUsersReg.nsh ${PROJECT_BINARY_DIR}/Installer/EnumUsersReg.nsh @ONLY)
CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/Installer/InstallerCommon.nsh.in ${PROJECT_BINARY_DIR}/Installer/InstallerCommon.nsh @ONLY)
			 
#-----------------------------------------------------------------------------
# Check if MFL is inside the ALBA tree.
#-----------------------------------------------------------------------------
ALBA_SUBPROJECT(MFL Libraries)

IF (MFL_CONFIG_FOUND)
  FIND_FILE (USE_MFL_FILE UseMFL.cmake "${ALBA_BINARY_DIR}/Libraries") 
  IF (USE_MFL_FILE)
    DBG_MESSAGE("ALBA: MFL Configured")
    SET (ALBA_CAN_BUILD 1)
  ENDIF (USE_MFL_FILE)   
ELSE (MFL_CONFIG_FOUND)
  # Search for MFL outside the ALBA tree: MFL must be already built!
  # If found the USE_MFL_FILE variable is set 
  MESSAGE(STATUS "ALBA: Searching for MFL libraries")
  INCLUDE (${ALBA_SOURCE_DIR}/modules/FindMFL.cmake)
  IF (MFL_FOUND)
    MESSAGE(STATUS "ALBA: Found MFL use file at: ${USE_MFL_FILE}")  
    SET (ALBA_CAN_BUILD 1)
  ENDIF (MFL_FOUND)
ENDIF (MFL_CONFIG_FOUND)

#-----------------------------------------------------------------------------
# If MFL found or configure continue with ALBA Configuration
#-----------------------------------------------------------------------------
IF (ALBA_CAN_BUILD)
  # include all necessary for using the MFL
  INCLUDE (${USE_MFL_FILE})

  # Add single dirs to inclusion path
  INCLUDE_DIRECTORIES(BEFORE ".")

  ###############add all subdirs but "modules"
  SET(INCLUDE_DIRS "${ALBA_BINARY_DIR}" "${ALBA_SOURCE_DIR}/Base" "${ALBA_SOURCE_DIR}/Common" "${ALBA_BINARY_DIR}/Interaction" "${ALBA_SOURCE_DIR}/Interaction" "${ALBA_SOURCE_DIR}/Interaction/Drivers" 
              "${ALBA_SOURCE_DIR}/Core" "${ALBA_SOURCE_DIR}/Service" "${ALBA_SOURCE_DIR}/Gui" "${ALBA_SOURCE_DIR}/IO" "${ALBA_SOURCE_DIR}/Operations" "${ALBA_SOURCE_DIR}/VME" "${ALBA_SOURCE_DIR}/VisualPipes" "${ALBA_SOURCE_DIR}/Views"
              "${ALBA_SOURCE_DIR}/VME/DataPipes" "${ALBA_SOURCE_DIR}/vtkALBA" "${ALBA_BINARY_DIR}/vtkALBA"  "${ALBA_SOURCE_DIR}/GPUAPI" 
              "${ALBA_SOURCE_DIR}/GPUAPI/GLEW" "${ALBA_SOURCE_DIR}/GPUAPI/FBO")
  SET(INCLUDE_DIRS ${INCLUDE_DIRS} "${ALBA_SOURCE_DIR}/albaDLL" "${ALBA_BINARY_DIR}/Libraries/ITK/Sources/InsightToolkit-3.0.1/Wrapping/WrapITK/ExternalProjects/ItkVtkGlue/src")
	
	if(BUILD_TESTING)
	  SET(INCLUDE_DIRS ${INCLUDE_DIRS} "${ALBA_SOURCE_DIR}/Testing" )
	endif(BUILD_TESTING)
  
  INCLUDE_DIRECTORIES(${INCLUDE_DIRS})
  

IF(WIN32)
	LINK_DIRECTORIES(${ALBA_SOURCE_DIR}/GPUAPI/GLEW)
ENDIF(WIN32)

# IF(ALBA_BUILD_ALBADLL)
  ################################
  #It's necessary to delete the previous files from the albaDLL project
  # FILE(GLOB h_to_delete "${ALBA_SOURCE_DIR}/albaDLL/*.h")
  # FILE(GLOB cpp_to_delete "${ALBA_SOURCE_DIR}/albaDLL/*.cpp")
  # FILE(GLOB cxx_to_delete "${ALBA_SOURCE_DIR}/albaDLL/*.cxx")
  # FILE(GLOB txx_to_delete "${ALBA_SOURCE_DIR}/albaDLL/*.txx")

  # FOREACH(f ${h_to_delete})
    # FILE(REMOVE "${f}")
  # ENDFOREACH(f ${h_to_delete})

  # FOREACH(f ${cpp_to_delete})
    # FILE(REMOVE "${f}")
  # ENDFOREACH(f ${cpp_to_delete})
  
  # FOREACH(f ${cxx_to_delete})
    # FILE(REMOVE "${f}")
  # ENDFOREACH(f ${cxx_to_delete})

  # FOREACH(f ${txx_to_delete})
    # FILE(REMOVE "${f}")
  # ENDFOREACH(f ${txx_to_delete})
  ################################
# ENDIF(ALBA_BUILD_ALBADLL)

  # Configuring ALBA kits
  DBG_MESSAGE("ALBA: Configuring ALBA Kits")
  MESSAGE(STATUS "ALBA: Configuring ALBA Kits")
  
  IF(ALBA_BUILD_ALBADLL)
  
    IF(WIN32)
      SUBDIRS (GPUAPI)
    ENDIF(WIN32)
    IF (ALBA_USE_VTK)
      SUBDIRS (vtkALBA)
    ENDIF (ALBA_USE_VTK)
    SUBDIRS (albaDLL)
    
  ELSE (ALBA_BUILD_ALBADLL)
  
    IF(WIN32)
      SUBDIRS (GPUAPI)
    ENDIF(WIN32)
      SUBDIRS (Base)
    IF (ALBA_USE_VTK)
      SUBDIRS (Interaction)
    ENDIF (ALBA_USE_VTK)	
    SUBDIRS (Gui)
    SUBDIRS (IO)
    SUBDIRS (Core)
	SUBDIRS (Service)
    IF (ALBA_USE_VTK)
	  SUBDIRS (Common)
      SUBDIRS (vtkALBA)
      SUBDIRS (VME)
			SUBDIRS (VisualPipes)
	  SUBDIRS (Views)
      SUBDIRS (Operations)
    ENDIF (ALBA_USE_VTK)
    
  ENDIF(ALBA_BUILD_ALBADLL)

  SET(BUILD_SHARED_LIBS 0)
  # IF(BUILD_SHARED_LIBS)
    # SET(BUILD_SHARED_LIBS 1)
  # ENDIF (BUILD_SHARED_LIBS)
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
  ${ALBA_BINARY_DIR}/ALBABuildSettings.cmake
  )
  
  IF(NOT (${CMAKE_VERSION} VERSION_LESS 3.0))
    cmake_policy(SET CMP0033 OLD)
  ENDIF(NOT (${CMAKE_VERSION} VERSION_LESS 3.0))
  
  EXPORT_LIBRARY_DEPENDENCIES(
  ${ALBA_BINARY_DIR}/ALBALibraryDepends.cmake
  )

  CONFIGURE_FILE(
  ${ALBA_SOURCE_DIR}/ALBAConfig.cmake.in
  ${ALBA_BINARY_DIR}/ALBAConfig.cmake
  @ONLY IMMEDIATE
  )
  
  #### Include directory for binary distribution of ALBA
  SET(ALBA_INSTALL_INCLUDE_DIRS ${INCLUDE_DIRS})
  
  
  CONFIGURE_FILE(
  ${ALBA_SOURCE_DIR}/ALBAUse.cmake.in
  ${ALBA_BINARY_DIR}/ALBAUse.cmake
  @ONLY IMMEDIATE
  )

  #-----------------------------------------------------------------------------
  # Build examples too ?
  #-----------------------------------------------------------------------------
  IF (BUILD_EXAMPLE_APP AND ALBA_CAN_BUILD)
    SUBDIRS(Examples)
  ENDIF (BUILD_EXAMPLE_APP AND ALBA_CAN_BUILD)
  
  #-----------------------------------------------------------------------------
  # QA stuff
  #-----------------------------------------------------------------------------
  IF(ALBA_QA)
    SUBDIRS(qa)
  ENDIF(ALBA_QA)
  
  #-----------------------------------------------------------------------------
  # Testing stuff
  #-----------------------------------------------------------------------------
  
  # TestData ALBA_DATA_ROOT should be filled even if BUILD_TESTING is set to off 
  FIND_PATH(ALBA_DATA_ROOT "TestingDataRoot.txt" "${ALBA_SOURCE_DIR}/Testing/unittestData" "${ALBA_SOURCE_DIR}/../mmData/TestingData" "${ALBA_SOURCE_DIR}/../unittestData" DOC "Test data repository")
    
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
    
    SUBDIRS (Testing)
    
  ENDIF(BUILD_TESTING)
ELSE (ALBA_CAN_BUILD)
  MESSAGE (FATAL_ERROR "MFL library not found or configuration error, cannot build ALBA")
ENDIF (ALBA_CAN_BUILD)

MARK_AS_ADVANCED(
   USE_MFL_FILE
)


#-----------------------------------------------------------------------------
# ALBA_PARABUILD deploy stuff.  
#-----------------------------------------------------------------------------

# This is specific for automatic deploy of alba through Parabuild.
# A ALBA_PARABUILD_VERSION variable is created to gather versioning information
      
# this values are set by hand when major or minor is released
SET(ALBA_PARABUILD_MAJOR_VERSION 2 CACHE INTERNAL "major version")
SET(ALBA_PARABUILD_MINOR_VERSION 2 CACHE INTERNAL "minor version")

# automatic values from parabuild created temporary environment variable
SET(ALBA_PARABUILD_BUILD_TIMESTAMP $ENV{PARABUILD_BUILD_TIMESTAMP} CACHE INTERNAL "build timestamp")
SET(ALBA_PARABUILD_BUILD_NUMBER $ENV{PARABUILD_BUILD_NUMBER} CACHE INTERNAL "build number")

# create the main version variable:
SET(ALBA_PARABUILD_VERSION "${ALBA_PARABUILD_MAJOR_VERSION}_${ALBA_PARABUILD_MINOR_VERSION}_TS_${ALBA_PARABUILD_BUILD_TIMESTAMP}_BUILD_${ALBA_PARABUILD_BUILD_NUMBER}" CACHE INTERNAL "big version identifier concatenating other identifier")
 
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
