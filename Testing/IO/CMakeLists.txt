PROJECT (mafCore_TEST)

LINK_LIBRARIES(mafCore)
IF (MAF_USE_XercesC)
  ############ testXMLStorage
  ADD_EXECUTABLE(testXMLStorage mafXMLStorageTest.cpp)
  ADD_TEST(testXMLStorage ${EXECUTABLE_OUTPUT_PATH}/testXMLStorage)
	IF (WIN32)
	  SET_TARGET_PROPERTIES(testXMLStorage PROPERTIES LINK_FLAGS "/MANIFEST")
	ENDIF (WIN32)
ENDIF (MAF_USE_XercesC)

# wxWidgets specific classes
IF (MAF_USE_WX)
ENDIF (MAF_USE_WX)

# VTK specific classes
IF (MAF_USE_VTK)
ENDIF (MAF_USE_VTK)  

# classes using VTK+wxWidgets
IF (MAF_USE_WX AND MAF_USE_VTK)
ENDIF (MAF_USE_WX AND MAF_USE_VTK)