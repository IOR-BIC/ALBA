#LINK_LIBRARIES(vtkMAF)

#INCLUDE(${MAF_SOURCE_DIR}/Libraries/modules/FindCppUnit.cmake)
#IF (CPPUNIT_FOUND)
#  #MESSAGE("Found Cppunit library")
#  INCLUDE_DIRECTORIES(${CPPUNIT_INCLUDE_DIR})
#  LINK_LIBRARIES(${CPPUNIT_LIBRARIES})
#
#ENDIF (CPPUNIT_FOUND)

PROJECT (vtkMAF_TEST)

IF(MAF_BUILD_MAFDLL)
  LINK_LIBRARIES(mafDLL)
ELSE(MAF_BUILD_MAFDLL)
  LINK_LIBRARIES(vtkMAF vtkGraphics vtkRendering mafCommon)
ENDIF (MAF_BUILD_MAFDLL)


#ADD_EXECUTABLE(vtkMAFTest vtkMAFTest.cxx)
#ADD_EXECUTABLE(testDicom testDicom.cxx)
#ADD_TEST(testDicom ${EXECUTABLE_OUTPUT_PATH}/testDicom)

# ADD_EXECUTABLE(testVTKVolumeSlicer testVTKVolumeSlicer.cxx)
# ADD_TEST(testVTKVolumeSlicer ${EXECUTABLE_OUTPUT_PATH}/testVTKVolumeSlicer)

