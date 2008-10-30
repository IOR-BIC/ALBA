PROJECT (MedicalVME_TEST)

LINK_LIBRARIES(medVME)
LINK_LIBRARIES(medCommon)
LINK_LIBRARIES(medOperations)
#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#-----------------------------------------------------------------------------
IF (MAF_USE_ITK)
  ADD_EXECUTABLE(mafVMERawMotionDataTest mafVMERawMotionDataTest.h mafVMERawMotionDataTest.cpp)
  ADD_TEST(mafVMERawMotionDataTest  ${EXECUTABLE_OUTPUT_PATH}/mafVMERawMotionDataTest)
ENDIF (MAF_USE_ITK)

ADD_EXECUTABLE(mafPolylineGraphTest mafPolylineGraphTest.h mafPolylineGraphTest.cpp)
ADD_TEST(mafPolylineGraphTest  ${EXECUTABLE_OUTPUT_PATH}/mafPolylineGraphTest)

ADD_EXECUTABLE(medVMEWrappedMeterTest medVMEWrappedMeterTest.h medVMEWrappedMeterTest.cpp)
ADD_TEST(medVMEWrappedMeterTest  ${EXECUTABLE_OUTPUT_PATH}/medVMEWrappedMeterTest)

ADD_EXECUTABLE(medVMELabeledVolumeTest medVMELabeledVolumeTest.h medVMELabeledVolumeTest.cpp)
ADD_TEST(medVMELabeledVolumeTest  ${EXECUTABLE_OUTPUT_PATH}/medVMELabeledVolumeTest)

ADD_EXECUTABLE(medPipeVolumeMIPTest medPipeVolumeMIPTest.h medPipeVolumeMIPTest.cpp)
ADD_TEST(medPipeVolumeMIPTest  ${EXECUTABLE_OUTPUT_PATH}/medPipeVolumeMIPTest)

ADD_EXECUTABLE(medPipeFactoryVMETest medPipeFactoryVMETest.h medPipeFactoryVMETest.cpp)
ADD_TEST(medPipeFactoryVMETest  ${EXECUTABLE_OUTPUT_PATH}/medPipeFactoryVMETest)

# wxWidgets specific classes
#IF (MAF_USE_WX)
#ENDIF (MAF_USE_WX)

# VTK specific classes
IF (MAF_USE_VTK)
ENDIF (MAF_USE_VTK)  

# classes using VTK+wxWidgets
#IF (MAF_USE_WX AND MAF_USE_VTK)
#ENDIF (MAF_USE_WX AND MAF_USE_VTK)