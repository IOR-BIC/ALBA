#
# Program:   MULTIMOD APPLICATION FRAMEWORK (MAF)
# Module:    $RCSfile: Configure.cmake,v $
# Language:  CMake 1.2
# Date:      $Date: 2007-03-07 09:45:58 $
# Version:   $Revision: 1.4 $
#
# Description:
# Project file for configuring the ITK library as an external project.
# Return variables:
# ITK_CONFIGURED  true if library configured correctly



INCLUDE (${MFL_SOURCE_DIR}/modules/PackagesMacro.cmake)
INCLUDE (${MFL_SOURCE_DIR}/modules/PatchMacro.cmake)

#
# This option is for using ANSI stream within ITK library
#
# this is to build ITK inside the MAF tree
IF (EXISTS "${ITK_SOURCE_DIR}/Sources")   
  SET (ITK_SOURCE_PATH "${ITK_SOURCE_DIR}/Sources")
ENDIF (EXISTS "${ITK_SOURCE_DIR}/Sources")

#look if there is a package for ITK
IF (EXISTS "${ITK_SOURCE_DIR}/Packages")   
  MESSAGE(STATUS "ITK: Found ITK packages")
  # here we should unpack into "${ITK_BINARY_DIR}/Sources"
  SET (ITK_PACKAGE_PATH "${ITK_SOURCE_DIR}/Packages")
  SET (ITK_UNPACK_PATH "${ITK_BINARY_DIR}/Sources")
  SET (ITK_PATCH_PATH "${ITK_SOURCE_DIR}/Patches")
  SET (ITK_SOURCE_PATH "${ITK_UNPACK_PATH}/InsightToolkit-3.0.1") # where sources are 
  
  ##The macro must run only one time
  IF (NOT EXISTS "${ITK_SOURCE_PATH}")
  	FIND_AND_UNPACK_PACKAGE (ITK ${ITK_PACKAGE_PATH} "${ITK_UNPACK_PATH}")
  	FIND_AND_APPLAY_PATCHES(ITK ${ITK_PATCH_PATH} "${ITK_SOURCE_PATH}")
  ENDIF (NOT EXISTS "${ITK_SOURCE_PATH}")
ENDIF (EXISTS "${ITK_SOURCE_DIR}/Packages")

##ci vorra' un controllo se tutto e' stato eseguito???


