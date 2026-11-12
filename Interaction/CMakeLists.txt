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
  albaInteractionAgent.cpp  						albaInteractionAgent.h
  albaInteractionTool.cpp  						albaInteractionTool.h
  albaCameraTool.cpp  						albaCameraTool.h
  albaCameraMouseTool.cpp  						albaCameraMouseTool.h
  albaSegmentationTool.cpp  					albaSegmentationTool.h
  albaSegmentationThresholdTool.cpp  					albaSegmentationThresholdTool.h
  albaSegmentationEditTool.cpp  					albaSegmentationEditTool.h
  albaSelectCellTool.cpp  					albaSelectCellTool.h
  albaSelectPointTool.cpp  					albaSelectPointTool.h
  albaVTKPickerTool.cpp  					albaVTKPickerTool.h
  albaExtractIsosurfaceTool.cpp  					albaExtractIsosurfaceTool.h
  albaDICOMImporterTool.cpp  					albaDICOMImporterTool.h
  albaCameraTrackerTool.cpp  						albaCameraTrackerTool.h
  albaRefSys.cpp  							albaRefSys.h  
  albaDeviceButtonsPad.cpp  					albaDeviceButtonsPad.h
  albaDeviceButtonsPadMouse.cpp  			albaDeviceButtonsPadMouse.h
  albaDeviceButtonsPadTracker.cpp  			albaDeviceButtonsPadTracker.h  
  albaInteractorCompositorMouse.cpp  		albaInteractorCompositorMouse.h
  albaInteractorConstraint.cpp  				albaInteractorConstraint.h
  albaInteractorGenericMouse.cpp  			albaInteractorGenericMouse.h
  albaInteractorPER.cpp  					albaInteractorPER.h
  albaInteractorPicker.cpp  					albaInteractorPicker.h
  albaInteractorSER.cpp  									albaInteractorSER.h  ######################
  albaPickerTool.cpp  						albaPickerTool.h
  albaPickHelper.cpp  											albaPickHelper.h
  albaGizmoPolylineGraph.cpp  						albaGizmoPolylineGraph.h
  albaGUITransformSliders.cpp  						albaGUITransformSliders.h    
  albaGizmoCrossRotateTranslate.cpp  			albaGizmoCrossRotateTranslate.h	  
  albaGizmoCrossRotate.cpp  							albaGizmoCrossRotate.h
  albaGizmoCrossRotateAxis.cpp  					albaGizmoCrossRotateAxis.h
  albaGizmoCrossRotateFan.cpp  						albaGizmoCrossRotateFan.h
  albaGizmoCrossTranslate.cpp  						albaGizmoCrossTranslate.h
  albaGizmoCrossTranslateAxis.cpp  				albaGizmoCrossTranslateAxis.h
  albaGizmoCrossTranslatePlane.cpp  			albaGizmoCrossTranslatePlane.h
	
	albaActor2dStackHelper.cpp							albaActor2dStackHelper.h
	alba2DMeasureTool.cpp							alba2DMeasureTool.h
	alba2DMeasureAngleTool.cpp				alba2DMeasureAngleTool.h
	alba2DMeasureAngleLineTool.cpp		alba2DMeasureAngleLineTool.h
	alba2DMeasureCenterPointTool.cpp	alba2DMeasureCenterPointTool.h
	alba2DMeasureDistanceTool.cpp 		alba2DMeasureDistanceTool.h
	alba2DMeasureLineTool.cpp				alba2DMeasureLineTool.h
	alba2DMeasureLineDistanceTool.cpp	alba2DMeasureLineDistanceTool.h
	alba2DMeasureMidPointTool.cpp		alba2DMeasureMidPointTool.h
	alba2DMeasurePointTool.cpp 			alba2DMeasurePointTool.h
	alba2DMeasureSegmentTool.cpp			alba2DMeasureSegmentTool.h
	alba2DMeasureTemplateTool.cpp		alba2DMeasureTemplateTool.h
	
	albaInteractorCompositorMouseFloatVME.cpp albaInteractorCompositorMouseFloatVME.h
	albaInteractorGenericMouseFloatVME.cpp	albaInteractorGenericMouseFloatVME.h
	alba2DMeasureEllipseTool.cpp alba2DMeasureEllipseTool.h
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
  SET(PROJECT_LIBS ${PROJECT_LIBS} ${VTK_LIBRARIES})
#ENDIF (ALBA_USE_VTK)

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})

ADD_DEPENDENCIES(${PROJECT_NAME} albaCommon albaVME vtkALBA albaService)
