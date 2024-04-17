PROJECT (albaIOTests)

IF(ALBA_BUILD_ALBADLL)
  SET(PROJECT_LIBS XercesC albaDLL vtkALBA albaTests) #to solve linking problems XercesC libs must be added before albaDLL
ELSE(ALBA_BUILD_ALBADLL)
  SET(PROJECT_LIBS albaTests albaBase albaGui albaIO ${VTK_LIBRARIES})
ENDIF(ALBA_BUILD_ALBADLL)

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


ADD_EXECUTABLE(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
ADD_TEST(${PROJECT_NAME}  ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME})
IF (WIN32)
  SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)