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
	albaToolSelectCellTest.h albaToolSelectCellTest.cpp
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
	albaTransformConstraintTest.h albaTransformConstraintTest.cpp
	albaRefSysTest.h albaRefSysTest.cpp
	albaGizmoTranslateGUITest.h albaGizmoTranslateGUITest.cpp
	albaGizmoRotateGUITest.h albaGizmoRotateGUITest.cpp
	albaGizmoScaleGUITest.h albaGizmoScaleGUITest.cpp
	albaTransformTextEntriesGUITest.h albaTransformTextEntriesGUITest.cpp
	albaSaveRestorePoseGUITest.h albaSaveRestorePoseGUITest.cpp
	albaPositionalEventRouterTest.h albaPositionalEventRouterTest.cpp
	albaGizmoScaleAxisTest.h albaGizmoScaleAxisTest.cpp
	albaGizmoScaleTest.h albaGizmoScaleTest.cpp
	albaToolExtractIsosurfaceTest.h albaToolExtractIsosurfaceTest.cpp
	albaInteractionBehaviorTest.h albaInteractionBehaviorTest.cpp
	albaGizmoInterfaceGUITest.h albaGizmoInterfaceGUITest.cpp
	albaTransformInterfaceGUITest.h albaTransformInterfaceGUITest.cpp
	albaRotateTranslateGUITest.h albaRotateTranslateGUITest.cpp
	albaBehaviorRotateTranslateTest.h albaBehaviorRotateTranslateTest.cpp
	albaToolFlyToTest.h albaToolFlyToTest.cpp
	albaToolSelectTest.h albaToolSelectTest.cpp
	albaGizmoPolylineGraphTest.h albaGizmoPolylineGraphTest.cpp
	albaToolVTKPickerTest.h albaToolVTKPickerTest.cpp
	albaPickHelperTest.h albaPickHelperTest.cpp
	albaInteractionRoutingTest.h albaInteractionRoutingTest.cpp
	albaToolSelectPointTest.h albaToolSelectPointTest.cpp
	albaToolDICOMImporterTest.h albaToolDICOMImporterTest.cpp
)

ADD_EXECUTABLE(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
ADD_TEST(${PROJECT_NAME}  ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME})
IF (WIN32)
  SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)
