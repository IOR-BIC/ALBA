PROJECT (mafDataPipes)

LINK_LIBRARIES(mafCore vtkMAF)
 

#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#-----------------------------------------------------------------------------

############ vtkMAFDataPipeTest
ADD_EXECUTABLE(vtkMAFDataPipeTest vtkMAFDataPipeTest.h vtkMAFDataPipeTest.cpp)
ADD_TEST(vtkMAFDataPipeTest ${EXECUTABLE_OUTPUT_PATH}/vtkMAFDataPipeTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(vtkMAFDataPipeTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafDataInterpolatorTest
ADD_EXECUTABLE(mafDataInterpolatorTest mafDataInterpolatorTest.h mafDataInterpolatorTest.cpp)
ADD_TEST(mafDataInterpolatorTest ${EXECUTABLE_OUTPUT_PATH}/mafDataInterpolatorTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafDataInterpolatorTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVTKInterpolatorTest
ADD_EXECUTABLE(mafVTKInterpolatorTest mafVTKInterpolatorTest.h mafVTKInterpolatorTest.cpp)
ADD_TEST(mafVTKInterpolatorTest ${EXECUTABLE_OUTPUT_PATH}/mafVTKInterpolatorTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVTKInterpolatorTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafDataPipeCustomTest
ADD_EXECUTABLE(mafDataPipeCustomTest mafDataPipeCustomTest.h mafDataPipeCustomTest.cpp)
ADD_TEST(mafDataPipeCustomTest ${EXECUTABLE_OUTPUT_PATH}/mafDataPipeCustomTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafDataPipeCustomTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)
