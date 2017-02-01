#
# Copyright and description to be placed here
#

PROJECT(mafVisualPipes)

DBG_MESSAGE("Configuring project ${PROJECT_NAME}")

# Set your list of sources here.
SET(PROJECT_SRCS
  mafPipeGenericPolydata.cpp 				mafPipeGenericPolydata.h
  mafPipeMesh.cpp 							mafPipeMesh.h
  mafPipeSurface.cpp 						mafPipeSurface.h
  mafPipeRefSys.cpp 						mafPipeRefSys.h
  mafPipeScalar.cpp  						mafPipeScalar.h
  mafVisualPipeVolumeRayCasting.cpp  		mafVisualPipeVolumeRayCasting.h
  mafPipeVolumeMIP.cpp  					mafPipeVolumeMIP.h
  mafPipeDensityDistance.cpp  				mafPipeDensityDistance.h
  mafPipeTrajectories.cpp  					mafPipeTrajectories.h
  mafPipeVolumeDRR.cpp  					mafPipeVolumeDRR.h
  mafPipeVolumeVR.cpp  						mafPipeVolumeVR.h
  mafPipePolylineGraphEditor.cpp  			mafPipePolylineGraphEditor.h
  mafPipeWrappedMeter.cpp  					mafPipeWrappedMeter.h
  mafPipeSurfaceEditor.cpp  				mafPipeSurfaceEditor.h
  mafVisualPipeSlicerSlice.cpp  			mafVisualPipeSlicerSlice.h  
  mafVisualPipePolylineGraph.cpp  			mafVisualPipePolylineGraph.h
  mafPipeComputeWrapping.cpp  				mafPipeComputeWrapping.h
  mafPipeVectorField.cpp  					mafPipeVectorField.h
  mafPipeVectorFieldMapWithArrows.cpp  		mafPipeVectorFieldMapWithArrows.h
  mafPipeVectorFieldGlyphs.cpp  			mafPipeVectorFieldGlyphs.h
  mafPipeVectorFieldSurface.cpp  			mafPipeVectorFieldSurface.h
  mafPipeVectorFieldSlice.cpp  				mafPipeVectorFieldSlice.h
  mafPipeTensorField.cpp  					mafPipeTensorField.h
  mafPipeTensorFieldGlyphs.cpp  			mafPipeTensorFieldGlyphs.h
  mafPipeTensorFieldSurface.cpp  			mafPipeTensorFieldSurface.h
  mafPipeTensorFieldSlice.cpp  				mafPipeTensorFieldSlice.h
  mafPipeCompound.cpp  						mafPipeCompound.h
  mafPipeCompoundVolume.cpp  				mafPipeCompoundVolume.h
  mafPipeCompoundVolumeFixedScalars.cpp  	mafPipeCompoundVolumeFixedScalars.h
  mafPipeSlice.cpp  						mafPipeSlice.h
  mafPipeMeshSlice.cpp  					mafPipeMeshSlice.h
  mafPipePolylineSlice.cpp  				mafPipePolylineSlice.h
  mafPipeSurfaceSlice.cpp  					mafPipeSurfaceSlice.h
  mafPipeVolumeSlice.cpp  					mafPipeVolumeSlice.h
  mafPipeVolumeSliceBlend.cpp  				mafPipeVolumeSliceBlend.h
  mafVisualPipeCollisionDetection.cpp  		mafVisualPipeCollisionDetection.h
  mafPipeVolumeSliceNotInterpolated.cpp  	mafPipeVolumeSliceNotInterpolated.h  
  mafPipeRayCast.cpp  						mafPipeRayCast.h	
  mafPipeWithScalar.cpp 					mafPipeWithScalar.h
)


IF (MAF_USE_ITK)
SET(PROJECT_SRCS  ${PROJECT_SRCS}
  mafPipeScalarMatrix.cpp  					mafPipeScalarMatrix.h
  )
ENDIF (MAF_USE_ITK)

IF (MAF_USE_VTK)
  SET(PROJECT_SRCS
  ${PROJECT_SRCS}
  mafPipeGizmo.cpp  						mafPipeGizmo.h
  mafPipeImage3D.cpp  						mafPipeImage3D.h
  mafPipeIsosurface.cpp  					mafPipeIsosurface.h
  mafPipeLandmarkCloud.cpp  				mafPipeLandmarkCloud.h
  mafPipeMeter.cpp  						mafPipeMeter.h
  mafPipePointSet.cpp  						mafPipePointSet.h
  mafPipePolyline.cpp  						mafPipePolyline.h
  mafPipeSurfaceTextured.cpp  				mafPipeSurfaceTextured.h
  mafPipeVector.cpp  						mafPipeVector.h
  mafPipeVolumeProjected.cpp  				mafPipeVolumeProjected.h
  mafPipeGraph.cpp  						mafPipeGraph.h
  )
ENDIF (MAF_USE_VTK)

IF(WIN32)
  SET(PROJECT_SRCS
  ${PROJECT_SRCS}
  mafPipeIsosurfaceGPU.cpp  				mafPipeIsosurfaceGPU.h
  )
ENDIF(WIN32)



IF (MAF_USE_CRYPTO)
  SET(PROJECT_LIBS ${PROJECT_LIBS} Crypto)
ENDIF (MAF_USE_CRYPTO)

IF (MAF_USE_VTK)
  SET(PROJECT_LIBS ${PROJECT_LIBS} mafVME vtkIO vtkRendering vtkGraphics vtkHybrid mafDataPipes)
ENDIF (MAF_USE_VTK)

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})

ADD_DEPENDENCIES(${PROJECT_NAME} mafService)

IF (MAF_USE_VTK)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_VTK_LIBRARY)
ENDIF (MAF_USE_VTK)

IF (MAF_USE_WX)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_WXWIN_LIBRARY)
ENDIF (MAF_USE_WX)

IF (MAF_USE_ITK)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_ITK_LIBRARY)
ENDIF (MAF_USE_ITK)
