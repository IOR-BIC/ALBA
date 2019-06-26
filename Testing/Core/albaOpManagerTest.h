/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpManagerTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaOpManagerTest_h__
#define __CPP_UNIT_albaOpManagerTest_h__

#include "albaTest.h"

#include <wx/config.h>
#include <wx/fileconf.h>

class albaOpManager;

/** albaOpManagerTest:
Test class for albaOpManager
*/
//------------------------------------------------------------------------------
class albaOpManagerTest: public albaTest
  //------------------------------------------------------------------------------
{

public:

  /** Executed before each test.*/
  void BeforeTest();
  /* Executed after each test.*/
  void AfterTest();
  /** Test suite begin. */
  CPPUNIT_TEST_SUITE( albaOpManagerTest );

  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestOpAdd );
  CPPUNIT_TEST( TestVmeSelected );
  CPPUNIT_TEST( TestOpRun1 );
  CPPUNIT_TEST( TestOpRun2 );
  CPPUNIT_TEST( TestOpRun3 );
  CPPUNIT_TEST( TestOpExec );
  CPPUNIT_TEST( TestOpSelect );
  CPPUNIT_TEST( TestRunning );
  CPPUNIT_TEST( TestClearUndoStack );
  CPPUNIT_TEST( TestForceStopWithOk );
  CPPUNIT_TEST( TestForceStopWithCancel );
  CPPUNIT_TEST( TestStopCurrentOperation );
  CPPUNIT_TEST( TestGetRunningOperation );
  CPPUNIT_TEST( TestGetOperationById );
  CPPUNIT_TEST( TestSetMafUser );
  
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestStaticAllocation();
  void TestDynamicAllocation();
  void TestOpAdd();
  void TestVmeSelected();
  void TestOpRun1();
  void TestOpRun2();
  void TestOpRun3();
  void TestOpExec();
  void TestOpSelect();
  void TestRunning();
  void TestClearUndoStack();
  void TestForceStopWithOk();
  void TestForceStopWithCancel();
  void TestStopCurrentOperation();
  void TestGetRunningOperation();
  void TestGetOperationById();
  void TestSetMafUser();

  albaOpManager *m_OpManager;
};


#endif // #ifndef __CPP_UNIT_albaOpManagerTest_h__