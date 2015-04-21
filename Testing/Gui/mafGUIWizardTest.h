/*=========================================================================

 Program: MAF2
 Module: mafGUIWizardTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafGUIWizardTest_H__
#define __CPP_UNIT_mafGUIWizardTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class TestApp;
class mafGUIFrame;

class mafGUIWizardTest : public CPPUNIT_NS::TestFixture
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  CPPUNIT_TEST_SUITE( mafGUIWizardTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestSetFirstPage );
  CPPUNIT_TEST( TestEnableChangePageOn );
  CPPUNIT_TEST( TestEnableChangePageOff );
  CPPUNIT_TEST( TestRun );
  CPPUNIT_TEST_SUITE_END();

protected:

  void TestDynamicAllocation();
  void TestSetFirstPage();
  void TestEnableChangePageOn();
  void TestEnableChangePageOff();
  void TestRun();

  bool m_Result;

  mafGUIFrame *m_Win;
};

#endif
