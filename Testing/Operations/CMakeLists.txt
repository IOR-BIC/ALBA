PROJECT (MedicalOperations_TEST)

LINK_LIBRARIES(medVME)
LINK_LIBRARIES(medOperations)
LINK_LIBRARIES(medCommon)



#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#-----------------------------------------------------------------------------

IF (MED_USE_Offis)
ADD_EXECUTABLE(medOpImporterDicomOffisTest medOpImporterDicomOffisTest.h medOpImporterDicomOffisTest.cpp)
ADD_TEST(medOpImporterDicomOffisTest ${EXECUTABLE_OUTPUT_PATH}/medOpImporterDicomOffisTest)

ADD_EXECUTABLE(medDicomCardiacMRIHelperTest medDicomCardiacMRIHelperTest.h medDicomCardiacMRIHelperTest.cpp)
ADD_TEST(medDicomCardiacMRIHelperTest ${EXECUTABLE_OUTPUT_PATH}/medDicomCardiacMRIHelperTest)
ENDIF (MED_USE_Offis)

ADD_EXECUTABLE(itkRawMotionImporterUtilityTest itkRawMotionImporterUtilityTest.h itkRawMotionImporterUtilityTest.cpp)
ADD_TEST(itkRawMotionImporterUtilityTest ${EXECUTABLE_OUTPUT_PATH}/itkRawMotionImporterUtilityTest)

ADD_EXECUTABLE(medOpMakeVMETimevaryingTest medOpMakeVMETimevaryingTest.h medOpMakeVMETimevaryingTest.cpp)
ADD_TEST(medOpMakeVMETimevaryingTest ${EXECUTABLE_OUTPUT_PATH}/medOpMakeVMETimevaryingTest)

ADD_EXECUTABLE(medOpEqualizeHistogramTest medOpEqualizeHistogramTest.h medOpEqualizeHistogramTest.cpp)
ADD_TEST(medOpEqualizeHistogramTest ${EXECUTABLE_OUTPUT_PATH}/medOpEqualizeHistogramTest)

ADD_EXECUTABLE(medOpCreateWrappedMeterTest medOpCreateWrappedMeterTest.h medOpCreateWrappedMeterTest.cpp)
ADD_TEST(medOpCreateWrappedMeterTest ${EXECUTABLE_OUTPUT_PATH}/medOpCreateWrappedMeterTest)

ADD_EXECUTABLE(medOpFlipNormalsTest medOpFlipNormalsTest.h medOpFlipNormalsTest.cpp)
ADD_TEST(medOpFlipNormalsTest ${EXECUTABLE_OUTPUT_PATH}/medOpFlipNormalsTest)

ADD_EXECUTABLE(medOpSmoothSurfaceCellsTest medOpSmoothSurfaceCellsTest.h medOpSmoothSurfaceCellsTest.cpp)
ADD_TEST(medOpSmoothSurfaceCellsTest ${EXECUTABLE_OUTPUT_PATH}/medOpSmoothSurfaceCellsTest)

ADD_EXECUTABLE(medOpMeshQualityTest medOpMeshQualityTest.h medOpMeshQualityTest.cpp)
ADD_TEST(medOpMeshQualityTest ${EXECUTABLE_OUTPUT_PATH}/medOpMeshQualityTest)

# wxWidgets specific classes
#IF (MAF_USE_WX)
#ENDIF (MAF_USE_WX)

# VTK specific classes
IF (MAF_USE_VTK)
ENDIF (MAF_USE_VTK)  

# classes using VTK+wxWidgets
#IF (MAF_USE_WX AND MAF_USE_VTK)
#ENDIF (MAF_USE_WX AND MAF_USE_VTK)

