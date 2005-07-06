#
# Copyright and description to be placed here
#

PROJECT(mafOperations)

DBG_MESSAGE("Configuring project mafOperations")

# Set your list of sources here.
SET(PROJECT_SRCS

)

IF (MAF_USE_VTK)
  SET(PROJECT_SRCS
    ${PROJECT_SRCS}
    mmoCreateGroup.cpp
    mmoCreateGroup.h
    mmoCreateMeter.cpp
    mmoCreateMeter.h
    mmoCreateSlicer.cpp
    mmoCreateSlicer.h
    mmoMAFTransform.cpp
    mmoMAFTransform.h
    mmoReparentTo.cpp
    mmoReparentTo.h
    mmoSTLExporter.cpp
    mmoSTLExporter.h
    mmoSTLImporter.cpp
    mmoSTLImporter.h
    mmoTransformInterface.cpp
    mmoTransformInterface.h
    mmoVTKExporter.cpp
    mmoVTKExporter.h
    mmoVTKImporter.cpp
    mmoVTKImporter.h
  )
ENDIF (MAF_USE_VTK)


# List libraries that are needed by this project.
SET(PROJECT_LIBS "mafVME" )

IF (MAF_USE_VTK)
  SET(PROJECT_LIBS ${PROJECT_LIBS} vtkCommon)
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
