PROJECT (Gui_TEST)

LINK_LIBRARIES(mafGui)

ADD_EXECUTABLE(mmgGuiTest mafGuiTest.cpp)
ADD_TEST(mmgGuiTest mafStringTest)

# wxWidgets specific classes
IF (MAF_USE_WX)
ENDIF (MAF_USE_WX)

# VTK specific classes
IF (MAF_USE_VTK)
ENDIF (MAF_USE_VTK)  

# classes using VTK+wxWidgets
IF (MAF_USE_WX AND MAF_USE_VTK)
ENDIF (MAF_USE_WX AND MAF_USE_VTK)