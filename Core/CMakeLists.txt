#
# Copyright and description to be placed here
#

PROJECT(mafCore)
MESSAGE(STATUS "Configuring project mafCore")
# Set your list of sources here.  Do not change the name of the
# vtkLocal_SRCS variable.
SET(PROJECT_SRCS
  mafDefines.h
  mafDirectory.cpp
  mafDirectory.h
  mafDynamicLoader.cpp
  mafDynamicLoader.h
  mafEventBase.h
  mafEventBase.cpp
#  mafEvent.h
#  mafEvent.cpp
  mafObject.cpp
  mafObject.h
  mafEventSource.h
  mafEventSource.cpp
  mafObjectFactory.cpp
  mafObjectFactory.h
  mafObserver.h
  mafObserver.cpp
  mafObserverCallback.h
  mafObserverCallback.cpp
  mafString.cpp
  mafString.h
)

# List the kits from VTK that are needed by this project.
SET(PROJECT_LIBS
  vtkCommon
)

#-----------------------------------------------------------------------------
# Do not change anything below this line.

# Need to include class headers and the configuration header.
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR})

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
##MESSAGE ("${PROJECT_NAME} linked libraries: ${PROJECT_LIBS}")
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
# this is necessaty to force MFL compilation....
##MESSAGE ("${PROJECT_NAME} dependencies: ${MFL_BUILD_TARGETS}")
ADD_DEPENDENCIES(${PROJECT_NAME} ${MFL_BUILD_TARGETS})

IF(BUILD_TESTING)
  SUBDIRS(Testing)
ENDIF(BUILD_TESTING)