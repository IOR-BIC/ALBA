#
# Program:   MULTIMOD APPLICATION FRAMEWORK (MAF)
# Module:    $RCSfile: CMakeLists.txt,v $
# Language:  CMake 1.2
# Date:      $Date: 2006-05-16 15:30:34 $
# Version:   $Revision: 1.2 $
#
# Description: MAF_DRIVERS Cmake project file.
#

PROJECT (MAF_DRIVERS)

INCLUDE_DIRECTORIES(${MAF_DRIVERS_BINARY_DIR})
INCLUDE_DIRECTORIES(${MAF_DRIVERS_SOURCE_DIR})

# link path (this is for binary drivers, like P5)
LINK_DIRECTORIES(${MAF_DRIVERS_BINARY_DIR}/lib)

#
# Source files
#
# Here is where you can add the name of your local common classes.
#

IF (WIN32)
SET (DRIVERS_SRCS
# Intersense driver
  isdriver.c
  isense.c
  itcom.c
  serial.c
  timer.c
  isdriver.h
  isense.h
  itcom.h
  serial.h
  timer.h
  wsock.c
  wsock.h
)
ENDIF (WIN32)

SET (DRIVERS_SRCS
  ${DRIVERS_SRCS}
  ClientUnit.cpp
  ClientUnit.h  
  )

IF (WIN32)
SET (DRIVERS_SRCS
  ${DRIVERS_SRCS}
  # Vicon driver
  vicon.cpp
  vicon.h
  )
ENDIF (WIN32)

# P5 driver
IF (MAF_USE_P5_GLOVE)
  SET (DRIVERS_SRCS 
  ${DRIVERS_SRCS}
  P5Bend.cpp
  P5Bend.h
  P5Motion.cpp
  P5Motion.h
  P5dll.h
  )
ENDIF (MAF_USE_P5_GLOVE)

#
# Create the mafDrivers library
#
ADD_LIBRARY (mafDrivers STATIC ${DRIVERS_SRCS})

# 
# Copy drivers libs to library and execute path
#
IF (MAF_USE_P5_GLOVE)
  FOREACH(config ${CMAKE_CONFIGURATION_TYPES})
    CONFIGURE_FILE(${MAF_DRIVERS_SOURCE_DIR}/P5DLL.lib ${EXECUTABLE_OUTPUT_PATH}/${config}/P5DLL.lib COPYONLY IMMEDIATE)
  ENDFOREACH(config)
  
  TARGET_LINK_LIBRARIES (mafDrivers P5DLL.lib)
ENDIF (MAF_USE_P5_GLOVE)
