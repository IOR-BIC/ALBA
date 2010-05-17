PROJECT (MedicalGui_TEST)

LINK_LIBRARIES(medGui)

#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#-----------------------------------------------------------------------------

ADD_EXECUTABLE(medGUISettingsAdvancedTest medGUISettingsAdvancedTest.h medGUISettingsAdvancedTest.cpp)
ADD_TEST(medGUISettingsAdvancedTest ${EXECUTABLE_OUTPUT_PATH}/medGUISettingsAdvancedTest)
