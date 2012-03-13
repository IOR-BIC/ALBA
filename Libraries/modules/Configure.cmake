#
# Program:   MULTIMOD APPLICATION FRAMEWORK (MAF)
# Module:    $RCSfile: Configure.cmake,v $
# Language:  CMake 1.2
# Date:      $Date: 2007-01-18 14:29:46 $
# Version:   $Revision: 1.5 $
#
# Description:
# Project file for configuring MFL modules

# Create module files in build directory
CONFIGURE_FILE(${MFL_MODULES_SOURCE_DIR}/FindITK.cmake.in ${MFL_MODULES_BINARY_DIR}/FindITK.cmake IMMEDIATE)
CONFIGURE_FILE(${MFL_MODULES_SOURCE_DIR}/FindVTK.cmake.in ${MFL_MODULES_BINARY_DIR}/FindVTK.cmake IMMEDIATE)
CONFIGURE_FILE(${MFL_MODULES_SOURCE_DIR}/FindVCollide.cmake.in ${MFL_MODULES_BINARY_DIR}/FindVCollide.cmake IMMEDIATE)
CONFIGURE_FILE(${MFL_MODULES_SOURCE_DIR}/FindSSL.cmake ${MFL_MODULES_BINARY_DIR}/FindSSL.cmake COPYONLY IMMEDIATE)
#SIL ------  CONFIGURE_FILE(${MFL_MODULES_SOURCE_DIR}/FindWX.cmake.in ${MFL_MODULES_BINARY_DIR}/FindWX.cmake COPYONLY IMMEDIATE)
#SIL ------  CONFIGURE_FILE(${MFL_MODULES_SOURCE_DIR}/UseWX.cmake.in ${MFL_MODULES_BINARY_DIR}/UseWX.cmake COPYONLY IMMEDIATE)
