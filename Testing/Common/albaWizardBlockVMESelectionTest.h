/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaWizardBlockVMESelectionTest
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaWizardBlockVMESelectionTest_H__
#define __CPP_UNIT_albaWizardBlockVMESelectionTest_H__

#include "albaTest.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------


/** 
class name: albaWizardBlockVMESelectionTest
  Test class for albaWizardBlockVMESelection
*/
class albaWizardBlockVMESelectionTest : public albaTest
{
public:

  /** Start test suite macro */
	CPPUNIT_TEST_SUITE( albaWizardBlockVMESelectionTest );

  /** macro for test TestSetGetVector */
  CPPUNIT_TEST( TestSetterGetter );

  /** macro for test Dynamic allocation */
  CPPUNIT_TEST( TestDynamicAllocation );

  /** End test suite macro */
	CPPUNIT_TEST_SUITE_END();

protected:

  /** Test Setter and Getter methods */
  void TestSetterGetter();

  /** Test Dynamic allocation */
  void TestDynamicAllocation();
};



#endif
