# il project name deve finire con test
PROJECT (testMinimalWX)

SET(testMinimalWX_SRCS 
testMinimalWX.cpp
)


# copia icona e file delle risorse nella Build
#CONFIGURE_FILE( ${testMinimalWX_SOURCE_DIR}/app.rc.in ${testMinimalWX_BINARY_DIR}/app.rc IMMEDIATE)
#CONFIGURE_FILE( ${testMinimalWX_SOURCE_DIR}/app.ico   ${testMinimalWX_BINARY_DIR}/app.ico COPYONLY)

IF (WIN32)
  ADD_EXECUTABLE(testMinimalWX WIN32 ${testMinimalWX_SRCS} ${testMinimalWX_SOURCE_DIR}/app.rc)
ELSE (WIN32)
  ADD_EXECUTABLE(testMinimalWX ${testMinimalWX_SRCS} )
ENDIF (WIN32)
