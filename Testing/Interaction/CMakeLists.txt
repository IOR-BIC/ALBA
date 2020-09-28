PROJECT (albaInteractionTests)

IF(ALBA_BUILD_ALBADLL)
  SET(PROJECT_LIBS albaDLL vtkALBA albaTests)
ELSE(ALBA_BUILD_ALBADLL)
  SET(PROJECT_LIBS albaTests albaInteraction albaCore albaVME albaVisualPipes albaBase albaCommon albaViews albaService GPUAPI)
ENDIF(ALBA_BUILD_ALBADLL)

#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#-----------------------------------------------------------------------------
SET(PROJECT_SRCS 
	albaInteractionTests.h albaInteractionTests.cpp
	albaGizmoPathTest.h albaGizmoPathTest.cpp
	albaGizmoPathRulerTest.h albaGizmoPathRulerTest.cpp
	albaInteractorSelectCellTest.h albaInteractorSelectCellTest.cpp
	albaGizmoTranslateAxisTest.h albaGizmoTranslateAxisTest.cpp
	albaGizmoTranslatePlaneTest.h albaGizmoTranslatePlaneTest.cpp
	albaGizmoBoundingBoxTest.h albaGizmoBoundingBoxTest.cpp
	albaGizmoScaleIsotropicTest.h albaGizmoScaleIsotropicTest.cpp
	albaGizmoRotateFanTest.h albaGizmoRotateFanTest.cpp
	albaGizmoROITest.h albaGizmoROITest.cpp
	albaGizmoHandleTest.h albaGizmoHandleTest.cpp
	albaAgentTest.h albaAgentTest.cpp
	albaGizmoRotateCircleTest.h albaGizmoRotateCircleTest.cpp
	albaGizmoTranslateTest.h albaGizmoTranslateTest.cpp
	albaGizmoRotateTest.h albaGizmoRotateTest.cpp
	albaGizmoSliceTest.h albaGizmoSliceTest.cpp
	albaGizmoInteractionDebuggerTest.h albaGizmoInteractionDebuggerTest.cpp
	albaGizmoInterfaceTest.h albaGizmoInterfaceTest.cpp
	albaInteractorConstraintTest.h albaInteractorConstraintTest.cpp
	albaRefSysTest.h albaRefSysTest.cpp
	albaDeviceSetTest.h albaDeviceSetTest.cpp
	albaGUIGizmoTranslateTest.h albaGUIGizmoTranslateTest.cpp
	albaGUIGizmoRotateTest.h albaGUIGizmoRotateTest.cpp
	albaGUIGizmoScaleTest.h albaGUIGizmoScaleTest.cpp
	albaEventInteractionTest.h albaEventInteractionTest.cpp
	albaDeviceManagerTest.h albaDeviceManagerTest.cpp
	albaInteractionFactoryTest.h albaInteractionFactoryTest.cpp
	albaGUITransformTextEntriesTest.h albaGUITransformTextEntriesTest.cpp
	albaInteractorCompositorMouseTest.h albaInteractorCompositorMouseTest.cpp
	albaActionTest.h albaActionTest.cpp
	albaGUISaveRestorePoseTest.h albaGUISaveRestorePoseTest.cpp
	albaAgentEventQueueTest.h albaAgentEventQueueTest.cpp
	albaAgentThreadedTest.h albaAgentThreadedTest.cpp
	albaInteractorPERTest.h albaInteractorPERTest.cpp
	albaDeviceTest.h albaDeviceTest.cpp
	albaGizmoScaleAxisTest.h albaGizmoScaleAxisTest.cpp
	albaGizmoScaleTest.h albaGizmoScaleTest.cpp
	#albaDeviceClientALBATest.h albaDeviceClientALBATest.cpp
	albaInteractorExtractIsosurfaceTest.h albaInteractorExtractIsosurfaceTest.cpp
	albaInteractorTest.h albaInteractorTest.cpp
	albaInteractor6DOFCameraMoveTest.h albaInteractor6DOFCameraMoveTest.cpp
	albaAvatar2DTest.h albaAvatar2DTest.cpp
	albaAvatarTest.h albaAvatarTest.cpp 
	albaInteractorSERTest.h albaInteractorSERTest.cpp
	albaInteractor6DOFTest.h albaInteractor6DOFTest.cpp
	albaInteractorGeneric6DOFTest.h albaInteractorGeneric6DOFTest.cpp
	albaGUIGizmoInterfaceTest.h albaGUIGizmoInterfaceTest.cpp
	albaGUITransformInterfaceTest.h albaGUITransformInterfaceTest.cpp
	albaAvatar3DConeTest.h albaAvatar3DConeTest.cpp
	albaGUITransformMouseTest.h albaGUITransformMouseTest.cpp
	albaGizmoPolylineGraphTest.h albaGizmoPolylineGraphTest.cpp
	albaDeviceButtonsPadMouseDialogTest.h albaDeviceButtonsPadMouseDialogTest.cpp
	albaGUITransformSlidersTest.h albaGUITransformSlidersTest.cpp
	mmiVTKPickerTest.h mmiVTKPickerTest.cpp
	mmiSelectPointTest.h mmiSelectPointTest.cpp
	mmiInfoImageTest.h mmiInfoImageTest.cpp
	albaInteractorDICOMImporterTest.h albaInteractorDICOMImporterTest.cpp
	albaInteractor2DDistanceTest.h albaInteractor2DDistanceTest.cpp
	albaInteractor2DAngleTest.h albaInteractor2DAngleTest.cpp
	albaInteractor2DIndicatorTest.h albaInteractor2DIndicatorTest.cpp
	albaInteractorSliderTest.h	albaInteractorSliderTest.cpp
)

ADD_EXECUTABLE(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
ADD_TEST(${PROJECT_NAME}  ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME})
IF (WIN32)
  SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)
