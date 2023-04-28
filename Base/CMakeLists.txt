#
# Copyright and description to be placed here
#

PROJECT(albaBase)

DBG_MESSAGE("Configuring project albaCore")

# Set your list of sources here.
SET(PROJECT_SRCS
albaBase.h
albaDecl.cpp				albaDecl.h
albaDbg.h
albaDataChecksum.h
albaDefines.cpp			albaDefines.h
albaDirectory.cpp		albaDirectory.h
albaDynamicLoader.cpp	albaDynamicLoader.h
albaEventBase.cpp		albaEventBase.h
albaEventSender.h
albaEventBroadcaster.cpp		albaEventBroadcaster.h
albaIncludeWIN32.h
albaIncludeWX.h
albaIndent.cpp			albaIndent.h
albaLUTLibrary.cpp		albaLUTLibrary.h
albaMTime.cpp			albaMTime.h
albaMatrix.cpp			albaMatrix.h
albaMatrix3x3.cpp		albaMatrix3x3.h
albaVect3d.cpp			albaVect3d.h
albaMultiThreader.cpp	albaMultiThreader.h
albaMutexLock.cpp		albaMutexLock.h
albaOBB.cpp				albaOBB.h
albaObject.cpp			albaObject.h
albaObjectFactory.cpp	albaObjectFactory.h
albaObserver.cpp			albaObserver.h
albaObserverCallback.cpp	albaObserverCallback.h
albaReferenceCounted.cpp	albaReferenceCounted.h
albaSmartPointer.h
albaString.cpp			albaString.h
albaTimeStamped.cpp		albaTimeStamped.h
albaTransform.cpp		albaTransform.h
albaTransformBase.cpp	albaTransformBase.h
albaTransformFrame.cpp	albaTransformFrame.h
albaVector.h
albaVersion.h
mmuIdFactory.cpp		mmuIdFactory.h
albaUtility.h
vtkALBASmartPointer.h
vtkALBAToLinearTransform.h
albaZipUtility.cpp		albaZipUtility.h
albaQuaternion.cpp	    albaQuaternion.h
)

IF (ALBA_USE_VTK)
  SET(PROJECT_SRCS
    ${PROJECT_SRCS}
    vtkALBAToLinearTransform.cxx		vtkALBAToLinearTransform.h
    vtkALBASmartPointer.h
  )
ENDIF (ALBA_USE_VTK)

# List libraries that are needed by this project.
#SET(PROJECT_LIBS "" )

IF (ALBA_USE_VTK)
  SET(PROJECT_LIBS ${PROJECT_LIBS} vtkCommon)
ENDIF (ALBA_USE_VTK)

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})

ADD_DEPENDENCIES(${PROJECT_NAME} albaService)

IF (ALBA_USE_VTK)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_VTK_LIBRARY)
ENDIF (ALBA_USE_VTK)

IF (ALBA_USE_WX)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_WXWIN_LIBRARY)
ENDIF (ALBA_USE_WX)
