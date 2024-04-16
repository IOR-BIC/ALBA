PROJECT (albaCommonTests)

IF(ALBA_BUILD_ALBADLL)
  SET(PROJECT_LIBS albaDLL vtkALBA albaTests)
ELSE(ALBA_BUILD_ALBADLL)
  SET(PROJECT_LIBS albaTests albaVME albaCommon albaViews albaVisualPipes albaService)
ENDIF(ALBA_BUILD_ALBADLL)

# Set your list of sources here.
SET(PROJECT_SRCS 
	albaCommonTests.cpp								albaCommonTests.h
	albaQueryObjectTest.cpp 							albaQueryObjectTest.h	
	albaResultQueryAbstractHandlerTest.cpp 			albaResultQueryAbstractHandlerTest.h	
	albaRelationalDatabaseAbstractConnectorTest.cpp	albaRelationalDatabaseAbstractConnectorTest.h	
	albaCurvilinearAbscissaOnSkeletonHelperTest.cpp 	albaCurvilinearAbscissaOnSkeletonHelperTest.h	
	albaHTMLTemplateParserTest.cpp 					albaHTMLTemplateParserTest.h	
	albaHTMLTemplateParserBlockTest.cpp				albaHTMLTemplateParserBlockTest.h	
	albaWizardBlockInformationTest.cpp				albaWizardBlockInformationTest.h	
	albaWizardBlockOperationTest.cpp					albaWizardBlockOperationTest.h	
	albaWizardBlockSelectionTest.cpp					albaWizardBlockSelectionTest.h	
	albaWizardBlockYesNoSelectionTest.cpp  			albaWizardBlockYesNoSelectionTest.h	
	albaWizardBlockTest.cpp							albaWizardBlockTest.h	
	albaWizardBlockTypeCheckTest.cpp					albaWizardBlockTypeCheckTest.h	
	albaWizardBlockVMESelectionTest.cpp  			albaWizardBlockVMESelectionTest.h	
	albaWizardManagerTest.cpp						albaWizardManagerTest.h	
	albaWizardSettingsTest.cpp						albaWizardSettingsTest.h	
	albaWizardWaitOpTest.cpp							albaWizardWaitOpTest.h	
	albaWizardTest.cpp								albaWizardTest.h
)
#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#----------------------------------------------------------------------------

ADD_EXECUTABLE(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
ADD_TEST(${PROJECT_NAME}  ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME})
IF (WIN32)
  SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)
