#
# Copyright and description to be placed here
#

PROJECT(mafGui)
MESSAGE(STATUS "Configuring project ${PROJECT_NAME}")

CONFIGURE_FILE("${MAF_SOURCE_DIR}/Gui/manifest.in" "${MAF_BINARY_DIR}/Gui/manifest.in" COPYONLY)

SET(PROJECT_SRCS
  mafEvent.cpp
  mafEvent.h
  mafGUIDialogFindVme.cpp
  mafGUIDialogFindVme.h
  mafGUIDialogLogin.cpp
  mafGUIDialogLogin.h
  
  mafGUIDialogTransferFunction2D.cpp
  mafGUIDialogTransferFunction2D.h
  mafTransferFunction2DInteractors.cpp
  
  mafGUISettings.cpp
  mafGUISettings.h
  mafGUISettingsStorage.cpp
  mafGUISettingsStorage.h
  mafGUISettingsTimeBar.cpp
  mafGUISettingsTimeBar.h
  
  ##### Example of SRB List Dialog requires OPENSSL #####
	#mafGUISRBBrowse.cpp
	#mafGUISRBBrowse.h
	#---------SRBList stub---------------
  #./SrbStorageWS/soapC.cpp
	#./SrbStorageWS/soapClient.cpp	
	#./SrbStorageWS/soapSrbUploadProxy.h
	#./SrbStorageWS/soapSrbDownloadProxy.h
	#./SrbStorageWS/soapSrbListProxy.h
	#./SrbStorageWS/stdsoap2.cpp
	#./SrbStorageWS/stdsoap2.h
	#------------------------------------
  mafPics.cpp
  mafPics.h
  mafWXLog.cpp
  mafWXLog.h
  mafGUIApplicationLayoutSettings.cpp
  mafGUIApplicationLayoutSettings.h
  mafGUIApplicationSettings.cpp
  mafGUIApplicationSettings.h
  mafGUIButton.cpp
  mafGUIButton.h
  mafGUICheckListBox.cpp
  mafGUICheckListBox.h
  mafGUICheckTree.cpp
  mafGUICheckTree.h
  mafGUIColorSwatch.cpp
  mafGUIColorSwatch.h
  mafGUIColorWidget.cpp
  mafGUIColorWidget.h
  mafGUICrossSplitter.cpp
  mafGUICrossSplitter.h
  mafGUIDialog.cpp
  mafGUIDialog.h
  mafGUIDockManager.cpp
  mafGUIDockManager.h
  mafGUIDockSettings.cpp
  mafGUIDockSettings.h
  mafGUIFileHistory.cpp
  mafGUIFileHistory.h
  mafGUIFlagSplitter.cpp
  mafGUIFlagSplitter.h
  mafGUIFloatSlider.cpp
  mafGUIFloatSlider.h
  mafGUIFrame.cpp
  mafGUIFrame.h
  mafGUI.cpp
  mafGUI.h
  mafGUIHolder.cpp
  mafGUIHolder.h
  mafGUILab.cpp
  mafGUILab.h
  mafGUIListBox.cpp
  mafGUIListBox.h
  mafGUIListCtrl.cpp
  mafGUIListCtrl.h
  mafGUILocaleSettings.cpp
  mafGUILocaleSettings.h
  mafGUIMeasureUnitSettings.cpp
  mafGUIMeasureUnitSettings.h
  mafGUIMovieCtrl.cpp
  mafGUIMovieCtrl.h
  mafGUIMDIChild.cpp
  mafGUIMDIChild.h
  mafGUIMDIFrame.cpp
  mafGUIMDIFrame.h
  mafGUIMutexPanel.cpp
  mafGUIMutexPanel.h
  mafGUINamedPanel.cpp
  mafGUINamedPanel.h
  mafGUIPanel.cpp
  mafGUIPanel.h
  mafGUIPanelStack.cpp
  mafGUIPanelStack.h
  mafGUIPicButton.cpp
  mafGUIPicButton.h
  mafGUIRangeSlider.cpp
  mafGUIRangeSlider.h
  mafGUIRollOut.cpp
  mafGUIRollOut.h
  mafGUISashPanel.h
  mafGUISashPanel.cpp
  mafGUIScrolledPanel.cpp
  mafGUIScrolledPanel.h
  mafGUISettingsDialog.cpp
  mafGUISettingsDialog.h
  mafGUISplittedPanel.cpp
  mafGUISplittedPanel.h
  mafGUITimeBar.cpp
  mafGUITimeBar.h
  mafGUITree.cpp
  mafGUITree.h
  mafGUIValidator.cpp
  mafGUIValidator.h
  mafGUIViewFrame.cpp
  mafGUIViewFrame.h
  mafGUIViewWin.cpp
  mafGUIViewWin.h
  mafGUICrossIncremental.cpp
  mafGUICrossIncremental.h

  mafGUIComboBox.cpp
  mafGUIComboBox.h
)

