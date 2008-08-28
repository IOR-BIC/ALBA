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
ADD_EXECUTABLE(vtkMEDImageUnsharpFilterTest vtkMEDImageUnsharpFilterTest.h vtkMEDImageUnsharpFilterTest.cpp)
ADD_TEST(vtkMEDImageUnsharpFilterTest ${EXECUTABLE_OUTPUT_PATH}/vtkMEDImageUnsharpFilterTest)




# wxWidgets specific classes
#IF (MAF_USE_WX)
#ENDIF (MAF_USE_WX)

# VTK specific classes
IF (MAF_USE_VTK)
ENDIF (MAF_USE_VTK)  

# classes using VTK+wxWidgets
#IF (MAF_USE_WX AND MAF_USE_VTK)
#ENDIF (MAF_USE_WX AND MAF_USE_VTK)
