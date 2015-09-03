/*=========================================================================

 Program: MAF2
 Module: mmaVolumeMaterialTest
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mmaVolumeMaterialTest_H__
#define __CPP_UNIT_mmaVolumeMaterialTest_H__

#include "mafTest.h"

/** Test for mmaVolumeMaterial; Use this suite to trace memory problems */
class mmaVolumeMaterialTest : public mafTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mmaVolumeMaterialTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestStaticAllocation);
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestDeepCopy);
  CPPUNIT_TEST(TestEquals);
  CPPUNIT_TEST(TestUpdateProp);
  CPPUNIT_TEST(TestUpdateFromTables);
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestStaticAllocation();
  void TestDynamicAllocation();
  void TestDeepCopy();
  void TestEquals();
  void TestUpdateProp();
  void TestUpdateFromTables();

  bool result;
};


#endif
