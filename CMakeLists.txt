#
# Program:   MULTIMOD APPLICATION FRAMEWORK (MAF)
# Module:    $RCSfile: CMakeLists.txt,v $
# Language:  CMake 2.0
# Date:      $Date: 2012-04-19 12:36:21 $
# Version:   $Revision: 1.12.2.10 $
#
# Description:
# CMake Project file for the Medical Library.

#SET (CMAKE_BACKWARDS_COMPATIBILITY "2.4" CACHE INSTERNAL "This project is for CMake 2.4")
CMAKE_MINIMUM_REQUIRED(VERSION 2.4) 

PROJECT (MED)


# Create the header to configure the classes.
CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/medDefines.h.in ${PROJECT_BINARY_DIR}/medDefines.h @ONLY)

#-----------------------------------------------------------------------------
# this define the MAF_SUBPROJECT macro
#-----------------------------------------------------------------------------
INCLUDE (${MED_SOURCE_DIR}/modules/MEDConfigMacro.cmake)

#-----------------------------------------------------------------------------
# where to place output binaries
#-----------------------------------------------------------------------------
SET (LIBRARY_OUTPUT_PATH ${MED_BINARY_DIR}/bin/ CACHE INTERNAL
     "Single output directory for building all libraries.")
SET (EXECUTABLE_OUTPUT_PATH ${MED_BINARY_DIR}/bin/ CACHE INTERNAL 
     "Single output directory for building all executables.")

MARK_AS_ADVANCED (
  LIBRARY_OUTPUT_PATH 
  EXECUTABLE_OUTPUT_PATH
)

#-----------------------------------------------------------------------------
# this is to enable precompiled headers under VC6.0
#-----------------------------------------------------------------------------
IF(WIN32)
  IF (NOT UNIX)
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /YX" CACHE STRING "Compiler flags")
  ENDIF (NOT UNIX)
ENDIF(WIN32)

