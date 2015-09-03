/*=========================================================================

 Program: MAF2
 Module: mafOpImporterC3DTest
 Authors: Simone Brazzale
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_mafOpImporterC3DTest_H
#define CPP_UNIT_mafOpImporterC3DTest_H

#include "mafTest.h"

class mafOpImporterC3DTest : public mafTest
{
  CPPUNIT_TEST_SUITE( mafOpImporterC3DTest );
  CPPUNIT_TEST( TestDynamicAllocation ); 
  CPPUNIT_TEST( TestCopy );
 
  CPPUNIT_TEST_SUITE_END();

  protected:
    void TestDynamicAllocation();
    void TestCopy();
};

#endif
