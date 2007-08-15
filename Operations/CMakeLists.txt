#
# Copyright and description to be placed here
#

PROJECT(medOperations)

# Set your list of sources here.
SET(PROJECT_SRCS
  mmoDICOMImporter.cpp
  mmoDICOMImporter.h
  mmoCTAImporter.cpp
  mmoCTAImporter.h
  mmoCTMRIImporter.cpp
  mmoCTMRIImporter.h
  mmoLandmarkImporter.cpp
  mmoLandmarkImporter.h
  mmoLandmarkImporterTXT.cpp
  mmoLandmarkImporterTXT.h
  mmoLandmarkImporterWS.cpp
  mmoLandmarkImporterWS.h
  mmoMML.cpp
  mmoMML.h
  mmoMMLContourWidget.cpp
  mmoMMLContourWidget.h
  mmoMMLModelView.cpp
  mmoMMLModelView.h
  mmoMMLParameterView.cpp
  mmoMMLParameterView.h
  mmoRegisterClusters.cpp
  mmoRegisterClusters.h
  mmoVolumeMeasure.cpp
  mmoVolumeMeasure.h
  mmoRAWImporterImages.cpp
  mmoRAWImporterImages.h
  mmoMeshQuality.cpp
  mmoMeshQuality.h
  mmoCropDeformableROI.cpp
  mmoCropDeformableROI.h
  mmoFlipNormals.cpp
  mmoFlipNormals.h
  mmoExtrusionHoles.cpp
  mmoExtrusionHoles.h
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
  medOpLabelizeSurface.cpp
  medOpLabelizeSurface.h
)

IF (MAF_USE_ITK)
SET(PROJECT_SRCS  ${PROJECT_SRCS}
  itkRawMotionImporterUtility.cxx
  itkRawMotionImporterUtility.h
  mmoMotionDataImporter.h
  mmoLandmarkExporter.cpp
  mmoLandmarkExporter.h
  mmoClassicICPRegistration.cpp
  mmoClassicICPRegistration.h
  mmoC3DImporter.cpp
  mmoC3DImporter.h
  mmoEMGImporterWS.cpp
  mmoEMGImporterWS.h
  mmoGRFImporterWS.cpp
  mmoGRFImporterWS.h
)
ENDIF (MAF_USE_ITK)

# List libraries that are needed by this project.
SET(PROJECT_LIBS "mafVME" "mafInteraction")

SET(PROJECT_LIBS ${PROJECT_LIBS} vtkMED medCommon)

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
