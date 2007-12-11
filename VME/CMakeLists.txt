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
  mafVMEOutputScalar.cpp
  mafVMEOutputScalar.h
  mafVMEScalar.cpp
  mafVMEScalar.h
  mafVMEItemScalar.cpp
  mafVMEItemScalar.h
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
  )
ENDIF (MAF_USE_VTK)

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
