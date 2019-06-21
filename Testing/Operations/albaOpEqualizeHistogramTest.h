/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpEqualizeHistogramTest
 Authors: Eleonora Mambrini
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_albaOpOpEqualizeHistogramTest_H
#define CPP_UNIT_albaOpOpEqualizeHistogramTest_H

#include "albaTest.h"

class albaOpEqualizeHistogramTest : public albaTest
{
  CPPUNIT_TEST_SUITE( albaOpEqualizeHistogramTest );
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
