PROJECT (Gui_TEST)

LINK_LIBRARIES(mafGui)

SUBDIRS(Minimal)
SUBDIRS(01_testGui)
SUBDIRS(02_testFrame)
SUBDIRS(03_testMenu)
SUBDIRS(04_testSash)
SUBDIRS(05_testLog)
SUBDIRS(06_testTab)
SUBDIRS(07_testSideBar)

# wxWidgets specific classes
IF (MAF_USE_WX)
ENDIF (MAF_USE_WX)

# VTK specific classes
IF (MAF_USE_VTK)
ENDIF (MAF_USE_VTK)  

# classes using VTK+wxWidgets
IF (MAF_USE_WX AND MAF_USE_VTK)
ENDIF (MAF_USE_WX AND MAF_USE_VTK)