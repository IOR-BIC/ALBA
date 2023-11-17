#
# Copyright and description to be placed here
#

PROJECT(albaCore)

DBG_MESSAGE("Configuring project albaCore")

# Set your list of sources here.
SET(PROJECT_SRCS
  albaAbsMatrixPipe.cpp  		albaAbsMatrixPipe.h
  albaAttribute.cpp  			albaAttribute.h
  albaAttributeTraceability.cpp  albaAttributeTraceability.h
  albaCoreFactory.cpp  			albaCoreFactory.h
  albaDataPipe.cpp  				albaDataPipe.h
  albaEventIO.cpp  				albaEventIO.h
  albaMatrixPipe.cpp  			albaMatrixPipe.h
  albaVMEIterator.cpp  			albaVMEIterator.h
  albaOp.cpp  albaOp.h
  albaOpContextStack.cpp  		albaOpContextStack.h
  albaOpManager.cpp  			albaOpManager.h
  albaOpSelect.cpp  				albaOpSelect.h
  albaOpStack.cpp  				albaOpStack.h
  albaPrintManager.cpp  			albaPrintManager.h
  albaTagArray.cpp  				albaTagArray.h
  albaTagItem.cpp  				albaTagItem.h
  albaTimeMap.h
  albaTimeMapScalar.h
  albaUser.cpp  					albaUser.h
  albaVME.cpp  					albaVME.h
  albaVMEManager.cpp  			albaVMEManager.h
  albaVMEOutput.cpp  			albaVMEOutput.h
  albaVMEOutputNULL.cpp  		albaVMEOutputNULL.h
  albaVMERoot.cpp  				albaVMERoot.h
  albaVMEStorage.cpp  			albaVMEStorage.h
  mmaApplicationLayout.cpp  	mmaApplicationLayout.h
  albaGUIListCtrlBitmap.cpp  	albaGUIListCtrlBitmap.h
  albaGUIVMEChooser.cpp  		albaGUIVMEChooser.h
  albaGUIVMEChooserTree.cpp  	albaGUIVMEChooserTree.h
  mmuTimeSet.cpp  				mmuTimeSet.h
  albaProgressBarHelper.cpp 		albaProgressBarHelper.h
)

IF (ALBA_USE_VTK)
  SET(PROJECT_SRCS
  ${PROJECT_SRCS}
  albaAxes.cpp  					albaAxes.h
  albaLODActor.cpp  				albaLODActor.h
  albaLogicWithManagers.cpp  	albaLogicWithManagers.h
  albaPipe.cpp  					albaPipe.h
  albaPipeBox.cpp  				albaPipeBox.h
  albaPipeFactory.cpp  			albaPipeFactory.h
  albaRWI.cpp  					albaRWI.h
  albaRWIBase.cpp  				albaRWIBase.h
  albaSceneGraph.cpp  			albaSceneGraph.h
  albaSceneNode.cpp  			albaSceneNode.h
  albaViewCompound.cpp  			albaViewCompound.h
  albaViewHTML.cpp  				albaViewHTML.h
  albaViewManager.cpp  			albaViewManager.h
  albaViewPlot.cpp  				albaViewPlot.h
  albaViewVTK.cpp  				albaViewVTK.h
  mmaMaterial.cpp  				mmaMaterial.h
  mmaVolumeMaterial.cpp  		mmaVolumeMaterial.h
  albaGUIContextualMenu.cpp  	albaGUIContextualMenu.h
  albaGUIMaterialButton.cpp  	albaGUIMaterialButton.h
  albaGUIMaterialChooser.cpp  	albaGUIMaterialChooser.h
  albaGUITreeContextualMenu.cpp  albaGUITreeContextualMenu.h
  vtkALBAAssembly.cpp  vtkALBAAssembly.h
  )
ENDIF (ALBA_USE_VTK)

# List libraries that are needed by this project.
SET(PROJECT_LIBS albaBase albaIO albaGui albaInteraction albaOperations curl Libraries::wx::net Libraries::wx::core Libraries::wx::base)

IF (ALBA_USE_VTK)
  SET(PROJECT_LIBS ${PROJECT_LIBS} vtkCommon)
ENDIF (ALBA_USE_VTK)

# ------------------------------------
# Find GTK include Dirs 
# as required by albaRWIBase
# ------------------------------------
#INCLUDE_DIRECTORIES(/usr/include/gtk-2.0 /usr/lib/gtk-2.0/include /usr/include/atk-1.0 /usr/X11R6/include /usr/include/glib-2.0 #/usr/lib/glib-2.0/include /usr/include/pango-1.0 /usr/include/freetype2 /usr/include/freetype2/config )

IF (NOT WIN32)
  INCLUDE (${ALBA_SOURCE_DIR}/modules/FindGTK2.cmake)
  INCLUDE_DIRECTORIES(${GTK_INCLUDE_DIRECTORIES})
ENDIF (NOT WIN32)

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})

IF (ALBA_USE_VTK)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_VTK_LIBRARY)
ENDIF (ALBA_USE_VTK)

IF (ALBA_USE_WX)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_WXWIN_LIBRARY)
ENDIF (ALBA_USE_WX)

IF (ALBA_USE_CRYPTO)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_CRYPTO_LIBRARY)
ENDIF (ALBA_USE_CRYPTO)

IF (ALBA_USE_CURL)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_CURL_LIBRARY)
ENDIF (ALBA_USE_CURL)

ADD_DEPENDENCIES(${PROJECT_NAME} albaService)