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
  medGUIDicomSettings.cpp
  medGUIDicomSettings.h
  medGUIContextualMenu.cpp
  medGUIContextualMenu.h
  medGUISettingsAdvanced.cpp
  medGUISettingsAdvanced.h
)


#IF (MAF_USE_VTK)
#  SET(PROJECT_SRCS
#  ${PROJECT_SRCS}
#  )
#ENDIF (MAF_USE_VTK)

# currently special drivers only supported under WIN32
#IF (WIN32)
# SUBDIRS(Drivers)
#ENDIF (WIN32)

# List libraries that are needed by this project.
SET(PROJECT_LIBS mafBase mafIO mafGui vtkMAF)

# currently special drivers only supported under WIN32
#IF (WIN32)
#  SET(PROJECT_LIBS ${PROJECT_LIBS} mafDrivers)
#ENDIF (WIN32)

#IF (MAF_USE_VTK)
#  SET(PROJECT_LIBS ${PROJECT_LIBS} vtkHybrid)
#ENDIF (MAF_USE_VTK)

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})

#IF (MAF_USE_WX)
#  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_WXWIN_LIBRARY)
#ENDIF (MAF_USE_WX)

#CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/medGuiConfigure.h.in ${PROJECT_BINARY_DIR}/medGuiConfigure.h IMMEDIATE)
