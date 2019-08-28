#
# Program:   ALBA (Agile Library for Biomedical Applications)
# Module:    $RCSfile: Configure.cmake,v $
# Language:  CMake 1.2
# Date:      $Date: 2008-03-13 17:12:14 $
# Version:   $Revision: 1.4 $
#
# Description:
# Project file for configuring the cURL library as an external project.
# Return variables:
# CURL_CONFIGURED  true if library configured correctly

INCLUDE (${MFL_SOURCE_DIR}/modules/PackagesMacro.cmake)
INCLUDE (${MFL_SOURCE_DIR}/modules/PatchMacro.cmake)

#look if there is a package for CURL
IF (EXISTS "${CURL_SOURCE_DIR}/Packages")   
  MESSAGE(STATUS "CURL: Found CURL packages")
  SET (CURL_PACKAGE_PATH "${CURL_SOURCE_DIR}/Packages")
  SET (CURL_PATCH_PATH "${CURL_SOURCE_DIR}/Patches")
  ##The macro must run only one time
  SET (CURL_SOURCE_PATH "${CURL_BINARY_DIR}/Sources")
  IF (NOT EXISTS "${CURL_SOURCE_PATH}")
    FIND_AND_UNPACK_PACKAGE (curl ${CURL_PACKAGE_PATH} "${CURL_BINARY_DIR}/Sources" ${CURL_SOURCE_PATH})
    FIND_AND_UNPACK_PACKAGE (CMake ${CURL_PACKAGE_PATH} "${CURL_BINARY_DIR}/Sources" ${CURL_SOURCE_PATH})
  ENDIF (NOT EXISTS "${CURL_SOURCE_PATH}")
		  	 
	FIND_AND_APPLAY_PATCHES(curl ${CURL_PATCH_PATH} "${CURL_SOURCE_PATH}")

ENDIF (EXISTS "${CURL_SOURCE_DIR}/Packages")


