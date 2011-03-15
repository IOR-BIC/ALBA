# Load CppUnit stuff
INCLUDE(${MED_SOURCE_DIR}/FindCppUnit.cmake)
IF (CPPUNIT_FOUND)
  #MESSAGE("Found Cppunit library")
  INCLUDE_DIRECTORIES(${CPPUNIT_INCLUDE_DIR})
  LINK_LIBRARIES(${CPPUNIT_LIBRARIES})

  LINK_LIBRARIES(vtkMED vtkGraphics vtkRendering medCommon)
  
ELSE (CPPUNIT_FOUND)
  MESSAGE("Cppunit library not found! Please fill the corresponding cache entries")
ENDIF (CPPUNIT_FOUND)

#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#-----------------------------------------------------------------------------
ADD_EXECUTABLE(vtkHoleConnectivityTest vtkHoleConnectivityTest.h vtkHoleConnectivityTest.cpp)
ADD_TEST(vtkHoleConnectivityTest ${EXECUTABLE_OUTPUT_PATH}/vtkHoleConnectivityTest)
ADD_EXECUTABLE(vtkDicomUnPackerTest vtkDicomUnPackerTest.h vtkDicomUnPackerTest.cpp)
ADD_TEST(vtkDicomUnPackerTest ${EXECUTABLE_OUTPUT_PATH}/vtkDicomUnPackerTest)
ADD_EXECUTABLE(vtkTriangleQualityRatioTest vtkTriangleQualityRatioTest.h vtkTriangleQualityRatioTest.cpp)
ADD_TEST(vtkTriangleQualityRatioTest ${EXECUTABLE_OUTPUT_PATH}/vtkTriangleQualityRatioTest)
ADD_EXECUTABLE(vtkMaskPolyDataFilterTest vtkMaskPolyDataFilterTest.h vtkMaskPolyDataFilterTest.cpp)
ADD_TEST(vtkMaskPolyDataFilterTest ${EXECUTABLE_OUTPUT_PATH}/vtkMaskPolyDataFilterTest)
ADD_EXECUTABLE(vtkMEDExtrudeToCircleTest vtkMEDExtrudeToCircleTest.h vtkMEDExtrudeToCircleTest.cpp)
ADD_TEST(vtkMEDExtrudeToCircleTest ${EXECUTABLE_OUTPUT_PATH}/vtkMEDExtrudeToCircleTest)
ADD_EXECUTABLE(vtkMEDPastValuesListTest vtkMEDPastValuesListTest.h vtkMEDPastValuesListTest.cpp)
ADD_TEST(vtkMEDPastValuesListTest ${EXECUTABLE_OUTPUT_PATH}/vtkMEDPastValuesListTest)
ADD_EXECUTABLE(vtkMEDPolyDataMirrorTest vtkMEDPolyDataMirrorTest.h vtkMEDPolyDataMirrorTest.cpp)
ADD_TEST(vtkMEDPolyDataMirrorTest ${EXECUTABLE_OUTPUT_PATH}/vtkMEDPolyDataMirrorTest)
# ADD_EXECUTABLE(vtkMEDImageUnsharpFilterTest vtkMEDImageUnsharpFilterTest.h vtkMEDImageUnsharpFilterTest.cpp)
# ADD_TEST(vtkMEDImageUnsharpFilterTest ${EXECUTABLE_OUTPUT_PATH}/vtkMEDImageUnsharpFilterTest)
ADD_EXECUTABLE(vtkMEDRegionGrowingLocalGlobalThresholdTest vtkMEDRegionGrowingLocalGlobalThresholdTest.h vtkMEDRegionGrowingLocalGlobalThresholdTest.cpp)
ADD_TEST(vtkMEDRegionGrowingLocalGlobalThresholdTest ${EXECUTABLE_OUTPUT_PATH}/vtkMEDRegionGrowingLocalGlobalThresholdTest)
ADD_EXECUTABLE(vtkImageUnPackerTest vtkImageUnPackerTest.h vtkImageUnPackerTest.cpp)
ADD_TEST(vtkImageUnPackerTest ${EXECUTABLE_OUTPUT_PATH}/vtkImageUnPackerTest)
ADD_EXECUTABLE(vtkPackedImageTest vtkPackedImageTest.h vtkPackedImageTest.cpp)
ADD_TEST(vtkPackedImageTest ${EXECUTABLE_OUTPUT_PATH}/vtkPackedImageTest)
ADD_EXECUTABLE(vtkMAFVolumeRayCastMapperTest vtkMAFVolumeRayCastMapperTest.h vtkMAFVolumeRayCastMapperTest.cpp)
ADD_TEST(vtkMAFVolumeRayCastMapperTest ${EXECUTABLE_OUTPUT_PATH}/vtkMAFVolumeRayCastMapperTest)
ADD_EXECUTABLE(vtkXRayVolumeMapperTest vtkXRayVolumeMapperTest.h vtkXRayVolumeMapperTest.cpp)
ADD_TEST(vtkXRayVolumeMapperTest ${EXECUTABLE_OUTPUT_PATH}/vtkXRayVolumeMapperTest)
ADD_EXECUTABLE(vtkMAFVolumeTextureMapper2DTest vtkMAFVolumeTextureMapper2DTest.h vtkMAFVolumeTextureMapper2DTest.cpp)
ADD_TEST(vtkMAFVolumeTextureMapper2DTest ${EXECUTABLE_OUTPUT_PATH}/vtkMAFVolumeTextureMapper2DTest)
ADD_EXECUTABLE(vtkMAFDataArrayDescriptorTest vtkMAFDataArrayDescriptorTest.h vtkMAFDataArrayDescriptorTest.cpp)
ADD_TEST(vtkMAFDataArrayDescriptorTest ${EXECUTABLE_OUTPUT_PATH}/vtkMAFDataArrayDescriptorTest)
ADD_EXECUTABLE(vtkMEDErythrocyteSourceTest vtkMEDErythrocyteSourceTest.h vtkMEDErythrocyteSourceTest.cpp)
ADD_TEST(vtkMEDErythrocyteSourceTest ${EXECUTABLE_OUTPUT_PATH}/vtkMEDErythrocyteSourceTest)
ADD_EXECUTABLE(vtkMAFMeshCutter_BESTest vtkMAFMeshCutter_BESTest.h vtkMAFMeshCutter_BESTest.cpp)
ADD_TEST(vtkMAFMeshCutter_BESTest ${EXECUTABLE_OUTPUT_PATH}/vtkMAFMeshCutter_BESTest)
ADD_EXECUTABLE(vtkMEDFillingHoleTest vtkMEDFillingHoleTest.h vtkMEDFillingHoleTest.cpp)
ADD_TEST(vtkMEDFillingHoleTest ${EXECUTABLE_OUTPUT_PATH}/vtkMEDFillingHoleTest)
ADD_EXECUTABLE(vtkMEDFixTopologyTest vtkMEDFixTopologyTest.h vtkMEDFixTopologyTest.cpp)
ADD_TEST(vtkMEDFixTopologyTest ${EXECUTABLE_OUTPUT_PATH}/vtkMEDFixTopologyTest)
ADD_EXECUTABLE(vtkWeightedLandmarkTransformTest vtkWeightedLandmarkTransformTest.h vtkWeightedLandmarkTransformTest.cpp)
ADD_TEST(vtkWeightedLandmarkTransformTest ${EXECUTABLE_OUTPUT_PATH}/vtkWeightedLandmarkTransformTest)

