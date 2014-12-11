PROJECT (mafVME_TEST)

IF(MAF_BUILD_MAFDLL)
  LINK_LIBRARIES(mafDLL)
ELSE(MAF_BUILD_MAFDLL)
  LINK_LIBRARIES(mafVME mafDataPipes GPUAPI vtkRendering vtkGraphics) # mafCommon mafOperations)
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

############## from medical

IF (MAF_USE_ITK)
  ADD_EXECUTABLE(mafVMERawMotionDataTest mafVMERawMotionDataTest.h mafVMERawMotionDataTest.cpp)
  ADD_TEST(mafVMERawMotionDataTest  ${EXECUTABLE_OUTPUT_PATH}/mafVMERawMotionDataTest)
ENDIF (MAF_USE_ITK)

############ mafGizmoROI_BESTest
ADD_EXECUTABLE(mafGizmoROI_BESTest mafGizmoROI_BESTest.h mafGizmoROI_BESTest.cpp)
ADD_TEST(mafGizmoROI_BESTest ${EXECUTABLE_OUTPUT_PATH}/mafGizmoROI_BESTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafGizmoROI_BESTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

ADD_EXECUTABLE(mafPolylineGraphTest mafPolylineGraphTest.h mafPolylineGraphTest.cpp)
ADD_TEST(mafPolylineGraphTest  ${EXECUTABLE_OUTPUT_PATH}/mafPolylineGraphTest)

ADD_EXECUTABLE(mafVMEWrappedMeterTest mafVMEWrappedMeterTest.h mafVMEWrappedMeterTest.cpp)
ADD_TEST(mafVMEWrappedMeterTest  ${EXECUTABLE_OUTPUT_PATH}/mafVMEWrappedMeterTest)

ADD_EXECUTABLE(mafVMELabeledVolumeTest mafVMELabeledVolumeTest.h mafVMELabeledVolumeTest.cpp)
ADD_TEST(mafVMELabeledVolumeTest  ${EXECUTABLE_OUTPUT_PATH}/mafVMELabeledVolumeTest)

ADD_EXECUTABLE(mafPipeVolumeMIPTest mafPipeVolumeMIPTest.h mafPipeVolumeMIPTest.cpp)
ADD_TEST(mafPipeVolumeMIPTest  ${EXECUTABLE_OUTPUT_PATH}/mafPipeVolumeMIPTest)

ADD_EXECUTABLE(mafPipeVolumeDRRTest mafPipeVolumeDRRTest.h mafPipeVolumeDRRTest.cpp)
ADD_TEST(mafPipeVolumeDRRTest  ${EXECUTABLE_OUTPUT_PATH}/mafPipeVolumeDRRTest)

ADD_EXECUTABLE(mafVMEAnalogTest mafVMEAnalogTest.h mafVMEAnalogTest.cpp)
ADD_TEST(mafVMEAnalogTest  ${EXECUTABLE_OUTPUT_PATH}/mafVMEAnalogTest)

ADD_EXECUTABLE(mafVMEOutputSurfaceEditorTest mafVMEOutputSurfaceEditorTest.h mafVMEOutputSurfaceEditorTest.cpp)
ADD_TEST(mafVMEOutputSurfaceEditorTest  ${EXECUTABLE_OUTPUT_PATH}/mafVMEOutputSurfaceEditorTest)

ADD_EXECUTABLE(mafVMEOutputPolylineEditorTest mafVMEOutputPolylineEditorTest.h mafVMEOutputPolylineEditorTest.cpp)
ADD_TEST(mafVMEOutputPolylineEditorTest  ${EXECUTABLE_OUTPUT_PATH}/mafVMEOutputPolylineEditorTest)

ADD_EXECUTABLE(mafVMESurfaceEditorTest mafVMESurfaceEditorTest.h mafVMESurfaceEditorTest.cpp)
ADD_TEST(mafVMESurfaceEditorTest  ${EXECUTABLE_OUTPUT_PATH}/mafVMESurfaceEditorTest)

ADD_EXECUTABLE(mafPipePolylineGraphEditorTest mafPipePolylineGraphEditorTest.h mafPipePolylineGraphEditorTest.cpp)
ADD_TEST(mafPipePolylineGraphEditorTest  ${EXECUTABLE_OUTPUT_PATH}/mafPipePolylineGraphEditorTest)

ADD_EXECUTABLE(mafVMEPolylineEditorTest mafVMEPolylineEditorTest.h mafVMEPolylineEditorTest.cpp)
ADD_TEST(mafVMEPolylineEditorTest  ${EXECUTABLE_OUTPUT_PATH}/mafVMEPolylineEditorTest)

ADD_EXECUTABLE(mafPipeMeshSlice_BESTest mafPipeMeshSlice_BESTest.h mafPipeMeshSlice_BESTest.cpp)
ADD_TEST(mafPipeMeshSlice_BESTest  ${EXECUTABLE_OUTPUT_PATH}/mafPipeMeshSlice_BESTest)

ADD_EXECUTABLE(mafPipeSurfaceSlice_BESTest mafPipeSurfaceSlice_BESTest.h mafPipeSurfaceSlice_BESTest.cpp)
ADD_TEST(mafPipeSurfaceSlice_BESTest  ${EXECUTABLE_OUTPUT_PATH}/mafPipeSurfaceSlice_BESTest)

ADD_EXECUTABLE(mafPipeVolumeSlice_BESTest mafPipeVolumeSlice_BESTest.h mafPipeVolumeSlice_BESTest.cpp)
ADD_TEST(mafPipeVolumeSlice_BESTest  ${EXECUTABLE_OUTPUT_PATH}/mafPipeVolumeSlice_BESTest)

ADD_EXECUTABLE(mafVMEMapsTest mafVMEMapsTest.h mafVMEMapsTest.cpp)
ADD_TEST(mafVMEMapsTest  ${EXECUTABLE_OUTPUT_PATH}/mafVMEMapsTest)

ADD_EXECUTABLE(mafPipeTrajectoriesTest mafPipeTrajectoriesTest.h mafPipeTrajectoriesTest.cpp)
ADD_TEST(mafPipeTrajectoriesTest  ${EXECUTABLE_OUTPUT_PATH}/mafPipeTrajectoriesTest)

ADD_EXECUTABLE(mafPipeSliceTest mafPipeSliceTest.h mafPipeSliceTest.cpp)
ADD_TEST(mafPipeSliceTest  ${EXECUTABLE_OUTPUT_PATH}/mafPipeSliceTest)

ADD_EXECUTABLE(mafPipePolylineSlice_BESTest mafPipePolylineSlice_BESTest.h mafPipePolylineSlice_BESTest.cpp)
ADD_TEST(mafPipePolylineSlice_BESTest  ${EXECUTABLE_OUTPUT_PATH}/mafPipePolylineSlice_BESTest)

ADD_EXECUTABLE(mafPipeWrappedMeterTest mafPipeWrappedMeterTest.h mafPipeWrappedMeterTest.cpp)
ADD_TEST(mafPipeWrappedMeterTest  ${EXECUTABLE_OUTPUT_PATH}/mafPipeWrappedMeterTest)

ADD_EXECUTABLE(mafVMEOutputWrappedMeterTest mafVMEOutputWrappedMeterTest.h mafVMEOutputWrappedMeterTest.cpp)
ADD_TEST(mafVMEOutputWrappedMeterTest  ${EXECUTABLE_OUTPUT_PATH}/mafVMEOutputWrappedMeterTest)

ADD_EXECUTABLE(mafPipeDensityDistanceTest mafPipeDensityDistanceTest.h mafPipeDensityDistanceTest.cpp)
ADD_TEST(mafPipeDensityDistanceTest  ${EXECUTABLE_OUTPUT_PATH}/mafPipeDensityDistanceTest)

ADD_EXECUTABLE(mafVMEPolylineGraphTest mafVMEPolylineGraphTest.h mafVMEPolylineGraphTest.cpp)
ADD_TEST(mafVMEPolylineGraphTest  ${EXECUTABLE_OUTPUT_PATH}/mafVMEPolylineGraphTest)

ADD_EXECUTABLE(mafPipeGraphTest mafPipeGraphTest.h mafPipeGraphTest.cpp)
ADD_TEST(mafPipeGraphTest  ${EXECUTABLE_OUTPUT_PATH}/mafPipeGraphTest)

ADD_EXECUTABLE(mafVMESegmentationVolumeTest mafVMESegmentationVolumeTest.h mafVMESegmentationVolumeTest.cpp)
ADD_TEST(mafVMESegmentationVolumeTest  ${EXECUTABLE_OUTPUT_PATH}/mafVMESegmentationVolumeTest)

ADD_EXECUTABLE(mafPipeCompoundTest mafPipeCompoundTest.h mafPipeCompoundTest.cpp)
ADD_TEST(mafPipeCompoundTest  ${EXECUTABLE_OUTPUT_PATH}/mafPipeCompoundTest)

ADD_EXECUTABLE(mafPipeCompoundVolumeTest mafPipeCompoundVolumeTest.h mafPipeCompoundVolumeTest.cpp)
ADD_TEST(mafPipeCompoundVolumeTest  ${EXECUTABLE_OUTPUT_PATH}/mafPipeCompoundVolumeTest)

ADD_EXECUTABLE(mafPipeCompoundVolumeFixedScalarsTest mafPipeCompoundVolumefixedScalarsTest.h mafPipeCompoundVolumeFixedScalarsTest.cpp)
ADD_TEST(mafPipeCompoundVolumeFixedScalarsTest  ${EXECUTABLE_OUTPUT_PATH}/mafPipeCompoundVolumeFixedScalarsTest)

ADD_EXECUTABLE(mafVisualPipePolylineGraphTest mafVisualPipePolylineGraphTest.h mafVisualPipePolylineGraphTest.cpp)
ADD_TEST(mafVisualPipePolylineGraphTest  ${EXECUTABLE_OUTPUT_PATH}/mafVisualPipePolylineGraphTest)

ADD_EXECUTABLE(mafPipeSurfaceEditorTest mafPipeSurfaceEditorTest.h mafPipeSurfaceEditorTest.cpp)
ADD_TEST(mafPipeSurfaceEditorTest  ${EXECUTABLE_OUTPUT_PATH}/mafPipeSurfaceEditorTest)

ADD_EXECUTABLE(mafPipeTensorFieldSurfaceTest mafPipeTensorFieldSurfaceTest.h mafPipeTensorFieldSurfaceTest.cpp)
ADD_TEST(mafPipeTensorFieldSurfaceTest  ${EXECUTABLE_OUTPUT_PATH}/mafPipeTensorFieldSurfaceTest)

ADD_EXECUTABLE(mafPipeVectorFieldSurfaceTest mafPipeVectorFieldSurfaceTest.h mafPipeVectorFieldSurfaceTest.cpp)
ADD_TEST(mafPipeVectorFieldSurfaceTest  ${EXECUTABLE_OUTPUT_PATH}/mafPipeVectorFieldSurfaceTest)

ADD_EXECUTABLE(mafPipeVectorFieldGlyphsTest mafPipeVectorFieldGlyphsTest.h mafPipeVectorFieldGlyphsTest.cpp)
ADD_TEST(mafPipeVectorFieldGlyphsTest  ${EXECUTABLE_OUTPUT_PATH}/mafPipeVectorFieldGlyphsTest)

ADD_EXECUTABLE(mafPipeVectorFieldMapWithArrowsTest mafPipeVectorFieldMapWithArrowsTest.h mafPipeVectorFieldMapWithArrowsTest.cpp)
ADD_TEST(mafPipeVectorFieldMapWithArrowsTest  ${EXECUTABLE_OUTPUT_PATH}/mafPipeVectorFieldMapWithArrowsTest)

ADD_EXECUTABLE(mafAttributeSegmentationVolumeTest mafAttributeSegmentationVolumeTest.h mafAttributeSegmentationVolumeTest.cpp)
ADD_TEST(mafAttributeSegmentationVolumeTest  ${EXECUTABLE_OUTPUT_PATH}/mafAttributeSegmentationVolumeTest)

ADD_EXECUTABLE(mafPipeTensorFieldSliceTest mafPipeTensorFieldSliceTest.h mafPipeTensorFieldSliceTest.cpp)
ADD_TEST(mafPipeTensorFieldSliceTest  ${EXECUTABLE_OUTPUT_PATH}/mafPipeTensorFieldSliceTest)

ADD_EXECUTABLE(mafPipeVectorFieldSliceTest mafPipeVectorFieldSliceTest.h mafPipeVectorFieldSliceTest.cpp)
ADD_TEST(mafPipeVectorFieldSliceTest  ${EXECUTABLE_OUTPUT_PATH}/mafPipeVectorFieldSliceTest)

ADD_EXECUTABLE(mafPipeTensorFieldGlyphsTest mafPipeTensorFieldGlyphsTest.h mafPipeTensorFieldGlyphsTest.cpp)
ADD_TEST(mafPipeTensorFieldGlyphsTest  ${EXECUTABLE_OUTPUT_PATH}/mafPipeTensorFieldGlyphsTest)

ADD_EXECUTABLE(mafVMEOutputComputeWrappingTest mafVMEOutputComputeWrappingTest.h mafVMEOutputComputeWrappingTest.cpp)
ADD_TEST(mafVMEOutputComputeWrappingTest  ${EXECUTABLE_OUTPUT_PATH}/mafVMEOutputComputeWrappingTest)

ADD_EXECUTABLE(mafPipeTensorFieldTest mafPipeTensorFieldTest.h mafPipeTensorFieldTest.cpp)
ADD_TEST(mafPipeTensorFieldTest  ${EXECUTABLE_OUTPUT_PATH}/mafPipeTensorFieldTest)

ADD_EXECUTABLE(mafPipeVectorFieldTest mafPipeVectorFieldTest.h mafPipeVectorFieldTest.cpp)
ADD_TEST(mafPipeVectorFieldTest  ${EXECUTABLE_OUTPUT_PATH}/mafPipeVectorFieldTest)

ADD_EXECUTABLE(mafVisualPipeSlicerSliceTest  mafVisualPipeSlicerSliceTest.h mafVisualPipeSlicerSliceTest.cpp)
ADD_TEST(mafVisualPipeSlicerSliceTest ${EXECUTABLE_OUTPUT_PATH}/mafVisualPipeSlicerSliceTest)

ADD_EXECUTABLE(mafPipeCompoundVolumeVRTest  mafPipeCompoundVolumeVRTest.h mafPipeCompoundVolumeVRTest.cpp)
ADD_TEST(mafPipeCompoundVolumeVRTest ${EXECUTABLE_OUTPUT_PATH}/mafPipeCompoundVolumeVRTest)

ADD_EXECUTABLE(mafPipeCompoundVolumeMIPTest  mafPipeCompoundVolumeMIPTest.h mafPipeCompoundVolumeMIPTest.cpp)
ADD_TEST(mafPipeCompoundVolumeMIPTest ${EXECUTABLE_OUTPUT_PATH}/mafPipeCompoundVolumeMIPTest)

ADD_EXECUTABLE(mafPipeCompoundVolumeDRRTest  mafPipeCompoundVolumeDRRTest.h mafPipeCompoundVolumeDRRTest.cpp)
ADD_TEST(mafPipeCompoundVolumeDRRTest ${EXECUTABLE_OUTPUT_PATH}/mafPipeCompoundVolumeDRRTest)

ADD_EXECUTABLE(mafPipeCompoundVolumeIsosurfaceTest  mafPipeCompoundVolumeIsosurfaceTest.h mafPipeCompoundVolumeIsosurfaceTest.cpp)
ADD_TEST(mafPipeCompoundVolumeIsosurfaceTest ${EXECUTABLE_OUTPUT_PATH}/mafPipeCompoundVolumeIsosurfaceTest)

ADD_EXECUTABLE(mafVMEVolumeLargeUtilsTest  mafVMEVolumeLargeUtilsTest.h mafVMEVolumeLargeUtilsTest.cpp)
ADD_TEST(mafVMEVolumeLargeUtilsTest ${EXECUTABLE_OUTPUT_PATH}/mafVMEVolumeLargeUtilsTest)

ADD_EXECUTABLE(mafPolylineGraphVertexTest  mafPolylineGraphVertexTest.h mafPolylineGraphVertexTest.cpp)
ADD_TEST(mafPolylineGraphVertexTest ${EXECUTABLE_OUTPUT_PATH}/mafPolylineGraphVertexTest)

ADD_EXECUTABLE(mafPolylineGraphEdgeTest  mafPolylineGraphEdgeTest.h mafPolylineGraphEdgeTest.cpp)
ADD_TEST(mafPolylineGraphEdgeTest ${EXECUTABLE_OUTPUT_PATH}/mafPolylineGraphEdgeTest)

ADD_EXECUTABLE(mafPolylineGraphBranchTest  mafPolylineGraphBranchTest.h mafPolylineGraphBranchTest.cpp)
ADD_TEST(mafPolylineGraphBranchTest ${EXECUTABLE_OUTPUT_PATH}/mafPolylineGraphBranchTest)

ADD_EXECUTABLE(mafVMEVolumeLargeTest  mafVMEVolumeLargeTest.h mafVMEVolumeLargeTest.cpp)
ADD_TEST(mafVMEVolumeLargeTest ${EXECUTABLE_OUTPUT_PATH}/mafVMEVolumeLargeTest)

ADD_EXECUTABLE(mafDataPipeCustomSegmentationVolumeTest  mafDataPipeCustomSegmentationVolumeTest.h mafDataPipeCustomSegmentationVolumeTest.cpp)
ADD_TEST(mafDataPipeCustomSegmentationVolumeTest ${EXECUTABLE_OUTPUT_PATH}/mafDataPipeCustomSegmentationVolumeTest)

ADD_EXECUTABLE(mafPipeRayCastTest  mafPipeRayCastTest.h mafPipeRayCastTest.cpp)
ADD_TEST(mafPipeRayCastTest ${EXECUTABLE_OUTPUT_PATH}/mafPipeRayCastTest)
 

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
