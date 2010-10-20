IF (BUILD_IPOSE)

PROJECT (iPose)

SET(PROJECT_SRCS 
	iPoseApp.cpp
	iPoseApp.h
)

IF (NOT EXECUTABLE_OUTPUT_PATH)
  SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin/ CACHE INTERNAL 
  "Single output directory for building all executables.")
ENDIF (NOT EXECUTABLE_OUTPUT_PATH)

IF (WIN32)
	IF(${CMAKE_GENERATOR} STREQUAL "Visual Studio 9 2008" OR ${CMAKE_GENERATOR} STREQUAL "Visual Studio 8 2005")
	  SET(NO_MANIFEST 1)
	ELSE(${CMAKE_GENERATOR} STREQUAL "Visual Studio 9 2008" OR ${CMAKE_GENERATOR} STREQUAL "Visual Studio 8 2005")
	  SET(NO_MANIFEST 0)
	ENDIF(${CMAKE_GENERATOR} STREQUAL "Visual Studio 9 2008" OR ${CMAKE_GENERATOR} STREQUAL "Visual Studio 8 2005")
	
	CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/iposeApplication.rc.in ${PROJECT_BINARY_DIR}/iposeApplication.rc @ONLY)
  ADD_EXECUTABLE(${PROJECT_NAME} WIN32 ${PROJECT_SRCS} "${PROJECT_BINARY_DIR}/iposeApplication.rc")
ELSE (WIN32)
  ADD_EXECUTABLE(${PROJECT_NAME} ${PROJECT_SRCS} )
ENDIF (WIN32)

SET(PROJECT_LIBS
  medOperations medGui medViews medVME medInteraction mafVME mafOperations vtkMAF
)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
IF (WIN32)
  SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

IF (MED_USE_GDCM)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_GDCM_LIBRARY)
ENDIF (MED_USE_GDCM)

ENDIF (BUILD_IPOSE)

IF(MAF_FOUND)
	#####################################################################################
	#### Copy GPU shaders to the executable output path of the application.
	FOREACH(config ${CMAKE_CONFIGURATION_TYPES})
		SET(OUT_SHADER_DIR ${EXECUTABLE_OUTPUT_PATH}/${config}/Shaders/iso-surface)
		FILE(MAKE_DIRECTORY ${OUT_SHADER_DIR})
		
		FILE(GLOB cg_shading_file_list "${MAF_SOURCE_DIR}/vtkMAF/iso-surface/*.cg")
		FILE(GLOB glshader_shading_file_list "${MAF_SOURCE_DIR}/vtkMAF/iso-surface/*.glshader")
		
		FOREACH(file ${cg_shading_file_list})
		  FILE(RELATIVE_PATH file_name "${MAF_SOURCE_DIR}/vtkMAF/iso-surface/" "${file}")
		  CONFIGURE_FILE("${file}" "${OUT_SHADER_DIR}/${file_name}" @ONLY)
		ENDFOREACH(file ${cg_shading_file_list})
		
		FOREACH(file ${glshader_shading_file_list})
		  FILE(RELATIVE_PATH file_name "${MAF_SOURCE_DIR}/vtkMAF/iso-surface/" "${file}")
		  CONFIGURE_FILE("${file}" "${OUT_SHADER_DIR}/${file_name}" @ONLY)
		ENDFOREACH(file ${glshader_shading_file_list})
	ENDFOREACH(config)
	#####################################################################################
ENDIF (MAF_FOUND)