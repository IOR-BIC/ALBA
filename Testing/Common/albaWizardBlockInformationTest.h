/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaWizardBlockInformationTest
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaWizardBlockInformationTest_H__
#define __CPP_UNIT_albaWizardBlockInformationTest_H__

#include "albaTest.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------


/** 
class name: albaWizardBlockInformationTest
  Test class for albaWizardBlockInformation
*/
class albaWizardBlockInformationTest : public albaTest
{
public:

  /** Start test suite macro */
	CPPUNIT_TEST_SUITE( albaWizardBlockInformationTest );

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
