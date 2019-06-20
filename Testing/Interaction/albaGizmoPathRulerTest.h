/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoPathRulerTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaGizmoPathRulerTest_H__
#define __CPP_UNIT_albaGizmoPathRulerTest_H__

#include "albaTest.h"
#include "albaVMEPolyline.h"
#include "albaVMERoot.h"

class albaGizmoPathRulerTest : public albaTest
{
  public:
  
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( albaGizmoPathRulerTest );
  CPPUNIT_TEST( TestFixture );
  CPPUNIT_TEST( TestConstructor );
  CPPUNIT_TEST( TestSetCurvilinearAbscissa );
  CPPUNIT_TEST_SUITE_END();

  protected:
    
    void TestFixture();
    void TestConstructor();
    void TestSetCurvilinearAbscissa();
	  void CreateTestData();

    albaVMEPolyline *m_TestPolyline;
    albaVMERoot *m_Root;

};

#endif
