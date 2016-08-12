/*=========================================================================

 Program: MAF2
 Module: mafGUIDicomSettingsTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafGUIDicomSettingsTest_H__
#define __CPP_UNIT_mafGUIDicomSettingsTest_H__

#include "mafTest.h"

class mafGUIDicomSettingsTest : public mafTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( mafGUIDicomSettingsTest );
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