ADD_EXECUTABLE(EdgeTest EdgeTest.h EdgeTest.cpp)
ADD_TEST(EdgeTest ${EXECUTABLE_OUTPUT_PATH}/EdgeTest)

ADD_EXECUTABLE(TriangleTest TriangleTest.h TriangleTest.cpp)
ADD_TEST(TriangleTest ${EXECUTABLE_OUTPUT_PATH}/TriangleTest)

ADD_EXECUTABLE(CoredPointIndexTest CoredPointIndexTest.h CoredPointIndexTest.cpp)
ADD_TEST(CoredPointIndexTest ${EXECUTABLE_OUTPUT_PATH}/CoredPointIndexTest)

ADD_EXECUTABLE(EdgeIndexTest EdgeIndexTest.h EdgeIndexTest.cpp)
ADD_TEST(EdgeIndexTest ${EXECUTABLE_OUTPUT_PATH}/EdgeIndexTest)

ADD_EXECUTABLE(CoredEdgeIndexTest CoredEdgeIndexTest.h CoredEdgeIndexTest.cpp)
ADD_TEST(CoredEdgeIndexTest ${EXECUTABLE_OUTPUT_PATH}/CoredEdgeIndexTest)

ADD_EXECUTABLE(TriangleIndexTest TriangleIndexTest.h TriangleIndexTest.cpp)
ADD_TEST(TriangleIndexTest ${EXECUTABLE_OUTPUT_PATH}/TriangleIndexTest)

