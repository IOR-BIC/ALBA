#
# Program:   MULTIMOD APPLICATION FRAMEWORK (MAF)
# Module:    $RCSfile: Configure.cmake,v $
# Language:  CMake 1.2
# Date:      $Date: 2012-02-14 15:32:02 $
# Version:   $Revision: 1.1.2.2 $
#
# Description:
# Project file for configuring the BTK library as an external project.
# Return variables:
# BTK_CONFIGURED  true if library configured correctly


OPTION(MAF_USE_CRYPTO "Find and Link the CRYPTO library." ON)
INCLUDE (${MFL_SOURCE_DIR}/modules/PackagesMacro.cmake)
INCLUDE (${MFL_SOURCE_DIR}/modules/PatchMacro.cmake)

# this is to build BTK inside the MAF tree
IF (EXISTS "${BTK_SOURCE_DIR}/Sources")   
  SET (BTK_SOURCE_PATH "${BTK_SOURCE_DIR}/Sources")
ENDIF (EXISTS "${BTK_SOURCE_DIR}/Sources")

#look if there is a package for BTK
IF (EXISTS "${BTK_SOURCE_DIR}/Packages")   
  MESSAGE(STATUS "BTK: Found BTK packages")
  # here we should unpack into "${BTK_BINARY_DIR}/Sources"
  SET (BTK_PACKAGE_PATH "${BTK_SOURCE_DIR}/Packages")
  SET (BTK_PATCH_PATH "${BTK_SOURCE_DIR}/Patches")
  SET (BTK_Extension_PACKAGE_PATH "${BTK_SOURCE_DIR}/Packages")
  ##The macro must run only one time
  SET (BTK_SOURCE_PATH "${BTK_BINARY_DIR}/Sources/BTK")
  SET (BTK_PATCH_PATH "${BTK_SOURCE_DIR}/Patches")
  IF (NOT EXISTS "${BTK_SOURCE_PATH}")
	  FIND_AND_UNPACK_PACKAGE (BTK ${BTK_PACKAGE_PATH} "${BTK_BINARY_DIR}/Sources" ${BTK_SOURCE_PATH})
	  FIND_AND_APPLAY_PATCHES (BTK ${BTK_PATCH_PATH} "${BTK_SOURCE_PATH}")
  ENDIF (NOT EXISTS "${BTK_SOURCE_PATH}")
ENDIF (EXISTS "${BTK_SOURCE_DIR}/Packages")

