PROJECT (MedicalViews_TEST)

IF(MAF_BUILD_MAFDLL)
  LINK_LIBRARIES(mafViews)
ELSE(MAF_BUILD_MAFDLL)
  LINK_LIBRARIES(GPUAPI mafVME mafCommon mafViews mafInteraction)
ENDIF(MAF_BUILD_MAFDLL)

#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#-----------------------------------------------------------------------------

ADD_EXECUTABLE(mafViewArbitrarySliceTest mafViewArbitrarySliceTest.h mafViewArbitrarySliceTest.cpp)
ADD_TEST(mafViewArbitrarySliceTest ${EXECUTABLE_OUTPUT_PATH}/mafViewArbitrarySliceTest)

ADD_EXECUTABLE(mafViewOrthoSliceTest mafViewOrthoSliceTest.h mafViewOrthoSliceTest.cpp)
ADD_TEST(mafViewOrthoSliceTest ${EXECUTABLE_OUTPUT_PATH}/mafViewOrthoSliceTest)

ADD_EXECUTABLE(mafViewRXTest mafViewRXTest.h mafViewRXTest.cpp)
ADD_TEST(mafViewRXTest ${EXECUTABLE_OUTPUT_PATH}/mafViewRXTest)

ADD_EXECUTABLE(mafViewRXCTTest mafViewRXCTTest.h mafViewRXCTTest.cpp)
ADD_TEST(mafViewRXCTTest ${EXECUTABLE_OUTPUT_PATH}/mafViewRXCTTest)

ADD_EXECUTABLE(mafViewSliceTest mafViewSliceTest.h mafViewSliceTest.cpp)
ADD_TEST(mafViewSliceTest ${EXECUTABLE_OUTPUT_PATH}/mafViewSliceTest)

ADD_EXECUTABLE(mafViewGlobalSliceTest mafViewGlobalSliceTest.h mafViewGlobalSliceTest.cpp)
ADD_TEST(mafViewGlobalSliceTest ${EXECUTABLE_OUTPUT_PATH}/mafViewGlobalSliceTest)

ADD_EXECUTABLE(mafViewGlobalSliceCompoundTest mafViewGlobalSliceCompoundTest.h mafViewGlobalSliceCompoundTest.cpp)
ADD_TEST(mafViewGlobalSliceCompoundTest ${EXECUTABLE_OUTPUT_PATH}/mafViewGlobalSliceCompoundTest)

ADD_EXECUTABLE(mafViewImageTest mafViewImageTest.h mafViewImageTest.cpp)
ADD_TEST(mafViewImageTest ${EXECUTABLE_OUTPUT_PATH}/mafViewImageTest)

ADD_EXECUTABLE(mafViewSingleSliceTest mafViewSingleSliceTest.h mafViewSingleSliceTest.cpp)
ADD_TEST(mafViewSingleSliceTest ${EXECUTABLE_OUTPUT_PATH}/mafViewSingleSliceTest)

ADD_EXECUTABLE(mafViewSingleSliceCompoundTest mafViewSingleSliceCompoundTest.h mafViewSingleSliceCompoundTest.cpp)
ADD_TEST(mafViewSingleSliceCompoundTest ${EXECUTABLE_OUTPUT_PATH}/mafViewSingleSliceCompoundTest)

ADD_EXECUTABLE(mafViewRXCompoundTest mafViewRXCompoundTest.h mafViewRXCompoundTest.cpp)
ADD_TEST(mafViewRXCompoundTest ${EXECUTABLE_OUTPUT_PATH}/mafViewRXCompoundTest)

ADD_EXECUTABLE(mafView3DTest mafView3DTest.h mafView3DTest.cpp)
ADD_TEST(mafView3DTest ${EXECUTABLE_OUTPUT_PATH}/mafView3DTest)

ADD_EXECUTABLE(mafViewImageCompoundTest mafViewImageCompoundTest.h mafViewImageCompoundTest.cpp)
ADD_TEST(mafViewImageCompoundTest ${EXECUTABLE_OUTPUT_PATH}/mafViewImageCompoundTest)

ADD_EXECUTABLE(mafViewCTTest mafViewCTTest.h mafViewCTTest.cpp)
ADD_TEST(mafViewCTTest ${EXECUTABLE_OUTPUT_PATH}/mafViewCTTest)

ADD_EXECUTABLE(medViewSlicerTest medViewSlicerTest.h medViewSlicerTest.cpp)
ADD_TEST(medViewSlicerTest ${EXECUTABLE_OUTPUT_PATH}/medViewSlicerTest)

ADD_EXECUTABLE(medViewVTKCompoundTest medViewVTKCompoundTest.h medViewVTKCompoundTest.cpp)
ADD_TEST(medViewVTKCompoundTest ${EXECUTABLE_OUTPUT_PATH}/medViewVTKCompoundTest)

ADD_EXECUTABLE(medViewSliceOnCurveCompoundTest medViewSliceOnCurveCompoundTest.h medViewSliceOnCurveCompoundTest.cpp)
ADD_TEST(medViewSliceOnCurveCompoundTest ${EXECUTABLE_OUTPUT_PATH}/medViewSliceOnCurveCompoundTest)

ADD_EXECUTABLE(medViewSliceOnCurveTest medViewSliceOnCurveTest.h medViewSliceOnCurveTest.cpp)
ADD_TEST(medViewSliceOnCurveTest ${EXECUTABLE_OUTPUT_PATH}/medViewSliceOnCurveTest)

ADD_EXECUTABLE(medViewSliceGlobalTest medViewSliceGlobalTest.h medViewSliceGlobalTest.cpp)
ADD_TEST(medViewSliceGlobalTest ${EXECUTABLE_OUTPUT_PATH}/medViewSliceGlobalTest)

ADD_EXECUTABLE(medViewCompoundWindowingTest medViewCompoundWindowingTest.h medViewCompoundWindowingTest.cpp)
ADD_TEST(medViewCompoundWindowingTest ${EXECUTABLE_OUTPUT_PATH}/medViewCompoundWindowingTest)

# wxWidgets specific classes
#IF (MAF_USE_WX)
#ENDIF (MAF_USE_WX)

# VTK specific classes
IF (MAF_USE_VTK)
ENDIF (MAF_USE_VTK)  

# classes using VTK+wxWidgets
#IF (MAF_USE_WX AND MAF_USE_VTK)
#ENDIF (MAF_USE_WX AND MAF_USE_VTK)