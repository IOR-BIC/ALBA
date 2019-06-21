PROJECT (Gui_TEST)

# albaGUI e' indipendente da Core, ma i test no -- Silvano 04/04/05
IF(ALBA_BUILD_ALBADLL)
  LINK_LIBRARIES(albaDLL)
ELSE(ALBA_BUILD_ALBADLL)
  LINK_LIBRARIES(albaTests albaCore albaVME albaGui albaService)
ENDIF (ALBA_BUILD_ALBADLL)

#SUBDIRS(Minimal)
#SUBDIRS(01_testGui)
#SUBDIRS(02_testFrame)
#SUBDIRS(03_testMenu)
#SUBDIRS(04_testSash)
#SUBDIRS(05_testLog)
#SUBDIRS(06_testTab)
#SUBDIRS(07_testSideBar)
#SUBDIRS(08_testStatusBar)
#SUBDIRS(09_testTimeBar)
#SUBDIRS(10_testPicFactory)
#SUBDIRS(11_testTree)
#SUBDIRS(12_Bmp2Xpm)
#SUBDIRS(13_testCheckTree)
#SUBDIRS(14_testOpMan)
#SUBDIRS(15_testDialog)
#SUBDIRS(16_testRWI)
#SUBDIRS(17_TestWXVTK)
#SUBDIRS(18_testView)
#SUBDIRS(19_testVMEChooser)


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
	albaGUILandmarkTest.h 			albaGUILandmarkTest.cpp
	albaGUIWizardPageNewTest.h 		albaGUIWizardPageNewTest.cpp
)

ADD_EXECUTABLE(albaGuiTests ${PROJECT_SRCS})
ADD_TEST(albaGuiTests  ${EXECUTABLE_OUTPUT_PATH}/albaGuiTests)
IF (WIN32)
  SET_TARGET_PROPERTIES(albaGuiTests PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)