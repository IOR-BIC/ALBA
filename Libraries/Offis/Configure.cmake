#
# Program:   MULTIMOD APPLICATION FRAMEWORK (MAF)
# Module:    $RCSfile: Configure.cmake,v $
# Language:  CMake 1.2
# Date:      $Date: 2009-04-21 12:31:39 $
# Version:   $Revision: 1.2.2.1 $
#
# Description:
# Project file for configuring the Offis library as an external project.
# Return variables:
# Offis_CONFIGURED  true if library configured correctly



INCLUDE (${MFL_SOURCE_PATH}/modules/PackagesMacro.cmake)
INCLUDE (${MFL_SOURCE_PATH}/modules/PatchMacro.cmake)

# this is to build Offis inside the MAF tree
IF (EXISTS "${Offis_SOURCE_DIR}/Sources")   
  SET (Offis_SOURCE_PATH "${Offis_SOURCE_DIR}/Sources")
ENDIF (EXISTS "${Offis_SOURCE_DIR}/Sources")

#look if there is a package for Offis
IF (EXISTS "${Offis_SOURCE_DIR}/Packages")   
  MESSAGE(STATUS "Offis: Found Offis packages")
  # here we should unpack into "${Offis_BINARY_DIR}/Sources"
  SET (Offis_PACKAGE_PATH "${Offis_SOURCE_DIR}/Packages")
  SET (Offis_PATCH_PATH "${Offis_SOURCE_DIR}/Patches")
  SET (Offis_Extension_PACKAGE_PATH "${Offis_SOURCE_DIR}/Packages")
  ##The macro must run only one time
  SET (Offis_SOURCE_PATH "${Offis_BINARY_DIR}/Sources/dcmtk-3.5.4")
  IF (NOT EXISTS "${Offis_SOURCE_PATH}")
	  FIND_AND_UNPACK_PACKAGE (dcmtk-3.5.4 ${Offis_PACKAGE_PATH} "${Offis_BINARY_DIR}/Sources" ${Offis_SOURCE_PATH})
	  FIND_AND_UNPACK_PACKAGE (dcmtk_Extras ${Offis_Extension_PACKAGE_PATH} "${Offis_BINARY_DIR}/Sources" ${Offis_SOURCE_PATH})
	  FIND_AND_APPLAY_PATCHES(dcmtk-3.5.4 ${Offis_PATCH_PATH} "${Offis_SOURCE_PATH}")
  ENDIF (NOT EXISTS "${Offis_SOURCE_PATH}")
ENDIF (EXISTS "${Offis_SOURCE_DIR}/Packages")

