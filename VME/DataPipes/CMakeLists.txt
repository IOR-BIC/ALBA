PROJECT(mafDataPipes)

DBG_MESSAGE("Configuring project ${PROJECT_NAME}")

# Set your list of sources here.
SET(PROJECT_SRCS
  mafDataInterpolator.cpp
  mafDataInterpolator.h
  mafDataPipeCustom.cpp
  mafDataPipeCustom.h
  mafDataPipeCustomProber.cpp
  mafDataPipeCustomProber.h
  mafVTKInterpolator.cpp
  mafVTKInterpolator.h
  vtkMAFDataPipe.cpp
  vtkMAFDataPipe.h
  mafScalarInterpolator.cpp
  mafScalarInterpolator.h
)

IF (MAF_USE_ITK)
SET(PROJECT_SRCS  ${PROJECT_SRCS}
  mafScalarMatrixInterpolator.cpp
  mafScalarMatrixInterpolator.h
  )
ENDIF (MAF_USE_ITK)

# List libraries that are needed by this project.
SET(PROJECT_LIBS mafCore vtkMAF)

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})

IF (MAF_USE_VTK)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_VTK_LIBRARY)
ENDIF (MAF_USE_VTK)
