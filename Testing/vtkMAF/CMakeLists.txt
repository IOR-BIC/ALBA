PROJECT (vtkMAF_TEST)

IF(MAF_BUILD_MAFDLL)
  LINK_LIBRARIES(mafDLL)
ELSE(MAF_BUILD_MAFDLL)
  LINK_LIBRARIES(mafTests vtkMAF vtkGraphics vtkRendering mafCommon mafService)
ENDIF (MAF_BUILD_MAFDLL)

#ADD_EXECUTABLE(vtkMAFTest vtkMAFTest.cxx)
#ADD_EXECUTABLE(testDicom testDicom.cxx)
#ADD_EXECUTABLE(testVTKVolumeSlicer testVTKVolumeSlicer.cxx)
#ADD_EXECUTABLE(vtkMAFPolyDataSingleSourceShortestPathTest vtkMAFPolyDataSingleSourceShortestPathTest.h vtkMAFPolyDataSingleSourceShortestPathTest.cpp)
# ADD_EXECUTABLE(vtkMAFImageUnsharpFilterTest vtkMAFImageUnsharpFilterTest.h vtkMAFImageUnsharpFilterTest.cpp)

#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#-----------------------------------------------------------------------------
SET(PROJECT_SRCS 
	vtkMAFTests.h vtkMAFTests.cpp
	vtkMAFContourVolumeMapperTest.h vtkMAFContourVolumeMapperTest.cpp
	vtkMAFRemoveCellsFilterTest.h vtkMAFRemoveCellsFilterTest.cpp
	vtkMAFPolyDataToSinglePolyLineTest.h vtkMAFPolyDataToSinglePolyLineTest.cpp
	vtkMAFTextOrientatorTest.h vtkMAFTextOrientatorTest.cpp
	vtkMAFGlobalAxisCoordinateTest.h vtkMAFGlobalAxisCoordinateTest.cpp
	vtkMAFLocalAxisCoordinateTest.h vtkMAFLocalAxisCoordinateTest.cpp
	vtkMAFVolumeResampleTest.h vtkMAFVolumeResampleTest.cpp
	vtkMAFDOFMatrixTest.h vtkMAFDOFMatrixTest.cpp
	vtkMAFImplicitPolyDataTest.h vtkMAFImplicitPolyDataTest.cpp
	vtkMAFTextActorMeterTest.h vtkMAFTextActorMeterTest.cpp
	vtkMAFCellsFilterTest.h vtkMAFCellsFilterTest.cpp
	vtkMAFGridActorTest.h vtkMAFGridActorTest.cpp
	vtkMAFProjectVolumeTest.h vtkMAFProjectVolumeTest.cpp
	vtkMAFClipSurfaceBoundingBoxTest.h vtkMAFClipSurfaceBoundingBoxTest.cpp
	vtkMAFRGSliceAccumulateTest.h vtkMAFRGSliceAccumulateTest.cpp
	vtkMAFRectilinearGridToRectilinearGridFilterTest.h vtkMAFRectilinearGridToRectilinearGridFilterTest.cpp
	vtkMAFDummyRectilinearGridToRectilinearGridFilter.h vtkMAFDummyRectilinearGridToRectilinearGridFilter.cpp
	vtkMAFProfilingActorTest.h vtkMAFProfilingActorTest.cpp
	vtkMAFRulerActor2DTest.h vtkMAFRulerActor2DTest.cpp
	vtkMAFSimpleRulerActor2DTest.h vtkMAFSimpleRulerActor2DTest.cpp
	vtkMAFFixedCutterTest.h vtkMAFFixedCutterTest.cpp
	vtkMAFHistogramTest.h vtkMAFHistogramTest.cpp
	vtkMAFExtendedGlyph3DTest.h vtkMAFExtendedGlyph3DTest.cpp
	vtkMAFTransferFunction2DTest.h vtkMAFTransferFunction2DTest.cpp
	vtkHoleConnectivityTest.h vtkHoleConnectivityTest.cpp
	vtkDicomUnPackerTest.h vtkDicomUnPackerTest.cpp
	vtkTriangleQualityRatioTest.h vtkTriangleQualityRatioTest.cpp
	vtkMAFTubeFilterTest.h vtkMAFTubeFilterTest.cpp
	vtkMaskPolyDataFilterTest.h vtkMaskPolyDataFilterTest.cpp
	vtkMAFExtrudeToCircleTest.h vtkMAFExtrudeToCircleTest.cpp
	vtkMAFPastValuesListTest.h vtkMAFPastValuesListTest.cpp
	vtkMAFPolyDataMirrorTest.h vtkMAFPolyDataMirrorTest.cpp
	vtkMAFRegionGrowingLocalGlobalThresholdTest.h vtkMAFRegionGrowingLocalGlobalThresholdTest.cpp
	vtkImageUnPackerTest.h vtkImageUnPackerTest.cpp
	vtkPackedImageTest.h vtkPackedImageTest.cpp
	vtkMAFVolumeRayCastMapperTest.h vtkMAFVolumeRayCastMapperTest.cpp
	vtkXRayVolumeMapperTest.h vtkXRayVolumeMapperTest.cpp
	vtkMAFVolumeTextureMapper2DTest.h vtkMAFVolumeTextureMapper2DTest.cpp
	vtkMAFDataArrayDescriptorTest.h vtkMAFDataArrayDescriptorTest.cpp
	vtkMAFErythrocyteSourceTest.h vtkMAFErythrocyteSourceTest.cpp
	vtkMAFMeshCutterTest.h vtkMAFMeshCutterTest.cpp
	vtkMAFFillingHoleTest.h vtkMAFFillingHoleTest.cpp
	vtkMAFFixTopologyTest.h vtkMAFFixTopologyTest.cpp
	vtkMAFLandmarkCloudOutlineCornerFilterTest.h vtkMAFLandmarkCloudOutlineCornerFilterTest.cpp
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
	vtkMAFExtrudeToCircleVertexDataTest.h vtkMAFExtrudeToCircleVertexDataTest.cpp
	vtkMAFFillingHoleCTriangleTest.h vtkMAFFillingHoleCTriangleTest.cpp
	vtkMAFFillingHoleCEdgeTest.h vtkMAFFillingHoleCEdgeTest.cpp
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
	vtkMAFCollisionDetectionFilterTest.h vtkMAFCollisionDetectionFilterTest.cpp
	vtkMAFImageFillHolesRemoveIslandsTest.h vtkMAFImageFillHolesRemoveIslandsTest.cpp
	vtkMAFRayCastCleanerTest.h vtkMAFRayCastCleanerTest.cpp
	vtkMAFLineStripSourceTest.h vtkMAFLineStripSourceTest.cpp
	vtkMafRGtoSPImageFilterTest.h vtkMafRGtoSPImageFilterTest.cpp
	vtkMAFDistanceFilterTest.h	vtkMAFDistanceFilterTest.cpp
)

ADD_EXECUTABLE(vtkMAFTests ${PROJECT_SRCS})
ADD_TEST(vtkMAFTests  ${EXECUTABLE_OUTPUT_PATH}/vtkMAFTests)
IF (WIN32)
  SET_TARGET_PROPERTIES(vtkMAFTests PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)