#
# Copyright and description to be placed here
#

PROJECT(mafVME)

DBG_MESSAGE("Configuring project ${PROJECT_NAME}")

# Set your list of sources here.
SET(PROJECT_SRCS
  mafDataVector.cpp
  mafDataVector.h
  mafMatrixInterpolator.cpp
  mafMatrixInterpolator.h
  mafMatrixVector.cpp
  mafMatrixVector.h
  mafPipeFactoryVME.cpp
  mafPipeFactoryVME.h
  mafScalarVector.cpp
  mafScalarVector.h
  mafVMEExternalData.cpp
  mafVMEExternalData.h
  mafVMEFactory.cpp
  mafVMEFactory.h
  mafVMEGenericAbstract.cpp
  mafVMEGenericAbstract.h
  mafVMEGroup.cpp
  mafVMEGroup.h
  mafVMEItem.cpp
  mafVMEItem.h
  mafVMEInfoText.cpp
  mafVMEInfoText.h
  mafMSFImporter.cpp
  mafMSFImporter.h
  mafPipePolylineSlice.cpp
  mafPipePolylineSlice.h
  mafPipeMesh.cpp
  mafPipeMesh.h
  mafPipeMeshSlice.cpp
  mafPipeMeshSlice.h
  mafPipeScalar.cpp
  mafPipeScalar.h
  mafVisualPipeVolumeRayCasting.cpp
  mafVisualPipeVolumeRayCasting.h
  mafVMEOutputScalar.cpp
  mafVMEOutputScalar.h
  mafVMEScalar.cpp
  mafVMEScalar.h
  ###############
  medVMEFactory.cpp 
  medVMEFactory.h
  mafVMELabeledVolume.cpp
  mafVMELabeledVolume.h
  medPipeFactoryVME.cpp
  medPipeFactoryVME.h

  mafPipeVolumeMIP.cpp
  mafPipeVolumeMIP.h
  mafPipeDensityDistance.cpp
  mafPipeDensityDistance.h
  mafPipeTrajectories.cpp
  mafPipeTrajectories.h
  mafPipeVolumeDRR.cpp
  mafPipeVolumeDRR.h
  mafPipeVolumeVR.cpp
  mafPipeVolumeVR.h

  # mafPolylineGraph.cpp
  # mafPolylineGraph.h
  mafVMEPolylineGraph.cpp
  mafVMEPolylineGraph.h
  mafVMEPolylineEditor.cpp
  mafVMEPolylineEditor.h
  mafPipePolylineGraphEditor.cpp
  mafPipePolylineGraphEditor.h
  mafVMEOutputPolylineEditor.cpp
  mafVMEOutputPolylineEditor.h
  mafVMEMaps.cpp
  mafVMEMaps.h
  
  mafVMEWrappedMeter.cpp
  mafVMEWrappedMeter.h
  mafVMEOutputWrappedMeter.cpp
  mafVMEOutputWrappedMeter.h
  mafPipeWrappedMeter.cpp
  mafPipeWrappedMeter.h

  mafVMESurfaceEditor.cpp
  mafVMESurfaceEditor.h
  mafPipeSurfaceEditor.cpp
  mafPipeSurfaceEditor.h
  mafVMEOutputSurfaceEditor.cpp
  mafVMEOutputSurfaceEditor.h
  
  mafVisualPipeSlicerSlice.cpp
  mafVisualPipeSlicerSlice.h
  
  mafVisualPipePolylineGraph.cpp
  mafVisualPipePolylineGraph.h

  mafVMEComputeWrapping.h
  mafVMEComputeWrapping.cpp
  mafPipeComputeWrapping.h
  mafPipeComputeWrapping.cpp
  mafVMEOutputComputeWrapping.cpp
  mafVMEOutputComputeWrapping.h 
  
  mafPipeVectorField.cpp
  mafPipeVectorField.h
  mafPipeVectorFieldMapWithArrows.cpp
  mafPipeVectorFieldMapWithArrows.h
  mafPipeVectorFieldGlyphs.cpp
  mafPipeVectorFieldGlyphs.h
  mafPipeVectorFieldSurface.cpp
  mafPipeVectorFieldSurface.h
  mafPipeVectorFieldSlice.cpp
  mafPipeVectorFieldSlice.h
  mafPipeTensorField.cpp
  mafPipeTensorField.h
  mafPipeTensorFieldGlyphs.cpp
  mafPipeTensorFieldGlyphs.h
  mafPipeTensorFieldSurface.cpp
  mafPipeTensorFieldSurface.h
  mafPipeTensorFieldSlice.cpp
  mafPipeTensorFieldSlice.h
  mafPipeCompound.cpp
  mafPipeCompound.h
  mafPipeCompoundVolume.cpp
  mafPipeCompoundVolume.h
  mafPipeCompoundVolumeFixedScalars.cpp
  mafPipeCompoundVolumeFixedScalars.h
  mafVMESegmentationVolume.cpp
  mafVMESegmentationVolume.h
  mafDataPipeCustomSegmentationVolume.cpp
  mafDataPipeCustomSegmentationVolume.h
  mafAttributeSegmentationVolume.cpp
  mafAttributeSegmentationVolume.h
  
  
  ### TO BE COMMITTED DOWN ###
  mafPipeSlice.cpp
  mafPipeSlice.h
  ############################
  
  mafGizmoROI_BES.cpp
  mafGizmoROI_BES.h
  mafPipeMeshSlice_BES.cpp
  mafPipeMeshSlice_BES.h
  mafPipePolylineSlice_BES.cpp
  mafPipePolylineSlice_BES.h
  mafPipeSurfaceSlice_BES.cpp
  mafPipeSurfaceSlice_BES.h
  mafPipeVolumeSlice_BES.cpp
  mafPipeVolumeSlice_BES.h
  
  ############# VolumeLarge Files - TO BE COMMITTED DOWN ########
  # the following files has been moved to mafIO
  #../BES_Beta/IO/mafBrickedFile.cpp    
  #../BES_Beta/IO/mafBrickedFile.h
  #../BES_Beta/IO/mafBrickedFileReader.cpp
  #../BES_Beta/IO/mafBrickedFileReader.h
  #../BES_Beta/IO/mafBrickedFileWriter.cpp
  #../BES_Beta/IO/mafBrickedFileWriter.h
  #../BES_Beta/IO/mafVolumeLargeReader.cpp
  #../BES_Beta/IO/mafVolumeLargeReader.h
  #../BES_Beta/IO/mafVolumeLargeWriter.cpp
  #../BES_Beta/IO/mafVolumeLargeWriter.h
  mafVMEVolumeLarge.cpp
  mafVMEVolumeLarge.h
  mafVMEVolumeLargeUtils.cpp
  mafVMEVolumeLargeUtils.h  
  ###############################################################

  mafPipeVolumeSliceBlend.cpp
  mafPipeVolumeSliceBlend.h

  mafVisualPipeCollisionDetection.cpp
  mafVisualPipeCollisionDetection.h

  mafPipeVolumeSliceNotInterpolated.cpp
  mafPipeVolumeSliceNotInterpolated.h
  
  mafPipeRayCast.cpp
  mafPipeRayCast.h
)

