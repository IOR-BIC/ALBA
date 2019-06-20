PROJECT (albaViews_TEST)

IF(ALBA_BUILD_ALBADLL)
  LINK_LIBRARIES(albaViews)
ELSE(ALBA_BUILD_ALBADLL)
  LINK_LIBRARIES(albaTests GPUAPI albaVME albaVisualPipes albaCommon albaViews albaInteraction albaService)
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
	albaViewGlobalSliceCompoundTest.h albaViewGlobalSliceCompoundTest.cpp
	albaViewImageTest.h albaViewImageTest.cpp
	albaViewRXCompoundTest.h albaViewRXCompoundTest.cpp
	albaView3DTest.h albaView3DTest.cpp
	albaViewImageCompoundTest.h albaViewImageCompoundTest.cpp
	albaViewCTTest.h albaViewCTTest.cpp
	albaViewSlicerTest.h albaViewSlicerTest.cpp
	albaViewVTKCompoundTest.h albaViewVTKCompoundTest.cpp
	albaViewSliceOnCurveCompoundTest.h albaViewSliceOnCurveCompoundTest.cpp
	albaViewSliceOnCurveTest.h albaViewSliceOnCurveTest.cpp
	albaViewSliceGlobalTest.h albaViewSliceGlobalTest.cpp
	albaViewCompoundWindowingTest.h albaViewCompoundWindowingTest.cpp
	albaViewIsosurfaceCompoundTest.h albaViewIsosurfaceCompoundTest.cpp
)

ADD_EXECUTABLE(albaViewsTests ${PROJECT_SRCS})
ADD_TEST(albaViewsTests  ${EXECUTABLE_OUTPUT_PATH}/albaViewsTests)
IF (WIN32)
  SET_TARGET_PROPERTIES(albaViewsTests PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)