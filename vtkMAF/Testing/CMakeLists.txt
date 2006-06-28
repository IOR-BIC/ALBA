LINK_LIBRARIES(vtkMAF vtkGraphics vtkRendering)
#ADD_EXECUTABLE(vtkMAFTest vtkMAFTest.cxx)
#ADD_EXECUTABLE(testDicom testDicom.cxx)
#ADD_TEST(testDicom ${EXECUTABLE_OUTPUT_PATH}/testDicom)
ADD_EXECUTABLE(testVTKVolumeSlicer testVTKVolumeSlicer.cxx)
ADD_TEST(testVTKVolumeSlicer ${EXECUTABLE_OUTPUT_PATH}/testVTKVolumeSlicer)

ADD_EXECUTABLE(vtkContourVolumeMapperTest vtkContourVolumeMapperTest.cpp)
ADD_TEST(testVTKVolumeSlicer ${EXECUTABLE_OUTPUT_PATH}/vtkContourVolumeMapperTest)
