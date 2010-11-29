PROJECT (MedicalVME_TEST)

LINK_LIBRARIES(medVME)
LINK_LIBRARIES(medCommon)
LINK_LIBRARIES(medOperations)
#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#-----------------------------------------------------------------------------


ADD_EXECUTABLE(medVMEOutputComputeWrappingTest medVMEOutputComputeWrappingTest.h medVMEOutputComputeWrappingTest.cpp)
ADD_TEST(medVMEOutputComputeWrappingTest  ${EXECUTABLE_OUTPUT_PATH}/medVMEOutputComputeWrappingTest)

ADD_EXECUTABLE(medPipeTensorFieldTest medPipeTensorFieldTest.h medPipeTensorFieldTest.cpp)
ADD_TEST(medPipeTensorFieldTest  ${EXECUTABLE_OUTPUT_PATH}/medPipeTensorFieldTest)

ADD_EXECUTABLE(medPipeVectorFieldTest medPipeVectorFieldTest.h medPipeVectorFieldTest.cpp)
ADD_TEST(medPipeVectorFieldTest  ${EXECUTABLE_OUTPUT_PATH}/medPipeVectorFieldTest)

ADD_EXECUTABLE(medVisualPipeSlicerSliceTest  medVisualPipeSlicerSliceTest.h medVisualPipeSlicerSliceTest.cpp)
ADD_TEST(medVisualPipeSlicerSliceTest ${EXECUTABLE_OUTPUT_PATH}/medVisualPipeSlicerSliceTest)

ADD_EXECUTABLE(medPipeCompoundVolumeVRTest  medPipeCompoundVolumeVRTest.h medPipeCompoundVolumeVRTest.cpp)
ADD_TEST(medPipeCompoundVolumeVRTest ${EXECUTABLE_OUTPUT_PATH}/medPipeCompoundVolumeVRTest)

ADD_EXECUTABLE(medPipeCompoundVolumeMIPTest  medPipeCompoundVolumeMIPTest.h medPipeCompoundVolumeMIPTest.cpp)
ADD_TEST(medPipeCompoundVolumeMIPTest ${EXECUTABLE_OUTPUT_PATH}/medPipeCompoundVolumeMIPTest)


#to be checked
#ADD_EXECUTABLE(medPipeVolumeVRTest medPipeVolumeVRTest.h medPipeVolumeVRTest.cpp)
#ADD_TEST(medPipeVolumeVRTest  ${EXECUTABLE_OUTPUT_PATH}/medPipeVolumeVRTest)


# wxWidgets specific classes
#IF (MAF_USE_WX)
#ENDIF (MAF_USE_WX)

# VTK specific classes
IF (MAF_USE_VTK)
ENDIF (MAF_USE_VTK)  

# classes using VTK+wxWidgets
#IF (MAF_USE_WX AND MAF_USE_VTK)
#ENDIF (MAF_USE_WX AND MAF_USE_VTK)