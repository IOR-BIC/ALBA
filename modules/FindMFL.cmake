#
# Program:   MULTIMOD APPLICATION FRAMEWORK (MAF)
# Module:    $RCSfile: FindMFL.cmake,v $
# Language:  CMake 2.0
# Date:      $Date: 2004-12-22 10:14:50 $
# Version:   $Revision: 1.2 $
#
# Description: Find the MFL location. MFL package can be shared by two
# different MAF distributions.
# This module defines
#
# USE_MFL_FILE - the full path and location of the UseVTK.cmake file
# MFL_FOUND - true if MFL was found
#

#
# Look for a MFL binary tree
# 

MESSAGE (STATUS "FindMFL: searching MFL binaries outside MAF tree")
# If not built within MAF project try standard places

# Get the system search path as a list.
IF(UNIX)
  STRING(REGEX MATCHALL "[^:]+" MFL_DIR_SEARCH1 "$ENV{PATH}")
ELSE(UNIX)
  STRING(REGEX REPLACE "\\\\" "/" MFL_DIR_SEARCH1 "$ENV{PATH}")
ENDIF(UNIX)
STRING(REGEX REPLACE "/;" ";" MFL_DIR_SEARCH2 "${MFL_DIR_SEARCH1}")

# Construct a set of paths relative to the system search path.
SET(MFL_DIR_SEARCH "")
FOREACH(dir ${MFL_DIR_SEARCH2})
  SET(MFL_DIR_SEARCH ${MFL_DIR_SEARCH} "${dir}/../lib/Libraries")
ENDFOREACH(dir)

#
# Look for an installation or build tree.
#
FIND_PATH(MFL_BINARY_PATH UseMFL.cmake

  # Look in places relative to the system executable search path.
  ${MFL_DIR_SEARCH}

  # Look in standard UNIX install locations.
  /usr/local/lib/Libraries
  /usr/lib/Libraries

  # Read from the CMakeSetup registry entries.  It is likely that
  # MFL will have been recently built.
  [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild1]
  [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild2]
  [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild3]
  [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild4]
  [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild5]
  [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild6]
  [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild7]
  [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild8]
  [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild9]
  [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild10]
  [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild1]\\Libraries
  [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild2]\\Libraries
  [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild3]\\Libraries
  [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild4]\\Libraries
  [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild5]\\Libraries
  [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild6]\\Libraries
  [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild7]\\Libraries
  [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild8]\\Libraries
  [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild9]\\Libraries
  [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild10]\\Libraries
  ../Libraries

  # Help the user find it if we cannot.
  DOC "The directory containing UseMFL.cmake.  This is either the root of the build tree, or PREFIX/lib/Libraries for an installation."
)

## TODO: Load cache information
IF (MFL_BINARY_PATH)
    FIND_FILE(USE_MFL_FILE UseMFL.cmake ${MFL_BINARY_PATH})  
ENDIF (MFL_BINARY_PATH)

IF (USE_MFL_FILE)
  SET (MFL_FOUND 1 CACHE INTERNAL "Found MFL library")
ENDIF (USE_MFL_FILE)


#MARK_AS_ADVANCED(
#  USE_MFL_FILE
#)