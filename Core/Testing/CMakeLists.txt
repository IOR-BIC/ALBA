PROJECT (mafCore_TEST)

LINK_LIBRARIES(mafCore)

ADD_EXECUTABLE(mafEventTest mafEventTest.cpp)
ADD_EXECUTABLE(mafObserverTest mafObserverTest.cpp)

# wxWidgets specific classes
IF (MAF_USE_WX)
ENDIF (MAF_USE_WX)

# VTK specific classes
IF (MAF_USE_VTK)
ENDIF (MAF_USE_VTK)  

# classes using VTK+wxWidgets
IF (MAF_USE_WX AND MAF_USE_VTK)
ENDIF (MAF_USE_WX AND MAF_USE_VTK)