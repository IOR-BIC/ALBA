#
# Copyright and description to be placed here
#

PROJECT(mafOperations)

DBG_MESSAGE("Configuring project mafOperations")

# Set your list of sources here.
SET(PROJECT_SRCS)

IF (MAF_USE_VTK)
  SET(PROJECT_SRCS
	${PROJECT_SRCS}
	mafOpDecomposeTimeVarVME.cpp	    mafOpDecomposeTimeVarVME.h
	mafOpImporterMSF.cpp							mafOpImporterMSF.h
	mafOpImporterExternalFile.cpp	    mafOpImporterExternalFile.h
	mafOpLabelExtractor.cpp	    			mafOpLabelExtractor.h
	mafOpOpenExternalFile.cpp	    		mafOpOpenExternalFile.h
	mafOp2DMeasure.cpp	    					mafOp2DMeasure.h
	mafOpAddLandmark.cpp	    				mafOpAddLandmark.h
	mafOpExporterBmp.cpp	    				mafOpExporterBmp.h
	mafOpClipSurface.cpp	    				mafOpClipSurface.h
	mafOpCreateGroup.cpp	    				mafOpCreateGroup.h
	mafOpCreateMeter.cpp	    				mafOpCreateMeter.h
	mafOpCreateSurfaceParametric.cpp	mafOpCreateSurfaceParametric.h
	mafOpCreateProber.cpp	    				mafOpCreateProber.h
	mafOpCreateRefSys.cpp	    				mafOpCreateRefSys.h
	mafOpCreateSlicer.cpp	    				mafOpCreateSlicer.h
	mafOpCreateSpline.cpp	    				mafOpCreateSpline.h
	mafOpCreateVolume.cpp	    				mafOpCreateVolume.h
	mafOpVolumeUnion.cpp	    				mafOpVolumeUnion.h
	mafOpRemoveCells.cpp	    				mafOpRemoveCells.h
	mafOpEditMetadata.cpp	    				mafOpEditMetadata.h
	mafOpExtractIsosurface.cpp	    	mafOpExtractIsosurface.h
	mafOpFilterSurface.cpp	    			mafOpFilterSurface.h
	mafOpDecimateSurface.cpp	    		mafOpDecimateSurface.h
	mafOpConnectivitySurface.cpp	    mafOpConnectivitySurface.h
	mafOpFilterVolume.cpp	    				mafOpFilterVolume.h
	mafOpImporterImage.cpp	 		   		mafOpImporterImage.h
	mafOpMAFTransform.cpp	   			 		mafOpMAFTransform.h
	mafOpExporterMSF.cpp	   			 		mafOpExporterMSF.h
	mafOpImporterMSF1x.cpp	    			mafOpImporterMSF1x.h
	mafOpExporterRaw.cpp	    				mafOpExporterRaw.h
	mafOpImporterRAWVolume.cpp	    	mafOpImporterRAWVolume.h
	mafOpReparentTo.cpp	    					mafOpReparentTo.h
	mafOpExporterSTL.cpp	  		  		mafOpExporterSTL.h
	mafOpImporterSTL.cpp	  		  		mafOpImporterSTL.h
	mafOpTransformInterface.cpp	    	mafOpTransformInterface.h
	mafOpValidateTree.cpp	    				mafOpValidateTree.h
	mafOpVOIDensity.cpp	    					mafOpVOIDensity.h
	mafOpVOIDensityEditor.cpp		    	mafOpVOIDensityEditor.h
	mafOpImporterVRML.cpp	  		  		mafOpImporterVRML.h
	mafOpExporterVTK.cpp	  		  		mafOpExporterVTK.h
	mafOpImporterVTK.cpp	  		  		mafOpImporterVTK.h
	mafOpCrop.cpp	    								mafOpCrop.h
	mafOpBooleanSurface.cpp	    			mafOpBooleanSurface.h
	mafOpEditNormals.cpp	    				mafOpEditNormals.h
	mafOpApplyTrajectory.cpp	   		 	mafOpApplyTrajectory.h
	mafOpGarbageCollectMSFDir.cpp			mafOpGarbageCollectMSFDir.h
	mafOpVolumeMirror.cpp							mafOpVolumeMirror.h
	###############################
	
	mafOpImporterAnsysCommon.cpp	mafOpImporterAnsysCommon.h	
	mafOpImporterAnsysInputFile.cpp	mafOpImporterAnsysInputFile.h
	mafOpImporterAnsysCDBFile.cpp	mafOpImporterAnsysCDBFile.h
	
	mafOpExporterAnsysCommon.cpp	mafOpExporterAnsysCommon.h
	mafOpExporterAnsysInputFile.cpp	mafOpExporterAnsysInputFile.h
	mafOpExporterAnsysCDBFile.cpp	mafOpExporterAnsysCDBFile.h
	
	mafGeometryEditorPolylineGraph.cpp	mafGeometryEditorPolylineGraph.h
	mafOpCreateLabeledVolume.cpp	mafOpCreateLabeledVolume.h
	mafOpImporterLandmark.cpp		mafOpImporterLandmark.h
	mafOpImporterLandmarkTXT.cpp	mafOpImporterLandmarkTXT.h
	mafOpImporterLandmarkWS.cpp		mafOpImporterLandmarkWS.h

	mafOpMatrixVectorMath.cpp		mafOpMatrixVectorMath.h

	mafOpMML.cpp  mafOpMML.h
	mafOpMMLContourWidget.cpp	mafOpMMLContourWidget.h
	mafOpMMLModelView.cpp		mafOpMMLModelView.h
	mafOpMMLParameterView.cpp	mafOpMMLParameterView.h

	mafOpMML3.cpp  mafOpMML3.h
	mafOpMML3ContourWidget.cpp	mafOpMML3ContourWidget.h
	mafOpMML3GlobalRegistration.cpp	mafOpMML3GlobalRegistration.h
	mafOpMML3ModelView.cpp	mafOpMML3ModelView.h
	mafOpMML3ModelView2DPipe.cpp	mafOpMML3ModelView2DPipe.h
	mafOpMML3ModelView3DPipe.cpp	mafOpMML3ModelView3DPipe.h
	mafOpMML3NonUniformSlicePipe.cpp	mafOpMML3NonUniformSlicePipe.h
	mafOpMML3ParameterView.cpp	mafOpMML3ParameterView.h

	mafOpRegisterClusters.cpp	mafOpRegisterClusters.h
	mafOpVolumeMeasure.cpp		mafOpVolumeMeasure.h
	mafOpImporterRAWImages.cpp	mafOpImporterRAWImages.h
	mafOpMeshQuality.cpp		mafOpMeshQuality.h
	mafOpCropDeformableROI.cpp	mafOpCropDeformableROI.h
	mafOpFlipNormals.cpp		mafOpFlipNormals.h
	mafOpExtrusionHoles.cpp		mafOpExtrusionHoles.h
	mafOpVolumeResample.cpp		mafOpVolumeResample.h
	mafOpSplitSurface.cpp		mafOpSplitSurface.h
	mafOpInteractiveClipSurface.cpp	mafOpInteractiveClipSurface.h
	mafOpCreateEditSkeleton.cpp	mafOpCreateEditSkeleton.h
	mafOpCreateSurface.cpp		mafOpCreateSurface.h
	mafOpTransform.cpp			mafOpTransform.h
	mafOpMove.cpp				mafOpMove.h
	mafOpScaleDataset.cpp		mafOpScaleDataset.h

	mafOpCreateWrappedMeter.cpp
	mafOpSegmentation.cpp  mafOpSegmentation.h
	mafOpExtractGeometry.cpp  mafOpExtractGeometry.h  
	mafOpCreateWrappedMeter.cpp	mafOpCreateWrappedMeter.h  
	mafOpComputeWrapping.cpp	mafOpComputeWrapping.h    
	mafOpComputeInertialTensor.cpp	mafOpComputeInertialTensor.h    
	mafOpExporterWrappedMeter.cpp	mafOpExporterWrappedMeter.h
	mafOpLabelizeSurface.cpp  mafOpLabelizeSurface.h
	mafOpFreezeVME.cpp  mafOpFreezeVME.h
  
	mafOpIterativeRegistration.cpp  mafOpIterativeRegistration.h
  
	mafOpSmoothSurface.cpp  mafOpSmoothSurface.h  
	mafOpCleanSurface.cpp  mafOpCleanSurface.h  
	mafOpTriangulateSurface.cpp  mafOpTriangulateSurface.h

	mafOpSurfaceMirror.cpp  mafOpSurfaceMirror.h  
	mafOpImporterVTKXML.cpp  mafOpImporterVTKXML.h  
	mafOpExporterVTKXML.cpp  mafOpExporterVTKXML.h
	mafOpSubdivide.cpp  mafOpSubdivide.h
    
	mafOpFillHoles.cpp  mafOpFillHoles.h
	mafOpMeshDeformation.cpp  mafOpMeshDeformation.h
  
	mafOpMakeVMETimevarying.cpp  mafOpMakeVMETimevarying.h
	mafOpSegmentationRegionGrowingConnectedThreshold.cpp  mafOpSegmentationRegionGrowingConnectedThreshold.h
	mafOpSegmentationRegionGrowingLocalAndGlobalThreshold.cpp	mafOpSegmentationRegionGrowingLocalAndGlobalThreshold.h
	  
	mafOpEqualizeHistogram.cpp  mafOpEqualizeHistogram.h
	  
	mafOpExporterAnalogWS.cpp		mafOpExporterAnalogWS.h
	mafOpExporterGRFWS.cpp		mafOpExporterGRFWS.h
	mafOpExporterLandmarkWS.cpp	mafOpExporterLandmarkWS.h
	  
	mafOpExporterFEMCommon.cpp	mafOpExporterFEMCommon.h
  )
