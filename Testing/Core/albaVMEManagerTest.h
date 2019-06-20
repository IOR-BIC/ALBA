/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEManagerTest
 Authors: Alberto Losi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaVMEManager_H__
#define __CPP_UNIT_albaVMEManager_H__

#include "albaTest.h"

#include <wx/config.h>
#include <wx/fileconf.h>

class albaVMEManager;

/** albaVMEManagerTest:
Test class for albaVMEManager
*/
class albaVMEManagerTest : public albaTest, public albaObserver
{
public: 
  // Executed before each test
  void BeforeTest();

	// CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaVMEManagerTest );
//   CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
//   CPPUNIT_TEST( TestDynamicAllocation );
//   CPPUNIT_TEST( TestStaticAllocation );
//   CPPUNIT_TEST( MSFNewTest );
//   CPPUNIT_TEST( MSFOpenTest );
//   CPPUNIT_TEST( ZIPOpenTest );
//   CPPUNIT_TEST( MSFSaveTest );
//   CPPUNIT_TEST( ZIPSaveTest );
//   CPPUNIT_TEST( VmeAddRemoveTest );
//   CPPUNIT_TEST( MSFIsModifiedTest );
//   CPPUNIT_TEST( TimeGetBoundsTest );
//   CPPUNIT_TEST( TimeSetTest );
  CPPUNIT_TEST( NotifyAddRemoveTest );
  CPPUNIT_TEST( SetGetFileNameTest );
  CPPUNIT_TEST( GetRootTest );
  CPPUNIT_TEST( GetStorageTest );
  CPPUNIT_TEST( MakeBakFileOnSaveTest );
  CPPUNIT_TEST( SetDirNameTest );
  CPPUNIT_TEST( SetSingleBinaryFileTest );

  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestDynamicAllocation();
  void TestStaticAllocation();
  void MSFNewTest();
  void MSFOpenTest();
  void ZIPOpenTest();
  void MSFSaveTest();
  void ZIPSaveTest();
  void MSFIsModifiedTest();
  void VmeAddRemoveTest();
  void TimeGetBoundsTest();
  void TimeSetTest();
  void NotifyAddRemoveTest();
  void SetGetFileNameTest();
  void GetRootTest();
  void GetStorageTest();
  void MakeBakFileOnSaveTest();
  void SetDirNameTest();
  void SetSingleBinaryFileTest();

  void OnEvent(albaEventBase *alba_event); // This method trap the events coming from the vme manager

  albaVMEManager *m_Manager;
  int m_TestId;
  bool m_Result;
  wxFileConfig *m_Config;
};

#endif

