#
# Copyright and description to be placed here
#

PROJECT(albaInteraction)

DBG_MESSAGE("Configuring project ${PROJECT_NAME}")

# Set your list of sources here.
SET(PROJECT_SRCS
  albaAction.cpp  							albaAction.h
  albaAgent.cpp  							albaAgent.h
  albaAgentEventHandler.cpp  				albaAgentEventHandler.h
  albaAgentEventQueue.cpp  					albaAgentEventQueue.h
  albaAgentThreaded.cpp  					albaAgentThreaded.h
  albaAvatar.cpp  							albaAvatar.h
  albaAvatar2D.cpp  							albaAvatar2D.h
  albaAvatar3D.cpp  							albaAvatar3D.h
  albaAvatar3DCone.cpp  						albaAvatar3DCone.h
  albaCameraTransform.cpp  					albaCameraTransform.h
  albaGizmoAutoscaleHelper.cpp  				albaGizmoAutoscaleHelper.h
  albaDevice.cpp  							albaDevice.h
  albaDeviceManager.cpp  					albaDeviceManager.h
  albaDeviceSet.cpp  						albaDeviceSet.h
  albaEventInteraction.cpp  					albaEventInteraction.h
  albaGizmoBoundingBox.cpp  					albaGizmoBoundingBox.h
  albaGizmoHandle.cpp  						albaGizmoHandle.h
  albaGizmoInterface.cpp  					albaGizmoInterface.h
  albaGizmoPath.cpp  						albaGizmoPath.h
  albaGizmoPathRuler.cpp  					albaGizmoPathRuler.h
  albaGizmoROI.cpp  							albaGizmoROI.h
  albaGizmoRotate.cpp  						albaGizmoRotate.h
  albaGizmoRotateCircle.cpp  				albaGizmoRotateCircle.h
  albaGizmoRotateFan.cpp  					albaGizmoRotateFan.h
  albaGizmoScale.cpp  						albaGizmoScale.h
  albaGizmoScaleAxis.cpp  					albaGizmoScaleAxis.h
  albaGizmoScaleIsotropic.cpp  				albaGizmoScaleIsotropic.h
  albaGizmoTranslate.cpp  					albaGizmoTranslate.h
  albaGizmoTranslateAxis.cpp  				albaGizmoTranslateAxis.h
  albaGizmoTranslatePlane.cpp  				albaGizmoTranslatePlane.h
  albaGizmoSlice.cpp  						albaGizmoSlice.h
  albaGizmoInteractionDebugger.cpp  			albaGizmoInteractionDebugger.h
  albaGUIGizmoInterface.cpp  				albaGUIGizmoInterface.h
  albaGUIGizmoRotate.cpp  					albaGUIGizmoRotate.h
  albaGUIGizmoScale.cpp  					albaGUIGizmoScale.h
  albaGUIGizmoTranslate.cpp  				albaGUIGizmoTranslate.h
  albaGUISaveRestorePose.cpp  				albaGUISaveRestorePose.h
  albaGUITransformInterface.cpp  			albaGUITransformInterface.h
  albaGUITransformMouse.cpp  				albaGUITransformMouse.h
  albaGUITransformTextEntries.cpp  			albaGUITransformTextEntries.h
  albaInteractionFactory.cpp  				albaInteractionFactory.h
  albaInteractionManager.cpp  				albaInteractionManager.h
  albaInteractor.cpp  						albaInteractor.h
  albaRefSys.cpp  							albaRefSys.h  
  albaDeviceButtonsPad.cpp  					albaDeviceButtonsPad.h
  albaDeviceButtonsPadMouse.cpp  			albaDeviceButtonsPadMouse.h
  albaDeviceButtonsPadTracker.cpp  			albaDeviceButtonsPadTracker.h  
  albaInteractor2DAngle.cpp  				albaInteractor2DAngle.h
  albaInteractor2DDistance.cpp  				albaInteractor2DDistance.h
  albaInteractor2DIndicator.cpp  			albaInteractor2DIndicator.h
  albaInteractor6DOF.cpp  					albaInteractor6DOF.h
  albaInteractor6DOFCameraMove.cpp  			albaInteractor6DOFCameraMove.h
  albaInteractorCameraMove.cpp  				albaInteractorCameraMove.h
  albaInteractorCompositorMouse.cpp  		albaInteractorCompositorMouse.h
  albaInteractorConstraint.cpp  				albaInteractorConstraint.h
  albaInteractorExtractIsosurface.cpp  		albaInteractorExtractIsosurface.h
  albaInteractorGeneric6DOF.cpp  			albaInteractorGeneric6DOF.h
  albaInteractorGenericInterface.cpp  		albaInteractorGenericInterface.h
  albaInteractorGenericMouse.cpp  			albaInteractorGenericMouse.h
  albaInteractorPER.cpp  					albaInteractorPER.h
  albaInteractorPicker.cpp  					albaInteractorPicker.h
  albaInteractorSelectCell.cpp  					albaInteractorSelectCell.h
  albaInteractorSER.cpp  									albaInteractorSER.h  ######################
  albaInteractorCameraPicker.cpp  				albaInteractorCameraPicker.h
  albaInteractorPERPicker.cpp  						albaInteractorPERPicker.h
  albaInteractorDICOMImporter.h  					albaInteractorDICOMImporter.cpp  
  albaInteractorSegmentationPicker.cpp  	albaInteractorSegmentationPicker.h 
  albaInteractorPERScalarInformation.cpp  albaInteractorPERScalarInformation.h
  albaInteractorPERBrushFeedback.cpp  		albaInteractorPERBrushFeedback.h
  mmiVTKPicker.cpp  											mmiVTKPicker.h
  mmiInfoImage.cpp  											mmiInfoImage.h
  mmiSelectPoint.cpp  										mmiSelectPoint.h
  albaGizmoPolylineGraph.cpp  						albaGizmoPolylineGraph.h
  albaDeviceButtonsPadMouseDialog.cpp	  	albaDeviceButtonsPadMouseDialog.h
  albaGUITransformSliders.cpp  						albaGUITransformSliders.h    
  albaGizmoCrossRotateTranslate.cpp  			albaGizmoCrossRotateTranslate.h	  
  albaGizmoCrossRotate.cpp  							albaGizmoCrossRotate.h
  albaGizmoCrossRotateAxis.cpp  					albaGizmoCrossRotateAxis.h
  albaGizmoCrossRotateFan.cpp  						albaGizmoCrossRotateFan.h
  albaGizmoCrossTranslate.cpp  						albaGizmoCrossTranslate.h
  albaGizmoCrossTranslateAxis.cpp  				albaGizmoCrossTranslateAxis.h
  albaGizmoCrossTranslatePlane.cpp  			albaGizmoCrossTranslatePlane.h
	albaInteractorSlider.cpp								albaInteractorSlider.h
	
	albaActor2dStackHelper.cpp							albaActor2dStackHelper.h
	albaInteractor2DMeasure.cpp							albaInteractor2DMeasure.h
	albaInteractor2DMeasure.cpp							albaInteractor2DMeasure.h
	albaInteractor2DMeasure_Angle.cpp				albaInteractor2DMeasure_Angle.h
	albaInteractor2DMeasure_AngleLine.cpp		albaInteractor2DMeasure_AngleLine.h
	albaInteractor2DMeasure_CenterPoint.cpp	albaInteractor2DMeasure_CenterPoint.h
	albaInteractor2DMeasure_Distance.cpp 		albaInteractor2DMeasure_Distance.h
	albaInteractor2DMeasure_Line.cpp				albaInteractor2DMeasure_Line.h
	albaInteractor2DMeasure_LineDistance.cpp	albaInteractor2DMeasure_LineDistance.h
	albaInteractor2DMeasure_MidPoint.cpp		albaInteractor2DMeasure_MidPoint.h
	albaInteractor2DMeasure_Point.cpp 			albaInteractor2DMeasure_Point.h
	albaInteractor2DMeasure_Segment.cpp			albaInteractor2DMeasure_Segment.h
	albaInteractor2DMeasure_Template.cpp		albaInteractor2DMeasure_Template.h
	
	albaInteractorCompositorMouseFloatVME.cpp albaInteractorCompositorMouseFloatVME.h
	albaInteractorGenericMouseFloatVME.cpp	albaInteractorGenericMouseFloatVME.h
)

