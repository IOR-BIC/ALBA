/*=========================================================================

 Program: MAF2
 Module: mafOpOpenExternalFileTest
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_MAFOPOPENEXTERNALFILETEST_H
#define CPP_UNIT_MAFOPOPENEXTERNALFILETEST_H

#include "mafTest.h"

class mafOpOpenExternalFileTest : public mafTest
{
public:


  CPPUNIT_TEST_SUITE( mafOpOpenExternalFileTest );
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
