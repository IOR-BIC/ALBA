#
# Program:   ALBA (Agile Library for Biomedical Applications)
# Module:    $RCSfile: Configure.cmake,v $
# Language:  CMake 1.2
# Date:      $Date: 2011-05-18 07:20:15 $
# Version:   $Revision: 1.10.10.1 $
#
# Description:
# Project file for configuring the XercesC library as an external project.
# Return variables:
# XercesC_CONFIGURED  true if library configured correctly



INCLUDE (${MFL_SOURCE_DIR}/modules/PackagesMacro.cmake)
INCLUDE (${MFL_SOURCE_DIR}/modules/PatchMacro.cmake)

# this is to build XercesC inside the ALBA tree
IF (EXISTS "${XercesC_SOURCE_DIR}/Sources")   
  SET (XercesC_SOURCE_PATH "${XercesC_SOURCE_DIR}/Sources")
ENDIF (EXISTS "${XercesC_SOURCE_DIR}/Sources")

#look if there is a package for XercesC
IF (EXISTS "${XercesC_SOURCE_DIR}/Packages")   
  MESSAGE(STATUS "XercesC: Found XercesC packages")
  # here we should unpack into "${XercesC_BINARY_DIR}/Sources"
  SET (XercesC_PACKAGE_PATH "${XercesC_SOURCE_DIR}/Packages")
  ##The macro must run only one time
  SET (XercesC_SOURCE_PATH "${XercesC_BINARY_DIR}/Sources/XercesC")
  SET (XercesC_PATCH_PATH "${XercesC_SOURCE_DIR}/Patches")
  IF (NOT EXISTS "${XercesC_SOURCE_PATH}")
  	FIND_AND_UNPACK_PACKAGE (XercesC ${XercesC_PACKAGE_PATH} "${XercesC_BINARY_DIR}/Sources" ${XercesC_SOURCE_PATH})
  	FIND_AND_APPLAY_PATCHES (XercesC ${XercesC_PATCH_PATH} "${XercesC_SOURCE_PATH}")
  ENDIF (NOT EXISTS "${XercesC_SOURCE_PATH}")
ENDIF (EXISTS "${XercesC_SOURCE_DIR}/Packages")



##ci vorra' un controllo se tutto e' stato eseguito???


