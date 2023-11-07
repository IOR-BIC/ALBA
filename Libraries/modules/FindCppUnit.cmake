
# Find the CppUnit includes and library
#
# This module defines
# CPPUNIT_INCLUDE_DIR, where to find cppunit include files, etc.
# CPPUNIT_LIBRARIES, the libraries to link against to use CppUnit.
# CPPUNIT_FOUND, If false, do not try to use CppUnit.

# also defined, but not for general use are
# CPPUNIT_LIBRARY, where to find the CppUnit library.

#MESSAGE("Searching for cppunit library ")

FIND_PATH(CPPUNIT_INCLUDE_DIR cppunit/TestCase.h
  /home/erc/include
  /usr/local/include
  /usr/include
  ${ALBA_SOURCE_DIR}/../cppunit-1.12.0/include
  ${ALBA_SOURCE_DIR}/../../cppunit-1.12.0/include
)

FIND_LIBRARY(CPPUNIT_LIBRARY cppunit
  /home/erc/lib
  /usr/local/lib
  /usr/lib
  ${ALBA_SOURCE_DIR}/../cppunit-1.12.0/lib
  ${ALBA_SOURCE_DIR}/../../cppunit-1.12.0/lib
)

FIND_LIBRARY(CPPUNIT_DEBUG_LIBRARY cppunitd
  /home/erc/lib
  /usr/local/lib
  /usr/lib
  ${ALBA_SOURCE_DIR}/../cppunit-1.12.0/lib
  ${ALBA_SOURCE_DIR}/../../cppunit-1.12.0/lib
)

IF(CPPUNIT_INCLUDE_DIR)
  
  IF(CPPUNIT_LIBRARY)
    SET(CPPUNIT_RELEASE_FOUND "YES")
    SET(CPPUNIT_LIBRARIES ${CPPUNIT_LIBRARY} ${CMAKE_DL_LIBS})
  ENDIF(CPPUNIT_LIBRARY)
  
  IF(CPPUNIT_DEBUG_LIBRARIES)
    SET(CPPUNIT_DEBUG_FOUND "YES")
    SET(CPPUNIT_DEBUG_LIBRARIES ${CPPUNIT_LIBRARY} ${CMAKE_DL_LIBS})
  ENDIF(CPPUNIT_DEBUG_LIBRARIES)

ENDIF(CPPUNIT_INCLUDE_DIR)