#-----------------------------------------------------------------------------
# This enables support for Visual Studio Edit & Continue, OpenMP, and parallel compilation
#-----------------------------------------------------------------------------
IF (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
	#Add OpenMP Support
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /openmp")

	#Add support for Edit & Continue
	SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI")
	IF (NOT ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 17.0)
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
	ENDIF (NOT ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 17.0)
ENDIF (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")

#-----------------------------------------------------------------------------
# Options for MAF building
#-----------------------------------------------------------------------------
# OPTION(BUILD_SHARED_LIBS "Build with shared libraries." OFF )
OPTION(BUILD_EXAMPLES "Build examples." ON )
OPTION(BUILD_TESTING "Build the testing tree." OFF)
OPTION(BUILD_IPOSE "Build iPose application." OFF)
OPTION(MED_QA "Generate QA" OFF)


SET (CONFIGURE_ERROR 0)

#-----------------------------------------------------------------------------
# QA stuff
#-----------------------------------------------------------------------------
IF(MED_QA)
  SUBDIRS(qa)
ENDIF(MED_QA)

#-----------------------------------------------------------------------------
# If MAF found or configure continue with MED Configuration
#-----------------------------------------------------------------------------
INCLUDE("${PROJECT_SOURCE_DIR}/FindMAF.cmake")

IF(MAF_FOUND)

  INCLUDE("${MAF_USE_FILE}")

	IF(MAF_BUILD_MAFDLL)
		OPTION(MED_BUILD_MEDDLL "Build medDLL.dll." OFF)
	ENDIF(MAF_BUILD_MAFDLL)

	IF(MED_BUILD_MEDDLL)
	  SET(MED_BUILD_MEDDLL 1)
	ELSE(MED_BUILD_MEDDLL)
    SET(MED_BUILD_MEDDLL 0)
	ENDIF (MED_BUILD_MEDDLL)
  
  CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/Common/medCommonDefines.h.in ${PROJECT_BINARY_DIR}/medCommonDefines.h @ONLY)
  CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/Gui/medGuiDefines.h.in ${PROJECT_BINARY_DIR}/medGuiDefines.h @ONLY)
  CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/Operations/medOperationsDefines.h.in ${PROJECT_BINARY_DIR}/medOperationsDefines.h @ONLY)
  CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/Views/medViewsDefines.h.in ${PROJECT_BINARY_DIR}/medViewsDefines.h @ONLY)
  CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/Interaction/medInteractionDefines.h.in ${PROJECT_BINARY_DIR}/medInteractionDefines.h @ONLY)
  CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/VME/medVMEDefines.h.in ${PROJECT_BINARY_DIR}/medVMEDefines.h @ONLY)

	
  SET(MFL_SOURCE_DIR ${MFL_SOURCE_PATH})
  MED_SUBPROJECT(MedFL Libraries)
	IF (MedFL_CONFIG_FOUND)
  	FIND_FILE (USE_MedFL_FILE UseMedFL.cmake "${MED_BINARY_DIR}/Libraries") 
	  IF (USE_MedFL_FILE)
	    MESSAGE(STATUS "MED: MedFL Configured")
	    SET (MED_CAN_BUILD 1)
	  ELSE (USE_MedFL_FILE)
	  	MESSAGE(ERROR "Error configuring Libraries.")
		ENDIF (USE_MedFL_FILE)
	ELSE (MedFL_CONFIG_FOUND)
		MESSAGE(ERROR "Error finding Libraries.")
	ENDIF (MedFL_CONFIG_FOUND)

	INCLUDE("${USE_MedFL_FILE}")

  # Add single dirs to inclusion path
  INCLUDE_DIRECTORIES(BEFORE ".")
  INCLUDE_DIRECTORIES("${MED_BINARY_DIR}" "${MED_SOURCE_DIR}/Gui" "${MED_SOURCE_DIR}/Common" "${MED_SOURCE_DIR}/Operations"
                      "${MED_SOURCE_DIR}/Interaction" "${MED_SOURCE_DIR}/VME" "${MED_SOURCE_DIR}/Views" "${MED_SOURCE_DIR}/vtkMED"
                      "${MED_BINARY_DIR}/vtkMED")

  SET(INCLUDE_DIRS "${MED_BINARY_DIR}" "${MED_SOURCE_DIR}/Gui" "${MED_SOURCE_DIR}/Common" "${MED_SOURCE_DIR}/Operations"
                 "${MED_SOURCE_DIR}/Interaction" "${MED_SOURCE_DIR}/VME" "${MED_SOURCE_DIR}/Views" "${MED_SOURCE_DIR}/vtkMED"
                 "${MED_BINARY_DIR}/vtkMED")

  #other inclusions
  INCLUDE_DIRECTORIES(${MAF_BINARY_PATH}/Libraries/ITK/Sources/InsightToolkit-3.0.1/Wrapping/WrapITK/ExternalProjects/ItkVtkGlue/src)

  # Configuring MED kits
  MESSAGE(STATUS "MAF: Configuring MAF Kits")
  
  IF(MED_BUILD_MEDDLL)
    # SET(BUILD_SHARED_LIBS 1)
    
    SUBDIRS (Operations)
    SUBDIRS (Common)
    SUBDIRS (Interaction)
    SUBDIRS (Views)
    SUBDIRS (Gui)
    SUBDIRS (VME)
    SUBDIRS (vtkMED)
  ELSE(MED_BUILD_MEDDLL)
    SUBDIRS (Gui)
    SUBDIRS (vtkMED)
    SUBDIRS (Common)
    SUBDIRS (Operations)
    SUBDIRS (Interaction)
    SUBDIRS (Views)
    SUBDIRS (VME)
  ENDIF (MED_BUILD_MEDDLL)
  
  # Configuring Doxygen documentation  
  SUBDIRS (Docs)
  
  IF(WIN32)
		# Disable automatic manifest generation.
		STRING(REPLACE "/MANIFEST" "/MANIFEST:NO" CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS})
		# Explicitly disable it since it is the default for newer versions of VS
    STRING(REGEX MATCH "MANIFEST:NO" tmp_manifest ${CMAKE_EXE_LINKER_FLAGS})
    IF(NOT tmp_manifest)
        SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO")
    ENDIF(NOT tmp_manifest)
	ENDIF(WIN32)

	IF(MED_BUILD_MEDDLL)
	  SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /FORCE:MULTIPLE")
	ENDIF (MED_BUILD_MEDDLL)
	

  # Export Build settings and library dependences
  INCLUDE(CMakeExportBuildSettings)
  
  CMAKE_EXPORT_BUILD_SETTINGS(
    ${MED_BINARY_DIR}/MEDBuildSettings.cmake
  )
  
  EXPORT_LIBRARY_DEPENDENCIES(
    ${MED_BINARY_DIR}/MEDLibraryDepends.cmake
  )

  #### Include directory for binary distribution of MAF
  SET(MED_INSTALL_INCLUDE_DIRS ${MED_BINARY_DIR}/Include)
  FILE(MAKE_DIRECTORY ${MED_INSTALL_INCLUDE_DIRS})
	
	#NO MEDICAL REBUILD
	#searching python executable
	FIND_PROGRAM(PYTHON_EXECUTABLE
        NAMES python python3.2 python3.1 python3.0 python2.7 python2.6 python2.5 
				python2.4 python2.3 python2.2 python2.1 python2.0 python1.6 python1.5
		PATHS
        [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\3.2\\InstallPath]
        [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\3.1\\InstallPath]
        [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\3.0\\InstallPath]
        [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\2.7\\InstallPath]
        [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\2.6\\InstallPath]
        [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\2.5\\InstallPath]
				[HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\2.4\\InstallPath]
        [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\2.3\\InstallPath]
        [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\2.2\\InstallPath]
        [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\2.1\\InstallPath]
        [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\2.0\\InstallPath]
        [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\1.6\\InstallPath]
        [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\1.5\\InstallPath]
        )
  
	#NO MEDICAL REBUILD
	#if Python executable is not present i use the old system and i delete all the source headers
	#in this case there is no improvement to the old system 
	IF (NOT PYTHON_EXECUTABLE)
		FILE(GLOB files_to_remove "${MED_INSTALL_INCLUDE_DIRS}/*.*")
		FOREACH(f ${files_to_remove})
			FILE(REMOVE "${f}")
		ENDFOREACH(f ${files_to_remove})
  ENDIF(NOT PYTHON_EXECUTABLE)
	
		  
  FOREACH(dirs ${INCLUDE_DIRS})
    FILE(GLOB include_file_list "${dirs}/*.h")
    FILE(GLOB template_file_list "${dirs}/*.txx")
    FILE(GLOB icons_file_list "${dirs}/*.xpm")
		
    FOREACH(file ${include_file_list})
      FILE(RELATIVE_PATH file_name "${dirs}" "${file}")
    	CONFIGURE_FILE("${file}" "${MED_INSTALL_INCLUDE_DIRS}/${file_name}")
    ENDFOREACH(file ${include_file_list})
    
    FOREACH(template_file ${template_file_list})
      FILE(RELATIVE_PATH template_file_name "${dirs}" "${template_file}")
    	CONFIGURE_FILE("${template_file}" "${MED_INSTALL_INCLUDE_DIRS}/${template_file_name}")
    ENDFOREACH(template_file ${template_file_list})
    
    FOREACH(icons_file ${icons_file_list})
      FILE(RELATIVE_PATH icons_file_name "${dirs}" "${icons_file}")
    	CONFIGURE_FILE("${icons_file}" "${MED_INSTALL_INCLUDE_DIRS}/${icons_file_name}")
    ENDFOREACH(icons_file ${icons_file_list})
  ENDFOREACH(dirs ${MAF_INCLUDE_DIRS})
  
	#NO MEDICAL REBUILD
	#if Python executable is present i delete only removed header with python script
	IF(PYTHON_EXECUTABLE)
		EXEC_PROGRAM("${PYTHON_EXECUTABLE}"
			ARGS "${MED_SOURCE_DIR}/RemoveDeletedH.py ${MED_INSTALL_INCLUDE_DIRS} ${INCLUDE_DIRS} "
			OUTPUT_VARIABLE py_output
		)
		message("${py_output}")
	ENDIF(PYTHON_EXECUTABLE)
  
  CONFIGURE_FILE(
    ${MED_SOURCE_DIR}/MEDConfig.cmake.in
    ${MED_BINARY_DIR}/MEDConfig.cmake
  @ONLY IMMEDIATE
  )
  
  CONFIGURE_FILE(
    ${MED_SOURCE_DIR}/MEDUse.cmake.in
    ${MED_BINARY_DIR}/MEDUse.cmake
  @ONLY IMMEDIATE
  )

  #-----------------------------------------------------------------------------
  # Build examples too ?
  #-----------------------------------------------------------------------------
  IF (BUILD_EXAMPLES)
  	LINK_DIRECTORIES(${MED_BINARY_DIR}/bin)
    SUBDIRS(Examples)
  ENDIF (BUILD_EXAMPLES)
  
  if (BUILD_IPOSE)
    LINK_DIRECTORIES(${MED_BINARY_DIR}/bin)
    SUBDIRS(iPose)
  ENDIF (BUILD_IPOSE)
  
  #-----------------------------------------------------------------------------
  # Testing stuff
  #-----------------------------------------------------------------------------
  IF(BUILD_TESTING)
    FIND_PATH(MED_DATA_ROOT "TestingDataRoot.txt" "${MED_SOURCE_DIR}/Testing/unittestData" "${MED_SOURCE_DIR}/../TestingData" "${MED_SOURCE_DIR}/../mmData/TestingData" "${MED_SOURCE_DIR}/../../TestingData"  DOC "Test data repository")
    INCLUDE (${CMAKE_ROOT}/Modules/Dart.cmake)
    ENABLE_TESTING()
  
    INCLUDE (${CMAKE_ROOT}/Modules/Dart.cmake)
    
    # where will executable tests be written ?
    IF (EXECUTABLE_OUTPUT_PATH)
        SET (CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})
    ELSE (EXECUTABLE_OUTPUT_PATH)
        SET (CXX_TEST_PATH .)
    ENDIF (EXECUTABLE_OUTPUT_PATH)
    
    SUBDIRS (Testing)
  ENDIF(BUILD_TESTING)
