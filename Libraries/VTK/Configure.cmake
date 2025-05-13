#
# Program:   ALBA (Agile Library for Biomedical Applications)
# Module:    $RCSfile: Configure.cmake,v $
# Language:  CMake 1.2
# Date:      $Date: 2011-05-25 09:59:17 $
# Version:   $Revision: 1.22.4.1 $
#
# Description:
# Project file for configuring the VTK library as an external project.
# Return variables:
# VTK_CONFIGURED  true if library configured correctly



INCLUDE (${MFL_SOURCE_DIR}/modules/PackagesMacro.cmake)
INCLUDE (${MFL_SOURCE_DIR}/modules/PatchMacro.cmake)

#
# This option is for using ANSI stream within VTK library
#
OPTION(VTK_USE_ANSI_STDLIB "Use the ANSI iostream stdlib library. This must be On to allow ITK<->VTK interfacing." OFF)
# this is to build VTK inside the ALBA tree
IF (EXISTS "${VTK_SOURCE_DIR}/Sources")   
  SET (VTK_SOURCE_PATH "${VTK_SOURCE_DIR}/Sources")
ENDIF (EXISTS "${VTK_SOURCE_DIR}/Sources")

#look if there is a package for VTK
IF (EXISTS "${VTK_SOURCE_DIR}/Packages")   
  MESSAGE(STATUS "VTK: Found VTK packages")
  # here we should unpack into "${VTK_BINARY_DIR}/Sources"
  SET (VTK_PACKAGE_PATH "${VTK_SOURCE_DIR}/Packages")
  SET (VTK_UNPACK_PATH "${VTK_BINARY_DIR}/Sources")
  SET (VTK_PATCH_PATH "${VTK_SOURCE_DIR}/Patches")
  SET (VTK_SOURCE_PATH "${VTK_UNPACK_PATH}/VTK") # where sources are 
  
  ##The macro must run only one time
  IF (NOT EXISTS "${VTK_SOURCE_PATH}")
  	FIND_AND_UNPACK_PACKAGE (VTK ${VTK_PACKAGE_PATH} "${VTK_UNPACK_PATH}")
  	FIND_AND_APPLAY_PATCHES(VTK ${VTK_PATCH_PATH} "${VTK_SOURCE_PATH}")
  ENDIF (NOT EXISTS "${VTK_SOURCE_PATH}")
ENDIF (EXISTS "${VTK_SOURCE_DIR}/Packages")

##ci vorra' un controllo se tutto e' stato eseguito???


