#
# Copyright and description to be placed here
#

PROJECT(mafVME)

DBG_MESSAGE("Configuring project ${PROJECT_NAME}")

# Set your list of sources here.
SET(PROJECT_SRCS
mafDataVector.cpp
mafDataVector.h
mafDataInterpolator.cpp
mafDataInterpolator.h
mafMatrixInterpolator.cpp
mafMatrixInterpolator.h
mafMatrixVector.cpp
mafMatrixVector.h
mafVMEFactory.cpp
mafVMEFactory.h
mafVMEGenericAbstract.cpp
mafVMEGenericAbstract.h
mafVMEImage.cpp
mafVMEImage.h
mafVMEItem.h
mafVMEItem.cpp
mafVMESurface.cpp
mafVMESurface.h
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
  mafVMEGeneric.h
  mafVMEGeneric.cpp
  mafVMEItemVTK.cpp
  mafVMEItemVTK.h
  mafVMEOutputVTK.cpp
  mafVMEOutputVTK.h
  mafVMEOutputImage.cpp
  mafVMEOutputImage.h
  mafVMEOutputSurface.cpp
  mafVMEOutputSurface.h
  mafVTKInterpolator.cpp
  mafVTKInterpolator.h
  vtkMAFDataPipe.cpp
  vtkMAFDataPipe.h
  )
ENDIF (MAF_USE_VTK)


# List libraries that are needed by this project.
SET(PROJECT_LIBS mafCore)

IF (MAF_USE_CRYPTO)
  SET(PROJECT_LIBS ${PROJECT_LIBS} Crypto)
ENDIF (MAF_USE_CRYPTO)

IF (MAF_USE_VTK)
  SET(PROJECT_LIBS ${PROJECT_LIBS} vtkIO)
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

# build tests
IF(BUILD_TESTING)
  SUBDIRS(Testing)
ENDIF(BUILD_TESTING)
