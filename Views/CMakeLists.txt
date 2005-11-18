#
# Copyright and description to be placed here
#

PROJECT(mafViews)

DBG_MESSAGE("Configuring project mafViews")

# Set your list of sources here.
SET(PROJECT_SRCS
  mafView.cpp
  mafView.h
)

IF (MAF_USE_VTK)
  SET(PROJECT_SRCS ${PROJECT_SRCS}
    mafRWI.cpp
    mafRWI.h
    mafRWIBase.cpp
    mafRWIBase.h
    mafSceneGraph.cpp
    mafSceneGraph.h
    mafSceneNode.cpp
    mafSceneNode.h
    mafViewCompound.cpp
    mafViewCompound.h
    mafViewVTK.cpp
    mafViewVTK.h
    mmgContextualMenu.cpp
    mmgContextualMenu.h
    mmgTreeContextualMenu.cpp
    mmgTreeContextualMenu.h
    vtkMAFAssembly.cpp
    vtkMAFAssembly.h
  )
ENDIF (MAF_USE_VTK)


# List libraries that are needed by this project.
SET(PROJECT_LIBS mafGui)

IF (MAF_USE_VTK)
  SET(PROJECT_LIBS ${PROJECT_LIBS} vtkCommon vtkMAF)
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

