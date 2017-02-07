PROJECT (mafInteraction_TEST)

IF(MAF_BUILD_MAFDLL)
  LINK_LIBRARIES(mafDLL)
ELSE(MAF_BUILD_MAFDLL)
  LINK_LIBRARIES(mafTests mafInteraction mafCore mafVME mafVisualPipes mafBase mafCommon mafViews mafService GPUAPI)
ENDIF (MAF_BUILD_MAFDLL)

#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#-----------------------------------------------------------------------------
SET(PROJECT_SRCS 
	mafInteractionTests.h mafInteractionTests.cpp
	mafGizmoPathTest.h mafGizmoPathTest.cpp
	mafGizmoPathRulerTest.h mafGizmoPathRulerTest.cpp
	mafInteractorSelectCellTest.h mafInteractorSelectCellTest.cpp
	mafGizmoTranslateAxisTest.h mafGizmoTranslateAxisTest.cpp
	mafGizmoTranslatePlaneTest.h mafGizmoTranslatePlaneTest.cpp
	mafGizmoBoundingBoxTest.h mafGizmoBoundingBoxTest.cpp
	mafGizmoScaleIsotropicTest.h mafGizmoScaleIsotropicTest.cpp
	mafGizmoRotateFanTest.h mafGizmoRotateFanTest.cpp
	mafGizmoROITest.h mafGizmoROITest.cpp
	mafGizmoHandleTest.h mafGizmoHandleTest.cpp
	mafAgentTest.h mafAgentTest.cpp
	mafGizmoRotateCircleTest.h mafGizmoRotateCircleTest.cpp
	mafGizmoTranslateTest.h mafGizmoTranslateTest.cpp
	mafGizmoRotateTest.h mafGizmoRotateTest.cpp
	mafGizmoSliceTest.h mafGizmoSliceTest.cpp
	mafGizmoInteractionDebuggerTest.h mafGizmoInteractionDebuggerTest.cpp
	mafGizmoInterfaceTest.h mafGizmoInterfaceTest.cpp
	mafInteractorConstraintTest.h mafInteractorConstraintTest.cpp
	mafRefSysTest.h mafRefSysTest.cpp
	mafDeviceSetTest.h mafDeviceSetTest.cpp
	mafGUIGizmoTranslateTest.h mafGUIGizmoTranslateTest.cpp
	mafGUIGizmoRotateTest.h mafGUIGizmoRotateTest.cpp
	mafGUIGizmoScaleTest.h mafGUIGizmoScaleTest.cpp
	mafEventInteractionTest.h mafEventInteractionTest.cpp
	mafDeviceManagerTest.h mafDeviceManagerTest.cpp
	mafInteractionFactoryTest.h mafInteractionFactoryTest.cpp
	mafGUITransformTextEntriesTest.h mafGUITransformTextEntriesTest.cpp
	mafInteractorCompositorMouseTest.h mafInteractorCompositorMouseTest.cpp
	mafActionTest.h mafActionTest.cpp
	mafGUISaveRestorePoseTest.h mafGUISaveRestorePoseTest.cpp
	mafAgentEventQueueTest.h mafAgentEventQueueTest.cpp
	mafAgentThreadedTest.h mafAgentThreadedTest.cpp
	mafInteractorPERTest.h mafInteractorPERTest.cpp
	mafDeviceTest.h mafDeviceTest.cpp
	mafGizmoScaleAxisTest.h mafGizmoScaleAxisTest.cpp
	mafGizmoScaleTest.h mafGizmoScaleTest.cpp
	#mafDeviceClientMAFTest.h mafDeviceClientMAFTest.cpp
	mafInteractorExtractIsosurfaceTest.h mafInteractorExtractIsosurfaceTest.cpp
	mafInteractorTest.h mafInteractorTest.cpp
	mafInteractor6DOFCameraMoveTest.h mafInteractor6DOFCameraMoveTest.cpp
	mafAvatar2DTest.h mafAvatar2DTest.cpp
	mafAvatarTest.h mafAvatarTest.cpp 
	mafInteractorSERTest.h mafInteractorSERTest.cpp
	mafInteractor6DOFTest.h mafInteractor6DOFTest.cpp
	mafInteractorGeneric6DOFTest.h mafInteractorGeneric6DOFTest.cpp
	mafGUIGizmoInterfaceTest.h mafGUIGizmoInterfaceTest.cpp
	mafGUITransformInterfaceTest.h mafGUITransformInterfaceTest.cpp
	mafAvatar3DConeTest.h mafAvatar3DConeTest.cpp
	mafGUITransformMouseTest.h mafGUITransformMouseTest.cpp
	mafGizmoPolylineGraphTest.h mafGizmoPolylineGraphTest.cpp
	mafDeviceButtonsPadMouseDialogTest.h mafDeviceButtonsPadMouseDialogTest.cpp
	mafGUITransformSlidersTest.h mafGUITransformSlidersTest.cpp
	mmiVTKPickerTest.h mmiVTKPickerTest.cpp
	mmiSelectPointTest.h mmiSelectPointTest.cpp
	mmiInfoImageTest.h mmiInfoImageTest.cpp
	mafInteractorDICOMImporterTest.h mafInteractorDICOMImporterTest.cpp
	medInteractor2DDistanceTest.h medInteractor2DDistanceTest.cpp
	mafInteractor2DAngleTest.h mafInteractor2DAngleTest.cpp
	medInteractor2DIndicatorTest.h medInteractor2DIndicatorTest.cpp
)

ADD_EXECUTABLE(mafInteractionTests ${PROJECT_SRCS})
ADD_TEST(mafInteractionTests  ${EXECUTABLE_OUTPUT_PATH}/mafInteractionTests)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafInteractionTests PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)
