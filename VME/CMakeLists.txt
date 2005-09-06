#
# Copyright and description to be placed here
#

PROJECT(mafVME)

DBG_MESSAGE("Configuring project ${PROJECT_NAME}")

# Set your list of sources here.
SET(PROJECT_SRCS
  mafDataInterpolator.cpp
  mafDataInterpolator.h
  mafDataVector.cpp
  mafDataVector.h
  mafMatrixInterpolator.cpp
  mafMatrixInterpolator.h
  mafMatrixVector.cpp
  mafMatrixVector.h
  mafPipeFactoryVME.cpp
  mafPipeFactoryVME.h
  mafVMEFactory.cpp
  mafVMEFactory.h
  mafVMEGenericAbstract.cpp
  mafVMEGenericAbstract.h
  mafVMEGroup.cpp
  mafVMEGroup.h
  mafVMEItem.cpp
  mafVMEItem.h
  mafMSFImporter.cpp
  mafMSFImporter.h
)

IF (MAF_USE_CRYPTO)
SET(PROJECT_SRCS  ${PROJECT_SRCS}
  mafCrypt.cpp
  mafCrypt.h
  )
ENDIF (MAF_USE_CRYPTO)

IF (MAF_USE_VTK)
  SET(PROJECT_SRCS
  ${PROJECT_SRCS}
  mafDataPipeCustom.cpp
  mafDataPipeCustom.h
  mafPipeGizmo.cpp
  mafPipeGizmo.h
  mafPipeIsosurface.cpp
  mafPipeIsosurface.h
  mafPipeLandmarkCloud.cpp
  mafPipeLandmarkCloud.h
  mafPipeMeter.cpp
  mafPipeMeter.h
  mafPipePointSet.cpp
  mafPipePointSet.h
  mafPipeSurface.cpp
  mafPipeSurface.h
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
  mafVMEMeter.cpp
  mafVMEMeter.h
  mafVMEOutputImage.cpp
  mafVMEOutputImage.h
  mafVMEOutputLandmarkCloud.cpp
  mafVMEOutputLandmarkCloud.h
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
  mafVMEProber.cpp
  mafVMEProber.h
  mafVMESlicer.cpp
  mafVMESlicer.h
  mafVMESurface.cpp
  mafVMESurface.h
  mafVMEVolume.cpp
  mafVMEVolume.h
  mafVMEVolumeGray.cpp
  mafVMEVolumeGray.h
  mafVMEVolumeRGB.cpp
  mafVMEVolumeRGB.h
  mafVTKInterpolator.cpp
  mafVTKInterpolator.h
  mmaMeter.cpp
  mmaMeter.h
  vtkMAFDataPipe.cpp
  vtkMAFDataPipe.h
  )
ENDIF (MAF_USE_VTK)


# List libraries that are needed by this project.
SET(PROJECT_LIBS mafCore vtkMAF)

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
