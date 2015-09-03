/*=========================================================================

 Program: MAF2
 Module: mafWizardBlockTypeCheckTest
 Authors: Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafWizardBlockTypeCheckTest_H__
#define __CPP_UNIT_mafWizardBlockTypeCheckTest_H__

#include "mafTest.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------


/** 
class name: mafWizardBlockTypeCheckTest
  Test class for mafWizardBlockTypeCheck
*/
class mafWizardBlockTypeCheckTest : public mafTest
{
public:

  /** Start test suite macro */
	CPPUNIT_TEST_SUITE( mafWizardBlockTypeCheckTest );

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
