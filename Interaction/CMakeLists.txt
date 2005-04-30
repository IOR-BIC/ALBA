#
# Copyright and description to be placed here
#

PROJECT(mafInteraction)

DBG_MESSAGE("Configuring project ${PROJECT_NAME}")

# Set your list of sources here.
SET(PROJECT_SRCS
mafAgent.cpp
mafAgent.h
mafAgentEventHandler.cpp
mafAgentEventHandler.h
mafAgentEventQueue.h
mafAgentEventQueue.cpp
mafAgentThreaded.h
mafAgentThreaded.cpp
mafAvatar.cpp
mafAvatar.h
mafAvatar2D.cpp
mafAvatar2D.h
mafAvatar3D.cpp
mafAvatar3D.h
mafAvatar3D2DPicker.cpp
mafAvatar3D2DPicker.h
mafAvatar3DCone.cpp
mafAvatar3DCone.h
mafDevice.cpp
mafDevice.h
mafDeviceSet.cpp
mafDeviceSet.h
mafDeviceManager.cpp
mafDeviceManager.h
mafEventInteraction.cpp
mafEventInteraction.h
mafInteractionFactory.cpp
mafInteractionFactory.h
mafInteractionManager.cpp
mafInteractionManager.h
mafInteractor.cpp
mafInteractor.h
mmiPER.h
mmiPER.cpp
mmiPicker.cpp
mmiPicker.h
mmiSER.h
mmiSER.cpp
mmdTracker.cpp
mmdTracker.h
mmdTrackerSet.cpp
mmdTrackerSet.h
mmdMouse.cpp
mmdMouse.h
mmdButtonsPad.cpp
mmdButtonsPad.h
mmdRemoteMouse.cpp
mmdRemoteMouse.h
mafCameraTransform.cpp
mafCameraTransform.h
)

#IF (MAF_USE_VTK)
#  SET(PROJECT_SRCS
#  ${PROJECT_SRCS}
#  )
#ENDIF (MAF_USE_VTK)

# List libraries that are needed by this project.
SET(PROJECT_LIBS mafBase)

#IF (MAF_USE_VTK)
#  SET(PROJECT_LIBS ${PROJECT_LIBS} vtkIO vtkRendering vtkGraphics)
#ENDIF (MAF_USE_VTK)

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})

#IF (MAF_USE_VTK)
#  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_VTK_LIBRARY)
#ENDIF (MAF_USE_VTK)

IF (MAF_USE_WX)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_WXWIN_LIBRARY)
ENDIF (MAF_USE_WX)

# build tests
IF(BUILD_TESTING)
  OPTION(BUILD_Interaction_TESTING "Enable building of testing for Interaction classes" ON)
  IF (BUILD_Interaction_TESTING)
    SUBDIRS(Testing)
  ENDIF (BUILD_Interaction_TESTING)
ENDIF(BUILD_TESTING)
