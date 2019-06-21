/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaWizardWaitOpTest
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaWizardWaitOpTest_H__
#define __CPP_UNIT_albaWizardWaitOpTest_H__

#include "albaTest.h"

/** 
class name: albaWizardWaitOpTest
  Test class for albaWizardWaitOp
*/
class albaWizardWaitOpTest : public albaTest
{
public:

  /** Start test suite macro */
	CPPUNIT_TEST_SUITE( albaWizardWaitOpTest );

  /** macro for test Dynamic allocation */
  CPPUNIT_TEST( TestDynamicAllocation );


  /** End test suite macro */
	CPPUNIT_TEST_SUITE_END();

protected:

  /** Test Dynamic allocation */
  void TestDynamicAllocation();
};




#endif
