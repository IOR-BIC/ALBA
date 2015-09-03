/*=========================================================================

 Program: MAF2
 Module: mafMatrixPipeTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafMatrixPipeTest_H__
#define __CPP_UNIT_mafMatrixPipeTest_H__

#include "mafTest.h"

//forward reference
class mafMatrixPipe;
class mafVMESurface;

/** Test for mafMatrix; Use this suite to trace memory problems */
class mafMatrixPipeTest : public mafTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafMatrixPipeTest );
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

  mafVMESurface *m_SurfaceTest;
  bool result;
};

#endif
