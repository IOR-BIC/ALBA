#
# Copyright and description to be placed here
#

PROJECT(mafOperations)

DBG_MESSAGE("Configuring project mafOperations")

# Set your list of sources here.
SET(PROJECT_SRCS
)

IF (MAF_USE_VTK)
  SET(PROJECT_SRCS
    ${PROJECT_SRCS}
    mafOpDecomposeTimeVarVME.cpp
    mafOpDecomposeTimeVarVME.h
    mafOpImporterMSF.cpp
    mafOpImporterMSF.h
    mafOpImporterExternalFile.cpp
    mafOpImporterExternalFile.h
    mafOpLabelExtractor.cpp
    mafOpLabelExtractor.h
    mafOpOpenExternalFile.cpp
    mafOpOpenExternalFile.h
    mafOp2DMeasure.cpp
    mafOp2DMeasure.h
    mafOpAddLandmark.cpp
    mafOpAddLandmark.h
    mafOpExporterBmp.cpp
    mafOpExporterBmp.h
    mafOpClipSurface.cpp
    mafOpClipSurface.h
    mafOpCreateGroup.cpp
    mafOpCreateGroup.h
    mafOpCreateMeter.cpp
    mafOpCreateMeter.h
    mafOpCreateSurfaceParametric.cpp
    mafOpCreateSurfaceParametric.h
    mafOpCreateProber.cpp
    mafOpCreateProber.h
    mafOpCreateRefSys.cpp
    mafOpCreateRefSys.h
    mafOpCreateSlicer.cpp
    mafOpCreateSlicer.h
    mafOpCreateSpline.cpp
    mafOpCreateSpline.h
    mafOpCreateVolume.cpp
    mafOpCreateVolume.h
	mafOpVolumeUnion.cpp
    mafOpVolumeUnion.h
    mafOpRemoveCells.cpp
    mafOpRemoveCells.h
    mafOpEditMetadata.cpp
    mafOpEditMetadata.h
    mafOpExplodeCollapse.cpp
    mafOpExplodeCollapse.h
    mafOpExtractIsosurface.cpp
    mafOpExtractIsosurface.h
    mafOpFilterSurface.cpp
    mafOpFilterSurface.h
    mafOpDecimateSurface.cpp
    mafOpDecimateSurface.h
    mafOpConnectivitySurface.cpp
    mafOpConnectivitySurface.h
    mafOpFilterVolume.cpp
    mafOpFilterVolume.h
    mafOpImporterImage.cpp
    mafOpImporterImage.h
    mafOpMAFTransform.cpp
    mafOpMAFTransform.h
    mafOpExporterMSF.cpp
    mafOpExporterMSF.h
    mafOpImporterMSF1x.cpp
    mafOpImporterMSF1x.h
    mafOpExporterRaw.cpp
    mafOpExporterRaw.h
    mafOpImporterRAWVolume.cpp
    mafOpImporterRAWVolume.h
    mafOpReparentTo.cpp
    mafOpReparentTo.h
    mafOpExporterSTL.cpp
    mafOpExporterSTL.h
    mafOpImporterSTL.cpp
    mafOpImporterSTL.h
    mafOpTransformInterface.cpp
    mafOpTransformInterface.h
    mafOpValidateTree.cpp
    mafOpValidateTree.h
    mafOpVolumeResample.cpp
    mafOpVolumeResample.h
    mafOpVOIDensity.cpp
    mafOpVOIDensity.h
    mafOpVOIDensityEditor.cpp
    mafOpVOIDensityEditor.h
    mafOpImporterVRML.cpp
    mafOpImporterVRML.h
    mafOpExporterVTK.cpp
    mafOpExporterVTK.h
    mafOpImporterVTK.cpp
    mafOpImporterVTK.h
    mafOpCrop.cpp
    mafOpCrop.h
    mafOpBooleanSurface.cpp
    mafOpBooleanSurface.h
    mafOpEditNormals.cpp
    mafOpEditNormals.h
    mafOpApplyTrajectory.cpp
    mafOpApplyTrajectory.h
	mafOpGarbageCollectMSFDir.cpp
	mafOpGarbageCollectMSFDir.h
	###############################
	
	  medGeometryEditorPolylineGraph.cpp
  medGeometryEditorPolylineGraph.h
  medOpCreateLabeledVolume.cpp
  medOpCreateLabeledVolume.h
  medOpImporterLandmark.cpp
  medOpImporterLandmark.h
  medOpImporterLandmarkTXT.cpp
  medOpImporterLandmarkTXT.h
  medOpImporterLandmarkWS.cpp
  medOpImporterLandmarkWS.h

  medOpMatrixVectorMath.cpp
  medOpMatrixVectorMath.h

  medOpMML.cpp
  medOpMML.h
  medOpMMLContourWidget.cpp
  medOpMMLContourWidget.h
  medOpMMLModelView.cpp
  medOpMMLModelView.h
  medOpMMLParameterView.cpp
  medOpMMLParameterView.h

  medOpMML3.cpp
  medOpMML3.h
  medOpMML3ContourWidget.cpp
  medOpMML3ContourWidget.h
  medOpMML3GlobalRegistration.cpp
  medOpMML3GlobalRegistration.h
  medOpMML3ModelView.cpp
  medOpMML3ModelView.h
  medOpMML3ModelView2DPipe.cpp
  medOpMML3ModelView2DPipe.h
  medOpMML3ModelView3DPipe.cpp
  medOpMML3ModelView3DPipe.h
  medOpMML3NonUniformSlicePipe.cpp
  medOpMML3NonUniformSlicePipe.h
  medOpMML3ParameterView.cpp
  medOpMML3ParameterView.h

  medOpRegisterClusters.cpp
  medOpRegisterClusters.h
  medOpVolumeMeasure.cpp
  medOpVolumeMeasure.h
  medOpImporterRAWImages.cpp
  medOpImporterRAWImages.h
  medOpMeshQuality.cpp
  medOpMeshQuality.h
  medOpCropDeformableROI.cpp
  medOpCropDeformableROI.h
  medOpFlipNormals.cpp
  medOpFlipNormals.h
  medOpExtrusionHoles.cpp
  medOpExtrusionHoles.h
  medOpVolumeResample.cpp
  medOpVolumeResample.h
  medOpSplitSurface.cpp
  medOpSplitSurface.h
  medOpInteractiveClipSurface.cpp
  medOpInteractiveClipSurface.h
  medOpCreateEditSkeleton.cpp
  medOpCreateEditSkeleton.h
  medOpCreateSurface.cpp
  medOpCreateSurface.h
  medOpMove.cpp
  medOpMove.h
  medOpScaleDataset.cpp
  medOpScaleDataset.h

  medOpCreateWrappedMeter.cpp
  medOpSegmentation.cpp
  medOpSegmentation.h
  medOpExtractGeometry.cpp
  medOpExtractGeometry.h
  
  medOpCreateWrappedMeter.cpp
  medOpCreateWrappedMeter.h
  
  medOpComputeWrapping.h
  medOpComputeWrapping.cpp  
  
  medOpComputeInertialTensor.h
  medOpComputeInertialTensor.cpp  
  
  medOpExporterWrappedMeter.cpp
  medOpExporterWrappedMeter.h

  medOpLabelizeSurface.cpp
  medOpLabelizeSurface.h
  medOp2DMeasure.cpp
  medOp2DMeasure.h
  
  medOpFreezeVME.cpp
  medOpFreezeVME.h
  
  medOpIterativeRegistration.cpp
  medOpIterativeRegistration.h
  
  medOpSmoothSurface.cpp
  medOpSmoothSurface.h
  
  medOpCleanSurface.cpp
  medOpCleanSurface.h
  
  medOpTriangulateSurface.cpp
  medOpTriangulateSurface.h

  medOpSurfaceMirror.cpp
  medOpSurfaceMirror.h
  
  medOpImporterVTK.cpp
  medOpImporterVTK.h
  
  medOpImporterVTKXML.cpp
  medOpImporterVTKXML.h
  
  medOpExporterVTKXML.cpp
  medOpExporterVTKXML.h

  medOpSubdivide.cpp
  medOpSubdivide.h
    
  medOpFillHoles.cpp
  medOpFillHoles.h
  medOpMeshDeformation.cpp
  medOpMeshDeformation.h
  
  medOpMakeVMETimevarying.cpp
  medOpMakeVMETimevarying.h
  medOpSegmentationRegionGrowingConnectedThreshold.cpp
  medOpSegmentationRegionGrowingConnectedThreshold.h
  #### VolumeLarge - TO BE COMMITTED DOWN    ####
  mafOpImporterBBF.cpp
  mafOpImporterBBF.h
  mafOpImporterRAWVolume_BES.cpp
  mafOpImporterRAWVolume_BES.h  
  ###############################################
  medOpSegmentationRegionGrowingLocalAndGlobalThreshold.cpp
  medOpSegmentationRegionGrowingLocalAndGlobalThreshold.h
  
  medOpEqualizeHistogram.cpp
  medOpEqualizeHistogram.h
  
  medOpExporterAnalogWS.cpp
  medOpExporterAnalogWS.h
  medOpExporterGRFWS.cpp
  medOpExporterGRFWS.h
  medOpExporterLandmarkWS.cpp
  medOpExporterLandmarkWS.h
  
  medOpMergeDicomSeries.cpp
  medOpMergeDicomSeries.h
  
  )
