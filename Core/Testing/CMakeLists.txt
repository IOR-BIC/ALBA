PROJECT (mafCore_TEST)

LINK_LIBRARIES(mafCore)

ADD_EXECUTABLE(testEvent mafEventTest.cpp)
ADD_EXECUTABLE(testObserver mafObserverTest.cpp)
ADD_EXECUTABLE(testString mafStringTest.cpp)
ADD_EXECUTABLE(testObject mafObjectTest.cpp)
ADD_EXECUTABLE(testSmartObject mafSmartObjectTest.cpp)
ADD_EXECUTABLE(testObjectFactory mafObjectFactoryTest.cpp)
ADD_EXECUTABLE(testNode mafNodeTest.cpp)
ADD_EXECUTABLE(testVector mafVectorTest.cpp)

# wxWidgets specific classes
IF (MAF_USE_WX)
ENDIF (MAF_USE_WX)

# VTK specific classes
IF (MAF_USE_VTK)
  ADD_EXECUTABLE(testTransform mafTransformTest.cpp)
ENDIF (MAF_USE_VTK)  

# classes using VTK+wxWidgets
IF (MAF_USE_WX AND MAF_USE_VTK)
ENDIF (MAF_USE_WX AND MAF_USE_VTK)