IF (ALBA_USE_P5_GLOVE)
  SET(PROJECT_SRCS
  ${PROJECT_SRCS}
  albaDeviceButtonsPadTrackerP5Glove.cpp		albaDeviceButtonsPadTrackerP5Glove.h
  )
ENDIF (ALBA_USE_P5_GLOVE)

IF (ALBA_USE_WIIMOTE)
  SET(PROJECT_SRCS
  ${PROJECT_SRCS}
  albaDeviceTrackerWIIMote.cpp				albaDeviceTrackerWIIMote.h
  )
ENDIF (ALBA_USE_WIIMOTE)



IF (WIN32)
  OPTION(ALBA_USE_P5_GLOVE "Link driver for P5 glove device" OFF)
  OPTION(ALBA_USE_WIIMOTE "Link driver for WIIMote device" OFF)
ENDIF (WIN32)

# currently special drivers only supported under WIN32
#IF (WIN32)
 SUBDIRS(Drivers)
#ENDIF (WIN32)

# List libraries that are needed by this project.
SET(PROJECT_LIBS albaBase albaIO albaGui vtkALBA ${wxWidgets_LIBRARIES})

# currently special drivers only supported under WIN32
#IF (WIN32)
  SET(PROJECT_LIBS ${PROJECT_LIBS} albaDrivers)
#ENDIF (WIN32)

#IF (ALBA_USE_VTK)
  SET(PROJECT_LIBS ${PROJECT_LIBS} vtkHybrid)
#ENDIF (ALBA_USE_VTK)

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})

ADD_DEPENDENCIES(${PROJECT_NAME} albaCommon albaVME vtkALBA albaService)

IF (ALBA_USE_WX)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_WXWIN_LIBRARY)
ENDIF (ALBA_USE_WX)
