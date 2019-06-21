PROJECT (vtkALBA_TEST)

IF(ALBA_BUILD_ALBADLL)
  LINK_LIBRARIES(albaDLL)
ELSE(ALBA_BUILD_ALBADLL)
  LINK_LIBRARIES(albaTests vtkALBA vtkGraphics vtkRendering albaCommon albaService)
ENDIF (ALBA_BUILD_ALBADLL)

#ADD_EXECUTABLE(vtkALBATest vtkALBATest.cxx)
#ADD_EXECUTABLE(testVTKVolumeSlicer testVTKVolumeSlicer.cxx)
#ADD_EXECUTABLE(vtkALBAPolyDataSingleSourceShortestPathTest vtkALBAPolyDataSingleSourceShortestPathTest.h vtkALBAPolyDataSingleSourceShortestPathTest.cpp)
# ADD_EXECUTABLE(vtkALBAImageUnsharpFilterTest vtkALBAImageUnsharpFilterTest.h vtkALBAImageUnsharpFilterTest.cpp)

#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#-----------------------------------------------------------------------------
SET(PROJECT_SRCS 
	vtkALBATests.h vtkALBATests.cpp
	vtkALBAContourVolumeMapperTest.h vtkALBAContourVolumeMapperTest.cpp
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
	vtkALBARectilinearGridToRectilinearGridFilterTest.h vtkALBARectilinearGridToRectilinearGridFilterTest.cpp
	vtkALBADummyRectilinearGridToRectilinearGridFilter.h vtkALBADummyRectilinearGridToRectilinearGridFilter.cpp
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
	vtkALBAVolumeRayCastMapperTest.h vtkALBAVolumeRayCastMapperTest.cpp
	vtkXRayVolumeMapperTest.h vtkXRayVolumeMapperTest.cpp
	vtkALBAVolumeTextureMapper2DTest.h vtkALBAVolumeTextureMapper2DTest.cpp
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
)

ADD_EXECUTABLE(vtkALBATests ${PROJECT_SRCS})
ADD_TEST(vtkALBATests  ${EXECUTABLE_OUTPUT_PATH}/vtkALBATests)
IF (WIN32)
  SET_TARGET_PROPERTIES(vtkALBATests PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)