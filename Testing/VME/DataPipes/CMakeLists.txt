PROJECT (mafDataPipes)

IF(MAF_BUILD_MAFDLL)
  LINK_LIBRARIES(mafDLL)
ELSE(MAF_BUILD_MAFDLL)
  LINK_LIBRARIES(mafCore vtkMAF)
ENDIF (MAF_BUILD_MAFDLL)
 

#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#-----------------------------------------------------------------------------

############ vtkMAFDataPipeTest
ADD_EXECUTABLE(vtkMAFDataPipeTest vtkMAFDataPipeTest.h vtkMAFDataPipeTest.cpp)
ADD_TEST(vtkMAFDataPipeTest ${EXECUTABLE_OUTPUT_PATH}/vtkMAFDataPipeTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(vtkMAFDataPipeTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafDataPipeInterpolatorTest
ADD_EXECUTABLE(mafDataPipeInterpolatorTest mafDataPipeInterpolatorTest.h mafDataPipeInterpolatorTest.cpp)
ADD_TEST(mafDataPipeInterpolatorTest ${EXECUTABLE_OUTPUT_PATH}/mafDataPipeInterpolatorTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafDataPipeInterpolatorTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafDataPipeInterpolatorVTKTest
ADD_EXECUTABLE(mafDataPipeInterpolatorVTKTest mafDataPipeInterpolatorVTKTest.h mafDataPipeInterpolatorVTKTest.cpp)
ADD_TEST(mafDataPipeInterpolatorVTKTest ${EXECUTABLE_OUTPUT_PATH}/mafDataPipeInterpolatorVTKTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafDataPipeInterpolatorVTKTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafDataPipeCustomTest
ADD_EXECUTABLE(mafDataPipeCustomTest mafDataPipeCustomTest.h mafDataPipeCustomTest.cpp)
ADD_TEST(mafDataPipeCustomTest ${EXECUTABLE_OUTPUT_PATH}/mafDataPipeCustomTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafDataPipeCustomTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafDataPipeInterpolatorScalarMatrixTest
ADD_EXECUTABLE(mafDataPipeInterpolatorScalarMatrixTest mafDataPipeInterpolatorScalarMatrixTest.h mafDataPipeInterpolatorScalarMatrixTest.cpp)
ADD_TEST(mafDataPipeInterpolatorScalarMatrixTest
${EXECUTABLE_OUTPUT_PATH}/mafDataPipeInterpolatorScalarMatrixTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(mafDataPipeInterpolatorScalarMatrixTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafDataPipeCustomProberTest
ADD_EXECUTABLE(mafDataPipeCustomProberTest mafDataPipeCustomProberTest.h mafDataPipeCustomProberTest.cpp)
ADD_TEST(mafDataPipeCustomProberTest
${EXECUTABLE_OUTPUT_PATH}/mafDataPipeCustomProberTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(mafDataPipeCustomProberTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)


