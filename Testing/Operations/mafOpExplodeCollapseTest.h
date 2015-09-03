/*=========================================================================

 Program: MAF2
 Module: mafOpExplodeCollapseTest
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafOpExplodeCollapseTest_H__
#define __CPP_UNIT_mafOpExplodeCollapseTest_H__

#include "mafTest.h"

/** Test for mafOpExplodeCollapse; Use this suite to trace memory problems */
class mafOpExplodeCollapseTest : public mafTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafOpExplodeCollapseTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  
  //ToDo: add your test here...
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestExplodeCollapse);
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  
  // ToDo: add your test methods here...
  void TestDynamicAllocation();
  void TestExplodeCollapse();


  bool result;

};

#endif
