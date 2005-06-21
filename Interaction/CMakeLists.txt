#
# Copyright and description to be placed here
#

PROJECT(mafInteraction)

DBG_MESSAGE("Configuring project ${PROJECT_NAME}")

# Set your list of sources here.
SET(PROJECT_SRCS
#mafAvatar3D2DPicker.cpp
#mafAvatar3D2DPicker.h
#mmdRemoteMouse.cpp
#mmdRemoteMouse.h
mafAction.cpp
mafAction.h
mafAgent.cpp
mafAgent.h
mafAgentEventHandler.cpp
mafAgentEventHandler.h
mafAgentEventQueue.cpp
mafAgentEventQueue.h
mafAgentThreaded.cpp
mafAgentThreaded.h
mafAvatar.cpp
mafAvatar.h
mafAvatar2D.cpp
mafAvatar2D.h
mafAvatar3D.cpp
mafAvatar3D.h
mafAvatar3DCone.cpp
mafAvatar3DCone.h
mafCameraTransform.cpp
mafCameraTransform.h
mafDevice.cpp
mafDevice.h
mafDeviceManager.cpp
mafDeviceManager.h
mafDeviceSet.cpp
mafDeviceSet.h
mafEventInteraction.cpp
mafEventInteraction.h
mafInteractionFactory.cpp
mafInteractionFactory.h
mafInteractionManager.cpp
mafInteractionManager.h
mafInteractor.cpp
mafInteractor.h
mafRefSys.cpp
mafRefSys.h
mmdButtonsPad.cpp
mmdButtonsPad.h
mmdMouse.cpp
mmdMouse.h
mmdTracker.cpp
mmdTracker.h
#mmdTrackerSet.cpp
#mmdTrackerSet.h
mmi6DOF.cpp
mmi6DOF.h
mmi6DOFCameraMove.cpp
mmi6DOFCameraMove.h
#mmi6DOFMove.cpp
#mmi6DOFMove.h
mmiCameraMove.cpp
mmiCameraMove.h
#mmiCompositorDevice.cpp
#mmiCompositorDevice.h
mmiCompositorMouse.cpp
mmiCompositorMouse.h
mmiConstraint.cpp
mmiConstraint.h
#mmiGeneric.cpp
#mmiGeneric.h
mmiGeneric6DOF.cpp
mmiGeneric6DOF.h
mmiGenericInterface.cpp
mmiGenericInterface.h
mmiGenericMouse.cpp
mmiGenericMouse.h
mmiPER.cpp
mmiPER.h
mmiPicker.cpp
mmiPicker.h
mmiSER.cpp
mmiSER.h
)

#IF (MAF_USE_VTK)
#  SET(PROJECT_SRCS
#  ${PROJECT_SRCS}
#  )
#ENDIF (MAF_USE_VTK)

# List libraries that are needed by this project.
SET(PROJECT_LIBS mafBase vtkMAF)

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
