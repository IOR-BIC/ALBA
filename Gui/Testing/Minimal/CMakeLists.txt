# il project name deve finire con test
PROJECT (MinimalTest)

SET(MinimalTest_SRCS 
MinimalTest.cpp
)


# copia icona e file delle risorse nella Build
#CONFIGURE_FILE( ${MinimalTest_SOURCE_DIR}/app.rc.in ${MinimalTest_BINARY_DIR}/app.rc IMMEDIATE)
#CONFIGURE_FILE( ${MinimalTest_SOURCE_DIR}/app.ico   ${MinimalTest_BINARY_DIR}/app.ico COPYONLY)

IF (WIN32)
  ADD_EXECUTABLE(MinimalTest WIN32 ${MinimalTest_SRCS} ${MinimalTest_SOURCE_DIR}/app.rc)
ELSE (WIN32)
  ADD_EXECUTABLE(MinimalTest ${MinimalTest_SRCS} )
ENDIF (WIN32)
