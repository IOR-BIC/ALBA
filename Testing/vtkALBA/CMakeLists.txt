PROJECT (vtkALBATests)


IF(ALBA_BUILD_ALBADLL)
  SET(PROJECT_LIBS albaDLL vtkALBA albaTests)
ELSE(ALBA_BUILD_ALBADLL)
  SET(PROJECT_LIBS albaTests vtkALBA ${VTK_LIBRARIES} albaCommon albaService)
ENDIF(ALBA_BUILD_ALBADLL)


#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#-----------------------------------------------------------------------------
SET(PROJECT_SRCS 
	vtkALBATests.h vtkALBATests.cpp
	vtkALBARemoveCellsFilterTest.h vtkALBARemoveCellsFilterTest.cpp
	vtkALBAPolyDataToSinglePolyLineTest.h vtkALBAPolyDataToSinglePolyLineTest.cpp
	vtkALBATextOrientatorTest.h vtkALBATextOrientatorTest.cpp
	vtkALBAGlobalAxisCoordinateTest.h vtkALBAGlobalAxisCoordinateTest.cpp
	vtkALBALocalAxisCoordinateTest.h vtkALBALocalAxisCoordinateTest.cpp
	vtkALBAVolumeResampleTest.h vtkALBAVolumeResampleTest.cpp
	vtkALBADOFMatrixTest.h vtkALBADOFMatrixTest.cpp
	vtkALBAImplicitPolyDataTest.h vtkALBAImplicitPolyDataTest.cpp
	vtkALBATextActorMeterTest.h vtkALBATextActorMeterTest.cpp
	vtkALBACellsFilterTest.h vtkALBACellsFilterTest.cpp
	vtkALBAGridActorTest.h vtkALBAGridActorTest.cpp
	vtkALBAProjectVolumeTest.h vtkALBAProjectVolumeTest.cpp
	vtkALBAClipSurfaceBoundingBoxTest.h vtkALBAClipSurfaceBoundingBoxTest.cpp
	vtkALBAProfilingActorTest.h vtkALBAProfilingActorTest.cpp
	vtkALBARulerActor2DTest.h vtkALBARulerActor2DTest.cpp
	vtkALBASimpleRulerActor2DTest.h vtkALBASimpleRulerActor2DTest.cpp
	vtkALBAFixedCutterTest.h vtkALBAFixedCutterTest.cpp
	vtkALBAHistogramTest.h vtkALBAHistogramTest.cpp
	vtkALBAExtendedGlyph3DTest.h vtkALBAExtendedGlyph3DTest.cpp
	vtkALBATransferFunction2DTest.h vtkALBATransferFunction2DTest.cpp
	vtkHoleConnectivityTest.h vtkHoleConnectivityTest.cpp
	vtkTriangleQualityRatioTest.h vtkTriangleQualityRatioTest.cpp
	vtkALBATubeFilterTest.h vtkALBATubeFilterTest.cpp
	vtkMaskPolyDataFilterTest.h vtkMaskPolyDataFilterTest.cpp
	vtkALBAExtrudeToCircleTest.h vtkALBAExtrudeToCircleTest.cpp
	vtkALBAPastValuesListTest.h vtkALBAPastValuesListTest.cpp
	vtkALBAPolyDataMirrorTest.h vtkALBAPolyDataMirrorTest.cpp
	vtkALBARegionGrowingLocalGlobalThresholdTest.h vtkALBARegionGrowingLocalGlobalThresholdTest.cpp
	vtkImageUnPackerTest.h vtkImageUnPackerTest.cpp
	vtkPackedImageTest.h vtkPackedImageTest.cpp
	vtkXRayVolumeMapperTest.h vtkXRayVolumeMapperTest.cpp
	vtkALBADataArrayDescriptorTest.h vtkALBADataArrayDescriptorTest.cpp
	vtkALBAErythrocyteSourceTest.h vtkALBAErythrocyteSourceTest.cpp
	vtkALBAMeshCutterTest.h vtkALBAMeshCutterTest.cpp
	vtkALBAFillingHoleTest.h vtkALBAFillingHoleTest.cpp
	vtkALBAFixTopologyTest.h vtkALBAFixTopologyTest.cpp
	vtkALBALandmarkCloudOutlineCornerFilterTest.h vtkALBALandmarkCloudOutlineCornerFilterTest.cpp
	vtkWeightedLandmarkTransformTest.h vtkWeightedLandmarkTransformTest.cpp
	EdgeTest.h EdgeTest.cpp
	TriangleTest.h TriangleTest.cpp
	CoredPointIndexTest.h CoredPointIndexTest.cpp
	EdgeIndexTest.h EdgeIndexTest.cpp
	CoredEdgeIndexTest.h CoredEdgeIndexTest.cpp
	TriangleIndexTest.h TriangleIndexTest.cpp
	TriangulationEdgeTest.h TriangulationEdgeTest.cpp
	TriangulationTriangleTest.h TriangulationTriangleTest.cpp
	CMatrixTest.h CMatrixTest.cpp
	CMatrixTestM1.h CMatrixTestM1.cpp
	CMatrixTestM2.h CMatrixTestM2.cpp
	RingDataTest.h RingDataTest.cpp
	MeshDataTest.h MeshDataTest.cpp
	vtkALBAExtrudeToCircleVertexDataTest.h vtkALBAExtrudeToCircleVertexDataTest.cpp
	vtkALBAFillingHoleCTriangleTest.h vtkALBAFillingHoleCTriangleTest.cpp
	vtkALBAFillingHoleCEdgeTest.h vtkALBAFillingHoleCEdgeTest.cpp
	MemoryInfoTest.h MemoryInfoTest.cpp
	VectorTest.h VectorTest.cpp
	SparseMatrixTest.h SparseMatrixTest.cpp
	NVectorTest.h NVectorTest.cpp
	TreeNodeDataTest.h TreeNodeDataTest.cpp
	SortedTreeNodesTest.h SortedTreeNodesTest.cpp
	CubeTest.h CubeTest.cpp
	SquareTest.h SquareTest.cpp
	CSkeletonEdgeTest.h CSkeletonEdgeTest.cpp
	VertexDataTest.h VertexDataTest.cpp
	TriangulationTest.h TriangulationTest.cpp
	AllocatorTest.h AllocatorTest.cpp
	BinaryNodeTest.h BinaryNodeTest.cpp
	PolynomialTest.h PolynomialTest.cpp
	StartingPolynomialTest.h StartingPolynomialTest.cpp
	CSkeletonVertexTest.h CSkeletonVertexTest.cpp
	PPolynomialTest.h PPolynomialTest.cpp
	CSkeletonEdgeM1Test.h CSkeletonEdgeM1Test.cpp
	CSkeletonVertexM1Test.h CSkeletonVertexM1Test.cpp
	CSkeletonVertexM2Test.h CSkeletonVertexM2Test.cpp
	vtkALBACollisionDetectionFilterTest.h vtkALBACollisionDetectionFilterTest.cpp
	vtkALBAImageFillHolesRemoveIslandsTest.h vtkALBAImageFillHolesRemoveIslandsTest.cpp
	vtkALBARayCastCleanerTest.h vtkALBARayCastCleanerTest.cpp
	vtkALBALineStripSourceTest.h vtkALBALineStripSourceTest.cpp
	vtkALBARGtoSPImageFilterTest.h vtkALBARGtoSPImageFilterTest.cpp
	vtkALBADistanceFilterTest.h	vtkALBADistanceFilterTest.cpp
	vtkALBACircleSourceTest.h	vtkALBACircleSourceTest.cpp
)

IF (DISABLE_GPU_VOLUME_RENDERING_TESTS)
SET(PROJECT_SRCS ${PROJECT_SRCS}
	vtkALBAVolumeTextureMapper2DTest.h vtkALBAVolumeTextureMapper2DTest.cpp
)
ENDIF (DISABLE_GPU_VOLUME_RENDERING_TESTS)
	

ADD_EXECUTABLE(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
ADD_TEST(${PROJECT_NAME} ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME})
IF (WIN32)
  SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)