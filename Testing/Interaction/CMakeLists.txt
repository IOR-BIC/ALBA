PROJECT (mafInteraction_TEST)

LINK_LIBRARIES(medInteraction)
LINK_LIBRARIES(medVME)
LINK_LIBRARIES(medCommon)

#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#-----------------------------------------------------------------------------

############ mafGizmoROI_BESTest
ADD_EXECUTABLE(mafGizmoROI_BESTest mafGizmoROI_BESTest.h mafGizmoROI_BESTest.cpp)
ADD_TEST(mafGizmoROI_BESTest ${EXECUTABLE_OUTPUT_PATH}/mafGizmoROI_BESTest)

############ medGizmoPolylineGraphTest
ADD_EXECUTABLE(medGizmoPolylineGraphTest medGizmoPolylineGraphTest.h medGizmoPolylineGraphTest.cpp)
ADD_TEST(medGizmoPolylineGraphTest ${EXECUTABLE_OUTPUT_PATH}/medGizmoPolylineGraphTest)

IF (WIN32)
  SET_TARGET_PROPERTIES(mafGizmoROI_BESTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)