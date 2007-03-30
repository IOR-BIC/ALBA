#
# Copyright and description to be placed here
#

PROJECT(mafOperations)

DBG_MESSAGE("Configuring project mafOperations")

# Set your list of sources here.
SET(PROJECT_SRCS
)

IF (MAF_USE_VTK)
  SET(PROJECT_SRCS
    ${PROJECT_SRCS}
    mmo2DMeasure.cpp
    mmo2DMeasure.h
    mmoAddLandmark.cpp
    mmoAddLandmark.h
    mmoClipSurface.cpp
    mmoClipSurface.h
    mmoCreateGroup.cpp
    mmoCreateGroup.h
    mmoCreateMeter.cpp
    mmoCreateMeter.h
    mmoCreateSurfaceParametric.cpp
    mmoCreateSurfaceParametric.h
    mmoCreateProber.cpp
    mmoCreateProber.h
    mmoCreateRefSys.cpp
    mmoCreateRefSys.h
    mmoCreateSlicer.cpp
    mmoCreateSlicer.h
    mmoCreateSpline.cpp
    mmoCreateSpline.h
    mmoRemoveCells.cpp
    mmoRemoveCells.h
    mmoEditMetadata.cpp
    mmoEditMetadata.h
    mmoExplodeCollapse.cpp
    mmoExplodeCollapse.h
    mmoExtractIsosurface.cpp
    mmoExtractIsosurface.h
    mmoFilterSurface.cpp
    mmoFilterSurface.h
    mmoDecimateSurface.cpp
    mmoDecimateSurface.h
    mmoConnectivitySurface.cpp
    mmoConnectivitySurface.h
    mmoFilterVolume.cpp
    mmoFilterVolume.h
    mmoImageImporter.cpp
    mmoImageImporter.h
    mmoMAFTransform.cpp
    mmoMAFTransform.h
    mmoMAFTransformScale.cpp
    mmoMAFTransformScale.h
    mmoMSFExporter.cpp
    mmoMSFExporter.h
    mmoMSF1xImporter.cpp
    mmoMSF1xImporter.h
    mmoRawExporter.cpp
    mmoRawExporter.h
    mmoRAWImporterVolume.cpp
    mmoRAWImporterVolume.h
    mmoReparentTo.cpp
    mmoReparentTo.h
    mmoSTLExporter.cpp
    mmoSTLExporter.h
    mmoSTLImporter.cpp
    mmoSTLImporter.h
    mmoTransformInterface.cpp
    mmoTransformInterface.h
    mmoVolumeResample.cpp
    mmoVolumeResample.h
    mmoVOIDensity.cpp
    mmoVOIDensity.h
    mmoVRMLImporter.cpp
    mmoVRMLImporter.h
    mmoVTKExporter.cpp
    mmoVTKExporter.h
    mmoVTKImporter.cpp
    mmoVTKImporter.h
    mmoCrop.cpp
    mmoCrop.h
    mmoBooleanSurface.cpp
    mmoBooleanSurface.h
    mmoEditNormals.cpp
    mmoEditNormals.h
  )
ENDIF (MAF_USE_VTK)

IF (MAF_USE_ITK)
  SET(PROJECT_SRCS
    ${PROJECT_SRCS}
    mafASCIIImporterUtility.cpp
    mafASCIIImporterUtility.h
    mmoASCIIImporter.cpp
    mmoASCIIImporter.h
    mmoScalarToSurface.cpp
    mmoScalarToSurface.h
  )
ENDIF (MAF_USE_ITK)

# List libraries that are needed by this project.
SET(PROJECT_LIBS "mafVME" "mafInteraction")

IF (MAF_USE_VTK)
  SET(PROJECT_LIBS ${PROJECT_LIBS} vtkCommon vtkMAF)
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

