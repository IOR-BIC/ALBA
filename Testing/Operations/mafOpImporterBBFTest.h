/*=========================================================================

 Program: MAF2
 Module: mafOpImporterBBFTest
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_mafOpImporterBBFTest_H
#define CPP_UNIT_mafOpImporterBBFTest_H

#include "mafTest.h"

class mafOpImporterBBFTest : public mafTest
{
  CPPUNIT_TEST_SUITE( mafOpImporterBBFTest );
  CPPUNIT_TEST( TestDynamicAllocation ); 
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestImport );
  CPPUNIT_TEST_SUITE_END();

  protected:
    void TestDynamicAllocation();
    void TestStaticAllocation();
    void TestImport();
};

#endif
