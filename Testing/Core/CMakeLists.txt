PROJECT (albaCoreTests)

IF(ALBA_BUILD_ALBADLL)
  SET(PROJECT_LIBS albaDLL vtkALBA albaTests albaViews)
ELSE(ALBA_BUILD_ALBADLL)
	SET(PROJECT_LIBS albaTests GPUAPI albaCore albaVME albaVisualPipes albaCommon albaViews albaInteraction albaService)
ENDIF(ALBA_BUILD_ALBADLL)

# Set your list of sources here.
SET(PROJECT_SRCS 
  albaCoreTests.h albaCoreTests.cpp
	albaViewVTKTest.h albaViewVTKTest.cpp
	albaViewHTMLTest.h albaViewHTMLTest.cpp
	albaAttributeTest.h albaAttributeTest.cpp
	albaTagItemTest.h albaTagItemTest.cpp
	albaTagArrayTest.h albaTagArrayTest.cpp
	albaVMETest.h albaVMETest.cpp
	albaTimeMapTest.h albaTimeMapTest.cpp
	albaVMEFactoryTest.h albaVMEFactoryTest.cpp
	mmaApplicationLayoutTest.h mmaApplicationLayoutTest.cpp
	albaVMETest.h albaVMETest.cpp
	albaOpTest.h albaOpTest.cpp
	albaEventIOTest.h albaEventIOTest.cpp
	albaUserTest.h albaUserTest.cpp
	albaLODActorTest.h albaLODActorTest.cpp
	albaTimeMapScalarTest.h albaTimeMapScalarTest.cpp
	albaOpSelectTest.h albaOpSelectTest.cpp
	albaVMERootTest.h albaVMERootTest.cpp
	albaOpCutTest.h albaOpCutTest.cpp
	albaOpCopyTest.h albaOpCopyTest.cpp
	albaOpPasteTest.h albaOpPasteTest.cpp
	albaOpEditTest.h albaOpEditTest.cpp
	albaOpContextStackTest.h albaOpContextStackTest.cpp
	albaVMEIteratorTest.h albaVMEIteratorTest.cpp
	albaPipeTest.h albaPipeTest.cpp
	albaMatrixPipeTest.h albaMatrixPipeTest.cpp
	albaPipeBoxTest.h albaPipeBoxTest.cpp
	albaVMEOutputTest.h albaVMEOutputTest.cpp
	albaDataPipeTest.h albaDataPipeTest.cpp
	albaPipeFactoryTest.h albaPipeFactoryTest.cpp
	mmaMaterialTest.h mmaMaterialTest.cpp
	mmaVolumeMaterialTest.h mmaVolumeMaterialTest.cpp
	albaSceneNodeTest.h albaSceneNodeTest.cpp
	albaSceneGraphTest.h albaSceneGraphTest.cpp
	albaVMEOutputNULLTest.h albaVMEOutputNULLTest.cpp
	albaVMEStorageTest.h albaVMEStorageTest.cpp
	albaVMEStorageTest2.h albaVMEStorageTest2.cpp
	albaGUITreeContextualMenuTest.h albaGUITreeContextualMenuTest.cpp
	albaAttributeTraceabilityTest.h albaAttributeTraceabilityTest.cpp
	albaAxesTest.h albaAxesTest.cpp
	albaCoreFactoryTest.h albaCoreFactoryTest.cpp
	albaOpStackTest.h albaOpStackTest.cpp
	albaOpManagerTest.h albaOpManagerTest.cpp
	albaViewManagerTest.h albaViewManagerTest.cpp
	albaVMEManagerTest.h albaVMEManagerTest.cpp
	albaAbsMatrixPipeTest.h albaAbsMatrixPipeTest.cpp
	vtkALBAAssemblyTest.h vtkALBAAssemblyTest.cpp
	albaViewPlotTest.h albaViewPlotTest.cpp
	TimeSetTest.h TimeSetTest.cpp
	VMEPoseTest.h VMEPoseTest.cpp
	albaProgressBarHelperTest.h albaProgressBarHelperTest.cpp
)
#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#----------------------------------------------------------------------------

ADD_EXECUTABLE(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
ADD_TEST(${PROJECT_NAME}  ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME})
IF (WIN32)
  SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)
