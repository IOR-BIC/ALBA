/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpEditMetadataTest
 Authors: Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_albaOpEditMetadataTest_H
#define CPP_UNIT_albaOpEditMetadataTest_H

#include "albaTest.h"

class albaOpEditMetadataTest : public albaTest
{
public:

  CPPUNIT_TEST_SUITE( albaOpEditMetadataTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestConstructor );
  CPPUNIT_TEST( TestAddNewTag );
  CPPUNIT_TEST( TestRemoveTag );
  CPPUNIT_TEST( TestSetTagName );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestFixture();
  void TestConstructor();
  void TestAddNewTag();
  void TestRemoveTag();
  void TestSetTagName();
};

#endif
