#
# Copyright and description to be placed here
#

PROJECT(medVME)

# Set your list of sources here.
SET(PROJECT_SRCS
  medPipeFactoryVME.cpp
  medPipeFactoryVME.h
  medPipeVolumeMIP.cpp
  medPipeVolumeMIP.h
  medPipeDensityDistance.cpp
  medPipeDensityDistance.h
  medPipeVolumeDRR.cpp
  medPipeVolumeDRR.h
)

IF (MAF_USE_ITK)
SET(PROJECT_SRCS  ${PROJECT_SRCS}
  mafVMERawMotionData.cpp
  mafVMERawMotionData.h
)
ENDIF (MAF_USE_ITK)

# List libraries that are needed by this project.
SET(PROJECT_LIBS "mafVME" "mafInteraction")

SET(PROJECT_LIBS ${PROJECT_LIBS} vtkMED)

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})

