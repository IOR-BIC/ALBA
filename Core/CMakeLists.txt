#
# Copyright and description to be placed here
#

PROJECT(mafCore)
MESSAGE(STATUS "Configuring project mafCore")
# Set your list of sources here.  Do not change the name of the
# vtkLocal_SRCS variable.
SET(PROJECT_SRCS
  mafDefines.cpp
  mafDefines.h
  mafDirectory.cpp
  mafDirectory.h
  mafDynamicLoader.cpp
  mafDynamicLoader.h
  mafEventBase.cpp
  mafEventBase.h
  mafEventSource.cpp
  mafEventSource.h
  mafMTime.cpp
  mafMTime.h
  mafMatrix.cpp
  mafMatrix.h
  mafMutexLock.cpp
  mafMutexLock.h
  mafOBB.cpp
  mafOBB.h
  mafObject.cpp
  mafObject.h
  mafObjectFactory.cpp
  mafObjectFactory.h
  mafObserver.cpp
  mafObserver.h
  mafObserverCallback.cpp
  mafObserverCallback.h
  mafSmartObject.cpp
  mafSmartObject.h
  mafString.cpp
  mafString.h
  mafTransform.cpp
  mafTransform.h
  mafTransformBase.cpp
  mafTransformBase.h
  mafTransformFrame.cpp
  mafTransformFrame.h
  mafVersion.h
  mafWIN32.h
  vtkMAFToLinearTransform.cxx
  vtkMAFToLinearTransform.h
#  mafEvent.cpp
#  mafEvent.h
)

IF (MAF_USE_VTK)
  #SET(PROJECT_SRCS
  #${PROJECT_SRCS}
  #)
ENDIF (MAF_USE_VTK)


# List libraries that are needed by this project.
SET(PROJECT_LIBS "" )

IF (MAF_USE_VTK)
  SET(PROJECT_LIBS ${PROJECT_LIBS} vtkCommon)
ENDIF (MAF_USE_VTK)

#-----------------------------------------------------------------------------
# Do not change anything below this line.

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
ADD_DEPENDENCIES(${PROJECT_NAME} ${MFL_BUILD_TARGETS})

IF(BUILD_TESTING)
  SUBDIRS(Testing)
ENDIF(BUILD_TESTING)