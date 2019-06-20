PROJECT (albaCommon_TEST)

IF(ALBA_BUILD_ALBADLL)
  LINK_LIBRARIES(albaTests albaVME albaCommon)
ELSE(ALBA_BUILD_ALBADLL)
  LINK_LIBRARIES(albaTests albaVME albaCommon albaService)
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

ADD_EXECUTABLE(albaCommonTests ${PROJECT_SRCS})
ADD_TEST(albaCommonTests  ${EXECUTABLE_OUTPUT_PATH}/albaCommonTests)
IF (WIN32)
  SET_TARGET_PROPERTIES(albaCommonTests PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)