ELSE (MAF_FOUND)
  MESSAGE (FATAL_ERROR "MAF library not found or configuration error, cannot build MED")
ENDIF (MAF_FOUND)


#-----------------------------------------------------------------------------
# MEDICAL_PARABUILD deploy stuff.  
#-----------------------------------------------------------------------------

# This is specific for automatic deploy of maf medical through Parabuild.
# A MEDICAL_PARABUILD_VERSION variable is created to gather versioning information
      
# this values are set by hand when major or minor is released
SET(MEDICAL_PARABUILD_MAJOR_VERSION 1 CACHE INTERNAL "major version")
SET(MEDICAL_PARABUILD_MINOR_VERSION 0 CACHE INTERNAL "minor version")

# automatic values from parabuild created temporary environment variable
SET(MEDICAL_PARABUILD_BUILD_TIMESTAMP $ENV{PARABUILD_BUILD_TIMESTAMP} CACHE INTERNAL "build timestamp")
SET(MEDICAL_PARABUILD_BUILD_NUMBER $ENV{PARABUILD_BUILD_NUMBER} CACHE INTERNAL "build number")

# create the main version variable:
SET(MEDICAL_PARABUILD_VERSION "${MEDICAL_PARABUILD_MAJOR_VERSION}_${MEDICAL_PARABUILD_MINOR_VERSION}_TS_${MEDICAL_PARABUILD_BUILD_TIMESTAMP}_BUILD_${MEDICAL_PARABUILD_BUILD_NUMBER}" CACHE INTERNAL "big version identifier concatenating other identifier")

