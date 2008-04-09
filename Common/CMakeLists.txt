#
# Copyright and description to be placed here
#

PROJECT(medCommon)

# Set your list of sources here.
SET(PROJECT_SRCS
  mmiDICOMImporterInteractor.cpp
  mmiDICOMImporterInteractor.h
  mafGizmoSlice.cpp
  mafGizmoSlice.h
  mmiInfoImage.cpp
  mmiInfoImage.h
  mmiSelectPoint.cpp
  mmiSelectPoint.h
  medLogicWithManagers.cpp
  medLogicWithManagers.h
  medGeometryEditorPolylineGraph.cpp
  medGeometryEditorPolylineGraph.h
  medDecl.h
  medCurvilinearAbscissaOnSkeletonHelper.cpp
  medCurvilinearAbscissaOnSkeletonHelper.h
  medGizmoInteractionDebugger.cpp
  medGizmoInteractionDebugger.h
)

# List libraries that are needed by this project.
SET(PROJECT_LIBS "mafVME" "mafInteraction")

SET(PROJECT_LIBS ${PROJECT_LIBS} vtkMED)

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
