PROJECT (albaBaseTests)	

IF(ALBA_BUILD_ALBADLL)
  SET(PROJECT_LIBS albaDLL vtkALBA albaTests)
ELSE(ALBA_BUILD_ALBADLL)
  SET(PROJECT_LIBS albaTests albaBase albaCore ${VTK_LIBRARIES} albaGui albaService albaOperations albaViews albaVisualPipes)
ENDIF(ALBA_BUILD_ALBADLL)

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

ADD_EXECUTABLE(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
ADD_TEST(${PROJECT_NAME}  ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME})
IF (WIN32)
  SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)