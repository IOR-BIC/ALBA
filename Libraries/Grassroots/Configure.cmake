#
# Program:   MULTIMOD APPLICATION FRAMEWORK (MAF)
# Module:    $RCSfile: Configure.cmake,v $
# Language:  CMake 1.2
# Date:      $Date: 2009-03-04 15:17:50 $
# Version:   $Revision: 1.1.2.1 $
#
# Description:
# Project file for configuring the Grassroots library as an external project.
# Return variables:
# Grassroots_CONFIGURED  true if library configured correctly



INCLUDE (${MFL_SOURCE_PATH}/modules/PackagesMacro.cmake)
INCLUDE (${MFL_SOURCE_PATH}/modules/PatchMacro.cmake)

# this is to build Grassroots inside the MAF tree
IF (EXISTS "${Grassroots_SOURCE_DIR}/Sources")   
  SET (Grassroots_SOURCE_PATH "${Grassroots_SOURCE_DIR}/Sources")
ENDIF (EXISTS "${Grassroots_SOURCE_DIR}/Sources")

#look if there is a package for Grassroots
IF (EXISTS "${Grassroots_SOURCE_DIR}/Packages")   
  MESSAGE(STATUS "Grassroots: Found Grassroots packages")
  # here we should unpack into "${Grassroots_BINARY_DIR}/Sources"
  SET (Grassroots_PACKAGE_PATH "${Grassroots_SOURCE_DIR}/Packages")
  SET (Grassroots_PATCH_PATH "${Grassroots_SOURCE_DIR}/Patches")
  SET (Grassroots_Extension_PACKAGE_PATH "${Grassroots_SOURCE_DIR}/Packages")
  ##The macro must run only one time
  SET (Grassroots_SOURCE_PATH "${Grassroots_BINARY_DIR}/Sources/gdcm-2.0.10")
  IF (NOT EXISTS "${Grassroots_SOURCE_PATH}")
	  FIND_AND_UNPACK_PACKAGE (gdcm-2.0.10 ${Grassroots_PACKAGE_PATH} "${Grassroots_BINARY_DIR}/Sources" ${Grassroots_SOURCE_PATH})
  ENDIF (NOT EXISTS "${Grassroots_SOURCE_PATH}")
ENDIF (EXISTS "${Grassroots_SOURCE_DIR}/Packages")

