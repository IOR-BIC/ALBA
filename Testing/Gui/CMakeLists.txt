PROJECT (albaGuiTests)

IF(ALBA_BUILD_ALBADLL)
  SET(PROJECT_LIBS albaDLL vtkALBA albaTests)
ELSE(ALBA_BUILD_ALBADLL)
  SET(PROJECT_LIBS albaTests albaCore albaVME albaGui albaService albaViews albaVisualPipes)
ENDIF(ALBA_BUILD_ALBADLL)


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

ADD_EXECUTABLE(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
ADD_TEST(${PROJECT_NAME}  ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME})
IF (WIN32)
  SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)