#-----------------------------------------------------------------------------
# IPOSE_PARABUILD deploy stuff.  
#-----------------------------------------------------------------------------

# This is specific for NSIS Nullsoft Installer that runs currently under Windows only and Parabuild.
# An IPOSE_VERSION variable is created to gather versioning information
IF (WIN32)
  
  SET(IPOSE_EXECUTABLE_OUTPUT_PATH "${EXECUTABLE_OUTPUT_PATH}/Debug/" CACHE PATH "Source directory for iPose executable. This is used by the installer to find the executable.")
  
  SET(IPOSE_MAJOR_VERSION 0 CACHE INTERNAL "major version")
  SET(IPOSE_MINOR_VERSION 1 CACHE INTERNAL "minor version")
  
  SET(IPOSE_BUILD_TIMESTAMP $ENV{PARABUILD_BUILD_TIMESTAMP} CACHE INTERNAL "build timestamp")
  SET(IPOSE_BUILD_NUMBER $ENV{PARABUILD_BUILD_NUMBER} CACHE INTERNAL "build number")
  
  # create the main version variable:
  SET(IPOSE_VERSION "${IPOSE_MAJOR_VERSION}_${IPOSE_MINOR_VERSION}_TS_${IPOSE_BUILD_TIMESTAMP}_BUILD_${IPOSE_BUILD_NUMBER}" CACHE INTERNAL "big version identifier concatenating other identifier")
    
  # config installer file
  # set the installer directory variable
  SET(IPOSE_INSTALLER_DIR ${PROJECT_SOURCE_DIR}/iPose/Installer)
  # convert it to Windows format
  STRING (REPLACE "/" "\\" IPOSE_INSTALLER_DIR_WIN ${IPOSE_INSTALLER_DIR} )
  
  # installer executable will be written to this directory
  SET(IPOSE_INSTALLER_EXECUTABLE_OUTPUT_DIR ${IPOSE_EXECUTABLE_OUTPUT_PATH} CACHE PATH "the installer executable will be written to this directory ")
  # convert it to Windows format  
  STRING(REPLACE "/" "\\" IPOSE_INSTALLER_EXECUTABLE_OUTPUT_DIR_WIN ${IPOSE_INSTALLER_EXECUTABLE_OUTPUT_DIR})
  SET(IPOSE_INSTALLER_EXECUTABLE_OUTPUT_DIR_WIN ${IPOSE_INSTALLER_EXECUTABLE_OUTPUT_DIR_WIN} CACHE INTERNAL "get the executable  from this directory")
    
  # IPOSE
  IF (BUILD_IPOSE)
  CONFIGURE_FILE(${IPOSE_INSTALLER_DIR}/iposeInstallerScript.nsi.in  ${IPOSE_EXECUTABLE_OUTPUT_PATH}/iposeInstallerScript.nsi @ONLY)
  ENDIF (BUILD_IPOSE)
  
  SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LARGEADDRESSAWARE " )
  
ENDIF (WIN32)               
