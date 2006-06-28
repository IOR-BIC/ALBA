LINK_LIBRARIES(vtkMAF vtkGraphics vtkRendering)

INCLUDE(${MAF_SOURCE_DIR}/Libraries/modules/FindCppUnit.cmake)
IF (CPPUNIT_FOUND)
  #MESSAGE("Found Cppunit library")
  INCLUDE_DIRECTORIES(${CPPUNIT_INCLUDE_DIR})
  LINK_LIBRARIES(${CPPUNIT_LIBRARIES})

ENDIF (CPPUNIT_FOUND)

#ADD_EXECUTABLE(vtkMAFTest vtkMAFTest.cxx)
#ADD_EXECUTABLE(testDicom testDicom.cxx)
#ADD_TEST(testDicom ${EXECUTABLE_OUTPUT_PATH}/testDicom)

# ADD_EXECUTABLE(testVTKVolumeSlicer testVTKVolumeSlicer.cxx)
# ADD_TEST(testVTKVolumeSlicer ${EXECUTABLE_OUTPUT_PATH}/testVTKVolumeSlicer)

ADD_EXECUTABLE(vtkContourVolumeMapperTest vtkContourVolumeMapperTest.cpp)
ADD_TEST(testVTKVolumeSlicer ${EXECUTABLE_OUTPUT_PATH}/vtkContourVolumeMapperTest)
