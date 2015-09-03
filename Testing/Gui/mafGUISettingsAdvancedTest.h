/*=========================================================================

 Program: MAF2
 Module: mafGUISettingsAdvancedTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafGUISettingsAdvancedTest_H__
#define __CPP_UNIT_mafGUISettingsAdvancedTest_H__

#include "mafTest.h"

class mafGUISettingsAdvancedTest : public mafTest
{
public: 

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( mafGUISettingsAdvancedTest );
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
