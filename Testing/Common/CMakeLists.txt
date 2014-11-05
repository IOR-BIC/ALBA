PROJECT (MedicalCommon_TEST)

IF(MED_BUILD_MEDDLL)
  LINK_LIBRARIES(medCommon medVME)
ELSE(MED_BUILD_MEDDLL)
  LINK_LIBRARIES(medVME medCommon)
ENDIF (MED_BUILD_MEDDLL)

#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#----------------------------------------------------------------------------

ADD_EXECUTABLE(mafDataChecksumTest mafDataChecksumTest.h mafDataChecksumTest.cpp)
ADD_TEST(mafDataChecksumTest  ${EXECUTABLE_OUTPUT_PATH}/mafDataChecksumTest)

ADD_EXECUTABLE(medQueryObjectTest medQueryObjectTest.h medQueryObjectTest.cpp)
ADD_TEST(medQueryObjectTest  ${EXECUTABLE_OUTPUT_PATH}/medQueryObjectTest)

ADD_EXECUTABLE(medResultQueryAbstractHandlerTest medResultQueryAbstractHandlerTest.h medResultQueryAbstractHandlerTest.cpp)
ADD_TEST(medResultQueryAbstractHandlerTest  ${EXECUTABLE_OUTPUT_PATH}/medResultQueryAbstractHandlerTest)

ADD_EXECUTABLE(medRelationalDatabaseAbstractConnectorTest medRelationalDatabaseAbstractConnectorTest.h medRelationalDatabaseAbstractConnectorTest.cpp)
ADD_TEST(medRelationalDatabaseAbstractConnectorTest  ${EXECUTABLE_OUTPUT_PATH}/medRelationalDatabaseAbstractConnectorTest)

ADD_EXECUTABLE(medCurvilinearAbscissaOnSkeletonHelperTest medCurvilinearAbscissaOnSkeletonHelperTest.h medCurvilinearAbscissaOnSkeletonHelperTest.cpp)
ADD_TEST(medCurvilinearAbscissaOnSkeletonHelperTest  ${EXECUTABLE_OUTPUT_PATH}/medCurvilinearAbscissaOnSkeletonHelperTest)

ADD_EXECUTABLE(mafGizmoSliceTest mafGizmoSliceTest.h mafGizmoSliceTest.cpp)
ADD_TEST(mafGizmoSliceTest  ${EXECUTABLE_OUTPUT_PATH}/mafGizmoSliceTest)

ADD_EXECUTABLE(medGizmoInteractionDebuggerTest  medGizmoInteractionDebuggerTest.h medGizmoInteractionDebuggerTest.cpp)
ADD_TEST(medGizmoInteractionDebuggerTest   ${EXECUTABLE_OUTPUT_PATH}/medGizmoInteractionDebuggerTest )

ADD_EXECUTABLE(medHTMLTemplateParserTest  medHTMLTemplateParserTest.h medHTMLTemplateParserTest.cpp)
ADD_TEST(medHTMLTemplateParserTest   ${EXECUTABLE_OUTPUT_PATH}/medHTMLTemplateParserTest )

ADD_EXECUTABLE(medHTMLTemplateParserBlockTest  medHTMLTemplateParserBlockTest.h medHTMLTemplateParserBlockTest.cpp)
ADD_TEST(medHTMLTemplateParserBlockTest   ${EXECUTABLE_OUTPUT_PATH}/medHTMLTemplateParserBlockTest )

ADD_EXECUTABLE(medVect3dTest  medVect3dTest.h medVect3dTest.cpp)
ADD_TEST(medVect3dTest   ${EXECUTABLE_OUTPUT_PATH}/medVect3dTest )

ADD_EXECUTABLE(medWizardBlockInformationTest  medWizardBlockInformationTest.cpp  medWizardBlockInformationTest.h)
ADD_TEST(medWizardBlockInformationTest   ${EXECUTABLE_OUTPUT_PATH}/medWizardBlockInformationTest )

ADD_EXECUTABLE(medWizardBlockOperationTest  medWizardBlockOperationTest.cpp  medWizardBlockOperationTest.h)
ADD_TEST(medWizardBlockOperationTest   ${EXECUTABLE_OUTPUT_PATH}/medWizardBlockOperationTest )

ADD_EXECUTABLE(medWizardBlockSelectionTest  medWizardBlockSelectionTest.cpp  medWizardBlockSelectionTest.h)
ADD_TEST(medWizardBlockSelectionTest   ${EXECUTABLE_OUTPUT_PATH}/medWizardBlockSelectionTest )

ADD_EXECUTABLE(medWizardBlockYesNoSelectionTest  medWizardBlockYesNoSelectionTest.cpp  medWizardBlockYesNoSelectionTest.h)
ADD_TEST(medWizardBlockYesNoSelectionTest   ${EXECUTABLE_OUTPUT_PATH}/medWizardBlockYesNoSelectionTest )

ADD_EXECUTABLE(medWizardBlockTest  medWizardBlockTest.cpp  medWizardBlockTest.h)
ADD_TEST(medWizardBlockTest   ${EXECUTABLE_OUTPUT_PATH}/medWizardBlockTest )

ADD_EXECUTABLE(medWizardBlockTypeCheckTest  medWizardBlockTypeCheckTest.cpp  medWizardBlockTypeCheckTest.h)
ADD_TEST(medWizardBlockTypeCheckTest   ${EXECUTABLE_OUTPUT_PATH}/medWizardBlockTypeCheckTest )

ADD_EXECUTABLE(medWizardBlockVMESelectionTest  medWizardBlockVMESelectionTest.cpp   medWizardBlockVMESelectionTest.h)
ADD_TEST(medWizardBlockVMESelectionTest   ${EXECUTABLE_OUTPUT_PATH}/medWizardBlockVMESelectionTest )

ADD_EXECUTABLE(medWizardManagerTest  medWizardManagerTest.cpp  medWizardManagerTest.h)
ADD_TEST(medWizardManagerTest   ${EXECUTABLE_OUTPUT_PATH}/medWizardManagerTest )

ADD_EXECUTABLE(medWizardSettingsTest  medWizardSettingsTest.cpp  medWizardSettingsTest.h)
ADD_TEST(medWizardSettingsTest   ${EXECUTABLE_OUTPUT_PATH}/medWizardSettingsTest )

ADD_EXECUTABLE(medWizardWaitOpTest  medWizardWaitOpTest.cpp  medWizardWaitOpTest.h)
ADD_TEST(medWizardWaitOpTest   ${EXECUTABLE_OUTPUT_PATH}/medWizardWaitOpTest )

ADD_EXECUTABLE(medWizardTest  medWizardTest.cpp  medWizardTest.h)
ADD_TEST(medWizardTest   ${EXECUTABLE_OUTPUT_PATH}/medWizardTest )






# wxWidgets specific classes
#IF (MAF_USE_WX)
#ENDIF (MAF_USE_WX)

# VTK specific classes
IF (MAF_USE_VTK)
ENDIF (MAF_USE_VTK)  

# classes using VTK+wxWidgets
#IF (MAF_USE_WX AND MAF_USE_VTK)
#ENDIF (MAF_USE_WX AND MAF_USE_VTK)