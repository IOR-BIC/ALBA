PROJECT (TestDialog)

SET(PROJECT_SRCS 
testDialogApp.cpp
testDialogApp.h
testDialogLogic.cpp
testDialogLogic.h
testDialogDlg.cpp
testDialogDlg.h
)

# copia icona e file delle risorse nella Build
#CONFIGURE_FILE( ${PROJECT_SOURCE_DIR}/app.rc.in ${PROJECT_BINARY_DIR}/app.rc IMMEDIATE)
#CONFIGURE_FILE( ${PROJECT_SOURCE_DIR}/app.ico   ${PROJECT_BINARY_DIR}/app.ico COPYONLY)

IF (WIN32)
  ADD_EXECUTABLE(${PROJECT_NAME} WIN32 ${PROJECT_SRCS} ${PROJECT_SOURCE_DIR}/app.rc)
  CONFIGURE_FILE( ${MAF_SOURCE_DIR}/Gui/manifest.in ${EXECUTABLE_OUTPUT_PATH}/Debug/${PROJECT_NAME}.exe.manifest COPYONLY)
ELSE (WIN32)
  ADD_EXECUTABLE(${PROJECT_NAME} ${PROJECT_SRCS} )
ENDIF (WIN32)

SET(PROJECT_LIBS
  mafBase
  mafCore
  mafVME
  vtkGraphics
  vtkCommon
  vtkRendering
)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})







