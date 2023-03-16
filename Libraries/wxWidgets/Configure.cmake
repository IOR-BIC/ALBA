#
# Program:   ALBA (Agile Library for Biomedical Applications)
# Module:    $RCSfile: Configure.cmake,v $
# Language:  CMake 1.2
# Date:      $Date: 2023-03-21 12:31:39 $
# Version:   $Revision: 1.2.2.1 $
#
# Description:
# Project file for configuring the WXWIN library as an external project.
# Return variables:
# WXWIN_CONFIGURED  true if library configured correctly


INCLUDE (${MFL_SOURCE_DIR}/modules/PackagesMacro.cmake)
INCLUDE (${MFL_SOURCE_DIR}/modules/PatchMacro.cmake)

SET (WXWIN_SOURCE_DIR ${PROJECT_SOURCE_DIR}/Libraries/wxWidgets)
SET (WXWIN_BINARY_DIR ${PROJECT_BINARY_DIR}/Libraries/wxWidgets)

SET (LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/Libraries/wxWidgets/bin/ CACHE INTERNAL
     "Single output directory for building all libraries.")
SET (EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/Libraries/wxWidgets/bin/ CACHE INTERNAL 
     "Single output directory for building all executables.")

# This is to build WXWIN inside the ALBA tree
IF (EXISTS "${WXWIN_SOURCE_DIR}/Sources")   
  SET (WXWIN_SOURCE_PATH "${WXWIN_SOURCE_DIR}/Sources")
ENDIF (EXISTS "${WXWIN_SOURCE_DIR}/Sources")

# Look if there is a package for WXWIN
IF (EXISTS "${WXWIN_SOURCE_DIR}/Packages")   
  MESSAGE(STATUS "WXWIN: Found wxWidgets Packages")

  SET (WXWIN_PACKAGE_PATH "${WXWIN_SOURCE_DIR}/Packages")
  SET (WXWIN_UNPACK_PATH "${WXWIN_BINARY_DIR}/Sources")
  #SET (WXWIN_PATCH_PATH "${WXWIN_SOURCE_DIR}/Patches")
  SET (WXWIN_SOURCE_PATH "${WXWIN_UNPACK_PATH}")

  IF (NOT EXISTS "${WXWIN_SOURCE_PATH}")  
	file(MAKE_DIRECTORY ${WXWIN_UNPACK_PATH})

	MESSAGE(STATUS "WXWIN: Unpacking wxWidgets File")	
	EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E tar xzf ${WXWIN_PACKAGE_PATH}/wxWidgets-3.1.7.tar.gz WORKING_DIRECTORY ${WXWIN_UNPACK_PATH})
	
  ENDIF (NOT EXISTS "${WXWIN_SOURCE_PATH}")
ENDIF (EXISTS "${WXWIN_SOURCE_DIR}/Packages")

