/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExtractIsosurfaceTest
 Authors: Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaOpExtractIsosurfaceTest_H__
#define __CPP_UNIT_albaOpExtractIsosurfaceTest_H__

#include "albaTest.h"

/** Test for albaOpApplyTrajectory; Use this suite to trace memory problems */
class albaOpExtractIsosurfaceTest : public albaTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaOpExtractIsosurfaceTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestExtractRG);
  CPPUNIT_TEST(TestExtractSP);
	CPPUNIT_TEST(TestExtractConnectivity);
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestDynamicAllocation();
  void TestExtractRG();
  void TestExtractSP();
	void TestExtractConnectivity();

  bool result;
};

#endif
