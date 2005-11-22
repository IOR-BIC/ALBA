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
  mafGizmoBoundingBox.cpp
  mafGizmoBoundingBox.h
  mafGizmoHandle.cpp
  mafGizmoHandle.h
  mafGizmoInterface.cpp
  mafGizmoInterface.h
  mafGizmoROI.cpp
  mafGizmoROI.h
  mafGizmoRotate.cpp
  mafGizmoRotate.h
  mafGizmoRotateCircle.cpp
  mafGizmoRotateCircle.h
  mafGizmoRotateFan.cpp
  mafGizmoRotateFan.h
  mafGizmoScale.cpp
  mafGizmoScale.h
  mafGizmoScaleAxis.cpp
  mafGizmoScaleAxis.h
  mafGizmoScaleIsotropic.cpp
  mafGizmoScaleIsotropic.h
  mafGizmoTranslate.cpp
  mafGizmoTranslate.h
  mafGizmoTranslateAxis.cpp
  mafGizmoTranslateAxis.h
  mafGizmoTranslatePlane.cpp
  mafGizmoTranslatePlane.h
  mafGuiGizmoInterface.cpp
  mafGuiGizmoInterface.h
  mafGuiGizmoRotate.cpp
  mafGuiGizmoRotate.h
  mafGuiGizmoScale.cpp
  mafGuiGizmoScale.h
  mafGuiGizmoTranslate.cpp
  mafGuiGizmoTranslate.h
  mafGuiSaveRestorePose.cpp
  mafGuiSaveRestorePose.h
  mafGuiTransformInterface.cpp
  mafGuiTransformInterface.h
  mafGuiTransformMouse.cpp
  mafGuiTransformMouse.h
  mafGuiTransformTextEntries.cpp
  mafGuiTransformTextEntries.h
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
  
  mmi2DMeter.cpp
  mmi2DMeter.h
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
  mmiExtractIsosurface.cpp
  mmiExtractIsosurface.h
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

IF (MAF_USE_P5_GLOVE)
  SET(PROJECT_SRCS
  ${PROJECT_SRCS}
  mmdP5Glove.cpp
  mmdP5Glove.h
  )
ENDIF (MAF_USE_P5_GLOVE)

#IF (MAF_USE_VTK)
#  SET(PROJECT_SRCS
#  ${PROJECT_SRCS}
#  )
#ENDIF (MAF_USE_VTK)

IF (WIN32)
  OPTION(MAF_USE_P5_GLOVE "Link driver for P5 glove device" OFF)
ENDIF (WIN32)

# currently special drivers only supported under WIN32
IF (WIN32)
 SUBDIRS(Drivers)
ENDIF (WIN32)

# List libraries that are needed by this project.
SET(PROJECT_LIBS mafBase mafIO mafGui vtkMAF)

# currently special drivers only supported under WIN32
IF (WIN32)
  SET(PROJECT_LIBS ${PROJECT_LIBS} mafDrivers)
ENDIF (WIN32)

#IF (MAF_USE_VTK)
  SET(PROJECT_LIBS ${PROJECT_LIBS} vtkHybrid)
#ENDIF (MAF_USE_VTK)

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})

IF (MAF_USE_WX)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_WXWIN_LIBRARY)
ENDIF (MAF_USE_WX)

CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/mafInteractionConfigure.h.in ${PROJECT_BINARY_DIR}/mafInteractionConfigure.h IMMEDIATE)
