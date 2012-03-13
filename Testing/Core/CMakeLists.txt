PROJECT (mafCore_TEST)

LINK_LIBRARIES(mafCore mafVME)

#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#-----------------------------------------------------------------------------
############ mafViewVTKTest
ADD_EXECUTABLE(mafViewVTKTest mafViewVTKTest.h mafViewVTKTest.cpp)
ADD_TEST(mafViewVTKTest ${EXECUTABLE_OUTPUT_PATH}/mafViewVTKTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafViewVTKTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafViewHTMLTest
ADD_EXECUTABLE(mafViewHTMLTest mafViewHTMLTest.h mafViewHTMLTest.cpp)
ADD_TEST(mafViewHTMLTest ${EXECUTABLE_OUTPUT_PATH}/mafViewHTMLTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafViewHTMLTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafAttributeTest
ADD_EXECUTABLE(mafAttributeTest mafAttributeTest.h mafAttributeTest.cpp)
ADD_TEST(mafAttributeTest ${EXECUTABLE_OUTPUT_PATH}/mafAttributeTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafAttributeTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafTagItemTest
ADD_EXECUTABLE(mafTagItemTest mafTagItemTest.h mafTagItemTest.cpp)
ADD_TEST(mafTagItemTest ${EXECUTABLE_OUTPUT_PATH}/mafTagItemTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafTagItemTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafTagArrayTest
ADD_EXECUTABLE(mafTagArrayTest mafTagArrayTest.h mafTagArrayTest.cpp)
ADD_TEST(mafTagArrayTest ${EXECUTABLE_OUTPUT_PATH}/mafTagArrayTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafTagArrayTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafNodeTest
ADD_EXECUTABLE(mafNodeTest mafNodeTest.h mafNodeTest.cpp)
ADD_TEST(mafNodeTest ${EXECUTABLE_OUTPUT_PATH}/mafNodeTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafNodeTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafTimeMapTest
ADD_EXECUTABLE(mafTimeMapTest mafTimeMapTest.h mafTimeMapTest.cpp)
ADD_TEST(mafTimeMapTest ${EXECUTABLE_OUTPUT_PATH}/mafTimeMapTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafTimeMapTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafNodeFactoryTest
ADD_EXECUTABLE(mafNodeFactoryTest mafNodeFactoryTest.h mafNodeFactoryTest.cpp)
ADD_TEST(mafNodeFactoryTest ${EXECUTABLE_OUTPUT_PATH}/mafNodeFactoryTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafNodeFactoryTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mmaApplicationLayoutTest
ADD_EXECUTABLE(mmaApplicationLayoutTest mmaApplicationLayoutTest.h mmaApplicationLayoutTest.cpp)
ADD_TEST(mmaApplicationLayoutTest ${EXECUTABLE_OUTPUT_PATH}/mmaApplicationLayoutTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mmaApplicationLayoutTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMETest
ADD_EXECUTABLE(mafVMETest mafVMETest.h mafVMETest.cpp)
ADD_TEST(mafVMETest ${EXECUTABLE_OUTPUT_PATH}/mafVMETest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVMETest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafOpTest
ADD_EXECUTABLE(mafOpTest mafOpTest.h mafOpTest.cpp)
ADD_TEST(mafOpTest ${EXECUTABLE_OUTPUT_PATH}/mafOpTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafOpTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafEventIOTest
ADD_EXECUTABLE(mafEventIOTest mafEventIOTest.h mafEventIOTest.cpp)
ADD_TEST(mafEventIOTest ${EXECUTABLE_OUTPUT_PATH}/mafEventIOTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafEventIOTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafUserTest
ADD_EXECUTABLE(mafUserTest mafUserTest.h mafUserTest.cpp)
ADD_TEST(mafUserTest ${EXECUTABLE_OUTPUT_PATH}/mafUserTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafUserTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafLODActorTest
ADD_EXECUTABLE(mafLODActorTest mafLODActorTest.h mafLODActorTest.cpp)
ADD_TEST(mafLODActorTest ${EXECUTABLE_OUTPUT_PATH}/mafLODActorTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafLODActorTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafTimeMapScalarTest
ADD_EXECUTABLE(mafTimeMapScalarTest mafTimeMapScalarTest.h mafTimeMapScalarTest.cpp)
ADD_TEST(mafTimeMapScalarTest ${EXECUTABLE_OUTPUT_PATH}/mafTimeMapScalarTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafTimeMapScalarTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafOpSelectTest
ADD_EXECUTABLE(mafOpSelectTest mafOpSelectTest.h mafOpSelectTest.cpp)
ADD_TEST(mafOpSelectTest ${EXECUTABLE_OUTPUT_PATH}/mafOpSelectTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafOpSelectTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafRootTest
ADD_EXECUTABLE(mafRootTest mafRootTest.h mafRootTest.cpp)
ADD_TEST(mafRootTest ${EXECUTABLE_OUTPUT_PATH}/mafRootTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafRootTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMERootTest
ADD_EXECUTABLE(mafVMERootTest mafVMERootTest.h mafVMERootTest.cpp)
ADD_TEST(mafVMERootTest ${EXECUTABLE_OUTPUT_PATH}/mafVMERootTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVMERootTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafOpCutTest
ADD_EXECUTABLE(mafOpCutTest mafOpCutTest.h mafOpCutTest.cpp)
ADD_TEST(mafOpCutTest ${EXECUTABLE_OUTPUT_PATH}/mafOpCutTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafOpCutTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafOpCopyTest
ADD_EXECUTABLE(mafOpCopyTest mafOpCopyTest.h mafOpCopyTest.cpp)
ADD_TEST(mafOpCopyTest ${EXECUTABLE_OUTPUT_PATH}/mafOpCopyTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafOpCopyTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafOpPasteTest
ADD_EXECUTABLE(mafOpPasteTest mafOpPasteTest.h mafOpPasteTest.cpp)
ADD_TEST(mafOpPasteTest ${EXECUTABLE_OUTPUT_PATH}/mafOpPasteTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafOpPasteTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafOpEditTest
ADD_EXECUTABLE(mafOpEditTest mafOpEditTest.h mafOpEditTest.cpp)
ADD_TEST(mafOpEditTest ${EXECUTABLE_OUTPUT_PATH}/mafOpEditTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafOpEditTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafOpContextStackTest
ADD_EXECUTABLE(mafOpContextStackTest mafOpContextStackTest.h mafOpContextStackTest.cpp)
ADD_TEST(mafOpContextStackTest ${EXECUTABLE_OUTPUT_PATH}/mafOpContextStackTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafOpContextStackTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafNodeIteratorTest
ADD_EXECUTABLE(mafNodeIteratorTest mafNodeIteratorTest.h mafNodeIteratorTest.cpp)
ADD_TEST(mafNodeIteratorTest ${EXECUTABLE_OUTPUT_PATH}/mafNodeIteratorTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafNodeIteratorTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafNodeLayoutTest
ADD_EXECUTABLE(mafNodeLayoutTest mafNodeLayoutTest.h mafNodeLayoutTest.cpp)
ADD_TEST(mafNodeLayoutTest ${EXECUTABLE_OUTPUT_PATH}/mafNodeLayoutTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafNodeLayoutTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafPipeTest
ADD_EXECUTABLE(mafPipeTest mafPipeTest.h mafPipeTest.cpp)
ADD_TEST(mafPipeTest ${EXECUTABLE_OUTPUT_PATH}/mafPipeTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafPipeTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafMatrixPipeTest
ADD_EXECUTABLE(mafMatrixPipeTest mafMatrixPipeTest.h mafMatrixPipeTest.cpp)
ADD_TEST(mafMatrixPipeTest ${EXECUTABLE_OUTPUT_PATH}/mafMatrixPipeTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafMatrixPipeTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafPipeBoxTest
ADD_EXECUTABLE(mafPipeBoxTest mafPipeBoxTest.h mafPipeBoxTest.cpp)
ADD_TEST(mafPipeBoxTest ${EXECUTABLE_OUTPUT_PATH}/mafPipeBoxTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafPipeBoxTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMEOutputTest
ADD_EXECUTABLE(mafVMEOutputTest mafVMEOutputTest.h mafVMEOutputTest.cpp)
ADD_TEST(mafVMEOutputTest ${EXECUTABLE_OUTPUT_PATH}/mafVMEOutputTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVMEOutputTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafDataPipeTest
ADD_EXECUTABLE(mafDataPipeTest mafDataPipeTest.h mafDataPipeTest.cpp)
ADD_TEST(mafDataPipeTest ${EXECUTABLE_OUTPUT_PATH}/mafDataPipeTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafDataPipeTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafNodeRootTest
ADD_EXECUTABLE(mafNodeRootTest mafNodeRootTest.h mafNodeRootTest.cpp)
ADD_TEST(mafNodeRootTest ${EXECUTABLE_OUTPUT_PATH}/mafNodeRootTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafNodeRootTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafPipeFactoryTest
ADD_EXECUTABLE(mafPipeFactoryTest mafPipeFactoryTest.h mafPipeFactoryTest.cpp)
ADD_TEST(mafPipeFactoryTest ${EXECUTABLE_OUTPUT_PATH}/mafPipeFactoryTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafPipeFactoryTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mmaMaterialTest
ADD_EXECUTABLE(mmaMaterialTest mmaMaterialTest.h mmaMaterialTest.cpp)
ADD_TEST(mmaMaterialTest ${EXECUTABLE_OUTPUT_PATH}/mmaMaterialTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mmaMaterialTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mmaVolumeMaterialTest
ADD_EXECUTABLE(mmaVolumeMaterialTest mmaVolumeMaterialTest.h mmaVolumeMaterialTest.cpp)
ADD_TEST(mmaVolumeMaterialTest ${EXECUTABLE_OUTPUT_PATH}/mmaVolumeMaterialTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mmaVolumeMaterialTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafSceneNodeTest
ADD_EXECUTABLE(mafSceneNodeTest mafSceneNodeTest.h mafSceneNodeTest.cpp)
ADD_TEST(mafSceneNodeTest ${EXECUTABLE_OUTPUT_PATH}/mafSceneNodeTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafSceneNodeTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafSceneGraphTest
ADD_EXECUTABLE(mafSceneGraphTest mafSceneGraphTest.h mafSceneGraphTest.cpp)
ADD_TEST(mafSceneGraphTest ${EXECUTABLE_OUTPUT_PATH}/mafSceneGraphTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafSceneGraphTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMEOutputNULLTest
ADD_EXECUTABLE(mafVMEOutputNULLTest mafVMEOutputNULLTest.h mafVMEOutputNULLTest.cpp)
ADD_TEST(mafVMEOutputNULLTest ${EXECUTABLE_OUTPUT_PATH}/mafVMEOutputNULLTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVMEOutputNULLTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMEStorageTest
ADD_EXECUTABLE(mafVMEStorageTest mafVMEStorageTest.h mafVMEStorageTest.cpp)
ADD_TEST(mafVMEStorageTest  ${EXECUTABLE_OUTPUT_PATH}/mafVMEStorageTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVMEStorageTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMEStorageTest2
ADD_EXECUTABLE(mafVMEStorageTest2 mafVMEStorageTest2.h mafVMEStorageTest2.cpp)
ADD_TEST(mafVMEStorageTest2  ${EXECUTABLE_OUTPUT_PATH}/mafVMEStorageTest2)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVMEStorageTest2 PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafGUITreeContextualMenuTest
ADD_EXECUTABLE(mafGUITreeContextualMenuTest mafGUITreeContextualMenuTest.h mafGUITreeContextualMenuTest.cpp)
ADD_TEST(mafGUITreeContextualMenuTest ${EXECUTABLE_OUTPUT_PATH}/mafGUITreeContextualMenuTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafGUITreeContextualMenuTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)


#-----------------------------------------------------------------------------
# tests not using cppunit testing framework: to be converted...
#-----------------------------------------------------------------------------
############ testTimeSet
ADD_EXECUTABLE(testTimeSet TestTimeSet.cpp)
ADD_TEST(testTimeSet ${EXECUTABLE_OUTPUT_PATH}/testTimeSet)
IF (WIN32)
  SET_TARGET_PROPERTIES(testTimeSet PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ testVMEPose
ADD_EXECUTABLE(testVMEPose testVMEPose.cpp)
ADD_TEST(testVMEPose ${EXECUTABLE_OUTPUT_PATH}/testVMEPose)
IF (WIN32)
  SET_TARGET_PROPERTIES(testVMEPose PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)
