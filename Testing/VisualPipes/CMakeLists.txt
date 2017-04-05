PROJECT (mafVME_TEST)

IF(MAF_BUILD_MAFDLL)
  LINK_LIBRARIES(mafDLL)
ELSE(MAF_BUILD_MAFDLL)
  LINK_LIBRARIES(mafTests mafVME mafVisualPipes mafDataPipes GPUAPI vtkRendering vtkGraphics mafService) # mafCommon mafOperations)
ENDIF (MAF_BUILD_MAFDLL)
 
#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#-----------------------------------------------------------------------------
SET(PROJECT_SRCS 
	mafVisualPipesTests.cpp mafVisualPipesTests.h
	mafPipeMeshTest.h mafPipeMeshTest.cpp
	mafPipeSurfaceTest.h mafPipeSurfaceTest.cpp
	mafPipeGizmoTest.h mafPipeGizmoTest.cpp
	mafPipeSurfaceTexturedTest.h mafPipeSurfaceTexturedTest.cpp
	mafPipeScalarTest.h mafPipeScalarTest.cpp
	mafPipeImage3DTest.h mafPipeImage3DTest.cpp
	mafPipeIsosurfaceTest.h mafPipeIsosurfaceTest.cpp
	mafPipePointSetTest.h mafPipePointSetTest.cpp
	mafPipeFactoryVMETest.h mafPipeFactoryVMETest.cpp
	mafPipeIsosurfaceGPUTest.h mafPipeIsosurfaceGPUTest.cpp
	mafPipePolylineTest.h mafPipePolylineTest.cpp
	mafPipeLandmarkCloudTest.h mafPipeLandmarkCloudTest.cpp
	mafPipeVectorTest.h mafPipeVectorTest.cpp
	mafPipeMeterTest.h mafPipeMeterTest.cpp
	mafPipeVolumeProjectedTest.h mafPipeVolumeProjectedTest.cpp
	mafPipeScalarMatrixTest.h mafPipeScalarMatrixTest.cpp
	mafVisualPipeVolumeRayCastingTest.h mafVisualPipeVolumeRayCastingTest.cpp
	mafPipeVolumeMIPTest.h mafPipeVolumeMIPTest.cpp
	mafPipeVolumeDRRTest.h mafPipeVolumeDRRTest.cpp
	mafPipePolylineGraphEditorTest.h mafPipePolylineGraphEditorTest.cpp
	mafPipeMeshSliceTest.h mafPipeMeshSliceTest.cpp
	mafPipeSurfaceSliceTest.h mafPipeSurfaceSliceTest.cpp
	#mafPipeVolumeArbSliceTest.h mafPipeVolumeArbSliceTest.cpp
	#mafPipeVolumeOrthoSliceTest.h mafPipeVolumeOrthoSliceTest.cpp
	mafPipeTrajectoriesTest.h mafPipeTrajectoriesTest.cpp
	mafPipeSliceTest.h mafPipeSliceTest.cpp
	mafPipePolylineSliceTest.h mafPipePolylineSliceTest.cpp
	mafPipeWrappedMeterTest.h mafPipeWrappedMeterTest.cpp
	mafPipeDensityDistanceTest.h mafPipeDensityDistanceTest.cpp
	mafPipeGraphTest.h mafPipeGraphTest.cpp
	mafPipeCompoundTest.h mafPipeCompoundTest.cpp
	mafPipeCompoundVolumeTest.h mafPipeCompoundVolumeTest.cpp
	mafPipeCompoundVolumefixedScalarsTest.h mafPipeCompoundVolumeFixedScalarsTest.cpp
	mafVisualPipePolylineGraphTest.h mafVisualPipePolylineGraphTest.cpp
	mafPipeSurfaceEditorTest.h mafPipeSurfaceEditorTest.cpp
	mafPipeTensorFieldSurfaceTest.h mafPipeTensorFieldSurfaceTest.cpp
	mafPipeVectorFieldSurfaceTest.h mafPipeVectorFieldSurfaceTest.cpp
	mafPipeVectorFieldGlyphsTest.h mafPipeVectorFieldGlyphsTest.cpp
	mafPipeVectorFieldMapWithArrowsTest.h mafPipeVectorFieldMapWithArrowsTest.cpp
	mafPipeTensorFieldSliceTest.h mafPipeTensorFieldSliceTest.cpp
	mafPipeVectorFieldSliceTest.h mafPipeVectorFieldSliceTest.cpp
	mafPipeTensorFieldGlyphsTest.h mafPipeTensorFieldGlyphsTest.cpp
	mafPipeTensorFieldTest.h mafPipeTensorFieldTest.cpp
	mafPipeVectorFieldTest.h mafPipeVectorFieldTest.cpp
	mafVisualPipeSlicerSliceTest.h mafVisualPipeSlicerSliceTest.cpp
	mafPipeCompoundVolumeVRTest.h mafPipeCompoundVolumeVRTest.cpp
	mafPipeCompoundVolumeMIPTest.h mafPipeCompoundVolumeMIPTest.cpp
	mafPipeCompoundVolumeDRRTest.h mafPipeCompoundVolumeDRRTest.cpp
	mafPipeCompoundVolumeIsosurfaceTest.h mafPipeCompoundVolumeIsosurfaceTest.cpp
	mafPipeRayCastTest.h mafPipeRayCastTest.cpp
)

ADD_EXECUTABLE(mafVisualPipesTests ${PROJECT_SRCS})
ADD_TEST(mafVisualPipesTests  ${EXECUTABLE_OUTPUT_PATH}/mafVisualPipesTests)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVisualPipesTests PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)