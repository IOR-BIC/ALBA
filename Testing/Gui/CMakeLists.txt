PROJECT (MedicalGui_TEST)

LINK_LIBRARIES(medGui mafCore)

#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#-----------------------------------------------------------------------------

ADD_EXECUTABLE(medGUISettingsAdvancedTest medGUISettingsAdvancedTest.h medGUISettingsAdvancedTest.cpp)
ADD_TEST(medGUISettingsAdvancedTest ${EXECUTABLE_OUTPUT_PATH}/medGUISettingsAdvancedTest)

ADD_EXECUTABLE(medGUIDicomSettingsTest medGUIDicomSettingsTest.h medGUIDicomSettingsTest.cpp)
ADD_TEST(medGUIDicomSettingsTest ${EXECUTABLE_OUTPUT_PATH}/medGUIDicomSettingsTest)

ADD_EXECUTABLE(medGUIWizardTest medGUIWizardTest.h medGUIWizardTest.cpp)
ADD_TEST(medGUIWizardTest ${EXECUTABLE_OUTPUT_PATH}/medGUIWizardTest)
