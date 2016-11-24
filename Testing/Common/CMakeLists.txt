PROJECT (mafCommon_TEST)

IF(MAF_BUILD_MAFDLL)
  LINK_LIBRARIES(mafTests mafVME mafCommon)
ELSE(MAF_BUILD_MAFDLL)
  LINK_LIBRARIES(mafTests mafVME mafCommon mafService)
ENDIF(MAF_BUILD_MAFDLL)

# Set your list of sources here.
SET(PROJECT_SRCS 
	mafCommonTests.cpp								mafCommonTests.h
	mafQueryObjectTest.cpp 							mafQueryObjectTest.h	
	mafResultQueryAbstractHandlerTest.cpp 			mafResultQueryAbstractHandlerTest.h	
	mafRelationalDatabaseAbstractConnectorTest.cpp	mafRelationalDatabaseAbstractConnectorTest.h	
	mafCurvilinearAbscissaOnSkeletonHelperTest.cpp 	mafCurvilinearAbscissaOnSkeletonHelperTest.h	
	mafHTMLTemplateParserTest.cpp 					mafHTMLTemplateParserTest.h	
	mafHTMLTemplateParserBlockTest.cpp				mafHTMLTemplateParserBlockTest.h	
	mafWizardBlockInformationTest.cpp				mafWizardBlockInformationTest.h	
	mafWizardBlockOperationTest.cpp					mafWizardBlockOperationTest.h	
	mafWizardBlockSelectionTest.cpp					mafWizardBlockSelectionTest.h	
	mafWizardBlockYesNoSelectionTest.cpp  			mafWizardBlockYesNoSelectionTest.h	
	mafWizardBlockTest.cpp							mafWizardBlockTest.h	
	mafWizardBlockTypeCheckTest.cpp					mafWizardBlockTypeCheckTest.h	
	mafWizardBlockVMESelectionTest.cpp  			mafWizardBlockVMESelectionTest.h	
	mafWizardManagerTest.cpp						mafWizardManagerTest.h	
	mafWizardSettingsTest.cpp						mafWizardSettingsTest.h	
	mafWizardWaitOpTest.cpp							mafWizardWaitOpTest.h	
	mafWizardTest.cpp								mafWizardTest.h
)
#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#----------------------------------------------------------------------------

ADD_EXECUTABLE(mafCommonTests ${PROJECT_SRCS})
ADD_TEST(mafCommonTests  ${EXECUTABLE_OUTPUT_PATH}/mafCommonTests)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafCommonTests PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)
