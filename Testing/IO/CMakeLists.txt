PROJECT (mafIO_TEST)

IF(MAF_BUILD_MAFDLL)
  LINK_LIBRARIES(XercesC mafDLL) #to solve linking problems XercesC libs must be added before mafDLL
ELSE(MAF_BUILD_MAFDLL)
  LINK_LIBRARIES(mafTests mafBase mafGui mafIO vtkRendering)
ENDIF (MAF_BUILD_MAFDLL)

#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#-----------------------------------------------------------------------------
SET(PROJECT_SRCS 
	mafIOTests.cpp 					mafIOTests.h
  mafStorableTest.cpp 				mafStorableTest.h
	mafStorageTest.cpp 				mafStorageTest.h
	mmuDOMTreeErrorReporterTest.cpp	mmuDOMTreeErrorReporterTest.h
	mafXMLStringTest.cpp 			mafXMLStringTest.h
	mmuXMLDOMTest.cpp 				mmuXMLDOMTest.h
	mmuXMLDOMElementTest.cpp 		mmuXMLDOMElementTest.h
	mafCurlUtilityTest.cpp 			mafCurlUtilityTest.h
)

ADD_EXECUTABLE(mafIOTests ${PROJECT_SRCS})
ADD_TEST(mafIOTests  ${EXECUTABLE_OUTPUT_PATH}/mafIOTests)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafIOTests PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)