#
# Copyright and description to be placed here
#

PROJECT(medVME)

# Set your list of sources here.
SET(PROJECT_SRCS
  medVMEFactory.cpp 
  medVMEFactory.h
  medVMELabeledVolume.cpp
  medVMELabeledVolume.h
  medPipeFactoryVME.cpp
  medPipeFactoryVME.h

  medPipeVolumeMIP.cpp
  medPipeVolumeMIP.h
  medPipeDensityDistance.cpp
  medPipeDensityDistance.h
  medPipeTrajectories.cpp
  medPipeTrajectories.h
  medPipeVolumeDRR.cpp
  medPipeVolumeDRR.h
  medPipeVolumeVR.cpp
  medPipeVolumeVR.h
  medVMEAnalog.cpp
  medVMEAnalog.h

  mafPolylineGraph.cpp
  mafPolylineGraph.h
  medVMEPolylineGraph.cpp
  medVMEPolylineGraph.h
  medVMEPolylineEditor.cpp
  medVMEPolylineEditor.h
  medPipePolylineGraphEditor.cpp
  medPipePolylineGraphEditor.h
  medVMEOutputPolylineEditor.cpp
  medVMEOutputPolylineEditor.h
  
  medVMEWrappedMeter.cpp
  medVMEWrappedMeter.h
  medVMEOutputWrappedMeter.cpp
  medVMEOutputWrappedMeter.h
  medPipeWrappedMeter.cpp
  medPipeWrappedMeter.h

  medVMESurfaceEditor.cpp
  medVMESurfaceEditor.h
  medPipeSurfaceEditor.cpp
  medPipeSurfaceEditor.h
  medVMEOutputSurfaceEditor.cpp
  medVMEOutputSurfaceEditor.h
  
  medVisualPipeSlicerSlice.cpp
  medVisualPipeSlicerSlice.h
  
  medVisualPipePolylineGraph.cpp
  medVisualPipePolylineGraph.h

  medVMEComputeWrapping.h
  medVMEComputeWrapping.cpp
  medPipeComputeWrapping.h
  medPipeComputeWrapping.cpp
  medVMEOutputComputeWrapping.cpp
  medVMEOutputComputeWrapping.h 

)

IF (MAF_USE_ITK)
SET(PROJECT_SRCS  ${PROJECT_SRCS}
  mafVMERawMotionData.cpp
  mafVMERawMotionData.h
  medPipeGraph.cpp
  medPipeGraph.h
)
ENDIF (MAF_USE_ITK)

# List libraries that are needed by this project.
SET(PROJECT_LIBS "mafVME" "mafInteraction" "vtkMAF")

SET(PROJECT_LIBS ${PROJECT_LIBS} vtkMED)

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})

