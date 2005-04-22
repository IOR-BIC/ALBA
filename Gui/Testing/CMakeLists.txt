PROJECT (Gui_TEST)

# mafGUI e' indipendente da Core, ma i test no -- Silvano 04/04/05
LINK_LIBRARIES(mafGui mafCore)

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
SUBDIRS(17_TestWXVTK)
SUBDIRS(18_testView)

# wxWidgets specific classes
IF (MAF_USE_WX)
ENDIF (MAF_USE_WX)

# VTK specific classes
IF (MAF_USE_VTK)
ENDIF (MAF_USE_VTK)  

# classes using VTK+wxWidgets
IF (MAF_USE_WX AND MAF_USE_VTK)
ENDIF (MAF_USE_WX AND MAF_USE_VTK)