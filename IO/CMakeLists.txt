#
# Copyright and description to be placed here
#

PROJECT(mafIO)

DBG_MESSAGE("Configuring project mafIO")

# Set your list of sources here.
SET(PROJECT_SRCS
  mafCurlUtility.cpp
  mafCurlUtility.h
  mafStorable.cpp
  mafStorable.h
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

IF(BUILD_SHARED_LIBS)
ELSE (BUILD_SHARED_LIBS)
  SET(PROJECT_LIBS mafBase)
ENDIF (BUILD_SHARED_LIBS)


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
    mmuXMLDOMElement.h
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
  # --- va messo solo se wx e' compilato internamente --- ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_WXWIN_LIBRARY)
#ENDIF (MAF_USE_WX)

IF (MAF_USE_XercesC)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_XercesC_LIBRARY)
ENDIF (MAF_USE_XercesC)

IF(BUILD_SHARED_LIBS)
  ADD_DEPENDENCIES(${PROJECT_NAME} mafBase) #substitute with  ${PROJECT_LIBS}   when external libraries are dynamic
ENDIF (BUILD_SHARED_LIBS)

