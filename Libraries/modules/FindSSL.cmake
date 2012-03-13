#
#
# Program:   MULTIMOD APPLICATION FRAMEWORK (MAF)
# Module:    $RCSfile: FindSSL.cmake,v $
# Language:  CMake 2.0
# Date:      $Date: 2007-01-23 13:34:36 $
# Version:   $Revision: 1.5 $
#
# Description: Find the OpenSSL location, Include headers and link libraries to MAF Project
# This module defines:
# SSL_INCULDE_DIR, where to find ".h"
# SSL_LIBRARIES, the librery to link


#----------------------------------------------------
# Find and link OpenSSL Lybrary
#----------------------------------------------------

FIND_LIBRARY (SSL_LIBRARIES
  NAMES ssleay32MT
  PATHS 
  "C:/OpenSSL/lib/VC"
  [HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\OpenSSL_is1;InstallLocation]\\lib\\VC
# It doesn't work, if user moved the installation directory
)

FIND_LIBRARY (LEY_LIBRARIES
  NAMES libeay32MT
  PATHS 
  "C:/OpenSSL/lib/VC"
  [HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\OpenSSL_is1;InstallLocation]\\lib\\VC
# It doesn't work, if user moved the installation directory
)



#-----------------------------------------------------
# Find and include ".h"
#-----------------------------------------------------
FIND_PATH(SSL_INCLUDE_DIR openssl/aes.h
  "C:/OpenSSL/include"
   [HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\OpenSSL_is1;InstallLocation]\\include
# It doesn't work, if user moved the installation directory
  )


IF (SSL_LIBRARIES AND SSL_INCLUDE_DIR)
  GET_FILENAME_COMPONENT(SSL_LIBRARY_NAME ${SSL_LIBRARIES} NAME CACHE)
  LINK_LIBRARIES(${SSL_LIBRARY_NAME})
  GET_FILENAME_COMPONENT(LEY_LIBRARY_NAME ${LEY_LIBRARIES} NAME CACHE)
  LINK_LIBRARIES(${LEY_LIBRARY_NAME})
  GET_FILENAME_COMPONENT(SSL_LINK_LIBRARIES_PATH ${SSL_LIBRARIES} PATH CACHE)
  LINK_DIRECTORIES(${SSL_LINK_LIBRARIES_PATH})
  INCLUDE_DIRECTORIES(${SSL_INCLUDE_DIR})
  MESSAGE (STATUS "Include dir: ${SSL_INCLUDE_DIR}")
  SET(SSL_FOUND 1 CACHE BOOL "SSL Library Found" FORCE)
ENDIF (SSL_LIBRARIES AND SSL_INCLUDE_DIR)

IF (SSL_FOUND)
  MESSAGE(STATUS "Found OpenSSL: ${SSL_LIBRARIES}")
ELSE (SSL_FOUND)
  MESSAGE(STATUS "Could not find OpenSSL")
ENDIF (SSL_FOUND)
