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
	albaSelectCellToolTest.h albaSelectCellToolTest.cpp
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
	albaCompositorMouseBehaviorTest.h albaCompositorMouseBehaviorTest.cpp
	albaActionTest.h albaActionTest.cpp
	albaGUISaveRestorePoseTest.h albaGUISaveRestorePoseTest.cpp
	albaAgentEventQueueTest.h albaAgentEventQueueTest.cpp
	albaAgentThreadedTest.h albaAgentThreadedTest.cpp
	albaPositionalEventRouterTest.h albaPositionalEventRouterTest.cpp
	albaDeviceTest.h albaDeviceTest.cpp
	albaGizmoScaleAxisTest.h albaGizmoScaleAxisTest.cpp
	albaGizmoScaleTest.h albaGizmoScaleTest.cpp
	#albaDeviceClientALBATest.h albaDeviceClientALBATest.cpp
	albaExtractIsosurfaceToolTest.h albaExtractIsosurfaceToolTest.cpp
	albaInteractionBehaviorTest.h albaInteractionBehaviorTest.cpp
	albaAvatarTest.h albaAvatarTest.cpp 
	albaStaticEventRouterTest.h albaStaticEventRouterTest.cpp
	albaGUIGizmoInterfaceTest.h albaGUIGizmoInterfaceTest.cpp
	albaGUITransformInterfaceTest.h albaGUITransformInterfaceTest.cpp
	albaAvatar3DConeTest.h albaAvatar3DConeTest.cpp
	albaGUITransformMouseTest.h albaGUITransformMouseTest.cpp
	albaGizmoPolylineGraphTest.h albaGizmoPolylineGraphTest.cpp
	albaGUITransformSlidersTest.h albaGUITransformSlidersTest.cpp
	albaVTKPickerToolTest.h albaVTKPickerToolTest.cpp
	albaPickHelperTest.h albaPickHelperTest.cpp
	albaInteractionRoutingTest.h albaInteractionRoutingTest.cpp
	albaSelectPointToolTest.h albaSelectPointToolTest.cpp
	albaDICOMImporterToolTest.h albaDICOMImporterToolTest.cpp
)

ADD_EXECUTABLE(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
ADD_TEST(${PROJECT_NAME}  ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME})
IF (WIN32)
  SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)
