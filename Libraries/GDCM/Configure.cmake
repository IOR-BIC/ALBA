#
# Program:   MULTIMOD APPLICATION FRAMEWORK (MAF)
# Module:    $RCSfile: Configure.cmake,v $
# Language:  CMake 1.2
# Date:      $Date: 2009-03-25 08:22:09 $
# Version:   $Revision: 1.1.2.2 $
#
# Description:
# Project file for configuring the GDCM library as an external project.
# Return variables:
# GDCM_CONFIGURED  true if library configured correctly



INCLUDE (${MFL_SOURCE_PATH}/modules/PackagesMacro.cmake)
INCLUDE (${MFL_SOURCE_PATH}/modules/PatchMacro.cmake)

# this is to build GDCM inside the MAF tree
IF (EXISTS "${GDCM_SOURCE_DIR}/Sources")   
  SET (GDCM_SOURCE_PATH "${GDCM_SOURCE_DIR}/Sources")
ENDIF (EXISTS "${GDCM_SOURCE_DIR}/Sources")


#look if there is a package for GDCM
IF (EXISTS "${GDCM_SOURCE_DIR}/Packages")   
  MESSAGE(STATUS "GDCM: Found GDCM packages")
  # here we should unpack into "${GDCM_BINARY_DIR}/Sources"
  SET (GDCM_PACKAGE_PATH "${GDCM_SOURCE_DIR}/Packages")
  SET (GDCM_PATCH_PATH "${GDCM_SOURCE_DIR}/Patches")
  SET (GDCM_Extension_PACKAGE_PATH "${GDCM_SOURCE_DIR}/Packages")
  ##The macro must run only one time
  SET (GDCM_SOURCE_PATH "${GDCM_BINARY_DIR}/Sources/gdcm-2.0.10")
  IF (NOT EXISTS "${GDCM_SOURCE_PATH}")
	  FIND_AND_UNPACK_PACKAGE (gdcm-2.0.10 ${GDCM_PACKAGE_PATH} "${GDCM_BINARY_DIR}/Sources" ${GDCM_SOURCE_PATH})
      FIND_AND_UNPACK_PACKAGE (gdcm_Extras ${GDCM_Extension_PACKAGE_PATH} "${GDCM_BINARY_DIR}/Sources" ${GDCM_SOURCE_PATH})
      FIND_AND_APPLAY_PATCHES (gdcm-2.0.10 ${GDCM_PATCH_PATH} "${GDCM_SOURCE_PATH}")
  ENDIF (NOT EXISTS "${GDCM_SOURCE_PATH}")
ENDIF (EXISTS "${GDCM_SOURCE_DIR}/Packages")


