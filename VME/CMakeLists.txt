#
# Copyright and description to be placed here
#

PROJECT(albaVME)

DBG_MESSAGE("Configuring project ${PROJECT_NAME}")

# Set your list of sources here.
SET(PROJECT_SRCS
  albaDataVector.cpp  albaDataVector.h
  albaMatrixInterpolator.cpp  				albaMatrixInterpolator.h
  albaMatrixVector.cpp  						albaMatrixVector.h
  albaPipeFactoryVME.cpp  					albaPipeFactoryVME.h
  albaScalarVector.cpp  						albaScalarVector.h
  albaVMEExternalData.cpp  					albaVMEExternalData.h
  albaVMEFactory.cpp  						albaVMEFactory.h
  albaVMEGenericAbstract.cpp  				albaVMEGenericAbstract.h
  albaVMEGroup.cpp  							albaVMEGroup.h
  albaVMEItem.cpp  							albaVMEItem.h
  albaVMEInfoText.cpp  						albaVMEInfoText.h
  albaMSFImporter.cpp  						albaMSFImporter.h
  albaVMEOutputScalar.cpp  					albaVMEOutputScalar.h
  albaVMEScalar.cpp  						albaVMEScalar.h
  ###############
  albaVMELabeledVolume.cpp  					albaVMELabeledVolume.h

 #albaPolylineGraph.cpp  					albaPolylineGraph.h
  albaVMEPolylineGraph.cpp  					albaVMEPolylineGraph.h
  albaVMEPolylineEditor.cpp  				albaVMEPolylineEditor.h
  albaVMEOutputPolylineEditor.cpp  			albaVMEOutputPolylineEditor.h
  albaVMEMaps.cpp  							albaVMEMaps.h  
  albaVMEWrappedMeter.cpp  					albaVMEWrappedMeter.h
  albaVMEOutputWrappedMeter.cpp  			albaVMEOutputWrappedMeter.h
  albaVMESurfaceEditor.cpp  					albaVMESurfaceEditor.h
  albaVMEOutputSurfaceEditor.cpp  			albaVMEOutputSurfaceEditor.h  
  albaVMEComputeWrapping.cpp  				albaVMEComputeWrapping.h
  albaVMEOutputComputeWrapping.cpp  			albaVMEOutputComputeWrapping.h   
  albaVMESegmentationVolume.cpp  			albaVMESegmentationVolume.h
  albaAttributeSegmentationVolume.cpp  		albaAttributeSegmentationVolume.h
	albaDataPipeCustomSegmentationVolume.cpp  	albaDataPipeCustomSegmentationVolume.h
	albaVMEPointCloud.cpp 					albaVMEPointCloud.h
	albaVMEProsthesis.cpp albaVMEProsthesis.h
)

IF (ALBA_USE_CRYPTO)
SET(PROJECT_SRCS  ${PROJECT_SRCS}
  albaCrypt.cpp  							albaCrypt.h
  )

	ADD_DEFINITIONS(-DCRYPTOPP_DISABLE_ASM)
ENDIF (ALBA_USE_CRYPTO)

IF (ALBA_USE_ITK)
SET(PROJECT_SRCS  ${PROJECT_SRCS}
  albaVMEOutputScalarMatrix.cpp  			albaVMEOutputScalarMatrix.h
  albaVMEScalarMatrix.cpp  					albaVMEScalarMatrix.h
  albaVMEItemScalarMatrix.cpp  				albaVMEItemScalarMatrix.h
  )
ENDIF (ALBA_USE_ITK)

IF (ALBA_USE_VTK)
  SET(PROJECT_SRCS
  ${PROJECT_SRCS}
  albaVMEGeneric.cpp  						albaVMEGeneric.h
  albaVMEGizmo.cpp  							albaVMEGizmo.h
  albaVMEImage.cpp  							albaVMEImage.h
  albaVMEItemVTK.cpp  						albaVMEItemVTK.h
  albaVMELandmark.cpp  						albaVMELandmark.h
  albaVMELandmarkCloud.cpp  					albaVMELandmarkCloud.h
  albaVMEMesh.cpp  							albaVMEMesh.h
  albaVMEMeter.cpp  							albaVMEMeter.h
  albaVMEOutputImage.cpp  					albaVMEOutputImage.h
  albaVMEOutputLandmarkCloud.cpp  			albaVMEOutputLandmarkCloud.h
  albaVMEOutputMesh.cpp  					albaVMEOutputMesh.h
  albaVMEOutputMeter.cpp  					albaVMEOutputMeter.h
  albaVMEOutputPointSet.cpp  				albaVMEOutputPointSet.h
  albaVMEOutputPolyline.cpp  				albaVMEOutputPolyline.h
  albaVMEOutputSurface.cpp  					albaVMEOutputSurface.h
  albaVMEOutputVolume.cpp  					albaVMEOutputVolume.h
  albaVMEOutputVTK.cpp  						albaVMEOutputVTK.h
  albaVMEPointSet.cpp  						albaVMEPointSet.h
  albaVMEPolyline.cpp  						albaVMEPolyline.h
  albaVMEPolylineSpline.cpp  				albaVMEPolylineSpline.h
  albaVMEProber.cpp  						albaVMEProber.h
  albaVMERefSys.cpp  						albaVMERefSys.h
  albaVMESlicer.cpp  						albaVMESlicer.h
  albaVMESurface.cpp  						albaVMESurface.h
  albaVMESurfaceParametric.cpp  				albaVMESurfaceParametric.h
  albaVMEVector.cpp  						albaVMEVector.h
  albaVMEVolume.cpp  						albaVMEVolume.h
  albaVMEVolumeGray.cpp  					albaVMEVolumeGray.h
  albaVMEVolumeRGB.cpp  						albaVMEVolumeRGB.h
  mmaMeter.cpp  							mmaMeter.h
  ###########
  albaVMERawMotionData.cpp  					albaVMERawMotionData.h
  albaVMEAnalog.cpp  						albaVMEAnalog.h
  
  #add for stent
  albaVMEStent.cpp  							albaVMEStent.h

  #stent used tool, to find nearest point
  kdtree.c  								kdtree.h
  )
ENDIF (ALBA_USE_VTK)

IF(WIN32)
  SET(PROJECT_SRCS
  ${PROJECT_SRCS}
  )
ENDIF(WIN32)

SUBDIRS(DataPipes)

# List libraries that are needed by this project.
#SET(PROJECT_LIBS albaCore vtkALBA)
SET(PROJECT_LIBS albaDataPipes ${wxWidgets_LIBRARIES})

IF (ALBA_USE_CRYPTO)
  SET(PROJECT_LIBS ${PROJECT_LIBS} Crypto)
ENDIF (ALBA_USE_CRYPTO)

IF (ALBA_USE_VTK)
  SET(PROJECT_LIBS ${PROJECT_LIBS} vtkIO vtkRendering vtkGraphics vtkHybrid)
ENDIF (ALBA_USE_VTK)

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})

ADD_DEPENDENCIES(${PROJECT_NAME} albaService)

IF (ALBA_USE_VTK)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_VTK_LIBRARY)
ENDIF (ALBA_USE_VTK)

IF (ALBA_USE_WX)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_WXWIN_LIBRARY)
ENDIF (ALBA_USE_WX)

IF (ALBA_USE_ITK)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_ITK_LIBRARY)
ENDIF (ALBA_USE_ITK)

