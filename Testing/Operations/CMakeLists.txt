PROJECT (MedicalOperations_TEST)

LINK_LIBRARIES(medVME)
LINK_LIBRARIES(medOperations)
LINK_LIBRARIES(medCommon)

#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#-----------------------------------------------------------------------------
IF (MAF_USE_ITK)
  ADD_EXECUTABLE(mmoLandmarkExporterTest mmoLandmarkExporterTest.h mmoLandmarkExporterTest.cpp)
  ADD_TEST(mmoLandmarkExporterTest ${EXECUTABLE_OUTPUT_PATH}/mmoLandmarkExporterTest)

  ADD_EXECUTABLE(mmoLandmarkImporterTest mmoLandmarkImporterTest.h mmoLandmarkImporterTest.cpp)
  ADD_TEST(mmoLandmarkImporterTest ${EXECUTABLE_OUTPUT_PATH}/mmoLandmarkImporterTest)
  
ENDIF (MAF_USE_ITK)

ADD_EXECUTABLE(mmoVolumeMeasureTest mmoVolumeMeasureTest.h mmoVolumeMeasureTest.cpp)
ADD_TEST(mmoVolumeMeasureTest ${EXECUTABLE_OUTPUT_PATH}/mmoVolumeMeasureTest)

ADD_EXECUTABLE(mmoRAWImporterImagesTest mmoRAWImporterImagesTest.h mmoRAWImporterImagesTest.cpp)
ADD_TEST(mmoRAWImporterImagesTest ${EXECUTABLE_OUTPUT_PATH}/mmoRAWImporterImagesTest)

ADD_EXECUTABLE(mmoLandmarkImporterTXTTest mmoLandmarkImporterTXTTest.h mmoLandmarkImporterTXTTest.cpp)
ADD_TEST(mmoLandmarkImporterTXTTest ${EXECUTABLE_OUTPUT_PATH}/mmoLandmarkImporterTXTTest)

 ADD_EXECUTABLE(mmoLandmarkImporterWSTest mmoLandmarkImporterWSTest.h mmoLandmarkImporterWSTest.cpp)
  ADD_TEST(mmoLandmarkImporterWSTest ${EXECUTABLE_OUTPUT_PATH}/mmoLandmarkImporterWSTest)
  
ADD_EXECUTABLE(mmoExtrusionHolesTest mmoExtrusionHolesTest.h mmoExtrusionHolesTest.cpp)
ADD_TEST(mmoExtrusionHolesTest ${EXECUTABLE_OUTPUT_PATH}/mmoExtrusionHolesTest)

# wxWidgets specific classes
#IF (MAF_USE_WX)
#ENDIF (MAF_USE_WX)

# VTK specific classes
IF (MAF_USE_VTK)
ENDIF (MAF_USE_VTK)  

# classes using VTK+wxWidgets
#IF (MAF_USE_WX AND MAF_USE_VTK)
#ENDIF (MAF_USE_WX AND MAF_USE_VTK)