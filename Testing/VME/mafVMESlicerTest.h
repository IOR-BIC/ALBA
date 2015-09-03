/*=========================================================================

 Program: MAF2
 Module: mafVMESlicerTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafVMESlicerTest_H__
#define __CPP_UNIT_mafVMESlicerTest_H__

#include "mafTest.h"

/** Test for mafMatrix; Use this suite to trace memory problems */
class mafVMESlicerTest : public mafTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafVMESlicerTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestBug2454And2524Regression );
  CPPUNIT_TEST_SUITE_END();

private:

  void TestFixture();
  
  // http://bugzilla.b3c.it/show_bug.cgi?id=2454
  void TestBug2454And2524Regression();
  
};

#endif

