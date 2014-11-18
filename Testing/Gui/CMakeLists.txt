PROJECT (Gui_TEST)

# mafGUI e' indipendente da Core, ma i test no -- Silvano 04/04/05
IF(MAF_BUILD_MAFDLL)
  LINK_LIBRARIES(mafDLL)
ELSE(MAF_BUILD_MAFDLL)
  LINK_LIBRARIES(mafCore mafVME mafGui)
ENDIF (MAF_BUILD_MAFDLL)

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
############ mafGUISettingsStorageTest
ADD_EXECUTABLE(mafGUISettingsStorageTest mafGUISettingsStorageTest.h mafGUISettingsStorageTest.cpp)
ADD_TEST(mafGUISettingsStorageTest ${EXECUTABLE_OUTPUT_PATH}/mafGUISettingsStorageTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafGUISettingsStorageTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafAttachCameraTest
ADD_EXECUTABLE(mafAttachCameraTest mafAttachCameraTest.h mafAttachCameraTest.cpp)
ADD_TEST(mafAttachCameraTest ${EXECUTABLE_OUTPUT_PATH}/mafAttachCameraTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafAttachCameraTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############# added from medical
ADD_EXECUTABLE(medGUISettingsAdvancedTest medGUISettingsAdvancedTest.h medGUISettingsAdvancedTest.cpp)
ADD_TEST(medGUISettingsAdvancedTest ${EXECUTABLE_OUTPUT_PATH}/medGUISettingsAdvancedTest)

ADD_EXECUTABLE(medGUIDicomSettingsTest medGUIDicomSettingsTest.h medGUIDicomSettingsTest.cpp)
ADD_TEST(medGUIDicomSettingsTest ${EXECUTABLE_OUTPUT_PATH}/medGUIDicomSettingsTest)

ADD_EXECUTABLE(medGUIWizardTest medGUIWizardTest.h medGUIWizardTest.cpp)
ADD_TEST(medGUIWizardTest ${EXECUTABLE_OUTPUT_PATH}/medGUIWizardTest)

ADD_EXECUTABLE(medGUIWizardPageTest medGUIWizardPageTest.h medGUIWizardPageTest.cpp)
ADD_TEST(medGUIWizardPageTest ${EXECUTABLE_OUTPUT_PATH}/medGUIWizardPageTest)

ADD_EXECUTABLE(medGUILandmarkTest medGUILandmarkTest.h medGUILandmarkTest.cpp)
ADD_TEST(medGUILandmarkTest ${EXECUTABLE_OUTPUT_PATH}/medGUILandmarkTest)

ADD_EXECUTABLE(medGUIWizardPageNewTest medGUIWizardPageNewTest.h medGUIWizardPageNewTest.cpp)
ADD_TEST(medGUIWizardPageNewTest ${EXECUTABLE_OUTPUT_PATH}/medGUIWizardPageNewTest)
