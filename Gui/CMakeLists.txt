#
# Copyright and description to be placed here
#

PROJECT(mafGui)
MESSAGE(STATUS "Configuring project ${PROJECT_NAME}")

CONFIGURE_FILE("${MAF_SOURCE_DIR}/Gui/manifest.in" "${MAF_BINARY_DIR}/Gui/manifest.in" COPYONLY)

SET(PROJECT_SRCS
  #mmgSashPanel.cpp
  #mmgSashPanel.h
  mafEvent.cpp
  mafEvent.h
  mafPics.cpp
  mafPics.h
  mafWXLog.cpp
  mafWXLog.h
  mmgApplicationSettings.cpp
  mmgApplicationSettings.h
  mmgButton.cpp
  mmgButton.h
  mmgCheckListBox.cpp
  mmgCheckListBox.h
  mmgCheckTree.cpp
  mmgCheckTree.h
  mmgColorSwatch.cpp
  mmgColorSwatch.h
  mmgColorWidget.cpp
  mmgColorWidget.h
  mmgCrossSplitter.cpp
  mmgCrossSplitter.h
  mmgDialog.cpp
  mmgDialog.h
  mmgDictionaryWidget.cpp
  mmgDictionaryWidget.h
  mmgDockManager.cpp
  mmgDockManager.h
  mmgDockSettings.cpp
  mmgDockSettings.h
  mmgFileHistory.cpp
  mmgFileHistory.h
  mmgFlagSplitter.cpp
  mmgFlagSplitter.h
  mmgFloatSlider.cpp
  mmgFloatSlider.h
  mmgFrame.cpp
  mmgFrame.h
  mmgGui.cpp
  mmgGui.h
  mmgGuiHolder.cpp
  mmgGuiHolder.h
  mmgLab.cpp
  mmgLab.h
  mmgListBox.cpp
  mmgListBox.h
  mmgListCtrl.cpp
  mmgListCtrl.h
  mmgLocaleSettings.cpp
  mmgLocaleSettings.h
  mmgMeasureUnitSettings.cpp
  mmgMeasureUnitSettings.h
  mmgMovieCtrl.cpp
  mmgMovieCtrl.h
  mmgMDIChild.cpp
  mmgMDIChild.h
  mmgMDIFrame.cpp
  mmgMDIFrame.h
  mmgMovieCtrl.cpp
  mmgMovieCtrl.h
  mmgMutexPanel.cpp
  mmgMutexPanel.h
  mmgNamedPanel.cpp
  mmgNamedPanel.h
  mmgPanel.cpp
  mmgPanel.h
  mmgPanelStack.cpp
  mmgPanelStack.h
  mmgPicButton.cpp
  mmgPicButton.h
  mmgRangeSlider.cpp
  mmgRangeSlider.h
  mmgScrolledPanel.cpp
  mmgScrolledPanel.h
  mmgSettingsDialog.cpp
  mmgSettingsDialog.h
  mmgSplittedPanel.cpp
  mmgSplittedPanel.h
  mmgTimeBar.cpp
  mmgTimeBar.h
  mmgTree.cpp
  mmgTree.h
  mmgValidator.cpp
  mmgValidator.h
  mmgViewFrame.cpp
  mmgViewFrame.h
  mmgViewWin.cpp
  mmgViewWin.h
)


IF (MAF_USE_VTK)
  SET(PROJECT_SRCS
  ${PROJECT_SRCS}
  mafLightKit.cpp
  mafLightKit.h

  mmgDialogPreview.cpp
  mmgDialogPreview.h

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
  mmgLutEditor.cpp
  mmgLutEditor.h
  mmgLutPreset.cpp
  mmgLutPreset.h
  mmgLutSlider.cpp
  mmgLutSlider.h
  mmgLutSwatch.cpp
  mmgLutSwatch.h
  mmgLutWidget.cpp
  mmgLutWidget.h

  mmgHistogramWidget.cpp
  mmgHistogramWidget.h
  )
ENDIF (MAF_USE_VTK)



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
