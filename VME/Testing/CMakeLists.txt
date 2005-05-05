PROJECT (mafVME_TEST)

LINK_LIBRARIES(mafVME vtkRendering vtkGraphics)

ADD_EXECUTABLE(testDataVector testDataVector.cpp)
ADD_TEST(testDataVector ${EXECUTABLE_OUTPUT_PATH}/testDataVector)

ADD_EXECUTABLE(testMatrixVector testMatrixVector.cpp)
ADD_TEST(testMatrixVector ${EXECUTABLE_OUTPUT_PATH}/testMatrixVector)

ADD_EXECUTABLE(testVMEGenericPose testVMEGenericPose.cpp)
ADD_TEST(testVMEGenericPose ${EXECUTABLE_OUTPUT_PATH}/testVMEGenericPose)

ADD_EXECUTABLE(testVMEGenericBounds testVMEGenericBounds.cpp)
ADD_TEST(testVMEGenericBounds ${EXECUTABLE_OUTPUT_PATH}/testVMEGenericBounds)

ADD_EXECUTABLE(testVMESlicer testVMESlicer.cpp)
ADD_TEST(testVMESlicer ${EXECUTABLE_OUTPUT_PATH}/testVMESlicer)

ADD_EXECUTABLE(testVMELandmarkCloud testVMELandmarkCloud.cpp)
ADD_TEST(testVMELandmarkCloud ${EXECUTABLE_OUTPUT_PATH}/testVMELandmarkCloud)

ADD_EXECUTABLE(testCustomDataPipe testCustomDataPipe.cpp)
ADD_TEST(testCustomDataPipe ${EXECUTABLE_OUTPUT_PATH}/testCustomDataPipe)

# wxWidgets specific classes
#IF (MAF_USE_WX)
#ENDIF (MAF_USE_WX)

# VTK specific classes
IF (MAF_USE_VTK)
  ADD_EXECUTABLE(testMSF testMSF.cpp)
  ADD_TEST(testMSF ${EXECUTABLE_OUTPUT_PATH}/testMSF)
  ADD_EXECUTABLE(testVMEGeneric testVMEGeneric.cpp)
  ADD_TEST(testVMEGeneric ${EXECUTABLE_OUTPUT_PATH}/testVMEGeneric)
ENDIF (MAF_USE_VTK)  

# classes using VTK+wxWidgets
#IF (MAF_USE_WX AND MAF_USE_VTK)
#ENDIF (MAF_USE_WX AND MAF_USE_VTK)