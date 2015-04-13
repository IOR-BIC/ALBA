PROJECT (mafDataPipes)

IF(MAF_BUILD_MAFDLL)
  LINK_LIBRARIES(mafDLL)
ELSE(MAF_BUILD_MAFDLL)
  LINK_LIBRARIES(mafCore vtkMAF)
ENDIF (MAF_BUILD_MAFDLL)

#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#-----------------------------------------------------------------------------
SET(PROJECT_SRCS 
	vtkMAFDataPipeTest.h vtkMAFDataPipeTest.cpp
	mafDataPipeInterpolatorTest.h mafDataPipeInterpolatorTest.cpp
	mafDataPipeInterpolatorVTKTest.h mafDataPipeInterpolatorVTKTest.cpp
	mafDataPipeCustomTest.h mafDataPipeCustomTest.cpp
	mafDataPipeInterpolatorScalarMatrixTest.h mafDataPipeInterpolatorScalarMatrixTest.cpp
	mafDataPipeCustomProberTest.h mafDataPipeCustomProberTest.cpp
	mafDataPipesTests.h mafDataPipesTests.cpp
)

ADD_EXECUTABLE(mafDataPipesTests ${PROJECT_SRCS})
ADD_TEST(mafDataPipesTests  ${EXECUTABLE_OUTPUT_PATH}/mafDataPipesTests)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafDataPipesTests PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)
