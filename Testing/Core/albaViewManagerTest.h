/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewManagerTest
 Authors: Alberto Losi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaViewManagerTest_H__
#define __CPP_UNIT_albaViewManagerTest_H__

#include "albaTest.h"

class albaViewManager;

/** albaViewManagerTest:
Test class for albaViewManager
*/
class albaViewManagerTest : public albaTest, public albaObserver
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaViewManagerTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( VmeAddTest );
  CPPUNIT_TEST( VmeRemoveTest );
  CPPUNIT_TEST( VmeSelectTest );
  CPPUNIT_TEST( VmeShowTest );
  CPPUNIT_TEST( ViewAddTest );
  CPPUNIT_TEST( ViewSelectedGetSelectedViewTest );
  CPPUNIT_TEST( ViewCreateTest );
  CPPUNIT_TEST( ViewInsertTest );
  CPPUNIT_TEST( ViewDeleteTest );
  CPPUNIT_TEST( CameraResetTest );   
  CPPUNIT_TEST( CameraUpdateTest );
  CPPUNIT_TEST( PropertyUpdateTest );
  CPPUNIT_TEST( GetCurrentRootTest );
  CPPUNIT_TEST( GetListTest );
  CPPUNIT_TEST( GetListTemplateTest );
  CPPUNIT_TEST( GetViewTest );
  CPPUNIT_TEST( SetMouseTest );
  CPPUNIT_TEST( GetFromListTest );
  //CPPUNIT_TEST( VmeModifiedTest ); EMPTY METHOD not tested
  //CPPUNIT_TEST( CameraFlyToModeTest ); EMPTY METHOD not tested
  //CPPUNIT_TEST( OnQuit ); EMPTY METHOD not tested
  CPPUNIT_TEST_SUITE_END();

private:

  void TestFixture();
  void TestDynamicAllocation();
  void TestStaticAllocation();
  void VmeAddTest();
  void VmeRemoveTest();
  void VmeSelectTest();
  void VmeShowTest();
  void ViewAddTest();
  void ViewSelectedGetSelectedViewTest();
  void ViewCreateTest();
  void ViewInsertTest();
  void ViewDeleteTest();
  void CameraResetTest();
  void CameraUpdateTest();
  void PropertyUpdateTest();
  void GetCurrentRootTest();
  void GetListTest();
  void GetListTemplateTest();
  void GetViewTest();
  void SetMouseTest();
  void GetFromListTest();

  void OnEvent(albaEventBase *alba_event); // This method trap the events coming from the view manager

  int m_EventResult;
  albaViewManager *m_Manager;

};


#endif

