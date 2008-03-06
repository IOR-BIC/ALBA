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
    mafOpDecomposeTimeVarVME.cpp
    mafOpDecomposeTimeVarVME.h
    mafOpImporterMSF.cpp
    mafOpImporterMSF.h
    mafOpImporterExternalFile.cpp
    mafOpImporterExternalFile.h
    mafOpLabelExtractor.cpp
    mafOpLabelExtractor.h
    mafOpOpenExternalFile.cpp
    mafOpOpenExternalFile.h
    mafOp2DMeasure.cpp
    mafOp2DMeasure.h
    mafOpAddLandmark.cpp
    mafOpAddLandmark.h
    mafOpExporterBmp.cpp
    mafOpExporterBmp.h
    mafOpClipSurface.cpp
    mafOpClipSurface.h
    mafOpCreateGroup.cpp
    mafOpCreateGroup.h
    mafOpCreateMeter.cpp
    mafOpCreateMeter.h
    mafOpCreateSurfaceParametric.cpp
    mafOpCreateSurfaceParametric.h
    mafOpCreateProber.cpp
    mafOpCreateProber.h
    mafOpCreateRefSys.cpp
    mafOpCreateRefSys.h
    mafOpCreateSlicer.cpp
    mafOpCreateSlicer.h
    mafOpCreateSpline.cpp
    mafOpCreateSpline.h
    mafOpRemoveCells.cpp
    mafOpRemoveCells.h
    mafOpEditMetadata.cpp
    mafOpEditMetadata.h
    mafOpExplodeCollapse.cpp
    mafOpExplodeCollapse.h
    mafOpExtractIsosurface.cpp
    mafOpExtractIsosurface.h
    mafOpFilterSurface.cpp
    mafOpFilterSurface.h
    mafOpDecimateSurface.cpp
    mafOpDecimateSurface.h
    mafOpConnectivitySurface.cpp
    mafOpConnectivitySurface.h
    mafOpFilterVolume.cpp
    mafOpFilterVolume.h
    mafOpImporterImage.cpp
    mafOpImporterImage.h
    mafOpMAFTransform.cpp
    mafOpMAFTransform.h
    mafOpExporterMSF.cpp
    mafOpExporterMSF.h
    mafOpImporterMSF1x.cpp
    mafOpImporterMSF1x.h
    mafOpExporterRaw.cpp
    mafOpExporterRaw.h
    mafOpImporterRAWVolume.cpp
    mafOpImporterRAWVolume.h
    mafOpReparentTo.cpp
    mafOpReparentTo.h
    mafOpExporterSTL.cpp
    mafOpExporterSTL.h
    mafOpImporterSTL.cpp
    mafOpImporterSTL.h
    mafOpTransformInterface.cpp
    mafOpTransformInterface.h
    mafOpVolumeResample.cpp
    mafOpVolumeResample.h
    mafOpVOIDensity.cpp
    mafOpVOIDensity.h
    mafOpImporterVRML.cpp
    mafOpImporterVRML.h
    mafOpExporterVTK.cpp
    mafOpExporterVTK.h
    mafOpImporterVTK.cpp
    mafOpImporterVTK.h
    mafOpCrop.cpp
    mafOpCrop.h
    mafOpBooleanSurface.cpp
    mafOpBooleanSurface.h
    mafOpEditNormals.cpp
    mafOpEditNormals.h
  )
ENDIF (MAF_USE_VTK)

IF (MAF_USE_ITK)
  SET(PROJECT_SRCS
    ${PROJECT_SRCS}
    mafASCIIImporterUtility.cpp
    mafASCIIImporterUtility.h
    mafOpImporterASCII.cpp
    mafOpImporterASCII.h
    mafVMEDataSetAttributesImporter.cpp
    mafVMEDataSetAttributesImporter.h
    mafVMEMeshAnsysTextImporter.cpp
    mafVMEMeshAnsysTextImporter.h
    mafVMEMeshAnsysTextExporter.cpp
    mafVMEMeshAnsysTextExporter.h
    mafParabolicMeshToLinearMeshFilter.cxx
    mafParabolicMeshToLinearMeshFilter.h
    mafOpImporterMesh.cpp
    mafOpImporterMesh.h
    mafOpScalarToSurface.cpp
    mafOpScalarToSurface.h
    mafOpImporterVMEDataSetAttributes.cpp
    mafOpImporterVMEDataSetAttributes.h
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

