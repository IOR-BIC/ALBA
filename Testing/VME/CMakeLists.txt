PROJECT (mafVME_TEST)

IF(MAF_BUILD_MAFDLL)
  LINK_LIBRARIES(mafDLL)
ELSE(MAF_BUILD_MAFDLL)
  LINK_LIBRARIES(mafVME mafDataPipes GPUAPI vtkRendering vtkGraphics)
ENDIF (MAF_BUILD_MAFDLL)
 
#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#-----------------------------------------------------------------------------

############ mafVMEExternalDataTest
ADD_EXECUTABLE(mafVMEExternalDataTest mafVMEExternalDataTest.h mafVMEExternalDataTest.cpp)
ADD_TEST(mafVMEExternalDataTest  ${EXECUTABLE_OUTPUT_PATH}/mafVMEExternalDataTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVMEExternalDataTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMEVolumeGrayTest
ADD_EXECUTABLE(mafVMEVolumeGrayTest mafVMEVolumeGrayTest.h mafVMEVolumeGrayTest.cpp)
ADD_TEST(mafVMEVolumeGrayTest ${EXECUTABLE_OUTPUT_PATH}/mafVMEVolumeGrayTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVMEVolumeGrayTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMESurfaceParametricTest
ADD_EXECUTABLE(mafVMESurfaceParametricTest mafVMESurfaceParametricTest.h mafVMESurfaceParametricTest.cpp)
ADD_TEST(mafVMESurfaceParametricTest ${EXECUTABLE_OUTPUT_PATH}/mafVMESurfaceParametricTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVMESurfaceParametricTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMESurfaceTest
ADD_EXECUTABLE(mafVMESurfaceTest mafVMESurfaceTest.h mafVMESurfaceTest.cpp)
ADD_TEST(mafVMESurfaceTest ${EXECUTABLE_OUTPUT_PATH}/mafVMESurfaceTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVMESurfaceTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMEGroupTest
ADD_EXECUTABLE(mafVMEGroupTest mafVMEGroupTest.h mafVMEGroupTest.cpp)
ADD_TEST(mafVMEGroupTest ${EXECUTABLE_OUTPUT_PATH}/mafVMEGroupTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVMEGroupTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMEGizmoTest
ADD_EXECUTABLE(mafVMEGizmoTest mafVMEGizmoTest.h mafVMEGizmoTest.cpp)
ADD_TEST(mafVMEGizmoTest  ${EXECUTABLE_OUTPUT_PATH}/mafVMEGizmoTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVMEGizmoTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

IF (MAF_USE_ITK)
	############ mafVMEScalarMatrixTest
	ADD_EXECUTABLE(mafVMEScalarMatrixTest mafVMEScalarMatrixTest.h mafVMEScalarMatrixTest.cpp)
	ADD_TEST(mafVMEScalarMatrixTest  ${EXECUTABLE_OUTPUT_PATH}/mafVMEScalarMatrixTest)
	IF (WIN32)
	  SET_TARGET_PROPERTIES(mafVMEScalarMatrixTest PROPERTIES LINK_FLAGS "/MANIFEST")
	ENDIF (WIN32)
ENDIF (MAF_USE_ITK)

############ mafVMEScalarTest
ADD_EXECUTABLE(mafVMEScalarTest mafVMEScalarTest.h mafVMEScalarTest.cpp)
ADD_TEST(mafVMEScalarTest  ${EXECUTABLE_OUTPUT_PATH}/mafVMEScalarTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVMEScalarTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMEPolylineTest
ADD_EXECUTABLE(mafVMEPolylineTest mafVMEPolylineTest.h mafVMEPolylineTest.cpp)
ADD_TEST(mafVMEPolylineTest  ${EXECUTABLE_OUTPUT_PATH}/mafVMEPolylineTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVMEPolylineTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMEPolylineSplineTest
ADD_EXECUTABLE(mafVMEPolylineSplineTest mafVMEPolylineSplineTest.h mafVMEPolylineSplineTest.cpp)
ADD_TEST(mafVMEPolylineSplineTest  ${EXECUTABLE_OUTPUT_PATH}/mafVMEPolylineSplineTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVMEPolylineSplineTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMEMeshTest
ADD_EXECUTABLE(mafVMEMeshTest mafVMEMeshTest.h mafVMEMeshTest.cpp)
ADD_TEST(mafVMEMeshTest  ${EXECUTABLE_OUTPUT_PATH}/mafVMEMeshTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVMEMeshTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafPipeMeshTest
ADD_EXECUTABLE(mafPipeMeshTest mafPipeMeshTest.h mafPipeMeshTest.cpp)
ADD_TEST(mafPipeMeshTest  ${EXECUTABLE_OUTPUT_PATH}/mafPipeMeshTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafPipeMeshTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafPipeMeshSliceTest
ADD_EXECUTABLE(mafPipeMeshSliceTest mafPipeMeshSliceTest.h mafPipeMeshSliceTest.cpp)
ADD_TEST(mafPipeMeshSliceTest  ${EXECUTABLE_OUTPUT_PATH}/mafPipeMeshSliceTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafPipeMeshSliceTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMELandmarkTest
ADD_EXECUTABLE(mafVMELandmarkTest mafVMELandmarkTest.h mafVMELandmarkTest.cpp)
ADD_TEST(mafVMELandmarkTest  ${EXECUTABLE_OUTPUT_PATH}/mafVMELandmarkTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVMELandmarkTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMELandmarkCloudTest
ADD_EXECUTABLE(mafVMELandmarkCloudTest mafVMELandmarkCloudTest.h mafVMELandmarkCloudTest.cpp)
ADD_TEST(mafVMELandmarkCloudTest  ${EXECUTABLE_OUTPUT_PATH}/mafVMELandmarkCloudTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVMELandmarkCloudTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)


############ mafMatrixVectorTest
ADD_EXECUTABLE(mafMatrixVectorTest mafMatrixVectorTest.h mafMatrixVectorTest.cpp)
ADD_TEST(mafMatrixVectorTest  ${EXECUTABLE_OUTPUT_PATH}/mafMatrixVectorTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafMatrixVectorTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)


############ mafVMEFactoryTest
ADD_EXECUTABLE(mafVMEFactoryTest mafVMEFactoryTest.h mafVMEFactoryTest.cpp)
ADD_TEST(mafVMEFactoryTest ${EXECUTABLE_OUTPUT_PATH}/mafVMEFactoryTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVMEFactoryTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafDataVectorTest
ADD_EXECUTABLE(mafDataVectorTest mafDataVectorTest.h mafDataVectorTest.cpp)
ADD_TEST(mafDataVectorTest ${EXECUTABLE_OUTPUT_PATH}/mafDataVectorTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafDataVectorTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMEItemVTKTest
ADD_EXECUTABLE(mafVMEItemVTKTest mafVMEItemVTKTest.h mafVMEItemVTKTest.cpp)
ADD_TEST(mafVMEItemVTKTest ${EXECUTABLE_OUTPUT_PATH}/mafVMEItemVTKTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVMEItemVTKTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafPipeSurfaceTest
ADD_EXECUTABLE(mafPipeSurfaceTest mafPipeSurfaceTest.h mafPipeSurfaceTest.cpp)
ADD_TEST(mafPipeSurfaceTest ${EXECUTABLE_OUTPUT_PATH}/mafPipeSurfaceTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafPipeSurfaceTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafPipeGizmoTest
ADD_EXECUTABLE(mafPipeGizmoTest mafPipeGizmoTest.h mafPipeGizmoTest.cpp)
ADD_TEST(mafPipeGizmoTest ${EXECUTABLE_OUTPUT_PATH}/mafPipeGizmoTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafPipeGizmoTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafPipeSurfaceTexturedTest
ADD_EXECUTABLE(mafPipeSurfaceTexturedTest mafPipeSurfaceTexturedTest.h mafPipeSurfaceTexturedTest.cpp)
ADD_TEST(mafPipeSurfaceTexturedTest ${EXECUTABLE_OUTPUT_PATH}/mafPipeSurfaceTexturedTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafPipeSurfaceTexturedTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafPipeScalarTest
ADD_EXECUTABLE(mafPipeScalarTest mafPipeScalarTest.h mafPipeScalarTest.cpp)
ADD_TEST(mafPipeScalarTest ${EXECUTABLE_OUTPUT_PATH}/mafPipeScalarTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafPipeScalarTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafPipeVolumeSliceTest
ADD_EXECUTABLE(mafPipeVolumeSliceTest mafPipeVolumeSliceTest.h mafPipeVolumeSliceTest.cpp)
ADD_TEST(mafPipeVolumeSliceTest ${EXECUTABLE_OUTPUT_PATH}/mafPipeVolumeSliceTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafPipeVolumeSliceTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafPipeImage3DTest
ADD_EXECUTABLE(mafPipeImage3DTest mafPipeImage3DTest.h mafPipeImage3DTest.cpp)
ADD_TEST(mafPipeImage3DTest ${EXECUTABLE_OUTPUT_PATH}/mafPipeImage3DTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafPipeImage3DTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafPipeIsosurfaceTest
ADD_EXECUTABLE(mafPipeIsosurfaceTest mafPipeIsosurfaceTest.h mafPipeIsosurfaceTest.cpp)
ADD_TEST(mafPipeIsosurfaceTest ${EXECUTABLE_OUTPUT_PATH}/mafPipeIsosurfaceTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafPipeIsosurfaceTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafPipePointSetTest
ADD_EXECUTABLE(mafPipePointSetTest mafPipePointSetTest.h mafPipePointSetTest.cpp)
ADD_TEST(mafPipePointSetTest  ${EXECUTABLE_OUTPUT_PATH}/mafPipePointSetTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafPipePointSetTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMEImageTest
ADD_EXECUTABLE(mafVMEImageTest mafVMEImageTest.h mafVMEImageTest.cpp)
ADD_TEST(mafVMEImageTest  ${EXECUTABLE_OUTPUT_PATH}/mafVMEImageTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVMEImageTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMEVolumeRGBTest
ADD_EXECUTABLE(mafVMEVolumeRGBTest mafVMEVolumeRGBTest.h mafVMEVolumeRGBTest.cpp)
ADD_TEST(mafVMEVolumeRGBTest  ${EXECUTABLE_OUTPUT_PATH}/mafVMEVolumeRGBTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVMEVolumeRGBTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMEInfoTextTest
ADD_EXECUTABLE(mafVMEInfoTextTest mafVMEInfoTextTest.h mafVMEInfoTextTest.cpp)
ADD_TEST(mafVMEInfoTextTest  ${EXECUTABLE_OUTPUT_PATH}/mafVMEInfoTextTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVMEInfoTextTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mmaMeterTest
ADD_EXECUTABLE(mmaMeterTest mmaMeterTest.h mmaMeterTest.cpp)
ADD_TEST(mmaMeterTest  ${EXECUTABLE_OUTPUT_PATH}/mmaMeterTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mmaMeterTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)


############ mafVMEOutputMeterTest
ADD_EXECUTABLE(mafVMEOutputMeterTest mafVMEOutputMeterTest.h mafVMEOutputMeterTest.cpp)
ADD_TEST(mafVMEOutputMeterTest  ${EXECUTABLE_OUTPUT_PATH}/mafVMEOutputMeterTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVMEOutputMeterTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMEPointSetTest
ADD_EXECUTABLE(mafVMEPointSetTest mafVMEPointSetTest.h mafVMEPointSetTest.cpp)
ADD_TEST(mafVMEPointSetTest  ${EXECUTABLE_OUTPUT_PATH}/mafVMEPointSetTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVMEPointSetTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMEVectorTest
ADD_EXECUTABLE(mafVMEVectorTest mafVMEVectorTest.h mafVMEVectorTest.cpp)
ADD_TEST(mafVMEVectorTest  ${EXECUTABLE_OUTPUT_PATH}/mafVMEVectorTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVMEVectorTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafPipeFactoryVMETest
ADD_EXECUTABLE(mafPipeFactoryVMETest mafPipeFactoryVMETest.h mafPipeFactoryVMETest.cpp)
ADD_TEST(mafPipeFactoryVMETest  ${EXECUTABLE_OUTPUT_PATH}/mafPipeFactoryVMETest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafPipeFactoryVMETest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMEOutputPolylineTest
ADD_EXECUTABLE(mafVMEOutputPolylineTest mafVMEOutputPolylineTest.h mafVMEOutputPolylineTest.cpp)
ADD_TEST(mafVMEOutputPolylineTest  ${EXECUTABLE_OUTPUT_PATH}/mafVMEOutputPolylineTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVMEOutputPolylineTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMEOutputSurfaceTest
ADD_EXECUTABLE(mafVMEOutputSurfaceTest mafVMEOutputSurfaceTest.h mafVMEOutputSurfaceTest.cpp)
ADD_TEST(mafVMEOutputSurfaceTest  ${EXECUTABLE_OUTPUT_PATH}/mafVMEOutputSurfaceTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVMEOutputSurfaceTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMEOutputPointSetTest
ADD_EXECUTABLE(mafVMEOutputPointSetTest mafVMEOutputPointSetTest.h mafVMEOutputPointSetTest.cpp)
ADD_TEST(mafVMEOutputPointSetTest  ${EXECUTABLE_OUTPUT_PATH}/mafVMEOutputPointSetTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVMEOutputPointSetTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMEOutputScalarTest
ADD_EXECUTABLE(mafVMEOutputScalarTest mafVMEOutputScalarTest.h mafVMEOutputScalarTest.cpp)
ADD_TEST(mafVMEOutputScalarTest  ${EXECUTABLE_OUTPUT_PATH}/mafVMEOutputScalarTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVMEOutputScalarTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMEOutputLandmarkCloudTest
ADD_EXECUTABLE(mafVMEOutputLandmarkCloudTest mafVMEOutputLandmarkCloudTest.h mafVMEOutputLandmarkCloudTest.cpp)
ADD_TEST(mafVMEOutputLandmarkCloudTest  ${EXECUTABLE_OUTPUT_PATH}/mafVMEOutputLandmarkCloudTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVMEOutputLandmarkCloudTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMEOutputScalarMatrixTest
ADD_EXECUTABLE(mafVMEOutputScalarMatrixTest mafVMEOutputScalarMatrixTest.h mafVMEOutputScalarMatrixTest.cpp)
ADD_TEST(mafVMEOutputScalarMatrixTest ${EXECUTABLE_OUTPUT_PATH}/mafVMEOutputScalarMatrixTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(mafVMEOutputScalarMatrixTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMEOutputMeshTest
ADD_EXECUTABLE(mafVMEOutputMeshTest mafVMEOutputMeshTest.h mafVMEOutputMeshTest.cpp)
ADD_TEST(mafVMEOutputMeshTest ${EXECUTABLE_OUTPUT_PATH}/mafVMEOutputMeshTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(mafVMEOutputMeshTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafPipeIsosurfaceGPUTest
ADD_EXECUTABLE(mafPipeIsosurfaceGPUTest mafPipeIsosurfaceGPUTest.h mafPipeIsosurfaceGPUTest.cpp)
ADD_TEST(mafPipeIsosurfaceGPUTest ${EXECUTABLE_OUTPUT_PATH}/mafPipeIsosurfaceGPUTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(mafPipeIsosurfaceGPUTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMEOutputImageTest
ADD_EXECUTABLE(mafVMEOutputImageTest mafVMEOutputImageTest.h mafVMEOutputImageTest.cpp)
ADD_TEST(mafVMEOutputImageTest ${EXECUTABLE_OUTPUT_PATH}/mafVMEOutputImageTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(mafVMEOutputImageTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMEOutputVolumeTest
ADD_EXECUTABLE(mafVMEOutputVolumeTest mafVMEOutputVolumeTest.h mafVMEOutputVolumeTest.cpp)
ADD_TEST(mafVMEOutputVolumeTest ${EXECUTABLE_OUTPUT_PATH}/mafVMEOutputVolumeTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVMEOutputVolumeTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafCryptTest
ADD_EXECUTABLE(mafCryptTest mafCryptTest.h mafCryptTest.cpp)
ADD_TEST(mafCryptTest  ${EXECUTABLE_OUTPUT_PATH}/mafCryptTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafCryptTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafPipePolylineTest
ADD_EXECUTABLE(mafPipePolylineTest mafPipePolylineTest.h mafPipePolylineTest.cpp)
ADD_TEST(mafPipePolylineTest ${EXECUTABLE_OUTPUT_PATH}/mafPipePolylineTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(mafPipePolylineTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafMatrixInterpolatorTest
ADD_EXECUTABLE(mafMatrixInterpolatorTest mafMatrixInterpolatorTest.h mafMatrixInterpolatorTest.cpp)
ADD_TEST(mafMatrixInterpolatorTest ${EXECUTABLE_OUTPUT_PATH}/mafMatrixInterpolatorTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(mafMatrixInterpolatorTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafPipeSurfaceSliceTest
ADD_EXECUTABLE(mafPipeSurfaceSliceTest mafPipeSurfaceSliceTest.h mafPipeSurfaceSliceTest.cpp)
ADD_TEST(mafPipeSurfaceSliceTest ${EXECUTABLE_OUTPUT_PATH}/mafPipeSurfaceSliceTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(mafPipeSurfaceSliceTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMEGenericTest
ADD_EXECUTABLE(mafVMEGenericTest mafVMEGenericTest.h mafVMEGenericTest.cpp)
ADD_TEST(mafVMEGenericTest ${EXECUTABLE_OUTPUT_PATH}/mafVMEGenericTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(mafVMEGenericTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafPipeLandmarkCloudTest
ADD_EXECUTABLE(mafPipeLandmarkCloudTest mafPipeLandmarkCloudTest.h mafPipeLandmarkCloudTest.cpp)
ADD_TEST(mafPipeLandmarkCloudTest ${EXECUTABLE_OUTPUT_PATH}/mafPipeLandmarkCloudTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(mafPipeLandmarkCloudTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMEOutputVTKTest
ADD_EXECUTABLE(mafVMEOutputVTKTest mafVMEOutputVTKTest.h mafVMEOutputVTKTest.cpp )
ADD_TEST(mafVMEOutputVTKTest ${EXECUTABLE_OUTPUT_PATH}/mafVMEOutputVTKTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(mafVMEOutputVTKTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMEItemScalarMatrixTest
ADD_EXECUTABLE(mafVMEItemScalarMatrixTest mafVMEItemScalarMatrixTest.h mafVMEItemScalarMatrixTest.cpp )
ADD_TEST(mafVMEItemScalarMatrixTest ${EXECUTABLE_OUTPUT_PATH}/mafVMEItemScalarMatrixTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(mafVMEItemScalarMatrixTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafScalarVectorTest
ADD_EXECUTABLE(mafScalarVectorTest mafScalarVectorTest.h mafScalarVectorTest.cpp )
ADD_TEST(mafScalarVectorTest ${EXECUTABLE_OUTPUT_PATH}/mafScalarVectorTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(mafScalarVectorTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafPipeVectorTest
ADD_EXECUTABLE(mafPipeVectorTest mafPipeVectorTest.h mafPipeVectorTest.cpp )
ADD_TEST(mafPipeVectorTest ${EXECUTABLE_OUTPUT_PATH}/mafPipeVectorTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(mafPipeVectorTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMEGenericAbstractTest
ADD_EXECUTABLE(mafVMEGenericAbstractTest mafVMEGenericAbstractTest.h mafVMEGenericAbstractTest.cpp )
ADD_TEST(mafVMEGenericAbstractTest ${EXECUTABLE_OUTPUT_PATH}/mafVMEGenericAbstractTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(mafVMEGenericAbstractTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafPipeMeterTest
ADD_EXECUTABLE(mafPipeMeterTest mafPipeMeterTest.h mafPipeMeterTest.cpp )
ADD_TEST(mafPipeMeterTest ${EXECUTABLE_OUTPUT_PATH}/mafPipeMeterTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(mafPipeMeterTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafPipeVolumeProjectedTest
ADD_EXECUTABLE(mafPipeVolumeProjectedTest mafPipeVolumeProjectedTest.h mafPipeVolumeProjectedTest.cpp )
ADD_TEST(mafPipeVolumeProjectedTest ${EXECUTABLE_OUTPUT_PATH}/mafPipeVolumeProjectedTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(mafPipeVolumeProjectedTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMEItemTest
ADD_EXECUTABLE(mafVMEItemTest mafVMEItemTest.h mafVMEItemTest.cpp )
ADD_TEST(mafVMEItemTest ${EXECUTABLE_OUTPUT_PATH}/mafVMEItemTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(mafVMEItemTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafPipePolylineSliceTest
ADD_EXECUTABLE(mafPipePolylineSliceTest mafPipePolylineSliceTest.h mafPipePolylineSliceTest.cpp )
ADD_TEST(mafPipePolylineSliceTest ${EXECUTABLE_OUTPUT_PATH}/mafPipePolylineSliceTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(mafPipePolylineSliceTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMERefSysTest
ADD_EXECUTABLE(mafVMERefSysTest mafVMERefSysTest.h mafVMERefSysTest.cpp )
ADD_TEST(mafVMERefSysTest ${EXECUTABLE_OUTPUT_PATH}/mafVMERefSysTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(mafVMERefSysTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMEProberTest
ADD_EXECUTABLE(mafVMEProberTest mafVMEProberTest.h mafVMEProberTest.cpp )
ADD_TEST(mafVMEProberTest ${EXECUTABLE_OUTPUT_PATH}/mafVMEProberTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(mafVMEProberTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMEMeterTest
ADD_EXECUTABLE(mafVMEMeterTest mafVMEMeterTest.h mafVMEMeterTest.cpp )
ADD_TEST(mafVMEMeterTest ${EXECUTABLE_OUTPUT_PATH}/mafVMEMeterTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(mafVMEMeterTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafMSFImporterTest
ADD_EXECUTABLE(mafMSFImporterTest mafMSFImporterTest.h mafMSFImporterTest.cpp )
ADD_TEST(mafMSFImporterTest ${EXECUTABLE_OUTPUT_PATH}/mafMSFImporterTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(mafMSFImporterTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafPipeScalarMatrixTest
ADD_EXECUTABLE(mafPipeScalarMatrixTest mafPipeScalarMatrixTest.h mafPipeScalarMatrixTest.cpp )
ADD_TEST(mafPipeScalarMatrixTest ${EXECUTABLE_OUTPUT_PATH}/mafPipeScalarMatrixTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(mafPipeScalarMatrixTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVisualPipeVolumeRayCastingTest
ADD_EXECUTABLE(mafVisualPipeVolumeRayCastingTest mafVisualPipeVolumeRayCastingTest.h mafVisualPipeVolumeRayCastingTest.cpp )
ADD_TEST(mafVisualPipeVolumeRayCastingTest ${EXECUTABLE_OUTPUT_PATH}/mafVisualPipeVolumeRayCastingTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(mafVisualPipeVolumeRayCastingTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

ADD_EXECUTABLE(mafVMESlicerTest mafVMESlicerTest.h mafVMESlicerTest.cpp)
ADD_TEST(mafVMESlicerTest ${EXECUTABLE_OUTPUT_PATH}/mafVMESlicerTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVMESlicerTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)
 

#-----------------------------------------------------------------------------
# commented tests: to be renamed to match test names standard
# and to be refactored to use cppunit.  Also to be checked if they use any external data.
#-----------------------------------------------------------------------------
# 
# 
# ADD_EXECUTABLE(testDataVector testDataVector.cpp)
# ADD_TEST(testDataVector ${EXECUTABLE_OUTPUT_PATH}/testDataVector)
# 
# ADD_EXECUTABLE(testMatrixVector testMatrixVector.cpp)
# ADD_TEST(testMatrixVector ${EXECUTABLE_OUTPUT_PATH}/testMatrixVector)
# 
# ADD_EXECUTABLE(testVMEGenericPose testVMEGenericPose.cpp)
# ADD_TEST(testVMEGenericPose ${EXECUTABLE_OUTPUT_PATH}/testVMEGenericPose)
# 
# ADD_EXECUTABLE(testVMEGenericBounds testVMEGenericBounds.cpp)
# ADD_TEST(testVMEGenericBounds ${EXECUTABLE_OUTPUT_PATH}/testVMEGenericBounds)
# 
# ADD_EXECUTABLE(testVMESlicer testVMESlicer.cpp)
# ADD_TEST(testVMESlicer ${EXECUTABLE_OUTPUT_PATH}/testVMESlicer)
# 
# ADD_EXECUTABLE(testVMELandmarkCloud testVMELandmarkCloud.cpp)
# ADD_TEST(testVMELandmarkCloud ${EXECUTABLE_OUTPUT_PATH}/testVMELandmarkCloud)
# 
# ADD_EXECUTABLE(testCustomDataPipe testCustomDataPipe.cpp)
# ADD_TEST(testCustomDataPipe ${EXECUTABLE_OUTPUT_PATH}/testCustomDataPipe)
# 
# wxWidgets specific classes
# IF (MAF_USE_WX)
# ENDIF (MAF_USE_WX)
# 
# # VTK specific classes
IF (MAF_USE_VTK)
	############ testMSF
  # ADD_EXECUTABLE(testMSF testMSF.cpp)
  # ADD_TEST(testMSF ${EXECUTABLE_OUTPUT_PATH}/testMSF)

	############ testVMEGeneric
	ADD_EXECUTABLE(testVMEGeneric testVMEGeneric.cpp)
	ADD_TEST(testVMEGeneric ${EXECUTABLE_OUTPUT_PATH}/testVMEGeneric)
	IF (WIN32)
	  SET_TARGET_PROPERTIES(testVMEGeneric PROPERTIES LINK_FLAGS "/MANIFEST")
	ENDIF (WIN32)
ENDIF (MAF_USE_VTK)  
# 
# classes using VTK+wxWidgets
# IF (MAF_USE_WX AND MAF_USE_VTK)
# ENDIF (MAF_USE_WX AND MAF_USE_VTK)
