#
# Program:   ALBA (Agile Library for Biomedical Applications)
# Module:    $RCSfile: CMakeLists.txt,v $
# Language:  CMake 1.2
# Date:      $Date: 2008-02-27 17:19:45 $
# Version:   $Revision: 1.3 $
#
# Description: ALBA_DRIVERS Cmake project file.
#

PROJECT (ALBA_DRIVERS)

INCLUDE_DIRECTORIES(${ALBA_DRIVERS_BINARY_DIR})
INCLUDE_DIRECTORIES(${ALBA_DRIVERS_SOURCE_DIR})

# link path (this is for binary drivers, like P5)
LINK_DIRECTORIES(${ALBA_DRIVERS_BINARY_DIR}/lib)

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
)
ENDIF (WIN32)

IF (WIN32)
SET (DRIVERS_SRCS
  ${DRIVERS_SRCS}
  # Vicon driver
  vicon.cpp
  vicon.h
  )
ENDIF (WIN32)

# P5 driver
IF (ALBA_USE_P5_GLOVE)
  SET (DRIVERS_SRCS 
  ${DRIVERS_SRCS}
  P5Bend.cpp
  P5Bend.h
  P5Motion.cpp
  P5Motion.h
  P5dll.h
  )
ENDIF (ALBA_USE_P5_GLOVE)

IF (ALBA_USE_WIIMOTE)
  SET (DRIVERS_SRCS 
  ${DRIVERS_SRCS}
  defines.h
  Console.h
  Console.cpp
  HIDDevice.h
  HIDDevice.cpp
  hidpi.h
  hidsdi.h
  hidusage.h
  setupapi.h
  Utils.h
  Wiimote.h
  Wiimote.cpp
  )
ENDIF (ALBA_USE_WIIMOTE)

#
# Create the albaDrivers library
#
ADD_LIBRARY (albaDrivers STATIC ${DRIVERS_SRCS})

# 
# Copy drivers libs to library and execute path
#
IF (ALBA_USE_P5_GLOVE)
  FOREACH(config ${CMAKE_CONFIGURATION_TYPES})
    CONFIGURE_FILE(${ALBA_DRIVERS_SOURCE_DIR}/P5DLL.lib ${EXECUTABLE_OUTPUT_PATH}/${config}/P5DLL.lib COPYONLY IMMEDIATE)
  ENDFOREACH(config)
  
  TARGET_LINK_LIBRARIES (albaDrivers P5DLL.lib)
ENDIF (ALBA_USE_P5_GLOVE)

IF (ALBA_USE_WIIMOTE)
  FOREACH(config ${CMAKE_CONFIGURATION_TYPES})
    CONFIGURE_FILE(${ALBA_DRIVERS_SOURCE_DIR}/hid.lib ${EXECUTABLE_OUTPUT_PATH}/${config}/hid.lib COPYONLY IMMEDIATE)
    CONFIGURE_FILE(${ALBA_DRIVERS_SOURCE_DIR}/setupapi.lib ${EXECUTABLE_OUTPUT_PATH}/${config}/setupapi.lib COPYONLY IMMEDIATE)
  ENDFOREACH(config)
  
  TARGET_LINK_LIBRARIES (albaDrivers hid.lib setupapi.lib)
ENDIF (ALBA_USE_WIIMOTE)
