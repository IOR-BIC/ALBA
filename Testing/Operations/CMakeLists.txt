PROJECT (Operations_TEST)

IF(MAF_BUILD_MAFDLL)
  LINK_LIBRARIES(mafDLL)
ELSE(MAF_BUILD_MAFDLL)
  LINK_LIBRARIES(mafTests mafOperations mafVME mafCommon)
ENDIF (MAF_BUILD_MAFDLL)

#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#-----------------------------------------------------------------------------
SET(PROJECT_SRCS 
	mafOperationsTests.h mafOperationsTests.cpp
	mafOpImporterMSF1xTest.h mafOpImporterMSF1xTest.cpp
	mafOpImporterSTLTest.h mafOpImporterSTLTest.cpp
	mafOpExporterSTLTest.h mafOpExporterSTLTest.cpp
	mafOpCropTest.h mafOpCropTest.cpp
	mafOpVOIDensityTest.h mafOpVOIDensityTest.cpp
	mafOpImporterImageTest.h mafOpImporterImageTest.cpp
	mafOpImporterRAWVolumeTest.h mafOpImporterRAWVolumeTest.cpp
	mafOpImporterVRMLTest.h mafOpImporterVRMLTest.cpp
	mafOpExporterRAWTest.h mafOpExporterRAWTest.cpp
	mafOpCreateGroupTest.h mafOpCreateGroupTest.cpp
	mafOpReparentToTest.h mafOpReparentToTest.cpp
	mafOpVolumeResampleTest.h mafOpVolumeResampleTest.cpp
	mafOpDecimateSurfaceTest.h mafOpDecimateSurfaceTest.cpp
	mafOpConnectivitySurfaceTest.h mafOpConnectivitySurfaceTest.cpp
	mafOpRemoveCellsTest.h mafOpRemoveCellsTest.cpp
	mafOpBooleanSurfaceTest.h mafOpBooleanSurfaceTest.cpp
	mafOpEditNormalsTest.h mafOpEditNormalsTest.cpp
	mafOpExporterBMPTest.h mafOpExporterBMPTest.cpp
	mafOpImporterMSFTest.h mafOpImporterMSFTest.cpp
	mafOpImporterExternalFileTest.h mafOpImporterExternalFileTest.cpp
	mafOpOpenExternalFileTest.h mafOpOpenExternalFileTest.cpp
	mafOpDecomposeTimeVarVMETest.h mafOpDecomposeTimeVarVMETest.cpp
	mafOpLabelExtractorTest.h mafOpLabelExtractorTest.cpp
	mafOpImporterVTKTest.h mafOpImporterVTKTest.cpp
	mafOpCreateVolumeTest.h mafOpCreateVolumeTest.cpp
	mafOpVOIDensityEditorTestTest.h mafOpVOIDensityEditorTestTest.cpp
	mafOpAddLandmarkTest.h mafOpAddLandmarkTest.cpp
	mafOpApplyTrajectoryTest.h mafOpApplyTrajectoryTest.cpp
	mafOpExtractIsosurfaceTest.h mafOpExtractIsosurfaceTest.cpp
	mafOpImporterASCIITest.h mafOpImporterASCIITest.cpp
	mafOpExporterMSFTest.h mafOpExporterMSFTest.cpp
	mafOpFilterSurfaceTest.h mafOpFilterSurfaceTest.cpp
	mafOpValidateTreeTest.h mafOpValidateTreeTest.cpp
	mafOpGarbageCollectMSFDirTest.h mafOpGarbageCollectMSFDirTest.cpp
	mafOpScalarToSurfaceTest.h mafOpScalarToSurfaceTest.cpp
	mafASCIIImporterUtilityTest.h mafASCIIImporterUtilityTest.cpp
	mafOpEditMetadataTest.h mafOpEditMetadataTest.cpp
	mafOpFilterVolumeTest.h mafOpFilterVolumeTest.cpp
	mafOpMAFTransformTest.h mafOpMAFTransformTest.cpp
	mafOpTransformInterfaceTest.h mafOpTransformInterfaceTest.cpp
	mafVMEMeshAnsysTextImporterTest.h mafVMEMeshAnsysTextImporterTest.cpp
	mafVMEMeshAnsysTextExporterTest.h mafVMEMeshAnsysTextExporterTest.cpp
	mafOpImporterMeshTest.h mafOpImporterMeshTest.cpp
	mafOpExporterMeshTest.h mafOpExporterMeshTest.cpp
	mafVMEDataSetAttributesImporterTest.h mafVMEDataSetAttributesImporterTest.cpp
	mafOpImporterVMEDataSetAttributesTest.h mafOpImporterVMEDataSetAttributesTest.cpp
	mafOpCreateMeterTest.h mafOpCreateMeterTest.cpp
	mafOpCreateProberTest.h mafOpCreateProberTest.cpp
	mafOpCreateRefSysTest.h mafOpCreateRefSysTest.cpp
	mafOpCreateSlicerTest.h mafOpCreateSlicerTest.cpp
	mafOpCreateSplineTest.h mafOpCreateSplineTest.cpp
	mafOpCreateSurfaceParametricTest.h mafOpCreateSurfaceParametricTest.cpp
	mafOpExporterVTKTest.h mafOpExporterVTKTest.cpp
	mafOpExporterLandmarkTest.h mafOpExporterLandmarkTest.cpp
	mafOpImporterLandmarkTest.h mafOpImporterLandmarkTest.cpp
	mafOpImporterGRFWSTest.h mafOpImporterGRFWSTest.cpp
	mafOpImporterAnalogWSTest.h mafOpImporterAnalogWSTest.cpp
	mafGeometryEditorPolylineGraphTest.h mafGeometryEditorPolylineGraphTest.cpp
	mafOpMatrixVectorMathTest.h mafOpMatrixVectorMathTest.cpp
	mafOpVolumeMeasureTest.h mafOpVolumeMeasureTest.cpp
	mafOpImporterRAWImagesTest.h mafOpImporterRAWImagesTest.cpp
	mafOpImporterLandmarkTXTTest.h mafOpImporterLandmarkTXTTest.cpp
	mafOpImporterLandmarkWSTest.h mafOpImporterLandmarkWSTest.cpp
	mafOpExtrusionHolesTest.h mafOpExtrusionHolesTest.cpp
	mafOpExporterWrappedMeterTest.h mafOpExporterWrappedMeterTest.cpp
	mafOpFreezeVMETest.h mafOpFreezeVMETest.cpp
	mafOpLabelizeSurfaceTest.h mafOpLabelizeSurfaceTest.cpp
	mafOpCleanSurfaceTest.h mafOpCleanSurfaceTest.cpp
	mafOpSmoothSurfaceTest.h mafOpSmoothSurfaceTest.cpp
	mafOpTriangulateSurfaceTest.h mafOpTriangulateSurfaceTest.cpp
	mafOpSurfaceMirrorTest.h mafOpSurfaceMirrorTest.cpp
	medOpImporterVTKTest.h medOpImporterVTKTest.cpp
	mafOpExporterMetersTest.h mafOpExporterMetersTest.cpp
	mafOpSubdivideTest.h mafOpSubdivideTest.cpp
	mafOpCreateSurfaceTest.h mafOpCreateSurfaceTest.cpp
	medOpVolumeResampleTest.h medOpVolumeResampleTest.cpp
	mafOpSegmentationRegionGrowingConnectedThresholdTest.h mafOpSegmentationRegionGrowingConnectedThresholdTest.cpp
	mafOpScaleDatasetTest.h mafOpScaleDatasetTest.cpp
	mafOpCreateLabeledVolumeTest.h mafOpCreateLabeledVolumeTest.cpp
	mafOpComputeWrappingTest.h mafOpComputeWrappingTest.cpp
	mafOpComputeInertialTensorTest.h mafOpComputeInertialTensorTest.cpp
	mafOpSplitSurfaceTest.h mafOpSplitSurfaceTest.cpp
	mafOpMML3Test.h mafOpMML3Test.cpp
	mafOpExtractGeometryTest.h mafOpExtractGeometryTest.cpp
	#mafOpImporterDicomOffisTest.h mafOpImporterDicomOffisTest.cpp
	itkRawMotionImporterUtilityTest.h itkRawMotionImporterUtilityTest.cpp
	mafOpMakeVMETimevaryingTest.h mafOpMakeVMETimevaryingTest.cpp
	mafOpEqualizeHistogramTest.h mafOpEqualizeHistogramTest.cpp
	mafOpCreateWrappedMeterTest.h mafOpCreateWrappedMeterTest.cpp
	mafOpFlipNormalsTest.h mafOpFlipNormalsTest.cpp
	mafOpSmoothSurfaceCellsTest.h mafOpSmoothSurfaceCellsTest.cpp
	mafOpMeshQualityTest.h mafOpMeshQualityTest.cpp
	mafOpImporterBBFTest.h mafOpImporterBBFTest.cpp
	mafOpCropDeformableROITest.h mafOpCropDeformableROITest.cpp
	mafOpImporterRAWVolume_BESTest.h mafOpImporterRAWVolume_BESTest.cpp
	mafOpMoveTest.h mafOpMoveTest.cpp
	mafOpExporterAnalogWSTest.h mafOpExporterAnalogWSTest.cpp
	mafOpExporterGRFWSTest.h mafOpExporterGRFWSTest.cpp
	mafOpExporterLandmarkWSTest.h mafOpExporterLandmarkWSTest.cpp
	mafDicomClassesTest.h mafDicomClassesTest.cpp
	mafOpImporterC3DTest.h mafOpImporterC3DTest.cpp
	mafOpImporterMotionDataTest.h mafOpImporterMotionDataTest.cpp
	mafOpFillHolesTest.h mafOpFillHolesTest.cpp
	mafOpImporterVTKXMLTest.h mafOpImporterVTKXMLTest.cpp
	mafOpInteractiveClipSurfaceTest.h mafOpInteractiveClipSurfaceTest.cpp
	mafOpCreateEditSkeletonTest.h mafOpCreateEditSkeletonTest.cpp
	mafOpRegisterClustersTest.h mafOpRegisterClustersTest.cpp
	mafOpClassicICPRegistrationTest.h mafOpClassicICPRegistrationTest.cpp
	mafOpMML3ParameterViewTest.h mafOpMML3ParameterViewTest.cpp
	mafOpInteractionDebuggerTest.h mafOpInteractionDebuggerTest.cpp
	mafOpIterativeRegistrationTest.h mafOpIterativeRegistrationTest.cpp
	mafOpTransformTest.h mafOpTransformTest.cpp
	mafOpImporterAnsysCDBFileTest.h mafOpImporterAnsysCDBFileTest.cpp
	mafOpExporterAnsysCDBFileTest.h mafOpExporterAnsysCDBFileTest.cpp
	mafOpImporterAnsysInputFileTest.h mafOpImporterAnsysInputFileTest.cpp
	mafOpExporterAnsysInputFileTest.h mafOpExporterAnsysInputFileTest.cpp
)

ADD_EXECUTABLE(mafOperationsTests ${PROJECT_SRCS})
ADD_TEST(mafOperationsTests  ${EXECUTABLE_OUTPUT_PATH}/mafOperationsTests)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafOperationsTests PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)