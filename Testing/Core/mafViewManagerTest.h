/*=========================================================================

 Program: MAF2
 Module: mafViewManagerTest
 Authors: Alberto Losi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafViewManagerTest_H__
#define __CPP_UNIT_mafViewManagerTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class mafViewManager;

/** mafViewManagerTest:
Test class for mafViewManager
*/
class mafViewManagerTest : public CPPUNIT_NS::TestFixture, public mafObserver
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafViewManagerTest );
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
  CPPUNIT_TEST( CollaborateTest );
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
  void CollaborateTest();
  void GetFromListTest();

  void OnEvent(mafEventBase *maf_event); // This method trap the events coming from the view manager

  int m_EventResult;
  mafViewManager *m_Manager;

};


#endif

