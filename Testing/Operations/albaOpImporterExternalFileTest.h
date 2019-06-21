/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterExternalFileTest
 Authors: Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_albaOpImporterExternalFileTest_H
#define CPP_UNIT_albaOpImporterExternalFileTest_H

#include "albaTest.h"

class albaOpImporterExternalFileTest : public albaTest
{
public:

  CPPUNIT_TEST_SUITE( albaOpImporterExternalFileTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestConstructor );
  CPPUNIT_TEST( TestImporter );
  CPPUNIT_TEST_SUITE_END();

  protected:
   
    void TestFixture();
    void TestConstructor();
    void TestImporter();
};

#endif
