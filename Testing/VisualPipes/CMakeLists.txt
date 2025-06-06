PROJECT (albaVisualPipesTests)

IF(ALBA_BUILD_ALBADLL)
  SET(PROJECT_LIBS albaDLL vtkALBA albaTests) 
ELSE(ALBA_BUILD_ALBADLL)
  SET(PROJECT_LIBS albaTests albaVME albaVisualPipes albaDataPipes GPUAPI vtkRendering vtkGraphics albaService)
ENDIF(ALBA_BUILD_ALBADLL)
 
 
#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#-----------------------------------------------------------------------------
SET(PROJECT_SRCS 
	albaVisualPipesTests.cpp albaVisualPipesTests.h
	albaPipeMeshTest.h albaPipeMeshTest.cpp
	albaPipeSurfaceTest.h albaPipeSurfaceTest.cpp
	albaPipeGizmoTest.h albaPipeGizmoTest.cpp
	albaPipeSurfaceTexturedTest.h albaPipeSurfaceTexturedTest.cpp
	albaPipeScalarTest.h albaPipeScalarTest.cpp
	albaPipeImage3DTest.h albaPipeImage3DTest.cpp
	albaPipeIsosurfaceTest.h albaPipeIsosurfaceTest.cpp
	albaPipePointSetTest.h albaPipePointSetTest.cpp
	albaPipeFactoryVMETest.h albaPipeFactoryVMETest.cpp
	albaPipeIsosurfaceGPUTest.h albaPipeIsosurfaceGPUTest.cpp
	albaPipePolylineTest.h albaPipePolylineTest.cpp
	albaPipeLandmarkCloudTest.h albaPipeLandmarkCloudTest.cpp
	albaPipeVectorTest.h albaPipeVectorTest.cpp
	albaPipeMeterTest.h albaPipeMeterTest.cpp
	albaPipeVolumeProjectedTest.h albaPipeVolumeProjectedTest.cpp
	albaPipeScalarMatrixTest.h albaPipeScalarMatrixTest.cpp
	albaVisualPipeVolumeRayCastingTest.h albaVisualPipeVolumeRayCastingTest.cpp
	albaPipeVolumeMIPTest.h albaPipeVolumeMIPTest.cpp
	albaPipeVolumeDRRTest.h albaPipeVolumeDRRTest.cpp
	albaPipePolylineGraphEditorTest.h albaPipePolylineGraphEditorTest.cpp
	albaPipeMeshSliceTest.h albaPipeMeshSliceTest.cpp
	albaPipeSurfaceSliceTest.h albaPipeSurfaceSliceTest.cpp
	albaPipeVolumeArbSliceTest.h albaPipeVolumeArbSliceTest.cpp
	albaPipeVolumeOrthoSliceTest.h albaPipeVolumeOrthoSliceTest.cpp
	albaPipeTrajectoriesTest.h albaPipeTrajectoriesTest.cpp
	albaPipeSliceTest.h albaPipeSliceTest.cpp
	albaPipePolylineSliceTest.h albaPipePolylineSliceTest.cpp
	albaPipeWrappedMeterTest.h albaPipeWrappedMeterTest.cpp
	albaPipeDensityDistanceTest.h albaPipeDensityDistanceTest.cpp
	albaPipeGraphTest.h albaPipeGraphTest.cpp
	albaPipeCompoundTest.h albaPipeCompoundTest.cpp
	albaPipeCompoundVolumeTest.h albaPipeCompoundVolumeTest.cpp
	albaPipeCompoundVolumefixedScalarsTest.h albaPipeCompoundVolumeFixedScalarsTest.cpp
	albaVisualPipePolylineGraphTest.h albaVisualPipePolylineGraphTest.cpp
	albaPipeSurfaceEditorTest.h albaPipeSurfaceEditorTest.cpp
	albaPipeTensorFieldSurfaceTest.h albaPipeTensorFieldSurfaceTest.cpp
	albaPipeVectorFieldSurfaceTest.h albaPipeVectorFieldSurfaceTest.cpp
	albaPipeVectorFieldGlyphsTest.h albaPipeVectorFieldGlyphsTest.cpp
	albaPipeVectorFieldMapWithArrowsTest.h albaPipeVectorFieldMapWithArrowsTest.cpp
	albaPipeTensorFieldSliceTest.h albaPipeTensorFieldSliceTest.cpp
	albaPipeVectorFieldSliceTest.h albaPipeVectorFieldSliceTest.cpp
	albaPipeTensorFieldGlyphsTest.h albaPipeTensorFieldGlyphsTest.cpp
	albaPipeTensorFieldTest.h albaPipeTensorFieldTest.cpp
	albaPipeVectorFieldTest.h albaPipeVectorFieldTest.cpp
	albaVisualPipeSlicerSliceTest.h albaVisualPipeSlicerSliceTest.cpp
	albaPipeCompoundVolumeVRTest.h albaPipeCompoundVolumeVRTest.cpp
	albaPipeCompoundVolumeMIPTest.h albaPipeCompoundVolumeMIPTest.cpp
	albaPipeCompoundVolumeDRRTest.h albaPipeCompoundVolumeDRRTest.cpp
	albaPipeCompoundVolumeIsosurfaceTest.h albaPipeCompoundVolumeIsosurfaceTest.cpp
	albaPipeRayCastTest.h albaPipeRayCastTest.cpp
	albaPipePointCloudTest.cpp albaPipePointCloudTest.h
)

ADD_EXECUTABLE(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
ADD_TEST(${PROJECT_NAME}  ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME})
IF (WIN32)
  SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)