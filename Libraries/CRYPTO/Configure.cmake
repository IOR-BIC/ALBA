#
# Program:   ALBA (Agile Library for Biomedical Applications)
# Module:    $RCSfile: Configure.cmake,v $
# Language:  CMake 1.2
# Date:      $Date: 2011-05-18 07:16:55 $
# Version:   $Revision: 1.10.2.1 $
#
# Description:
# Project file for configuring the CRYPTO library as an external project.
# Return variables:
# CRYPTO_CONFIGURED  true if library configured correctly



INCLUDE (${MFL_SOURCE_DIR}/modules/PackagesMacro.cmake)
INCLUDE (${MFL_SOURCE_DIR}/modules/PatchMacro.cmake)

# this is to build CRYPTO inside the ALBA tree
IF (EXISTS "${CRYPTO_SOURCE_DIR}/Sources")   
  SET (CRYPTO_SOURCE_PATH "${CRYPTO_SOURCE_DIR}/Sources")
ENDIF (EXISTS "${CRYPTO_SOURCE_DIR}/Sources")

#look if there is a package for CRYPTO
IF (EXISTS "${CRYPTO_SOURCE_DIR}/Packages")   
  MESSAGE(STATUS "CRYPTO: Found CRYPTO packages")
  # here we should unpack into "${CRYPTO_BINARY_DIR}/Sources"
  SET (CRYPTO_PACKAGE_PATH "${CRYPTO_SOURCE_DIR}/Packages")
	
  ##The macro must run only one time
  SET (CRYPTO_SOURCE_PATH "${CRYPTO_BINARY_DIR}/Sources")
  SET (CRYPTO_PATCH_PATH "${CRYPTO_SOURCE_DIR}/Patches")
  IF (NOT EXISTS "${CRYPTO_SOURCE_PATH}")
				FIND_AND_UNPACK_PACKAGE (CRYPTOPP565 ${CRYPTO_PACKAGE_PATH} "${CRYPTO_BINARY_DIR}/Sources" ${CRYPTO_SOURCE_PATH})
  ENDIF (NOT EXISTS "${CRYPTO_SOURCE_PATH}")
	
	FIND_AND_APPLAY_PATCHES (CRYPTOPP565 ${CRYPTO_PATCH_PATH} "${CRYPTO_SOURCE_PATH}")
	
ENDIF (EXISTS "${CRYPTO_SOURCE_DIR}/Packages")

