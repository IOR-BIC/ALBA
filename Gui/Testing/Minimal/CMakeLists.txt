# il project name deve finire con test
PROJECT (silvano)

SET(silvano_SRCS 
silvano.cpp
)


# copia icona e file delle risorse nella Build
#CONFIGURE_FILE( ${silvano_SOURCE_DIR}/app.rc.in ${silvano_BINARY_DIR}/app.rc IMMEDIATE)
#CONFIGURE_FILE( ${silvano_SOURCE_DIR}/app.ico   ${silvano_BINARY_DIR}/app.ico COPYONLY)

IF (WIN32)
#  ADD_EXECUTABLE(silvano WIN32 ${silvano_SRCS} ${silvano_SOURCE_DIR}/app.rc)
  ADD_EXECUTABLE(silvano ${silvano_SRCS} ${silvano_SOURCE_DIR}/app.rc)
ELSE (WIN32)
  ADD_EXECUTABLE(silvano ${silvano_SRCS} )
ENDIF (WIN32)