IF (Grassroots_SOURCE_PATH)
  MESSAGE(STATUS "Grassroots: Configuring Grassroots")

  # default setting for building the Grassroots library  
  SET (Grassroots_BINARY_PATH "${Grassroots_BINARY_DIR}/Build" )
  OPTION (Grassroots_FORCE_CONFIGURE "Weither to force configuration of Grassroots" FALSE)
  # check if already configured
  IF (Grassroots_FORCE_CONFIGURE OR NOT EXISTS ${Grassroots_BINARY_DIR}/build.cmake)
    # Configuring the Grassroots library    
    FILE(MAKE_DIRECTORY "${Grassroots_BINARY_PATH}")
    
    # Run configuration of Grassroots library
    MESSAGE(STATUS "Grassroots: Configuring external Grassroots project")
    EXEC_PROGRAM(${CMAKE_COMMAND} "${Grassroots_BINARY_PATH}" ARGS "${Grassroots_SOURCE_PATH}" -G"${CMAKE_GENERATOR}" -DLIBRARY_OUTPUT_PATH:PATH="${LIBRARY_OUTPUT_PATH}" -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DEXECUTABLE_OUTPUT_PATH:PATH="${EXECUTABLE_OUTPUT_PATH}" -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
     OUTPUT_VARIABLE CMAKE_OUTPUT RETURN_VALUE CMAKE_RETURN)
    
    # write configure log file
    FILE(WRITE ${Grassroots_BINARY_DIR}/CMake-Log.txt "CMake configure output ${CMAKE_OUTPUT}" )
  
    IF (CMAKE_RETURN)
      # in case of error prompt an error dialog
      MESSAGE("CMake configure returned value = " ${CMAKE_RETURN})
      MESSAGE(FATAL_ERROR "${SUBPROJECT_NAME} configure error, cmake output = " ${CMAKE_OUTPUT})     
    ELSE (CMAKE_RETURN)
      # load Grassroots build command from cmake's cache
      LOAD_CACHE(${Grassroots_BINARY_PATH} READ_WITH_PREFIX Grassroots_ MAKECOMMAND)
      LOAD_CACHE(${Grassroots_BINARY_PATH} READ_WITH_PREFIX Grassroots_ BUILD_SHARED)
      
      #Split space separated arguments into a semi-colon separated list. Necessary for correct command line generation
      SEPARATE_ARGUMENTS(Grassroots_MAKECOMMAND)
      IF("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 2.0)
      	STRING (REPLACE "\\" "\\\\"  Grassroots_MAKECOMMAND "${Grassroots_MAKECOMMAND}")
      ENDIF("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 2.0)
      MESSAGE(STATUS "Creating file ${Grassroots_BINARY_DIR}/build.cmake")
    	CONFIGURE_FILE("${Grassroots_SOURCE_DIR}/build.cmake.in" "${Grassroots_BINARY_DIR}/build.cmake" ESCAPE_QUOTES @ONLY IMMEDIATE)
    	
    ENDIF (CMAKE_RETURN)
  ENDIF (Grassroots_FORCE_CONFIGURE OR NOT EXISTS ${Grassroots_BINARY_DIR}/build.cmake)
  
  IF (EXISTS ${Grassroots_BINARY_DIR}/build.cmake)
    # custom command to build the Grassroots library
    IF (Grassroots_BUILD_SHARED)
    	MESSAGE(STATUS "Adding custom command for Grassroots SHARED library: ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}Grassroots${CMAKE_SHARED_LIBRARY_SUFFIX}")
      ADD_CUSTOM_COMMAND(OUTPUT "${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}Grassroots${CMAKE_SHARED_LIBRARY_SUFFIX}"
                     COMMAND ${CMAKE_COMMAND}
                     ARGS 
                     -DMY_BUILD_PATH:STRING=${CMAKE_CFG_INTDIR} 
                     -P ${Grassroots_BINARY_DIR}/build.cmake 
                     MAIN_DEPENDENCY ${Grassroots_BINARY_DIR}/build.cmake
                     COMMENT "Custom command used to build Grassroots library")
      ADD_CUSTOM_TARGET(BUILD_Grassroots_LIBRARY DEPENDS "${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}Grassroots${CMAKE_SHARED_LIBRARY_SUFFIX}" )
    ELSE (Grassroots_BUILD_SHARED)
      MESSAGE(STATUS "Adding custom command for Grassroots STATIC library: ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}Grassroots${CMAKE_STATIC_LIBRARY_SUFFIX}")
      ADD_CUSTOM_COMMAND(OUTPUT "${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}Grassroots${CMAKE_STATIC_LIBRARY_SUFFIX}"
                     COMMAND ${CMAKE_COMMAND}
                     ARGS 
                     -DMY_BUILD_PATH:STRING=${CMAKE_CFG_INTDIR} 
                     -P ${Grassroots_BINARY_DIR}/build.cmake 
                     MAIN_DEPENDENCY ${Grassroots_BINARY_DIR}/build.cmake
                     COMMENT "Custom command used to build Grassroots library")
      ADD_CUSTOM_TARGET(BUILD_Grassroots_LIBRARY ALL DEPENDS "${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}Grassroots${CMAKE_STATIC_LIBRARY_SUFFIX}" )
    ENDIF (Grassroots_BUILD_SHARED)
    
    SET (Grassroots_CONFIGURED 1)
    
    # Add Grassroots library to MFL_TARGETS
    SET (MFL_TARGETS ${MFL_TARGETS} BUILD_Grassroots_LIBRARY)      
    
  ELSE (EXISTS ${Grassroots_BINARY_DIR}/build.cmake)
      SET (Grassroots_CONFIGURED 0)
      SET (CONFIGURE_ERROR 1)
  ENDIF (EXISTS ${Grassroots_BINARY_DIR}/build.cmake)
ENDIF (Grassroots_SOURCE_PATH)

MARK_AS_ADVANCED (
  Grassroots_BUILD_FILE
  Grassroots_FORCE_CONFIGURE
)

# Write the list of variables and their values that are defined by CMake when running this CMakeList file
IF (DEBUG_MESSAGES)
  #FILE(WRITE ${Grassroots_BINARY_DIR}/AllVariables.txt "")
  GET_CMAKE_PROPERTY(VARS VARIABLES)
  FOREACH(var ${VARS})
  	FILE(APPEND ${Grassroots_BINARY_DIR}/AllVariables.txt 
  							"${var} \"${${var}}\"\n")
  ENDFOREACH(var ${VARS})
ENDIF (DEBUG_MESSAGES)
