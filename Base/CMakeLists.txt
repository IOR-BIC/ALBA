#
# Copyright and description to be placed here
#

PROJECT(mafBase)

SET(PROJECT_SRCS
  mafDefines.h
  mafDefines.cpp
  mafDirectory.cpp
  mafDirectory.h
  mafDynamicLoader.cpp
  mafDynamicLoader.h
  mafMutexLock.cpp
  mafMutexLock.h
  mafObject.cpp
  mafObject.h
  mafObjectFactory.cpp
  mafObjectFactory.h
  mafSmartObject.cpp
  mafSmartObject.h
  mafString.cpp
  mafString.h
  mafMTime.cpp
  mafMTime.h
  mafVersion.h
  mafWIN32.h
)

# List libraries that are needed by this project.
#SET(PROJECT_LIBS
#)

#-----------------------------------------------------------------------------
# Do not change anything below this line.

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
#TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
ADD_DEPENDENCIES(${PROJECT_NAME} ${MFL_BUILD_TARGETS})

IF(BUILD_TESTING)
  SUBDIRS(Testing)
ENDIF(BUILD_TESTING)