#
# Program:   ALBA (Agile Library for Biomedical Applications)
# Module:    $RCSfile: Configure.cmake,v $
# Language:  CMake 1.2
# Date:      $Date: 2011-05-25 09:59:40 $
# Version:   $Revision: 1.4.2.1 $
#
# Description:
# Project file for configuring the WXWIN library as an external project.
# Return variables:
# WXWIN_CONFIGURED  true if library configured correctly


# SIL NOTE:
#
# 1) - unpack wxWidgets.tgz, wxCmake.tgz and apply the patch
# 2) - configure (syncronously) the wxWindow project, and generate the Build.cmake 
# 3) - create the custom target BUILD_WX_LIBRARY
#
# 
# the Custom Target BUILD_WX_LIBRARY does nothing but depends on ...  
# ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}${WX_LIB_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}
# (which evaluate to wxwin.lib complete with path) 
#
# if wxWin is not up-to-date it will run a 'custom command'
# which in turn will execute the Build.cmake using CMake as shell
#
# Build.cmake -(configured from Build.cmake.in)
# will basically call:
# EXEC_PROGRAM( @WXWIN_MAKECOMMAND@  @WXWIN_BINARY_PATH@ )
#
# and WXWIN_MAKECOMMAND was defined in (and loaded from)
# the CMakeCache.cmake resulted from the configutation of the wxWindow-project


INCLUDE (${MFL_SOURCE_DIR}/modules/PackagesMacro.cmake)
INCLUDE (${MFL_SOURCE_DIR}/modules/PatchMacro.cmake)

# this option is to build WXWIN inside the ALBA tree
IF (EXISTS "${WXWIN_SOURCE_DIR}/Sources")   
  SET (WXWIN_SOURCE_PATH "${WXWIN_SOURCE_DIR}/Sources")
ENDIF (EXISTS "${WXWIN_SOURCE_DIR}/Sources")

#look if there is a package for WXWIN
IF (EXISTS "${WXWIN_SOURCE_DIR}/Packages")   
  MESSAGE(STATUS "WXWIN: Found WXWIN packages")
  # here we should unpack into "${WXWIN_BINARY_DIR}/Sources"
  SET (WXWIN_PACKAGE_PATH "${WXWIN_SOURCE_DIR}/Packages")
  SET (WXWIN_UNPACK_PATH "${WXWIN_BINARY_DIR}/Sources")
  SET (WXWIN_PATCH_PATH "${WXWIN_SOURCE_DIR}/Patches")
  SET (WXWIN_SOURCE_PATH "${WXWIN_UNPACK_PATH}") # where sources are 
  
  ##The macro must run only one time
  IF (NOT EXISTS "${WXWIN_SOURCE_PATH}")
  	FIND_AND_UNPACK_PACKAGE (wxWidgets-2.6.3.tar      ${WXWIN_PACKAGE_PATH} "${WXWIN_UNPACK_PATH}")
  	FIND_AND_UNPACK_PACKAGE (wxWidgets-2.6.3-Patch-2  ${WXWIN_PACKAGE_PATH} "${WXWIN_UNPACK_PATH}")
  	FIND_AND_UNPACK_PACKAGE (wxCMake                  ${WXWIN_PACKAGE_PATH} "${WXWIN_UNPACK_PATH}")
  ENDIF (NOT EXISTS "${WXWIN_SOURCE_PATH}")
ENDIF (EXISTS "${WXWIN_SOURCE_DIR}/Packages")

FIND_AND_APPLAY_PATCHES (wxWin ${WXWIN_PATCH_PATH} "${WXWIN_SOURCE_PATH}")

##ci vorra' un controllo se tutto e' stato eseguito???

IF (WXWIN_SOURCE_PATH)
  MESSAGE(STATUS "WXWIN: Configuring WXWIN")

  # default setting for building the WXWIN library
