PROJECT (mafCore_TEST)

LINK_LIBRARIES(mafCore)

ADD_EXECUTABLE(testEvent mafEventTest.cpp)
ADD_EXECUTABLE(testObserver mafObserverTest.cpp)
ADD_EXECUTABLE(testNode mafNodeTest.cpp)
ADD_EXECUTABLE(testObjectFactory mafObjectFactoryTest.cpp)
ADD_EXECUTABLE(testTagArray testTagArray.cpp)
ADD_EXECUTABLE(testTimeSet TestTimeSet.cpp)
#ADD_EXECUTABLE(testDataSetVector TestDataSetVector.cpp)
#ADD_EXECUTABLE(testMatrixVector TestMatrixVector.cpp)
ADD_EXECUTABLE(testTimeMap TestTimeMap.cpp)

# wxWidgets specific classes
#IF (MAF_USE_WX)
#ENDIF (MAF_USE_WX)

# VTK specific classes
#IF (MAF_USE_VTK)
#ENDIF (MAF_USE_VTK)  

# classes using VTK+wxWidgets
#IF (MAF_USE_WX AND MAF_USE_VTK)
#ENDIF (MAF_USE_WX AND MAF_USE_VTK)