/*=========================================================================

 Program: MAF2
 Module: mafOpEqualizeHistogramTest
 Authors: Eleonora Mambrini
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_mafOpOpEqualizeHistogramTest_H
#define CPP_UNIT_mafOpOpEqualizeHistogramTest_H

#include "mafTest.h"

class mafOpEqualizeHistogramTest : public mafTest
{
  CPPUNIT_TEST_SUITE( mafOpEqualizeHistogramTest );
  CPPUNIT_TEST( TestDynamicAllocation ); 
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestAccept );
  CPPUNIT_TEST( TestOpUndo );
  CPPUNIT_TEST( TestOpExecute );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestStaticAllocation();
  void TestAccept();
  void TestOpUndo();
  void TestOpExecute();

};

#endif