ENDIF (MAF_USE_VTK)

IF (MAF_USE_GDCM)
SET(PROJECT_SRCS ${PROJECT_SRCS}
    mafOpImporterDicom.cpp	mafOpImporterDicom.h
		mafDicomTagDefines.h
    )
ENDIF (MAF_USE_GDCM)

IF(MAF_USE_BTK)
	 SET(PROJECT_SRCS ${PROJECT_SRCS} mafOpImporterC3D.cpp			mafOpImporterC3D.h)
ENDIF(MAF_USE_BTK)

IF (MAF_USE_ITK)
  SET(PROJECT_SRCS
    ${PROJECT_SRCS}
    mafASCIIImporterUtility.cpp			mafASCIIImporterUtility.h
    mafOpImporterASCII.cpp				mafOpImporterASCII.h
    mafVMEDataSetAttributesImporter.cpp	mafVMEDataSetAttributesImporter.h
    mafVMEMeshAnsysTextImporter.cpp		mafVMEMeshAnsysTextImporter.h
    mafVMEMeshAnsysTextExporter.cpp		mafVMEMeshAnsysTextExporter.h
    mafParabolicMeshToLinearMeshFilter.cxx	mafParabolicMeshToLinearMeshFilter.h
    mafOpImporterMesh.cpp				mafOpImporterMesh.h
    mafOpExporterMesh.cpp				mafOpExporterMesh.h
    mafOpScalarToSurface.cpp			mafOpScalarToSurface.h
    mafOpImporterVMEDataSetAttributes.cpp	mafOpImporterVMEDataSetAttributes.h
	
	##########################################
	# itkRawMotionImporterUtility.cxx
    # itkRawMotionImporterUtility.h
    mafOpImporterMotionData.h
    mafOpExporterLandmark.cpp		mafOpExporterLandmark.h
    mafOpClassicICPRegistration.cpp	mafOpClassicICPRegistration.h
    mafOpImporterGRFWS.cpp			mafOpImporterGRFWS.h
    mafOpImporterAnalogWS.cpp		mafOpImporterAnalogWS.h
    mafOpSmoothSurfaceCells.cpp		mafOpSmoothSurfaceCells.h
    mafOpExporterMeters.cpp			mafOpExporterMeters.h
    mafOpInteractionDebugger.cpp	mafOpInteractionDebugger.h
  )
ENDIF (MAF_USE_ITK)

# List libraries that are needed by this project.
SET(PROJECT_LIBS "mafVME" "mafInteraction")

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})

IF (MAF_USE_VTK)
  SET(PROJECT_LIBS ${PROJECT_LIBS} mafCommon vtkCommon vtkMAF)
ENDIF (MAF_USE_VTK)

IF (MAF_USE_GDCM)
 ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_GDCM_LIBRARY)
 SET(PROJECT_LIBS ${PROJECT_LIBS} ${GDCM_LIBRARIES})
ENDIF (MAF_USE_GDCM)

IF (MAF_USE_BTK)
 ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_BTK_LIBRARY)
ENDIF (MAF_USE_BTK)


ADD_DEPENDENCIES(${PROJECT_NAME} mafCommon vtkMAF mafService)

IF (MAF_USE_VTK)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_VTK_LIBRARY)
ENDIF (MAF_USE_VTK)

IF (MAF_USE_WX)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_WXWIN_LIBRARY)
ENDIF (MAF_USE_WX)


TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})

