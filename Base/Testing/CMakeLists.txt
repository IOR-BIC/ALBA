PROJECT (mafBase_TEST)

LINK_LIBRARIES(mafBase)

ADD_EXECUTABLE(mafStringTest mafStringTest.cpp)
ADD_EXECUTABLE(mafObjectTest mafObjectTest.cpp)
ADD_EXECUTABLE(mafObjectFactoryTest mafObjectFactoryTest.cpp)

# wxWidgets specific classes
IF (MAF_USE_WX)
ENDIF (MAF_USE_WX)

# VTK specific classes
IF (MAF_USE_VTK)
ENDIF (MAF_USE_VTK)  

# classes using VTK+wxWidgets
IF (MAF_USE_WX AND MAF_USE_VTK)
ENDIF (MAF_USE_WX AND MAF_USE_VTK)