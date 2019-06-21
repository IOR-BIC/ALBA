PROJECT (albaCore_TEST)

IF(ALBA_BUILD_ALBADLL)
  LINK_LIBRARIES(albaDLL)
ELSE(ALBA_BUILD_ALBADLL)
  LINK_LIBRARIES(albaTests albaCore albaVME albaService)
ENDIF (ALBA_BUILD_ALBADLL)

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

ADD_EXECUTABLE(albaCoreTests ${PROJECT_SRCS})
ADD_TEST(albaCoreTests  ${EXECUTABLE_OUTPUT_PATH}/albaCoreTests)
IF (WIN32)
  SET_TARGET_PROPERTIES(albaCoreTests PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

