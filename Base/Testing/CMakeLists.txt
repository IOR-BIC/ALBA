PROJECT (mafCore_TEST)

LINK_LIBRARIES(mafBase)

ADD_EXECUTABLE(testString mafStringTest.cpp)
ADD_TEST(testString ${EXECUTABLE_OUTPUT_PATH}/testString)
ADD_EXECUTABLE(testObject mafObjectTest.cpp)
ADD_TEST(testObject ${EXECUTABLE_OUTPUT_PATH}/testObject)
ADD_EXECUTABLE(testReferenceCount mafReferenceCountedTest.cpp)
ADD_TEST(testReferenceCount ${EXECUTABLE_OUTPUT_PATH}/testReferenceCount)
ADD_EXECUTABLE(testVector mafVectorTest.cpp)
ADD_TEST(testVector ${EXECUTABLE_OUTPUT_PATH}/testVector)
ADD_EXECUTABLE(testTransform mafTransformTest.cpp)
ADD_TEST(testTransform ${EXECUTABLE_OUTPUT_PATH}/testTransform)
#ADD_EXECUTABLE(testIdFactory testIdFactory.cpp)
#ADD_TEST(testIdFactory ${EXECUTABLE_OUTPUT_PATH}/testIdFactory)
#ADD_EXECUTABLE(test_sorted_vector test_sorted_vector.cpp)
#ADD_TEST(test_sorted_vector ${EXECUTABLE_OUTPUT_PATH}/test_sorted_vector)
ADD_EXECUTABLE(testEvent mafEventTest.cpp)
ADD_TEST(testEvent ${EXECUTABLE_OUTPUT_PATH}/testEvent)
ADD_EXECUTABLE(testObserver mafObserverTest.cpp)
ADD_TEST(testObserver ${EXECUTABLE_OUTPUT_PATH}/testObserver)


# wxWidgets specific classes
#IF (MAF_USE_WX)
#ENDIF (MAF_USE_WX)

# VTK specific classes
#IF (MAF_USE_VTK)
#ENDIF (MAF_USE_VTK)  

# classes using VTK+wxWidgets
#IF (MAF_USE_WX AND MAF_USE_VTK)
#ENDIF (MAF_USE_WX AND MAF_USE_VTK)