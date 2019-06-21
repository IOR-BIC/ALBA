/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIDicomSettingsTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaGUIDicomSettingsTest_H__
#define __CPP_UNIT_albaGUIDicomSettingsTest_H__

#include "albaTest.h"

class albaGUIDicomSettingsTest : public albaTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( albaGUIDicomSettingsTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestGetBuildStep );
  CPPUNIT_TEST( TestAutoVMEType );
  CPPUNIT_TEST( TestGetVMEType );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestGetBuildStep();
  void TestAutoVMEType();
  void TestGetVMEType();

  bool m_Result;
};

#endif
