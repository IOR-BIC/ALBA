/*=========================================================================

 Program: MAF2
 Module: mafOpCreateWrappedMeterTest
 Authors: Eleonora Mambrini
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_mafOpCreateWrappedMeterTest_H
#define CPP_UNIT_mafOpCreateWrappedMeterTest_H

#include "mafTest.h"

class mafOpCreateWrappedMeterTest : public mafTest
{
  CPPUNIT_TEST_SUITE( mafOpCreateWrappedMeterTest );
  CPPUNIT_TEST( TestDynamicAllocation ); 
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestAccept );
  CPPUNIT_TEST( TestOpExecute );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestStaticAllocation();
  void TestAccept();
  void TestOpExecute();

};

#endif