ENDIF (MAF_USE_VTK)

IF (MED_USE_Offis)
SET(PROJECT_SRCS ${PROJECT_SRCS}
    medOpImporterDicomOffis.h
    medOpImporterDicomOffis.cpp
  
    medDicomCardiacMRIHelper.cpp
    medDicomCardiacMRIHelper.h
   )
ENDIF (MED_USE_Offis)

IF (MAF_USE_ITK)
  SET(PROJECT_SRCS
    ${PROJECT_SRCS}
    mafASCIIImporterUtility.cpp
    mafASCIIImporterUtility.h
    mafOpImporterASCII.cpp
    mafOpImporterASCII.h
    mafVMEDataSetAttributesImporter.cpp
    mafVMEDataSetAttributesImporter.h
    mafVMEMeshAnsysTextImporter.cpp
    mafVMEMeshAnsysTextImporter.h
    mafVMEMeshAnsysTextExporter.cpp
    mafVMEMeshAnsysTextExporter.h
    mafParabolicMeshToLinearMeshFilter.cxx
    mafParabolicMeshToLinearMeshFilter.h
    mafOpImporterMesh.cpp
    mafOpImporterMesh.h
    mafOpExporterMesh.cpp
    mafOpExporterMesh.h
    mafOpScalarToSurface.cpp
    mafOpScalarToSurface.h
    mafOpImporterVMEDataSetAttributes.cpp
    mafOpImporterVMEDataSetAttributes.h
	
	##########################################
	# itkRawMotionImporterUtility.cxx
    # itkRawMotionImporterUtility.h
    medOpImporterMotionData.h
    medOpExporterLandmark.cpp
    medOpExporterLandmark.h
    medOpClassicICPRegistration.cpp
    medOpClassicICPRegistration.h
    medOpImporterC3D.cpp
    medOpImporterC3D.h
    medOpImporterGRFWS.cpp
    medOpImporterGRFWS.h
    medOpImporterAnalogWS.cpp
    medOpImporterAnalogWS.h
    medOpSmoothSurfaceCells.cpp
    medOpSmoothSurfaceCells.h
    medOpExporterMeters.cpp
    medOpExporterMeters.h
    medOpInteractionDebugger.cpp
    medOpInteractionDebugger.h
  )
ENDIF (MAF_USE_ITK)

# List libraries that are needed by this project.
SET(PROJECT_LIBS "mafVME" "mafInteraction")

IF (MAF_USE_VTK)
  SET(PROJECT_LIBS ${PROJECT_LIBS} medCommon vtkCommon vtkMAF vtkMED)
ENDIF (MAF_USE_VTK)

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})

ADD_DEPENDENCIES(${PROJECT_NAME} medCommon vtkMAF vtkMED)

IF (MAF_USE_VTK)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_VTK_LIBRARY)
ENDIF (MAF_USE_VTK)

IF (MAF_USE_WX)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_WXWIN_LIBRARY)
ENDIF (MAF_USE_WX)
