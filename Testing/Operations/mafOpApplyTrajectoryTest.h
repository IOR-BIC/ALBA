/*=========================================================================

 Program: MAF2
 Module: mafOpApplyTrajectoryTest
 Authors: Roberto Mucci, Simone Brazzale
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafOpApplyTrajectoryTest_H__
#define __CPP_UNIT_mafOpApplyTrajectoryTest_H__

#include "mafTest.h"

/** Test for mafOpApplyTrajectory; Use this suite to trace memory problems */
class mafOpApplyTrajectoryTest : public mafTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafOpApplyTrajectoryTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestFromFile);
  CPPUNIT_TEST(TestFromVME);
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestDynamicAllocation();
  void TestFromFile();
  void TestFromVME();

  bool result;

};

#endif
