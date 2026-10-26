PROJECT (albaViewsTests)

IF(ALBA_BUILD_ALBADLL)
  SET(PROJECT_LIBS albaDLL vtkALBA albaTests) 
ELSE(ALBA_BUILD_ALBADLL)
  SET(PROJECT_LIBS albaTests GPUAPI albaVME albaVisualPipes albaCommon albaViews albaInteraction albaService)
ENDIF(ALBA_BUILD_ALBADLL)

#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#-----------------------------------------------------------------------------
SET(PROJECT_SRCS 
	albaViewsTests.h albaViewsTests.cpp
  albaViewArbitrarySliceTest.h albaViewArbitrarySliceTest.cpp
	albaViewOrthoSliceTest.h albaViewOrthoSliceTest.cpp
	albaViewRXTest.h albaViewRXTest.cpp
	albaViewRXCTTest.h albaViewRXCTTest.cpp
	albaViewSliceTest.h albaViewSliceTest.cpp
	albaViewGlobalSliceTest.h albaViewGlobalSliceTest.cpp
	albaViewImageTest.h albaViewImageTest.cpp
	albaViewRXCompoundTest.h albaViewRXCompoundTest.cpp
	albaView3DTest.h albaView3DTest.cpp
	albaViewImageCompoundTest.h albaViewImageCompoundTest.cpp
	albaViewVTKCompoundTest.h albaViewVTKCompoundTest.cpp
	albaViewSliceOnCurveCompoundTest.h albaViewSliceOnCurveCompoundTest.cpp
	albaViewSliceOnCurveTest.h albaViewSliceOnCurveTest.cpp
	albaViewCompoundWindowingTest.h albaViewCompoundWindowingTest.cpp
)

ADD_EXECUTABLE(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
ADD_TEST(${PROJECT_NAME}  ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME})
IF (WIN32)
  SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)