PROJECT (mafIO_TEST)

IF(MAF_BUILD_MAFDLL)
  LINK_LIBRARIES(XercesC mafDLL) #to solve linking problems XercesC libs must be added before mafDLL
ELSE(MAF_BUILD_MAFDLL)
  LINK_LIBRARIES(mafIO)
ENDIF (MAF_BUILD_MAFDLL)

############ mafStorableTest
ADD_EXECUTABLE(mafStorableTest mafStorableTest.cpp mafStorableTest.h)
ADD_TEST(mafStorableTest ${EXECUTABLE_OUTPUT_PATH}/mafStorableTest)
IF (WIN32)
	SET_TARGET_PROPERTIES(mafStorableTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafStorageTest
ADD_EXECUTABLE(mafStorageTest mafStorageTest.cpp mafStorageTest.h)
ADD_TEST(mafStorageTest ${EXECUTABLE_OUTPUT_PATH}/mafStorageTest)
IF (WIN32)
	SET_TARGET_PROPERTIES(mafStorageTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)


IF (MAF_USE_XercesC)
  ############ testXMLStorage
  ADD_EXECUTABLE(testXMLStorage mafXMLStorageTest.cpp)
  ADD_TEST(testXMLStorage ${EXECUTABLE_OUTPUT_PATH}/testXMLStorage)
	IF (WIN32)
	  SET_TARGET_PROPERTIES(testXMLStorage PROPERTIES LINK_FLAGS "/MANIFEST")
	ENDIF (WIN32)

############ mmuDOMTreeErrorReporterTest
ADD_EXECUTABLE(mmuDOMTreeErrorReporterTest mmuDOMTreeErrorReporterTest.h mmuDOMTreeErrorReporterTest.cpp)
ADD_TEST(mmuDOMTreeErrorReporterTest ${EXECUTABLE_OUTPUT_PATH}/mmuDOMTreeErrorReporterTest)
IF (WIN32)
    SET_TARGET_PROPERTIES(mmuDOMTreeErrorReporterTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

    ############ mafXMLStringTest
    ADD_EXECUTABLE(mafXMLStringTest mafXMLStringTest.h mafXMLStringTest.cpp)
    ADD_TEST(mafXMLStringTest ${EXECUTABLE_OUTPUT_PATH}/mafXMLStringTest)
    IF (WIN32)
        SET_TARGET_PROPERTIES(mafXMLStringTest PROPERTIES LINK_FLAGS "/MANIFEST")
    ENDIF (WIN32)

ENDIF (MAF_USE_XercesC)

############ mmuXMLDOMElementElementTest
ADD_EXECUTABLE(mmuXMLDOMTest mmuXMLDOMTest.cpp mmuXMLDOMTest.h)
ADD_TEST(mmuXMLDOMTest ${EXECUTABLE_OUTPUT_PATH}/mmuXMLDOMTest)
IF (WIN32)
	SET_TARGET_PROPERTIES(mmuXMLDOMTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mmuXMLDOMElementElementElementTest
ADD_EXECUTABLE(mmuXMLDOMElementTest mmuXMLDOMElementTest.cpp mmuXMLDOMElementTest.h)
ADD_TEST(mmuXMLDOMElementTest ${EXECUTABLE_OUTPUT_PATH}/mmuXMLDOMElementTest)
IF (WIN32)
	SET_TARGET_PROPERTIES(mmuXMLDOMElementTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafCurlUtility
ADD_EXECUTABLE(mafCurlUtilityTest mafCurlUtilityTest.cpp mafCurlUtilityTest.h)
ADD_TEST(mafCurlUtilityTest ${EXECUTABLE_OUTPUT_PATH}/mafCurlUtilityTest)
IF (WIN32)
	SET_TARGET_PROPERTIES(mafCurlUtilityTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)



# wxWidgets specific classes
IF (MAF_USE_WX)
ENDIF (MAF_USE_WX)

# VTK specific classes
IF (MAF_USE_VTK)
ENDIF (MAF_USE_VTK)  

# classes using VTK+wxWidgets
IF (MAF_USE_WX AND MAF_USE_VTK)
ENDIF (MAF_USE_WX AND MAF_USE_VTK)