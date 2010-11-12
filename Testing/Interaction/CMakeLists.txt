PROJECT (mafInteraction_TEST)

LINK_LIBRARIES(medInteraction)
LINK_LIBRARIES(medVME)
LINK_LIBRARIES(medViews)
LINK_LIBRARIES(medCommon)

#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#-----------------------------------------------------------------------------

############ mafGizmoROI_BESTest
ADD_EXECUTABLE(mafGizmoROI_BESTest mafGizmoROI_BESTest.h mafGizmoROI_BESTest.cpp)
ADD_TEST(mafGizmoROI_BESTest ${EXECUTABLE_OUTPUT_PATH}/mafGizmoROI_BESTest)

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

IF (WIN32)
  SET_TARGET_PROPERTIES(mafGizmoROI_BESTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)