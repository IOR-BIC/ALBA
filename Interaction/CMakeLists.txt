#
# Copyright and description to be placed here
#

PROJECT(mafInteraction)

DBG_MESSAGE("Configuring project ${PROJECT_NAME}")

# Set your list of sources here.
SET(PROJECT_SRCS
  
  #mafAvatar3D2DPicker.cpp
  #mafAvatar3D2DPicker.h
  
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
  mafGizmoAutoscaleHelper.cpp
  mafGizmoAutoscaleHelper.h
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
  mafGizmoPath.cpp
  mafGizmoPath.h
  mafGizmoPathRuler.cpp
  mafGizmoPathRuler.h
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
  mafGizmoSlice.cpp
  mafGizmoSlice.h
  medGizmoInteractionDebugger.cpp
  medGizmoInteractionDebugger.h
  mafGUIGizmoInterface.cpp
  mafGUIGizmoInterface.h
  mafGUIGizmoRotate.cpp
  mafGUIGizmoRotate.h
  mafGUIGizmoScale.cpp
  mafGUIGizmoScale.h
  mafGUIGizmoTranslate.cpp
  mafGUIGizmoTranslate.h
  mafGUISaveRestorePose.cpp
  mafGUISaveRestorePose.h
  mafGUITransformInterface.cpp
  mafGUITransformInterface.h
  mafGUITransformMouse.cpp
  mafGUITransformMouse.h
  mafGUITransformTextEntries.cpp
  mafGUITransformTextEntries.h
  mafInteractionFactory.cpp
  mafInteractionFactory.h
  mafInteractionManager.cpp
  mafInteractionManager.h
  mafInteractor.cpp
  mafInteractor.h
  mafRefSys.cpp
  mafRefSys.h
  
  mafDeviceButtonsPad.cpp
  mafDeviceButtonsPad.h
  mafDeviceButtonsPadMouse.cpp
  mafDeviceButtonsPadMouse.h
  mafDeviceButtonsPadMouseRemote.cpp
  mafDeviceButtonsPadMouseRemote.h
  mafDeviceButtonsPadTracker.cpp
  mafDeviceButtonsPadTracker.h
  
  mafInteractor2DAngle.cpp
  mafInteractor2DAngle.h
  mafInteractor2DDistance.cpp
  mafInteractor2DDistance.h
  mafInteractor2DIndicator.cpp
  mafInteractor2DIndicator.h

  mafInteractor6DOF.cpp
  mafInteractor6DOF.h
  mafInteractor6DOFCameraMove.cpp
  mafInteractor6DOFCameraMove.h
  mafInteractorCameraMove.cpp
  mafInteractorCameraMove.h
  mafInteractorCompositorMouse.cpp
  mafInteractorCompositorMouse.h
  mafInteractorConstraint.cpp
  mafInteractorConstraint.h
  mafInteractorExtractIsosurface.cpp
  mafInteractorExtractIsosurface.h
  mafInteractorGeneric6DOF.cpp
  mafInteractorGeneric6DOF.h
  mafInteractorGenericInterface.cpp
  mafInteractorGenericInterface.h
  mafInteractorGenericMouse.cpp
  mafInteractorGenericMouse.h
  mafInteractorPER.cpp
  mafInteractorPER.h
  mafInteractorPicker.cpp
  mafInteractorPicker.h
  mafInteractorSelectCell.cpp
  mafInteractorSelectCell.h
  mafInteractorSER.cpp
  mafInteractorSER.h
  ######################
  medInteractorPicker.cpp
  medInteractorPicker.h
  medInteractorDICOMImporter.h
  medInteractorDICOMImporter.cpp  
  medInteractor2DAngle.cpp
  medInteractor2DAngle.h
  medInteractor2DDistance.cpp
  medInteractor2DDistance.h
  medInteractor2DIndicator.cpp
  medInteractor2DIndicator.h
  medInteractorSegmentationPicker.cpp
  medInteractorSegmentationPicker.h 
  medInteractorPERScalarInformation.cpp
  medInteractorPERScalarInformation.h
  medInteractorPERBrushFeedback.cpp
  medInteractorPERBrushFeedback.h
  mmiVTKPicker.cpp
  mmiVTKPicker.h
  mmiInfoImage.cpp
  mmiInfoImage.h
  mmiSelectPoint.cpp
  mmiSelectPoint.h
  medGizmoPolylineGraph.cpp
  medGizmoPolylineGraph.h
  medDeviceButtonsPadMouseDialog.h
  medDeviceButtonsPadMouseDialog.cpp
  medGUITransformSliders.cpp
  medGUITransformSliders.h
  
  ##### Introduced with VolumeLarge - to be committed down ###
  # mafGizmoROI_BES.cpp
  # mafGizmoROI_BES.h  
  ############################################################

  medGizmoCrossRotateTranslate.cpp
  medGizmoCrossRotateTranslate.h
	  
  medGizmoCrossRotate.cpp
  medGizmoCrossRotate.h
  medGizmoCrossRotateAxis.cpp
  medGizmoCrossRotateAxis.h
  medGizmoCrossRotateFan.cpp
  medGizmoCrossRotateFan.h

  medGizmoCrossTranslate.cpp
  medGizmoCrossTranslate.h
  medGizmoCrossTranslateAxis.cpp
  medGizmoCrossTranslateAxis.h
  medGizmoCrossTranslatePlane.cpp
  medGizmoCrossTranslatePlane.h
)