IF (GDCM_SOURCE_PATH)
  MESSAGE(STATUS "GDCM: Configuring GDCM")

  # default setting for building the GDCM library  
  SET (GDCM_BINARY_PATH "${GDCM_BINARY_DIR}/Build" )
  OPTION (GDCM_FORCE_CONFIGURE "Weither to force configuration of GDCM" TRUE)
  
  # check if already configured
  IF (GDCM_FORCE_CONFIGURE OR NOT EXISTS ${GDCM_BINARY_DIR}/build.cmake)
    # Configuring the GDCM library    
    FILE(MAKE_DIRECTORY "${GDCM_BINARY_PATH}")
    
    # Run configuration of GDCM library
    MESSAGE(STATUS "GDCM: Configuring external GDCM project")
    EXEC_PROGRAM(${CMAKE_COMMAND} "${GDCM_BINARY_PATH}" ARGS "${GDCM_SOURCE_PATH}" -G"${CMAKE_GENERATOR}" -DLIBRARY_OUTPUT_PATH:PATH="${LIBRARY_OUTPUT_PATH}" -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DEXECUTABLE_OUTPUT_PATH:PATH="${EXECUTABLE_OUTPUT_PATH}" -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
     OUTPUT_VARIABLE CMAKE_OUTPUT RETURN_VALUE CMAKE_RETURN)
    
    # write configure log file
    FILE(WRITE ${GDCM_BINARY_DIR}/CMake-Log.txt "CMake configure output ${CMAKE_OUTPUT}" )
  
    IF (CMAKE_RETURN)
      # in case of error prompt an error dialog
      MESSAGE("CMake configure returned value = " ${CMAKE_RETURN})
      MESSAGE(FATAL_ERROR "${SUBPROJECT_NAME} configure error, cmake output = " ${CMAKE_OUTPUT})     
    ELSE (CMAKE_RETURN)
      # load GDCM build command from cmake's cache
      LOAD_CACHE(${GDCM_BINARY_PATH} READ_WITH_PREFIX GDCM_ MAKECOMMAND)
      LOAD_CACHE(${GDCM_BINARY_PATH} READ_WITH_PREFIX GDCM_ BUILD_SHARED)
      
      #Split space separated arguments into a semi-colon separated list. Necessary for correct command line generation
      SEPARATE_ARGUMENTS(GDCM_MAKECOMMAND)
      IF("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 2.0)
      	STRING (REPLACE "\\" "\\\\"  GDCM_MAKECOMMAND "${GDCM_MAKECOMMAND}")
      ENDIF("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 2.0)
      MESSAGE(STATUS "Creating file ${GDCM_BINARY_DIR}/build.cmake")
    	CONFIGURE_FILE("${GDCM_SOURCE_DIR}/build.cmake.in" "${GDCM_BINARY_DIR}/build.cmake" ESCAPE_QUOTES @ONLY IMMEDIATE)
    	
    ENDIF (CMAKE_RETURN)
  ENDIF (GDCM_FORCE_CONFIGURE OR NOT EXISTS ${GDCM_BINARY_DIR}/build.cmake)
  
  IF (EXISTS ${GDCM_BINARY_DIR}/build.cmake)
    # custom command to build the GDCM library
    IF (GDCM_BUILD_SHARED)
    	MESSAGE(STATUS "Adding custom command for GDCM SHARED library: ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}GDCM${CMAKE_SHARED_LIBRARY_SUFFIX}")
      ADD_CUSTOM_COMMAND(OUTPUT "${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}GDCM${CMAKE_SHARED_LIBRARY_SUFFIX}"
                     COMMAND ${CMAKE_COMMAND}
                     ARGS 
                     -DMY_BUILD_PATH:STRING=${CMAKE_CFG_INTDIR} 
                     -P ${GDCM_BINARY_DIR}/build.cmake 
                     MAIN_DEPENDENCY ${GDCM_BINARY_DIR}/build.cmake
                     COMMENT "Custom command used to build GDCM library")
      ADD_CUSTOM_TARGET(BUILD_GDCM_LIBRARY DEPENDS "${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}GDCM${CMAKE_SHARED_LIBRARY_SUFFIX}" )
    ELSE (GDCM_BUILD_SHARED)
      MESSAGE(STATUS "Adding custom command for GDCM STATIC library: ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}GDCM${CMAKE_STATIC_LIBRARY_SUFFIX}")
      ADD_CUSTOM_COMMAND(OUTPUT "${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}GDCM${CMAKE_STATIC_LIBRARY_SUFFIX}"
                     COMMAND ${CMAKE_COMMAND}
                     ARGS 
                     -DMY_BUILD_PATH:STRING=${CMAKE_CFG_INTDIR} 
                     -P ${GDCM_BINARY_DIR}/build.cmake 
                     MAIN_DEPENDENCY ${GDCM_BINARY_DIR}/build.cmake
                     COMMENT "Custom command used to build GDCM library")
      ADD_CUSTOM_TARGET(BUILD_GDCM_LIBRARY ALL DEPENDS "${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}GDCM${CMAKE_STATIC_LIBRARY_SUFFIX}" )
    ENDIF (GDCM_BUILD_SHARED)
    
    SET (GDCM_CONFIGURED 1)
    
    # Add GDCM library to MFL_TARGETS
    SET (MFL_TARGETS ${MFL_TARGETS} BUILD_GDCM_LIBRARY)      
    
  ELSE (EXISTS ${GDCM_BINARY_DIR}/build.cmake)
      SET (GDCM_CONFIGURED 0)
      SET (CONFIGURE_ERROR 1)
  ENDIF (EXISTS ${GDCM_BINARY_DIR}/build.cmake)
ENDIF (GDCM_SOURCE_PATH)

MARK_AS_ADVANCED (
  GDCM_BUILD_FILE
  GDCM_FORCE_CONFIGURE
)

# Write the list of variables and their values that are defined by CMake when running this CMakeList file
IF (DEBUG_MESSAGES)
  #FILE(WRITE ${GDCM_BINARY_DIR}/AllVariables.txt "")
  GET_CMAKE_PROPERTY(VARS VARIABLES)
  FOREACH(var ${VARS})
  	FILE(APPEND ${GDCM_BINARY_DIR}/AllVariables.txt 
  							"${var} \"${${var}}\"\n")
  ENDFOREACH(var ${VARS})
ENDIF (DEBUG_MESSAGES)
