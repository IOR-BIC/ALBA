#
# Copyright and description to be placed here
#

PROJECT(mafGui)
MESSAGE(STATUS "Configuring project ${PROJECT_NAME}")

SET(PROJECT_SRCS
mafPics.cpp
mafPics.h
mafRWI.cpp
mafRWI.h
mafRWIBase.cpp
mafRWIBase.h
mafSideBar.cpp
mafSideBar.h
mafWXLog.cpp
mafWXLog.h
mmaMaterial.cpp
mmaMaterial.h
mmgButton.cpp
mmgButton.h
mmgCheckTree.cpp
mmgCheckTree.h
mmgCrossSplitter.cpp
mmgCrossSplitter.h
mmgDialog.cpp
mmgDialog.h
mmgDialogPreview.cpp
mmgDialogPreview.h
mafEvent.cpp
mafEvent.h
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
mmgMDIChild.cpp
mmgMDIChild.h
mmgMDIFrame.cpp
mmgMDIFrame.h
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
mmgSashPanel.cpp
mmgSashPanel.h
mmgScrolledPanel.cpp
mmgScrolledPanel.h
mmgSplittedPanel.cpp
mmgSplittedPanel.h
mmgTimeBar.cpp
mmgTimeBar.h
mmgTree.cpp
mmgTree.h
mmgTreeContextualMenu.cpp
mmgTreeContextualMenu.h
mmgValidator.cpp
mmgValidator.h
mmgViewFrame.cpp
mmgViewFrame.h
mmgViewWin.cpp
mmgViewWin.h
mmgVMEChooser.cpp
mmgVMEChooser.h
mmgVMEChooserAccept.cpp
mmgVMEChooserAccept.h
mmgVMEChooserTree.cpp
mmgVMEChooserTree.h
)

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

IF (NOT WIN32)
  #INCLUDE (${MAF_SOURCE_DIR}/modules/FindGTK20.cmake)
  INCLUDE (${CMAKE_ROOT}/Modules/FindGTK.cmake)
  INCLUDE_DIRECTORIES(${GTK_gtk_INCLUDE_PATH})
  INCLUDE_DIRECTORIES(${GTK_glib_INCLUDE_PATH})
  INCLUDE_DIRECTORIES(${GTK_glibconfig_INCLUDE_PATH})
  #INCLUDE_DIRECTORIES(${GTK_gtkgl_INCLUDE_PATH})
ENDIF (NOT WIN32)

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

IF(BUILD_TESTING)
  OPTION(BUILD_GUI_TESTING "Enable building of testing for GUI classes" ON)
  IF (BUILD_GUI_TESTING)
    SUBDIRS(Testing)
  ENDIF (BUILD_GUI_TESTING)
ENDIF(BUILD_TESTING)