PROJECT (mafCommon_TEST)

IF(MAF_BUILD_MAFDLL)
  LINK_LIBRARIES(mafVME mafCommon)
ELSE(MAF_BUILD_MAFDLL)
  LINK_LIBRARIES(mafVME mafCommon)
ENDIF(MAF_BUILD_MAFDLL)

#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#----------------------------------------------------------------------------

ADD_EXECUTABLE(mafQueryObjectTest mafQueryObjectTest.h mafQueryObjectTest.cpp)
ADD_TEST(mafQueryObjectTest  ${EXECUTABLE_OUTPUT_PATH}/mafQueryObjectTest)

ADD_EXECUTABLE(mafResultQueryAbstractHandlerTest mafResultQueryAbstractHandlerTest.h mafResultQueryAbstractHandlerTest.cpp)
ADD_TEST(mafResultQueryAbstractHandlerTest  ${EXECUTABLE_OUTPUT_PATH}/mafResultQueryAbstractHandlerTest)

ADD_EXECUTABLE(mafRelationalDatabaseAbstractConnectorTest mafRelationalDatabaseAbstractConnectorTest.h mafRelationalDatabaseAbstractConnectorTest.cpp)
ADD_TEST(mafRelationalDatabaseAbstractConnectorTest  ${EXECUTABLE_OUTPUT_PATH}/mafRelationalDatabaseAbstractConnectorTest)

ADD_EXECUTABLE(mafCurvilinearAbscissaOnSkeletonHelperTest mafCurvilinearAbscissaOnSkeletonHelperTest.h mafCurvilinearAbscissaOnSkeletonHelperTest.cpp)
ADD_TEST(mafCurvilinearAbscissaOnSkeletonHelperTest  ${EXECUTABLE_OUTPUT_PATH}/mafCurvilinearAbscissaOnSkeletonHelperTest)

ADD_EXECUTABLE(mafHTMLTemplateParserTest  mafHTMLTemplateParserTest.h mafHTMLTemplateParserTest.cpp)
ADD_TEST(mafHTMLTemplateParserTest   ${EXECUTABLE_OUTPUT_PATH}/mafHTMLTemplateParserTest )

ADD_EXECUTABLE(mafHTMLTemplateParserBlockTest  mafHTMLTemplateParserBlockTest.h mafHTMLTemplateParserBlockTest.cpp)
ADD_TEST(mafHTMLTemplateParserBlockTest   ${EXECUTABLE_OUTPUT_PATH}/mafHTMLTemplateParserBlockTest )

ADD_EXECUTABLE(mafWizardBlockInformationTest  mafWizardBlockInformationTest.cpp  mafWizardBlockInformationTest.h)
ADD_TEST(mafWizardBlockInformationTest   ${EXECUTABLE_OUTPUT_PATH}/mafWizardBlockInformationTest )

ADD_EXECUTABLE(mafWizardBlockOperationTest  mafWizardBlockOperationTest.cpp  mafWizardBlockOperationTest.h)
ADD_TEST(mafWizardBlockOperationTest   ${EXECUTABLE_OUTPUT_PATH}/mafWizardBlockOperationTest )

ADD_EXECUTABLE(mafWizardBlockSelectionTest  mafWizardBlockSelectionTest.cpp  mafWizardBlockSelectionTest.h)
ADD_TEST(mafWizardBlockSelectionTest   ${EXECUTABLE_OUTPUT_PATH}/mafWizardBlockSelectionTest )

ADD_EXECUTABLE(mafWizardBlockYesNoSelectionTest  mafWizardBlockYesNoSelectionTest.cpp  mafWizardBlockYesNoSelectionTest.h)
ADD_TEST(mafWizardBlockYesNoSelectionTest   ${EXECUTABLE_OUTPUT_PATH}/mafWizardBlockYesNoSelectionTest )

ADD_EXECUTABLE(mafWizardBlockTest  mafWizardBlockTest.cpp  mafWizardBlockTest.h)
ADD_TEST(mafWizardBlockTest   ${EXECUTABLE_OUTPUT_PATH}/mafWizardBlockTest )

ADD_EXECUTABLE(mafWizardBlockTypeCheckTest  mafWizardBlockTypeCheckTest.cpp  mafWizardBlockTypeCheckTest.h)
ADD_TEST(mafWizardBlockTypeCheckTest   ${EXECUTABLE_OUTPUT_PATH}/mafWizardBlockTypeCheckTest )

ADD_EXECUTABLE(mafWizardBlockVMESelectionTest  mafWizardBlockVMESelectionTest.cpp   mafWizardBlockVMESelectionTest.h)
ADD_TEST(mafWizardBlockVMESelectionTest   ${EXECUTABLE_OUTPUT_PATH}/mafWizardBlockVMESelectionTest )

ADD_EXECUTABLE(mafWizardManagerTest  mafWizardManagerTest.cpp  mafWizardManagerTest.h)
ADD_TEST(mafWizardManagerTest   ${EXECUTABLE_OUTPUT_PATH}/mafWizardManagerTest )

ADD_EXECUTABLE(mafWizardSettingsTest  mafWizardSettingsTest.cpp  mafWizardSettingsTest.h)
ADD_TEST(mafWizardSettingsTest   ${EXECUTABLE_OUTPUT_PATH}/mafWizardSettingsTest )

ADD_EXECUTABLE(mafWizardWaitOpTest  mafWizardWaitOpTest.cpp  mafWizardWaitOpTest.h)
ADD_TEST(mafWizardWaitOpTest   ${EXECUTABLE_OUTPUT_PATH}/mafWizardWaitOpTest )

ADD_EXECUTABLE(mafWizardTest  mafWizardTest.cpp  mafWizardTest.h)
ADD_TEST(mafWizardTest   ${EXECUTABLE_OUTPUT_PATH}/mafWizardTest )






# wxWidgets specific classes
#IF (MAF_USE_WX)
#ENDIF (MAF_USE_WX)

# VTK specific classes
#IF (MAF_USE_VTK)
#ENDIF (MAF_USE_VTK)  

# classes using VTK+wxWidgets
#IF (MAF_USE_WX AND MAF_USE_VTK)
#ENDIF (MAF_USE_WX AND MAF_USE_VTK)