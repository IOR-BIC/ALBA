#
# Copyright and description to be placed here
#

PROJECT(mafBase)

DBG_MESSAGE("Configuring project mafCore")

# Set your list of sources here.
SET(PROJECT_SRCS
mafBase.h
mafDecl.cpp
mafDecl.h
mafDefines.cpp
mafDefines.h
mafDirectory.cpp
mafDirectory.h
mafDynamicLoader.cpp
mafDynamicLoader.h
mafEvent.cpp
mafEvent.h
mafEventBase.cpp
mafEventBase.h
mafEventSender.h
mafEventSource.cpp
mafEventSource.h
mafIncludeWIN32.h
mafIncludeWX.h
mafIndent.cpp
mafIndent.h
mafMTime.cpp
mafMTime.h
mafMatrix.cpp
mafMatrix.h
mafMatrix3x3.cpp
mafMatrix3x3.h
mafMutexLock.cpp
mafMutexLock.h
mafOBB.cpp
mafOBB.h
mafObject.cpp
mafObject.h
mafObjectFactory.cpp
mafObjectFactory.h
mafObserver.cpp
mafObserver.h
mafObserverCallback.cpp
mafObserverCallback.h
mafReferenceCounted.cpp
mafReferenceCounted.h
mafSmartPointer.h
mafString.cpp
mafString.h
mafTimeStamped.h
mafTransform.cpp
mafTransform.h
mafTransformBase.cpp
mafTransformBase.h
mafTransformFrame.cpp
mafTransformFrame.h
mafVector.h
mafVersion.h
mmuIdFactory.cpp
mmuIdFactory.h
mmuUtility.h
vtkMAFSmartPointer.h
vtkMAFToLinearTransform.h
)

IF (MAF_USE_VTK)
  SET(PROJECT_SRCS
    ${PROJECT_SRCS}
    vtkMAFToLinearTransform.cxx
    vtkMAFToLinearTransform.h
    vtkMAFSmartPointer.h
  )
ENDIF (MAF_USE_VTK)


# List libraries that are needed by this project.
#SET(PROJECT_LIBS "" )

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

# build tests
IF(BUILD_TESTING)
  SUBDIRS(Testing)
ENDIF(BUILD_TESTING)
