#
# Copyright and description to be placed here
#

PROJECT(medOperations)

# Set your list of sources here.
SET(PROJECT_SRCS
  mmoBonemat.cpp
  mmoBonemat.h
  mmoDICOMImporter.cpp
  mmoDICOMImporter.h
#  bonemat.cxx
  dataset.cxx
  dataset.h
  elements.cxx
  elements.h
  hashtable.cxx
  hashtable.h
  mesh.cxx
  mesh.h
  types.h
)

# List libraries that are needed by this project.
SET(PROJECT_LIBS "mafVME" "mafInteraction")

SET(PROJECT_LIBS ${PROJECT_LIBS} vtkMED medCommon)

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
