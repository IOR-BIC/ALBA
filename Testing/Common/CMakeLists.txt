PROJECT (MedicalVME_TEST)

LINK_LIBRARIES(medVME)
LINK_LIBRARIES(medCommon)
#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#-----------------------------------------------------------------------------

ADD_EXECUTABLE(medGeometryEditorPolylineGraphTest medGeometryEditorPolylineGraphTest.h medGeometryEditorPolylineGraphTest.cpp)
ADD_TEST(medGeometryEditorPolylineGraphTest  ${EXECUTABLE_OUTPUT_PATH}/medGeometryEditorPolylineGraphTest)

# wxWidgets specific classes
#IF (MAF_USE_WX)
#ENDIF (MAF_USE_WX)

# VTK specific classes
IF (MAF_USE_VTK)
ENDIF (MAF_USE_VTK)  

# classes using VTK+wxWidgets
#IF (MAF_USE_WX AND MAF_USE_VTK)
#ENDIF (MAF_USE_WX AND MAF_USE_VTK)