IF (Offis_SOURCE_PATH)
  MESSAGE(STATUS "Offis: Configuring Offis")

  # default setting for building the Offis library  
  SET (Offis_BINARY_PATH "${Offis_BINARY_DIR}/Build" )
  OPTION (Offis_FORCE_CONFIGURE "Weither to force configuration of Offis" FALSE)
  # check if already configured
  IF (Offis_FORCE_CONFIGURE OR NOT EXISTS ${Offis_BINARY_DIR}/build.cmake)
    # Configuring the Offis library    
    FILE(MAKE_DIRECTORY "${Offis_BINARY_PATH}")
    
    # Run configuration of Offis library
    MESSAGE(STATUS "Offis: Configuring external Offis project")
    EXEC_PROGRAM(${CMAKE_COMMAND} "${Offis_BINARY_PATH}" ARGS "${Offis_SOURCE_PATH}" -G"${CMAKE_GENERATOR}" -DLIBRARY_OUTPUT_PATH:PATH="${LIBRARY_OUTPUT_PATH}" -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DEXECUTABLE_OUTPUT_PATH:PATH="${EXECUTABLE_OUTPUT_PATH}" -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
     OUTPUT_VARIABLE CMAKE_OUTPUT RETURN_VALUE CMAKE_RETURN)
    
    # write configure log file
    FILE(WRITE ${Offis_BINARY_DIR}/CMake-Log.txt "CMake configure output ${CMAKE_OUTPUT}" )
  
    IF (CMAKE_RETURN)
      # in case of error prompt an error dialog
      MESSAGE("CMake configure returned value = " ${CMAKE_RETURN})
      MESSAGE(FATAL_ERROR "${SUBPROJECT_NAME} configure error, cmake output = " ${CMAKE_OUTPUT})     
    ELSE (CMAKE_RETURN)
      # load Offis build command from cmake's cache
      LOAD_CACHE(${Offis_BINARY_PATH} READ_WITH_PREFIX Offis_ MAKECOMMAND)
      LOAD_CACHE(${Offis_BINARY_PATH} READ_WITH_PREFIX Offis_ BUILD_SHARED)
      
      #Split space separated arguments into a semi-colon separated list. Necessary for correct command line generation
      SEPARATE_ARGUMENTS(Offis_MAKECOMMAND)
      IF("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 2.0)
      	STRING (REPLACE "\\" "\\\\"  Offis_MAKECOMMAND "${Offis_MAKECOMMAND}")
      ENDIF("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 2.0)
      MESSAGE(STATUS "Creating file ${Offis_BINARY_DIR}/build.cmake")
    	CONFIGURE_FILE("${Offis_SOURCE_DIR}/build.cmake.in" "${Offis_BINARY_DIR}/build.cmake" ESCAPE_QUOTES @ONLY IMMEDIATE)
    	
    ENDIF (CMAKE_RETURN)
  ENDIF (Offis_FORCE_CONFIGURE OR NOT EXISTS ${Offis_BINARY_DIR}/build.cmake)
  
  IF (EXISTS ${Offis_BINARY_DIR}/build.cmake)
    # custom command to build the Offis library
    IF (Offis_BUILD_SHARED)
    	MESSAGE(STATUS "Adding custom command for Offis SHARED library: ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}Offis${CMAKE_SHARED_LIBRARY_SUFFIX}")
      ADD_CUSTOM_COMMAND(OUTPUT "${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}Offis${CMAKE_SHARED_LIBRARY_SUFFIX}"
                     COMMAND ${CMAKE_COMMAND}
                     ARGS 
                     -DMY_BUILD_PATH:STRING=${CMAKE_CFG_INTDIR} 
                     -P ${Offis_BINARY_DIR}/build.cmake 
                     MAIN_DEPENDENCY ${Offis_BINARY_DIR}/build.cmake
                     COMMENT "Custom command used to build Offis library")
      ADD_CUSTOM_TARGET(BUILD_Offis_LIBRARY DEPENDS "${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}Offis${CMAKE_SHARED_LIBRARY_SUFFIX}" )
    ELSE (Offis_BUILD_SHARED)
      MESSAGE(STATUS "Adding custom command for Offis STATIC library: ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}Offis${CMAKE_STATIC_LIBRARY_SUFFIX}")
      ADD_CUSTOM_COMMAND(OUTPUT "${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}Offis${CMAKE_STATIC_LIBRARY_SUFFIX}"
                     COMMAND ${CMAKE_COMMAND}
                     ARGS 
                     -DMY_BUILD_PATH:STRING=${CMAKE_CFG_INTDIR} 
                     -P ${Offis_BINARY_DIR}/build.cmake 
                     MAIN_DEPENDENCY ${Offis_BINARY_DIR}/build.cmake
                     COMMENT "Custom command used to build Offis library")
      ADD_CUSTOM_TARGET(BUILD_Offis_LIBRARY ALL DEPENDS "${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}Offis${CMAKE_STATIC_LIBRARY_SUFFIX}" )
    ENDIF (Offis_BUILD_SHARED)
    
    SET (Offis_CONFIGURED 1)
    
    # Add Offis library to MFL_TARGETS
    SET (MFL_TARGETS ${MFL_TARGETS} BUILD_Offis_LIBRARY)      
    
  ELSE (EXISTS ${Offis_BINARY_DIR}/build.cmake)
      SET (Offis_CONFIGURED 0)
      SET (CONFIGURE_ERROR 1)
  ENDIF (EXISTS ${Offis_BINARY_DIR}/build.cmake)
ENDIF (Offis_SOURCE_PATH)

MARK_AS_ADVANCED (
  Offis_BUILD_FILE
  Offis_FORCE_CONFIGURE
)

# Write the list of variables and their values that are defined by CMake when running this CMakeList file
IF (DEBUG_MESSAGES)
  #FILE(WRITE ${Offis_BINARY_DIR}/AllVariables.txt "")
  GET_CMAKE_PROPERTY(VARS VARIABLES)
  FOREACH(var ${VARS})
  	FILE(APPEND ${Offis_BINARY_DIR}/AllVariables.txt 
  							"${var} \"${${var}}\"\n")
  ENDFOREACH(var ${VARS})
ENDIF (DEBUG_MESSAGES)
