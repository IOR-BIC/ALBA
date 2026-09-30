#
# Copyright and description to be placed here
#

PROJECT(albaIO)

DBG_MESSAGE("Configuring project albaIO")

# Set your list of sources here.
SET(PROJECT_SRCS
  albaCurlUtility.cpp  			albaCurlUtility.h
  albaStorable.cpp  				albaStorable.h
  albaStorage.cpp  				albaStorage.h
  albaStorageElement.cpp  		albaStorageElement.h
  mmuDOMTreeErrorReporter.h  	mmuDOMTreeErrorReporter.cpp
)

# List libraries that are needed by this project.

IF(BUILD_SHARED_LIBS)
ELSE (BUILD_SHARED_LIBS)
  SET(PROJECT_LIBS albaBase)
ENDIF (BUILD_SHARED_LIBS)


# IO classes for XML related objects
IF (ALBA_USE_XercesC)
  SET(PROJECT_SRCS
    ${PROJECT_SRCS}
    albaXMLElement.cpp    		albaXMLElement.h
    albaXMLStorage.cpp    		albaXMLStorage.h
    albaXMLString.cpp    		albaXMLString.h
    mmuXMLDOM.h
    mmuXMLDOMElement.h
  )
  SET(PROJECT_LIBS ${PROJECT_LIBS} XercesC ${wxWidgets_LIBRARIES})
ENDIF (ALBA_USE_XercesC)

IF (ALBA_USE_VTK)
  SET(PROJECT_LIBS ${PROJECT_LIBS} ${VTK_LIBRARIES})
ENDIF (ALBA_USE_VTK)

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})

#IF (ALBA_USE_VTK)
#  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_VTK_LIBRARY)
#ENDIF (ALBA_USE_VTK)


ADD_DEPENDENCIES(${PROJECT_NAME} albaService)

IF (ALBA_USE_XercesC)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_XercesC_LIBRARY)
ENDIF (ALBA_USE_XercesC)

IF(BUILD_SHARED_LIBS)
  ADD_DEPENDENCIES(${PROJECT_NAME} albaBase) #substitute with  ${PROJECT_LIBS}   when external libraries are dynamic
ENDIF (BUILD_SHARED_LIBS)