IF (CURL_SOURCE_PATH)
  MESSAGE(STATUS "CURL: Configuring CURL")

  # default setting for building the CURL library  
  SET (CURL_BINARY_PATH "${CURL_BINARY_DIR}/Build" )
  OPTION (CURL_FORCE_CONFIGURE "Weither to force configuration of CURL" FALSE)
  # check if already configured
  IF (CURL_FORCE_CONFIGURE OR NOT EXISTS ${CURL_BINARY_DIR}/build.cmake)
    # Configuring the CURL library    
    FILE(MAKE_DIRECTORY "${CURL_BINARY_PATH}")
    
    # Run configuration of CURL library
    MESSAGE(STATUS "CURL: Configuring external CURL project")
    EXEC_PROGRAM(${CMAKE_COMMAND} "${CURL_BINARY_PATH}" ARGS "${CURL_SOURCE_PATH}" -G"${CMAKE_GENERATOR}" -DLIBRARY_OUTPUT_PATH:PATH="${LIBRARY_OUTPUT_PATH}" -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DEXECUTABLE_OUTPUT_PATH:PATH="${EXECUTABLE_OUTPUT_PATH}" -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
     OUTPUT_VARIABLE CMAKE_OUTPUT RETURN_VALUE CMAKE_RETURN)
    
    # write configure log file
    FILE(WRITE ${CURL_BINARY_DIR}/CMake-Log.txt "CMake configure output ${CMAKE_OUTPUT}" )
  
    IF (CMAKE_RETURN)
      # in case of error prompt an error dialog
      MESSAGE("CMake configure returned value = " ${CMAKE_RETURN})
      MESSAGE(FATAL_ERROR "${SUBPROJECT_NAME} configure error, cmake output = " ${CMAKE_OUTPUT})     
    ELSE (CMAKE_RETURN)
      # load CURL build command from cmake's cache
      LOAD_CACHE(${CURL_BINARY_PATH} READ_WITH_PREFIX CURL_ MAKECOMMAND)
      LOAD_CACHE(${CURL_BINARY_PATH} READ_WITH_PREFIX CURL_ BUILD_SHARED)
      
      #Split space separated arguments into a semi-colon separated list. Necessary for correct command line generation
      SEPARATE_ARGUMENTS(CURL_MAKECOMMAND)
      IF("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 2.0)
      	STRING (REPLACE "\\" "\\\\"  CURL_MAKECOMMAND "${CURL_MAKECOMMAND}")
      ENDIF("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 2.0)
      MESSAGE(STATUS "Creating file ${CURL_BINARY_DIR}/build.cmake")
    	CONFIGURE_FILE("${CURL_SOURCE_DIR}/build.cmake.in" "${CURL_BINARY_DIR}/build.cmake" ESCAPE_QUOTES @ONLY IMMEDIATE)

    ENDIF (CMAKE_RETURN)
  ENDIF (CURL_FORCE_CONFIGURE OR NOT EXISTS ${CURL_BINARY_DIR}/build.cmake)
  
  
  IF (EXISTS ${CURL_BINARY_DIR}/build.cmake)
  
#    IF (WIN32)
      # custom command to build the CURL library
      IF (CURL_BUILD_SHARED)
          MESSAGE(STATUS "Adding custom command for CURL SHARED library: ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}CURL${CMAKE_SHARED_LIBRARY_SUFFIX}")
        ADD_CUSTOM_COMMAND(OUTPUT "${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}CURL${CMAKE_SHARED_LIBRARY_SUFFIX}"
                      COMMAND ${CMAKE_COMMAND}
                      ARGS 
                      -DMY_BUILD_PATH:STRING=${CMAKE_CFG_INTDIR} 
                      -P ${CURL_BINARY_DIR}/build.cmake 
                      MAIN_DEPENDENCY ${CURL_BINARY_DIR}/build.cmake
                      COMMENT "Custom command used to build CURL shared library")
        ADD_CUSTOM_TARGET(BUILD_CURL_LIBRARY DEPENDS "${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}CURL${CMAKE_SHARED_LIBRARY_SUFFIX}" )
      ELSE (CURL_BUILD_SHARED)
        MESSAGE(STATUS "Adding custom command for CURL STATIC library: ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}CURL${CMAKE_STATIC_LIBRARY_SUFFIX}")
        ADD_CUSTOM_COMMAND(OUTPUT "${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}CURL${CMAKE_STATIC_LIBRARY_SUFFIX}"
                      COMMAND ${CMAKE_COMMAND}
                      ARGS 
                      -DMY_BUILD_PATH:STRING=${CMAKE_CFG_INTDIR} 
                      -P ${CURL_BINARY_DIR}/build.cmake 
                      MAIN_DEPENDENCY ${CURL_BINARY_DIR}/build.cmake
                      COMMENT "Custom command used to build CURL static library")
        ADD_CUSTOM_TARGET(BUILD_CURL_LIBRARY ALL DEPENDS "${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}CURL${CMAKE_STATIC_LIBRARY_SUFFIX}" )
      ENDIF (CURL_BUILD_SHARED)
      
      SET (CURL_CONFIGURED 1)
      
      # Add CURL library to MFL_TARGETS
      SET (MFL_TARGETS ${MFL_TARGETS} BUILD_CURL_LIBRARY)      
    # -------------------------
    # create the BUILD_WXWIN_LIBRARY custom target
    # (Unix version)
    # -------------------------
#    ELSE(WIN32)
#        ADD_CUSTOM_TARGET( 
#          BUILD_WXWIN_LIBRARY
#          ALL
#          ${CMAKE_COMMAND} -E chdir ${CURL_BINARY_DIR}/Sources make install
#        )
#    ENDIF(WIN32)
  ELSE (EXISTS ${CURL_BINARY_DIR}/build.cmake)
      SET (CURL_CONFIGURED 0)
      SET (CONFIGURE_ERROR 1)
  ENDIF (EXISTS ${CURL_BINARY_DIR}/build.cmake)
ENDIF (CURL_SOURCE_PATH)

MARK_AS_ADVANCED (
  CURL_BUILD_FILE
  CURL_FORCE_CONFIGURE
)

# Write the list of variables and their values that are defined by CMake when running this CMakeList file
IF (DEBUG_MESSAGES)
  #FILE(WRITE ${CURL_BINARY_DIR}/AllVariables.txt "")
  GET_CMAKE_PROPERTY(VARS VARIABLES)
  FOREACH(var ${VARS})
  	FILE(APPEND ${CURL_BINARY_DIR}/AllVariables.txt 
  							"${var} \"${${var}}\"\n")
  ENDFOREACH(var ${VARS})
ENDIF (DEBUG_MESSAGES)
