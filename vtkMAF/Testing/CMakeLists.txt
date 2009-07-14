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
