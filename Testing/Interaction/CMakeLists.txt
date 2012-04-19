PROJECT (mafInteraction_TEST)

IF(MED_BUILD_MEDDLL)
  LINK_LIBRARIES(medInteraction medViews)
ELSE(MED_BUILD_MEDDLL)
  LINK_LIBRARIES(medVME medCommon medViews medInteraction GPUAPI)
ENDIF (MED_BUILD_MEDDLL)

#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#-----------------------------------------------------------------------------

############ medGizmoPolylineGraphTest
ADD_EXECUTABLE(medGizmoPolylineGraphTest medGizmoPolylineGraphTest.h medGizmoPolylineGraphTest.cpp)
ADD_TEST(medGizmoPolylineGraphTest ${EXECUTABLE_OUTPUT_PATH}/medGizmoPolylineGraphTest)

############ medDeviceButtonsPadMouseDialogTest
ADD_EXECUTABLE(medDeviceButtonsPadMouseDialogTest medDeviceButtonsPadMouseDialogTest.h medDeviceButtonsPadMouseDialogTest.cpp)
ADD_TEST(medDeviceButtonsPadMouseDialogTest ${EXECUTABLE_OUTPUT_PATH}/medDeviceButtonsPadMouseDialogTest)

############ medGUITransformSlidersTest
ADD_EXECUTABLE(medGUITransformSlidersTest medGUITransformSlidersTest.h medGUITransformSlidersTest.cpp)
ADD_TEST(medGUITransformSlidersTest ${EXECUTABLE_OUTPUT_PATH}/medGUITransformSlidersTest)

############ mmiVTKPickerTest
ADD_EXECUTABLE(mmiVTKPickerTest mmiVTKPickerTest.h mmiVTKPickerTest.cpp)
ADD_TEST(mmiVTKPickerTest ${EXECUTABLE_OUTPUT_PATH}/mmiVTKPickerTest)

############ mmiSelectPointTest
ADD_EXECUTABLE(mmiSelectPointTest mmiSelectPointTest.h mmiSelectPointTest.cpp)
ADD_TEST(mmiSelectPointTest ${EXECUTABLE_OUTPUT_PATH}/mmiSelectPointTest)


############ mmiInfoImageTest
ADD_EXECUTABLE(mmiInfoImageTest mmiInfoImageTest.h mmiInfoImageTest.cpp)
ADD_TEST(mmiInfoImageTest ${EXECUTABLE_OUTPUT_PATH}/mmiInfoImageTest)

############ medInteractorDICOMImporterTest
ADD_EXECUTABLE(medInteractorDICOMImporterTest medInteractorDICOMImporterTest.h medInteractorDICOMImporterTest.cpp)
ADD_TEST(medInteractorDICOMImporterTest ${EXECUTABLE_OUTPUT_PATH}/medInteractorDICOMImporterTest)

############ medInteractor2DDistanceTest
ADD_EXECUTABLE(medInteractor2DDistanceTest medInteractor2DDistanceTest.h medInteractor2DDistanceTest.cpp)
ADD_TEST(medInteractor2DDistanceTest ${EXECUTABLE_OUTPUT_PATH}/medInteractor2DDistanceTest) 

############ medInteractor2DAngleTest
ADD_EXECUTABLE(medInteractor2DAngleTest medInteractor2DAngleTest.h medInteractor2DAngleTest.cpp)
ADD_TEST(medInteractor2DAngleTest ${EXECUTABLE_OUTPUT_PATH}/medInteractor2DAngleTest) 

############ medInteractor2DIndicatorTest
ADD_EXECUTABLE(medInteractor2DIndicatorTest medInteractor2DIndicatorTest.h medInteractor2DIndicatorTest.cpp)
ADD_TEST(medInteractor2DIndicatorTest ${EXECUTABLE_OUTPUT_PATH}/medInteractor2DIndicatorTest) 


IF (WIN32)
  SET_TARGET_PROPERTIES(mafGizmoROI_BESTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)