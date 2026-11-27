#
# Copyright and description to be placed here
#

PROJECT(albaInteraction)

DBG_MESSAGE("Configuring project ${PROJECT_NAME}")

# Set your list of sources here.
SET(PROJECT_SRCS
  albaAgent.cpp  							albaAgent.h
  albaGizmoAutoscaleHelper.cpp  				albaGizmoAutoscaleHelper.h
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
  albaGizmoInterfaceGUI.cpp  				albaGizmoInterfaceGUI.h
  albaGizmoRotateGUI.cpp  					albaGizmoRotateGUI.h
  albaGizmoScaleGUI.cpp  					albaGizmoScaleGUI.h
  albaGizmoTranslateGUI.cpp  				albaGizmoTranslateGUI.h
  albaSaveRestorePoseGUI.cpp  				albaSaveRestorePoseGUI.h
  albaTransformInterfaceGUI.cpp  			albaTransformInterfaceGUI.h
  albaRotateTranslateGUI.cpp  				albaRotateTranslateGUI.h
  albaTransformTextEntriesGUI.cpp  			albaTransformTextEntriesGUI.h
  albaInteractionManager.cpp  				albaInteractionManager.h
  albaInteractionBehavior.cpp  						albaInteractionBehavior.h
  albaInteractionAgent.cpp  						albaInteractionAgent.h
  albaInteractionTool.cpp  						albaInteractionTool.h
  albaToolCamera.cpp  						albaToolCamera.h
  albaToolSegmentation.cpp  					albaToolSegmentation.h
  albaToolSegmentationThreshold.cpp  					albaToolSegmentationThreshold.h
  albaToolSegmentationEdit.cpp  					albaToolSegmentationEdit.h
  albaToolSelectCell.cpp  					albaToolSelectCell.h
  albaToolSelectPoint.cpp  					albaToolSelectPoint.h
  albaToolVTKPicker.cpp  					albaToolVTKPicker.h
  albaToolExtractIsosurface.cpp  					albaToolExtractIsosurface.h
  albaToolDICOMImporter.cpp  					albaToolDICOMImporter.h
  albaRefSys.cpp  							albaRefSys.h  
  albaBehaviorRotateTranslate.cpp  					albaBehaviorRotateTranslate.h
  albaBehaviorProsthesis.cpp  					albaBehaviorProsthesis.h
  albaBehaviorCutPlane.cpp  					albaBehaviorCutPlane.h
  albaTransformConstraint.cpp  				albaTransformConstraint.h
  albaBehaviorTransform.cpp  			albaBehaviorTransform.h
  albaPositionalEventRouter.cpp  					albaPositionalEventRouter.h
  albaBehaviorPicker.cpp  					albaBehaviorPicker.h
  albaToolPicker.cpp  						albaToolPicker.h
  albaToolFlyTo.cpp  						albaToolFlyTo.h
  albaToolSelect.cpp  						albaToolSelect.h
  albaPickHelper.cpp  											albaPickHelper.h
  albaGizmoPolylineGraph.cpp  						albaGizmoPolylineGraph.h
  albaGizmoCrossRotateTranslate.cpp  			albaGizmoCrossRotateTranslate.h	  
  albaGizmoCrossRotate.cpp  							albaGizmoCrossRotate.h
  albaGizmoCrossRotateAxis.cpp  					albaGizmoCrossRotateAxis.h
  albaGizmoCrossRotateFan.cpp  						albaGizmoCrossRotateFan.h
  albaGizmoCrossTranslate.cpp  						albaGizmoCrossTranslate.h
  albaGizmoCrossTranslateAxis.cpp  				albaGizmoCrossTranslateAxis.h
  albaGizmoCrossTranslatePlane.cpp  			albaGizmoCrossTranslatePlane.h
	
	albaActor2dStackHelper.cpp							albaActor2dStackHelper.h
	albaTool2DMeasure.cpp							albaTool2DMeasure.h
	albaTool2DMeasureAngle.cpp				albaTool2DMeasureAngle.h
	albaTool2DMeasureAngleLine.cpp		albaTool2DMeasureAngleLine.h
	albaTool2DMeasureCenterPoint.cpp	albaTool2DMeasureCenterPoint.h
	albaTool2DMeasureDistance.cpp 		albaTool2DMeasureDistance.h
	albaTool2DMeasureLine.cpp				albaTool2DMeasureLine.h
	albaTool2DMeasureLineDistance.cpp	albaTool2DMeasureLineDistance.h
	albaTool2DMeasureMidPoint.cpp		albaTool2DMeasureMidPoint.h
	albaTool2DMeasurePoint.cpp 			albaTool2DMeasurePoint.h
	albaTool2DMeasureSegment.cpp			albaTool2DMeasureSegment.h
	albaTool2DMeasureTemplate.cpp		albaTool2DMeasureTemplate.h
	
	albaTool2DMeasureEllipse.cpp albaTool2DMeasureEllipse.h
)

# List libraries that are needed by this project.
SET(PROJECT_LIBS albaBase albaIO albaGui vtkALBA ${wxWidgets_LIBRARIES})

#IF (ALBA_USE_VTK)
  SET(PROJECT_LIBS ${PROJECT_LIBS} ${VTK_LIBRARIES})
#ENDIF (ALBA_USE_VTK)

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})

ADD_DEPENDENCIES(${PROJECT_NAME} albaCommon albaVME vtkALBA albaService)