ADD_EXECUTABLE(TriangulationEdgeTest TriangulationEdgeTest.h TriangulationEdgeTest.cpp)
ADD_TEST(TriangulationEdgeTest ${EXECUTABLE_OUTPUT_PATH}/TriangulationEdgeTest)

ADD_EXECUTABLE(TriangulationTriangleTest TriangulationTriangleTest.h TriangulationTriangleTest.cpp)
ADD_TEST(TriangulationTriangleTest ${EXECUTABLE_OUTPUT_PATH}/TriangulationTriangleTest)

ADD_EXECUTABLE(CMatrixTest CMatrixTest.h CMatrixTest.cpp)
ADD_TEST(CMatrixTest ${EXECUTABLE_OUTPUT_PATH}/CMatrixTest)

ADD_EXECUTABLE(CMatrixTestM1 CMatrixTestM1.h CMatrixTestM1.cpp)
ADD_TEST(CMatrixTestM1 ${EXECUTABLE_OUTPUT_PATH}/CMatrixTestM1)

ADD_EXECUTABLE(CMatrixTestM2 CMatrixTestM2.h CMatrixTestM2.cpp)
ADD_TEST(CMatrixTestM2 ${EXECUTABLE_OUTPUT_PATH}/CMatrixTestM2)

ADD_EXECUTABLE(RingDataTest RingDataTest.h RingDataTest.cpp)
ADD_TEST(RingDataTest ${EXECUTABLE_OUTPUT_PATH}/RingDataTest)

ADD_EXECUTABLE(MeshDataTest MeshDataTest.h MeshDataTest.cpp)
ADD_TEST(MeshDataTest ${EXECUTABLE_OUTPUT_PATH}/MeshDataTest)

ADD_EXECUTABLE(vtkMEDExtrudeToCircleVertexDataTest vtkMEDExtrudeToCircleVertexDataTest.h vtkMEDExtrudeToCircleVertexDataTest.cpp)
ADD_TEST(vtkMEDExtrudeToCircleVertexDataTest ${EXECUTABLE_OUTPUT_PATH}/vtkMEDExtrudeToCircleVertexDataTest)

ADD_EXECUTABLE(vtkMEDFillingHoleCTriangleTest vtkMEDFillingHoleCTriangleTest.h vtkMEDFillingHoleCTriangleTest.cpp)
ADD_TEST(vtkMEDFillingHoleCTriangleTest ${EXECUTABLE_OUTPUT_PATH}/vtkMEDFillingHoleCTriangleTest)