#2  IF (WIN32)
#2    SET(WX_LIB_NAME wxmsw CACHE INTERNAL "name of wx windows library")
#2  ELSE (WIN32)
#2    SET(WX_LIB_NAME wxgtk CACHE INTERNAL "name of wx windows library")
#2  ENDIF (WIN32)
  
  SET (WXWIN_BINARY_PATH "${WXWIN_BINARY_DIR}/Build" )
  OPTION (WXWIN_FORCE_CONFIGURE "Weither to force configuration of WXWIN" FALSE)

  # check if already configured
  IF (WXWIN_FORCE_CONFIGURE OR NOT EXISTS ${WXWIN_BINARY_DIR}/wxUse.cmake)
    
    # Configuring the WXWIN library    
    FILE(MAKE_DIRECTORY "${WXWIN_BINARY_PATH}")
    
    # Run configuration of WXWIN library
    MESSAGE(STATUS "WXWIN: Configuring external WXWIN project")
    EXEC_PROGRAM(${CMAKE_COMMAND} "${WXWIN_BINARY_PATH}" ARGS "${WXWIN_SOURCE_PATH}" 
      -G"${CMAKE_GENERATOR}" 
      -DWX_USE_SOCKET:BOOL=TRUE 
      -DLIBRARY_OUTPUT_PATH:PATH="${LIBRARY_OUTPUT_PATH}" 
      -DEXECUTABLE_OUTPUT_PATH:PATH="${EXECUTABLE_OUTPUT_PATH}" 
      -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
      -DBUILD_SHARED_LIBS:BOOL=${ALBA_BUILD_ALBADLL}
      -DWX_USE_HTML:BOOL=TRUE
      OUTPUT_VARIABLE CMAKE_OUTPUT 
      RETURN_VALUE CMAKE_RETURN)
    
    # write configure log file
    FILE(WRITE ${WXWIN_BINARY_DIR}/CMake-Log.txt "CMake configure output ${CMAKE_OUTPUT}" )
  
    IF (CMAKE_RETURN)
      # in case of error prompt an error dialog
      MESSAGE("CMake configure returned value = " ${CMAKE_RETURN})
      MESSAGE(FATAL_ERROR "${SUBPROJECT_NAME} configure error, cmake output = " ${CMAKE_OUTPUT})     
    ELSE (CMAKE_RETURN)
      
      # load WXWIN build command from cmake's cache
      LOAD_CACHE(${WXWIN_BINARY_PATH} READ_WITH_PREFIX WXWIN_ MAKECOMMAND)
      
      #Split space separated arguments into a semi-colon separated list. Necessary for correct command line generation
      SEPARATE_ARGUMENTS(WXWIN_MAKECOMMAND)
      IF("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 2.0)
      	STRING (REPLACE "\\" "\\\\"  WXWIN_MAKECOMMAND "${WXWIN_MAKECOMMAND}")
      ENDIF("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 2.0)

      MESSAGE(STATUS "Creating file ${WXWIN_BINARY_DIR}/build.cmake")

      CONFIGURE_FILE("${WXWIN_SOURCE_DIR}/build.cmake.in" "${WXWIN_BINARY_DIR}/build.cmake" ESCAPE_QUOTES @ONLY IMMEDIATE)

    ENDIF (CMAKE_RETURN)

  ENDIF(WXWIN_FORCE_CONFIGURE OR NOT EXISTS ${WXWIN_BINARY_DIR}/wxUse.cmake)
  
ENDIF (WXWIN_SOURCE_PATH)

# -------------------------
# create the BUILD_WXWIN_LIBRARY custom target
# (WIN32 version)
# -------------------------
IF(WIN32)

  IF (EXISTS ${WXWIN_BINARY_DIR}/build.cmake)
    # custom command to build the WXWIN library
    #LOAD_CACHE(${WXWIN_BINARY_PATH} READ_WITH_PREFIX WXWIN_ BUILD_SHARED_LIBS)

    SET( NAME_OF_A_WX_LIB "${WXWIN_BINARY_PATH}/${CMAKE_CFG_INTDIR}/wxbase26.lib" )
    #MESSAGE( "DBG: Added custom command for WXWIN library: ${NAME_OF_A_WX_LIB}")

    ADD_CUSTOM_COMMAND(OUTPUT ${NAME_OF_A_WX_LIB}
                   COMMAND ${CMAKE_COMMAND}
                   ARGS 
                   -DMY_BUILD_PATH:STRING=${CMAKE_CFG_INTDIR} 
                   -P ${WXWIN_BINARY_DIR}/build.cmake 
                   MAIN_DEPENDENCY ${WXWIN_BINARY_DIR}/build.cmake
                   COMMENT "Custom command used to build WXWIN library")

    ADD_CUSTOM_TARGET(BUILD_WXWIN_LIBRARY ALL DEPENDS ${NAME_OF_A_WX_LIB} )

    MESSAGE(STATUS "Added custom command for WXWIN library: ${NAME_OF_A_WX_LIB}")

  ELSE (EXISTS ${WXWIN_BINARY_DIR}/build.cmake)
      SET (WXWIN_CONFIGURED 0)
      SET (CONFIGURE_ERROR 1)
  ENDIF (EXISTS ${WXWIN_BINARY_DIR}/build.cmake)

# -------------------------
# create the BUILD_WXWIN_LIBRARY custom target
# (Unix version)
# -------------------------
ELSE(WIN32)
    ADD_CUSTOM_TARGET( 
      BUILD_WXWIN_LIBRARY
      ALL
      ${CMAKE_COMMAND} -E chdir ${WXWIN_BINARY_DIR}/Build/configure_dir make 
    )
ENDIF(WIN32)

# -------------------------
# add BUILD_WXWIN_LIBRARY to MFL_TARGETS
# -------------------------
SET (MFL_TARGETS ${MFL_TARGETS}BUILD_WXWIN_LIBRARY)      

MARK_AS_ADVANCED (
  WXWIN_BUILD_FILE
  WXWIN_FORCE_CONFIGURE
)

#--------------------------------------------------------------------------------------------------------
# Write the list of variables and their values that are defined by CMake when running this CMakeList file
#--------------------------------------------------------------------------------------------------------
IF (DEBUG_MESSAGES)
  FILE(WRITE ${WXWIN_BINARY_DIR}/AllVariables.txt "")
  GET_CMAKE_PROPERTY(VARS VARIABLES)
  FOREACH(var ${VARS})
  	FILE(APPEND ${WXWIN_BINARY_DIR}/AllVariables.txt 
  							"${var} \"${${var}}\"\n")
  ENDFOREACH(var ${VARS})
ENDIF (DEBUG_MESSAGES)
