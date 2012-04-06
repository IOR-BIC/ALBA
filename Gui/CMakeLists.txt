#
# Copyright and description to be placed here
#

PROJECT(medGui)

#DBG_MESSAGE("Configuring project ${PROJECT_NAME}")

# Set your list of sources here.
SET(PROJECT_SRCS
  medGUILandmark.cpp
  medGUILandmark.h
  medGUIWizard.cpp
  medGUIWizard.h
  medGUIWizardPage.cpp
  medGUIWizardPage.h
  medGUIWizardPageNew.cpp
  medGUIWizardPageNew.h
  medGUIDicomSettings.cpp
  medGUIDicomSettings.h
  # medGUIContextualMenu.cpp
  # medGUIContextualMenu.h
  medGUISettingsAdvanced.cpp
  medGUISettingsAdvanced.h
  medGUIDynamicVP.cpp
  medGUIDynamicVP.h
  medGUILutHistogramSwatch.cpp
  medGUILutHistogramSwatch.h
  medGUILutHistogramEditor.cpp
  medGUILutHistogramEditor.h
  
)


# List libraries that are needed by this project.
SET(PROJECT_LIBS mafBase mafIO mafGui vtkMAF)

IF(MED_BUILD_MEDDLL)
  SET(BUILD_SHARED_LIBS 1)
  ADD_DEFINITIONS(-DMED_GUI_EXPORTS)
  # Create the library.
  ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} mafDLL)
ELSE(MED_BUILD_MEDDLL)
  # Create the library.
  ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
ENDIF (MED_BUILD_MEDDLL)
