#
# Copyright and description to be placed here
#

PROJECT(mafCore)
MESSAGE(STATUS "Configuring project mafCore")
# Set your list of sources here.  Do not change the name of the
# vtkLocal_SRCS variable.
SET(PROJECT_SRCS
  mafEventBase.h
  mafEventBase.cpp
#  mafEvent.h
#  mafEvent.cpp
  mafEventSource.h
  mafEventSource.cpp
  mafMatrix.cpp
  mafMatrix.h
  mafObserver.h
  mafObserver.cpp
  mafObserverCallback.h
  mafObserverCallback.cpp
  mafOBB.cpp
  mafOBB.h
)

IF (MAF_USE_VTK)
  #SET(PROJECT_SRCS
  #${PROJECT_SRCS}
  #)
ENDIF (MAF_USE_VTK)


# List libraries that are needed by this project.
SET(PROJECT_LIBS mafBase )

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