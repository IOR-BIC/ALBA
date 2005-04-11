#
# Copyright and description to be placed here
#

PROJECT(mafCore)

DBG_MESSAGE("Configuring project mafCore")

# Set your list of sources here.
SET(PROJECT_SRCS
mafAbsMatrixPipe.cpp
mafAbsMatrixPipe.h
mafAttribute.cpp
mafAttribute.h
mafCoreFactory.cpp
mafCoreFactory.h
mafDataPipe.cpp
mafDataPipe.h
mafEvent.cpp
mafEvent.h
mafEventBase.cpp
mafEventBase.h
mafEventSender.h
mafEventSource.cpp
mafEventSource.h
mafEventIO.cpp
mafEventIO.h
mafMatrixPipe.cpp
mafMatrixPipe.h
mafNode.cpp
mafNode.h
mafNodeFactory.h
mafNodeFactory.cpp
mafNodeIterator.cpp
mafNodeIterator.h
mafNodeRoot.cpp
mafNodeRoot.h
mafObserver.cpp
mafObserver.h
mafObserverCallback.cpp
mafObserverCallback.h
mafTagArray.cpp
mafTagArray.h
mafTagItem.cpp
mafTagItem.h
mafTimeMap.h
mafTimeVector.txx
mafTimeVector.h
mafVME.cpp
mafVME.h
mafVMEOutput.cpp
mafVMEOutput.h
mafVMEOutputNULL.cpp
mafVMEOutputNULL.h
mafVMERoot.cpp
mafVMERoot.h
mafView.cpp
mafView.h
mmuTimeSet.cpp
mmuTimeSet.h
)

IF (MAF_USE_VTK)
  #SET(PROJECT_SRCS
  #${PROJECT_SRCS}
  #)
ENDIF (MAF_USE_VTK)

# List libraries that are needed by this project.
SET(PROJECT_LIBS mafBase mafIO mafGui)

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

IF (MAF_USE_CRYPTO)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_CRYPTO_LIBRARY)
ENDIF (MAF_USE_CRYPTO)

# build tests
IF(BUILD_TESTING)
  SUBDIRS(Testing)
ENDIF(BUILD_TESTING)