IF (ITK_SOURCE_PATH)
  MESSAGE(STATUS "ITK: Configuring ITK")

  # default setting for building the ITK library
  #SET (VTK_USE_PARALLEL 1)
  #SET (VTK_USE_PATENTED 1)
  #SET (VTK_USE_HYBRID 1)
  
  SET (ITK_BINARY_PATH "${ITK_BINARY_DIR}/Build" )
  OPTION (ITK_FORCE_CONFIGURE "Weither to force configuration of ITK" FALSE)
  # check if already configured
  IF (ITK_FORCE_CONFIGURE OR NOT EXISTS ${ITK_BINARY_DIR}/build.cmake)
    # Configuring the ITK library    
    FILE(MAKE_DIRECTORY "${ITK_BINARY_PATH}")
    
    # Run configuration of ITK library
    MESSAGE(STATUS "ITK: Configuring external ITK project")
    EXEC_PROGRAM(${CMAKE_COMMAND} "${ITK_BINARY_PATH}" ARGS "${ITK_SOURCE_PATH}" -G"${CMAKE_GENERATOR}" -DLIBRARY_OUTPUT_PATH:PATH="${LIBRARY_OUTPUT_PATH}" -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DEXECUTABLE_OUTPUT_PATH:PATH="${EXECUTABLE_OUTPUT_PATH}" -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS} -DBUILD_TESTING:BOOL=OFF -DBUILD_EXAMPLES:BOOL=OFF
     OUTPUT_VARIABLE CMAKE_OUTPUT RETURN_VALUE CMAKE_RETURN)
    
    # write configure log file
    FILE(WRITE ${ITK_BINARY_DIR}/CMake-Log.txt "CMake configure output ${CMAKE_OUTPUT}" )
  
    IF (CMAKE_RETURN)
      # in case of error prompt an error dialog
      MESSAGE("CMake configure returned value = " ${CMAKE_RETURN})
      MESSAGE(FATAL_ERROR "${SUBPROJECT_NAME} configure error, cmake output = " ${CMAKE_OUTPUT})     
    ELSE (CMAKE_RETURN)
      # load ITK build command from cmake's cache
      LOAD_CACHE(${ITK_BINARY_PATH} READ_WITH_PREFIX ITK_ MAKECOMMAND)
      LOAD_CACHE(${ITK_BINARY_PATH} READ_WITH_PREFIX ITK_ BUILD_SHARED)
      
      #Split space separated arguments into a semi-colon separated list. Necessary for correct command line generation
      SEPARATE_ARGUMENTS(ITK_MAKECOMMAND)
      IF("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 2.0)
      	STRING (REPLACE "\\" "\\\\"  ITK_MAKECOMMAND "${ITK_MAKECOMMAND}")
      ENDIF("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 2.0)
      
      MESSAGE(STATUS "Creating file ${ITK_BINARY_DIR}/build.cmake")
    	CONFIGURE_FILE("${ITK_SOURCE_DIR}/build.cmake.in" "${ITK_BINARY_DIR}/build.cmake" ESCAPE_QUOTES @ONLY IMMEDIATE)
    ENDIF (CMAKE_RETURN)
  ENDIF (ITK_FORCE_CONFIGURE OR NOT EXISTS ${ITK_BINARY_DIR}/build.cmake)
  
  IF (EXISTS ${ITK_BINARY_DIR}/build.cmake)
    # custom command to build the ITK library
    IF (ITK_BUILD_SHARED)
    	MESSAGE(STATUS "Adding custom command for ITK SHARED library: ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}itkCommon${CMAKE_SHARED_LIBRARY_SUFFIX}")
      ADD_CUSTOM_COMMAND(OUTPUT "${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}itkCommon${CMAKE_SHARED_LIBRARY_SUFFIX}"
                     COMMAND ${CMAKE_COMMAND}
                     ARGS 
                     -DMY_BUILD_PATH:STRING=${CMAKE_CFG_INTDIR} 
                     -P ${ITK_BINARY_DIR}/build.cmake 
                     MAIN_DEPENDENCY ${ITK_BINARY_DIR}/build.cmake
                     COMMENT "Custom command used to build ITK library")
      ADD_CUSTOM_TARGET(BUILD_ITK_LIBRARY DEPENDS "${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}itkCommon${CMAKE_SHARED_LIBRARY_SUFFIX}" )
    ELSE (ITK_BUILD_SHARED)
      MESSAGE(STATUS "Adding custom command for ITK STATIC library: ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}itkCommon${CMAKE_STATIC_LIBRARY_SUFFIX}")
      ADD_CUSTOM_COMMAND(OUTPUT "${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}itkCommon${CMAKE_STATIC_LIBRARY_SUFFIX}"
                     COMMAND ${CMAKE_COMMAND}
                     ARGS 
                     -DMY_BUILD_PATH:STRING=${CMAKE_CFG_INTDIR} 
                     -P ${ITK_BINARY_DIR}/build.cmake 
                     MAIN_DEPENDENCY ${ITK_BINARY_DIR}/build.cmake
                     COMMENT "Custom command used to build ITK library")
      ADD_CUSTOM_TARGET(BUILD_ITK_LIBRARY ALL DEPENDS "${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}itkCommon${CMAKE_STATIC_LIBRARY_SUFFIX}" )
    ENDIF (ITK_BUILD_SHARED)
    
    SET (ITK_CONFIGURED 1)
    
    # Add ITK library to MFL_TARGETS
    SET (MFL_TARGETS ${MFL_TARGETS} BUILD_ITK_LIBRARY)      
    
  ELSE (EXISTS ${ITK_BINARY_DIR}/build.cmake)
      SET (ITK_CONFIGURED 0)
      SET (CONFIGURE_ERROR 1)
  ENDIF (EXISTS ${ITK_BINARY_DIR}/build.cmake)
ENDIF (ITK_SOURCE_PATH)

 

MARK_AS_ADVANCED (
  ITK_FORCE_CONFIGURE
  #VTK_USE_PARALLEL
  #VTK_USE_PATENTED
  #VTK_USE_HYBRID
  #VTK_USE_RENDERING
  #VTK_USE_ANSI_STDLIB
  ITK_BUILD_FILE
)

# Write the list of variables and their values that are defined by CMake when running this CMakeList file
IF (DEBUG_MESSAGES)
  FILE(WRITE ${ITK_BINARY_DIR}/AllVariables.txt "")
  GET_CMAKE_PROPERTY(VARS VARIABLES)
  FOREACH(var ${VARS})
  	FILE(APPEND ${ITK_BINARY_DIR}/AllVariables.txt 
  							"${var} \"${${var}}\"\n")
  ENDFOREACH(var ${VARS})
ENDIF (DEBUG_MESSAGES)
