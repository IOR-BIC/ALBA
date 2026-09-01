PROJECT (albaVMETests)

IF(ALBA_BUILD_ALBADLL)
  SET(PROJECT_LIBS XercesC albaDLL vtkALBA albaTests) 
ELSE(ALBA_BUILD_ALBADLL)
  SET(PROJECT_LIBS albaTests albaVME albaVisualPipes albaDataPipes GPUAPI albaService ${VTK_LIBRARIES})
ENDIF(ALBA_BUILD_ALBADLL)


#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#-----------------------------------------------------------------------------
SET(PROJECT_SRCS 
	albaVMETests.h albaVMETests.cpp
	albaVMEExternalDataTest.h albaVMEExternalDataTest.cpp
	albaVMEVolumeGrayTest.h albaVMEVolumeGrayTest.cpp
	albaVMESurfaceParametricTest.h albaVMESurfaceParametricTest.cpp
	albaVMESurfaceTest.h albaVMESurfaceTest.cpp
	albaVMEGroupTest.h albaVMEGroupTest.cpp
	albaVMEGizmoTest.h albaVMEGizmoTest.cpp
	albaVMEScalarMatrixTest.h albaVMEScalarMatrixTest.cpp
	albaVMEScalarTest.h albaVMEScalarTest.cpp
	albaVMEPolylineTest.h albaVMEPolylineTest.cpp
	albaVMEPolylineSplineTest.h albaVMEPolylineSplineTest.cpp
	albaVMEMeshTest.h albaVMEMeshTest.cpp
	albaVMELandmarkTest.h albaVMELandmarkTest.cpp
	albaVMELandmarkCloudTest.h albaVMELandmarkCloudTest.cpp
	albaMatrixVectorTest.h albaMatrixVectorTest.cpp
	albaVMEFactoryTest.h albaVMEFactoryTest.cpp
	albaDataVectorTest.h albaDataVectorTest.cpp
	albaVMEItemVTKTest.h albaVMEItemVTKTest.cpp
	albaVMEImageTest.h albaVMEImageTest.cpp
	albaVMEVolumeRGBTest.h albaVMEVolumeRGBTest.cpp
	albaVMEInfoTextTest.h albaVMEInfoTextTest.cpp
	mmaMeterTest.h mmaMeterTest.cpp
	albaVMEOutputMeterTest.h albaVMEOutputMeterTest.cpp
	albaVMEPointSetTest.h albaVMEPointSetTest.cpp
	albaVMEVectorTest.h albaVMEVectorTest.cpp
	albaVMEOutputPolylineTest.h albaVMEOutputPolylineTest.cpp
	albaVMEOutputSurfaceTest.h albaVMEOutputSurfaceTest.cpp
	albaVMEOutputPointSetTest.h albaVMEOutputPointSetTest.cpp
	albaVMEOutputScalarTest.h albaVMEOutputScalarTest.cpp
	albaVMEOutputLandmarkCloudTest.h albaVMEOutputLandmarkCloudTest.cpp
	albaVMEOutputScalarMatrixTest.h albaVMEOutputScalarMatrixTest.cpp
	albaVMEOutputMeshTest.h albaVMEOutputMeshTest.cpp
	albaVMEOutputImageTest.h albaVMEOutputImageTest.cpp
	albaVMEOutputVolumeTest.h albaVMEOutputVolumeTest.cpp
	albaCryptTest.h albaCryptTest.cpp
	albaMatrixInterpolatorTest.h albaMatrixInterpolatorTest.cpp
	albaVMEGenericTest.h albaVMEGenericTest.cpp
	albaVMEOutputVTKTest.h albaVMEOutputVTKTest.cpp
	albaVMEItemScalarMatrixTest.h albaVMEItemScalarMatrixTest.cpp
	albaScalarVectorTest.h albaScalarVectorTest.cpp
	albaVMEGenericAbstractTest.h albaVMEGenericAbstractTest.cpp
	albaVMEItemTest.h albaVMEItemTest.cpp
	albaVMERefSysTest.h albaVMERefSysTest.cpp
	albaVMEProberTest.h albaVMEProberTest.cpp
	albaVMEMeterTest.h albaVMEMeterTest.cpp
	albaMSFImporterTest.h albaMSFImporterTest.cpp
	albaVMESlicerTest.h albaVMESlicerTest.cpp
	albaVMERawMotionDataTest.h albaVMERawMotionDataTest.cpp
	albaPolylineGraphTest.h albaPolylineGraphTest.cpp
	albaVMEWrappedMeterTest.h albaVMEWrappedMeterTest.cpp
	albaVMELabeledVolumeTest.h albaVMELabeledVolumeTest.cpp
	albaVMEAnalogTest.h albaVMEAnalogTest.cpp
	albaVMEOutputSurfaceEditorTest.h albaVMEOutputSurfaceEditorTest.cpp
	albaVMEOutputPolylineEditorTest.h albaVMEOutputPolylineEditorTest.cpp
	albaVMESurfaceEditorTest.h albaVMESurfaceEditorTest.cpp
	albaVMEPolylineEditorTest.h albaVMEPolylineEditorTest.cpp
	albaVMEMapsTest.h albaVMEMapsTest.cpp
	albaVMEPointCloudTest.cpp albaVMEPointCloudTest.h
	albaVMEOutputWrappedMeterTest.h albaVMEOutputWrappedMeterTest.cpp
	albaVMEPolylineGraphTest.h albaVMEPolylineGraphTest.cpp
	albaVMESegmentationVolumeTest.h albaVMESegmentationVolumeTest.cpp
	albaAttributeSegmentationVolumeTest.h albaAttributeSegmentationVolumeTest.cpp
	albaVMEOutputComputeWrappingTest.h albaVMEOutputComputeWrappingTest.cpp
	albaPolylineGraphVertexTest.h albaPolylineGraphVertexTest.cpp
	albaPolylineGraphEdgeTest.h albaPolylineGraphEdgeTest.cpp
	albaPolylineGraphBranchTest.h albaPolylineGraphBranchTest.cpp
	albaDataPipeCustomSegmentationVolumeTest.h albaDataPipeCustomSegmentationVolumeTest.cpp
	VMEGenericPoseTest.h VMEGenericPoseTest.cpp
	VMEGenericBoundsTest.h VMEGenericBoundsTest.cpp
	CustomDataPipeTest.h CustomDataPipeTest.cpp
	VMEGenericTest.h VMEGenericTest.cpp
)

ADD_EXECUTABLE(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
ADD_TEST(${PROJECT_NAME}  ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME})
IF (WIN32)
  SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)