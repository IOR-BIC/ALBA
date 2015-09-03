/*=========================================================================

 Program: MAF2
 Module: mafOpImporterAnalogWSTest
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_mafOpImporterAnalogWSTest_H
#define CPP_UNIT_mafOpImporterAnalogWSTest_H

#include "mafTest.h"

class mafOpImporterAnalogWSTest : public mafTest
{
  CPPUNIT_TEST_SUITE( mafOpImporterAnalogWSTest );
  CPPUNIT_TEST( TestDynamicAllocation ); 
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestRead );
  CPPUNIT_TEST_SUITE_END();

  protected:
    void TestDynamicAllocation();
    void TestStaticAllocation();
    void TestRead();
};

#endif
