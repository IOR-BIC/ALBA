PROJECT (MedicalVME_TEST)

LINK_LIBRARIES(medVME)
LINK_LIBRARIES(medCommon)
#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#-----------------------------------------------------------------------------

ADD_EXECUTABLE(medGeometryEditorPolylineGraphTest medGeometryEditorPolylineGraphTest.h medGeometryEditorPolylineGraphTest.cpp)
ADD_TEST(medGeometryEditorPolylineGraphTest  ${EXECUTABLE_OUTPUT_PATH}/medGeometryEditorPolylineGraphTest)

ADD_EXECUTABLE(mafDataChecksumTest mafDataChecksumTest.h mafDataChecksumTest.cpp)
ADD_TEST(mafDataChecksumTest  ${EXECUTABLE_OUTPUT_PATH}/mafDataChecksumTest)

ADD_EXECUTABLE(medQueryObjectTest medQueryObjectTest.h medQueryObjectTest.cpp)
ADD_TEST(medQueryObjectTest  ${EXECUTABLE_OUTPUT_PATH}/medQueryObjectTest)

ADD_EXECUTABLE(medResultQueryAbstractHandlerTest medResultQueryAbstractHandlerTest.h medResultQueryAbstractHandlerTest.cpp)
ADD_TEST(medResultQueryAbstractHandlerTest  ${EXECUTABLE_OUTPUT_PATH}/medResultQueryAbstractHandlerTest)

ADD_EXECUTABLE(medRelationalDatabaseAbstractConnectorTest medRelationalDatabaseAbstractConnectorTest.h medRelationalDatabaseAbstractConnectorTest.cpp)
ADD_TEST(medRelationalDatabaseAbstractConnectorTest  ${EXECUTABLE_OUTPUT_PATH}/medRelationalDatabaseAbstractConnectorTest)



# wxWidgets specific classes
#IF (MAF_USE_WX)
#ENDIF (MAF_USE_WX)

# VTK specific classes
IF (MAF_USE_VTK)
ENDIF (MAF_USE_VTK)  

# classes using VTK+wxWidgets
#IF (MAF_USE_WX AND MAF_USE_VTK)
#ENDIF (MAF_USE_WX AND MAF_USE_VTK)