## SIL == removed under Linux - since it requires mafDrivers - which seems to be Win32 only
SET(PROJECT_SRCS
  ${PROJECT_SRCS}
  mafDeviceClientMAF.cpp
  mafDeviceClientMAF.h
  )


IF (MAF_USE_P5_GLOVE)
  SET(PROJECT_SRCS
  ${PROJECT_SRCS}
  mafDeviceButtonsPadTrackerP5Glove.cpp
  mafDeviceButtonsPadTrackerP5Glove.h
  )
ENDIF (MAF_USE_P5_GLOVE)

IF (MAF_USE_WIIMOTE)
  SET(PROJECT_SRCS
  ${PROJECT_SRCS}
  mafDeviceTrackerWIIMote.cpp
  mafDeviceTrackerWIIMote.h
  )
ENDIF (MAF_USE_WIIMOTE)



#IF (MAF_USE_VTK)
#  SET(PROJECT_SRCS
#  ${PROJECT_SRCS}
#  )
#ENDIF (MAF_USE_VTK)

IF (WIN32)
  OPTION(MAF_USE_P5_GLOVE "Link driver for P5 glove device" OFF)
  OPTION(MAF_USE_WIIMOTE "Link driver for WIIMote device" OFF)
ENDIF (WIN32)

# currently special drivers only supported under WIN32
#IF (WIN32)
 SUBDIRS(Drivers)
#ENDIF (WIN32)

# List libraries that are needed by this project.
SET(PROJECT_LIBS mafBase mafIO mafGui vtkMAF)

# currently special drivers only supported under WIN32
#IF (WIN32)
  SET(PROJECT_LIBS ${PROJECT_LIBS} mafDrivers)
#ENDIF (WIN32)

#IF (MAF_USE_VTK)
  SET(PROJECT_LIBS ${PROJECT_LIBS} vtkHybrid)
#ENDIF (MAF_USE_VTK)

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})

ADD_DEPENDENCIES(${PROJECT_NAME} mafCommon mafVME vtkMAF)

IF (MAF_USE_WX)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_WXWIN_LIBRARY)
ENDIF (MAF_USE_WX)

#CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/mafInteractionConfigure.h.in ${PROJECT_BINARY_DIR}/mafInteractionConfigure.h IMMEDIATE)

