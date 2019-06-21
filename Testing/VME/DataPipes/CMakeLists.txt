PROJECT (albaDataPipes)

IF(ALBA_BUILD_ALBADLL)
  LINK_LIBRARIES(albaDLL)
ELSE(ALBA_BUILD_ALBADLL)
  LINK_LIBRARIES(albaTests albaCore vtkALBA albaService)
ENDIF (ALBA_BUILD_ALBADLL)

#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#-----------------------------------------------------------------------------
SET(PROJECT_SRCS 
	vtkALBADataPipeTest.h 						vtkALBADataPipeTest.cpp
	albaDataPipeInterpolatorTest.h 				albaDataPipeInterpolatorTest.cpp
	albaDataPipeInterpolatorVTKTest.h 			albaDataPipeInterpolatorVTKTest.cpp
	albaDataPipeCustomTest.h 					albaDataPipeCustomTest.cpp
	albaDataPipeInterpolatorScalarMatrixTest.h 	albaDataPipeInterpolatorScalarMatrixTest.cpp
	albaDataPipeCustomProberTest.h 				albaDataPipeCustomProberTest.cpp
	albaDataPipesTests.h 						albaDataPipesTests.cpp
)

ADD_EXECUTABLE(albaDataPipesTests ${PROJECT_SRCS})
ADD_TEST(albaDataPipesTests  ${EXECUTABLE_OUTPUT_PATH}/albaDataPipesTests)
IF (WIN32)
  SET_TARGET_PROPERTIES(albaDataPipesTests PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)
