#
# Copyright and description to be placed here
#

PROJECT(medOperations)

# Set your list of sources here.
SET(PROJECT_SRCS
  medOpCreateLabeledVolume.cpp
  medOpCreateLabeledVolume.h
  mmoDICOMImporter.cpp
  mmoDICOMImporter.h
  medOpImporterCTA.cpp
  medOpImporterCTA.h
  medOpImporterCTMRI.cpp
  medOpImporterCTMRI.h
  medOpImporterLandmark.cpp
  medOpImporterLandmark.h
  medOpImporterLandmarkTXT.cpp
  medOpImporterLandmarkTXT.h
  medOpImporterLandmarkWS.cpp
  medOpImporterLandmarkWS.h
  medOpMML.cpp
  medOpMML.h
  medOpMMLContourWidget.cpp
  medOpMMLContourWidget.h
  medOpMMLModelView.cpp
  medOpMMLModelView.h
  medOpMMLParameterView.cpp
  medOpMMLParameterView.h
  medOpRegisterClusters.cpp
  medOpRegisterClusters.h
  medOpVolumeMeasure.cpp
  medOpVolumeMeasure.h
  medOpImporterRAWImages.cpp
  medOpImporterRAWImages.h
  medOpMeshQuality.cpp
  medOpMeshQuality.h
  medOpCropDeformableROI.cpp
  medOpCropDeformableROI.h
  medOpFlipNormals.cpp
  medOpFlipNormals.h
  medOpExtrusionHoles.cpp
  medOpExtrusionHoles.h
  medOpVolumeResample.cpp
  medOpVolumeResample.h
  medOpSplitSurface.cpp
  medOpSplitSurface.h
  medOpInteractiveClipSurface.cpp
  medOpInteractiveClipSurface.h
  medOpCreateEditSkeleton.cpp
  medOpCreateEditSkeleton.h
  medOpCreateSurface.cpp
  medOpCreateSurface.h
  medOpMove.cpp
  medOpMove.h
  medOpScaleDataset.cpp
  medOpScaleDataset.h

  medOpCreateWrappedMeter.cpp
  medOpCreateWrappedMeter.h
  
  medOpComputeWrapping.h
  medOpComputeWrapping.cpp  
  
  medOpExporterWrappedMeter.cpp
  medOpExporterWrappedMeter.h

  medOpLabelizeSurface.cpp
  medOpLabelizeSurface.h
  medOp2DMeasure.cpp
  medOp2DMeasure.h
  
  medOpFreezeVME.cpp
  medOpFreezeVME.h
  
  medOpIterativeRegistration.cpp
  medOpIterativeRegistration.h
  
  medOpImporterDicom.cpp
  medOpImporterDicom.h
  
  medOpSmoothSurface.cpp
  medOpSmoothSurface.h
  
  medOpCleanSurface.cpp
  medOpCleanSurface.h
  
  medOpTriangulateSurface.cpp
  medOpTriangulateSurface.h

  medOpSurfaceMirror.cpp
  medOpSurfaceMirror.h
  
  medOpImporterVTK.cpp
  medOpImporterVTK.h

  medOpSubdivide.cpp
  medOpSubdivide.h
  
  medOpImporterDicomXA.cpp
  medOpImporterDicomXA.h
)
  
IF (MED_USE_OFFIS)
SET(PROJECT_SRCS ${PROJECT_SRCS}
  medOpImporterDicomOffis.h
  medOpImporterDicomOffis.cpp
)
ENDIF (MED_USE_OFFIS)
  


IF (MED_USE_GDCM)
SET(PROJECT_SRCS ${PROJECT_SRCS}
  medOpImporterDicomGrassroots.cpp
  medOpImporterDicomGrassroots.h
 )
ENDIF (MED_USE_GDCM)

  
IF (MAF_USE_ITK)
SET(PROJECT_SRCS  ${PROJECT_SRCS}
  itkRawMotionImporterUtility.cxx
  itkRawMotionImporterUtility.h
  medOpImporterMotionData.h
  medOpExporterLandmark.cpp
  medOpExporterLandmark.h
  medOpClassicICPRegistration.cpp
  medOpClassicICPRegistration.h
  medOpImporterC3D.cpp
  medOpImporterC3D.h
  medOpImporterGRFWS.cpp
  medOpImporterGRFWS.h
  medOpImporterAnalogWS.cpp
  medOpImporterAnalogWS.h
  medOpSmoothSurfaceCells.cpp
  medOpSmoothSurfaceCells.h
  medOpExporterMeters.cpp
  medOpExporterMeters.h
  medOpInteractionDebugger.cpp
  medOpInteractionDebugger.h
)
ENDIF (MAF_USE_ITK)

# List libraries that are needed by this project.
SET(PROJECT_LIBS "mafVME" "mafInteraction")

SET(PROJECT_LIBS ${PROJECT_LIBS} vtkMED medCommon medInteraction medGUI)

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})

ADD_DEPENDENCIES(${PROJECT_NAME} medGui)
  
IF (MED_USE_GDCM)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_GDCM_LIBRARY)
ENDIF (MED_USE_GDCM)