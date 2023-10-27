PROJECT (albaDataPipesTests)

IF(ALBA_BUILD_ALBADLL)
  SET(PROJECT_LIBS albaDLL vtkALBA albaTests) 
ELSE(ALBA_BUILD_ALBADLL)
  SET(PROJECT_LIBS albaTests albaCore vtkALBA albaService albaViews albaVisualPipes)
ENDIF(ALBA_BUILD_ALBADLL)


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

ADD_EXECUTABLE(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
ADD_TEST(${PROJECT_NAME}  ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME})
IF (WIN32)
  SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)
