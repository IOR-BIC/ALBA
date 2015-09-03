/*=========================================================================

 Program: MAF2
 Module: mafOpCreateVolumeTest
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafOpCreateVolumeTest_H__
#define __CPP_UNIT_mafOpCreateVolumeTest_H__

#include "mafTest.h"

/** Test for mafOpCreateVolume; Use this suite to trace memory problems */
class mafOpCreateVolumeTest : public mafTest
{
public: 

	// CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafOpCreateVolumeTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(DynamicAllocationTest);
  CPPUNIT_TEST(VolumeParametersTest);
  CPPUNIT_TEST(VolumeCreatedTest);
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void DynamicAllocationTest();
  void VolumeParametersTest();
  void VolumeCreatedTest();

  bool result;
};

#endif
