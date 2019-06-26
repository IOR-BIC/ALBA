/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpVOIDensityEditorTestTest
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaOpVOIDensityEditorTestTest_H__
#define __CPP_UNIT_albaOpVOIDensityEditorTestTest_H__

#include "albaTest.h"

/** Test for albaOpVOIDensityEditorTest; Use this suite to trace memory problems */
class albaOpVOIDensityEditorTestTest : public albaTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaOpVOIDensityEditorTestTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(DynamicAllocationTest);
  CPPUNIT_TEST(EditVolumeScalarsTest);

  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void DynamicAllocationTest();
  void EditVolumeScalarsTest();

  bool result;
};

#endif
