#
# Copyright and description to be placed here
#

PROJECT(mafCore)

DBG_MESSAGE("Configuring project mafCore")

# Set your list of sources here.
SET(PROJECT_SRCS
  mafCoreFactory.cpp
  mafCoreFactory.h
  mafCoreDecl.cpp
  mafCoreDecl.h
  mafEventBase.cpp
  mafEventBase.h
  mafEventSource.cpp
  mafEventSource.h
  mafNode.cpp
  mafNode.h
  mafNodeIterator.cpp
  mafNodeIterator.h
  mafObserver.cpp
  mafObserver.h
  mafObserverCallback.cpp
  mafObserverCallback.h
  mafAttribute.cpp
  mafAttribute.h
  mmaTagArray.cpp
  mmaTagArray.h
  mmuTagItem.cpp
  mmuTagItem.h
#  mafEvent.cpp
#  mafEvent.h
)

IF (MAF_USE_VTK)
  #SET(PROJECT_SRCS
  #${PROJECT_SRCS}
  #)
ENDIF (MAF_USE_VTK)


# List libraries that are needed by this project.
SET(PROJECT_LIBS "mafBase" )

IF (MAF_USE_VTK)
  SET(PROJECT_LIBS ${PROJECT_LIBS} vtkCommon)
ENDIF (MAF_USE_VTK)

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})

IF (MAF_USE_VTK)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_VTK_LIBRARY)
ENDIF (MAF_USE_VTK)

IF (MAF_USE_WX)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_WXWIN_LIBRARY)
ENDIF (MAF_USE_WX)

# build tests
IF(BUILD_TESTING)
  SUBDIRS(Testing)
ENDIF(BUILD_TESTING)
