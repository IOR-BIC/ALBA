/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMESlicerTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaVMESlicerTest_H__
#define __CPP_UNIT_albaVMESlicerTest_H__

#include "albaTest.h"

/** Test for albaMatrix; Use this suite to trace memory problems */
class albaVMESlicerTest : public albaTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaVMESlicerTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestBug2454And2524Regression );
  CPPUNIT_TEST_SUITE_END();

private:

  void TestFixture();
  
  void TestBug2454And2524Regression();
  
};

#endif

