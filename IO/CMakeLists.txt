#
# Copyright and description to be placed here
#

PROJECT(mafIO)

DBG_MESSAGE("Configuring project mafIO")

# Set your list of sources here.
SET(PROJECT_SRCS
  mafStorable.h
  mafStorable.cpp
  mafStorage.cpp
  mafStorage.h
  mafStorageElement.cpp
  mafStorageElement.h
)

# IO classes for VTK related objects
IF (MAF_USE_VTK)
  #SET(PROJECT_SRCS
  #${PROJECT_SRCS}
  #)
ENDIF (MAF_USE_VTK)

# List libraries that are needed by this project.
SET(PROJECT_LIBS "mafCore" )

# IO classes for XML related objects
IF (MAF_USE_XercesC)
  SET(PROJECT_SRCS
    ${PROJECT_SRCS}
    mafXMLElement.cpp
    mafXMLElement.h
    mafXMLStorage.cpp
    mafXMLStorage.h
    mafXMLString.cpp
    mafXMLString.h
    mmuXMLDOM.h
  )
  SET(PROJECT_LIBS ${PROJECT_LIBS} XercesC)
ENDIF (MAF_USE_XercesC)

IF (MAF_USE_VTK)
  SET(PROJECT_LIBS ${PROJECT_LIBS} vtkIO)
ENDIF (MAF_USE_VTK)

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})

#IF (MAF_USE_VTK)
#  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_VTK_LIBRARY)
#ENDIF (MAF_USE_VTK)

#IF (MAF_USE_WX)
#  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_WXWIN_LIBRARY)
#ENDIF (MAF_USE_WX)

IF (MAF_USE_XercesC)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_XercesC_LIBRARY)
ENDIF (MAF_USE_XercesC)

IF(BUILD_TESTING)
#  SUBDIRS(Testing)
ENDIF(BUILD_TESTING)
