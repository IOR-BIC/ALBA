PROJECT (mafCore_TEST)

LINK_LIBRARIES(mafBase)

ADD_EXECUTABLE(testString mafStringTest.cpp)
ADD_EXECUTABLE(testObject mafObjectTest.cpp)
ADD_EXECUTABLE(testReferenceCount mafReferenceCountedTest.cpp)
ADD_EXECUTABLE(testVector mafVectorTest.cpp)
ADD_EXECUTABLE(testTransform mafTransformTest.cpp)

# wxWidgets specific classes
#IF (MAF_USE_WX)
#ENDIF (MAF_USE_WX)

# VTK specific classes
#IF (MAF_USE_VTK)
#ENDIF (MAF_USE_VTK)  

# classes using VTK+wxWidgets
#IF (MAF_USE_WX AND MAF_USE_VTK)
#ENDIF (MAF_USE_WX AND MAF_USE_VTK)