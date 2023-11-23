#
# Copyright and description to be placed here
#

PROJECT(albaOperations)

DBG_MESSAGE("Configuring project albaOperations")

# Set your list of sources here.
SET(PROJECT_SRCS)

IF (ALBA_USE_VTK)
  SET(PROJECT_SRCS
	${PROJECT_SRCS}
	albaOpDecomposeTimeVarVME.cpp	    albaOpDecomposeTimeVarVME.h
	albaOpImporterMSF.cpp							albaOpImporterMSF.h
	albaOpImporterExternalFile.cpp	    albaOpImporterExternalFile.h
	albaOpLabelExtractor.cpp	    			albaOpLabelExtractor.h
	albaOpOpenExternalFile.cpp	    		albaOpOpenExternalFile.h
	albaOp2DMeasure.cpp	    					albaOp2DMeasure.h
	albaOpMeasure2D.cpp	    					albaOpMeasure2D.h
	albaOpAddLandmark.cpp	    				albaOpAddLandmark.h
	albaOpExporterBmp.cpp	    				albaOpExporterBmp.h
	albaOpClipSurface.cpp	    				albaOpClipSurface.h
	albaOpCreateGroup.cpp	    				albaOpCreateGroup.h
	albaOpCreateInfoText.cpp					albaOpCreateInfoText.h
	albaOpCreateMeter.cpp	    				albaOpCreateMeter.h
	albaOpCreateSurfaceParametric.cpp	albaOpCreateSurfaceParametric.h
	albaOpCreateProber.cpp	    				albaOpCreateProber.h
	albaOpCreateRefSys.cpp	    				albaOpCreateRefSys.h
	albaOpCreateRefSysFromViewSlice.cpp	albaOpCreateRefSysFromViewSlice.h
	albaOpCreateSlicer.cpp	    				albaOpCreateSlicer.h
	albaOpCreateSpline.cpp	    				albaOpCreateSpline.h
	albaOpCreateVolume.cpp	    				albaOpCreateVolume.h
	albaOpCreateAverageLandmark.cpp		albaOpCreateAverageLandmark.h
	albaOpVolumeUnion.cpp	    				albaOpVolumeUnion.h
	albaOpRemoveCells.cpp	    				albaOpRemoveCells.h
	albaOpEditMetadata.cpp	    				albaOpEditMetadata.h
	albaOpExtractIsosurface.cpp	    	albaOpExtractIsosurface.h
	albaOpFilterSurface.cpp	    			albaOpFilterSurface.h
	albaOpDecimateSurface.cpp	    		albaOpDecimateSurface.h
	albaOpConnectivitySurface.cpp	    albaOpConnectivitySurface.h
	albaOpFilterVolume.cpp	    				albaOpFilterVolume.h
	albaOpImporterImage.cpp	 		   		albaOpImporterImage.h
	albaOpTransformOld.cpp	   			 		albaOpTransformOld.h
	albaOpExporterMSF.cpp	   			 		albaOpExporterMSF.h
	albaOpImporterMSF1x.cpp	    			albaOpImporterMSF1x.h
	albaOpExporterRaw.cpp	    				albaOpExporterRaw.h
	albaOpImporterRAWVolume.cpp	    	albaOpImporterRAWVolume.h
	albaOpReparentTo.cpp	    					albaOpReparentTo.h
	albaOpExporterSTL.cpp	  		  		albaOpExporterSTL.h
	albaOpExporterPLY.cpp	  		  		albaOpExporterPLY.h
	albaOpImporterSTL.cpp	  		  		albaOpImporterSTL.h
	albaOpImporterPLY.cpp	  		  		albaOpImporterPLY.h
	albaOpTransformInterface.cpp	    	albaOpTransformInterface.h
	albaOpValidateTree.cpp	    				albaOpValidateTree.h
	albaOpVOIDensity.cpp	    					albaOpVOIDensity.h
	albaOpVOIDensityEditor.cpp		    	albaOpVOIDensityEditor.h
	albaOpImporterVRML.cpp	  		  		albaOpImporterVRML.h
	albaOpExporterVTK.cpp	  		  		albaOpExporterVTK.h
	albaOpImporterVTK.cpp	  		  		albaOpImporterVTK.h
	albaOpExporterMetaImage.cpp	  		  		albaOpExporterMetaImage.h
	albaOpImporterMetaImage.cpp	  		  		albaOpImporterMetaImage.h
	albaOpCrop.cpp	    								albaOpCrop.h
	albaOpBooleanSurface.cpp	    			albaOpBooleanSurface.h
	albaOpEditNormals.cpp	    				albaOpEditNormals.h
	albaOpApplyTrajectory.cpp	   		 	albaOpApplyTrajectory.h
	albaOpGarbageCollectMSFDir.cpp			albaOpGarbageCollectMSFDir.h
	albaOpVolumeMirror.cpp							albaOpVolumeMirror.h
	###############################
	
	albaOpImporterAnsysCommon.cpp	albaOpImporterAnsysCommon.h	
	albaOpImporterAnsysInputFile.cpp	albaOpImporterAnsysInputFile.h
	albaOpImporterAnsysCDBFile.cpp	albaOpImporterAnsysCDBFile.h
	
	albaOpExporterAnsysCommon.cpp	albaOpExporterAnsysCommon.h
	albaOpExporterAnsysInputFile.cpp	albaOpExporterAnsysInputFile.h
	albaOpExporterAnsysCDBFile.cpp	albaOpExporterAnsysCDBFile.h
	
	albaOpImporterAbaqusFile.cpp	albaOpImporterAbaqusFile.h
	albaOpExporterAbaqusFile.cpp	albaOpExporterAbaqusFile.h
	
	albaGeometryEditorPolylineGraph.cpp	albaGeometryEditorPolylineGraph.h
	albaOpCreateLabeledVolume.cpp	albaOpCreateLabeledVolume.h
	albaOpImporterLandmark.cpp		albaOpImporterLandmark.h
	albaOpImporterLandmarkTXT.cpp	albaOpImporterLandmarkTXT.h
	albaOpImporterLandmarkWS.cpp		albaOpImporterLandmarkWS.h

	albaOpMatrixVectorMath.cpp		albaOpMatrixVectorMath.h

	albaOpMML.cpp  albaOpMML.h
	albaOpMMLContourWidget.cpp	albaOpMMLContourWidget.h
	albaOpMMLModelView.cpp		albaOpMMLModelView.h
	albaOpMMLParameterView.cpp	albaOpMMLParameterView.h

	albaOpMML3.cpp  albaOpMML3.h
	albaOpMML3ContourWidget.cpp	albaOpMML3ContourWidget.h
	albaOpMML3GlobalRegistration.cpp	albaOpMML3GlobalRegistration.h
	albaOpMML3ModelView.cpp	albaOpMML3ModelView.h
	albaOpMML3ModelView2DPipe.cpp	albaOpMML3ModelView2DPipe.h
	albaOpMML3ModelView3DPipe.cpp	albaOpMML3ModelView3DPipe.h
	albaOpMML3NonUniformSlicePipe.cpp	albaOpMML3NonUniformSlicePipe.h
	albaOpMML3ParameterView.cpp	albaOpMML3ParameterView.h

	albaOpRegisterClusters.cpp	albaOpRegisterClusters.h
	albaOpVolumeMeasure.cpp		albaOpVolumeMeasure.h
	albaOpImporterRAWImages.cpp	albaOpImporterRAWImages.h
	albaOpMeshQuality.cpp		albaOpMeshQuality.h
	albaOpCropDeformableROI.cpp	albaOpCropDeformableROI.h
	albaOpFlipNormals.cpp		albaOpFlipNormals.h
	albaOpExtrusionHoles.cpp		albaOpExtrusionHoles.h
	albaOpVolumeResample.cpp		albaOpVolumeResample.h
	albaOpSplitSurface.cpp		albaOpSplitSurface.h
	albaOpInteractiveClipSurface.cpp	albaOpInteractiveClipSurface.h
	albaOpCreateEditSkeleton.cpp	albaOpCreateEditSkeleton.h
	albaOpCreateSurface.cpp		albaOpCreateSurface.h
	albaOpTransform.cpp			albaOpTransform.h
	albaOpMove.cpp				albaOpMove.h
	albaOpScaleDataset.cpp		albaOpScaleDataset.h
	albaOpCreateWrappedMeter.cpp

	albaOpSegmentation.cpp  albaOpSegmentation.h
	albaOpSegmentationHelper.cpp  albaOpSegmentationHelper.h


	albaOpExtractGeometry.cpp  albaOpExtractGeometry.h  
	albaOpCreateWrappedMeter.cpp	albaOpCreateWrappedMeter.h  
	albaOpComputeWrapping.cpp	albaOpComputeWrapping.h    
	albaOpComputeInertialTensor.cpp	albaOpComputeInertialTensor.h    
	albaOpExporterWrappedMeter.cpp	albaOpExporterWrappedMeter.h
	albaOpLabelizeSurface.cpp  albaOpLabelizeSurface.h
	albaOpFreezeVME.cpp  albaOpFreezeVME.h
  
	albaOpIterativeRegistration.cpp  albaOpIterativeRegistration.h
  
	albaOpSmoothSurface.cpp  albaOpSmoothSurface.h  
	albaOpCleanSurface.cpp  albaOpCleanSurface.h  
	albaOpTriangulateSurface.cpp  albaOpTriangulateSurface.h

	albaOpSurfaceMirror.cpp  albaOpSurfaceMirror.h  
	albaOpImporterVTKXML.cpp  albaOpImporterVTKXML.h  
	albaOpExporterVTKXML.cpp  albaOpExporterVTKXML.h
	albaOpSubdivide.cpp  albaOpSubdivide.h
    
	albaOpFillHoles.cpp  albaOpFillHoles.h
	albaOpMeshDeformation.cpp  albaOpMeshDeformation.h
  
	albaOpMakeVMETimevarying.cpp  albaOpMakeVMETimevarying.h
	albaOpSegmentationRegionGrowingConnectedThreshold.cpp  albaOpSegmentationRegionGrowingConnectedThreshold.h
	albaOpSegmentationRegionGrowingLocalAndGlobalThreshold.cpp	albaOpSegmentationRegionGrowingLocalAndGlobalThreshold.h
	  
	albaOpEqualizeHistogram.cpp  albaOpEqualizeHistogram.h
	  
	albaOpExporterAnalogWS.cpp		albaOpExporterAnalogWS.h
	albaOpExporterGRFWS.cpp		albaOpExporterGRFWS.h
	albaOpExporterLandmarkWS.cpp	albaOpExporterLandmarkWS.h
	  
	albaOpExporterFEMCommon.cpp	albaOpExporterFEMCommon.h
	
	albaOpESPCalibration.cpp		albaOpESPCalibration.h
	
	albaOpExtractImageFromArbitraryView.cpp		albaOpExtractImageFromArbitraryView.h
	albaOpImporterDicFile.cpp		albaOpImporterDicFile.h
	albaOpImporterPointCloud.cpp	albaOpImporterPointCloud.h
	albaOpCreateProsthesis.cpp		albaOpCreateProsthesis.h
	albaOpImportProsthesisToDB.cpp  albaOpImportProsthesisToDB.h
	
	albaComputeHausdorffDistance.cpp	albaComputeHausdorffDistance.h
	albaOpComputeHausdorffDistance.cpp albaOpComputeHausdorffDistance.h
	
	albaOpCreatePolyline.cpp	albaOpCreatePolyline.h
	
	albaOpImporterEmgWS.cpp albaOpImporterEmgWS.h
	albaOpImporterRawMotionData.cpp albaOpImporterRawMotionData.h
	albaOpInteractorSliderSample.cpp albaOpInteractorSliderSample.h
	albaOpShowHistory.cpp	albaOpShowHistory.h
  )