IF (MAF_USE_CRYPTO)
SET(PROJECT_SRCS  ${PROJECT_SRCS}
  mafCrypt.cpp
  mafCrypt.h
  )
ENDIF (MAF_USE_CRYPTO)

IF (MAF_USE_ITK)
SET(PROJECT_SRCS  ${PROJECT_SRCS}
  mafPipeScalarMatrix.cpp
  mafPipeScalarMatrix.h
  mafVMEOutputScalarMatrix.cpp
  mafVMEOutputScalarMatrix.h
  mafVMEScalarMatrix.cpp
  mafVMEScalarMatrix.h
  mafVMEItemScalarMatrix.cpp
  mafVMEItemScalarMatrix.h
  )
ENDIF (MAF_USE_ITK)

IF (MAF_USE_VTK)
  SET(PROJECT_SRCS
  ${PROJECT_SRCS}
  mafPipeGizmo.cpp
  mafPipeGizmo.h
  mafPipeImage3D.cpp
  mafPipeImage3D.h
  mafPipeIsosurface.cpp
  mafPipeIsosurface.h
  mafPipeLandmarkCloud.cpp
  mafPipeLandmarkCloud.h
  mafPipeMeter.cpp
  mafPipeMeter.h
  mafPipePointSet.cpp
  mafPipePointSet.h
  mafPipePolyline.cpp
  mafPipePolyline.h
  mafPipeSurface.cpp
  mafPipeSurface.h
  mafPipeSurfaceSlice.cpp
  mafPipeSurfaceSlice.h
  mafPipeSurfaceTextured.cpp
  mafPipeSurfaceTextured.h
  mafPipeVector.cpp
  mafPipeVector.h
  mafPipeVolumeProjected.cpp
  mafPipeVolumeProjected.h
  mafPipeVolumeSlice.cpp
  mafPipeVolumeSlice.h
  mafVMEGeneric.cpp
  mafVMEGeneric.h
  mafVMEGizmo.cpp
  mafVMEGizmo.h
  mafVMEImage.cpp
  mafVMEImage.h
  mafVMEItemVTK.cpp
  mafVMEItemVTK.h
  mafVMELandmark.cpp
  mafVMELandmark.h
  mafVMELandmarkCloud.cpp
  mafVMELandmarkCloud.h
  mafVMEMesh.cpp
  mafVMEMesh.h
  mafVMEMeter.cpp
  mafVMEMeter.h
  mafVMEOutputImage.cpp
  mafVMEOutputImage.h
  mafVMEOutputLandmarkCloud.cpp
  mafVMEOutputLandmarkCloud.h
  mafVMEOutputMesh.cpp
  mafVMEOutputMesh.h
  mafVMEOutputMeter.cpp
  mafVMEOutputMeter.h
  mafVMEOutputPointSet.cpp
  mafVMEOutputPointSet.h
  mafVMEOutputPolyline.cpp
  mafVMEOutputPolyline.h
  mafVMEOutputSurface.cpp
  mafVMEOutputSurface.h
  mafVMEOutputVolume.cpp
  mafVMEOutputVolume.h
  mafVMEOutputVTK.cpp
  mafVMEOutputVTK.h
  mafVMEPointSet.cpp
  mafVMEPointSet.h
  mafVMEPolyline.cpp
  mafVMEPolyline.h
  mafVMEPolylineSpline.cpp
  mafVMEPolylineSpline.h
  mafVMEProber.cpp
  mafVMEProber.h
  mafVMERefSys.cpp
  mafVMERefSys.h
  mafVMESlicer.cpp
  mafVMESlicer.h
  mafVMESurface.cpp
  mafVMESurface.h
  mafVMESurfaceParametric.cpp
  mafVMESurfaceParametric.h
  mafVMEVector.cpp
  mafVMEVector.h
  mafVMEVolume.cpp
  mafVMEVolume.h
  mafVMEVolumeGray.cpp
  mafVMEVolumeGray.h
  mafVMEVolumeRGB.cpp
  mafVMEVolumeRGB.h
  mmaMeter.cpp
  mmaMeter.h
  ###########
  mafVMERawMotionData.cpp
  mafVMERawMotionData.h
  mafPipeGraph.cpp
  mafPipeGraph.h
  mafVMEAnalog.cpp
  mafVMEAnalog.h
  
 #add for stent
  mafVMEStent.cpp
  mafVMEStent.h

  #stent used tool, to find nearest point
  kdtree.c
  kdtree.h
  )
ENDIF (MAF_USE_VTK)

IF(WIN32)
  SET(PROJECT_SRCS
  ${PROJECT_SRCS}
  mafPipeIsosurfaceGPU.cpp
  mafPipeIsosurfaceGPU.h
  )
ENDIF(WIN32)

SUBDIRS(DataPipes)

# List libraries that are needed by this project.
#SET(PROJECT_LIBS mafCore vtkMAF)
SET(PROJECT_LIBS mafDataPipes)

IF (MAF_USE_CRYPTO)
  SET(PROJECT_LIBS ${PROJECT_LIBS} Crypto)
ENDIF (MAF_USE_CRYPTO)

IF (MAF_USE_VTK)
  SET(PROJECT_LIBS ${PROJECT_LIBS} vtkIO vtkRendering vtkGraphics vtkHybrid)
ENDIF (MAF_USE_VTK)

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})

IF (MAF_USE_VTK)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_VTK_LIBRARY)
ENDIF (MAF_USE_VTK)

IF (MAF_USE_WX)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_WXWIN_LIBRARY)
ENDIF (MAF_USE_WX)

IF (MAF_USE_ITK)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_ITK_LIBRARY)
ENDIF (MAF_USE_ITK)