IF (CRYPTO_SOURCE_PATH)
  MESSAGE(STATUS "CRYPTO: Configuring CRYPTO")

  # default setting for building the CRYPTO library  
  SET (CRYPTO_BINARY_PATH "${CRYPTO_BINARY_DIR}/Build" )
  OPTION (CRYPTO_FORCE_CONFIGURE "Weither to force configuration of CRYPTO" FALSE)
  # check if already configured
  IF (CRYPTO_FORCE_CONFIGURE OR NOT EXISTS ${CRYPTO_BINARY_DIR}/build.cmake)
    # Configuring the CRYPTO library    
    FILE(MAKE_DIRECTORY "${CRYPTO_BINARY_PATH}")
    
    # Run configuration of CRYPTO library
    MESSAGE(STATUS "CRYPTO: Configuring external CRYPTO project")
    EXEC_PROGRAM(${CMAKE_COMMAND} "${CRYPTO_BINARY_PATH}" ARGS "${CRYPTO_SOURCE_PATH}" -G"${CMAKE_GENERATOR}" -DLIBRARY_OUTPUT_PATH:PATH="${LIBRARY_OUTPUT_PATH}" -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DEXECUTABLE_OUTPUT_PATH:PATH="${EXECUTABLE_OUTPUT_PATH}" -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
     OUTPUT_VARIABLE CMAKE_OUTPUT RETURN_VALUE CMAKE_RETURN)
    
    # write configure log file
    FILE(WRITE ${CRYPTO_BINARY_DIR}/CMake-Log.txt "CMake configure output ${CMAKE_OUTPUT}" )
  
    IF (CMAKE_RETURN)
      # in case of error prompt an error dialog
      MESSAGE("CMake configure returned value = " ${CMAKE_RETURN})
      MESSAGE(FATAL_ERROR "${SUBPROJECT_NAME} configure error, cmake output = " ${CMAKE_OUTPUT})     
    ELSE (CMAKE_RETURN)
      # load CRYPTO build command from cmake's cache
      LOAD_CACHE(${CRYPTO_BINARY_PATH} READ_WITH_PREFIX CRYPTO_ MAKECOMMAND)
      LOAD_CACHE(${CRYPTO_BINARY_PATH} READ_WITH_PREFIX CRYPTO_ BUILD_SHARED)
      
      #Split space separated arguments into a semi-colon separated list. Necessary for correct command line generation
      SEPARATE_ARGUMENTS(CRYPTO_MAKECOMMAND)
      IF("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 2.0)
      	STRING (REPLACE "\\" "\\\\"  CRYPTO_MAKECOMMAND "${CRYPTO_MAKECOMMAND}")
      ENDIF("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 2.0)
      MESSAGE(STATUS "Creating file ${CRYPTO_BINARY_DIR}/build.cmake")
    	CONFIGURE_FILE("${CRYPTO_SOURCE_DIR}/build.cmake.in" "${CRYPTO_BINARY_DIR}/build.cmake" ESCAPE_QUOTES @ONLY IMMEDIATE)
    	
    ENDIF (CMAKE_RETURN)
  ENDIF (CRYPTO_FORCE_CONFIGURE OR NOT EXISTS ${CRYPTO_BINARY_DIR}/build.cmake)
  
  IF (EXISTS ${CRYPTO_BINARY_DIR}/build.cmake)
    # custom command to build the CRYPTO library
    IF (CRYPTO_BUILD_SHARED)
    	MESSAGE(STATUS "Adding custom command for CRYPTO SHARED library: ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}CRYPTO${CMAKE_SHARED_LIBRARY_SUFFIX}")
      ADD_CUSTOM_COMMAND(OUTPUT "${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}CRYPTO${CMAKE_SHARED_LIBRARY_SUFFIX}"
                     COMMAND ${CMAKE_COMMAND}
                     ARGS 
                     -DMY_BUILD_PATH:STRING=${CMAKE_CFG_INTDIR} 
                     -P ${CRYPTO_BINARY_DIR}/build.cmake 
                     MAIN_DEPENDENCY ${CRYPTO_BINARY_DIR}/build.cmake
                     COMMENT "Custom command used to build CRYPTO library")
      ADD_CUSTOM_TARGET(BUILD_CRYPTO_LIBRARY DEPENDS "${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}CRYPTO${CMAKE_SHARED_LIBRARY_SUFFIX}" )
    ELSE (CRYPTO_BUILD_SHARED)
      MESSAGE(STATUS "Adding custom command for CRYPTO STATIC library: ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}CRYPTO${CMAKE_STATIC_LIBRARY_SUFFIX}")
      ADD_CUSTOM_COMMAND(OUTPUT "${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}CRYPTO${CMAKE_STATIC_LIBRARY_SUFFIX}"
                     COMMAND ${CMAKE_COMMAND}
                     ARGS 
                     -DMY_BUILD_PATH:STRING=${CMAKE_CFG_INTDIR} 
                     -P ${CRYPTO_BINARY_DIR}/build.cmake 
                     MAIN_DEPENDENCY ${CRYPTO_BINARY_DIR}/build.cmake
                     COMMENT "Custom command used to build CRYPTO library")
      ADD_CUSTOM_TARGET(BUILD_CRYPTO_LIBRARY ALL DEPENDS "${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}CRYPTO${CMAKE_STATIC_LIBRARY_SUFFIX}" )
    ENDIF (CRYPTO_BUILD_SHARED)
    
    SET (CRYPTO_CONFIGURED 1)
    
    # Add CRYPTO library to MFL_TARGETS
    SET (MFL_TARGETS ${MFL_TARGETS} BUILD_CRYPTO_LIBRARY)      
    
  ELSE (EXISTS ${CRYPTO_BINARY_DIR}/build.cmake)
      SET (CRYPTO_CONFIGURED 0)
      SET (CONFIGURE_ERROR 1)
  ENDIF (EXISTS ${CRYPTO_BINARY_DIR}/build.cmake)
ENDIF (CRYPTO_SOURCE_PATH)

MARK_AS_ADVANCED (
  CRYPTO_BUILD_FILE
  CRYPTO_FORCE_CONFIGURE
)

# Write the list of variables and their values that are defined by CMake when running this CMakeList file
IF (DEBUG_MESSAGES)
  #FILE(WRITE ${CRYPTO_BINARY_DIR}/AllVariables.txt "")
  GET_CMAKE_PROPERTY(VARS VARIABLES)
  FOREACH(var ${VARS})
  	FILE(APPEND ${CRYPTO_BINARY_DIR}/AllVariables.txt 
  							"${var} \"${${var}}\"\n")
  ENDFOREACH(var ${VARS})
ENDIF (DEBUG_MESSAGES)
