#
# Copyright and description to be placed here
#

PROJECT(albaGui)
DBG_MESSAGE("Configuring project albaCore")

CONFIGURE_FILE("${ALBA_SOURCE_DIR}/Gui/manifest.in" "${ALBA_BINARY_DIR}/Gui/manifest.in" COPYONLY)

SET(PROJECT_SRCS
  albaEvent.cpp  albaEvent.h
  albaGUIDialogFindVme.cpp  albaGUIDialogFindVme.h
  albaGUIDialogLogin.cpp  albaGUIDialogLogin.h
  
  albaGUIDialogTransferFunction2D.cpp  albaGUIDialogTransferFunction2D.h
  albaTransferFunction2DInteractors.cpp
  
  albaGUISettings.cpp  albaGUISettings.h
  albaGUISettingsTimeBar.cpp  albaGUISettingsTimeBar.h
  
  albaPics.cpp  						albaPics.h
  albaWXLog.cpp  					albaWXLog.h
  albaGUIApplicationSettings.cpp  	albaGUIApplicationSettings.h
  albaGUIButton.cpp  				albaGUIButton.h
  albaGUICheckListBox.cpp  			albaGUICheckListBox.h
  albaGUICheckTree.cpp  				albaGUICheckTree.h
  albaGUIColorSwatch.cpp  			albaGUIColorSwatch.h
  albaGUIColorWidget.cpp  			albaGUIColorWidget.h
  albaGUICrossSplitter.cpp  			albaGUICrossSplitter.h
  albaGUIDialog.cpp  				albaGUIDialog.h
  albaGUIDockManager.cpp  			albaGUIDockManager.h
  albaGUIFileHistory.cpp  			albaGUIFileHistory.h
  albaGUIFlagSplitter.cpp  			albaGUIFlagSplitter.h
  albaGUIFloatSlider.cpp  			albaGUIFloatSlider.h
  albaGUIFrame.cpp  					albaGUIFrame.h
  albaGUI.cpp  						albaGUI.h
  albaGUIHolder.cpp  				albaGUIHolder.h
  albaGUILab.cpp  					albaGUILab.h
  albaGUIListBox.cpp  				albaGUIListBox.h
  albaGUIListCtrl.cpp  				albaGUIListCtrl.h
  albaGUIMovieCtrl.cpp  				albaGUIMovieCtrl.h
  albaGUIMDIChild.cpp  				albaGUIMDIChild.h
  albaGUIMDIFrame.cpp  				albaGUIMDIFrame.h
  albaGUIMutexPanel.cpp  			albaGUIMutexPanel.h
  albaGUINamedPanel.cpp  			albaGUINamedPanel.h
  albaGUIPanel.cpp  					albaGUIPanel.h
  albaGUIPanelStack.cpp  			albaGUIPanelStack.h
  albaGUIPicButton.cpp  				albaGUIPicButton.h
  albaGUIRangeSlider.cpp  			albaGUIRangeSlider.h
  albaGUIRollOut.cpp  				albaGUIRollOut.h
  albaGUISashPanel.h  				albaGUISashPanel.cpp
  albaGUIScrolledPanel.cpp  			albaGUIScrolledPanel.h
  albaGUISettingsDialog.cpp  		albaGUISettingsDialog.h
  albaGUISplittedPanel.cpp  			albaGUISplittedPanel.h
  albaGUITimeBar.cpp  				albaGUITimeBar.h
  albaGUITree.cpp  					albaGUITree.h
  albaGUIValidator.cpp  				albaGUIValidator.h
  albaGUIViewFrame.cpp  				albaGUIViewFrame.h
  albaGUIViewWin.cpp  				albaGUIViewWin.h
  albaGUICrossIncremental.cpp  		albaGUICrossIncremental.h
  albaGUIComboBox.cpp  				albaGUIComboBox.h
  albaGUILandmark.cpp  				albaGUILandmark.h
  albaGUIWizard.cpp  				albaGUIWizard.h
  albaGUIWizardPage.cpp  			albaGUIWizardPage.h
  albaGUIWizardPageNew.cpp  			albaGUIWizardPageNew.h
  albaGUIDicomSettings.cpp  			albaGUIDicomSettings.h
  albaGUISettingsAdvanced.cpp  		albaGUISettingsAdvanced.h
  albaGUIDynamicVP.cpp  				albaGUIDynamicVP.h
  albaGUILutHistogramSwatch.cpp  	albaGUILutHistogramSwatch.h
  albaGUILutHistogramEditor.cpp  	albaGUILutHistogramEditor.h
	albaGUIImageViewer.cpp							albaGUIImageViewer.h
	albaGUIAboutDialog.cpp				albaGUIAboutDialog.h
	albaGUISplashScreen.cpp				albaGUISplashScreen.h
	albaGUIDialogProsthesisSelection.cpp	albaGUIDialogProsthesisSelection.h
	albaGUIHyperLink.cpp					albaGUIHyperLink.h
)

IF (ALBA_USE_VTK)
  SET(PROJECT_SRCS
  ${PROJECT_SRCS}
  albaLightKit.cpp					  albaLightKit.h
  albaGUIDialogPreview.cpp			  albaGUIDialogPreview.h
  albaGUIDictionaryWidget.cpp		  albaGUIDictionaryWidget.h
  albaAnimate.cpp					  albaAnimate.h
  albaAttachCamera.cpp				  albaAttachCamera.h
	albaAbsSideBar.h
  albaSideBar.cpp					  albaSideBar.h
  albaTextKit.cpp					  albaTextKit.h
  albaVTKLog.cpp						  albaVTKLog.h

  # depends on albaAssembly -> albaRWIBase
  albaView.cpp						  albaView.h

  # these depends on vtkLookupTable
  albaColor.cpp						  albaColor.h
  albaGUILutEditor.cpp				  albaGUILutEditor.h
  albaGUILutPreset.cpp				  albaGUILutPreset.h
  albaGUILutSlider.cpp				  albaGUILutSlider.h
  albaGUILutSwatch.cpp				  albaGUILutSwatch.h
  albaGUILutWidget.cpp				  albaGUILutWidget.h
  albaGUIHistogramWidget.cpp			  albaGUIHistogramWidget.h
  )
ENDIF (ALBA_USE_VTK)

IF (WITH_OPENSSL)

ENDIF(WITH_OPENSSL)


# List the kits from VTK that are needed by this project.
SET(PROJECT_LIBS albaBase)

#........................................................
#solo la vtkCommon e' linkata per default
#per linkare altre lib inserirle qui di seguito
#
#  # VTK specific classes
#  IF (ALBA_USE_VTK)
#  SET(PROJECT_LIBS ${PROJECT_LIBS} 
#    ...... place libname here .....     
#  )
#
#  SET(PROJECT_SRCS
#    ${PROJECT_SRCS}
#    ...... place source filenames here .....     
#  )
#ENDIF (ALBA_USE_VTK)
#........................................................

#--------------------------------------------------------
# Do not change anything below this line.

##### SIL ------------ IF (NOT WIN32)
##### SIL ------------   #INCLUDE (${ALBA_SOURCE_DIR}/modules/FindGTK20.cmake)
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
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS} GPUAPI)

# this is necessaty to force MFL compilation....
##MESSAGE ("${PROJECT_NAME} dependencies: ${MFL_BUILD_TARGETS}")
##ADD_DEPENDENCIES(${PROJECT_NAME} ${MFL_BUILD_TARGETS})

#--------------------------------------------------------
#SUBDIRS(examples)

ADD_DEPENDENCIES(${PROJECT_NAME} albaService)