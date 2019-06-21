/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaWizardBlockTest
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaWizardBlockTest_H__
#define __CPP_UNIT_albaWizardBlockTest_H__

#include "albaTest.h"

/** 
class name: albaWizardBlockTest
  Test class for albaWizardBlock
*/
class albaWizardBlockTest : public albaTest
{
public:

  /** Start test suite macro */
	CPPUNIT_TEST_SUITE( albaWizardBlockTest );

  /** macro for test TestSetGetVector */
  CPPUNIT_TEST( TestSetterGetter );

  /** macro for test TestSetGetVector */
  CPPUNIT_TEST( TestConstructorDistructor );

  /** macro for test TestSetGetVector */
  CPPUNIT_TEST( TestAbort );

  /** macro for test Dynamic allocation */
  CPPUNIT_TEST( TestDynamicAllocation );

  /** End test suite macro */
	CPPUNIT_TEST_SUITE_END();

protected:

  /** Test Setter and Getter methods */
  void TestSetterGetter();

  /** Test Constructors and destructor */
	void TestConstructorDistructor();

  /** Test Constructors and destructor */
  void TestAbort();

  /** Test Dynamic allocation */
  void TestDynamicAllocation();

};




#endif
