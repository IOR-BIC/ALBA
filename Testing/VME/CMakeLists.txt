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

ADD_EXECUTABLE(medPipeVolumeDRRTest medPipeVolumeDRRTest.h medPipeVolumeDRRTest.cpp)
ADD_TEST(medPipeVolumeDRRTest  ${EXECUTABLE_OUTPUT_PATH}/medPipeVolumeDRRTest)

ADD_EXECUTABLE(medVMEAnalogTest medVMEAnalogTest.h medVMEAnalogTest.cpp)
ADD_TEST(medVMEAnalogTest  ${EXECUTABLE_OUTPUT_PATH}/medVMEAnalogTest)

ADD_EXECUTABLE(medVMEFactoryTest medVMEFactoryTest.h medVMEFactoryTest.cpp)
ADD_TEST(medVMEFactoryTest  ${EXECUTABLE_OUTPUT_PATH}/medVMEFactoryTest)

ADD_EXECUTABLE(medVMEOutputSurfaceEditorTest medVMEOutputSurfaceEditorTest.h medVMEOutputSurfaceEditorTest.cpp)
ADD_TEST(medVMEOutputSurfaceEditorTest  ${EXECUTABLE_OUTPUT_PATH}/medVMEOutputSurfaceEditorTest)

ADD_EXECUTABLE(medVMEOutputPolylineEditorTest medVMEOutputPolylineEditorTest.h medVMEOutputPolylineEditorTest.cpp)
ADD_TEST(medVMEOutputPolylineEditorTest  ${EXECUTABLE_OUTPUT_PATH}/medVMEOutputPolylineEditorTest)

ADD_EXECUTABLE(medVMESurfaceEditorTest medVMESurfaceEditorTest.h medVMESurfaceEditorTest.cpp)
ADD_TEST(medVMESurfaceEditorTest  ${EXECUTABLE_OUTPUT_PATH}/medVMESurfaceEditorTest)

ADD_EXECUTABLE(medPipePolylineGraphEditorTest medPipePolylineGraphEditorTest.h medPipePolylineGraphEditorTest.cpp)
ADD_TEST(medPipePolylineGraphEditorTest  ${EXECUTABLE_OUTPUT_PATH}/medPipePolylineGraphEditorTest)

ADD_EXECUTABLE(medVMEPolylineEditorTest medVMEPolylineEditorTest.h medVMEPolylineEditorTest.cpp)
ADD_TEST(medVMEPolylineEditorTest  ${EXECUTABLE_OUTPUT_PATH}/medVMEPolylineEditorTest)

ADD_EXECUTABLE(mafPipeMeshSlice_BESTest mafPipeMeshSlice_BESTest.h mafPipeMeshSlice_BESTest.cpp)
ADD_TEST(mafPipeMeshSlice_BESTest  ${EXECUTABLE_OUTPUT_PATH}/mafPipeMeshSlice_BESTest)

ADD_EXECUTABLE(mafPipeSurfaceSlice_BESTest mafPipeSurfaceSlice_BESTest.h mafPipeSurfaceSlice_BESTest.cpp)
ADD_TEST(mafPipeSurfaceSlice_BESTest  ${EXECUTABLE_OUTPUT_PATH}/mafPipeSurfaceSlice_BESTest)

ADD_EXECUTABLE(mafPipeVolumeSlice_BESTest mafPipeVolumeSlice_BESTest.h mafPipeVolumeSlice_BESTest.cpp)
ADD_TEST(mafPipeVolumeSlice_BESTest  ${EXECUTABLE_OUTPUT_PATH}/mafPipeVolumeSlice_BESTest)

ADD_EXECUTABLE(medVMEMapsTest medVMEMapsTest.h medVMEMapsTest.cpp)
ADD_TEST(medVMEMapsTest  ${EXECUTABLE_OUTPUT_PATH}/medVMEMapsTest)

ADD_EXECUTABLE(medPipeTrajectoriesTest medPipeTrajectoriesTest.h medPipeTrajectoriesTest.cpp)
ADD_TEST(medPipeTrajectoriesTest  ${EXECUTABLE_OUTPUT_PATH}/medPipeTrajectoriesTest)

