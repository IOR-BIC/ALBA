PROJECT (MedicalCommon_TEST)

IF(MED_BUILD_MEDDLL)
  LINK_LIBRARIES(medDLL)
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


# wxWidgets specific classes
#IF (MAF_USE_WX)
#ENDIF (MAF_USE_WX)

# VTK specific classes
IF (MAF_USE_VTK)
ENDIF (MAF_USE_VTK)  

# classes using VTK+wxWidgets
#IF (MAF_USE_WX AND MAF_USE_VTK)
#ENDIF (MAF_USE_WX AND MAF_USE_VTK)