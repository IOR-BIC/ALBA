/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpApplyTrajectoryTest
 Authors: Roberto Mucci, Simone Brazzale
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaOpApplyTrajectoryTest_H__
#define __CPP_UNIT_albaOpApplyTrajectoryTest_H__

#include "albaTest.h"

/** Test for albaOpApplyTrajectory; Use this suite to trace memory problems */
class albaOpApplyTrajectoryTest : public albaTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaOpApplyTrajectoryTest );
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
