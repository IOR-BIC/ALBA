/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaMatrixPipeTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaMatrixPipeTest_H__
#define __CPP_UNIT_albaMatrixPipeTest_H__

#include "albaTest.h"

//forward reference
class albaMatrixPipe;
class albaVMESurface;

/** Test for albaMatrix; Use this suite to trace memory problems */
class albaMatrixPipeTest : public albaTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaMatrixPipeTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestDynamicAllocation ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestStaticAllocation ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestUpdateMatrixObserverSet_Get_On_Off );
  CPPUNIT_TEST( TestSetGetVME );
  CPPUNIT_TEST( TestSetGetTimeStamp );
  CPPUNIT_TEST( TestGetMTime );
  CPPUNIT_TEST( TestAccept );
  CPPUNIT_TEST( TestGetMatrix );
  CPPUNIT_TEST( TestMakeACopy );
  CPPUNIT_TEST( TestDeepCopy );
  CPPUNIT_TEST( TestUpdate );

  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestDynamicAllocation();
  void TestStaticAllocation();
  void TestUpdateMatrixObserverSet_Get_On_Off();
  void TestSetGetVME();
  void TestSetGetTimeStamp();
  void TestGetMTime();
  void TestAccept();
  void TestGetMatrix();
  void TestMakeACopy();
  void TestDeepCopy();
  void TestUpdate();

  albaVMESurface *m_SurfaceTest;
  bool result;
};

#endif
