/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaWizardBlockTypeCheckTest
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaWizardBlockTypeCheckTest_H__
#define __CPP_UNIT_albaWizardBlockTypeCheckTest_H__

#include "albaTest.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------


/** 
class name: albaWizardBlockTypeCheckTest
  Test class for albaWizardBlockTypeCheck
*/
class albaWizardBlockTypeCheckTest : public albaTest
{
public:

  /** Start test suite macro */
	CPPUNIT_TEST_SUITE( albaWizardBlockTypeCheckTest );

  /** macro for test TestSetGetVector */
  CPPUNIT_TEST( TestSetterGetter );

  /** macro for test Accept list */
  CPPUNIT_TEST( TestAcceptList );

  /** macro for test Dynamic allocation */
  CPPUNIT_TEST( TestDynamicAllocation );

  /** End test suite macro */
	CPPUNIT_TEST_SUITE_END();

protected:

  /** Test Setter and Getter methods */
  void TestSetterGetter();

  /** Test Accept list */
  void TestAcceptList();

  /** Test Dynamic allocation */
  void TestDynamicAllocation();
};



#endif
