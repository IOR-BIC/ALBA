PROJECT (testFrame)

SET(PROJECT_SRCS 
testFrameApp.cpp
testFrameApp.h
testFrameLogic.cpp
testFrameLogic.h
)

# copia icona e file delle risorse nella Build
#CONFIGURE_FILE( ${PROJECT_SOURCE_DIR}/app.rc.in ${PROJECT_BINARY_DIR}/app.rc IMMEDIATE)
#CONFIGURE_FILE( ${PROJECT_SOURCE_DIR}/app.ico   ${PROJECT_BINARY_DIR}/app.ico COPYONLY)

IF (WIN32)
  ADD_EXECUTABLE(${PROJECT_NAME} WIN32 ${PROJECT_SRCS} ${PROJECT_SOURCE_DIR}/app.rc)
ELSE (WIN32)
  ADD_EXECUTABLE(${PROJECT_NAME} ${PROJECT_SRCS} )
ENDIF (WIN32)

