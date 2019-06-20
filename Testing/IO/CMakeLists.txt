PROJECT (albaIO_TEST)

IF(ALBA_BUILD_ALBADLL)
  LINK_LIBRARIES(XercesC albaDLL) #to solve linking problems XercesC libs must be added before albaDLL
ELSE(ALBA_BUILD_ALBADLL)
  LINK_LIBRARIES(albaTests albaBase albaGui albaIO vtkRendering)
ENDIF (ALBA_BUILD_ALBADLL)

#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#-----------------------------------------------------------------------------
SET(PROJECT_SRCS 
	albaIOTests.cpp 					albaIOTests.h
  albaStorableTest.cpp 				albaStorableTest.h
	albaStorageTest.cpp 				albaStorageTest.h
	mmuDOMTreeErrorReporterTest.cpp	mmuDOMTreeErrorReporterTest.h
	albaXMLStringTest.cpp 			albaXMLStringTest.h
	mmuXMLDOMTest.cpp 				mmuXMLDOMTest.h
	mmuXMLDOMElementTest.cpp 		mmuXMLDOMElementTest.h
	albaCurlUtilityTest.cpp 			albaCurlUtilityTest.h
)

ADD_EXECUTABLE(albaIOTests ${PROJECT_SRCS})
ADD_TEST(albaIOTests  ${EXECUTABLE_OUTPUT_PATH}/albaIOTests)
IF (WIN32)
  SET_TARGET_PROPERTIES(albaIOTests PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)