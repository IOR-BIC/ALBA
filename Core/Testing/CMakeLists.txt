PROJECT (mafCore_TEST)

LINK_LIBRARIES(mafCore)

ADD_EXECUTABLE(testEvent mafEventTest.cpp)
ADD_TEST(testEvent ${EXECUTABLE_OUTPUT_PATH}/testEvent)
ADD_EXECUTABLE(testObserver mafObserverTest.cpp)
ADD_TEST(testObserver ${EXECUTABLE_OUTPUT_PATH}/testObserver)
ADD_EXECUTABLE(testNode mafNodeTest.cpp)
ADD_TEST(testNode ${EXECUTABLE_OUTPUT_PATH}/testNode)
ADD_EXECUTABLE(testObjectFactory mafObjectFactoryTest.cpp)
ADD_TEST(testObjectFactory ${EXECUTABLE_OUTPUT_PATH}/testObjectFactory)
ADD_EXECUTABLE(testTagArray testTagArray.cpp)
ADD_TEST(testTagArray ${EXECUTABLE_OUTPUT_PATH}/testTagArray)
ADD_EXECUTABLE(testTimeSet TestTimeSet.cpp)
ADD_TEST(testTimeSet ${EXECUTABLE_OUTPUT_PATH}/testTimeSet)
ADD_EXECUTABLE(testTimeMap TestTimeMap.cpp)
ADD_TEST(testTimeMap ${EXECUTABLE_OUTPUT_PATH}/testTimeMap)
#ADD_EXECUTABLE(testVME testVME.cpp)
#ADD_TEST(testVME ${EXECUTABLE_OUTPUT_PATH}/testVME)

# wxWidgets specific classes
#IF (MAF_USE_WX)
#ENDIF (MAF_USE_WX)

# VTK specific classes
#IF (MAF_USE_VTK)
#ENDIF (MAF_USE_VTK)  

# classes using VTK+wxWidgets
#IF (MAF_USE_WX AND MAF_USE_VTK)
#ENDIF (MAF_USE_WX AND MAF_USE_VTK)