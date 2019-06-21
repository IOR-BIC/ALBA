/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEItemVTKTest
 Authors: mgiacom
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaVMEItemVTKTest_H__
#define __CPP_UNIT_albaVMEItemVTKTest_H__

#include "albaTest.h"

/** Test for albaVMEItemVTK; Use this suite to trace memory problems */
class albaVMEItemVTKTest : public albaTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaVMEItemVTKTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestSetData );
  CPPUNIT_TEST( TestEquals );
  CPPUNIT_TEST( TestIsDataPresent );
  CPPUNIT_TEST( TestGetDataFileExtension );
  CPPUNIT_TEST( TestDeepCopy );
  CPPUNIT_TEST( TestShallowCopy );
  CPPUNIT_TEST( TestReadData );
  CPPUNIT_TEST( TestInternalStoreData );
  CPPUNIT_TEST( TestStoreToArchive );
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestSetData();
  void TestEquals();
  void TestIsDataPresent();
  void TestGetDataFileExtension();
  void TestDeepCopy();
  void TestShallowCopy();
  void TestReadData();
  void TestInternalStoreData();
  void TestStoreToArchive();

  bool result;
};

#endif
