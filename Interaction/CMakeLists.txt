#
# Copyright and description to be placed here
#

PROJECT(mafInteraction)

DBG_MESSAGE("Configuring project ${PROJECT_NAME}")

# Set your list of sources here.
SET(PROJECT_SRCS
mafAgent.cpp
mafAgent.h
mafAgentEventQueue.h
mafAgentEventQueue.cpp
mafAgentThreaded.h
mafAgentThreaded.cpp
mafDevice.cpp
mafDevice.h
mafDeviceSet.cpp
mafDeviceSet.h
#mafDeviceManager.cpp
#mafDeviceManager.h
#mafEventHandler.cpp
#mafEventHandler.h
mafEventInteraction.cpp
mafEventInteraction.h
#mafInteractor.cpp
#mafInteractor.h
)

#IF (MAF_USE_VTK)
#  SET(PROJECT_SRCS
#  ${PROJECT_SRCS}
#  )
#ENDIF (MAF_USE_VTK)

# List libraries that are needed by this project.
SET(PROJECT_LIBS mafBase)

#IF (MAF_USE_VTK)
#  SET(PROJECT_LIBS ${PROJECT_LIBS} vtkIO vtkRendering vtkGraphics)
#ENDIF (MAF_USE_VTK)

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})

#IF (MAF_USE_VTK)
#  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_VTK_LIBRARY)
#ENDIF (MAF_USE_VTK)

IF (MAF_USE_WX)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_WXWIN_LIBRARY)
ENDIF (MAF_USE_WX)

# build tests
IF(BUILD_TESTING)
  OPTION(BUILD_Interaction_TESTING "Enable building of testing for Interaction classes" ON)
  IF (BUILD_Interaction_TESTING)
    SUBDIRS(Testing)
  ENDIF (BUILD_Interaction_TESTING)
ENDIF(BUILD_TESTING)
