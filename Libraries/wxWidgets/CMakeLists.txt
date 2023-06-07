# Program:   ALBA (Agile Library for Biomedical Applications)
# Module:    $RCSfile: CMakeLists.txt,v $
# Language:  CMake 1.2
# Date:      $Date: 2010-12-29 18:50:18 $
# Version:   $Revision: 1.11.22.1 $
#
# Description:
# Project file of the Multimod Fundation Layer.

MESSAGE("Configuring wxWidgets")

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
	
    # Run configuration of WXWIN library
    MESSAGE(STATUS "WXWIN: Configuring external wxWidgets project")
    EXEC_PROGRAM(${CMAKE_COMMAND} "${WXWIN_BINARY_PATH}" ARGS "${WXWIN_SOURCE_PATH}" 
      -G"${CMAKE_GENERATOR}" 
      -DWX_USE_SOCKET:BOOL=TRUE 
      -DLIBRARY_OUTPUT_PATH:PATH="${LIBRARY_OUTPUT_PATH}" 
      -DEXECUTABLE_OUTPUT_PATH:PATH="${EXECUTABLE_OUTPUT_PATH}" 
      -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
      -DBUILD_SHARED_LIBS:BOOL=TRUE
      -DWX_USE_HTML:BOOL=TRUE
      OUTPUT_VARIABLE CMAKE_OUTPUT 
      RETURN_VALUE CMAKE_RETURN)
  
    # Write configure log file
    FILE(WRITE ${WXWIN_BINARY_DIR}/CMake-Log.txt "CMake configure output ${CMAKE_OUTPUT}" )

  ENDIF(WXWIN_FORCE_CONFIGURE OR NOT EXISTS ${WXWIN_BINARY_PATH}/wxUse.cmake) 
 ENDIF (WXWIN_SOURCE_PATH) 