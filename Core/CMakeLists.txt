#
# Copyright and description to be placed here
#

PROJECT(mafCore)

DBG_MESSAGE("Configuring project mafCore")

# Set your list of sources here.
SET(PROJECT_SRCS
#mafLogic_full_MAF1.cpp
#mafLogic_full_MAF1.h
#mafSceneGraph.cpp
#mafSceneGraph.h
#mafSceneNode.cpp
#mafSceneNode.h
#mafTimeVector.h
#mafTimeVector.txx
#mafVmeData.cpp
#mafVmeData.h
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
mafEventIO.cpp
mafEventIO.h
mafEventSender.h
mafEventSource.cpp
mafEventSource.h
mafLogicWithGUI.cpp
mafLogicWithGUI.h
mafLogicWithManagers.cpp
mafLogicWithManagers.h
mafMatrixPipe.cpp
mafMatrixPipe.h
mafNode.cpp
mafNode.h
mafNodeFactory.cpp
mafNodeFactory.h
mafNodeGeneric.cpp
mafNodeGeneric.h
mafNodeIterator.cpp
mafNodeIterator.h
mafNodeManager.cpp
mafNodeManager.h
mafNodeRoot.cpp
mafNodeRoot.h
mafObserver.cpp
mafObserver.h
mafObserverCallback.cpp
mafObserverCallback.h
mafOp.cpp
mafOp.h
mafOpContextStack.cpp
mafOpContextStack.h
mafOpManager.cpp
mafOpManager.h
mafOpSelect.cpp
mafOpSelect.h
mafOpStack.cpp
mafOpStack.h
mafPipe.cpp
mafPipe.h
mafRoot.cpp
mafRoot.h
mafSceneGraph.cpp
mafSceneGraph.h
mafSceneNode.cpp
mafSceneNode.h
mafTagArray.cpp
mafTagArray.h
mafTagItem.cpp
mafTagItem.h
mafTimeMap.h
mafVME.cpp
mafVME.h
mafVMEManager.cpp
mafVMEManager.h
mafVMEOutput.cpp
mafVMEOutput.h
mafVMEOutputNULL.cpp
mafVMEOutputNULL.h
mafVMERoot.cpp
mafVMERoot.h
mafView.cpp
mafView.h
mafViewManager.cpp
mafViewManager.h
mafViewVTK.cpp
mafViewVTK.h
mmuTimeSet.cpp
mmuTimeSet.h
vtkMAFAssembly.cpp
vtkMAFAssembly.h
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
  OPTION(BUILD_CORE_TESTING "Enable building of testing for CORE classes" ON)
  IF (BUILD_CORE_TESTING)
    SUBDIRS(Testing)
  ENDIF (BUILD_CORE_TESTING)
  MARK_AS_ADVANCED(BUILD_CORE_TESTING)
ENDIF(BUILD_TESTING)