IF (XercesC_SOURCE_PATH)
  MESSAGE(STATUS "XercesC: Configuring XercesC")

  # default setting for building the XercesC library  
  SET (XercesC_BINARY_PATH "${XercesC_BINARY_DIR}/Build" )
  OPTION (XercesC_FORCE_CONFIGURE "Weither to force configuration of XercesC" FALSE)
  # check if already configured
  IF (XercesC_FORCE_CONFIGURE OR NOT EXISTS ${XercesC_BINARY_DIR}/build.cmake)
    # Configuring the XercesC library    
    FILE(MAKE_DIRECTORY "${XercesC_BINARY_PATH}")
    
    # Run configuration of XercesC library
    MESSAGE(STATUS "XercesC: Configuring external XercesC project")
    EXEC_PROGRAM(${CMAKE_COMMAND} "${XercesC_BINARY_PATH}" ARGS "${XercesC_SOURCE_PATH}" -G"${CMAKE_GENERATOR}" -DLIBRARY_OUTPUT_PATH:PATH="${LIBRARY_OUTPUT_PATH}" -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DEXECUTABLE_OUTPUT_PATH:PATH="${EXECUTABLE_OUTPUT_PATH}" -DBUILD_SHARED_LIBS:BOOL=${ALBA_BUILD_ALBADLL}
     OUTPUT_VARIABLE CMAKE_OUTPUT RETURN_VALUE CMAKE_RETURN)
    
    # write configure log file
    FILE(WRITE ${XercesC_BINARY_DIR}/CMake-Log.txt "CMake configure output ${CMAKE_OUTPUT}" )
  
    IF (CMAKE_RETURN)
      # in case of error prompt an error dialog
      MESSAGE("CMake configure returned value = " ${CMAKE_RETURN})
      MESSAGE(FATAL_ERROR "${SUBPROJECT_NAME} configure error, cmake output = " ${CMAKE_OUTPUT})     
    ELSE (CMAKE_RETURN)
      # load XercesC build command from cmake's cache
      LOAD_CACHE(${XercesC_BINARY_PATH} READ_WITH_PREFIX XercesC_ MAKECOMMAND)
      
      #Split space separated arguments into a semi-colon separated list. Necessary for correct command line generation
      SEPARATE_ARGUMENTS(XercesC_MAKECOMMAND)
      IF("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 2.0)
      	STRING (REPLACE "\\" "\\\\"  XercesC_MAKECOMMAND "${XercesC_MAKECOMMAND}")
      ENDIF("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 2.0)
      MESSAGE(STATUS "Creating file ${XercesC_BINARY_DIR}/build.cmake")
    	CONFIGURE_FILE("${XercesC_SOURCE_DIR}/build.cmake.in" "${XercesC_BINARY_DIR}/build.cmake" ESCAPE_QUOTES @ONLY IMMEDIATE)
    	
    ENDIF (CMAKE_RETURN)
  ENDIF (XercesC_FORCE_CONFIGURE OR NOT EXISTS ${XercesC_BINARY_DIR}/build.cmake)
  
  IF (EXISTS ${XercesC_BINARY_DIR}/build.cmake)
    # custom command to build the XercesC library
    LOAD_CACHE(${XercesC_BINARY_PATH} READ_WITH_PREFIX XercesC_ BUILD_SHARED_LIBS)
    IF (XercesC_BUILD_SHARED_LIBS)
    	MESSAGE(STATUS "Adding custom command for XercesC SHARED library: ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}XercesC${CMAKE_SHARED_LIBRARY_SUFFIX}")
      ADD_CUSTOM_COMMAND(OUTPUT "${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}XercesC${CMAKE_SHARED_LIBRARY_SUFFIX}"
                     COMMAND ${CMAKE_COMMAND}
                     ARGS 
                     -DMY_BUILD_PATH:STRING=${CMAKE_CFG_INTDIR} 
                     -P ${XercesC_BINARY_DIR}/build.cmake 
                     MAIN_DEPENDENCY ${XercesC_BINARY_DIR}/build.cmake
                     COMMENT "Custom command used to build XercesC library")
      ADD_CUSTOM_TARGET(BUILD_XercesC_LIBRARY DEPENDS "${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}XercesC${CMAKE_SHARED_LIBRARY_SUFFIX}" )
    ELSE (XercesC_BUILD_SHARED_LIBS)
      MESSAGE(STATUS "Adding custom command for XercesC STATIC library: ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}XercesC${CMAKE_STATIC_LIBRARY_SUFFIX}")
      ADD_CUSTOM_COMMAND(OUTPUT "${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}XercesC${CMAKE_STATIC_LIBRARY_SUFFIX}"
                     COMMAND ${CMAKE_COMMAND}
                     ARGS 
                     -DMY_BUILD_PATH:STRING=${CMAKE_CFG_INTDIR} 
                     -P ${XercesC_BINARY_DIR}/build.cmake 
                     MAIN_DEPENDENCY ${XercesC_BINARY_DIR}/build.cmake
                     COMMENT "Custom command used to build XercesC library")
      ADD_CUSTOM_TARGET(BUILD_XercesC_LIBRARY ALL DEPENDS "${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}XercesC${CMAKE_STATIC_LIBRARY_SUFFIX}" )
    ENDIF (XercesC_BUILD_SHARED_LIBS)
    
    SET (XercesC_CONFIGURED 1)
    
    # Add XercesC library to MFL_TARGETS
    SET (MFL_TARGETS ${MFL_TARGETS} BUILD_XercesC_LIBRARY)      
    
  ELSE (EXISTS ${XercesC_BINARY_DIR}/build.cmake)
      SET (XercesC_CONFIGURED 0)
      SET (CONFIGURE_ERROR 1)
  ENDIF (EXISTS ${XercesC_BINARY_DIR}/build.cmake)
ENDIF (XercesC_SOURCE_PATH)

MARK_AS_ADVANCED (
  XercesC_BUILD_FILE
  XercesC_FORCE_CONFIGURE
)

# Write the list of variables and their values that are defined by CMake when running this CMakeList file
IF (DEBUG_MESSAGES)
  #FILE(WRITE ${XercesC_BINARY_DIR}/AllVariables.txt "")
  GET_CMAKE_PROPERTY(VARS VARIABLES)
  FOREACH(var ${VARS})
  	FILE(APPEND ${XercesC_BINARY_DIR}/AllVariables.txt 
  							"${var} \"${${var}}\"\n")
  ENDFOREACH(var ${VARS})
ENDIF (DEBUG_MESSAGES)