IF (MAF_USE_CURL)
	SET(PROJECT_SRCS
	${PROJECT_SRCS}
	mafGUIDialogRemoteFile.cpp
	mafGUIDialogRemoteFile.h
	)
ENDIF (MAF_USE_CURL)

IF (MAF_USE_VTK)
  SET(PROJECT_SRCS
  ${PROJECT_SRCS}
  mafLightKit.cpp
  mafLightKit.h

  mafGUIDialogPreview.cpp
  mafGUIDialogPreview.h

  mafGUIDictionaryWidget.cpp
  mafGUIDictionaryWidget.h

  mafAnimate.cpp
  mafAnimate.h
  mafAttachCamera.cpp
  mafAttachCamera.h

  mafSideBar.cpp
  mafSideBar.h
  mafTextKit.cpp
  mafTextKit.h
  mafVTKLog.cpp
  mafVTKLog.h

  # depends on mafAssembly -> mafRWIBase
  mafView.cpp
  mafView.h

  # these depends on vtkLookupTable
  mafColor.cpp
  mafColor.h
  mafGUILutEditor.cpp
  mafGUILutEditor.h
  mafGUILutPreset.cpp
  mafGUILutPreset.h
  mafGUILutSlider.cpp
  mafGUILutSlider.h
  mafGUILutSwatch.cpp
  mafGUILutSwatch.h
  mafGUILutWidget.cpp
  mafGUILutWidget.h

  mafGUIHistogramWidget.cpp
  mafGUIHistogramWidget.h
  )
ENDIF (MAF_USE_VTK)

IF (WITH_OPENSSL)


ENDIF(WITH_OPENSSL)



# List the kits from VTK that are needed by this project.
SET(PROJECT_LIBS
  mafBase
)

#........................................................
#solo la vtkCommon e' linkata per default
#per linkare altre lib inserirle qui di seguito
#
#  # VTK specific classes
#  IF (MAF_USE_VTK)
#  SET(PROJECT_LIBS ${PROJECT_LIBS} 
#    ...... place libname here .....     
#  )
#
#  SET(PROJECT_SRCS
#    ${PROJECT_SRCS}
#    ...... place source filenames here .....     
#  )
#ENDIF (MAF_USE_VTK)
#........................................................

#--------------------------------------------------------
# Do not change anything below this line.

##### SIL ------------ IF (NOT WIN32)
##### SIL ------------   #INCLUDE (${MAF_SOURCE_DIR}/modules/FindGTK20.cmake)
##### SIL ------------ INCLUDE (${CMAKE_ROOT}/Modules/FindGTK.cmake)
##### SIL ------------   INCLUDE_DIRECTORIES(${GTK_gtk_INCLUDE_PATH})
##### SIL ------------   INCLUDE_DIRECTORIES(${GTK_glib_INCLUDE_PATH})
##### SIL ------------   INCLUDE_DIRECTORIES(${GTK_glibconfig_INCLUDE_PATH})
##### SIL ------------   #INCLUDE_DIRECTORIES(${GTK_gtkgl_INCLUDE_PATH})
##### SIL ------------ ENDIF (NOT WIN32)

# Need to include class headers and the configuration header.
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR})

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
##MESSAGE ("${PROJECT_NAME} linked libraries: ${PROJECT_LIBS}")
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})

# this is necessaty to force MFL compilation....
##MESSAGE ("${PROJECT_NAME} dependencies: ${MFL_BUILD_TARGETS}")
##ADD_DEPENDENCIES(${PROJECT_NAME} ${MFL_BUILD_TARGETS})

#--------------------------------------------------------
#SUBDIRS(examples)
