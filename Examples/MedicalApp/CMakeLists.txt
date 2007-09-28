PROJECT (exMedical)

SET(PROJECT_SRCS 
	exMedicalApp.cpp
	exMedicalApp.h
	mmoInteractionOp.cpp
	mmoInteractionOp.h
)

IF (NOT EXECUTABLE_OUTPUT_PATH)
  SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin/ CACHE INTERNAL 
     "Single output directory for building all executables.")
ENDIF (NOT EXECUTABLE_OUTPUT_PATH)

IF (WIN32)
  ADD_EXECUTABLE(${PROJECT_NAME} WIN32 ${PROJECT_SRCS} "${PROJECT_SOURCE_DIR}/app.rc")
#  CONFIGURE_FILE( "${MAF_SOURCE_DIR}/Gui/manifest.in" "${EXECUTABLE_OUTPUT_PATH}/Debug/${PROJECT_NAME}.exe.manifest" COPYONLY)
ELSE (WIN32)
  ADD_EXECUTABLE(${PROJECT_NAME} ${PROJECT_SRCS} )
ENDIF (WIN32)

SET(PROJECT_LIBS
  medOperations medGui medViews medVME medInteraction mafVME mafOperations vtkMAF
)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