ADD_EXECUTABLE(vtkMEDFillingHoleCEdgeTest vtkMEDFillingHoleCEdgeTest.h vtkMEDFillingHoleCEdgeTest.cpp)
ADD_TEST(vtkMEDFillingHoleCEdgeTest ${EXECUTABLE_OUTPUT_PATH}/vtkMEDFillingHoleCEdgeTest)

ADD_EXECUTABLE(MemoryInfoTest MemoryInfoTest.h MemoryInfoTest.cpp)
ADD_TEST(MemoryInfoTest ${EXECUTABLE_OUTPUT_PATH}/MemoryInfoTest)

ADD_EXECUTABLE(VectorTest VectorTest.h VectorTest.cpp)
ADD_TEST(VectorTest ${EXECUTABLE_OUTPUT_PATH}/VectorTest)

ADD_EXECUTABLE(SparseMatrixTest SparseMatrixTest.h SparseMatrixTest.cpp)
ADD_TEST(SparseMatrixTest ${EXECUTABLE_OUTPUT_PATH}/SparseMatrixTest)

ADD_EXECUTABLE(NVectorTest NVectorTest.h NVectorTest.cpp)
ADD_TEST(NVectorTest ${EXECUTABLE_OUTPUT_PATH}/NVectorTest)

ADD_EXECUTABLE(TreeNodeDataTest TreeNodeDataTest.h TreeNodeDataTest.cpp)
ADD_TEST(TreeNodeDataTest ${EXECUTABLE_OUTPUT_PATH}/TreeNodeDataTest)

ADD_EXECUTABLE(SortedTreeNodesTest SortedTreeNodesTest.h SortedTreeNodesTest.cpp)
ADD_TEST(SortedTreeNodesTest ${EXECUTABLE_OUTPUT_PATH}/SortedTreeNodesTest)

ADD_EXECUTABLE(CubeTest CubeTest.h CubeTest.cpp)
ADD_TEST(CubeTest ${EXECUTABLE_OUTPUT_PATH}/CubeTest)

ADD_EXECUTABLE(SquareTest SquareTest.h SquareTest.cpp)
ADD_TEST(SquareTest ${EXECUTABLE_OUTPUT_PATH}/SquareTest)

ADD_EXECUTABLE(SparseNMatrixTest SparseNMatrixTest.h SparseNMatrixTest.cpp)
ADD_TEST(SparseNMatrixTest ${EXECUTABLE_OUTPUT_PATH}/SparseNMatrixTest)

ADD_EXECUTABLE(VertexDataTest VertexDataTest.h VertexDataTest.cpp)
ADD_TEST(VertexDataTest ${EXECUTABLE_OUTPUT_PATH}/VertexDataTest)

ADD_EXECUTABLE(TriangulationTest TriangulationTest.h TriangulationTest.cpp)
ADD_TEST(TriangulationTest ${EXECUTABLE_OUTPUT_PATH}/TriangulationTest)

ADD_EXECUTABLE(AllocatorTest AllocatorTest.h AllocatorTest.cpp)
ADD_TEST(AllocatorTest ${EXECUTABLE_OUTPUT_PATH}/AllocatorTest)

ADD_EXECUTABLE(BinaryNodeTest BinaryNodeTest.h BinaryNodeTest.cpp)
ADD_TEST(BinaryNodeTest ${EXECUTABLE_OUTPUT_PATH}/BinaryNodeTest)

ADD_EXECUTABLE(PolynomialTest PolynomialTest.h PolynomialTest.cpp)
ADD_TEST(PolynomialTest ${EXECUTABLE_OUTPUT_PATH}/PolynomialTest)

# wxWidgets specific classes
#IF (MAF_USE_WX)
#ENDIF (MAF_USE_WX)

# VTK specific classes
IF (MAF_USE_VTK)
ENDIF (MAF_USE_VTK)  

# classes using VTK+wxWidgets
#IF (MAF_USE_WX AND MAF_USE_VTK)
#ENDIF (MAF_USE_WX AND MAF_USE_VTK)
