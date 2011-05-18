PROJECT (exOperations)

SET(PROJECT_SRCS 
	exOperationApp.cpp
	exOperationApp.h
	
	#SRBUpload/SrbUpload.nsmap
	#SRBUpload/soapH.h
	#SRBUpload/soapC.cpp
	#SRBUpload/soapClient.cpp
	#SRBUpload/stdsoap2.cpp
	#SRBUpload/stdsoap2.h
	#mmoSRBUpload.cpp
	#mmoSRBUpload.h
)

IF (WIN32)
	IF(${CMAKE_GENERATOR} STREQUAL "Visual Studio 9 2008" OR ${CMAKE_GENERATOR} STREQUAL "Visual Studio 8 2005" OR ${CMAKE_GENERATOR} STREQUAL "Visual Studio 10")
	  SET(NO_MANIFEST 1)
	ELSE(${CMAKE_GENERATOR} STREQUAL "Visual Studio 9 2008" OR ${CMAKE_GENERATOR} STREQUAL "Visual Studio 8 2005" OR ${CMAKE_GENERATOR} STREQUAL "Visual Studio 10")
	  SET(NO_MANIFEST 0)
	ENDIF(${CMAKE_GENERATOR} STREQUAL "Visual Studio 9 2008" OR ${CMAKE_GENERATOR} STREQUAL "Visual Studio 8 2005" OR ${CMAKE_GENERATOR} STREQUAL "Visual Studio 10")
	
	CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/app.rc.in ${PROJECT_BINARY_DIR}/app.rc @ONLY)
  ADD_EXECUTABLE(${PROJECT_NAME} WIN32 ${PROJECT_SRCS} "${PROJECT_BINARY_DIR}/app.rc")
ELSE (WIN32)
  ADD_EXECUTABLE(${PROJECT_NAME} ${PROJECT_SRCS} )
ENDIF (WIN32)

SET(PROJECT_LIBS
  mafVME mafOperations
)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
IF (WIN32)
  SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

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
