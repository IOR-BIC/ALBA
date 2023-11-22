#
# Copyright and description to be placed here
#

PROJECT(albaVisualPipes)

DBG_MESSAGE("Configuring project ${PROJECT_NAME}")

# Set your list of sources here.
SET(PROJECT_SRCS
  albaPipeGenericPolydata.cpp 				albaPipeGenericPolydata.h
  albaPipeMesh.cpp 							albaPipeMesh.h
  albaPipeSurface.cpp 						albaPipeSurface.h
  albaPipeRefSys.cpp 						albaPipeRefSys.h
  albaPipeScalar.cpp  						albaPipeScalar.h
  albaVisualPipeVolumeRayCasting.cpp  		albaVisualPipeVolumeRayCasting.h
  albaPipeVolumeMIP.cpp  					albaPipeVolumeMIP.h
  albaPipeDensityDistance.cpp  				albaPipeDensityDistance.h
  albaPipeTrajectories.cpp  					albaPipeTrajectories.h
  albaPipeVolumeDRR.cpp  					albaPipeVolumeDRR.h
  albaPipeVolumeVR.cpp  						albaPipeVolumeVR.h
  albaPipePolylineGraphEditor.cpp  			albaPipePolylineGraphEditor.h
  albaPipeWrappedMeter.cpp  					albaPipeWrappedMeter.h
  albaPipeSurfaceEditor.cpp  				albaPipeSurfaceEditor.h
  albaVisualPipeSlicerSlice.cpp  			albaVisualPipeSlicerSlice.h  
  albaVisualPipePolylineGraph.cpp  			albaVisualPipePolylineGraph.h
  albaPipeComputeWrapping.cpp  				albaPipeComputeWrapping.h
  albaPipeVectorField.cpp  					albaPipeVectorField.h
  albaPipeVectorFieldMapWithArrows.cpp  		albaPipeVectorFieldMapWithArrows.h
  albaPipeVectorFieldGlyphs.cpp  			albaPipeVectorFieldGlyphs.h
  albaPipeVectorFieldSurface.cpp  			albaPipeVectorFieldSurface.h
  albaPipeVectorFieldSlice.cpp  				albaPipeVectorFieldSlice.h
  albaPipeTensorField.cpp  					albaPipeTensorField.h
  albaPipeTensorFieldGlyphs.cpp  			albaPipeTensorFieldGlyphs.h
  albaPipeTensorFieldSurface.cpp  			albaPipeTensorFieldSurface.h
  albaPipeTensorFieldSlice.cpp  				albaPipeTensorFieldSlice.h
  albaPipeCompound.cpp  						albaPipeCompound.h
  albaPipeCompoundVolume.cpp  				albaPipeCompoundVolume.h
  albaPipeCompoundVolumeFixedScalars.cpp  	albaPipeCompoundVolumeFixedScalars.h
  albaPipeSlice.cpp  						albaPipeSlice.h
  albaPipeMeshSlice.cpp  					albaPipeMeshSlice.h
  albaPipePolylineSlice.cpp  				albaPipePolylineSlice.h
  albaPipeSurfaceSlice.cpp  					albaPipeSurfaceSlice.h
	albaPipeVolumeOrthoSlice.cpp				albaPipeVolumeOrthoSlice.h
	albaPipeVolumeArbSlice.cpp				albaPipeVolumeArbSlice.h
  albaPipeVolumeSliceBlend.cpp  				albaPipeVolumeSliceBlend.h
  albaVisualPipeCollisionDetection.cpp  		albaVisualPipeCollisionDetection.h
  albaPipeRayCast.cpp  						albaPipeRayCast.h	
  albaPipeWithScalar.cpp 					albaPipeWithScalar.h
  albaPipeVolumeArbOrthoSlice.cpp			albaPipeVolumeArbOrthoSlice.h
  albaPipePointCloud.cpp					albaPipePointCloud.h
)


IF (ALBA_USE_ITK)
SET(PROJECT_SRCS  ${PROJECT_SRCS}
  albaPipeScalarMatrix.cpp  					albaPipeScalarMatrix.h
  )
ENDIF (ALBA_USE_ITK)

IF (ALBA_USE_VTK)
  SET(PROJECT_SRCS
  ${PROJECT_SRCS}
  albaPipeGizmo.cpp  						albaPipeGizmo.h
  albaPipeImage3D.cpp  						albaPipeImage3D.h
  albaPipeIsosurface.cpp  					albaPipeIsosurface.h
  albaPipeLandmarkCloud.cpp  				albaPipeLandmarkCloud.h
  albaPipeMeter.cpp  						albaPipeMeter.h
  albaPipePointSet.cpp  						albaPipePointSet.h
  albaPipePolyline.cpp  						albaPipePolyline.h
  albaPipeSurfaceTextured.cpp  				albaPipeSurfaceTextured.h
  albaPipeVector.cpp  						albaPipeVector.h
  albaPipeVolumeProjected.cpp  				albaPipeVolumeProjected.h
  albaPipeGraph.cpp  						albaPipeGraph.h
  )
ENDIF (ALBA_USE_VTK)

IF(WIN32)
  SET(PROJECT_SRCS
  ${PROJECT_SRCS}
  albaPipeIsosurfaceGPU.cpp  				albaPipeIsosurfaceGPU.h
  )
ENDIF(WIN32)



IF (ALBA_USE_CRYPTO)
  SET(PROJECT_LIBS ${PROJECT_LIBS} Crypto )
ENDIF (ALBA_USE_CRYPTO)

IF (ALBA_USE_VTK)
  SET(PROJECT_LIBS ${PROJECT_LIBS} albaVME vtkIO vtkRendering vtkGraphics vtkHybrid albaDataPipes ${wxWidgets_LIBRARIES})
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
