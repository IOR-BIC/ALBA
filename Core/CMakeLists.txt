#
# Copyright and description to be placed here
#

PROJECT(mafCore)
MESSAGE(STATUS "Configuring project mafCore")
# Set your list of sources here.  Do not change the name of the
# vtkLocal_SRCS variable.
SET(mafCore_SRCS
  mafString.cpp
  mafString.h
  mafDefines.h
)

# List the kits from VTK that are needed by this project.
SET(mafCore_LIBS
  vtkCommon
)

#-----------------------------------------------------------------------------
# Do not change anything below this line.

# Need to include class headers and the configuration header.
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR})

# Create the header to configure the classes.
CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/Configure.h.in
               ${PROJECT_BINARY_DIR}/Configure.h
               @ONLY IMMEDIATE)

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${mafCore_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${mafCore_LIBS})
# this is necessaty to force MFL compilation....
MESSAGE ("${PROJECT_NAME} dependencies: ${MFL_BUILD_TARGETS}")
ADD_DEPENDENCIES(${PROJECT_NAME} ${MFL_BUILD_TARGETS})

IF(BUILD_TESTING)
  SUBDIRS(Testing)
ENDIF(BUILD_TESTING)