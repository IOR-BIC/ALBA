/*=========================================================================

 Program: MAF2
 Module: mafWizardManagerTest
 Authors: Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafWizardManagerTest_H__
#define __CPP_UNIT_mafWizardManagerTest_H__

#include "mafTest.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------


/** 
class name: mafWizardManagerTest
  Test class for mafWizardManager
*/
class mafWizardManagerTest : public mafTest
{
public:
  // Executed after each test
  void AfterTest();

  /** Start test suite macro */
	CPPUNIT_TEST_SUITE( mafWizardManagerTest );

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
