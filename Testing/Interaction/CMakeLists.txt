PROJECT (mafInteraction_TEST)

IF(MAF_BUILD_MAFDLL)
  LINK_LIBRARIES(mafDLL)
ELSE(MAF_BUILD_MAFDLL)
  LINK_LIBRARIES(mafInteraction mafCore mafVME mafBase mafCommon mafViews GPUAPI)
ENDIF (MAF_BUILD_MAFDLL)

#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#-----------------------------------------------------------------------------
############ mafGizmoPathTest
ADD_EXECUTABLE(mafGizmoPathTest mafGizmoPathTest.h mafGizmoPathTest.cpp)
ADD_TEST(mafGizmoPathTest ${EXECUTABLE_OUTPUT_PATH}/mafGizmoPathTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafGizmoPathTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafGizmoPathRulerTest
ADD_EXECUTABLE(mafGizmoPathRulerTest mafGizmoPathRulerTest.h mafGizmoPathRulerTest.cpp)
ADD_TEST(mafGizmoPathRulerTest ${EXECUTABLE_OUTPUT_PATH}/mafGizmoPathRulerTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafGizmoPathRulerTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafInteractorSelectCellTest
ADD_EXECUTABLE(mafInteractorSelectCellTest mafInteractorSelectCellTest.h mafInteractorSelectCellTest.cpp)
ADD_TEST(mafInteractorSelectCellTest ${EXECUTABLE_OUTPUT_PATH}/mafInteractorSelectCellTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafInteractorSelectCellTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafGizmoTranslateAxisTest
ADD_EXECUTABLE(mafGizmoTranslateAxisTest mafGizmoTranslateAxisTest.h mafGizmoTranslateAxisTest.cpp)
ADD_TEST(mafGizmoTranslateAxisTest ${EXECUTABLE_OUTPUT_PATH}/mafGizmoTranslateAxisTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafGizmoTranslateAxisTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafGizmoTranslatePlaneTest
ADD_EXECUTABLE(mafGizmoTranslatePlaneTest mafGizmoTranslatePlaneTest.h mafGizmoTranslatePlaneTest.cpp)
ADD_TEST(mafGizmoTranslatePlaneTest ${EXECUTABLE_OUTPUT_PATH}/mafGizmoTranslatePlaneTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafGizmoTranslatePlaneTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafGizmoBoundingBoxTest
ADD_EXECUTABLE(mafGizmoBoundingBoxTest mafGizmoBoundingBoxTest.h mafGizmoBoundingBoxTest.cpp)
ADD_TEST(mafGizmoBoundingBoxTest ${EXECUTABLE_OUTPUT_PATH}/mafGizmoBoundingBoxTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafGizmoBoundingBoxTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafGizmoScaleIsotropicTest
ADD_EXECUTABLE(mafGizmoScaleIsotropicTest mafGizmoScaleIsotropicTest.h mafGizmoScaleIsotropicTest.cpp)
ADD_TEST(mafGizmoScaleIsotropicTest ${EXECUTABLE_OUTPUT_PATH}/mafGizmoScaleIsotropicTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafGizmoScaleIsotropicTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafGizmoRotateFanTest
ADD_EXECUTABLE(mafGizmoRotateFanTest mafGizmoRotateFanTest.h mafGizmoRotateFanTest.cpp)
ADD_TEST(mafGizmoRotateFanTest ${EXECUTABLE_OUTPUT_PATH}/mafGizmoRotateFanTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafGizmoRotateFanTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)


############ mafGizmoROITest
ADD_EXECUTABLE(mafGizmoROITest mafGizmoROITest.h mafGizmoROITest.cpp)
ADD_TEST(mafGizmoROITest ${EXECUTABLE_OUTPUT_PATH}/mafGizmoROITest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafGizmoROITest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)


############ mafGizmoHandleTest
ADD_EXECUTABLE(mafGizmoHandleTest mafGizmoHandleTest.h mafGizmoHandleTest.cpp)
ADD_TEST(mafGizmoHandleTest ${EXECUTABLE_OUTPUT_PATH}/mafGizmoHandleTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafGizmoHandleTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafAgentTest
ADD_EXECUTABLE(mafAgentTest mafAgentTest.h mafAgentTest.cpp)
ADD_TEST(mafAgentTest ${EXECUTABLE_OUTPUT_PATH}/mafAgentTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafAgentTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)


############ mafGizmoRotateCircleTest
ADD_EXECUTABLE(mafGizmoRotateCircleTest mafGizmoRotateCircleTest.h mafGizmoRotateCircleTest.cpp)
ADD_TEST(mafGizmoRotateCircleTest ${EXECUTABLE_OUTPUT_PATH}/mafGizmoRotateCircleTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafGizmoRotateCircleTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafGizmoTranslateTest
ADD_EXECUTABLE(mafGizmoTranslateTest mafGizmoTranslateTest.h mafGizmoTranslateTest.cpp)
ADD_TEST(mafGizmoTranslateTest ${EXECUTABLE_OUTPUT_PATH}/mafGizmoTranslateTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafGizmoTranslateTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafGizmoRotateTest
ADD_EXECUTABLE(mafGizmoRotateTest mafGizmoRotateTest.h mafGizmoRotateTest.cpp)
ADD_TEST(mafGizmoRotateTest ${EXECUTABLE_OUTPUT_PATH}/mafGizmoRotateTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafGizmoRotateTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafGizmoSliceTest
ADD_EXECUTABLE(mafGizmoSliceTest mafGizmoSliceTest.h mafGizmoSliceTest.cpp)
ADD_TEST(mafGizmoSliceTest  ${EXECUTABLE_OUTPUT_PATH}/mafGizmoSliceTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafGizmoSliceTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ medGizmoInteractionDebuggerTest
ADD_EXECUTABLE(medGizmoInteractionDebuggerTest  medGizmoInteractionDebuggerTest.h medGizmoInteractionDebuggerTest.cpp)
ADD_TEST(medGizmoInteractionDebuggerTest   ${EXECUTABLE_OUTPUT_PATH}/medGizmoInteractionDebuggerTest )
IF (WIN32)
  SET_TARGET_PROPERTIES(medGizmoInteractionDebuggerTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafGizmoInterfaceTest
ADD_EXECUTABLE(mafGizmoInterfaceTest mafGizmoInterfaceTest.h mafGizmoInterfaceTest.cpp)
ADD_TEST(mafGizmoInterfaceTest ${EXECUTABLE_OUTPUT_PATH}/mafGizmoInterfaceTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafGizmoInterfaceTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafInteractorConstraintTest
ADD_EXECUTABLE(mafInteractorConstraintTest mafInteractorConstraintTest.h mafInteractorConstraintTest.cpp)
ADD_TEST(mafInteractorConstraintTest ${EXECUTABLE_OUTPUT_PATH}/mafInteractorConstraintTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafInteractorConstraintTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafRefSysTest
ADD_EXECUTABLE(mafRefSysTest mafRefSysTest.h mafRefSysTest.cpp)
ADD_TEST(mafRefSysTest ${EXECUTABLE_OUTPUT_PATH}/mafRefSysTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafRefSysTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafDeviceButtonsPadTest
ADD_EXECUTABLE(mafDeviceButtonsPadTest mafDeviceButtonsPadTest.h mafDeviceButtonsPadTest.cpp)
ADD_TEST(mafDeviceButtonsPadTest ${EXECUTABLE_OUTPUT_PATH}/mafDeviceButtonsPadTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafDeviceButtonsPadTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafDeviceSetTest
ADD_EXECUTABLE(mafDeviceSetTest mafDeviceSetTest.h mafDeviceSetTest.cpp)
ADD_TEST(mafDeviceSetTest ${EXECUTABLE_OUTPUT_PATH}/mafDeviceSetTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(mafDeviceSetTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafGUIGizmoTranslateTest
ADD_EXECUTABLE(mafGUIGizmoTranslateTest mafGUIGizmoTranslateTest.h mafGUIGizmoTranslateTest.cpp)
ADD_TEST(mafGUIGizmoTranslateTest ${EXECUTABLE_OUTPUT_PATH}/mafGUIGizmoTranslateTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(mafGUIGizmoTranslateTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafGUIGizmoRotateTest
ADD_EXECUTABLE(mafGUIGizmoRotateTest mafGUIGizmoRotateTest.h mafGUIGizmoRotateTest.cpp)
ADD_TEST(mafGUIGizmoRotateTest ${EXECUTABLE_OUTPUT_PATH}/mafGUIGizmoRotateTest)
IF (WIN32)
SET_TARGET_PROPERTIES(mafGUIGizmoRotateTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafGUIGizmoScaleTest
ADD_EXECUTABLE(mafGUIGizmoScaleTest mafGUIGizmoScaleTest.h mafGUIGizmoScaleTest.cpp)
ADD_TEST(mafGUIGizmoScaleTest ${EXECUTABLE_OUTPUT_PATH}/mafGUIGizmoScaleTest)
IF (WIN32)
SET_TARGET_PROPERTIES(mafGUIGizmoScaleTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafEventInteractionTest
ADD_EXECUTABLE(mafEventInteractionTest mafEventInteractionTest.h mafEventInteractionTest.cpp)
ADD_TEST(mafEventInteractionTest ${EXECUTABLE_OUTPUT_PATH}/mafEventInteractionTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(mafEventInteractionTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafDeviceManagerTest
ADD_EXECUTABLE(mafDeviceManagerTest mafDeviceManagerTest.h mafDeviceManagerTest.cpp)
ADD_TEST(mafDeviceManagerTest ${EXECUTABLE_OUTPUT_PATH}/mafDeviceManagerTest)
IF (WIN32)                                            
 SET_TARGET_PROPERTIES(mafDeviceManagerTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)         

############ mafInteractionFactoryTest
ADD_EXECUTABLE(mafInteractionFactoryTest mafInteractionFactoryTest.h mafInteractionFactoryTest.cpp )
ADD_TEST(mafInteractionFactoryTest ${EXECUTABLE_OUTPUT_PATH}/mafInteractionFactoryTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(mafInteractionFactoryTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafGUITransformTextEntriesTest
ADD_EXECUTABLE(mafGUITransformTextEntriesTest mafGUITransformTextEntriesTest.h mafGUITransformTextEntriesTest.cpp)
ADD_TEST(mafGUITransformTextEntriesTest ${EXECUTABLE_OUTPUT_PATH}/mafGUITransformTextEntriesTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafGUITransformTextEntriesTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafInteractorCompositorMouseTest
ADD_EXECUTABLE(mafInteractorCompositorMouseTest mafInteractorCompositorMouseTest.h mafInteractorCompositorMouseTest.cpp)
ADD_TEST(mafInteractorCompositorMouseTest ${EXECUTABLE_OUTPUT_PATH}/mafInteractorCompositorMouseTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafInteractorCompositorMouseTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafActionTest
ADD_EXECUTABLE(mafActionTest mafActionTest.h mafActionTest.cpp)
ADD_TEST(mafActionTest ${EXECUTABLE_OUTPUT_PATH}/mafActionTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafActionTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafGUISaveRestorePoseTest
ADD_EXECUTABLE(mafGUISaveRestorePoseTest mafGUISaveRestorePoseTest.h mafGUISaveRestorePoseTest.cpp)
ADD_TEST(mafGUISaveRestorePoseTest ${EXECUTABLE_OUTPUT_PATH}/mafGUISaveRestorePoseTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafGUISaveRestorePoseTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafAgentEventQueueTest.h
ADD_EXECUTABLE(mafAgentEventQueueTest mafAgentEventQueueTest.h mafAgentEventQueueTest.cpp)
ADD_TEST(mafAgentEventQueueTest ${EXECUTABLE_OUTPUT_PATH}/mafAgentEventQueueTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafAgentEventQueueTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafAgentThreadedTest.h
ADD_EXECUTABLE(mafAgentThreadedTest mafAgentThreadedTest.h mafAgentThreadedTest.cpp)
ADD_TEST(mafAgentThreadedTest ${EXECUTABLE_OUTPUT_PATH}/mafAgentThreadedTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafAgentThreadedTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafDeviceButtonsPadMouseTest.h
ADD_EXECUTABLE(mafDeviceButtonsPadMouseTest mafDeviceButtonsPadMouseTest.h mafDeviceButtonsPadMouseTest.cpp)
ADD_TEST(mafDeviceButtonsPadMouseTest ${EXECUTABLE_OUTPUT_PATH}/mafDeviceButtonsPadMouseTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafDeviceButtonsPadMouseTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafInteractorPERTest
ADD_EXECUTABLE(mafInteractorPERTest mafInteractorPERTest.h mafInteractorPERTest.cpp)
ADD_TEST(mafInteractorPERTest ${EXECUTABLE_OUTPUT_PATH}/mafInteractorPERTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(mafInteractorPERTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafDeviceTest
ADD_EXECUTABLE(mafDeviceTest mafDeviceTest.h mafDeviceTest.cpp)
ADD_TEST(mafDeviceTest ${EXECUTABLE_OUTPUT_PATH}/mafDeviceTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(mafDeviceTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafGizmoScaleAxisTest
ADD_EXECUTABLE(mafGizmoScaleAxisTest mafGizmoScaleAxisTest.h mafGizmoScaleAxisTest.cpp)
ADD_TEST(mafGizmoScaleAxisTest ${EXECUTABLE_OUTPUT_PATH}/mafGizmoScaleAxisTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(mafGizmoScaleAxisTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafGizmoScaleTest
ADD_EXECUTABLE(mafGizmoScaleTest mafGizmoScaleTest.h mafGizmoScaleTest.cpp)
ADD_TEST(mafGizmoScaleTest ${EXECUTABLE_OUTPUT_PATH}/mafGizmoScaleTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(mafGizmoScaleTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafDeviceClientMAFTest
ADD_EXECUTABLE(mafDeviceClientMAFTest mafDeviceClientMAFTest.h mafDeviceClientMAFTest.cpp)
ADD_TEST(mafDeviceClientMAFTest ${EXECUTABLE_OUTPUT_PATH}/mafDeviceClientMAFTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(mafDeviceClientMAFTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafInteractorExtractIsosurfaceTest
ADD_EXECUTABLE(mafInteractorExtractIsosurfaceTest mafInteractorExtractIsosurfaceTest.h mafInteractorExtractIsosurfaceTest.cpp)
ADD_TEST(mafInteractorExtractIsosurfaceTest ${EXECUTABLE_OUTPUT_PATH}/mafInteractorExtractIsosurfaceTest )
IF (WIN32)
 SET_TARGET_PROPERTIES(mafInteractorExtractIsosurfaceTest  PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafInteractorTest
ADD_EXECUTABLE(mafInteractorTest mafInteractorTest.h mafInteractorTest.cpp)
ADD_TEST(mafInteractorTest ${EXECUTABLE_OUTPUT_PATH}/mafInteractorTest )
IF (WIN32)
 SET_TARGET_PROPERTIES(mafInteractorTest  PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafDeviceButtonsPadMouseRemoteTest
ADD_EXECUTABLE(mafDeviceButtonsPadMouseRemoteTest mafDeviceButtonsPadMouseRemoteTest.h mafDeviceButtonsPadMouseRemoteTest.cpp)
ADD_TEST(mafDeviceButtonsPadMouseRemoteTest ${EXECUTABLE_OUTPUT_PATH}/mafDeviceButtonsPadMouseRemoteTest )
IF (WIN32)
 SET_TARGET_PROPERTIES(mafDeviceButtonsPadMouseRemoteTest  PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafInteractor6DOFCameraMoveTest.h
ADD_EXECUTABLE(mafInteractor6DOFCameraMoveTest mafInteractor6DOFCameraMoveTest.h mafInteractor6DOFCameraMoveTest.cpp)
ADD_TEST(mafInteractor6DOFCameraMoveTest ${EXECUTABLE_OUTPUT_PATH}/mafInteractor6DOFCameraMoveTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(mafInteractor6DOFCameraMoveTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafAvatar2DTest.h
ADD_EXECUTABLE(mafAvatar2DTest mafAvatar2DTest.h mafAvatar2DTest.cpp) 
ADD_TEST(mafAvatar2DTest ${EXECUTABLE_OUTPUT_PATH}/mafAvatar2DTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(mafAvatar2DTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafAvatarTest.h
ADD_EXECUTABLE(mafAvatarTest mafAvatarTest.h mafAvatarTest.cpp) 
ADD_TEST(mafAvatarTest ${EXECUTABLE_OUTPUT_PATH}/mafAvatarTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(mafAvatarTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafInteractorSERTest.h
ADD_EXECUTABLE(mafInteractorSERTest mafInteractorSERTest.h
mafInteractorSERTest.cpp)
ADD_TEST(mafInteractorSERTest ${EXECUTABLE_OUTPUT_PATH}/mafInteractorSERTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(mafInteractorSERTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafInteractor6DOFTest
ADD_EXECUTABLE(mafInteractor6DOFTest mafInteractor6DOFTest.h mafInteractor6DOFTest.cpp)
ADD_TEST(mafInteractor6DOFTest ${EXECUTABLE_OUTPUT_PATH}/mafInteractor6DOFTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(mafInteractor6DOFTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafInteractorGeneric6DOFTest
ADD_EXECUTABLE(mafInteractorGeneric6DOFTest mafInteractorGeneric6DOFTest.h mafInteractorGeneric6DOFTest.cpp)
ADD_TEST(mafInteractorGeneric6DOFTest ${EXECUTABLE_OUTPUT_PATH}/mafInteractorGeneric6DOFTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(mafInteractorGeneric6DOFTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafGUIGizmoInterfaceTest
ADD_EXECUTABLE(mafGUIGizmoInterfaceTest mafGUIGizmoInterfaceTest.h mafGUIGizmoInterfaceTest.cpp)
ADD_TEST(mafGUIGizmoInterfaceTest ${EXECUTABLE_OUTPUT_PATH}/mafGUIGizmoInterfaceTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(mafGUIGizmoInterfaceTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafGUITransformInterfaceTest
ADD_EXECUTABLE(mafGUITransformInterfaceTest mafGUITransformInterfaceTest.h mafGUITransformInterfaceTest.cpp)
ADD_TEST(mafGUITransformInterfaceTest ${EXECUTABLE_OUTPUT_PATH}/mafGUITransformInterfaceTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(mafGUITransformInterfaceTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafAvatar3DConeTest
ADD_EXECUTABLE(mafAvatar3DConeTest mafAvatar3DConeTest.h mafAvatar3DConeTest.cpp)
ADD_TEST(mafAvatar3DConeTest ${EXECUTABLE_OUTPUT_PATH}/mafAvatar3DConeTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(mafAvatar3DConeTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafGUITransformMouseTest
ADD_EXECUTABLE(mafGUITransformMouseTest mafGUITransformMouseTest.h mafGUITransformMouseTest.cpp)
ADD_TEST(mafGUITransformMouseTest ${EXECUTABLE_OUTPUT_PATH}/mafGUITransformMouseTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(mafGUITransformMouseTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############### from medical

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
