PROJECT (mafVME_TEST)

LINK_LIBRARIES(mafVME vtkRendering vtkGraphics)

ADD_EXECUTABLE(testVMEGeneric testVMEGeneric.cpp)
ADD_TEST(testVMEGeneric ${EXECUTABLE_OUTPUT_PATH}/testVMEGeneric)
ADD_EXECUTABLE(testDataVector testDataVector.cpp)
ADD_TEST(testDataVector ${EXECUTABLE_OUTPUT_PATH}/testDataVector)
ADD_EXECUTABLE(testMatrixVector TestMatrixVector.cpp)
ADD_TEST(testMatrixVector ${EXECUTABLE_OUTPUT_PATH}/testMatrixVector)

# wxWidgets specific classes
#IF (MAF_USE_WX)
#ENDIF (MAF_USE_WX)

# VTK specific classes
#IF (MAF_USE_VTK)
#ENDIF (MAF_USE_VTK)  

# classes using VTK+wxWidgets
#IF (MAF_USE_WX AND MAF_USE_VTK)
#ENDIF (MAF_USE_WX AND MAF_USE_VTK)