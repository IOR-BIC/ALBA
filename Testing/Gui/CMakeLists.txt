PROJECT (albaGuiTests)

IF(ALBA_BUILD_ALBADLL)
  SET(PROJECT_LIBS albaDLL vtkALBA albaTests)
ELSE(ALBA_BUILD_ALBADLL)
  SET(PROJECT_LIBS albaTests GPUAPI albaCore albaVME albaGui albaVisualPipes albaCommon albaViews albaInteraction albaService)
ENDIF(ALBA_BUILD_ALBADLL)

#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#-----------------------------------------------------------------------------
SET(PROJECT_SRCS 
	albaGuiTests.h 					albaGuiTests.cpp
	albaAttachCameraTest.h 			albaAttachCameraTest.cpp
	albaGUISettingsAdvancedTest.h 	albaGUISettingsAdvancedTest.cpp
	albaGUIDicomSettingsTest.h 		albaGUIDicomSettingsTest.cpp
	albaGUIWizardTest.h 				albaGUIWizardTest.cpp
	albaGUIWizardPageTest.h 			albaGUIWizardPageTest.cpp
	albaGUIWizardPageNewTest.h 		albaGUIWizardPageNewTest.cpp
)

ADD_EXECUTABLE(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
ADD_TEST(${PROJECT_NAME}  ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME})
IF (WIN32)
  SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)