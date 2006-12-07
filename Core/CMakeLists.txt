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
  mafEventIO.cpp
  mafEventIO.h
  mafLogicWithGUI.cpp
  mafLogicWithGUI.h
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
  mafNodeLayout.cpp
  mafNodeLayout.h
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
  mafPrintSupport.cpp
  mafPrintSupport.h
  mafRemoteLogic.cpp
  mafRemoteLogic.h
  mafRemoteStorage.cpp
  mafRemoteStorage.h
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
  mmaApplicationLayout.cpp
  mmaApplicationLayout.h
  mmgListCtrlBitmap.cpp
  mmgListCtrlBitmap.h
  mmgVMEChooser.cpp
  mmgVMEChooser.h
  mmgVMEChooserAccept.cpp
  mmgVMEChooserAccept.h
  mmgVMEChooserTree.cpp
  mmgVMEChooserTree.h
  mmuTimeSet.cpp
  mmuTimeSet.h

)

IF (MAF_USE_VTK)
  SET(PROJECT_SRCS
  ${PROJECT_SRCS}
  mafAxes.cpp
  mafAxes.h
  mafLODActor.cpp
  mafLODActor.h
  mafLogicWithManagers.cpp
  mafLogicWithManagers.h
  mafPipe.cpp
  mafPipe.h
  mafPipeBox.cpp
  mafPipeBox.h
  mafPipeFactory.cpp
  mafPipeFactory.h
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
  mafViewHTML.cpp
  mafViewHTML.h
  mafViewManager.cpp
  mafViewManager.h
  mafViewPlot.cpp
  mafViewPlot.h
  mafViewVTK.cpp
  mafViewVTK.h
  mmaMaterial.cpp
  mmaMaterial.h
  mmaVolumeMaterial.cpp
  mmaVolumeMaterial.h
  mmgContextualMenu.cpp
  mmgContextualMenu.h
  mmgMaterialButton.cpp
  mmgMaterialButton.h
  mmgMaterialChooser.cpp
  mmgMaterialChooser.h
  mmgTreeContextualMenu.cpp
  mmgTreeContextualMenu.h
  vtkMAFAssembly.cpp
  vtkMAFAssembly.h
  )
ENDIF (MAF_USE_VTK)

# List libraries that are needed by this project.
SET(PROJECT_LIBS mafBase mafIO mafGui mafInteraction mafOperations)

IF (MAF_USE_VTK)
  SET(PROJECT_LIBS ${PROJECT_LIBS} vtkCommon)
ENDIF (MAF_USE_VTK)



# ------------------------------------
# Find GTK include Dirs 
# as required by mafRWIBase
# ------------------------------------
#INCLUDE_DIRECTORIES(/usr/include/gtk-2.0 /usr/lib/gtk-2.0/include /usr/include/atk-1.0 /usr/X11R6/include /usr/include/glib-2.0 #/usr/lib/glib-2.0/include /usr/include/pango-1.0 /usr/include/freetype2 /usr/include/freetype2/config )

IF (NOT WIN32)
  INCLUDE (${MAF_SOURCE_DIR}/modules/FindGTK2.cmake)
  INCLUDE_DIRECTORIES(${GTK_INCLUDE_DIRECTORIES})
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

