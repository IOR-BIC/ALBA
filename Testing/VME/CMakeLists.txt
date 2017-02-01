PROJECT (mafVME_TEST)

IF(MAF_BUILD_MAFDLL)
  LINK_LIBRARIES(mafDLL)
ELSE(MAF_BUILD_MAFDLL)
  LINK_LIBRARIES(mafTests mafVME mafVisualPipes mafDataPipes GPUAPI vtkRendering vtkGraphics mafService) # mafCommon mafOperations)
ENDIF (MAF_BUILD_MAFDLL)
 
#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#-----------------------------------------------------------------------------
SET(PROJECT_SRCS 
	mafVMETests.h mafVMETests.cpp
	mafVMEExternalDataTest.h mafVMEExternalDataTest.cpp
	mafVMEVolumeGrayTest.h mafVMEVolumeGrayTest.cpp
	mafVMESurfaceParametricTest.h mafVMESurfaceParametricTest.cpp
	mafVMESurfaceTest.h mafVMESurfaceTest.cpp
	mafVMEGroupTest.h mafVMEGroupTest.cpp
	mafVMEGizmoTest.h mafVMEGizmoTest.cpp
	mafVMEScalarMatrixTest.h mafVMEScalarMatrixTest.cpp
	mafVMEScalarTest.h mafVMEScalarTest.cpp
	mafVMEPolylineTest.h mafVMEPolylineTest.cpp
	mafVMEPolylineSplineTest.h mafVMEPolylineSplineTest.cpp
	mafVMEMeshTest.h mafVMEMeshTest.cpp
	mafVMELandmarkTest.h mafVMELandmarkTest.cpp
	mafVMELandmarkCloudTest.h mafVMELandmarkCloudTest.cpp
	mafMatrixVectorTest.h mafMatrixVectorTest.cpp
	mafVMEFactoryTest.h mafVMEFactoryTest.cpp
	mafDataVectorTest.h mafDataVectorTest.cpp
	mafVMEItemVTKTest.h mafVMEItemVTKTest.cpp
	mafVMEImageTest.h mafVMEImageTest.cpp
	mafVMEVolumeRGBTest.h mafVMEVolumeRGBTest.cpp
	mafVMEInfoTextTest.h mafVMEInfoTextTest.cpp
	mmaMeterTest.h mmaMeterTest.cpp
	mafVMEOutputMeterTest.h mafVMEOutputMeterTest.cpp
	mafVMEPointSetTest.h mafVMEPointSetTest.cpp
	mafVMEVectorTest.h mafVMEVectorTest.cpp
	mafVMEOutputPolylineTest.h mafVMEOutputPolylineTest.cpp
	mafVMEOutputSurfaceTest.h mafVMEOutputSurfaceTest.cpp
	mafVMEOutputPointSetTest.h mafVMEOutputPointSetTest.cpp
	mafVMEOutputScalarTest.h mafVMEOutputScalarTest.cpp
	mafVMEOutputLandmarkCloudTest.h mafVMEOutputLandmarkCloudTest.cpp
	mafVMEOutputScalarMatrixTest.h mafVMEOutputScalarMatrixTest.cpp
	mafVMEOutputMeshTest.h mafVMEOutputMeshTest.cpp
	mafVMEOutputImageTest.h mafVMEOutputImageTest.cpp
	mafVMEOutputVolumeTest.h mafVMEOutputVolumeTest.cpp
	mafCryptTest.h mafCryptTest.cpp
	mafMatrixInterpolatorTest.h mafMatrixInterpolatorTest.cpp
	mafVMEGenericTest.h mafVMEGenericTest.cpp
	mafVMEOutputVTKTest.h mafVMEOutputVTKTest.cpp
	mafVMEItemScalarMatrixTest.h mafVMEItemScalarMatrixTest.cpp
	mafScalarVectorTest.h mafScalarVectorTest.cpp
	mafVMEGenericAbstractTest.h mafVMEGenericAbstractTest.cpp
	mafVMEItemTest.h mafVMEItemTest.cpp
	mafVMERefSysTest.h mafVMERefSysTest.cpp
	mafVMEProberTest.h mafVMEProberTest.cpp
	mafVMEMeterTest.h mafVMEMeterTest.cpp
	mafMSFImporterTest.h mafMSFImporterTest.cpp
	mafVMESlicerTest.h mafVMESlicerTest.cpp
	mafVMERawMotionDataTest.h mafVMERawMotionDataTest.cpp
	mafPolylineGraphTest.h mafPolylineGraphTest.cpp
	mafVMEWrappedMeterTest.h mafVMEWrappedMeterTest.cpp
	mafVMELabeledVolumeTest.h mafVMELabeledVolumeTest.cpp
	mafVMEAnalogTest.h mafVMEAnalogTest.cpp
	mafVMEOutputSurfaceEditorTest.h mafVMEOutputSurfaceEditorTest.cpp
	mafVMEOutputPolylineEditorTest.h mafVMEOutputPolylineEditorTest.cpp
	mafVMESurfaceEditorTest.h mafVMESurfaceEditorTest.cpp
	mafVMEPolylineEditorTest.h mafVMEPolylineEditorTest.cpp
	mafVMEMapsTest.h mafVMEMapsTest.cpp
	mafVMEOutputWrappedMeterTest.h mafVMEOutputWrappedMeterTest.cpp
	mafVMEPolylineGraphTest.h mafVMEPolylineGraphTest.cpp
	mafVMESegmentationVolumeTest.h mafVMESegmentationVolumeTest.cpp
	mafAttributeSegmentationVolumeTest.h mafAttributeSegmentationVolumeTest.cpp
	mafVMEOutputComputeWrappingTest.h mafVMEOutputComputeWrappingTest.cpp
	mafPolylineGraphVertexTest.h mafPolylineGraphVertexTest.cpp
	mafPolylineGraphEdgeTest.h mafPolylineGraphEdgeTest.cpp
	mafPolylineGraphBranchTest.h mafPolylineGraphBranchTest.cpp
	mafDataPipeCustomSegmentationVolumeTest.h mafDataPipeCustomSegmentationVolumeTest.cpp
	VMEGenericPoseTest.h VMEGenericPoseTest.cpp
	VMEGenericBoundsTest.h VMEGenericBoundsTest.cpp
	CustomDataPipeTest.h CustomDataPipeTest.cpp
	VMEGenericTest.h VMEGenericTest.cpp
)

ADD_EXECUTABLE(mafVMETests ${PROJECT_SRCS})
ADD_TEST(mafVMETests  ${EXECUTABLE_OUTPUT_PATH}/mafVMETests)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVMETests PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)