/*=========================================================================

 Program: MAF2
 Module: mafGUIWizardPageTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafTests_H__
#define __CPP_UNIT_mafTests_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestResult.h>
#include "mafIncludeWX.h"

// Helper class used to build a fake application needed by the wxConfig
// to store settings into the registry with the same name of the application
// ===============================================================================
class TestApp : public wxApp
	// ===============================================================================
{
public:
	bool OnInit();
	int  OnExit();
};
DECLARE_APP(TestApp)


class mafTest : public CPPUNIT_NS::TestFixture
{
public: 
  /** CPPUNIT fixture: executed before each test, prepares an application for test runs
			DO NOT OVERLOAD THIS METHOD, use BeforeTest instead */
  void setUp();

	/** CPPUNIT fixture: executed after each test, clean an application for test runs
			DO NOT OVERLOAD THIS METHOD, use AfterTest instead */
  void tearDown();

	/** Test specific stuff executed before each test */
	virtual void BeforeTest() {};

	/** Test specific stuff executed after each test */
	virtual void AfterTest() {};

protected:
  TestApp *m_App;
};

#endif
