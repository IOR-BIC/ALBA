PROJECT (mafInteraction_TEST)

LINK_LIBRARIES(mafInteraction mafCore mafVME mafBase)

#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#-----------------------------------------------------------------------------
############ mafGizmoPathTest
ADD_EXECUTABLE(mafGizmoPathTest mafGizmoPathTest.h mafGizmoPathTest.cpp)
ADD_TEST(mafGizmoPathTest ${EXECUTABLE_OUTPUT_PATH}/mafGizmoPathTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafGizmoPathTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafGizmoPathRulerTest
ADD_EXECUTABLE(mafGizmoPathRulerTest mafGizmoPathRulerTest.h mafGizmoPathRulerTest.cpp)
ADD_TEST(mafGizmoPathRulerTest ${EXECUTABLE_OUTPUT_PATH}/mafGizmoPathRulerTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafGizmoPathRulerTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mmiSelectCellTest
ADD_EXECUTABLE(mmiSelectCellTest mmiSelectCellTest.h mmiSelectCellTest.cpp)
ADD_TEST(mmiSelectCellTest ${EXECUTABLE_OUTPUT_PATH}/mmiSelectCellTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mmiSelectCellTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafGizmoTranslateAxisTest
ADD_EXECUTABLE(mafGizmoTranslateAxisTest mafGizmoTranslateAxisTest.h mafGizmoTranslateAxisTest.cpp)
ADD_TEST(mafGizmoTranslateAxisTest ${EXECUTABLE_OUTPUT_PATH}/mafGizmoTranslateAxisTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafGizmoTranslateAxisTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafGizmoTranslatePlaneTest
ADD_EXECUTABLE(mafGizmoTranslatePlaneTest mafGizmoTranslatePlaneTest.h mafGizmoTranslatePlaneTest.cpp)
ADD_TEST(mafGizmoTranslatePlaneTest ${EXECUTABLE_OUTPUT_PATH}/mafGizmoTranslatePlaneTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafGizmoTranslatePlaneTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafGizmoBoundingBoxTest
ADD_EXECUTABLE(mafGizmoBoundingBoxTest mafGizmoBoundingBoxTest.h mafGizmoBoundingBoxTest.cpp)
ADD_TEST(mafGizmoBoundingBoxTest ${EXECUTABLE_OUTPUT_PATH}/mafGizmoBoundingBoxTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafGizmoBoundingBoxTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafGizmoScaleIsotropicTest
ADD_EXECUTABLE(mafGizmoScaleIsotropicTest mafGizmoScaleIsotropicTest.h mafGizmoScaleIsotropicTest.cpp)
ADD_TEST(mafGizmoScaleIsotropicTest ${EXECUTABLE_OUTPUT_PATH}/mafGizmoScaleIsotropicTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafGizmoScaleIsotropicTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafGizmoRotateFanTest
ADD_EXECUTABLE(mafGizmoRotateFanTest mafGizmoRotateFanTest.h mafGizmoRotateFanTest.cpp)
ADD_TEST(mafGizmoRotateFanTest ${EXECUTABLE_OUTPUT_PATH}/mafGizmoRotateFanTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafGizmoRotateFanTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)


############ mafGizmoROITest
ADD_EXECUTABLE(mafGizmoROITest mafGizmoROITest.h mafGizmoROITest.cpp)
ADD_TEST(mafGizmoROITest ${EXECUTABLE_OUTPUT_PATH}/mafGizmoROITest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafGizmoROITest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafAgentTest
ADD_EXECUTABLE(mafAgentTest mafAgentTest.h mafAgentTest.cpp)
ADD_TEST(mafAgentTest ${EXECUTABLE_OUTPUT_PATH}/mafAgentTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafAgentTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)


