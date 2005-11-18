#
# Copyright and description to be placed here
#

PROJECT(mafCore)

DBG_MESSAGE("Configuring project mafCore")

# Set your list of sources here.
SET(PROJECT_SRCS
  mafAxes.cpp
  mafAxes.h
  mafAbsMatrixPipe.cpp
  mafAbsMatrixPipe.h
  mafAttribute.cpp
  mafAttribute.h
  mafCoreFactory.cpp
  mafCoreFactory.h
  mafDataPipe.cpp
  mafDataPipe.h
  mafEventIO.cpp
  mafEventIO.h
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
  mafPipeFactory.cpp
  mafPipeFactory.h
  mafRoot.cpp
  mafRoot.h
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
  mafVMEStorage.cpp
  mafVMEStorage.h
  mafViewManager.cpp
  mafViewManager.h
  mmgListCtrlBitmap.cpp
  mmgListCtrlBitmap.h
  mmgMaterialButton.cpp
  mmgMaterialButton.h
  mmgMaterialChooser.cpp
  mmgMaterialChooser.h
  mmgVMEChooser.cpp
  mmgVMEChooser.h
  mmgVMEChooserAccept.cpp
  mmgVMEChooserAccept.h
  mmgVMEChooserTree.cpp
  mmgVMEChooserTree.h
  mmuTimeSet.cpp
  mmuTimeSet.h
	mafRWI.cpp
	mafRWI.h
	mafRWIBase.cpp
	mafRWIBase.h
	mafSceneGraph.cpp
	mafSceneGraph.h
	mafSceneNode.cpp
	mafSceneNode.h
	mafViewCompound.cpp
	mafViewCompound.h
	mafViewVTK.cpp
	mafViewVTK.h
	mmgContextualMenu.cpp
	mmgContextualMenu.h
	mmgTreeContextualMenu.cpp
	mmgTreeContextualMenu.h
	vtkMAFAssembly.cpp
	vtkMAFAssembly.h
)

IF (MAF_USE_VTK)
  SET(PROJECT_SRCS
  ${PROJECT_SRCS}
  mafPipeBox.cpp
  mafPipeBox.h
  mmaMaterial.cpp
  mmaMaterial.h
  )
ENDIF (MAF_USE_VTK)

# List libraries that are needed by this project.
SET(PROJECT_LIBS mafBase mafIO mafGui mafInteraction)

IF (MAF_USE_VTK)
  SET(PROJECT_LIBS ${PROJECT_LIBS} vtkCommon)
ENDIF (MAF_USE_VTK)

IF (NOT WIN32)
  #INCLUDE (${MAF_SOURCE_DIR}/modules/FindGTK20.cmake)
  INCLUDE (${CMAKE_ROOT}/Modules/FindGTK.cmake)
  INCLUDE_DIRECTORIES(${GTK_gtk_INCLUDE_PATH})
  INCLUDE_DIRECTORIES(${GTK_glib_INCLUDE_PATH})
  INCLUDE_DIRECTORIES(${GTK_glibconfig_INCLUDE_PATH})
  #INCLUDE_DIRECTORIES(${GTK_gtkgl_INCLUDE_PATH})
ENDIF (NOT WIN32)

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