IF (BTK_SOURCE_PATH)
  MESSAGE(STATUS "BTK: Configuring BTK")

  # default setting for building the BTK library  
  SET (BTK_BINARY_PATH "${BTK_BINARY_DIR}/Build" )
  OPTION (BTK_FORCE_CONFIGURE "Weither to force configuration of BTK" FALSE)
  # check if already configured
  IF (BTK_FORCE_CONFIGURE OR NOT EXISTS ${BTK_BINARY_DIR}/build.cmake)
    # Configuring the BTK library    
    FILE(MAKE_DIRECTORY "${BTK_BINARY_PATH}")
    
    # Run configuration of BTK library
    MESSAGE(STATUS "BTK: Configuring external BTK project")
    EXEC_PROGRAM(${CMAKE_COMMAND} "${BTK_BINARY_PATH}" ARGS "${BTK_SOURCE_PATH}" -G"${CMAKE_GENERATOR}" -DLIBRARY_OUTPUT_PATH:PATH="${LIBRARY_OUTPUT_PATH}" -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DEXECUTABLE_OUTPUT_PATH:PATH="${EXECUTABLE_OUTPUT_PATH}" -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
     OUTPUT_VARIABLE CMAKE_OUTPUT RETURN_VALUE CMAKE_RETURN)
    
    # write configure log file
    FILE(WRITE ${BTK_BINARY_DIR}/CMake-Log.txt "CMake configure output ${CMAKE_OUTPUT}" )
  
    IF (CMAKE_RETURN)
      # in case of error prompt an error dialog
      MESSAGE("CMake configure returned value = " ${CMAKE_RETURN})
      MESSAGE(FATAL_ERROR "${SUBPROJECT_NAME} configure error, cmake output = " ${CMAKE_OUTPUT})     
    ELSE (CMAKE_RETURN)
      # load BTK build command from cmake's cache
      LOAD_CACHE(${BTK_BINARY_PATH} READ_WITH_PREFIX BTK_ MAKECOMMAND)
      LOAD_CACHE(${BTK_BINARY_PATH} READ_WITH_PREFIX BTK_ BUILD_SHARED)
      
      #Split space separated arguments into a semi-colon separated list. Necessary for correct command line generation
      SEPARATE_ARGUMENTS(BTK_MAKECOMMAND)
      IF("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 2.0)
      	STRING (REPLACE "\\" "\\\\"  BTK_MAKECOMMAND "${BTK_MAKECOMMAND}")
      ENDIF("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 2.0)
      MESSAGE(STATUS "Creating file ${BTK_BINARY_DIR}/build.cmake")
    	CONFIGURE_FILE("${BTK_SOURCE_DIR}/build.cmake.in" "${BTK_BINARY_DIR}/build.cmake" ESCAPE_QUOTES @ONLY IMMEDIATE)
    	
    ENDIF (CMAKE_RETURN)
  ENDIF (BTK_FORCE_CONFIGURE OR NOT EXISTS ${BTK_BINARY_DIR}/build.cmake)
  
  IF (EXISTS ${BTK_BINARY_DIR}/build.cmake)
    # custom command to build the BTK library
    IF (BTK_BUILD_SHARED)
    	MESSAGE(STATUS "Adding custom command for BTK SHARED library: ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}BTK${CMAKE_SHARED_LIBRARY_SUFFIX}")
      ADD_CUSTOM_COMMAND(OUTPUT "${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}BTK${CMAKE_SHARED_LIBRARY_SUFFIX}"
                     COMMAND ${CMAKE_COMMAND}
                     ARGS 
                     -DMY_BUILD_PATH:STRING=${CMAKE_CFG_INTDIR} 
                     -P ${BTK_BINARY_DIR}/build.cmake 
                     MAIN_DEPENDENCY ${BTK_BINARY_DIR}/build.cmake
                     COMMENT "Custom command used to build BTK library")
      ADD_CUSTOM_TARGET(BUILD_BTK_LIBRARY DEPENDS "${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}BTK${CMAKE_SHARED_LIBRARY_SUFFIX}" )
    ELSE (BTK_BUILD_SHARED)
      MESSAGE(STATUS "Adding custom command for BTK STATIC library: ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}BTK${CMAKE_STATIC_LIBRARY_SUFFIX}")
      ADD_CUSTOM_COMMAND(OUTPUT "${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}BTK${CMAKE_STATIC_LIBRARY_SUFFIX}"
                     COMMAND ${CMAKE_COMMAND}
                     ARGS 
                     -DMY_BUILD_PATH:STRING=${CMAKE_CFG_INTDIR} 
                     -P ${BTK_BINARY_DIR}/build.cmake 
                     MAIN_DEPENDENCY ${BTK_BINARY_DIR}/build.cmake
                     COMMENT "Custom command used to build BTK library")
      ADD_CUSTOM_TARGET(BUILD_BTK_LIBRARY ALL DEPENDS "${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}BTK${CMAKE_STATIC_LIBRARY_SUFFIX}" )
    ENDIF (BTK_BUILD_SHARED)
    
    SET (BTK_CONFIGURED 1)
    
    # Add BTK library to MFL_TARGETS
    SET (MFL_TARGETS ${MFL_TARGETS} BUILD_BTK_LIBRARY)      
    
  ELSE (EXISTS ${BTK_BINARY_DIR}/build.cmake)
      SET (BTK_CONFIGURED 0)
      SET (CONFIGURE_ERROR 1)
  ENDIF (EXISTS ${BTK_BINARY_DIR}/build.cmake)
ENDIF (BTK_SOURCE_PATH)

MARK_AS_ADVANCED (
  BTK_BUILD_FILE
  BTK_FORCE_CONFIGURE
)

# Write the list of variables and their values that are defined by CMake when running this CMakeList file
IF (DEBUG_MESSAGES)
  #FILE(WRITE ${BTK_BINARY_DIR}/AllVariables.txt "")
  GET_CMAKE_PROPERTY(VARS VARIABLES)
  FOREACH(var ${VARS})
  	FILE(APPEND ${BTK_BINARY_DIR}/AllVariables.txt 
  							"${var} \"${${var}}\"\n")
  ENDFOREACH(var ${VARS})
ENDIF (DEBUG_MESSAGES)