############ vtkMAFContourVolumeMapperTest
ADD_EXECUTABLE(vtkMAFContourVolumeMapperTest vtkMAFContourVolumeMapperTest.h vtkMAFContourVolumeMapperTest.cpp)
ADD_TEST(vtkMAFContourVolumeMapperTest ${EXECUTABLE_OUTPUT_PATH}/vtkMAFContourVolumeMapperTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(vtkMAFContourVolumeMapperTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ vtkMAFRemoveCellsFilterTest
ADD_EXECUTABLE(vtkMAFRemoveCellsFilterTest vtkMAFRemoveCellsFilterTest.h vtkMAFRemoveCellsFilterTest.cpp)
ADD_TEST(vtkMAFRemoveCellsFilterTest ${EXECUTABLE_OUTPUT_PATH}/vtkMAFRemoveCellsFilterTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(vtkMAFRemoveCellsFilterTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ vtkMAFMeshCutterTest
ADD_EXECUTABLE(vtkMAFMeshCutterTest vtkMAFMeshCutterTest.h vtkMAFMeshCutterTest.cpp)
ADD_TEST(vtkMAFMeshCutterTest ${EXECUTABLE_OUTPUT_PATH}/vtkMAFMeshCutterTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(vtkMAFMeshCutterTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ vtkMAFPolyDataToSinglePolyLineTest
ADD_EXECUTABLE(vtkMAFPolyDataToSinglePolyLineTest vtkMAFPolyDataToSinglePolyLineTest.h vtkMAFPolyDataToSinglePolyLineTest.cpp)
ADD_TEST(vtkMAFPolyDataToSinglePolyLineTest ${EXECUTABLE_OUTPUT_PATH}/vtkMAFPolyDataToSinglePolyLineTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(vtkMAFPolyDataToSinglePolyLineTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ vtkMAFTextOrientatorTest
ADD_EXECUTABLE(vtkMAFTextOrientatorTest vtkMAFTextOrientatorTest.h vtkMAFTextOrientatorTest.cpp)
ADD_TEST(vtkMAFTextOrientatorTest ${EXECUTABLE_OUTPUT_PATH}/vtkMAFTextOrientatorTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(vtkMAFTextOrientatorTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ vtkMAFGlobalAxisCoordinate
ADD_EXECUTABLE(vtkMAFGlobalAxisCoordinateTest vtkMAFGlobalAxisCoordinateTest.h vtkMAFGlobalAxisCoordinateTest.cpp)
ADD_TEST(vtkMAFGlobalAxisCoordinateTest ${EXECUTABLE_OUTPUT_PATH}/vtkMAFGlobalAxisCoordinateTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(vtkMAFGlobalAxisCoordinateTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ vtkMAFGlobalAxisCoordinate
ADD_EXECUTABLE(vtkMAFLocalAxisCoordinateTest vtkMAFLocalAxisCoordinateTest.h vtkMAFLocalAxisCoordinateTest.cpp)
ADD_TEST(vtkMAFLocalAxisCoordinateTest ${EXECUTABLE_OUTPUT_PATH}/vtkMAFLocalAxisCoordinateTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(vtkMAFLocalAxisCoordinateTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ vtkMAFPolyDataSingleSourceShortestPathTest
#ADD_EXECUTABLE(vtkMAFPolyDataSingleSourceShortestPathTest vtkMAFPolyDataSingleSourceShortestPathTest.h vtkMAFPolyDataSingleSourceShortestPathTest.cpp)
#ADD_TEST(vtkMAFPolyDataSingleSourceShortestPathTest ${EXECUTABLE_OUTPUT_PATH}/vtkMAFPolyDataSingleSourceShortestPathTest)
#IF (WIN32)
#  SET_TARGET_PROPERTIES(vtkMAFPolyDataSingleSourceShortestPathTest PROPERTIES LINK_FLAGS "/MANIFEST")
#ENDIF (WIN32)


############ vtkMAFVolumeResampleTest
ADD_EXECUTABLE(vtkMAFVolumeResampleTest vtkMAFVolumeResampleTest.h vtkMAFVolumeResampleTest.cpp)
ADD_TEST(vtkMAFVolumeResampleTest ${EXECUTABLE_OUTPUT_PATH}/vtkMAFVolumeResampleTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(vtkMAFVolumeResampleTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ vtkMAFDOFMatrixTest
ADD_EXECUTABLE(vtkMAFDOFMatrixTest vtkMAFDOFMatrixTest.h vtkMAFDOFMatrixTest.cpp)
ADD_TEST(vtkMAFDOFMatrixTest ${EXECUTABLE_OUTPUT_PATH}/vtkMAFDOFMatrixTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(vtkMAFDOFMatrixTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ vtkMAFImplicitPolyDataTest
ADD_EXECUTABLE(vtkMAFImplicitPolyDataTest vtkMAFImplicitPolyDataTest.h vtkMAFImplicitPolyDataTest.cpp)
ADD_TEST(vtkMAFImplicitPolyDataTest ${EXECUTABLE_OUTPUT_PATH}/vtkMAFImplicitPolyDataTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(vtkMAFImplicitPolyDataTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ vtkMAFTextOrientatorTest
ADD_EXECUTABLE(vtkMAFTextActorMeterTest vtkMAFTextActorMeterTest.h vtkMAFTextActorMeterTest.cpp)
ADD_TEST(vtkMAFTextActorMeterTest ${EXECUTABLE_OUTPUT_PATH}/vtkMAFTextActorMeterTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(vtkMAFTextActorMeterTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ vtkMAFCellsFilterTest
ADD_EXECUTABLE(vtkMAFCellsFilterTest vtkMAFCellsFilterTest.h vtkMAFCellsFilterTest.cpp)
ADD_TEST(vtkMAFCellsFilterTest ${EXECUTABLE_OUTPUT_PATH}/vtkMAFCellsFilterTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(vtkMAFCellsFilterTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ vtkMAFGridActorTest
ADD_EXECUTABLE(vtkMAFGridActorTest vtkMAFGridActorTest.h vtkMAFGridActorTest.cpp)
ADD_TEST(vtkMAFGridActorTest ${EXECUTABLE_OUTPUT_PATH}/vtkMAFGridActorTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(vtkMAFGridActorTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ vtkMAFProjectSPTest
ADD_EXECUTABLE(vtkMAFProjectSPTest vtkMAFProjectSPTest.h vtkMAFProjectSPTest.cpp)
ADD_TEST(vtkMAFProjectSPTest ${EXECUTABLE_OUTPUT_PATH}/vtkMAFProjectSPTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(vtkMAFProjectSPTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ vtkMAFProjectRGTest
ADD_EXECUTABLE(vtkMAFProjectRGTest vtkMAFProjectRGTest.h vtkMAFProjectRGTest.cpp)
ADD_TEST(vtkMAFProjectRGTest ${EXECUTABLE_OUTPUT_PATH}/vtkMAFProjectRGTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(vtkMAFProjectRGTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ vtkMAFClipSurfaceBoundingBoxTest
ADD_EXECUTABLE(vtkMAFClipSurfaceBoundingBoxTest vtkMAFClipSurfaceBoundingBoxTest.h vtkMAFClipSurfaceBoundingBoxTest.cpp)
ADD_TEST(vtkMAFClipSurfaceBoundingBoxTest ${EXECUTABLE_OUTPUT_PATH}/vtkMAFClipSurfaceBoundingBoxTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(vtkMAFClipSurfaceBoundingBoxTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ vtkMAFRGSliceAccumulateTest
ADD_EXECUTABLE(vtkMAFRGSliceAccumulateTest vtkMAFRGSliceAccumulateTest.h vtkMAFRGSliceAccumulateTest.cpp)
ADD_TEST(vtkMAFRGSliceAccumulateTest ${EXECUTABLE_OUTPUT_PATH}/vtkMAFRGSliceAccumulateTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(vtkMAFRGSliceAccumulateTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ vtkMAFRectilinearGridToRectilinearGridFilterTest
ADD_EXECUTABLE(vtkMAFRectilinearGridToRectilinearGridFilterTest vtkMAFRectilinearGridToRectilinearGridFilterTest.h vtkMAFRectilinearGridToRectilinearGridFilterTest.cpp
vtkMAFDummyRectilinearGridToRectilinearGridFilter.h vtkMAFDummyRectilinearGridToRectilinearGridFilter.cpp)
ADD_TEST(vtkMAFRectilinearGridToRectilinearGridFilterTest ${EXECUTABLE_OUTPUT_PATH}/vtkMAFRectilinearGridToRectilinearGridFilterTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(vtkMAFRectilinearGridToRectilinearGridFilterTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ vtkMAFProfilingActorTest
ADD_EXECUTABLE(vtkMAFProfilingActorTest vtkMAFProfilingActorTest.h vtkMAFProfilingActorTest.cpp)
ADD_TEST(vtkMAFProfilingActorTest ${EXECUTABLE_OUTPUT_PATH}/vtkMAFProfilingActorTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(vtkMAFProfilingActorTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ vtkMAFRulerActor2DTest
ADD_EXECUTABLE(vtkMAFRulerActor2DTest vtkMAFRulerActor2DTest.h vtkMAFRulerActor2DTest.cpp)
ADD_TEST(vtkMAFRulerActor2DTest ${EXECUTABLE_OUTPUT_PATH}/vtkMAFRulerActor2DTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(vtkMAFRulerActor2DTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ vtkMAFSimpleRulerActor2DTest
ADD_EXECUTABLE(vtkMAFSimpleRulerActor2DTest vtkMAFSimpleRulerActor2DTest.h vtkMAFSimpleRulerActor2DTest.cpp)
ADD_TEST(vtkMAFSimpleRulerActor2DTest ${EXECUTABLE_OUTPUT_PATH}/vtkMAFSimpleRulerActor2DTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(vtkMAFSimpleRulerActor2DTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ vtkMAFFixedCutterTest
ADD_EXECUTABLE(vtkMAFFixedCutterTest vtkMAFFixedCutterTest.h vtkMAFFixedCutterTest.cpp)
ADD_TEST(vtkMAFFixedCutterTest ${EXECUTABLE_OUTPUT_PATH}/vtkMAFFixedCutterTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(vtkMAFFixedCutterTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ vtkMAFHistogramTest
ADD_EXECUTABLE(vtkMAFHistogramTest vtkMAFHistogramTest.h vtkMAFHistogramTest.cpp)
ADD_TEST(vtkMAFHistogramTest ${EXECUTABLE_OUTPUT_PATH}/vtkMAFHistogramTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(vtkMAFHistogramTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ vtkMAFExtendedGlyph3DTest
ADD_EXECUTABLE(vtkMAFExtendedGlyph3DTest vtkMAFExtendedGlyph3DTest.h vtkMAFExtendedGlyph3DTest.cpp)
ADD_TEST(vtkMAFExtendedGlyph3DTest ${EXECUTABLE_OUTPUT_PATH}/vtkMAFExtendedGlyph3DTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(vtkMAFExtendedGlyph3DTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ vtkMAFTransferFunction2DTest
ADD_EXECUTABLE(vtkMAFTransferFunction2DTest vtkMAFTransferFunction2DTest.h vtkMAFTransferFunction2DTest.cpp)
ADD_TEST(vtkMAFTransferFunction2DTest ${EXECUTABLE_OUTPUT_PATH}/vtkMAFTransferFunction2DTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(vtkMAFTransferFunction2DTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

################# from medical
ADD_EXECUTABLE(vtkHoleConnectivityTest vtkHoleConnectivityTest.h vtkHoleConnectivityTest.cpp)
ADD_TEST(vtkHoleConnectivityTest ${EXECUTABLE_OUTPUT_PATH}/vtkHoleConnectivityTest)
ADD_EXECUTABLE(vtkDicomUnPackerTest vtkDicomUnPackerTest.h vtkDicomUnPackerTest.cpp)
ADD_TEST(vtkDicomUnPackerTest ${EXECUTABLE_OUTPUT_PATH}/vtkDicomUnPackerTest)
ADD_EXECUTABLE(vtkTriangleQualityRatioTest vtkTriangleQualityRatioTest.h vtkTriangleQualityRatioTest.cpp)
ADD_TEST(vtkTriangleQualityRatioTest ${EXECUTABLE_OUTPUT_PATH}/vtkTriangleQualityRatioTest)
ADD_EXECUTABLE(vtkMaskPolyDataFilterTest vtkMaskPolyDataFilterTest.h vtkMaskPolyDataFilterTest.cpp)
ADD_TEST(vtkMaskPolyDataFilterTest ${EXECUTABLE_OUTPUT_PATH}/vtkMaskPolyDataFilterTest)
ADD_EXECUTABLE(vtkMAFExtrudeToCircleTest vtkMAFExtrudeToCircleTest.h vtkMAFExtrudeToCircleTest.cpp)
ADD_TEST(vtkMAFExtrudeToCircleTest ${EXECUTABLE_OUTPUT_PATH}/vtkMAFExtrudeToCircleTest)
ADD_EXECUTABLE(vtkMAFPastValuesListTest vtkMAFPastValuesListTest.h vtkMAFPastValuesListTest.cpp)
ADD_TEST(vtkMAFPastValuesListTest ${EXECUTABLE_OUTPUT_PATH}/vtkMAFPastValuesListTest)
ADD_EXECUTABLE(vtkMAFPolyDataMirrorTest vtkMAFPolyDataMirrorTest.h vtkMAFPolyDataMirrorTest.cpp)
ADD_TEST(vtkMAFPolyDataMirrorTest ${EXECUTABLE_OUTPUT_PATH}/vtkMAFPolyDataMirrorTest)
# ADD_EXECUTABLE(vtkMAFImageUnsharpFilterTest vtkMAFImageUnsharpFilterTest.h vtkMAFImageUnsharpFilterTest.cpp)
# ADD_TEST(vtkMAFImageUnsharpFilterTest ${EXECUTABLE_OUTPUT_PATH}/vtkMAFImageUnsharpFilterTest)
ADD_EXECUTABLE(vtkMAFRegionGrowingLocalGlobalThresholdTest vtkMAFRegionGrowingLocalGlobalThresholdTest.h vtkMAFRegionGrowingLocalGlobalThresholdTest.cpp)
ADD_TEST(vtkMAFRegionGrowingLocalGlobalThresholdTest ${EXECUTABLE_OUTPUT_PATH}/vtkMAFRegionGrowingLocalGlobalThresholdTest)
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
ADD_EXECUTABLE(vtkMAFErythrocyteSourceTest vtkMAFErythrocyteSourceTest.h vtkMAFErythrocyteSourceTest.cpp)
ADD_TEST(vtkMAFErythrocyteSourceTest ${EXECUTABLE_OUTPUT_PATH}/vtkMAFErythrocyteSourceTest)
ADD_EXECUTABLE(vtkMAFMeshCutter_BESTest vtkMAFMeshCutter_BESTest.h vtkMAFMeshCutter_BESTest.cpp)
ADD_TEST(vtkMAFMeshCutter_BESTest ${EXECUTABLE_OUTPUT_PATH}/vtkMAFMeshCutter_BESTest)
ADD_EXECUTABLE(vtkMAFFillingHoleTest vtkMAFFillingHoleTest.h vtkMAFFillingHoleTest.cpp)
ADD_TEST(vtkMAFFillingHoleTest ${EXECUTABLE_OUTPUT_PATH}/vtkMAFFillingHoleTest)
ADD_EXECUTABLE(vtkMAFFixTopologyTest vtkMAFFixTopologyTest.h vtkMAFFixTopologyTest.cpp)
ADD_TEST(vtkMAFFixTopologyTest ${EXECUTABLE_OUTPUT_PATH}/vtkMAFFixTopologyTest)
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

ADD_EXECUTABLE(vtkMAFExtrudeToCircleVertexDataTest vtkMAFExtrudeToCircleVertexDataTest.h vtkMAFExtrudeToCircleVertexDataTest.cpp)
ADD_TEST(vtkMAFExtrudeToCircleVertexDataTest ${EXECUTABLE_OUTPUT_PATH}/vtkMAFExtrudeToCircleVertexDataTest)

ADD_EXECUTABLE(vtkMAFFillingHoleCTriangleTest vtkMAFFillingHoleCTriangleTest.h vtkMAFFillingHoleCTriangleTest.cpp)
ADD_TEST(vtkMAFFillingHoleCTriangleTest ${EXECUTABLE_OUTPUT_PATH}/vtkMAFFillingHoleCTriangleTest)

ADD_EXECUTABLE(vtkMAFFillingHoleCEdgeTest vtkMAFFillingHoleCEdgeTest.h vtkMAFFillingHoleCEdgeTest.cpp)
ADD_TEST(vtkMAFFillingHoleCEdgeTest ${EXECUTABLE_OUTPUT_PATH}/vtkMAFFillingHoleCEdgeTest)

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

ADD_EXECUTABLE(CSkeletonEdgeTest CSkeletonEdgeTest.h CSkeletonEdgeTest.cpp)
ADD_TEST(CSkeletonEdgeTest ${EXECUTABLE_OUTPUT_PATH}/CSkeletonEdgeTest)

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

ADD_EXECUTABLE(StartingPolynomialTest StartingPolynomialTest.h StartingPolynomialTest.cpp)
ADD_TEST(StartingPolynomialTest ${EXECUTABLE_OUTPUT_PATH}/StartingPolynomialTest)

ADD_EXECUTABLE(CSkeletonVertexTest CSkeletonVertexTest.h CSkeletonVertexTest.cpp)
ADD_TEST(CSkeletonVertexTest ${EXECUTABLE_OUTPUT_PATH}/CSkeletonVertexTest)

ADD_EXECUTABLE(PPolynomialTest PPolynomialTest.h PPolynomialTest.cpp)
ADD_TEST(PPolynomialTest ${EXECUTABLE_OUTPUT_PATH}/PPolynomialTest)

ADD_EXECUTABLE(CSkeletonEdgeM1Test CSkeletonEdgeM1Test.h CSkeletonEdgeM1Test.cpp)
ADD_TEST(CSkeletonEdgeM1Test ${EXECUTABLE_OUTPUT_PATH}/CSkeletonEdgeM1Test)

ADD_EXECUTABLE(CSkeletonVertexM1Test CSkeletonVertexM1Test.h CSkeletonVertexM1Test.cpp)
ADD_TEST(CSkeletonVertexM1Test ${EXECUTABLE_OUTPUT_PATH}/CSkeletonVertexM1Test)

ADD_EXECUTABLE(CSkeletonVertexM2Test CSkeletonVertexM2Test.h CSkeletonVertexM2Test.cpp)
ADD_TEST(CSkeletonVertexM2Test ${EXECUTABLE_OUTPUT_PATH}/CSkeletonVertexM2Test)

ADD_EXECUTABLE(vtkMAFCollisionDetectionFilterTest vtkMAFCollisionDetectionFilterTest.h vtkMAFCollisionDetectionFilterTest.cpp)
ADD_TEST(vtkMAFCollisionDetectionFilterTest ${EXECUTABLE_OUTPUT_PATH}/vtkMAFCollisionDetectionFilterTest)

ADD_EXECUTABLE(vtkMAFImageFillHolesRemoveIslandsTest vtkMAFImageFillHolesRemoveIslandsTest.h vtkMAFImageFillHolesRemoveIslandsTest.cpp)
ADD_TEST(vtkMAFImageFillHolesRemoveIslandsTest ${EXECUTABLE_OUTPUT_PATH}/vtkMAFImageFillHolesRemoveIslandsTest)

ADD_EXECUTABLE(vtkMAFRayCastCleanerTest vtkMAFRayCastCleanerTest.h vtkMAFRayCastCleanerTest.cpp)
ADD_TEST(vtkMAFRayCastCleanerTest ${EXECUTABLE_OUTPUT_PATH}/vtkMAFRayCastCleanerTest)

# wxWidgets specific classes
#IF (MAF_USE_WX)
#ENDIF (MAF_USE_WX)

# VTK specific classes
IF (MAF_USE_VTK)
ENDIF (MAF_USE_VTK)  

# classes using VTK+wxWidgets
#IF (MAF_USE_WX AND MAF_USE_VTK)
#ENDIF (MAF_USE_WX AND MAF_USE_VTK)