ADD_EXECUTABLE(mafPipeSliceTest mafPipeSliceTest.h mafPipeSliceTest.cpp)
ADD_TEST(mafPipeSliceTest  ${EXECUTABLE_OUTPUT_PATH}/mafPipeSliceTest)

ADD_EXECUTABLE(mafPipePolylineSlice_BESTest mafPipePolylineSlice_BESTest.h mafPipePolylineSlice_BESTest.cpp)
ADD_TEST(mafPipePolylineSlice_BESTest  ${EXECUTABLE_OUTPUT_PATH}/mafPipePolylineSlice_BESTest)

ADD_EXECUTABLE(medPipeWrappedMeterTest medPipeWrappedMeterTest.h medPipeWrappedMeterTest.cpp)
ADD_TEST(medPipeWrappedMeterTest  ${EXECUTABLE_OUTPUT_PATH}/medPipeWrappedMeterTest)

ADD_EXECUTABLE(medVMEOutputWrappedMeterTest medVMEOutputWrappedMeterTest.h medVMEOutputWrappedMeterTest.cpp)
ADD_TEST(medVMEOutputWrappedMeterTest  ${EXECUTABLE_OUTPUT_PATH}/medVMEOutputWrappedMeterTest)

ADD_EXECUTABLE(medPipeDensityDistanceTest medPipeDensityDistanceTest.h medPipeDensityDistanceTest.cpp)
ADD_TEST(medPipeDensityDistanceTest  ${EXECUTABLE_OUTPUT_PATH}/medPipeDensityDistanceTest)

ADD_EXECUTABLE(medVMEPolylineGraphTest medVMEPolylineGraphTest.h medVMEPolylineGraphTest.cpp)
ADD_TEST(medVMEPolylineGraphTest  ${EXECUTABLE_OUTPUT_PATH}/medVMEPolylineGraphTest)

ADD_EXECUTABLE(medPipeGraphTest medPipeGraphTest.h medPipeGraphTest.cpp)
ADD_TEST(medPipeGraphTest  ${EXECUTABLE_OUTPUT_PATH}/medPipeGraphTest)

ADD_EXECUTABLE(medVMESegmentationVolumeTest medVMESegmentationVolumeTest.h medVMESegmentationVolumeTest.cpp)
ADD_TEST(medVMESegmentationVolumeTest  ${EXECUTABLE_OUTPUT_PATH}/medVMESegmentationVolumeTest)

ADD_EXECUTABLE(medPipeCompoundTest medPipeCompoundTest.h medPipeCompoundTest.cpp)
ADD_TEST(medPipeCompoundTest  ${EXECUTABLE_OUTPUT_PATH}/medPipeCompoundTest)

ADD_EXECUTABLE(medPipeCompoundVolumeTest medPipeCompoundVolumeTest.h medPipeCompoundVolumeTest.cpp)
ADD_TEST(medPipeCompoundVolumeTest  ${EXECUTABLE_OUTPUT_PATH}/medPipeCompoundVolumeTest)

ADD_EXECUTABLE(medPipeCompoundVolumeFixedScalarsTest medPipeCompoundVolumefixedScalarsTest.h medPipeCompoundVolumeFixedScalarsTest.cpp)
ADD_TEST(medPipeCompoundVolumeFixedScalarsTest  ${EXECUTABLE_OUTPUT_PATH}/medPipeCompoundVolumeFixedScalarsTest)

ADD_EXECUTABLE(medVisualPipePolylineGraphTest medVisualPipePolylineGraphTest.h medVisualPipePolylineGraphTest.cpp)
ADD_TEST(medVisualPipePolylineGraphTest  ${EXECUTABLE_OUTPUT_PATH}/medVisualPipePolylineGraphTest)

ADD_EXECUTABLE(medPipeSurfaceEditorTest medPipeSurfaceEditorTest.h medPipeSurfaceEditorTest.cpp)
ADD_TEST(medPipeSurfaceEditorTest  ${EXECUTABLE_OUTPUT_PATH}/medPipeSurfaceEditorTest)

