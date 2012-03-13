PROJECT (Gui_TEST)

# mafGUI e' indipendente da Core, ma i test no -- Silvano 04/04/05
LINK_LIBRARIES(mafCore mafVME)

#SUBDIRS(Minimal)
SUBDIRS(01_testGui)
SUBDIRS(02_testFrame)
SUBDIRS(03_testMenu)
SUBDIRS(04_testSash)
SUBDIRS(05_testLog)
SUBDIRS(06_testTab)
SUBDIRS(07_testSideBar)
SUBDIRS(08_testStatusBar)
SUBDIRS(09_testTimeBar)
SUBDIRS(10_testPicFactory)
SUBDIRS(11_testTree)
SUBDIRS(12_Bmp2Xpm)
SUBDIRS(13_testCheckTree)
SUBDIRS(14_testOpMan)
SUBDIRS(15_testDialog)
SUBDIRS(16_testRWI)
#SUBDIRS(17_TestWXVTK)
SUBDIRS(18_testView)
SUBDIRS(19_testVMEChooser)

#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#-----------------------------------------------------------------------------
############ mafGUISettingsStorageTest
ADD_EXECUTABLE(mafGUISettingsStorageTest mafGUISettingsStorageTest.h mafGUISettingsStorageTest.cpp)
ADD_TEST(mafGUISettingsStorageTest ${EXECUTABLE_OUTPUT_PATH}/mafGUISettingsStorageTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafGUISettingsStorageTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)
