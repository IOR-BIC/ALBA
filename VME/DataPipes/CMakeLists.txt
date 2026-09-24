PROJECT(albaDataPipes)

DBG_MESSAGE("Configuring project ${PROJECT_NAME}")

# Set your list of sources here.
SET(PROJECT_SRCS
  albaDataPipeInterpolator.cpp
  albaDataPipeInterpolator.h
  albaDataPipeCustom.cpp
  albaDataPipeCustom.h
  albaDataPipeCustomProber.cpp
  albaDataPipeCustomProber.h
  albaDataPipeInterpolatorVTK.cpp
  albaDataPipeInterpolatorVTK.h
  vtkALBADataPipe.cpp
  vtkALBADataPipe.h
  albaDataPipeInterpolatorScalarMatrix.cpp
  albaDataPipeInterpolatorScalarMatrix.h
  )

# List libraries that are needed by this project.
SET(PROJECT_LIBS albaCore vtkALBA)

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})

IF (ALBA_USE_VTK)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_VTK_LIBRARY)
ENDIF (ALBA_USE_VTK)
