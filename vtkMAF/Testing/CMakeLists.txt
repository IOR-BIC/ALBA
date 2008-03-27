LINK_LIBRARIES(mafBase vtkMAF vtkGraphics vtkRendering)

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

############ vtkContourVolumeMapperTest
ADD_EXECUTABLE(vtkContourVolumeMapperTest vtkContourVolumeMapperTest.h vtkContourVolumeMapperTest.cpp)
ADD_TEST(vtkContourVolumeMapperTest ${EXECUTABLE_OUTPUT_PATH}/vtkContourVolumeMapperTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(vtkContourVolumeMapperTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ vtkRemoveCellsFilterTest
ADD_EXECUTABLE(vtkRemoveCellsFilterTest vtkRemoveCellsFilterTest.h vtkRemoveCellsFilterTest.cpp)
ADD_TEST(vtkRemoveCellsFilterTest ${EXECUTABLE_OUTPUT_PATH}/vtkRemoveCellsFilterTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(vtkRemoveCellsFilterTest PROPERTIES LINK_FLAGS "/MANIFEST")
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