ADD_EXECUTABLE(medPipeTensorFieldSurfaceTest medPipeTensorFieldSurfaceTest.h medPipeTensorFieldSurfaceTest.cpp)
ADD_TEST(medPipeTensorFieldSurfaceTest  ${EXECUTABLE_OUTPUT_PATH}/medPipeTensorFieldSurfaceTest)

ADD_EXECUTABLE(medPipeVectorFieldSurfaceTest medPipeVectorFieldSurfaceTest.h medPipeVectorFieldSurfaceTest.cpp)
ADD_TEST(medPipeVectorFieldSurfaceTest  ${EXECUTABLE_OUTPUT_PATH}/medPipeVectorFieldSurfaceTest)

ADD_EXECUTABLE(medPipeVectorFieldGlyphsTest medPipeVectorFieldGlyphsTest.h medPipeVectorFieldGlyphsTest.cpp)
ADD_TEST(medPipeVectorFieldGlyphsTest  ${EXECUTABLE_OUTPUT_PATH}/medPipeVectorFieldGlyphsTest)

ADD_EXECUTABLE(medPipeVectorFieldMapWithArrowsTest medPipeVectorFieldMapWithArrowsTest.h medPipeVectorFieldMapWithArrowsTest.cpp)
ADD_TEST(medPipeVectorFieldMapWithArrowsTest  ${EXECUTABLE_OUTPUT_PATH}/medPipeVectorFieldMapWithArrowsTest)

ADD_EXECUTABLE(medAttributeSegmentationVolumeTest medAttributeSegmentationVolumeTest.h medAttributeSegmentationVolumeTest.cpp)
ADD_TEST(medAttributeSegmentationVolumeTest  ${EXECUTABLE_OUTPUT_PATH}/medAttributeSegmentationVolumeTest)

ADD_EXECUTABLE(medPipeTensorFieldSliceTest medPipeTensorFieldSliceTest.h medPipeTensorFieldSliceTest.cpp)
ADD_TEST(medPipeTensorFieldSliceTest  ${EXECUTABLE_OUTPUT_PATH}/medPipeTensorFieldSliceTest)

ADD_EXECUTABLE(medPipeVectorFieldSliceTest medPipeVectorFieldSliceTest.h medPipeVectorFieldSliceTest.cpp)
ADD_TEST(medPipeVectorFieldSliceTest  ${EXECUTABLE_OUTPUT_PATH}/medPipeVectorFieldSliceTest)

ADD_EXECUTABLE(medPipeTensorFieldGlyphsTest medPipeTensorFieldGlyphsTest.h medPipeTensorFieldGlyphsTest.cpp)
ADD_TEST(medPipeTensorFieldGlyphsTest  ${EXECUTABLE_OUTPUT_PATH}/medPipeTensorFieldGlyphsTest)

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

ADD_EXECUTABLE(medPipeCompoundVolumeDRRTest  medPipeCompoundVolumeDRRTest.h medPipeCompoundVolumeDRRTest.cpp)
ADD_TEST(medPipeCompoundVolumeDRRTest ${EXECUTABLE_OUTPUT_PATH}/medPipeCompoundVolumeDRRTest)

ADD_EXECUTABLE(medPipeCompoundVolumeIsosurfaceTest  medPipeCompoundVolumeIsosurfaceTest.h medPipeCompoundVolumeIsosurfaceTest.cpp)
ADD_TEST(medPipeCompoundVolumeIsosurfaceTest ${EXECUTABLE_OUTPUT_PATH}/medPipeCompoundVolumeIsosurfaceTest)

ADD_EXECUTABLE(mafVMEVolumeLargeUtilsTest  mafVMEVolumeLargeUtilsTest.h mafVMEVolumeLargeUtilsTest.cpp)
ADD_TEST(mafVMEVolumeLargeUtilsTest ${EXECUTABLE_OUTPUT_PATH}/mafVMEVolumeLargeUtilsTest)

ADD_EXECUTABLE(mafPolylineGraphVertexTest  mafPolylineGraphVertexTest.h mafPolylineGraphVertexTest.cpp)
ADD_TEST(mafPolylineGraphVertexTest ${EXECUTABLE_OUTPUT_PATH}/mafPolylineGraphVertexTest)

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