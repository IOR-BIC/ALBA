PROJECT (MedicalOperations_TEST)

LINK_LIBRARIES(medVME)
LINK_LIBRARIES(medOperations)
LINK_LIBRARIES(medCommon)

#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#-----------------------------------------------------------------------------
IF (MAF_USE_ITK)
  ADD_EXECUTABLE(medOpExporterLandmarkTest medOpExporterLandmarkTest.h medOpExporterLandmarkTest.cpp)
  ADD_TEST(medOpExporterLandmarkTest ${EXECUTABLE_OUTPUT_PATH}/medOpExporterLandmarkTest)

  ADD_EXECUTABLE(medOpImporterLandmarkTest medOpImporterLandmarkTest.h medOpImporterLandmarkTest.cpp)
  ADD_TEST(medOpImporterLandmarkTest ${EXECUTABLE_OUTPUT_PATH}/medOpImporterLandmarkTest)
  
ENDIF (MAF_USE_ITK)

ADD_EXECUTABLE(medOpVolumeMeasureTest medOpVolumeMeasureTest.h medOpVolumeMeasureTest.cpp)
ADD_TEST(medOpVolumeMeasureTest ${EXECUTABLE_OUTPUT_PATH}/medOpVolumeMeasureTest)

ADD_EXECUTABLE(medOpImporterRAWImagesTest medOpImporterRAWImagesTest.h medOpImporterRAWImagesTest.cpp)
ADD_TEST(medOpImporterRAWImagesTest ${EXECUTABLE_OUTPUT_PATH}/medOpImporterRAWImagesTest)

ADD_EXECUTABLE(medOpImporterLandmarkTXTTest medOpImporterLandmarkTXTTest.h medOpImporterLandmarkTXTTest.cpp)
ADD_TEST(medOpImporterLandmarkTXTTest ${EXECUTABLE_OUTPUT_PATH}/medOpImporterLandmarkTXTTest)

ADD_EXECUTABLE(medOpImporterLandmarkWSTest medOpImporterLandmarkWSTest.h medOpImporterLandmarkWSTest.cpp)
ADD_TEST(medOpImporterLandmarkWSTest ${EXECUTABLE_OUTPUT_PATH}/medOpImporterLandmarkWSTest)
  
ADD_EXECUTABLE(medOpExtrusionHolesTest medOpExtrusionHolesTest.h medOpExtrusionHolesTest.cpp)
ADD_TEST(medOpExtrusionHolesTest ${EXECUTABLE_OUTPUT_PATH}/medOpExtrusionHolesTest)

ADD_EXECUTABLE(medOpExporterWrappedMeterTest medOpExporterWrappedMeterTest.h medOpExporterWrappedMeterTest.cpp)
ADD_TEST(medOpExporterWrappedMeterTest ${EXECUTABLE_OUTPUT_PATH}/medOpExporterWrappedMeterTest)

ADD_EXECUTABLE(medOpFreezeVMETest medOpFreezeVMETest.h medOpFreezeVMETest.cpp)
ADD_TEST(medOpFreezeVMETest ${EXECUTABLE_OUTPUT_PATH}/medOpFreezeVMETest)

ADD_EXECUTABLE(medOpLabelizeSurfaceTest medOpLabelizeSurfaceTest.h medOpLabelizeSurfaceTest.cpp)
ADD_TEST(medOpLabelizeSurfaceTest ${EXECUTABLE_OUTPUT_PATH}/medOpLabelizeSurfaceTest)

ADD_EXECUTABLE(medOpCleanSurfaceTest medOpCleanSurfaceTest.h medOpCleanSurfaceTest.cpp)
ADD_TEST(medOpCleanSurfaceTest ${EXECUTABLE_OUTPUT_PATH}/medOpCleanSurfaceTest)

ADD_EXECUTABLE(medOpSmoothSurfaceTest medOpSmoothSurfaceTest.h medOpSmoothSurfaceTest.cpp)
ADD_TEST(medOpSmoothSurfaceTest ${EXECUTABLE_OUTPUT_PATH}/medOpSmoothSurfaceTest)

ADD_EXECUTABLE(medOpTriangulateSurfaceTest medOpTriangulateSurfaceTest.h medOpTriangulateSurfaceTest.cpp)
ADD_TEST(medOpTriangulateSurfaceTest ${EXECUTABLE_OUTPUT_PATH}/medOpTriangulateSurfaceTest)

ADD_EXECUTABLE(medOpSurfaceMirrorTest medOpSurfaceMirrorTest.h medOpSurfaceMirrorTest.cpp)
ADD_TEST(medOpSurfaceMirrorTest ${EXECUTABLE_OUTPUT_PATH}/medOpSurfaceMirrorTest)

ADD_EXECUTABLE(medOpImporterVTKTest medOpImporterVTKTest.h medOpImporterVTKTest.cpp)
ADD_TEST(medOpImporterVTKTest ${EXECUTABLE_OUTPUT_PATH}/medOpImporterVTKTest)

ADD_EXECUTABLE(medOpExporterMetersTest medOpExporterMetersTest.h medOpExporterMetersTest.cpp)
ADD_TEST(medOpExporterMetersTest ${EXECUTABLE_OUTPUT_PATH}/medOpExporterMetersTest)


# wxWidgets specific classes
#IF (MAF_USE_WX)
#ENDIF (MAF_USE_WX)

# VTK specific classes
IF (MAF_USE_VTK)
ENDIF (MAF_USE_VTK)  

# classes using VTK+wxWidgets
#IF (MAF_USE_WX AND MAF_USE_VTK)
#ENDIF (MAF_USE_WX AND MAF_USE_VTK)
