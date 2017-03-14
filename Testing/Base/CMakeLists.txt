PROJECT (mafBase_TEST)	

IF(MAF_BUILD_MAFDLL)
  LINK_LIBRARIES(mafDLL)
ELSE(MAF_BUILD_MAFDLL)
  LINK_LIBRARIES(mafTests mafBase vtkRendering vtkGraphics mafGui mafService)
ENDIF (MAF_BUILD_MAFDLL)

SET(PROJECT_SRCS 
	mafBaseTests.h 					mafBaseTests.cpp
	mafStringTest.h 				mafStringTest.cpp
	mafTransformTest.h 				mafTransformTest.cpp
	mafTransformFrameTest.h 		mafTransformFrameTest.cpp
	mafMatrixTest.h 				mafMatrixTest.cpp
	mafMatrix3x3Test.h 				mafMatrix3x3Test.cpp
	mafOBBTest.h 					mafOBBTest.cpp
	mafDirectoryTest.h 				mafDirectoryTest.cpp
	mafEventSenderTest.h 			mafEventSenderTest.cpp
	mafEventBaseTest.h 				mafEventBaseTest.cpp
	mafEventBroadcasterTest.h 	mafEventBroadcasterTest.cpp
	mafObjectTest.h 				mafObjectTest.cpp
	mafIndentTest.h 				mafIndentTest.cpp
	mafDirectoryTest.h 				mafDirectoryTest.cpp
	mafTransformBaseTest.h 			mafTransformBaseTest.cpp
	mafObserverTest.h 				mafObserverTest.cpp
	mafVersionTest.h 				mafVersionTest.cpp
	mmuIdFactoryTest.h 				mmuIdFactoryTest.cpp
	mafVectorTest.h 				mafVectorTest.cpp
	mafObjectFactoryTest.h 			mafObjectFactoryTest.cpp
	vtkMAFToLinearTransformTest.h 	vtkMAFToLinearTransformTest.cpp
	mafReferenceCountedTest.h 		mafReferenceCountedTest.cpp
	mafLUTLibraryTest.h 			mafLUTLibraryTest.cpp
	mafObserversListTest.h 			mafObserversListTest.cpp
	mafDynamicLoaderTest.h 			mafDynamicLoaderTest.cpp
	mafDataChecksumTest.h 			mafDataChecksumTest.cpp
	mafVect3dTest.h 				mafVect3dTest.cpp
	mafEventTest.cpp 				mafEventTest.h
	multiThreaderTest.cpp 			multiThreaderTest.h
)

ADD_EXECUTABLE(mafBaseTests ${PROJECT_SRCS})
ADD_TEST(mafBaseTests  ${EXECUTABLE_OUTPUT_PATH}/mafBaseTests)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafBaseTests PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)