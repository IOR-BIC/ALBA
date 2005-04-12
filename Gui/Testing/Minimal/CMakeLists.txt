# il project name deve finire con test
PROJECT (TestMinimalWX)

SET(TestMinimalWX_SRCS 
testMinimalWX.cpp
)


# copia icona e file delle risorse nella Build
#CONFIGURE_FILE( ${TestMinimalWX_SOURCE_DIR}/app.rc.in ${TestMinimalWX_BINARY_DIR}/app.rc IMMEDIATE)
#CONFIGURE_FILE( ${TestMinimalWX_SOURCE_DIR}/app.ico   ${TestMinimalWX_BINARY_DIR}/app.ico COPYONLY)

IF (WIN32)
#  ADD_EXECUTABLE(TestMinimalWX WIN32 ${TestMinimalWX_SRCS} ${TestMinimalWX_SOURCE_DIR}/app.rc)
  ADD_EXECUTABLE(TestMinimalWX ${TestMinimalWX_SRCS} ${TestMinimalWX_SOURCE_DIR}/app.rc)
ELSE (WIN32)
  ADD_EXECUTABLE(TestMinimalWX ${TestMinimalWX_SRCS} )
ENDIF (WIN32)
