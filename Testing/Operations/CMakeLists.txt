PROJECT (albaOperationsTests)

IF(ALBA_BUILD_ALBADLL)
  SET(PROJECT_LIBS XercesC albaDLL vtkALBA albaTests) 
ELSE(ALBA_BUILD_ALBADLL)
  SET(PROJECT_LIBS albaTests albaOperations albaVisualPipes albaVME albaCommon albaService)
ENDIF(ALBA_BUILD_ALBADLL)


#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#-----------------------------------------------------------------------------
SET(PROJECT_SRCS 
	albaOperationsTests.h albaOperationsTests.cpp
	albaOpImporterMSF1xTest.h albaOpImporterMSF1xTest.cpp
	albaOpImporterSTLTest.h albaOpImporterSTLTest.cpp
	albaOpExporterSTLTest.h albaOpExporterSTLTest.cpp
	albaOpCropTest.h albaOpCropTest.cpp
	albaOpVOIDensityTest.h albaOpVOIDensityTest.cpp
	albaOpImporterImageTest.h albaOpImporterImageTest.cpp
	albaOpImporterRAWVolumeTest.h albaOpImporterRAWVolumeTest.cpp
	albaOpImporterVRMLTest.h albaOpImporterVRMLTest.cpp
	albaOpExporterRAWTest.h albaOpExporterRAWTest.cpp
	albaOpCreateGroupTest.h albaOpCreateGroupTest.cpp
	albaOpReparentToTest.h albaOpReparentToTest.cpp
	albaOpDecimateSurfaceTest.h albaOpDecimateSurfaceTest.cpp
	albaOpConnectivitySurfaceTest.h albaOpConnectivitySurfaceTest.cpp
	albaOpRemoveCellsTest.h albaOpRemoveCellsTest.cpp
	albaOpBooleanSurfaceTest.h albaOpBooleanSurfaceTest.cpp
	albaOpEditNormalsTest.h albaOpEditNormalsTest.cpp
	albaOpExporterBMPTest.h albaOpExporterBMPTest.cpp
	albaOpImporterMSFTest.h albaOpImporterMSFTest.cpp
	albaOpImporterExternalFileTest.h albaOpImporterExternalFileTest.cpp
	albaOpOpenExternalFileTest.h albaOpOpenExternalFileTest.cpp
	albaOpDecomposeTimeVarVMETest.h albaOpDecomposeTimeVarVMETest.cpp
	albaOpLabelExtractorTest.h albaOpLabelExtractorTest.cpp
	albaOpImporterVTKTest.h albaOpImporterVTKTest.cpp
	albaOpCreateVolumeTest.h albaOpCreateVolumeTest.cpp
	albaOpVOIDensityEditorTestTest.h albaOpVOIDensityEditorTestTest.cpp
	albaOpAddLandmarkTest.h albaOpAddLandmarkTest.cpp
	albaOpApplyTrajectoryTest.h albaOpApplyTrajectoryTest.cpp
	albaOpExtractIsosurfaceTest.h albaOpExtractIsosurfaceTest.cpp
	albaOpImporterASCIITest.h albaOpImporterASCIITest.cpp
	albaOpExporterMSFTest.h albaOpExporterMSFTest.cpp
	albaOpFilterSurfaceTest.h albaOpFilterSurfaceTest.cpp
	albaOpValidateTreeTest.h albaOpValidateTreeTest.cpp
	albaOpGarbageCollectMSFDirTest.h albaOpGarbageCollectMSFDirTest.cpp
	albaOpScalarToSurfaceTest.h albaOpScalarToSurfaceTest.cpp
	albaASCIIImporterUtilityTest.h albaASCIIImporterUtilityTest.cpp
	albaOpEditMetadataTest.h albaOpEditMetadataTest.cpp
	albaOpFilterVolumeTest.h albaOpFilterVolumeTest.cpp
	albaOpTransformOldTest.h albaOpTransformOldTest.cpp
	albaOpTransformInterfaceTest.h albaOpTransformInterfaceTest.cpp
	albaVMEMeshAnsysTextImporterTest.h albaVMEMeshAnsysTextImporterTest.cpp
	albaVMEMeshAnsysTextExporterTest.h albaVMEMeshAnsysTextExporterTest.cpp
	albaOpImporterMeshTest.h albaOpImporterMeshTest.cpp
	albaOpExporterMeshTest.h albaOpExporterMeshTest.cpp
	albaVMEDataSetAttributesImporterTest.h albaVMEDataSetAttributesImporterTest.cpp
	albaOpImporterVMEDataSetAttributesTest.h albaOpImporterVMEDataSetAttributesTest.cpp
	albaOpCreateMeterTest.h albaOpCreateMeterTest.cpp
	albaOpCreateProberTest.h albaOpCreateProberTest.cpp
	albaOpCreateRefSysTest.h albaOpCreateRefSysTest.cpp
	albaOpCreateSlicerTest.h albaOpCreateSlicerTest.cpp
	albaOpCreateSplineTest.h albaOpCreateSplineTest.cpp
	albaOpCreateSurfaceParametricTest.h albaOpCreateSurfaceParametricTest.cpp
	albaOpExporterVTKTest.h albaOpExporterVTKTest.cpp
	albaOpExporterLandmarkTest.h albaOpExporterLandmarkTest.cpp
	albaOpImporterLandmarkTest.h albaOpImporterLandmarkTest.cpp
	albaOpImporterGRFWSTest.h albaOpImporterGRFWSTest.cpp
	albaOpImporterAnalogWSTest.h albaOpImporterAnalogWSTest.cpp
	albaGeometryEditorPolylineGraphTest.h albaGeometryEditorPolylineGraphTest.cpp
	albaOpMatrixVectorMathTest.h albaOpMatrixVectorMathTest.cpp
	albaOpVolumeMeasureTest.h albaOpVolumeMeasureTest.cpp
	albaOpImporterRAWImagesTest.h albaOpImporterRAWImagesTest.cpp
	albaOpImporterLandmarkTXTTest.h albaOpImporterLandmarkTXTTest.cpp
	albaOpImporterLandmarkWSTest.h albaOpImporterLandmarkWSTest.cpp
	albaOpExtrusionHolesTest.h albaOpExtrusionHolesTest.cpp
	albaOpExporterWrappedMeterTest.h albaOpExporterWrappedMeterTest.cpp
	albaOpFreezeVMETest.h albaOpFreezeVMETest.cpp
	albaOpLabelizeSurfaceTest.h albaOpLabelizeSurfaceTest.cpp
	albaOpCleanSurfaceTest.h albaOpCleanSurfaceTest.cpp
	albaOpSmoothSurfaceTest.h albaOpSmoothSurfaceTest.cpp
	albaOpTriangulateSurfaceTest.h albaOpTriangulateSurfaceTest.cpp
	albaOpSurfaceMirrorTest.h albaOpSurfaceMirrorTest.cpp
	albaOpVolumeMirrorTest.h albaOpVolumeMirrorTest.cpp
	albaOpExporterMetersTest.h albaOpExporterMetersTest.cpp
	albaOpSubdivideTest.h albaOpSubdivideTest.cpp
	albaOpCreateSurfaceTest.h albaOpCreateSurfaceTest.cpp
	albaOpVolumeResampleTest.h albaOpVolumeResampleTest.cpp
	albaOpSegmentationRegionGrowingConnectedThresholdTest.h albaOpSegmentationRegionGrowingConnectedThresholdTest.cpp
	albaOpScaleDatasetTest.h albaOpScaleDatasetTest.cpp
	albaOpCreateLabeledVolumeTest.h albaOpCreateLabeledVolumeTest.cpp
	albaOpComputeWrappingTest.h albaOpComputeWrappingTest.cpp
	albaOpComputeInertialTensorTest.h albaOpComputeInertialTensorTest.cpp
	albaOpSplitSurfaceTest.h albaOpSplitSurfaceTest.cpp
	albaOpMML3Test.h albaOpMML3Test.cpp
	albaOpExtractGeometryTest.h albaOpExtractGeometryTest.cpp
	albaOpImporterDicomTest.h albaOpImporterDicomTest.cpp
	itkRawMotionImporterUtilityTest.h itkRawMotionImporterUtilityTest.cpp
	albaOpMakeVMETimevaryingTest.h albaOpMakeVMETimevaryingTest.cpp
	albaOpEqualizeHistogramTest.h albaOpEqualizeHistogramTest.cpp
	albaOpCreateWrappedMeterTest.h albaOpCreateWrappedMeterTest.cpp
	albaOpFlipNormalsTest.h albaOpFlipNormalsTest.cpp
	albaOpSmoothSurfaceCellsTest.h albaOpSmoothSurfaceCellsTest.cpp
	albaOpMeshQualityTest.h albaOpMeshQualityTest.cpp
	albaOpCropDeformableROITest.h albaOpCropDeformableROITest.cpp
	albaOpMoveTest.h albaOpMoveTest.cpp
	albaOpExporterAnalogWSTest.h albaOpExporterAnalogWSTest.cpp
	albaOpExporterGRFWSTest.h albaOpExporterGRFWSTest.cpp
	albaOpExporterLandmarkWSTest.h albaOpExporterLandmarkWSTest.cpp
	albaDicomClassesTest.h albaDicomClassesTest.cpp
	albaOpImporterC3DTest.h albaOpImporterC3DTest.cpp
	albaOpImporterMotionDataTest.h albaOpImporterMotionDataTest.cpp
	albaOpFillHolesTest.h albaOpFillHolesTest.cpp
	albaOpImporterVTKXMLTest.h albaOpImporterVTKXMLTest.cpp
	albaOpInteractiveClipSurfaceTest.h albaOpInteractiveClipSurfaceTest.cpp
	albaOpCreateEditSkeletonTest.h albaOpCreateEditSkeletonTest.cpp
	albaOpRegisterClustersTest.h albaOpRegisterClustersTest.cpp
	albaOpClassicICPRegistrationTest.h albaOpClassicICPRegistrationTest.cpp
	albaOpMML3ParameterViewTest.h albaOpMML3ParameterViewTest.cpp
	albaOpInteractionDebuggerTest.h albaOpInteractionDebuggerTest.cpp
	albaOpIterativeRegistrationTest.h albaOpIterativeRegistrationTest.cpp
	albaOpTransformTest.h albaOpTransformTest.cpp
	albaOpImporterAnsysCDBFileTest.h albaOpImporterAnsysCDBFileTest.cpp
	albaOpExporterAnsysCDBFileTest.h albaOpExporterAnsysCDBFileTest.cpp
	albaOpImporterAnsysInputFileTest.h albaOpImporterAnsysInputFileTest.cpp
	albaOpExporterAnsysInputFileTest.h albaOpExporterAnsysInputFileTest.cpp
	albaOpImporterAbaqusFileTest.h albaOpImporterAbaqusFileTest.cpp
	albaOpExporterAbaqusFileTest.h albaOpExporterAbaqusFileTest.cpp
	albaOpImporterDICFileTest.cpp albaOpImporterDICFileTest.h
	albaOpImporterPointCloudTest.cpp albaOpImporterPointCloudTest.h
	albaOpFilterImageTest.cpp albaOpFilterImageTest.h
)


ADD_EXECUTABLE(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
ADD_TEST(${PROJECT_NAME}  ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME})
IF (WIN32)
  SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)