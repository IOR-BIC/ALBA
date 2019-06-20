PROJECT (albaBase_TEST)	

IF(ALBA_BUILD_ALBADLL)
  LINK_LIBRARIES(albaDLL)
ELSE(ALBA_BUILD_ALBADLL)
  LINK_LIBRARIES(albaTests albaBase vtkRendering vtkGraphics albaGui albaService)
ENDIF (ALBA_BUILD_ALBADLL)

SET(PROJECT_SRCS 
	albaBaseTests.h 					albaBaseTests.cpp
	albaStringTest.h 				albaStringTest.cpp
	albaTransformTest.h 				albaTransformTest.cpp
	albaTransformFrameTest.h 		albaTransformFrameTest.cpp
	albaMatrixTest.h 				albaMatrixTest.cpp
	albaMatrix3x3Test.h 				albaMatrix3x3Test.cpp
	albaOBBTest.h 					albaOBBTest.cpp
	albaDirectoryTest.h 				albaDirectoryTest.cpp
	albaEventSenderTest.h 			albaEventSenderTest.cpp
	albaEventBaseTest.h 				albaEventBaseTest.cpp
	albaEventBroadcasterTest.h 	albaEventBroadcasterTest.cpp
	albaObjectTest.h 				albaObjectTest.cpp
	albaIndentTest.h 				albaIndentTest.cpp
	albaDirectoryTest.h 				albaDirectoryTest.cpp
	albaTransformBaseTest.h 			albaTransformBaseTest.cpp
	albaObserverTest.h 				albaObserverTest.cpp
	albaVersionTest.h 				albaVersionTest.cpp
	mmuIdFactoryTest.h 				mmuIdFactoryTest.cpp
	albaVectorTest.h 				albaVectorTest.cpp
	albaObjectFactoryTest.h 			albaObjectFactoryTest.cpp
	vtkALBAToLinearTransformTest.h 	vtkALBAToLinearTransformTest.cpp
	albaReferenceCountedTest.h 		albaReferenceCountedTest.cpp
	albaLUTLibraryTest.h 			albaLUTLibraryTest.cpp
	albaDynamicLoaderTest.h 			albaDynamicLoaderTest.cpp
	albaDataChecksumTest.h 			albaDataChecksumTest.cpp
	albaVect3dTest.h 				albaVect3dTest.cpp
	albaEventTest.cpp 				albaEventTest.h
	multiThreaderTest.cpp 			multiThreaderTest.h
)

ADD_EXECUTABLE(albaBaseTests ${PROJECT_SRCS})
ADD_TEST(albaBaseTests  ${EXECUTABLE_OUTPUT_PATH}/albaBaseTests)
IF (WIN32)
  SET_TARGET_PROPERTIES(albaBaseTests PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)