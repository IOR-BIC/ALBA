PROJECT (mafBase_TEST)

IF(MAF_BUILD_MAFDLL)
  LINK_LIBRARIES(mafDLL)
ELSE(MAF_BUILD_MAFDLL)
  LINK_LIBRARIES(mafBase vtkRendering vtkGraphics mafGui)
ENDIF (MAF_BUILD_MAFDLL)

#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#-----------------------------------------------------------------------------
############ mafStringTest
ADD_EXECUTABLE(mafStringTest mafStringTest.h mafStringTest.cpp)
ADD_TEST(mafStringTest ${EXECUTABLE_OUTPUT_PATH}/mafStringTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafStringTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafTransformTest
ADD_EXECUTABLE(mafTransformTest mafTransformTest.h mafTransformTest.cpp)
ADD_TEST(mafTransformTest ${EXECUTABLE_OUTPUT_PATH}/mafTransformTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafTransformTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafTransformFrameTest
ADD_EXECUTABLE(mafTransformFrameTest mafTransformFrameTest.h mafTransformFrameTest.cpp)
ADD_TEST(mafTransformFrameTest ${EXECUTABLE_OUTPUT_PATH}/mafTransformFrameTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafTransformFrameTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafMatrixTest
ADD_EXECUTABLE(mafMatrixTest mafMatrixTest.h mafMatrixTest.cpp)
ADD_TEST(mafMatrixTest ${EXECUTABLE_OUTPUT_PATH}/mafMatrixTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafMatrixTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafMatrix3x3Test
ADD_EXECUTABLE(mafMatrix3x3Test mafMatrix3x3Test.h mafMatrix3x3Test.cpp)
ADD_TEST(mafMatrix3x3Test ${EXECUTABLE_OUTPUT_PATH}/mafMatrix3x3Test)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafMatrix3x3Test PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafOBBTest
ADD_EXECUTABLE(mafOBBTest mafOBBTest.h mafOBBTest.cpp)
ADD_TEST(mafOBBTest ${EXECUTABLE_OUTPUT_PATH}/mafOBBTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafOBBTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafDirectoryTest
ADD_EXECUTABLE(mafDirectoryTest mafDirectoryTest.h mafDirectoryTest.cpp)
ADD_TEST(mafDirectoryTest ${EXECUTABLE_OUTPUT_PATH}/mafDirectoryTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafDirectoryTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafEventSenderTest
ADD_EXECUTABLE(mafEventSenderTest mafEventSenderTest.h mafEventSenderTest.cpp)
ADD_TEST(mafEventSenderTest ${EXECUTABLE_OUTPUT_PATH}/mafEventSenderTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafEventSenderTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafEventBaseTest
ADD_EXECUTABLE(mafEventBaseTest mafEventBaseTest.h mafEventBaseTest.cpp)
ADD_TEST(mafEventBaseTest ${EXECUTABLE_OUTPUT_PATH}/mafEventBaseTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafEventBaseTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafEventSourceTest
ADD_EXECUTABLE(mafEventSourceTest mafEventSourceTest.h mafEventSourceTest.cpp)
ADD_TEST(mafEventSourceTest ${EXECUTABLE_OUTPUT_PATH}/mafEventSourceTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafEventSourceTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafObjectTest
ADD_EXECUTABLE(mafObjectTest mafObjectTest.h mafObjectTest.cpp)
ADD_TEST(mafObjectTest ${EXECUTABLE_OUTPUT_PATH}/mafObjectTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafObjectTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafIndentTest
ADD_EXECUTABLE(mafIndentTest mafIndentTest.h mafIndentTest.cpp)
ADD_TEST(mafIndentTest ${EXECUTABLE_OUTPUT_PATH}/mafIndentTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafIndentTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafDirectoryTest
ADD_EXECUTABLE(mafDirectoryTest mafDirectoryTest.h mafDirectoryTest.cpp)
ADD_TEST(mafDirectoryTest ${EXECUTABLE_OUTPUT_PATH}/mafDirectoryTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafDirectoryTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafTransformBaseTest
ADD_EXECUTABLE(mafTransformBaseTest mafTransformBaseTest.h mafTransformBaseTest.cpp)
ADD_TEST(mafTransformBaseTest ${EXECUTABLE_OUTPUT_PATH}/mafTransformBaseTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafTransformBaseTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafObserverTest
ADD_EXECUTABLE(mafObserverTest mafObserverTest.h mafObserverTest.cpp)
ADD_TEST(mafObserverTest ${EXECUTABLE_OUTPUT_PATH}/mafObserverTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafObserverTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVersionTest
ADD_EXECUTABLE(mafVersionTest mafVersionTest.h mafVersionTest.cpp)
ADD_TEST(mafVersionTest ${EXECUTABLE_OUTPUT_PATH}/mafVersionTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVersionTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mmuIdFactoryTest
ADD_EXECUTABLE(mmuIdFactoryTest mmuIdFactoryTest.h mmuIdFactoryTest.cpp)
ADD_TEST(mmuIdFactoryTest ${EXECUTABLE_OUTPUT_PATH}/mmuIdFactoryTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mmuIdFactoryTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVectorTest
ADD_EXECUTABLE(mafVectorTest mafVectorTest.h mafVectorTest.cpp)
ADD_TEST(mafVectorTest ${EXECUTABLE_OUTPUT_PATH}/mafVectorTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVectorTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafObjectFactoryTest
ADD_EXECUTABLE(mafObjectFactoryTest mafObjectFactoryTest.h mafObjectFactoryTest.cpp)
ADD_TEST(mafObjectFactoryTest ${EXECUTABLE_OUTPUT_PATH}/mafObjectFactoryTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafObjectFactoryTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ vtkMAFToLinearTransformTest
ADD_EXECUTABLE(vtkMAFToLinearTransformTest vtkMAFToLinearTransformTest.h vtkMAFToLinearTransformTest.cpp)
ADD_TEST(vtkMAFToLinearTransformTest ${EXECUTABLE_OUTPUT_PATH}/vtkMAFToLinearTransformTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(vtkMAFToLinearTransformTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafReferenceCountedTest
ADD_EXECUTABLE(mafReferenceCountedTest mafReferenceCountedTest.h mafReferenceCountedTest.cpp)
ADD_TEST(mafReferenceCountedTest ${EXECUTABLE_OUTPUT_PATH}/mafReferenceCountedTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafReferenceCountedTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)


############ mafLUTLibraryTest
ADD_EXECUTABLE(mafLUTLibraryTest mafLUTLibraryTest.h mafLUTLibraryTest.cpp)
ADD_TEST(mafLUTLibraryTest ${EXECUTABLE_OUTPUT_PATH}/mafLUTLibraryTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafLUTLibraryTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafObserversListTest
ADD_EXECUTABLE(mafObserversListTest mafObserversListTest.h mafObserversListTest.cpp)
ADD_TEST(mafObserversListTest ${EXECUTABLE_OUTPUT_PATH}/mafObserversListTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafObserversListTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafDynamicLoaderTest
ADD_EXECUTABLE(mafDynamicLoaderTest mafDynamicLoaderTest.h mafDynamicLoaderTest.cpp)
ADD_TEST(mafDynamicLoaderTest ${EXECUTABLE_OUTPUT_PATH}/mafDynamicLoaderTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafDynamicLoaderTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafDataChecksum
ADD_EXECUTABLE(mafDataChecksumTest mafDataChecksumTest.h mafDataChecksumTest.cpp)
ADD_TEST(mafDataChecksumTest  ${EXECUTABLE_OUTPUT_PATH}/mafDataChecksumTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafDataChecksumTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ medVect3dTest
ADD_EXECUTABLE(medVect3dTest  medVect3dTest.h medVect3dTest.cpp)
ADD_TEST(medVect3dTest   ${EXECUTABLE_OUTPUT_PATH}/medVect3dTest )
IF (WIN32)
  SET_TARGET_PROPERTIES(medVect3dTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

#-----------------------------------------------------------------------------
# tests not using cppunit testing framework: to be converted...
#-----------------------------------------------------------------------------
############ mafReferenceCountedTest
ADD_TEST(mafReferenceCountedTest ${EXECUTABLE_OUTPUT_PATH}/mafReferenceCountedTest)
ADD_EXECUTABLE(mafEventTest mafEventTest.cpp)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafReferenceCountedTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafEventTest
ADD_TEST(mafEventTest ${EXECUTABLE_OUTPUT_PATH}/mafEventTest)
ADD_EXECUTABLE(MultithreaderTest testMultithreader.cpp)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafEventTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ testVMEPose
ADD_TEST(MultithreaderTest ${EXECUTABLE_OUTPUT_PATH}/MultithreaderTest)

#-----------------------------------------------------------------------------
# this tests do not build: to be fixed...
#-----------------------------------------------------------------------------
# ADD_EXECUTABLE(test_sorted_vector test_sorted_vector.cpp)
# ADD_TEST(test_sorted_vector ${EXECUTABLE_OUTPUT_PATH}/test_sorted_vector)


# wxWidgets specific classes
#IF (MAF_USE_WX)
#ENDIF (MAF_USE_WX)

# VTK specific classes
#IF (MAF_USE_VTK)
#ENDIF (MAF_USE_VTK)  

# classes using VTK+wxWidgets
#IF (MAF_USE_WX AND MAF_USE_VTK)
#ENDIF (MAF_USE_WX AND MAF_USE_VTK)
