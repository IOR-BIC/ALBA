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
  mmoRegisterClusters.cpp
  mmoRegisterClusters.h
  mmoVolumeMeasure.cpp
  mmoVolumeMeasure.h
  mmoRAWImporterImages.cpp
  mmoRAWImporterImages.h
  mmoCreatePanoramic.cpp
  mmoCreatePanoramic.h
)

IF (MAF_USE_ITK)
SET(PROJECT_SRCS  ${PROJECT_SRCS}
  mmoRawMotionDataImporter.cpp
  mmoRawMotionDataImporter.h
  mmoLandmarkExporter.cpp
  mmoLandmarkExporter.h
  mmoClassicICPRegistration.cpp
  mmoClassicICPRegistration.h
  )
ENDIF (MAF_USE_ITK)

# List libraries that are needed by this project.
SET(PROJECT_LIBS "mafVME" "mafInteraction")

SET(PROJECT_LIBS ${PROJECT_LIBS} vtkMED medCommon)

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