IF (WXWIN_SOURCE_PATH)
  MESSAGE(STATUS "WXWIN: Configuring WXWIN")

  # default setting for building the WXWIN library  
  SET (WXWIN_BINARY_PATH "${WXWIN_BINARY_DIR}/Build" )
  OPTION (WXWIN_FORCE_CONFIGURE "Weither to force configuration of WXWIN" FALSE)
  # check if already configured
  IF (WXWIN_FORCE_CONFIGURE OR NOT EXISTS ${WXWIN_BINARY_DIR}/build.cmake)
    # Configuring the WXWIN library    
    FILE(MAKE_DIRECTORY "${WXWIN_BINARY_PATH}")
    
    # Run configuration of WXWIN library
    MESSAGE(STATUS "WXWIN: Configuring external WXWIN project")
    EXEC_PROGRAM(${CMAKE_COMMAND} "${WXWIN_BINARY_PATH}" ARGS "${WXWIN_SOURCE_PATH}" -G"${CMAKE_GENERATOR}" -DLIBRARY_OUTPUT_PATH:PATH="${LIBRARY_OUTPUT_PATH}" -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DEXECUTABLE_OUTPUT_PATH:PATH="${EXECUTABLE_OUTPUT_PATH}" -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
     OUTPUT_VARIABLE CMAKE_OUTPUT RETURN_VALUE CMAKE_RETURN)
    
    # write configure log file
    FILE(WRITE ${WXWIN_BINARY_DIR}/CMake-Log.txt "CMake configure output ${CMAKE_OUTPUT}" )
  
    IF (CMAKE_RETURN)
      # in case of error prompt an error dialog
      MESSAGE("CMake configure returned value = " ${CMAKE_RETURN})
      MESSAGE(FATAL_ERROR "${SUBPROJECT_NAME} configure error, cmake output = " ${CMAKE_OUTPUT})     
    ELSE (CMAKE_RETURN)
      # load WXWIN build command from cmake's cache
      LOAD_CACHE(${WXWIN_BINARY_PATH} READ_WITH_PREFIX WXWIN_ MAKECOMMAND)
      LOAD_CACHE(${WXWIN_BINARY_PATH} READ_WITH_PREFIX WXWIN_ BUILD_SHARED)
      
      #Split space separated arguments into a semi-colon separated list. Necessary for correct command line generation
      SEPARATE_ARGUMENTS(WXWIN_MAKECOMMAND)
      IF("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 2.0)
      	STRING (REPLACE "\\" "\\\\"  WXWIN_MAKECOMMAND "${WXWIN_MAKECOMMAND}")
      ENDIF("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 2.0)
      MESSAGE(STATUS "Creating file ${WXWIN_BINARY_DIR}/build.cmake")
    	CONFIGURE_FILE("${WXWIN_SOURCE_DIR}/build.cmake.in" "${WXWIN_BINARY_DIR}/build.cmake" ESCAPE_QUOTES @ONLY IMMEDIATE)
    	
    ENDIF (CMAKE_RETURN)
  ENDIF (WXWIN_FORCE_CONFIGURE OR NOT EXISTS ${WXWIN_BINARY_DIR}/build.cmake)
  
  IF (EXISTS ${WXWIN_BINARY_DIR}/build.cmake)
    # custom command to build the WXWIN library
    IF (WXWIN_BUILD_SHARED)
    	MESSAGE(STATUS "Adding custom command for WXWIN SHARED library: ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}WXWIN${CMAKE_SHARED_LIBRARY_SUFFIX}")
      ADD_CUSTOM_COMMAND(OUTPUT "${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}WXWIN${CMAKE_SHARED_LIBRARY_SUFFIX}"
                     COMMAND ${CMAKE_COMMAND}
                     ARGS 
                     -DMY_BUILD_PATH:STRING=${CMAKE_CFG_INTDIR} 
                     -P ${WXWIN_BINARY_DIR}/build.cmake 
                     MAIN_DEPENDENCY ${WXWIN_BINARY_DIR}/build.cmake
                     COMMENT "Custom command used to build WXWIN library")
      ADD_CUSTOM_TARGET(BUILD_WXWIN_LIBRARY DEPENDS "${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}WXWIN${CMAKE_SHARED_LIBRARY_SUFFIX}" )
    ELSE (WXWIN_BUILD_SHARED)
      MESSAGE(STATUS "Adding custom command for WXWIN STATIC library: ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}WXWIN${CMAKE_STATIC_LIBRARY_SUFFIX}")
      ADD_CUSTOM_COMMAND(OUTPUT "${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}WXWIN${CMAKE_STATIC_LIBRARY_SUFFIX}"
                     COMMAND ${CMAKE_COMMAND}
                     ARGS 
                     -DMY_BUILD_PATH:STRING=${CMAKE_CFG_INTDIR} 
                     -P ${WXWIN_BINARY_DIR}/build.cmake 
                     MAIN_DEPENDENCY ${WXWIN_BINARY_DIR}/build.cmake
                     COMMENT "Custom command used to build WXWIN library")
      ADD_CUSTOM_TARGET(BUILD_WXWIN_LIBRARY ALL DEPENDS "${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}WXWIN${CMAKE_STATIC_LIBRARY_SUFFIX}" )
    ENDIF (WXWIN_BUILD_SHARED)
    
    SET (WXWIN_CONFIGURED 1)
    
    # Add WXWIN library to MFL_TARGETS
    SET (MFL_TARGETS ${MFL_TARGETS} BUILD_WXWIN_LIBRARY)      
    
  ELSE (EXISTS ${WXWIN_BINARY_DIR}/build.cmake)
      SET (WXWIN_CONFIGURED 0)
      SET (CONFIGURE_ERROR 1)
  ENDIF (EXISTS ${WXWIN_BINARY_DIR}/build.cmake)
ENDIF (WXWIN_SOURCE_PATH)

MARK_AS_ADVANCED (
  WXWIN_BUILD_FILE
  WXWIN_FORCE_CONFIGURE
)

# Write the list of variables and their values that are defined by CMake when running this CMakeList file
IF (DEBUG_MESSAGES)
  #FILE(WRITE ${WXWIN_BINARY_DIR}/AllVariables.txt "")
  GET_CMAKE_PROPERTY(VARS VARIABLES)
  FOREACH(var ${VARS})
  	FILE(APPEND ${WXWIN_BINARY_DIR}/AllVariables.txt 
  							"${var} \"${${var}}\"\n")
  ENDFOREACH(var ${VARS})
ENDIF (DEBUG_MESSAGES)

