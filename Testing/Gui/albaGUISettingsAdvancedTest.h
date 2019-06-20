/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUISettingsAdvancedTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaGUISettingsAdvancedTest_H__
#define __CPP_UNIT_albaGUISettingsAdvancedTest_H__

#include "albaTest.h"

class albaGUISettingsAdvancedTest : public albaTest
{
public: 

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( albaGUISettingsAdvancedTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestSetConversionType );
  CPPUNIT_TEST( TestSettingsAdvanced );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestSettingsAdvanced();
  void TestSetConversionType();
  bool m_Result;

};

#endif
