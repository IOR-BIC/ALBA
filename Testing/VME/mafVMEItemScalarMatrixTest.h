/*=========================================================================

 Program: MAF2
 Module: mafVMEItemScalarMatrixTest
 Authors: Alberto Losi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafVMEItemScalarMatrixTest_H__
#define __CPP_UNIT_mafVMEItemScalarMatrixTest_H__

#include "mafTest.h"

/** Test for mafVMEItemScalarMatrixTest; Use this suite to trace memory problems */
class mafVMEItemScalarMatrixTest : public mafTest
{
public: 
  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafVMEItemScalarMatrixTest );
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
