# Program:   ALBA (Agile Library for Biomedical Applications)
# Module:    $RCSfile: CMakeLists.txt,v $
# Language:  CMake 1.2
# Date:      $Date: 2010-12-29 18:50:18 $
# Version:   $Revision: 1.11.22.1 $
#
# Description:
# Project file of the Multimod Fundation Layer.

MESSAGE(STATUS "Configuring wxWidgets")

INCLUDE (${MFL_SOURCE_DIR}/modules/PatchMacro.cmake)

 SET(WXWIN_SOURCE_DIR ${PROJECT_SOURCE_DIR}/Libraries/wxWidgets)
 SET(WXWIN_BINARY_DIR ${PROJECT_BINARY_DIR}/Libraries/wxWidgets)
# SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/Libraries/wxWidgets/bin/ CACHE INTERNAL
#     "Single output directory for building all libraries.")
# SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/Libraries/wxWidgets/bin/ CACHE INTERNAL 
#     "Single output directory for building all executables.")
	
#This option is to build WXWIN inside the APP tree
 IF (EXISTS "${WXWIN_SOURCE_DIR}/Sources")   
  SET (WXWIN_SOURCE_PATH "${WXWIN_SOURCE_DIR}/Sources")
 ENDIF (EXISTS "${WXWIN_SOURCE_DIR}/Sources")

 # Look if there is a package for WXWIN
 IF (EXISTS "${WXWIN_SOURCE_DIR}/Packages")   
  MESSAGE(STATUS "WXWIN: Found wxWidgets Packages")

  SET (WXWIN_PACKAGE_PATH "${WXWIN_SOURCE_DIR}/Packages")
  SET (WXWIN_UNPACK_PATH "${WXWIN_BINARY_DIR}/Sources")
  SET (WXWIN_PATCH_PATH "${WXWIN_SOURCE_DIR}/Patches")
  SET (WXWIN_SOURCE_PATH "${WXWIN_UNPACK_PATH}")

  IF (NOT EXISTS "${WXWIN_SOURCE_PATH}")  
	file(MAKE_DIRECTORY ${WXWIN_UNPACK_PATH})

	MESSAGE(STATUS "WXWIN: Unpacking wxWidgets File")	
	EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E tar xzf ${WXWIN_PACKAGE_PATH}/wxWidgets-3.1.7.tar.gz WORKING_DIRECTORY ${WXWIN_UNPACK_PATH})
	
	FIND_AND_APPLAY_PATCHES (wxWin ${WXWIN_PATCH_PATH} "${WXWIN_SOURCE_PATH}")
	
  ENDIF (NOT EXISTS "${WXWIN_SOURCE_PATH}")
 ENDIF (EXISTS "${WXWIN_SOURCE_DIR}/Packages")

 # Configuring WXWIN
 IF (WXWIN_SOURCE_PATH)
  MESSAGE(STATUS "WXWIN: Configuring wxWidgets")
    
  SET (WXWIN_BINARY_PATH "${WXWIN_BINARY_DIR}/Build" )
  OPTION (WXWIN_FORCE_CONFIGURE "Weither to force configuration of wxWidgets" FALSE)
 
  # Check if already configured
  IF (WXWIN_FORCE_CONFIGURE OR NOT EXISTS ${WXWIN_BINARY_PATH}/wxUse.cmake)
  
    # Configuring the WXWIN library    
    FILE(MAKE_DIRECTORY "${WXWIN_BINARY_PATH}")
	
	
	set(wxWidgets_ROOT_DIR "${PROJECT_BINARY_DIR}/wxWidgets" CACHE PATH "wxWidgets base/installation directory")
	
	# Run configuration of WXWIN library
    MESSAGE(STATUS "WXWIN: Configuring external wxWidgets project")
    EXEC_PROGRAM(${CMAKE_COMMAND} "${WXWIN_BINARY_PATH}" ARGS "${WXWIN_SOURCE_PATH}" 
      -G"${CMAKE_GENERATOR}" 
      -DwxUSE_UNICODE:BOOL=FALSE
	  -DCMAKE_INSTALL_PREFIX:PATH="${wxWidgets_ROOT_DIR}"
      -DLIBRARY_OUTPUT_PATH:PATH="${LIBRARY_OUTPUT_PATH}" 
      -DEXECUTABLE_OUTPUT_PATH:PATH="${EXECUTABLE_OUTPUT_PATH}" 
      -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
      -DBUILD_SHARED_LIBS:BOOL=FALSE
	  -DwxBUILD_SHARED:BOOL=FALSE
      -DwxUSE_HTML:BOOL=TRUE
      OUTPUT_VARIABLE CMAKE_OUTPUT 
      RETURN_VALUE CMAKE_RETURN)
	  
	
  
    # Write configure log file
    FILE(WRITE ${WXWIN_BINARY_DIR}/CMake-Log.txt "CMake configure output ${CMAKE_OUTPUT}" )
	
	
	#---------------CMAKE SUCCESS IF-----------------
	IF (CMAKE_RETURN)
      # in case of error prompt an error dialog
      MESSAGE("CMake configure returned value = ${CMAKE_RETURN}")
      MESSAGE(FATAL_ERROR "${SUBPROJECT_NAME} configure error, cmake output = ${CMAKE_OUTPUT}")     
    ELSE (CMAKE_RETURN)
	
	  MESSAGE(STATUS "WXWIN: Compiling external wxWidgets project")
		
	  # load WXWIN build command from cmake's cache
      LOAD_CACHE(${WXWIN_BINARY_PATH} READ_WITH_PREFIX WXWIN_ MAKECOMMAND)
      
      #Split space separated arguments into a semi-colon separated list. Necessary for correct command line generation
      SEPARATE_ARGUMENTS(WXWIN_MAKECOMMAND)
      IF("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 2.0)
      	STRING (REPLACE "\\" "\\\\"  WXWIN_MAKECOMMAND "${WXWIN_MAKECOMMAND}")
      ENDIF("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 2.0)
	  STRING (REPLACE ";" " " WXWIN_MAKECOMMAND "${WXWIN_MAKECOMMAND}")
	  
	  STRING(APPEND WXWIN_MAKECOMMAND " --target Install")
	 
	  EXEC_PROGRAM(${WXWIN_MAKECOMMAND} ${WXWIN_BINARY_PATH}
	    OUTPUT_VARIABLE BUILD_OUTPUT
	    RETURN_VALUE BUILD_RETURN)
	  
	  #---------------BUILD SUCCESS IF-----------------
	  IF (BUILD_RETURN)
		# in case of error prompt an error dialog
        MESSAGE("CMake configure returned value = ${BUILD_RETURN}")
        MESSAGE(FATAL_ERROR "${SUBPROJECT_NAME} configure error, cmake output = ${BUILD_OUTPUT}")
	  ELSE (BUILD_RETURN)
		MESSAGE(STATUS "WXWIN: Compiling external wxWidgets project in Debug mode")
		
		STRING (REPLACE "\"Release\""  "\"DEBUG\"" WXWIN_MAKEDEBUGCOMMAND ${WXWIN_MAKECOMMAND})
	
		EXEC_PROGRAM(${WXWIN_MAKEDEBUGCOMMAND} ${WXWIN_BINARY_PATH}
	      OUTPUT_VARIABLE DEBUG_OUTPUT
	      RETURN_VALUE DEBUG_RETURN)
		   
		#---------------DEBUG SUCCESS IF-----------------
	    IF (DEBUG_RETURN)
		  # in case of error prompt an error dialog
          MESSAGE("CMake configure returned value = ${DEBUG_RETURN}")
          MESSAGE(FATAL_ERROR "${SUBPROJECT_NAME} configure error, cmake output = ${DEBUG_OUTPUT}")
	    ELSE (DEBUG_RETURN)
		
		  MESSAGE(STATUS "Creating file ${WXWIN_BINARY_PATH}/wxUse.cmake")
    	CONFIGURE_FILE("${WXWIN_SOURCE_DIR}/wxUse.cmake.in" "${WXWIN_BINARY_PATH}/wxUse.cmake" ESCAPE_QUOTES @ONLY IMMEDIATE)
		
		  MESSAGE(STATUS "WXWIN: Installed!!!")
		ENDIF(DEBUG_RETURN)
	  ENDIF (BUILD_RETURN)
	
	  SET(ALBA_USE_WX TRUE CACHE BOOL "Alba uses WX")
	  

    ENDIF (CMAKE_RETURN)
	
				
  ENDIF(WXWIN_FORCE_CONFIGURE OR NOT EXISTS ${WXWIN_BINARY_PATH}/wxUse.cmake) 
 ENDIF (WXWIN_SOURCE_PATH) 
 

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