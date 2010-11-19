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
#ADD_EXECUTABLE(vtkMEDImageUnsharpFilterTest vtkMEDImageUnsharpFilterTest.h vtkMEDImageUnsharpFilterTest.cpp)
#ADD_TEST(vtkMEDImageUnsharpFilterTest ${EXECUTABLE_OUTPUT_PATH}/vtkMEDImageUnsharpFilterTest)
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

# wxWidgets specific classes
#IF (MAF_USE_WX)
#ENDIF (MAF_USE_WX)

# VTK specific classes
IF (MAF_USE_VTK)
ENDIF (MAF_USE_VTK)  

# classes using VTK+wxWidgets
#IF (MAF_USE_WX AND MAF_USE_VTK)
#ENDIF (MAF_USE_WX AND MAF_USE_VTK)