IF (VTK_SOURCE_PATH)
  MESSAGE(STATUS "VTK: Configuring VTK")

  # default setting for building the VTK library
  SET (VTK_USE_PARALLEL 1)
  SET (VTK_USE_PATENTED 1)
  SET (VTK_USE_HYBRID 1)
  SET (VTK_RENDERING_BACKEND "OpenGL")
	
  SET (VTK_BINARY_PATH "${VTK_BINARY_DIR}/Build" )
  OPTION (VTK_FORCE_CONFIGURE "Weither to force configuration of VTK" FALSE)
    
  # check if already configured
  IF(NOT EXISTS ${VTK_BINARY_DIR}/build.cmake)
	  SET(VTK_BUILD_SHARED_LIBS ${ALBA_BUILD_ALBADLL})
	ELSE(NOT EXISTS ${VTK_BINARY_DIR}/build.cmake)
    LOAD_CACHE(${VTK_BINARY_PATH} READ_WITH_PREFIX VTK_ BUILD_SHARED_LIBS)
  ENDIF(NOT EXISTS ${VTK_BINARY_DIR}/build.cmake)
	
  IF(NOT VTK_BUILD_SHARED_LIBS EQUAL ${ALBA_BUILD_ALBADLL})
  	SET(VTK_FORCE_CONFIGURE TRUE)
  ELSE(NOT VTK_BUILD_SHARED_LIBS EQUAL ${ALBA_BUILD_ALBADLL})
  	SET(VTK_FORCE_CONFIGURE FALSE)
  ENDIF(NOT VTK_BUILD_SHARED_LIBS EQUAL ${ALBA_BUILD_ALBADLL})
  
  IF (VTK_FORCE_CONFIGURE OR NOT EXISTS ${VTK_BINARY_DIR}/build.cmake)
    # Configuring the VTK library    
    FILE(MAKE_DIRECTORY "${VTK_BINARY_PATH}")
    
    # Run configuration of VTK library
    MESSAGE(STATUS "VTK: Configuring external VTK project")
    EXEC_PROGRAM(${CMAKE_COMMAND} "${VTK_BINARY_PATH}" ARGS "${VTK_SOURCE_PATH}" -G"${CMAKE_GENERATOR}" -DLIBRARY_OUTPUT_PATH:PATH="${LIBRARY_OUTPUT_PATH}" -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DEXECUTABLE_OUTPUT_PATH:PATH="${EXECUTABLE_OUTPUT_PATH}" -DVTK_USE_PARALLEL:BOOL=${VTK_USE_PARALLEL} -DVTK_USE_PATENTED:BOOL=${VTK_USE_PATENTED}
    -DVTK_USE_HYBRID:BOOL=${VTK_USE_HYBRID} -DVTK_USE_64BIT_IDS:BOOL=TRUE -DVTK_USE_ANSI_STDLIB:BOOL=${VTK_USE_ANSI_STDLIB} -DBUILD_TESTING:BOOL=OFF -DBUILD_EXAMPLES:BOOL=OFF -DVTK_RENDERING_BACKEND:STRING="${VTK_RENDERING_BACKEND}"
    -DBUILD_SHARED_LIBS=${ALBA_BUILD_ALBADLL}
     OUTPUT_VARIABLE CMAKE_OUTPUT RETURN_VALUE CMAKE_RETURN)
    
    # write configure log file
    FILE(WRITE ${VTK_BINARY_DIR}/CMake-Log.txt "CMake configure output ${CMAKE_OUTPUT}" )
  
    IF (CMAKE_RETURN)
      # in case of error prompt an error dialog
      MESSAGE("CMake configure returned value = " ${CMAKE_RETURN})
      MESSAGE(FATAL_ERROR "${SUBPROJECT_NAME} configure error, cmake output = " ${CMAKE_OUTPUT})     
    ELSE (CMAKE_RETURN)
      # load VTK build command from cmake's cache
      LOAD_CACHE(${VTK_BINARY_PATH} READ_WITH_PREFIX VTK_ MAKECOMMAND)
      
      #Split space separated arguments into a semi-colon separated list. Necessary for correct command line generation
      SEPARATE_ARGUMENTS(VTK_MAKECOMMAND)
      IF("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 2.0)
      	STRING (REPLACE "\\" "\\\\"  VTK_MAKECOMMAND "${VTK_MAKECOMMAND}")
      ENDIF("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 2.0)
      	
      MESSAGE(STATUS "Creating file ${VTK_BINARY_DIR}/build.cmake")
    	CONFIGURE_FILE("${VTK_SOURCE_DIR}/build.cmake.in" "${VTK_BINARY_DIR}/build.cmake" ESCAPE_QUOTES @ONLY IMMEDIATE)
    ENDIF (CMAKE_RETURN)
  ENDIF (VTK_FORCE_CONFIGURE OR NOT EXISTS ${VTK_BINARY_DIR}/build.cmake)
  
  IF (EXISTS ${VTK_BINARY_DIR}/build.cmake)
    # custom command to build the VTK library
    
    IF (VTK_BUILD_SHARED_LIBS)
    	MESSAGE(STATUS "Adding custom command for VTK SHARED library: ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}vtkCommon${CMAKE_SHARED_LIBRARY_SUFFIX}")
      ADD_CUSTOM_COMMAND(OUTPUT "${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}vtkCommon${CMAKE_SHARED_LIBRARY_SUFFIX}"
                     COMMAND ${CMAKE_COMMAND}
                     ARGS 
                     -DMY_BUILD_PATH:STRING=${CMAKE_CFG_INTDIR} 
                     -P ${VTK_BINARY_DIR}/build.cmake 
                     MAIN_DEPENDENCY ${VTK_BINARY_DIR}/build.cmake
                     COMMENT "Custom command used to build VTK library")
      ADD_CUSTOM_TARGET(BUILD_VTK_LIBRARY DEPENDS "${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}vtkCommon${CMAKE_SHARED_LIBRARY_SUFFIX}" )
    ELSE (VTK_BUILD_SHARED_LIBS)
      MESSAGE(STATUS "Adding custom command for VTK STATIC library: ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}vtkCommon${CMAKE_STATIC_LIBRARY_SUFFIX}")
      ADD_CUSTOM_COMMAND(OUTPUT "${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}vtkCommon${CMAKE_STATIC_LIBRARY_SUFFIX}"
                     COMMAND ${CMAKE_COMMAND}
                     ARGS 
                     -DMY_BUILD_PATH:STRING=${CMAKE_CFG_INTDIR} 
                     -P ${VTK_BINARY_DIR}/build.cmake 
                     MAIN_DEPENDENCY ${VTK_BINARY_DIR}/build.cmake
                     COMMENT "Custom command used to build VTK library")
      ADD_CUSTOM_TARGET(BUILD_VTK_LIBRARY ALL DEPENDS "${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}vtkCommon${CMAKE_STATIC_LIBRARY_SUFFIX}" )
    ENDIF (VTK_BUILD_SHARED_LIBS)
    
    SET (VTK_CONFIGURED 1)
    
    # Add VTK library to MFL_TARGETS
    SET (MFL_TARGETS ${MFL_TARGETS} BUILD_VTK_LIBRARY)      
    
  ELSE (EXISTS ${VTK_BINARY_DIR}/build.cmake)
      SET (VTK_CONFIGURED 0)
      SET (CONFIGURE_ERROR 1)
  ENDIF (EXISTS ${VTK_BINARY_DIR}/build.cmake)
ENDIF (VTK_SOURCE_PATH)

 

MARK_AS_ADVANCED (
  VTK_FORCE_CONFIGURE
  VTK_USE_PARALLEL
  VTK_USE_PATENTED
  VTK_USE_HYBRID
  VTK_USE_RENDERING
  VTK_USE_ANSI_STDLIB
  VTK_BUILD_FILE
)

# Write the list of variables and their values that are defined by CMake when running this CMakeList file
IF (DEBUG_MESSAGES)
  FILE(WRITE ${VTK_BINARY_DIR}/AllVariables.txt "")
  GET_CMAKE_PROPERTY(VARS VARIABLES)
  FOREACH(var ${VARS})
  	FILE(APPEND ${VTK_BINARY_DIR}/AllVariables.txt 
  							"${var} \"${${var}}\"\n")
  ENDFOREACH(var ${VARS})
ENDIF (DEBUG_MESSAGES)
