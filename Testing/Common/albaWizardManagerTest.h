/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaWizardManagerTest
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaWizardManagerTest_H__
#define __CPP_UNIT_albaWizardManagerTest_H__

#include "albaTest.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------


/** 
class name: albaWizardManagerTest
  Test class for albaWizardManager
*/
class albaWizardManagerTest : public albaTest
{
public:
  // Executed after each test
  void AfterTest();

  /** Start test suite macro */
	CPPUNIT_TEST_SUITE( albaWizardManagerTest );

  /** macro for test TestSetGetVector */
  CPPUNIT_TEST( TestSetterGetter );

  /** macro for test Dynamic allocation */
  CPPUNIT_TEST( TestDynamicAllocation );

  /** macro for test Dynamic allocation */
  CPPUNIT_TEST( TestAddWizard );

  /** End test suite macro */
	CPPUNIT_TEST_SUITE_END();

protected:

  /** Test Setter and Getter methods */
  void TestSetterGetter();

  /** Test wizard Add */
	void TestAddWizard();

  /** Test Dynamic allocation */
  void TestDynamicAllocation();

};




#endif
