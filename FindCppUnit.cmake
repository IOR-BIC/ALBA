
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
  ${MED_SOURCE_DIR}/../cppunit-1.12.0/include
  ${MED_SOURCE_DIR}/../../cppunit-1.12.0/include
)

FIND_LIBRARY(CPPUNIT_LIBRARY cppunitd
  /home/erc/lib
  /usr/local/lib
  /usr/lib
  ${MED_SOURCE_DIR}/../cppunit-1.12.0/lib
  ${MED_SOURCE_DIR}/../../cppunit-1.12.0/lib
)

IF(CPPUNIT_INCLUDE_DIR)
  IF(CPPUNIT_LIBRARY)
    SET(CPPUNIT_FOUND "YES")
    SET(CPPUNIT_LIBRARIES ${CPPUNIT_LIBRARY} ${CMAKE_DL_LIBS})
  ENDIF(CPPUNIT_LIBRARY)
ENDIF(CPPUNIT_INCLUDE_DIR)

