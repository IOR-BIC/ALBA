/*=========================================================================

 Program: MAF2
 Module: mafGizmoPathTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafGizmoPathTest_H__
#define __CPP_UNIT_mafGizmoPathTest_H__

#include "mafTest.h"
#include "mafVMEPolyline.h"
#include "mafVMERoot.h"

class mafGizmoPathTest : public mafTest
{
  public:
  
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( mafGizmoPathTest );
  CPPUNIT_TEST( TestFixture );
  CPPUNIT_TEST( TestConstructor );
  CPPUNIT_TEST( TestSetCurvilinearAbscissa );
  CPPUNIT_TEST_SUITE_END();

  protected:
    
    void TestFixture();
    void TestConstructor();
    void TestSetCurvilinearAbscissa();
	  void CreateTestData();

    mafVMEPolyline *m_TestPolyline;
    mafVMERoot *m_Root;

};

#endif
