/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDataVectorTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaDataVectorTest_H__
#define __CPP_UNIT_albaDataVectorTest_H__

#include "albaTest.h"

/** Test for albaMatrix; Use this suite to trace memory problems */
class albaDataVectorTest : public albaTest
{
public: 

	// CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaDataVectorTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestSetSingleFileMode );
  CPPUNIT_TEST( TestAppendItem );
  CPPUNIT_TEST( TestPrependItem );
  CPPUNIT_TEST( TestInsertItem );
  CPPUNIT_TEST( TestModified );
  CPPUNIT_TEST( TestSetCrypting );
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestStaticAllocation();
  void TestDynamicAllocation();
  void TestSetSingleFileMode();
  void TestAppendItem();
  void TestPrependItem();
  void TestInsertItem();
  void TestModified();
  void TestSetCrypting();
};

#endif