ENDIF (ALBA_USE_VTK)

IF (ALBA_USE_GDCM)
SET(PROJECT_SRCS ${PROJECT_SRCS}
	albaOpExporterDicom.cpp albaOpExporterDicom.h
    albaOpImporterDicom.cpp	albaOpImporterDicom.h
		albaOpImporterDicomSliceAccHelper.cpp albaOpImporterDicomSliceAccHelper.h
		albaDicomTagDefines.h
    )
ENDIF (ALBA_USE_GDCM)

IF(ALBA_USE_BTK)
	 SET(PROJECT_SRCS ${PROJECT_SRCS} albaOpImporterC3D.cpp			albaOpImporterC3D.h)
ENDIF(ALBA_USE_BTK)

IF (ALBA_USE_ITK)
  SET(PROJECT_SRCS
    ${PROJECT_SRCS}
    albaASCIIImporterUtility.cpp			albaASCIIImporterUtility.h
    albaOpImporterASCII.cpp				albaOpImporterASCII.h
    albaVMEDataSetAttributesImporter.cpp	albaVMEDataSetAttributesImporter.h
    albaVMEMeshAnsysTextImporter.cpp		albaVMEMeshAnsysTextImporter.h
    albaVMEMeshAnsysTextExporter.cpp		albaVMEMeshAnsysTextExporter.h
    albaParabolicMeshToLinearMeshFilter.cxx	albaParabolicMeshToLinearMeshFilter.h
    albaOpImporterMesh.cpp				albaOpImporterMesh.h
    albaOpExporterMesh.cpp				albaOpExporterMesh.h
    albaOpScalarToSurface.cpp			albaOpScalarToSurface.h
    albaOpImporterVMEDataSetAttributes.cpp	albaOpImporterVMEDataSetAttributes.h
	
	##########################################
	# itkRawMotionImporterUtility.cxx
    # itkRawMotionImporterUtility.h
    albaOpImporterMotionData.h
    albaOpExporterLandmark.cpp		albaOpExporterLandmark.h
    albaOpClassicICPRegistration.cpp	albaOpClassicICPRegistration.h
    albaOpImporterGRFWS.cpp			albaOpImporterGRFWS.h
    albaOpImporterAnalogWS.cpp		albaOpImporterAnalogWS.h
    albaOpSmoothSurfaceCells.cpp		albaOpSmoothSurfaceCells.h
    albaOpExporterMeters.cpp			albaOpExporterMeters.h
    albaOpInteractionDebugger.cpp	albaOpInteractionDebugger.h
	albaOpFilterImage.cpp			albaOpFilterImage.h
	albaOpCreateAveragePlane.cpp	albaOpCreateAveragePlane.h
  )
ENDIF (ALBA_USE_ITK)

# List libraries that are needed by this project.
SET(PROJECT_LIBS "albaVME" "albaInteraction" ${wxWidgets_LIBRARIES})

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})

IF (ALBA_USE_VTK)
  SET(PROJECT_LIBS ${PROJECT_LIBS} albaCommon vtkCommon vtkALBA)
ENDIF (ALBA_USE_VTK)

IF (ALBA_USE_GDCM)
 ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_GDCM_LIBRARY)
 SET(PROJECT_LIBS ${PROJECT_LIBS} ${GDCM_LIBRARIES})
ENDIF (ALBA_USE_GDCM)

IF (ALBA_USE_BTK)
 ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_BTK_LIBRARY)
ENDIF (ALBA_USE_BTK)


ADD_DEPENDENCIES(${PROJECT_NAME} albaVME albaCommon vtkALBA albaService)

IF (ALBA_USE_VTK)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_VTK_LIBRARY)
ENDIF (ALBA_USE_VTK)

IF (ALBA_USE_WX)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_WXWIN_LIBRARY)
ENDIF (ALBA_USE_WX)


TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})

